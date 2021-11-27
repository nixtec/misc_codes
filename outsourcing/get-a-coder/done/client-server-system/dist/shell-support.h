#ifndef SHELL_SUPPORT_H
#define SHELL_SUPPORT_H

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#ifdef __WIN32__
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#endif
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "config.h"

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxlen);

#endif
