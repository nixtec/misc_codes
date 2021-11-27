/*
 * pthread_cond_init.c
 */

#include "pthread.h"

int 
pthread_cond_init(pthread_cond_t *cv, const pthread_condattr_t *attr)
{
  cv->waiters_count_ = 0;
  cv->was_broadcast_ = 0;
  cv->sema_ = CreateSemaphore(NULL,       // no security
      0,          // initially 0
      0x7fffffff, // max count
      NULL);      // unnamed 
  InitializeCriticalSection(&cv->waiters_count_lock_);
  cv->waiters_done_ = CreateEvent(NULL,  // no security
      FALSE, // auto-reset
      FALSE, // non-signaled initially
      NULL); // unnamed

  if (cv->waiters_done_ == INVALID_HANDLE_VALUE) {
    return (int) GetLastError();
  }

  return 0;
}
