/*
 * pthread_mutex_destroy.c
 */
#include "pthread.h"

int pthread_mutex_destroy(pthread_mutex_t *m)
{
  CloseHandle(*m);
  return 0;
}
