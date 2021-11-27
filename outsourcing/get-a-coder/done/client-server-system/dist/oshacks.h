/*
 * oshacks.h
 * some macro and other hacks for wrapping functions/syscalls
 */
#ifndef OSHACKS_H
#define OSHACKS_H

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* arg2 = error_value */
#define DEBUG(arg1,arg2) fprintf(stderr, "<%s:%d> %s: %s. %s\n", __FILE__, \
    __LINE__, __func__, arg1, (arg2) ? strerror(arg2) : "")

#define TRACE_LINE() fprintf(stderr, "<%s:%d> func=%s errno=%d\n", \
    __FILE__, __LINE__, __func__, errno)

/* Windows specific code */
#ifdef __WIN32__
#include <windows.h>
#include <io.h>
#include <sys/locking.h>
#include <share.h>
#else
#include <sys/file.h>
#endif

#ifdef __WIN32__

#define _fullpath(res,path,size) \
  (GetFullPathName ((path), (size), (res), NULL) ? (res) : NULL)

/* windows doesn't have realpath() */
#define realpath(path,resolved_path) _fullpath(resolved_path, path, MAX_PATH)

struct tm *localtime_r(const time_t *timep, struct tm *result);

#define LOCK_UN 0
#define LOCK_SH 1
#define LOCK_EX 2

int flock(int fd, int operation);

#endif /* __WIN32__ */


int open_and_lock(const char *pathname, int flags, mode_t mode, int op);
int unlock(int fd);

#endif /* !OSHACKS_H */
