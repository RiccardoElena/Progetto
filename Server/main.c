/*********************************************************************************
 * ===== FILE: main.c =====
 * Main server entry point and initialization
 *********************************************************************************/

#include "network.h"
#include "thread_pool.h"
#include "util.h"

/**
 * @brief Signal handler for graceful shutdown
 * Handles SIGINT (Ctrl+C) and SIGTERM to shutdown cleanly
 * @param signum Signal number received
 */
static void
signal_handler(int signum)
{

  printf("\n[INFO] Received signal %d (%s), shutting down gracefully...\n",
         signum, (signum == SIGINT) ? "SIGINT" : "SIGTERM");

  g_server.running = 0;
}

/**
 * @brief Initialize the server
 * Sets up sockets, epoll, thread pool, and all server components
 * @param port Port number to listen on
 * @param gemini_api_key Google Gemini API key
 * @return 0 on success, -1 on error
 */
static int
init_server(int port, const char *gemini_api_key)
{
#if SHOW_INFO
  printf("[INFO] Initializing Robot Dialog Server...\n");
#endif

  // Clear server structure
  memset(&g_server, 0, sizeof(server_context_t));

  // Store API key
  safe_strncpy(g_server.gemini_api_key, gemini_api_key, sizeof(g_server.gemini_api_key));

  // Initialize cURL library for HTTP requests
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to initialize cURL library\n");
#endif
    return (-1);
  }

  // Create main server socket
  g_server.server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (g_server.server_fd == -1)
  {
    perror("socket");
    curl_global_cleanup();
    return (-1);
  }

  // Set socket options for address reuse
  int opt = 1;
  if (setsockopt(g_server.server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
  {
    perror("setsockopt");
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Make server socket non-blocking
  if (make_socket_non_blocking(g_server.server_fd) < 0)
  {
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Setup server address
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
  server_addr.sin_port = htons(port);

  // Bind socket to address
  if (bind(g_server.server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("bind");
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Start listening for connections
  if (listen(g_server.server_fd, BACKLOG) < 0)
  {
    perror("listen");
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Create epoll instance for efficient I/O monitoring
  g_server.epoll_fd = epoll_create1(0);
  if (g_server.epoll_fd == -1)
  {
    perror("epoll_create1");
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Add server socket to epoll monitoring
  struct epoll_event event;
  event.events = EPOLLIN; // Monitor for incoming connections
  event.data.fd = g_server.server_fd;

  if (epoll_ctl(g_server.epoll_fd, EPOLL_CTL_ADD, g_server.server_fd, &event) == -1)
  {
    perror("epoll_ctl: server_fd");
    close(g_server.epoll_fd);
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Create thread pool for concurrent request processing
  g_server.pool = thread_pool_create(THREAD_POOL_INITIAL_SIZE);
  if (!g_server.pool)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to create enhanced thread pool\n");
#endif
    close(g_server.epoll_fd);
    close(g_server.server_fd);
    curl_global_cleanup();
    return (-1);
  }

  // Mark server as running
  g_server.running = 1;

#if SHOW_INFO
  printf("[INFO] Robot Dialog Server initialized successfully\n");
  printf("[INFO] - Port: %d\n", port);
  printf("[INFO] - Max clients: %d\n", MAX_CLIENTS);
  printf("[INFO] - Thread pool size: %d\n", THREAD_POOL_INITIAL_SIZE);
  printf("[INFO] - Supported languages: ALL\n");
  printf("[INFO] - AI Provider: Google Gemini\n");
#endif

  return (0);
}

/**
 * @brief Clean up server resources
 * Properly closes all connections and frees memory
 */
static void
cleanup_server(void)
{
#if SHOW_INFO
  printf("[INFO] Cleaning up server resources...\n");
#endif
  // Stop accepting new work
  g_server.running = 0;

  // Destroy thread pool (waits for all threads to finish)
  if (g_server.pool)
  {
    thread_pool_destroy(g_server.pool);
  }

  // Close server sockets
  if (g_server.epoll_fd != -1)
  {
    close(g_server.epoll_fd);
  }

  if (g_server.server_fd != -1)
  {
    close(g_server.server_fd);
  }

  // Clean up cURL
  curl_global_cleanup();

#if SHOW_INFO
  printf("[INFO] Server cleanup completed\n");
#endif
}

/**
 * @brief Main server event loop
 * Uses epoll to efficiently handle multiple client connections
 */
static void run_server(void)
{
  struct epoll_event events[MAX_EVENTS];
  time_t last_stats_time = time(NULL);

#if SHOW_INFO
  printf("[INFO] Server started, waiting for connections...\n");
  printf("[INFO] Press Ctrl+C to shutdown gracefully\n");
#endif

  // Main event loop
  while (g_server.running)
  {
    int nfds = epoll_wait(g_server.epoll_fd, events, MAX_EVENTS, 1000);

    if (nfds == -1)
    {
      if (errno == EINTR)
        continue;
      perror("epoll_wait");
      break;
    }

    for (int i = 0; i < nfds; ++i)
    {
      int fd = events[i].data.fd;
      if (fd == g_server.server_fd)
      {
        accept_new_connection();
      }
    }

    // Print statistics every 30 seconds
    time_t now = time(NULL);
    if (now - last_stats_time >= 30)
    {
      thread_pool_print_stats(g_server.pool);
      last_stats_time = now;
    }
  }
#if SHOW_INFO
  printf("[INFO] Server event loop terminated\n");
#endif
}

/**
 * @brief Print usage information
 * Shows command line options and examples
 * @param program_name Name of the executable
 */
static void
print_usage(const char *program_name)
{
  printf("Robot Dialog Server - University Project\n");
  printf("A server for robot-human dialogue with personality adaptation\n\n");
  printf("Usage: %s [OPTIONS]\n\n", program_name);
  printf("Required Options:\n");
  printf("  Google Gemini API key (required)\n\n");
  printf("Optional Options:\n");
  printf("  -p PORT           Server port (default: %d)\n", DEFAULT_PORT);
  printf("  -h                Show this help message\n\n");
  printf("Supported Languages: EVERITHING\n\n");
  printf("Example:\n");
  printf("  %s -p 8080\n\n", program_name);
  printf("Client Message Format:\n");
  printf("  Personality: \"extraversion:5.2,agreeableness:4.1,conscientiousness:6.0,emotional_stability:3.8,openness:5.5|en\"\n");
  printf("  User Message: \"Hello, how are you?\"\n");
}

/**
 * @brief Main function - entry point
 * Handles command line arguments and starts the server
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return EXIT_SUCCESS on normal termination, EXIT_FAILURE on error
 */
int main(int argc, char *argv[])
{
  int port = DEFAULT_PORT;
  char gemini_api_key[256] = {0};

  // Get API key from environment
  char *env_key = getenv("GEMINI_API_KEY");
  if (env_key)
  {
    safe_strncpy(gemini_api_key, env_key, sizeof(gemini_api_key));
  }

  printf("Robot Dialog Server - LSO Project\n");
  printf("========================================\n");

  // Parse command line arguments
  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
    {
      // Port number
      port = atoi(argv[++i]);
      if (port <= 0 || port > 65535)
      {
#if SHOW_ERROR
        printf("[ERROR] Invalid port number: %s (must be 1-65535)\n", argv[i]);
#endif
        return (EXIT_FAILURE);
      }
    }
    else if (strcmp(argv[i], "-h") == 0)
    {
      // Help
      print_usage(argv[0]);
      return (EXIT_SUCCESS);
    }
    else
    {
#if SHOW_ERROR
      printf("[ERROR] Unknown option: %s\n", argv[i]);
#endif
      print_usage(argv[0]);
      return (EXIT_FAILURE);
    }
  }

  // Validate required parameters
  if (strlen(gemini_api_key) == 0)
  {
#if SHOW_ERROR
    printf("[ERROR] Gemini API key is required as environment variable.\n");
    printf("Get your API key from: https://aistudio.google.com/app/apikey\n\n");
#endif
    print_usage(argv[0]);
    return (EXIT_FAILURE);
  }

  // Setup signal handlers for graceful shutdown
  signal(SIGINT, signal_handler);  // Ctrl+C
  signal(SIGTERM, signal_handler); // Termination request
  signal(SIGPIPE, SIG_IGN);        // Ignore broken pipe signals

  // Initialize and run server
  if (init_server(port, gemini_api_key) < 0)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to initialize server\n");
#endif
    return (EXIT_FAILURE);
  }

  // Run main event loop
  run_server();

  // Clean up resources
  cleanup_server();
#if SHOW_INFO
  printf("[INFO] Robot Dialog Server terminated successfully\n");
#endif
  return (EXIT_SUCCESS);
}
