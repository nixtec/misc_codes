/*
 * pthread_detach.h
 */
#include "pthread.h"

/* silly wrapper */
int pthread_detach(pthread_t t)
{
  CloseHandle(t);
  return 0;
}

