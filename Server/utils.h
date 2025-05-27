#ifndef UTILS_H
#define UTILS_H

#include "server.h"
#include <fcntl.h>

int make_socket_non_blocking(int fd);
void safe_strncpy(char * dest, const char * src, size_t size);
char * trim_whitespace(char * str);
char * test_response();

#endif /* UTILS_H */
