/*
 * support.c
 */
#include "support.h"
 
#ifndef __WIN32__
Sigfunc *Signal(int signo, Sigfunc *func)
{
  struct sigaction act, oact;

  act.sa_handler = func;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  if (signo == SIGALRM) {
#ifdef  SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;     /* SunOS 4.x */
#endif
  } else {
#ifdef  SA_RESTART
    act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
  }
  if (sigaction (signo, &act, &oact) < 0)
    return (SIG_ERR);
  return (oact.sa_handler);
}
#endif


/* receives n bytes of data from fd [socket] */
ssize_t recvn(int fd, void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nrecv;
  char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nrecv = recv(fd, ptr, nleft, 0)) < 0) {
      if (errno == EINTR)
	nrecv = 0;
      else
	return (-1);
    } else if (nrecv == 0)
      break; /* EOF */
    nleft -= nrecv;
    ptr += nrecv;
  }

  return (n - nleft);
}

/* writes n bytes of data to fd [socket] */
ssize_t sendn(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nsent;
  const char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nsent = send(fd, ptr, nleft, 0)) <= 0) {
      if (errno == EINTR)
	nsent = 0; /* and call send() again */
      else
	return (-1); /* error */
    }
    nleft -= nsent;
    ptr += nsent;
  }
  return (n);
}

#ifndef __WIN32__

/* here goes the thread-specific implementation */
/* why using thread-specific data ?
 * this is to save memory and performance issue and for thread safety
 * when you use thread-specific data, your data is allocated only once during
 * the use of the data and when thread exits, the data is freed automatically
 */
static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

/* destroys the data */
static void recvline_destructor(void *ptr)
{
  free(ptr);
}

static void recvline_once(void)
{
  int ret;
  if ((ret = pthread_key_create(&rl_key, recvline_destructor)) != 0) {
    printf("recvline_once: pthread_key_create: %s\n", strerror(ret));
  }
}

typedef struct {
  int rl_cnt;  /* initialize to 0 */
  char *rl_bufptr; /* initialize to rl_buf */
  char rl_buf[MAXLINE];
} Rline;


static ssize_t my_recv(Rline *tsd, int fd, char *ptr)
{
  if (tsd->rl_cnt <= 0) {
again:
    if ((tsd->rl_cnt = recv(fd, tsd->rl_buf, MAXLINE, 0)) < 0) {
      if (errno == EINTR)
	goto again;
      return (-1);
    } else if (tsd->rl_cnt == 0)
      return (0);
    tsd->rl_bufptr = tsd->rl_buf;
  }
  tsd->rl_cnt--;
  *ptr = *tsd->rl_bufptr++;

  return (1);
}

/* reads a line from socket [fd] */
ssize_t recvline(int fd, void *vptr, size_t maxlen)
{
  int n, rc;
  char c, *ptr;
  Rline *tsd;

  int ret;

  ret = pthread_once(&rl_once, recvline_once);
  if (ret != 0) {
    printf("recvline: pthread_once: %s\n", strerror(ret));
  }

  if ((tsd = pthread_getspecific(rl_key)) == NULL) {
    tsd = calloc(1, sizeof(Rline)); /* init to 0 */
    if ((ret = pthread_setspecific(rl_key, tsd)) != 0) {
      printf("recvline: pthread_setspecific: %s\n", strerror(ret));
    }
  }

  ptr = vptr;
  for (n = 1; n < (int) maxlen; n++) {
    if ((rc = my_recv(tsd, fd, &c)) == 1) {
      *ptr++ = c;
      if (c == '\n') /* store the newline, like fgets() */
	break;
    } else if (rc == 0) {
      if (n == 1)
	return (0); /* EOF, no data read */
      else
	break; /* EOF, some data read */
    } else
      return (-1);
  }
  *ptr = 0; /* nul terminate, like fgets() */

  return (n);
}
#else
ssize_t recvline(int fd, void *vptr, size_t maxlen)
{
  return (recv(fd, vptr, maxlen, 0));
}

#endif /* !__WIN32__ */
