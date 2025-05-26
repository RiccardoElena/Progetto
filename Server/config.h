/*********************************************************************************
 * ===== FILE: config.h =====
 * Contains all configuration constants and system limits
 *********************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

// ========== SERVER CONFIGURATION ==========
#define MAX_CLIENTS                1024        // Maximum concurrent clients
#define MAX_EVENTS                 64          // Maximum epoll events per cycle
#define BUFFER_SIZE                4096        // Network buffer size
#define MAX_MESSAGE_SIZE           2048        // Maximum message content size
#define THREAD_POOL_SIZE           4           // Number of worker threads
#define DEFAULT_PORT               8080        // Default server port
#define BACKLOG                    128         // Listen queue size
#define CLIENT_SOCKET_TIMEOUT_SEC  30          // Max timeout

// ========== AI CONFIGURATION ==========
#define MAX_AI_RESPONSE_SIZE       1024        // Maximum AI response length
#define AI_TIMEOUT_SECONDS         15          // Timeout for AI requests
#define GEMINI_API_URL             "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent"

// ========== MESSAGE PROTOCOL ==========
#define MSG_REQUEST                1
#define MSG_RESPONSE               2
#define MSG_DISCONNECT             3
#define MSG_ERROR                  4

#endif /* CONFIG_H */
