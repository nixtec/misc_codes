/*
 * server-funcs.h
 */
#ifndef SERVER_FUNCS_H
#define SERVER_FUNCS_H

#include "server.h"	/* server definitions */
#include "support.h"	/* thread-safe readline() and other routines */

int process_exec(int connfd, const char *buf); /* handle 'execute' */
int process_function(int connfd, const char *buf); /* handle functions */
int proc_upload(int connfd, const char *buf); /* handle upload */
int proc_download(int connfd, const char *buf); /* handle download */
int proc_move(int connfd, const char *buf); /* server-server move */
int do_local_copy(const char *src, const char *dst); /* local copy files */
int proc_none(int connfd, const char *buf); /* handle functions which don't
					       need message transfer */
int proc_exec(int connfd, const char *buf);	/* main execute routine */
int do_getcwd(int connfd); /* get current directory */

//extern pthread_mutex_t pwlock; /* defined in server.c */

extern pthread_rwlock_t rwlock; /* defined and initialized in server.c */
extern char global_shell_path[]; /* defined and initialized in server.c */

#endif
