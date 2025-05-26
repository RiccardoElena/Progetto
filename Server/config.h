/*********************************************************************************
 * ===== FILE: config.h =====
 * Contains all configuration constants and system limits
 *********************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

// ========== SERVER CONFIGURATION ==========
#define MAX_CLIENTS 1024             // Maximum concurrent clients
#define MAX_EVENTS 64                // Maximum epoll events per cycle
#define BUFFER_SIZE 4096             // Network buffer size
#define MAX_MESSAGE_SIZE 2048        // Maximum message content size
#define THREAD_POOL_SIZE 4           // Number of worker threads
#define DEFAULT_PORT 8080            // Default server port
#define BACKLOG 128                  // Listen queue size
#define CLIENT_SOCKET_TIMEOUT_SEC 30 // Max timeout

// ========== AI CONFIGURATION ==========
#define MAX_AI_RESPONSE_SIZE 2048// Maximum AI response length
#define MAX_AI_ROBOT_BEHAVIOR 2048// TODO: to be deleted
#define AI_TIMEOUT_SECONDS 15      // Timeout for AI requests
#define GEMINI_API_URL "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent"

// ========== THREAD POOL CONFIGURATION ==========
#define THREAD_POOL_MIN_SIZE 4               // Minimum threads
#define THREAD_POOL_MAX_SIZE 32              // Maximum threads
#define THREAD_POOL_INITIAL_SIZE 8           // Starting threads
#define THREAD_POOL_SCALE_INTERVAL 5         // Seconds between scaling checks
#define THREAD_POOL_SCALE_UP_THRESHOLD 0.8   // Load ratio to scale up
#define THREAD_POOL_SCALE_DOWN_THRESHOLD 0.3 // Load ratio to scale down
#define THREAD_POOL_QUEUE_HIGH_WATER 20      // Queue size to trigger scale up
#define THREAD_POOL_QUEUE_LOW_WATER 2        // Queue size to trigger scale down

// ========== MESSAGE PROTOCOL ==========
#define MSG_REQUEST 1
#define MSG_RESPONSE 2
#define MSG_DISCONNECT 3
#define MSG_ERROR 4

#endif /* CONFIG_H */
