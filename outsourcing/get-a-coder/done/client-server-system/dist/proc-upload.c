/*
 * proc-upload.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 */
#include "server-funcs.h"

/* 500 means success */
/* please read error-codes.txt for values used in protocol */

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* please read 'protocol.txt' for understanding the protocol used */
/* handle file upload */
int proc_upload(int connfd, const char *buf)
{
  int fd = -1;
  int ret = -1;
  ssize_t len;
  mode_t mode = 00644;
  char comd[10];
  char cwd[PATH_MAX+1];
  char file[PATH_MAX+1];
  char buffer[1024];
  int locked = 0;
  int n = 0;

  /* COPY|MOVE <cwd>:<file> */
  sscanf(buf, "%s", comd);
  len = strlen(comd);
  len++; /* skip the <space> */
  buf += len; /* buf now points to <cwd>:... */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(cwd, buf, len);
  cwd[len] = 0;
  len++; /* skip the DELIM */
  buf += len; /* buf now points to <file> */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(file, buf, len);
  file[len] = 0;
  len++; /* skip the DELIM */
  buf += len; /* buf now points to <file> */

  //sscanf(buf, "%s %[^\n]", comd, file);
  printf("<%s:%d> cwd=<%s> file=<%s>\n", __FILE__, __LINE__, cwd, file);

  char *ptr;
  ptr = file;
  /* if 'file' contains starting '/', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
    if (ptr[0] != DIR_DELIM)
#endif
    { // relative path name
      if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
	strcpy(buffer, cwd);
	n = strlen(buffer);
	if (buffer[n-1] != DIR_DELIM) {
	  buffer[n] = DIR_DELIM; // add delimeter
	  n++;
	  buffer[n] = 0;
	}
	strcat(buffer, ptr);
	strcpy(file, buffer);
      }
    }
  /* now ptr = file = the full path of file */

  if ((ret = pthread_rwlock_wrlock(&rwlock)) != 0) {
    fprintf(stderr, "pthread_rwlock_wrlock: %s\n", strerror(ret));
    return -1;
  }

  locked = 1;

  if (access(file, F_OK) == 0) {
    DEBUG("Output file exists!", 0);
    sprintf(buffer, "400 server: Output file exists!\n");
    len = strlen(buffer);
    sendn(connfd, buffer, len);
    goto cleanup;
  }

  /* create file for writing */
  if ((fd = open(file, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, mode)) < 0) {
    perror("open");
    sprintf(buffer, "400 server: open: %s\n", strerror(errno));
    sendn(connfd, buffer, strlen(buffer));
    goto cleanup;
  }

  sprintf(buffer, "500 OK\n");
  len = strlen(buffer);
  ret = sendn(connfd, buffer, len);
  if (ret < len) {
    DEBUG("sendn", errno);
    goto cleanup;
  }

  /* get the file from client and write to file */
  while ((len = recv(connfd, buffer, sizeof(buffer), 0)) > 0) {
    ret = write(fd, buffer, len);
    if (ret < 0) {
      perror("write");
      sprintf(buffer, "server: write: %s\n", strerror(errno));
      len = strlen(buffer);
      sendn(connfd, buffer, len);
      goto cleanup;
    }
  }

  ret = 0;
cleanup:
  if (fd > 0) {
    close(fd);
  }

  if (locked) {
    pthread_rwlock_unlock(&rwlock);
  }

  return ret;
}
