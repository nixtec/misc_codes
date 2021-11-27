#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __WIN32__
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>	/* gethostbyname */
extern int h_errno;
#include <sys/wait.h>
#include <sys/file.h>
#include <pwd.h>
#include <grp.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "config.h"
#include "myunix.h"
#include "mywinx.h"


#if defined(USE_WORDEXP_H)
#include <wordexp.h>
#elif defined(USE_GLOB_H)
#include <glob.h>
#endif

//#include "exec_cmd.h"
#include "shell-support.h"
#include "cwdent.h"

//extern char cmdbuf[]; /* defined in shell.c */
//extern int cmdbufsize;
extern char redir_in[]; /* defined in shell.c */
extern int redir_in_size;
extern char redir_out[]; /* defined in shell.c */
extern int redir_out_size;

int upload_file(const char *src, const char *dstaddr,
    const char *dst, int del);
int download_file(const char *srcaddr, const char *src,
    const char *dst, int del);
int move_file(const char *srcaddr, const char *src,
    const char *dst, int del);
int copy_file(const char *src, const char *dst, int del);
int delete_file(const char *dst, const char *path);
int delete_local_file(const char *path);
int list_file(const char *srcaddr, const char *path);
int list_local_file(const char *path);
int create_folder(const char *srcaddr, const char *path);
int create_local_folder(const char *path);
int delete_folder(const char *srcaddr, const char *path);
int delete_local_folder(const char *path);
int change_folder(const char *srcaddr, const char *path);
int change_local_folder(const char *path);
int get_current_dir(const char *srcaddr);
int get_local_current_dir(void);
int execute_command(const char *srcaddr, const char *src);
void fix_path(char *path, size_t len);

#ifdef __WIN32__
/* getpass() for Windows */
char *getpass(const char *prompt);
#endif

#endif
