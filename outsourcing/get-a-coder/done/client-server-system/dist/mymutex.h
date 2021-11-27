/*
 * mymutex.h
 * pthread_mutex_* wrappers for Win32
 */
#ifndef MYMUTEX_H
#define MYMUTEX_H

#include "pthread.h"

typedef HANDLE pthread_mutex_t;
typedef int pthread_mutexattr_t; /* dummy: not used in our project */

//typedef HANDLE pthread_mutex_t;

int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_init(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

#endif
