/*
 * pthread_mutex_init.c
 */
#include "pthread.h"

int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *attr)
{
  *m = CreateMutex(NULL, FALSE, NULL);
  if (*m == NULL) {
    return ((int) GetLastError());
  }

  return 0;
}
