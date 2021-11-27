/* server.h */
#ifndef SERVER_H
#define SERVER_H


#include <stdio.h>		/* for standard input/output functions */
#include <stdlib.h>		/* for exit and some useful functions */
#include <string.h>		/* string manipulation */
#include <errno.h>		/* error handling and logging */
#include <sys/stat.h>		/* file statistics */
#include <sys/types.h>		/* standard type definitions */
#ifndef __WIN32__
#include <sys/socket.h>		/* for socket (client/server) interaction */
#include <arpa/inet.h>		/* internet related functions */
#include <netinet/in.h>		/*        do        */
#include <sys/wait.h>		/* wait(), waitpid(), etc.*/
#include <sys/select.h>
#include <pthread.h>		/* POSIX thread specific functions */
#include <sys/file.h>		/* flock() */
#include <pwd.h>		/* passwd info */
#include <grp.h>		/* group info */
#endif
#include <time.h>		/* time() */
#include <signal.h>		/* ANSI signal handling */
#include <unistd.h>		/* universal system specific functions */
#include <fcntl.h>		/* open(), and related definitions */


#include "config.h"		/* common things between server and client */
#include "myunix.h"
#include "mywinx.h"
#include "cmdmap.h"		/* command mapping */


/* function prototypes */
void manage_client(int connfd);		/* handle client */
void reload_config(int signo);		/* signal handler */
void handle_signal(int signo);		/* signal handler */
void handle_sigchld(int signo);		/* SIGCHLD */
void pipe_signal(int signo);		/* SIGPIPE */

#endif  /* SERVER_H */
