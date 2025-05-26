/*********************************************************************************
 * ===== FILE: protocol.c =====
 * Text-based MESSAGE_TYPE|payload communication protocol handling
 *********************************************************************************/

#include "protocol.h"
#include "utils.h"
#include <string.h>
#include <stdio.h> // TODO: ???

/**
 * @brief Send a message to a client using MESSAGE_TYPE|payload format
 * Sends message as text string: "MESSAGE_TYPE|payload\n"
 * @param client_fd Client socket file descriptor
 * @param msg_type Type of message (see MSG_* constants)
 * @param data Message payload (can be NULL or empty)
 * @return 0 on success, -1 on error
 */
int send_message(int client_fd, int msg_type, const char *data)
{
  char message_buffer[MAX_MESSAGE_SIZE + 32]; // Extra space for type and separator

  // Format message as "MESSAGE_TYPE|payload\n"
  if (data && strlen(data) > 0)
  {
    snprintf(message_buffer, sizeof(message_buffer), "%d|%s\n", msg_type, data);
  }
  else
  {
    snprintf(message_buffer, sizeof(message_buffer), "%d|\n", msg_type); // Empty payload
  }

  size_t message_len = strlen(message_buffer);
  ssize_t total_sent = 0;

  printf("[DEBUG] Sending message: '%.*s' to client fd %d\n",
         (int)(message_len - 1), message_buffer, client_fd); // -1 to skip \n in log

  // Send the complete message
  while (total_sent < (ssize_t)message_len)
  {
    ssize_t sent = send(client_fd, message_buffer + total_sent,
                        message_len - total_sent, MSG_NOSIGNAL);

    if (sent <= 0)
    {
      if (sent == 0 || (errno != EAGAIN && errno != EWOULDBLOCK))
      {
        printf("[WARNING] Failed to send message to client fd %d: %s\n",
               client_fd, strerror(errno));
        return (-1);
      }
      // For EAGAIN/EWOULDBLOCK, continue trying
      continue;
    }

    total_sent += sent;
  }

  printf("[DEBUG] Successfully sent message type %d (%zu bytes) to client fd %d\n",
         msg_type, message_len, client_fd);
  return (0);
}

/**
 * @brief Receive a line from socket (helper function)
 * Reads data character by character until newline is found
 * Handles non-blocking sockets properly
 * @param client_fd Client socket file descriptor
 * @param buffer Output buffer for received line
 * @param buffer_size Size of output buffer
 * @return Number of bytes received (excluding newline), -1 on error
 *
 * @example "Ciao! \nCome stai" -> "Ciao! Come stai"
 */
static int
receive_line(int client_fd, char *buffer, size_t buffer_size)
{
  size_t pos = 0;
  char ch;

  while (pos < buffer_size - 1)
  {
    ssize_t received = recv(client_fd, &ch, 1, 0);

    if (received <= 0)
    {
      if (received == 0)
      {
        printf("[INFO] Client fd %d disconnected while reading line\n", client_fd);
        return (-1);
      }

      printf("[WARNING] Error receiving from client fd %d: %s\n",
             client_fd, strerror(errno));
      return (-1);
    }

    // Skip carriage return (handle Windows line endings)
    if (ch == '\r' || ch == '\n')
    {
      continue;
    }

    // Add character to buffer
    buffer[pos++] = ch;
  }

  // Null-terminate the string
  buffer[pos] = '\0';

  printf("[DEBUG] Received line (%zu bytes): '%s'\n", pos, buffer);
  return ((int)pos);
}

/**
 * @brief Receive and parse MESSAGE_TYPE|payload into message_t struct
 * Reads text line from socket and parses it into structured format
 * @param client_fd Client socket file descriptor
 * @param msg Output message structure to populate
 * @return 0 on success, -1 on error
 */
int receive_message(int client_fd, message_t *msg)
{
  char line_buffer[MAX_MESSAGE_SIZE + 64]; // Extra space for message type and separators
  // Initialize message structure
  memset(msg, 0, sizeof(message_t));

  // Step 1: Receive complete line from socket
  int line_length = receive_line(client_fd, line_buffer, sizeof(line_buffer));
  if (line_length < 0)
  {
    return (-1);
  }

  // Step 2: Validate minimum message format
  if (line_length < 2)
  {
    printf("[WARNING] Received too short message from client fd %d: '%s'\n",
           client_fd, line_buffer);
    return (-1);
  }

  // Step 3: Find the pipe separator
  char *pipe_pos = strchr(line_buffer, '|');
  if (!pipe_pos)
  {
    printf("[WARNING] Invalid message format from client fd %d (missing '|'): '%s'\n",
           client_fd, line_buffer);
    return (-1);
  }

  // Step 4: Extract and parse message type
  *pipe_pos = '\0'; // Split the string at pipe
  char *type_str = trim_whitespace(line_buffer);
  char *payload_str = pipe_pos + 1; // Payload starts after pipe

  // Parse message type as integer
  char *endptr;
  long parsed_type = strtol(type_str, &endptr, 10);

  // Validate message type
  if (*endptr != '\0' || parsed_type < 0 || parsed_type > 1000)
  {
    printf("[WARNING] Invalid message type from client fd %d: '%s'\n",
           client_fd, type_str);
    return (-1);
  }

  // Step 5: Populate message_t structure
  msg->type = (int)parsed_type;

  // Handle payload
  size_t payload_len = strlen(payload_str);
  if (payload_len > MAX_MESSAGE_SIZE - 1)
  {
    printf("[WARNING] Payload too large from client fd %d (%zu bytes), truncating to %d\n",
           client_fd, payload_len, MAX_MESSAGE_SIZE - 1);
    payload_len = MAX_MESSAGE_SIZE - 1;
  }

  msg->length = (int)payload_len;
  if (payload_len > 0)
  {
    memcpy(msg->data, payload_str, payload_len);
  }
  msg->data[payload_len] = '\0'; // Ensure null termination

  printf("[DEBUG] Successfully received message from client fd %d: type=%d, length=%d\n",
         client_fd, msg->type, msg->length);
  printf("[DEBUG] Message payload: '%.100s%s'\n",
         msg->data, (payload_len > 100) ? "..." : "");

  return (0);
}

/**
 * @brief Parse complete stateless client message into components
 * Expected payload format: "personality_data|language_code|conversation"
 * @param data Input message payload (from msg->data)
 * @param parsed_msg Output structure with parsed components
 * @return 0 on success, -1 on error
 */
int parse_client_message(const char *data, client_message_t *parsed_msg)
{
  printf("[DEBUG] Parsing stateless client message payload: %.100s%s\n",
         data, (strlen(data) > 100) ? "..." : "");

  // Initialize structure
  memset(parsed_msg, 0, sizeof(client_message_t));

  // Check for empty data
  if (!data || strlen(data) == 0)
  {
    printf("[ERROR] Empty message payload\n");
    return (-1);
  }

  // Make a copy since strtok modifies the string
  char *data_copy = strdup(data);
  if (!data_copy)
  {
    printf("[ERROR] Failed to allocate memory for message parsing\n");
    return (-1);
  }

  // Parse the three components: personality|language|conversation
  char *parts[3];
  int part_count = 0;
  char *token = strtok(data_copy, "|");

  while (token != NULL && part_count < 3)
  {
    parts[part_count] = token;
    token = strtok(NULL, "|");
    ++part_count;
  }

  // Validate we got all required parts
  if (part_count != 3)
  {
    printf("[ERROR] Invalid stateless message format: expected 3 parts, got %d\n", part_count);
    printf("[ERROR] Expected format: personality|language|conversation\n");
    printf("[ERROR] Example: 'extraversion:5.2,agreeableness:4.1|en|[xxx]Hello robot'\n");
    free(data_copy);
    return (-1);
  }

  // Copy components to output structure with bounds checking
  safe_strncpy(parsed_msg->personality, parts[0], sizeof(parsed_msg->personality));
  safe_strncpy(parsed_msg->language, parts[1], sizeof(parsed_msg->language));
  safe_strncpy(parsed_msg->conversation, parts[2], sizeof(parsed_msg->conversation));

  // Mark as successfully parsed
  parsed_msg->is_valid = 1;

  free(data_copy);

  printf("[INFO] Successfully parsed stateless message:\n");
  printf("[INFO] - Language: %s\n", parsed_msg->language);
  printf("[INFO] - Personality: %.50s%s\n",
         parsed_msg->personality, (strlen(parsed_msg->personality) > 50) ? "..." : "");
  printf("[INFO] - Conversation: %.50s%s\n",
         parsed_msg->conversation, (strlen(parsed_msg->conversation) > 50) ? "..." : "");

  return (0);
}
