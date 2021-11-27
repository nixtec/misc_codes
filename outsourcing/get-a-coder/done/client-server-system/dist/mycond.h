/*
 * mycond.h
 * POSIX condition variables implementation for Win32
 */
#ifndef MYCOND_H
#define MYCOND_H

#include "mypthread.h"

typedef struct {
  /* # of waiting threads */
  int waiters_count_;

  /* serialize access to waiters_count_ */
  CRITICAL_SECTION waiters_count_lock_;

  /* semaphore used to queue up threads waiting for the condition to become
   * signaled
   */
  HANDLE sema_;

  /* an auto-reset event used by the broadcast/signal thread to wait for all
   * the waiting thread(s) to wake up and be released from the semaphore
   */
  HANDLE waiters_done_;

  /* keep track of whether we were broadcasting or signaling. this allows us
   * to optimize the code if we're just signaling.
   */
  size_t was_broadcast_;
} pthread_cond_t;

typedef int pthread_condattr_t;

int pthread_cond_broadcast(pthread_cond_t *cv);
int pthread_cond_destroy(pthread_cond_t *cv);
int pthread_cond_init(pthread_cond_t *cv, const pthread_condattr_t *attr);
int pthread_cond_signal(pthread_cond_t *cv);
int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *external_mutex);



#endif
