/*
 * pthread_create.c
 */
#include "pthread.h"

int pthread_create(pthread_t *t,
    const pthread_attr_t *attr,
    void *(*start)(void *),
    void *arg)
{
  *t = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) start, arg, 0, NULL);
  if (*t == NULL) {
    return ((int) GetLastError());
  }

  return 0;
}

