#ifndef NETWORK_H
#define NETWORK_H

#include "server.h"

// Network function declarations
void remove_client(int fd);
void accept_new_connection(void);
void handle_client_data(int client_fd);

// Global server context
extern server_context_t g_server;

#endif /* NETWORK_H */
