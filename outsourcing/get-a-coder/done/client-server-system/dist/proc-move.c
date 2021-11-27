/*
 * proc-move.c 
 */
#include "server-funcs.h"

//#define _MODIFIED_CODE_

/* 500 means success */
/* please read error-codes.txt for values used in protocol */

/* please read 'protocol.txt' for understanding the protocol used */
/* handle 'move' */

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------------------------------------
 * PROC_MOVE
 * -------------------------------------------------------------------------
 *
 */
 
int proc_move(int connfd, const char *buf)
{
  //  int fd = -1;
  int ret = -1;
  size_t n = -1;
  size_t len = -1;
  //  mode_t mode = 00644;
  char comd[10] = {};
  char cwd[PATH_MAX+1];
  char src[PATH_MAX] = {};
  char dst[PATH_MAX] = {};
  char buffer[PATH_MAX+1] = {};
  const char *p, *ptr;

  /* MOVE <cwd>:<src>:<dst> */
  sscanf(buf, "%s", comd); /* comd = COPY|MOVE */
  len = strlen(comd);
  len++; /* skip the <space> */
  buf += len; /* buf now points to <cwd>:... */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(cwd, buf, len);
  cwd[len] = 0;
  len++; /* len++ will skip the DELIM */
  buf += len; /* buf now points to <src> */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(src, buf, len);
  src[len] = 0;
  len++; /* len++ will skip the DELIM */
  buf += len; /* buf now points to <dst> */

  len = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(dst, buf, len);
  dst[len] = 0; /* n++ will skip the DELIM */
  len++;
  buf += len;

  ptr = src;
  /* if 'src' contains starting '/', then we can safely ignore cwd */
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
	p = realpath(ptr, src);
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
  /* now ptr = src = the full path of src */

  ptr = dst;
  /* if 'dst' contains starting '/', then we can safely ignore cwd */
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
	strcpy(dst, buffer);
      }
    }
  /* now ptr = dst = the full path of dst */

  printf("<%s:%d> src=<%s> dst=<%s>\n", __FILE__, __LINE__, src, dst);

  if (!strcmp(comd, "COPY")) {
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

  if (access(src, F_OK) != 0) 
  { // file exists
    fprintf(stderr, "Input file not exists\n");
    sprintf(buffer, "400 server: Input file not exists\n");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    goto cleanup; // don't proceed
  }


  if (access(dst, F_OK) == 0) 
  { // file exists
    fprintf(stderr, "Output file already exists\n");
    sprintf(buffer, "400 server: Output file already exists\n");
    //pthread_mutex_unlock(&mutex);
    n = strlen(buffer);
    sendn(connfd, buffer, n);
    ret = 0; // don't proceed
    goto cleanup;
  }


  sprintf(buffer, "500 OK\n"); /* command supported */
  n = strlen(buffer);
  ret = sendn(connfd, buffer, n);

  if (ret < n) 
  {
    DEBUG("sendn", errno);
    goto cleanup;
  }

  if (!strcmp(comd, "COPY")) 
  {
    ret = do_local_copy(src, dst);
    if (ret != 0) 
    {
      sprintf(buffer, "server: %s\n", strerror(ret));
      n = strlen(buffer);
      ret = sendn(connfd, buffer, n);
      if (ret < n) 
      {
	DEBUG("sendn", errno);
	goto cleanup;
      }
    }
  } 
  else 
  { /* MOVE */
    ret = rename(src, dst);
    if (ret == 0)
      goto cleanup;

    if (errno == EXDEV) 
    {
      ret = do_local_copy(src, dst);
      if (ret != 0) 
      {
	sprintf(buffer, "server: %s\n", strerror(ret));
	n = strlen(buffer);
	ret = sendn(connfd, buffer, n);
	if (ret < n) 
	{
	  DEBUG("sendn", errno);
	  goto cleanup;
	}
      } 
      else 
      {
	ret = remove(src); /* remove the src file */
	if (ret != 0) 
	{
	  TRACE_LINE();
	  sprintf(buffer, "server: %s\n", strerror(errno));
	  n = strlen(buffer);
	  ret = sendn(connfd, buffer, n);
	  if (ret < n) 
	  {
	    DEBUG("sendn", errno);
	    goto cleanup;
	  }
	}
      }
    } 
    else 
    {
      sprintf(buffer, "server: %s\n", strerror(errno));
      n = strlen(buffer);
      ret = sendn(connfd, buffer, n);
      if (ret < n) 
      {
	DEBUG("sendn", errno);
	goto cleanup;
      }
    }
  }

  ret = 0;

cleanup:

  pthread_rwlock_unlock(&rwlock);

  return ret;
}

/* -------------------------------------------------------------------------
 * DO_LOCAL_COPY
 * -------------------------------------------------------------------------
 *
 */

int do_local_copy(const char *src, const char *dst)
{
  //  fprintf(stderr, "%s entered\n", __func__);

  int ret = -1;
  ssize_t nread = -1;
  char buf[1024] = {};
  int fd1 = -1;
  int fd2 = -1;

  if ((fd1 = open(src, O_RDONLY|O_BINARY)) < 0) {
    ret = errno;
    perror("open");
    goto cleanup;
  }

  /* create file for writing */
  if ((fd2 = open(dst, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, 00644)) < 0) {
    ret = errno; // don't proceed
    perror("open");
    goto cleanup;
  }


  while ((nread = read(fd1, buf, sizeof(buf))) > 0) {
    ret = write(fd2, buf, nread);
    if (ret < nread) 
    {
      ret = errno;
      perror("write");
      goto cleanup;
    }
  }

  ret = 0;

cleanup:

  if (fd1 > 0)
    close(fd1);
  if (fd2 > 0)
    close(fd2);

  return ret;
}
