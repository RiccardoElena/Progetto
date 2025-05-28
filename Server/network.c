/*********************************************************************************
 * ===== FILE: network.c =====
 * Network management - PURE STATELESS IMPLEMENTATION
 * Each connection handles exactly one request then closes automatically
 *********************************************************************************/

#include "thread_pool.h"
#include "network.h"
#include "gemini_ai.h"
#include "protocol.h"
#include "utils.h"

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
#if SHOW_INFO
  printf("[INFO] Processing stateless request on fd %d\n", client_fd);
#endif

  // Set socket timeouts to prevent indefinite blocking
  struct timeval timeout;
  timeout.tv_sec = CLIENT_SOCKET_TIMEOUT_SEC;
  timeout.tv_usec = 0;

  if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
  {
#if SHOW_WARNING
    printf("[WARNING] Failed to set receive timeout for fd %d\n", client_fd);
#endif
  }

  if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
  {
#if SHOW_WARNING
    printf("[WARNING] Failed to set send timeout for fd %d\n", client_fd);
#endif
  }

  // Receive exactly one message
  message_t msg;
  if (receive_message(client_fd, &msg) < 0)
  {
#if SHOW_WARNING
    printf("[WARNING] Failed to receive message from fd %d\n", client_fd);
#endif
    close(client_fd);
    return;
  }

#if SHOW_INFO
  printf("[INFO] Received message type %d from fd %d\n", msg.type, client_fd);
#endif
  // Process the message based on type
  switch (msg.type)
  {
  case MSG_AI_DIALOG_REQUEST:
  {
#if SHOW_INFO
    printf("[INFO] Processing MSG_REQUEST from fd %d\n", client_fd);
#endif
    // Parse the complete stateless request
    client_message_t request;
    if (parse_client_dialog_message(msg.data, &request) != 0)
    {
#if SHOW_ERROR
      printf("[ERROR] Failed to parse client message from fd %d\n", client_fd);
#endif
      send_message(client_fd, MSG_ERROR, "Invalid request format");
      close(client_fd);
      return;
    }

    // Validate required components
    if (strlen(request.personality) == 0 ||
        strlen(request.language) == 0 ||
        strlen(request.conversation) == 0)
    {
#if SHOW_ERROR
      printf("[ERROR] Missing required fields from fd %d\n", client_fd);
#endif
      send_message(client_fd, MSG_ERROR, "Missing required fields");
      close(client_fd);
      return;
    }
#if SHOW_INFO
    printf("[INFO] AI request for fd %d: personality=%.30s..., language=%s\n",
           client_fd, request.personality, request.language);
#endif
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
      char response_ai[MAX_AI_RESPONSE_SIZE];
      snprintf(response_ai, sizeof(response_ai), "%s", ai_response.response);

      if (send_message(client_fd, MSG_AI_DIALOG_RESPONSE, response_ai) == 0)
      {
#if SHOW_INFO
        printf("[INFO] Successfully processed ai request for fd %d\n", client_fd);
#endif
      }
      else
      {
#if SHOW_WARNING
        printf("[WARNING] Failed to send response to fd %d\n", client_fd);
#endif
      }
      break; // to make continue in test case
    }
    else
    {
#if SHOW_ERROR
      printf("[ERROR] AI response failed for fd %d\n", client_fd);
#endif
      // send_message(client_fd, MSG_ERROR, "Failed to generate AI response");
    }
  }

  case MSG_TEST_DIALOG_REQUEST:
  {
    // Parse the complete stateless request
    client_message_t request;
    if (parse_client_dialog_message(msg.data, &request) != 0)
    {
#if SHOW_ERROR
      printf("[ERROR] Failed to parse client message from fd %d\n", client_fd);
#endif
      send_message(client_fd, MSG_ERROR, "Invalid request format");
      close(client_fd);
      return;
    }

    if (send_message(client_fd, MSG_TEST_DIALOG_RESPONSE, test_response(request.language)) == 0)
    {
#if SHOW_INFO
      printf("[INFO] Successfully processed test request for fd %d\n", client_fd);
#endif
    }
    else
    {
#if SHOW_WARNING
      printf("[WARNING] Failed to send response to fd %d\n", client_fd);
#endif
    }
    break;
  }

  default:
  {
#if SHOW_WARNING
    printf("[WARNING] Unknown message type %d from fd %d\n", msg.type, client_fd);
#endif
    send_message(client_fd, MSG_ERROR, "Unknown message type");
    break;
  }
  }

// Close connection after processing (stateless behavior)
#if SHOW_INFO
  printf("[INFO] Closing connection fd %d (stateless mode)\n", client_fd);
#endif
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
#if SHOW_DEBUG
  printf("[DEBUG] Worker thread handling client fd %d\n", client_fd);
#endif
  process_single_request_and_close(client_fd);
#if SHOW_DEBUG
  printf("[DEBUG] Worker thread finished with client fd %d\n", client_fd);
#endif
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

#if SHOW_INFO
  printf("[INFO] New connection: fd=%d from %s:%d\n",
         client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
#endif
  // NO CLIENT TRACKING - immediately schedule for processing
  int *fd_ptr = malloc(sizeof(int));
  if (!fd_ptr)
  {
#if SHOW_ERROR
    printf("[ERROR] Memory allocation failed for client task\n");
#endif
    close(client_fd);
    return;
  }

  *fd_ptr = client_fd;

  // Add to thread pool for immediate processing
  if (thread_pool_add_task(g_server.pool, handle_client_task, fd_ptr) < 0)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to add task to thread pool\n");
#endif
    free(fd_ptr);
    close(client_fd);
    return;
  }
#if SHOW_DEBUG
  printf("[DEBUG] Client fd %d scheduled for processing\n", client_fd);
#endif
}

/**
 * @brief Handle client data - NOT USED in pure stateless mode
 * In stateless mode, client sockets are never added to epoll
 * Only the server socket is monitored with epoll
 */
void handle_client_data(int client_fd)
{
#if SHOW_WARNING
  printf("[WARNING] handle_client_data should not be called in stateless mode\n");
#endif
  close(client_fd);
}

/**
 * @brief Remove client
 * In stateless mode, this just closes the socket (no state to remove)
 * @param fd Client socket file descriptor
 */
void remove_client(int fd)
{
#if SHOW_DEBUG
  printf("[DEBUG] remove_client called for fd %d (stateless mode)\n", fd);
#endif
  close(fd);
}
