/*
 * pthread_cond_signal.c
 */
#include "pthread.h"

int
pthread_cond_signal (pthread_cond_t *cv)
{
  EnterCriticalSection (&cv->waiters_count_lock_);
  int have_waiters = cv->waiters_count_ > 0;
  LeaveCriticalSection (&cv->waiters_count_lock_);

  // If there aren't any waiters, then this is a no-op.  
  if (have_waiters) {
    ReleaseSemaphore (cv->sema_, 1, 0);
  }

  return 0;
}
