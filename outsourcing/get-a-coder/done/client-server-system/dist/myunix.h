/*
 * myunix.h
 */
#ifndef MYUNIX_H
#define MYUNIX_H

#ifndef __WIN32__

#include <errno.h>
#include <netdb.h>
extern int h_errno;

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;

/* unix doesn't need O_BINARY, but windows needs, so defining as dummy */
#define O_BINARY 0

/* use 'closesocket' as a wrapper for 'close' since Windows has closesocket */
#define CLOSE(fd) close(fd)
#define WSAGetLastError() (h_errno ? h_errno : errno)
#define GetLastError() errno

#define MKDIR(name,mode) mkdir(name,mode)

#endif /* !__WIN32__ */

#endif
