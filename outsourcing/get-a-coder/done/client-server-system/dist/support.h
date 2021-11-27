/*
 * thread-safe recvline()
 */
#ifndef _SUPPORT_H_
#define _SUPPORT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef __WIN32__
#include <ws2tcpip.h>
#else
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>
#endif
#include <errno.h>
#include <unistd.h>

ssize_t recvn(int fd, void *vptr, size_t n);
ssize_t sendn(int fd, const void *vptr, size_t n);
ssize_t recvline(int fd, void *vptr, size_t maxlen);

#ifndef __WIN32__
typedef    void    Sigfunc(int);
Sigfunc *Signal(int signo, Sigfunc *func);
#else
#define Signal signal
#endif

#ifndef MAXLINE
#define MAXLINE 120
#endif

#endif
