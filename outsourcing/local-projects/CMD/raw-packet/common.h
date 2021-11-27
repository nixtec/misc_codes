/*
 * common.h
 * common header file for all
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#ifndef __USE_BSD
#define __USE_BSD		/* use BSD socket semantics */
#endif
#include <netinet/ip.h>		/* struct ip { } */
#ifndef __FAVOR_BSD
#define __FAVOR_BSD		/* use BSD structure */
#endif
#include <netinet/udp.h>	/* struct udphdr { } */
#include <netinet/tcp.h>	/* struct tcphdr { } */

#define MAXLINE 1024

typedef struct sockaddr SA; /* for ease of typing */

#endif
