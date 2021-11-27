/*
 * pthread_cond_destroy.c
 */

#include "pthread.h"

int 
pthread_cond_destroy(pthread_cond_t *cv)
{
  cv->waiters_count_ = 0;
  cv->was_broadcast_ = 0;
  CloseHandle(cv->sema_);
  DeleteCriticalSection(&cv->waiters_count_lock_);
  CloseHandle(cv->waiters_done_);

  return 0;
}
