/*
 * pthread_mutex_unlock.c
 */
#include "pthread.h"

int pthread_mutex_unlock(pthread_mutex_t *m)
{
  if (ReleaseMutex(*m) == 0) {
    return ((int) GetLastError());
  }

  return 0;
}
