/*********************************************************************************
 * ===== FILE: server.h =====
 * Main data structures and type definitions
 *********************************************************************************/

#ifndef SERVER_H
#define SERVER_H

// Standard C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdatomic.h>

// External libraries for AI integration
#include <curl/curl.h>   // For HTTP requests to Gemini API
#include <json-c/json.h> // For JSON parsing

// Include our configuration
#include "config.h"

/**
 * @brief Client message structure
 * Used temporarily to parse the complete client message
 */
typedef struct
{
  char personality[512];   // Personality
  char language[8];        // Language code (en, it, es, fr)
  char conversation[2048]; // JSON conversation history
  int is_valid;            // 1 if parsing was successful
} client_message_t;

/**
 * @brief AI response structure
 * Contains the response from Gemini AI with behavioral instructions
 */
typedef struct
{
  char response[MAX_AI_RESPONSE_SIZE];        // The actual text response
  char robot_behavior[MAX_AI_ROBOT_BEHAVIOR]; // The robot behavior suggested
  int success;                                // 1 if AI call was successful
} ai_response_t;

/**
 * @brief Message type
 * Represents the message type of the Client-Server protocol
 */
typedef struct
{
  int type;                    // Message type
  int length;                  // Length of data field
  char data[MAX_MESSAGE_SIZE]; // Message payload
} message_t;

/**
 * @brief Thread pool task
 * Represents work to be done by worker threads
 */
typedef struct task
{
  void (*function)(void *); // Function to execute
  void *argument;           // Argument to pass to function
  struct task *next;        // Next task in queue
} task_t;

/**
 * @brief Thread pool management
 * Handles concurrent processing of client requests
 */
typedef struct
{
  pthread_t *threads; // Array of worker threads
  int thread_count;   // Number of threads in pool
  int min_threads;    // Minimum thread count
  int max_threads;    // Maximum thread count

  // Task queue
  task_t *queue_head; // First task in queue
  task_t *queue_tail; // Last task in queue

  // Synchronization
  pthread_mutex_t queue_mutex; // Protects task queue
  pthread_cond_t queue_cond;   // Signals when work available
  int shutdown;                // 1 when shutting down

  // Auto-scaling metrics
  atomic_int active_threads;         // Number of threads currently working
  atomic_int queue_size;             // Current queue size
  atomic_long total_tasks_completed; // Total tasks completed
  time_t last_scale_time;            // Last time we checked for scaling

  // Performance tracking
  atomic_long total_task_time_ms; // Total time spent on tasks
  atomic_long min_task_time_ms;   // Minimum task time
  atomic_long max_task_time_ms;   // Maximum task time
} thread_pool_t;

/**
 * @brief Main server context
 * Contains all server state and configuration
 */
typedef struct
{
  // Network components
  int server_fd; // Main server socket
  int epoll_fd;  // epoll instance for async I/O

  // Processing
  thread_pool_t *pool; // Thread pool for request processing

  // Server state
  volatile int running; // 1 when server should keep running

  // AI configuration
  char gemini_api_key[256]; // Google Gemini API key
} server_context_t;

#endif /* SERVER_H */
