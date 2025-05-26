/*********************************************************************************
 * ===== FILE: thread_pool.h/thread_pool.c =====
 * Thread pool implementation for concurrent request processing
 *********************************************************************************/

#include "thread_pool.h"

/**
 * @brief Thread pool worker function
 * Each worker thread runs this function continuously, waiting for tasks
 * @param arg Pointer to thread_pool_t structure
 * @return NULL when thread exits
 */
static void *
thread_pool_worker(void * arg)
{
    thread_pool_t * pool = (thread_pool_t *)arg;
    
    printf("[DEBUG] Worker thread %lu started\n", pthread_self()); // TODO: delete
    
    while (1) {
        // Lock the queue to check for work
        pthread_mutex_lock(&pool->queue_mutex);
        
        // Wait for work or shutdown signal
        while (pool->queue_head == NULL && !pool->shutdown) {
            pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
        }
        
        // Check if we should shutdown
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->queue_mutex);
            printf("[DEBUG] Worker thread %lu shutting down\n", pthread_self()); // TODO: delete
            break;
        }
        
        // Get task from queue
        task_t * task = pool->queue_head;
        pool->queue_head = task->next;
        if (pool->queue_head == NULL) {
            pool->queue_tail = NULL;  // Queue is now empty
        }
        
        pthread_mutex_unlock(&pool->queue_mutex);
        
        // Execute the task
        printf("[DEBUG] Worker thread %lu executing task\n", pthread_self()); // TODO: delete
        task->function(task->argument);
        free(task);  // Clean up task memory
    }
    
    return (NULL);
}

/**
 * @brief Create and initialize a thread pool
 * @param thread_count Number of worker threads to create
 * @return Pointer to thread pool, or NULL on error
 */
thread_pool_t *
thread_pool_create(int thread_count)
{
    printf("[INFO] Creating thread pool with %d threads\n", thread_count);
    
    // Allocate thread pool structure
    thread_pool_t * pool = malloc(sizeof(thread_pool_t));
    if (!pool) {
        printf("[ERROR] Failed to allocate thread pool\n");
        return (NULL);
    }
    
    // Allocate array for thread IDs
    pool->threads = malloc(sizeof(pthread_t) * thread_count);
    if (!pool->threads) {
        printf("[ERROR] Failed to allocate thread array\n");
        free(pool);
        return (NULL);
    }
    
    // Initialize pool structure
    pool->thread_count = thread_count;
    pool->queue_head = NULL;
    pool->queue_tail = NULL;
    pool->shutdown = 0;
    
    // Initialize synchronization primitives
    if (pthread_mutex_init(&pool->queue_mutex, NULL) != 0) {
        printf("[ERROR] Failed to initialize queue mutex\n");
        free(pool->threads);
        free(pool);
        return (NULL);
    }
    
    if (pthread_cond_init(&pool->queue_cond, NULL) != 0) {
        printf("[ERROR] Failed to initialize queue condition variable\n");
        pthread_mutex_destroy(&pool->queue_mutex);
        free(pool->threads);
        free(pool);
        return (NULL);
    }
    
    // Create worker threads
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool) != 0) {
            printf("[ERROR] Failed to create worker thread %d\n", i);
            
            // Clean up already created threads
            pool->shutdown = 1;
            pthread_cond_broadcast(&pool->queue_cond);
            for (int j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }
            
            pthread_mutex_destroy(&pool->queue_mutex);
            pthread_cond_destroy(&pool->queue_cond);
            free(pool->threads);
            free(pool);
            return (NULL);
        }
    }
    
    printf("[INFO] Thread pool created successfully\n");
    return (pool);
}

/**
 * @brief Add a task to the thread pool queue
 * @param pool Thread pool to add task to
 * @param function Function to execute
 * @param argument Argument to pass to function
 * @return 0 on success, -1 on error
 */
int
thread_pool_add_task(thread_pool_t * pool, void (*function)(void*), void * argument)
{
    if (!pool || !function) {
        printf("[ERROR] Invalid parameters for thread pool task\n");
        return (-1);
    }
    
    // Create new task
    task_t * task = malloc(sizeof(task_t));
    if (!task) {
        printf("[ERROR] Failed to allocate memory for task\n");
        return (-1);
    }
    
    task->function = function;
    task->argument = argument;
    task->next = NULL;
    
    // Add task to queue
    pthread_mutex_lock(&pool->queue_mutex);
    
    // Check if pool is shutting down
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->queue_mutex);
        free(task);
        printf("[WARNING] Cannot add task: thread pool shutting down\n");
        return (-1);
    }
    
    // Add to end of queue
    if (pool->queue_head == NULL) {
        pool->queue_head = task;
        pool->queue_tail = task;
    } else {
        pool->queue_tail->next = task;
        pool->queue_tail = task;
    }
    
    // Signal waiting workers
    pthread_cond_signal(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_mutex);
    
    printf("[DEBUG] Task added to thread pool queue\n"); // TODO: delete
    return (0);
}

/**
 * @brief Destroy thread pool and clean up resources
 * @param pool Thread pool to destroy
 */
void
thread_pool_destroy(thread_pool_t * pool)
{
    if (!pool) {
        return;
    }
    
    printf("[INFO] Destroying thread pool\n");
    
    // Signal shutdown to all threads
    pthread_mutex_lock(&pool->queue_mutex);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_mutex);
    
    // Wait for all threads to finish
    for (int i = 0; i < pool->thread_count; ++i) {
        pthread_join(pool->threads[i], NULL);
        printf("[DEBUG] Worker thread %d joined\n", i); // TODO: ricca gia sai
    }
    
    // Clean up remaining tasks in queue
    while (pool->queue_head) {
        task_t * task = pool->queue_head;
        pool->queue_head = task->next;
        free(task);
    }
    
    // Clean up synchronization primitives
    pthread_mutex_destroy(&pool->queue_mutex);
    pthread_cond_destroy(&pool->queue_cond);
    
    // Free memory
    free(pool->threads);
    free(pool);
    
    printf("[INFO] Thread pool destroyed\n");
}
