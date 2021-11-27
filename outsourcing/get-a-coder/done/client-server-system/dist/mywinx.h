/*
 * mywin.h
 * windows specific routines and definitons
 */
#ifndef MYWINX_H
#define MYWINX_H

#ifdef __WIN32__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <io.h>			/* open(), and file related routines */

/* windows doesn't have uid_t, so make it dummy integer */
typedef unsigned int uid_t;	/* in windows we will ignore it */
//typedef unsigned int socklen_t;

// windows has SHUT_WR as SD_SEND in winsock2.h
#define SHUT_WR SD_SEND
#define SHUT_RD SD_RECV

#define MKDIR(name,mode) mkdir(name)
#define CLOSE(sock) closesocket(sock)

#include "pthread.h"

#endif		/* __WIN32__ */

#endif		/* !MYWINX_H */
