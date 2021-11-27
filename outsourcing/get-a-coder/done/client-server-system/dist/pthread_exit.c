/*
 * pthread_exit.c
 */
#include "pthread.h"

/* silly wrapper */
void pthread_exit(void *arg)
{
  ExitThread(0);
}
