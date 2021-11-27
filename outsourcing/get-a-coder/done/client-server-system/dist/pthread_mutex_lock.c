/*
 * pthread_mutex_lock.c
 */
#include "pthread.h"

int pthread_mutex_lock(pthread_mutex_t *m)
{
  int ret = 0;

  ret = WaitForSingleObject(*m, INFINITE);
  switch (ret) {
    case WAIT_OBJECT_0:
      ret = 0;
      break;
    default:
      fprintf(stderr, "WaitForSingleObject [%s:%d] error!\n",
	  __FILE__, __LINE__);
      ret = ((int) GetLastError());
      break;
  }

  return ret;
}
