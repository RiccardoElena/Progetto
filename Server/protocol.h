#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "server.h"

int send_message(int client_fd, int msg_type, const char * data);
int receive_message(int client_fd, message_t * msg);
int parse_client_message(const char * data, client_message_t * parsed_msg);

#endif /* PROTOCOL_H */
