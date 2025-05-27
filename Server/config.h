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
#define THREAD_POOL_SIZE 4           // Number of worker threads
#define DEFAULT_PORT 8080            // Default server port
#define BACKLOG 128                  // Listen queue size
#define CLIENT_SOCKET_TIMEOUT_SEC 30 // Max timeout

// ========== AI CONFIGURATION ==========
#define MAX_AI_RESPONSE_SIZE 2048 // Maximum AI response length
#define AI_TIMEOUT_SECONDS 15     // Timeout for AI requests
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
#define MSG_AI_DIALOG_REQUEST 1
#define MSG_TEST_DIALOG_REQUEST 2
#define MSG_AI_DIALOG_RESPONSE 3
#define MSG_TEST_DIALOG_RESPONSE 4
#define MSG_ERROR 5

// ========== MESSAGE PROTOCOL CONFIGURATION ==========
#define MAX_MESSAGE_SIZE 3000 // Maximum message content size
#define MAX_PERSONALITY_SIZE 512
#define MAX_LANGUAGE_SIZE 8
#define MAX_CONVERSATION_SIZE 2048

// ========== COMPILATION FLAGS ==========
#define SHOW_DEBUG 0
#define SHOW_ERROR 0
#define SHOW_INFO 0
#define SHOW_WARNING 0
#define EASTER_EGG_MODE 0

#endif /* CONFIG_H */
