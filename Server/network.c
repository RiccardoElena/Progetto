/*********************************************************************************
 * ===== FILE: network.c =====
 * Network management - PURE STATELESS IMPLEMENTATION
 * Each connection handles exactly one request then closes automatically
 *********************************************************************************/

#include "server.h"
#include "thread_pool.h"
#include "network.h"
#include "gemini_ai.h"
#include "protocol.h"

// Global server context - accessible to all network functions
server_context_t g_server;

/**
 * @brief Process a single client request and immediately close connection
 * PURE STATELESS: Each TCP connection handles exactly one request
 * @param client_fd Client socket file descriptor
 */
static void
process_single_request_and_close(int client_fd)
{
  printf("[INFO] Processing stateless request on fd %d\n", client_fd);

  // Set socket timeouts to prevent indefinite blocking
  struct timeval timeout;
  timeout.tv_sec = CLIENT_SOCKET_TIMEOUT_SEC;
  timeout.tv_usec = 0;

  if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
  {
    printf("[WARNING] Failed to set receive timeout for fd %d\n", client_fd);
  }

  if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
  {
    printf("[WARNING] Failed to set send timeout for fd %d\n", client_fd);
  }
  // Send welcome message
  if (send_message(client_fd, MSG_RESPONSE, "Connected - Send your request") < 0)
  {
    printf("[WARNING] Failed to send welcome message to fd %d\n", client_fd);
    close(client_fd);
    return;
  }

  // Receive exactly one message
  message_t msg;
  if (receive_message(client_fd, &msg) < 0)
  {
    printf("[WARNING] Failed to receive message from fd %d\n", client_fd);
    close(client_fd);
    return;
  }

  printf("[INFO] Received message type %d from fd %d\n", msg.type, client_fd);

  // Process the message based on type
  switch (msg.type)
  {
  case MSG_REQUEST:
  {
    printf("[INFO] Processing MSG_REQUEST from fd %d\n", client_fd);

    // Parse the complete stateless request
    client_message_t request;
    if (parse_client_message(msg.data, &request) != 0)
    {
      printf("[ERROR] Failed to parse client message from fd %d\n", client_fd);
      send_message(client_fd, MSG_ERROR, "Invalid request format");
      close(client_fd);
      return;
    }

    // Validate required components
    if (strlen(request.personality) == 0 ||
        strlen(request.language) == 0 ||
        strlen(request.conversation) == 0)
    {
      printf("[ERROR] Missing required fields from fd %d\n", client_fd);
      send_message(client_fd, MSG_ERROR, "Missing required fields");
      close(client_fd);
      return;
    }

    printf("[INFO] AI request for fd %d: personality=%.30s..., language=%s\n",
           client_fd, request.personality, request.language);

    // Generate AI response - COMPLETELY STATELESS
    ai_response_t ai_response;
    int ai_result = generate_ai_response(g_server.gemini_api_key,
                                         request.personality,
                                         request.language,
                                         request.conversation,
                                         &ai_response);

    if (ai_result == 0 && ai_response.success)
    {
      // Send response with behavioral cues
      char response_with_behavior[MAX_AI_RESPONSE_SIZE + 256];
      snprintf(response_with_behavior, sizeof(response_with_behavior),
               "%s | [Robot behavior: %s]",
               ai_response.response, ai_response.robot_behavior);

      if (send_message(client_fd, MSG_RESPONSE, response_with_behavior) == 0)
      {
        printf("[INFO] Successfully processed request for fd %d\n", client_fd);
      }
      else
      {
        printf("[WARNING] Failed to send response to fd %d\n", client_fd);
      }
    }
    else
    {
      printf("[ERROR] AI response failed for fd %d\n", client_fd);
      send_message(client_fd, MSG_ERROR, "Failed to generate AI response");
    }
    break;
  }

  case MSG_DISCONNECT:
  {
    printf("[INFO] Processing MSG_DISCONNECT from fd %d\n", client_fd);
    send_message(client_fd, MSG_DISCONNECT, "Goodbye!");
    break;
  }

  default:
  {
    printf("[WARNING] Unknown message type %d from fd %d\n", msg.type, client_fd);
    send_message(client_fd, MSG_ERROR, "Unknown message type");
    break;
  }
  }

  // Close connection after processing (stateless behavior)
  printf("[INFO] Closing connection fd %d (stateless mode)\n", client_fd);
  close(client_fd);
}

/**
 * @brief Worker function for handling client request
 * Executed by thread pool worker
 * @param arg Pointer to client file descriptor (allocated memory)
 */
static void
handle_client_task(void *arg)
{
  int client_fd = *((int *)arg);
  free(arg); // Free the memory allocated in accept_new_connection

  printf("[DEBUG] Worker thread handling client fd %d\n", client_fd);
  process_single_request_and_close(client_fd);
  printf("[DEBUG] Worker thread finished with client fd %d\n", client_fd);
}

/**
 * @brief Accept new connection and immediately schedule for processing
 * STATELESS: No client tracking, immediate processing via thread pool
 */
void accept_new_connection(void)
{
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);

  // Accept the connection
  int client_fd = accept(g_server.server_fd, (struct sockaddr *)&client_addr, &addr_len);
  if (client_fd == -1)
  {
    if (errno != EAGAIN && errno != EWOULDBLOCK)
    {
      perror("accept");
    }
    return;
  }

  printf("[INFO] New connection: fd=%d from %s:%d\n",
         client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

  // NO CLIENT TRACKING - immediately schedule for processing
  int *fd_ptr = malloc(sizeof(int));
  if (!fd_ptr)
  {
    printf("[ERROR] Memory allocation failed for client task\n");
    close(client_fd);
    return;
  }

  *fd_ptr = client_fd;

  // Add to thread pool for immediate processing
  if (thread_pool_add_task(g_server.pool, handle_client_task, fd_ptr) < 0)
  {
    printf("[ERROR] Failed to add task to thread pool\n");
    free(fd_ptr);
    close(client_fd);
    return;
  }

  printf("[DEBUG] Client fd %d scheduled for processing\n", client_fd);
}

/**
 * @brief Handle client data - NOT USED in pure stateless mode
 * In stateless mode, client sockets are never added to epoll
 * Only the server socket is monitored with epoll
 */
void handle_client_data(int client_fd)
{
  printf("[WARNING] handle_client_data should not be called in stateless mode\n");
  close(client_fd);
}

/**
 * @brief Remove client
 * In stateless mode, this just closes the socket (no state to remove)
 * @param fd Client socket file descriptor
 */
void remove_client(int fd)
{
  printf("[DEBUG] remove_client called for fd %d (stateless mode)\n", fd);
  close(fd);
}
