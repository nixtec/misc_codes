/*
 * mypthread.h
 */
#ifndef MYPTHREAD_H
#define MYPTHREAD_H

#include <stdio.h>
#include <windows.h>
#include <errno.h>

/* thread is handle */
typedef HANDLE pthread_t;
typedef int pthread_attr_t; /* dummy: not used in our project */


/* function prototypes */
int pthread_create(pthread_t *t,
    const pthread_attr_t *attr,
    void *(*start)(void *),
    void *arg);
int pthread_detach(pthread_t t);
void pthread_exit(void *arg);

#include "mymutex.h"

#endif /* !MYPTHREAD_H */
