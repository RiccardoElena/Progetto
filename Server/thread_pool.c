/*********************************************************************************
 * ===== FILE: thread_pool.h/thread_pool.c =====
 * Thread pool implementation for concurrent request processing and autoscaling
 *********************************************************************************/

#include "thread_pool.h"
#include "utils.h"

/**
 * @brief Handler function to delete thread
 */
static void
thread_cleanup(void *mutex)
{
  pthread_mutex_unlock(mutex);
}

/**
 * @brief Enhanced worker thread function with performance tracking
 */
static void *thread_pool_worker(void *arg)
{
  thread_pool_t *pool = (thread_pool_t *)arg;

  pthread_cleanup_push(thread_cleanup, &pool->queue_mutex);

#if SHOW_DEBUG
  printf("[DEBUG] Worker thread %lu started\n", pthread_self());
#endif
  while (1)
  {
    struct timespec task_start, task_end;

    // Lock the queue to check for work
    pthread_mutex_lock(&pool->queue_mutex);

    // Make thread cancellable
    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))
    {
#if SHOW_ERROR
      printf("[ERROR] Thread %d cannot cancel state\n", pthread_self());
#endif
      break;
    }

    // Wait for work or shutdown signal
    while (pool->queue_head == NULL && !pool->shutdown)
    {
      pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
    }

    // Make thread cancellable
    if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL))
    {
#if SHOW_ERROR
      printf("[ERROR] Thread %d cannot cancel state\n", pthread_self());
#endif
      break;
    }

    // Check if we should shutdown
    if (pool->shutdown)
    {
      pthread_mutex_unlock(&pool->queue_mutex);
#if SHOW_DEBUG
      printf("[DEBUG] Worker thread %lu shutting down\n", pthread_self());
#endif
      break;
    }
    // Get task from queue
    task_t *task = pool->queue_head;
    pool->queue_head = task->next;
    if (pool->queue_head == NULL)
    {
      pool->queue_tail = NULL;
    }

    // Update metrics
    atomic_fetch_sub(&pool->queue_size, 1);
    atomic_fetch_add(&pool->active_threads, 1);

    pthread_mutex_unlock(&pool->queue_mutex);

    // Execute the task with timing
    clock_gettime(CLOCK_MONOTONIC, &task_start);

#if SHOW_DEBUG
    printf("[DEBUG] Worker thread %lu executing task\n", pthread_self());
#endif
    task->function(task->argument);

    clock_gettime(CLOCK_MONOTONIC, &task_end);

    // Calculate task execution time
    long task_time_ms = (task_end.tv_sec - task_start.tv_sec) * 1000 +
                        (task_end.tv_nsec - task_start.tv_nsec) / 1000000;

    pthread_mutex_lock(&pool->queue_mutex);

    // Update performance metrics
    atomic_fetch_add(&pool->total_tasks_completed, 1);
    atomic_fetch_add(&pool->total_task_time_ms, task_time_ms);

    // Update min/max task times
    long current_min = atomic_load(&pool->min_task_time_ms);
    if (current_min == 0 || task_time_ms < current_min)
    {
      atomic_store(&pool->min_task_time_ms, task_time_ms);
    }

    long current_max = atomic_load(&pool->max_task_time_ms);
    if (task_time_ms > current_max)
    {
      atomic_store(&pool->max_task_time_ms, task_time_ms);
    }

    atomic_fetch_sub(&pool->active_threads, 1);

    pthread_mutex_unlock(&pool->queue_mutex);

    free(task);
  }

  pthread_cleanup_pop(0);

  return NULL;
}

/**
 * @brief Create thread pool with enhanced configuration
 */
thread_pool_t *thread_pool_create(int initial_thread_count)
{
#if SHOW_INFO
  printf("[INFO] Creating enhanced thread pool with %d initial threads\n", initial_thread_count);
#endif
  thread_pool_t *pool = malloc(sizeof(thread_pool_t));
  if (!pool)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to allocate thread pool\n");
#endif
    return NULL;
  }

  // Initialize with auto-scaling parameters
  if (thread_pool_create_with_limits(pool,
                                     THREAD_POOL_MIN_SIZE,
                                     THREAD_POOL_MAX_SIZE,
                                     initial_thread_count) != 0)
  {
    free(pool);
    return NULL;
  }

  return pool;
}

/**
 * @brief Initialize thread pool with specific limits
 */
int thread_pool_create_with_limits(thread_pool_t *pool, int min_threads, int max_threads, int initial_threads)
{
  // Validate parameters
  if (initial_threads < min_threads || initial_threads > max_threads)
  {
#if SHOW_ERROR
    printf("[ERROR] Invalid thread pool parameters: min=%d, initial=%d, max=%d\n",
           min_threads, initial_threads, max_threads);
#endif
    return -1;
  }

  // Allocate thread array for maximum size
  pool->threads = malloc(sizeof(pthread_t) * max_threads);
  if (!pool->threads)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to allocate thread array\n");
#endif
    return -1;
  }

  // Initialize pool structure
  pool->thread_count = initial_threads;
  pool->min_threads = min_threads;
  pool->max_threads = max_threads;
  pool->queue_head = NULL;
  pool->queue_tail = NULL;
  pool->shutdown = 0;
  pool->last_scale_time = time(NULL);

  // Initialize atomic counters
  atomic_init(&pool->active_threads, 0);
  atomic_init(&pool->queue_size, 0);
  atomic_init(&pool->total_tasks_completed, 0);
  atomic_init(&pool->total_task_time_ms, 0);
  atomic_init(&pool->min_task_time_ms, 0);
  atomic_init(&pool->max_task_time_ms, 0);

  // Initialize synchronization primitives
  if (pthread_mutex_init(&pool->queue_mutex, NULL) != 0)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to initialize queue mutex\n");
#endif
    free(pool->threads);
    return -1;
  }

  if (pthread_cond_init(&pool->queue_cond, NULL) != 0)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to initialize queue condition variable\n");
#endif
    pthread_mutex_destroy(&pool->queue_mutex);
    free(pool->threads);
    return -1;
  }

  // Create initial worker threads
  for (int i = 0; i < initial_threads; ++i)
  {
    if (pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool) != 0)
    {
#if SHOW_ERROR
      printf("[ERROR] Failed to create worker thread %d\n", i);
#endif

      // Clean up already created threads
      pool->shutdown = 1;
      pthread_cond_broadcast(&pool->queue_cond);
      for (int j = 0; j < i; j++)
      {
        pthread_join(pool->threads[j], NULL);
      }

      pthread_mutex_destroy(&pool->queue_mutex);
      pthread_cond_destroy(&pool->queue_cond);
      free(pool->threads);
      return -1;
    }
  }
#if SHOW_INFO
  printf("[INFO] Enhanced thread pool created: %d threads (min=%d, max=%d)\n",
         initial_threads, min_threads, max_threads);
#endif
  return 0;
}

/**
 * @brief Auto-scaling logic - THE CORE OF STRATEGY 1
 */
void thread_pool_auto_scale(thread_pool_t *pool)
{
  if (!pool)
    return;

  time_t now = time(NULL);

  // Check scaling interval
  if (now - pool->last_scale_time < THREAD_POOL_SCALE_INTERVAL)
  {
    return;
  }

  pthread_mutex_lock(&pool->queue_mutex);

  int current_threads = pool->thread_count;
  int active_threads = atomic_load(&pool->active_threads);
  int queue_size = atomic_load(&pool->queue_size);

  // Calculate load ratio
  float load_ratio = (float)active_threads / current_threads;

#if SHOW_DEBUG
  printf("[DEBUG] Auto-scale check: threads=%d, active=%d, queue=%d, load=%.1f%%\n",
         current_threads, active_threads, queue_size, load_ratio * 100);
#endif

  // SCALE UP logic
  if (load_ratio > THREAD_POOL_SCALE_UP_THRESHOLD &&
      queue_size > THREAD_POOL_QUEUE_HIGH_WATER &&
      current_threads < pool->max_threads)
  {

    int new_thread_count = ((current_threads * 3) + 1) / 2; // Scale up by 50%
    if (new_thread_count > pool->max_threads)
    {
      new_thread_count = pool->max_threads;
    }

#if SHOW_INFO
    printf("[INFO] SCALING UP: %d -> %d threads (load=%.1f%%, queue=%d)\n",
           current_threads, new_thread_count, load_ratio * 100, queue_size);
#endif
    //  Create additional threads
    for (int i = current_threads; i < new_thread_count; i++)
    {
      if (pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool) == 0)
      {
        pool->thread_count++;
#if SHOW_DEBUG
        printf("[DEBUG] Created additional worker thread %d\n", i);
        printf("[DEBUG] thread count = %d\n", pool->thread_count);
#endif
      }
      else
      {
#if SHOW_ERROR
        printf("[ERROR] Failed to create additional thread %d\n", i);
#endif
        break;
      }
    }
  }

  // SCALE DOWN logic
  else if (load_ratio < THREAD_POOL_SCALE_DOWN_THRESHOLD &&
           queue_size < THREAD_POOL_QUEUE_LOW_WATER &&
           current_threads > pool->min_threads)
  {

    int new_thread_count = ((current_threads * 3) + 1) / 4; // Scale down by 25%
    if (new_thread_count < pool->min_threads)
    {
      new_thread_count = pool->min_threads;
    }

#if SHOW_INFO
    printf("[INFO] SCALING DOWN: %d -> %d threads (load=%.1f%%, queue=%d)\n",
           current_threads, new_thread_count, load_ratio * 100, queue_size);
#endif
    for (int i = (pool->thread_count - 1); i >= new_thread_count; --i)
    {
      if (pthread_cancel(pool->threads[i]))
      {
#if SHOW_ERROR
        printf("[ERROR] Failed to cancel thread %d\n", i);
#endif
        break;
      }
#if SHOW_INFO
      printf("[INFO] Thread %d correctly canceled\n", i);
#endif
    }

    pool->thread_count = new_thread_count;
  }

  pool->last_scale_time = now;
  pthread_mutex_unlock(&pool->queue_mutex);
}

/**
 * @brief Enhanced task addition with queue size tracking
 */
int thread_pool_add_task(thread_pool_t *pool, void (*function)(void *), void *argument)
{
  if (!pool || !function)
  {
#if SHOW_ERROR
    printf("[ERROR] Invalid parameters for thread pool task\n");
#endif
    return -1;
  }

  // Create new task
  task_t *task = malloc(sizeof(task_t));
  if (!task)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to allocate memory for task\n");
#endif
    return -1;
  }

  task->function = function;
  task->argument = argument;
  task->next = NULL;

  // Add task to queue
  pthread_mutex_lock(&pool->queue_mutex);

  // Check if pool is shutting down
  if (pool->shutdown)
  {
    pthread_mutex_unlock(&pool->queue_mutex);
    free(task);
#if SHOW_WARNING
    printf("[WARNING] Cannot add task: thread pool shutting down\n");
#endif
    return -1;
  }

  // Add to end of queue
  if (pool->queue_head == NULL)
  {
    pool->queue_head = task;
    pool->queue_tail = task;
  }
  else
  {
    pool->queue_tail->next = task;
    pool->queue_tail = task;
  }

  // Update queue size
  atomic_fetch_add(&pool->queue_size, 1);

  // Signal waiting workers
  pthread_cond_signal(&pool->queue_cond);
  pthread_mutex_unlock(&pool->queue_mutex);

#if SHOW_DEBUG
  printf("[DEBUG] Task added to thread pool queue (queue_size=%d)\n",
         atomic_load(&pool->queue_size));
#endif
  // Trigger auto-scaling check
  thread_pool_auto_scale(pool);

  return 0;
}

/**
 * @brief Get current load percentage for monitoring
 */
int thread_pool_get_load_percentage(thread_pool_t *pool)
{
  if (!pool)
    return 0;

  int active = atomic_load(&pool->active_threads);
  int total = pool->thread_count;

  if (total == 0)
    return 0;
  return (active * 100) / total;
}

/**
 * @brief Print detailed thread pool statistics
 */
void thread_pool_print_stats(thread_pool_t *pool)
{
  if (!pool)
    return;

  long total_tasks = atomic_load(&pool->total_tasks_completed);
  long total_time = atomic_load(&pool->total_task_time_ms);
  long min_time = atomic_load(&pool->min_task_time_ms);
  long max_time = atomic_load(&pool->max_task_time_ms);

  printf("\n=== THREAD POOL STATISTICS ===\n");
  printf("Thread count: %d (min=%d, max=%d)\n",
         pool->thread_count, pool->min_threads, pool->max_threads);
  printf("Active threads: %d\n", atomic_load(&pool->active_threads));
  printf("Queue size: %d\n", atomic_load(&pool->queue_size));
  printf("Load percentage: %d%%\n", thread_pool_get_load_percentage(pool));
  printf("Total tasks completed: %ld\n", total_tasks);

  if (total_tasks > 0)
  {
    printf("Average task time: %ld ms\n", total_time / total_tasks);
    printf("Min task time: %ld ms\n", min_time);
    printf("Max task time: %ld ms\n", max_time);
  }
  printf("===============================\n\n");
}

/**
 * @brief Enhanced destroy function (AGGIORNATO)
 */
void thread_pool_destroy(thread_pool_t *pool)
{
  if (!pool)
    return;

#if SHOW_INFO
  printf("[INFO] Destroying enhanced thread pool\n");
#endif
  // Print final statistics
  thread_pool_print_stats(pool);

  // Signal shutdown to all threads
  pthread_mutex_lock(&pool->queue_mutex);
  pool->shutdown = 1;
  pthread_cond_broadcast(&pool->queue_cond);
  pthread_mutex_unlock(&pool->queue_mutex);

  // Wait for all threads to finish (use max_threads, not thread_count)
  for (int i = 0; i < pool->max_threads; ++i)
  {
    if (i < pool->thread_count)
    {
      pthread_join(pool->threads[i], NULL);
#if SHOW_DEBUG
      printf("[DEBUG] Worker thread %d joined\n", i);
#endif
    }
  }

  // Clean up remaining tasks in queue
  while (pool->queue_head)
  {
    task_t *task = pool->queue_head;
    pool->queue_head = task->next;
    free(task);
  }

  // Clean up synchronization primitives
  pthread_mutex_destroy(&pool->queue_mutex);
  pthread_cond_destroy(&pool->queue_cond);

  // Free memory
  free(pool->threads);
  free(pool);

#if SHOW_DEBUG
  printf("[INFO] Enhanced thread pool destroyed\n");
#endif
}
