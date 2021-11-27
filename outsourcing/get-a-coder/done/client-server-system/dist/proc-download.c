/*
 * proc-download.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 */
#include "server-funcs.h"

/* this file was modified by io_pigio according to his requirements */
//#define _MODIFIED_CODE_

/* 500 means success */
/* please read error-codes.txt for values used in protocol */

/* please read 'protocol.txt' for understanding the protocol used */
/* handle download */
int proc_download(int connfd, const char *buf)
{
  int fd = -1;
  int ret = -1;
  size_t n;
  size_t len;
  char comd[10];
  char path[PATH_MAX+1];
  char cwd[PATH_MAX+1];
  char buffer[1024];
  int del;
  const char *p, *ptr;
  struct stat statbuf;

  /* here for simplicity in comments, i'm using : as delimeter */
  /* COPY|MOVE <cwd>:<path>:<del> */
  sscanf(buf, "%s", comd); /* comd = COPY|MOVE */
  len = strlen(comd);
  len++; /* skip the <space> */
  buf += len; /* buf now points to <cwd>:... */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(cwd, buf, len);
  cwd[len] = 0;
  len++; /* skip the DELIM */
  buf += len; /* buf now points to <path> */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(path, buf, len);
  path[len] = 0;
  len++; /* skip the DELIM */
  buf += len; /* buf now points to <del> */

  del = *buf - '0'; /* get the numeric value */

  ptr = path;
  /* if 'path' contains starting '/', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
  if (ptr[0] != DIR_DELIM)
#endif
  {
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
      strcat(buffer, ptr);
      ptr = buffer;
      p = realpath(ptr, cwd);
      if (!p) {
	perror("realpath");
	sprintf(buffer, "400 ERROR server: %s\n", strerror(errno));
	sendn(connfd, buffer, strlen(buffer));
	return 1;
      } else {
	ptr = p;
      }
    }
  }
  printf("<%s:%d> path=<%s> del=<%d>\n", __FILE__, __LINE__, ptr, del);

  /* ptr now points to the path name of the file */

  memset(&statbuf, 0, sizeof(statbuf));
  if (stat(ptr, &statbuf) != 0) {
    perror("stat");
    sprintf(buffer, "400 server: stat: %s: %s\n", path, strerror(errno));
    sendn(connfd, buffer, strlen(buffer));
    return -1;
  }
//  fprintf(stderr, "size of %s is %d\n", file, (int) statbuf.st_size);

  /* hold a read-lock here, so that while i am reading, no other threads will
   * be writing in it
   */
  /* modification */
  if (del == 0) {
    if ((ret = pthread_rwlock_rdlock(&rwlock)) != 0) {
      fprintf(stderr, "pthread_rwlock_rdlock: %s\n", strerror(ret));
      return -1;
    }
  } else {
    if ((ret = pthread_rwlock_wrlock(&rwlock)) != 0) {
      fprintf(stderr, "pthread_rwlock_wrlock: %s\n", strerror(ret));
      return -1;
    }
  }

  if ((fd = open(ptr, O_RDONLY|O_BINARY)) < 0) {
    perror("open");
    sprintf(buffer, "400 ERROR server: open: %s\n", strerror(errno));
    sendn(connfd, buffer, strlen(buffer));
    goto cleanup;
  }


  /* we're opening the file holding a lock, so the following checking is not
   * needed, because 'open' will fail for nonexisting files
   */
#ifdef __NO__
  if (stat(ptr, &statbuf) != 0) {
    perror("stat");
    sprintf(buffer,
       	"400 File is removed before the processing of your request\n");
    sendn(connfd, buffer, strlen(buffer));
    close(fd);
    return -1;
  }
#endif

  /* inform client that we've opened [and locked] the file successfully */
  sprintf(buffer, "500 OK %d\n", (int) statbuf.st_size);
  n = strlen(buffer);
  ret = sendn(connfd, buffer, n);
  if (ret < n) {
    DEBUG("sendn", errno);
    goto cleanup;
  }

  while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
    ret = sendn(connfd, buffer, n);
    if (ret < 0) {
      DEBUG("sendn", errno);
      goto cleanup;
    }
  }

  ret = 0;
cleanup:

  if (fd > 0)
    close(fd);

  if (del == 0) {
    pthread_rwlock_unlock(&rwlock);
    return ret; // no need to go down
  }

  /* we come here only if del == 1 */

  /* if successful, delete the file if 'del' is TRUE */
  if (ret == 0 && del == 1) {
//    fprintf(stderr, "Removing %s\n", ptr);
    if (remove(ptr) != 0) {
      perror("remove");
      sprintf(buffer, "400 ERROR: remove: %s\n", strerror(errno));
      n = strlen(buffer);
      ret = sendn(connfd, buffer, n);
      if (ret < n) {
	DEBUG("sendn", errno);
      }
    }
    pthread_rwlock_unlock(&rwlock);
  }

  return ret;
}
