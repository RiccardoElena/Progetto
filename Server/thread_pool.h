#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "server.h"

thread_pool_t * thread_pool_create(int thread_count);
int thread_pool_add_task(thread_pool_t * pool, void (*function)(void*), void * argument);
void thread_pool_destroy(thread_pool_t * pool);

#endif
