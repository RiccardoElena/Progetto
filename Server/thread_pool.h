#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "server.h"

thread_pool_t * thread_pool_create(int thread_count);
int thread_pool_add_task(thread_pool_t * pool, void (*function)(void*), void * argument);
void thread_pool_destroy(thread_pool_t * pool);
int thread_pool_create_with_limits(thread_pool_t * pool, int min_threads, int max_threads, int initial_threads);
void thread_pool_auto_scale(thread_pool_t * pool);
void thread_pool_print_stats(thread_pool_t * pool);
int thread_pool_get_load_percentage(thread_pool_t * pool);

#endif
