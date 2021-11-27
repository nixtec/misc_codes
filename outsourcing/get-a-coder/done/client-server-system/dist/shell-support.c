/*
 * shell-support.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 */
#include "shell-support.h"

/* reads n bytes from socket */
ssize_t readn(int fd, void *vptr, size_t n)
{
  size_t  nleft;
  ssize_t nread;
  char   *ptr;
  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nread = recv(fd, ptr, nleft, 0)) < 0) {
      if (errno == EINTR)
	nread = 0; /* and call read() again */
      else
	return (-1);
    } else if (nread == 0)
      break; /* EOF */
    nleft -= nread;
    ptr += nread;
  }
  return (n - nleft); /* return >= 0 */
}

/* writes n bytes to socket */
ssize_t writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;
  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = send(fd, ptr, nleft, 0)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
	nwritten = 0;   /* and call write() again */
      else
	return (-1);    /* error */
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return (n);
}

/* readline() routine, not-reentrant */
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];
static ssize_t my_read(int fd, char *ptr)
{
  if (read_cnt <= 0) {
again:
    if ( (read_cnt = recv(fd, read_buf, sizeof(read_buf), 0)) < 0) {
      if (errno == EINTR)
	goto again;
      return (-1);
    } else if (read_cnt == 0)
      return (0);
    read_ptr = read_buf;
  }
  read_cnt--;
  *ptr = *read_ptr++;
  return (1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t n, rc;
  char    c, *ptr;
  ptr = vptr;
  for (n = 1; n < maxlen; n++) {
    if ( (rc = my_read(fd, &c)) == 1) {
      *ptr++ = c;
      if (c  == '\n')
	break; /* newline is stored, like fgets() */
    } else if (rc == 0) {
      *ptr = 0;
      return (n - 1); /* EOF, n - 1 bytes were read */
    } else
      return (-1); /* error, errno set by read() */
  }
  *ptr  = 0; /* null terminate like fgets() */
  return (n);
}

/* not used, you may use this function to investigate how much data
 * was really read */
ssize_t readlinebuf(void **vptrptr)
{
  if (read_cnt)
    *vptrptr = read_ptr;
  return (read_cnt);
}
