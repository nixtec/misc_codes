/*
 * server.c
 * main server routines
 * Copyright (C) Ayub <mrayub@gmail.com>
 * for any details of any functions, use manpages
 */
#include "server.h"		/* definitions corresponding to server */
#include "server-funcs.h"
#include "support.h"
#include "cmdmap.h"

void *do_work(void *arg);

void fix_path(char *path, size_t len);

//pthread_mutex_t pwlock = PTHREAD_MUTEX_INITIALIZER;

pthread_rwlock_t rwlock;

char global_shell_path[PATH_MAX+1];

static int s_sock; // globally declared so that can be closed from sig

int main(int argc, char **argv)
{
  pthread_t tid;		/* thread identifier */

  int connfd;
  struct sockaddr_in servaddr, cliaddr;

  socklen_t clilen;		/* client's socket struct length */

//  int i;
  int ret;			/* return value */

  const int on = 1; /* SO_REUSEADDR on [socket options] */

  (void) argc;			/* just ignore warnings about argc */
  (void) argv;			/* just ignore warnings about argv */

  pthread_rwlock_init(&rwlock, NULL);

#ifdef __WIN32__
  strcpy(global_shell_path, "cmd.exe");
#endif

  /*
  if (geteuid() != 0) {
    fprintf(stderr, "You must have to be root to run this program!\n");
    return 1;
  }
  */

#ifdef __WIN32__
  /* initialize dll */
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
    printf("Socket Initialization Error. Program aborted\n");
    return 1;
  }
#endif


  /* create server socket */
  if ((s_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    DEBUG("socket", errno);
//    fprintf(stderr, "main: socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* set socket option for reusing existing connection */
  if (setsockopt(s_sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &on,
       	sizeof(on)) < 0) {
    DEBUG("setsockopt", errno);
    fprintf(stderr, "main: setsockopt: %s\n", strerror(errno));
  }

  /* initialize server socket address structure */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = PF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* to network byte order */
  servaddr.sin_port = htons(SERV_PORT); /* convert to network byte order */

  /* bind socket to our desitred port */
  if (bind(s_sock, (struct sockaddr *) &servaddr,
       	sizeof(servaddr)) == -1) {
    DEBUG("bind", errno);
//    fprintf(stderr, "main: bind: %s\n", strerror(errno));
    CLOSE(s_sock);
    exit(EXIT_FAILURE);
  }

  /* now listen for connections */
  if (listen(s_sock, LISTENQ) == -1) {
    DEBUG("listen", errno);
    CLOSE(s_sock);
    exit(EXIT_FAILURE);
  }

  /* handle some signals [you may handle as many signals as needed] */
  // Windows doesn't have SIGHUP
#ifndef __WIN32__
  Signal(SIGHUP, reload_config); /* define reload_config() to do something */
  Signal(SIGPIPE, pipe_signal); /* handle broken pipe */
  /* must handle it otherwise ambiguous things happen */
  Signal(SIGCHLD, handle_sigchld);
  //signal(SIGCHLD, SIG_IGN);
#endif
  Signal(SIGINT, handle_signal); /* exit */
  Signal(SIGTERM, handle_signal); /* exit */

#ifndef __WIN32__
  /* be daemon */
  setsid(); // You may use FreeConsole() for Windows here
#endif

  /* load command map here */
  cmdmap_init();

  DEBUG("Waiting for connections...", 0);

  /* Windows doesn't allow NULL as sockaddr buffer in the accept() call
  WSAEFAULT
  10014
  Bad address.
    The system detected an invalid pointer address in attempting to use a
    pointer argument of a call. This error occurs if an application passes an
    invalid pointer value, or if the length of the buffer is too small. For
    instance, if the length of an argument, which is a sockaddr structure, is
    smaller than the sizeof(sockaddr).
   */
  while (1) {
    clilen = sizeof(struct sockaddr_in);
    if ((connfd = accept(s_sock, (struct sockaddr *) &cliaddr,
	    &clilen)) == -1) {
      if (errno == EINTR) { /* signal handled before connection */
	DEBUG("accept: EINTR", errno);
	continue; /* ignore it */
      } else {
	DEBUG("accept", errno);
	continue; /* ignore anything */
      }
    }

    /* we have accepted connection from a client */
    if ((ret = pthread_create(&tid, NULL, do_work, (void *) connfd)) != 0) {
      DEBUG("pthread_create", ret);
      CLOSE(connfd); /* close connection */
      continue; /* go on */
    }
    pthread_detach(tid);
//    fprintf(stderr, "tid = %u\n", (unsigned int) tid);
  } /* while() */

#ifdef _WIN32
  WSACleanup();
#endif

  return 0;
}

void *do_work(void *arg)
{
  int connfd;

  /* client connection was sent as thread argument */
  connfd = (int) arg;

  /* pointer to socket is sent because parent process in exec_cmd() needs to
   * close the socket, otherwise a EPIPE is received
   */
  manage_client(connfd); /* handle client */

  /* shutdown write end of the socket [send FIN] */
  //shutdown(connfd, SHUT_WR); // server exits prematurely if client closes
  /* close connection */
  CLOSE(connfd);

  pthread_exit(NULL);

  return (NULL); // never reach here
}

void manage_client(int connfd)
{
//  fprintf(stderr, "<%s>\n", __func__);

  char buf[MAXLINE];
  ssize_t nread;
  //(void) connfd;


  /* get a line from client */
  nread = recvline(connfd, buf, sizeof(buf)-1);
  buf[nread] = 0;
  //fprintf(stderr, "<%s:%d> nread = %d\n", __FILE__, __LINE__, (int) nread);
  if (nread > 0) {	/* some data read */
    fix_path(buf, (size_t) nread); // convert '/' to '\' and vice versa
//    fprintf(stdout, "%s\n", buf);
    /* please see protocol.txt file for understanding the protocol used here */
    if (!strncmp(buf, "FUNC ", 5))
      process_function(connfd, buf+5);
    else if (!strncmp(buf, "EXEC ", 5))
      process_exec(connfd, buf+5);
  } else {
    DEBUG("recvline", errno);
  }
}


/* if you have any configuration files to reload runtime, you may write here */
void reload_config(int signo)
{
  (void) signo;
  DEBUG("reloading configuration...", 0);

  cmdmap_destroy();
  cmdmap_init();

  return;
}

/* signal handler */
void handle_signal(int signo)
{
  //(void) signo;
  fprintf(stderr, "<%d> caught, exiting...\n", signo);
  cmdmap_destroy();
  CLOSE(s_sock);
  _exit(signo);
}

#ifndef __WIN32__
/* handle broken pipe */
void pipe_signal(int signo)
{
  write(2, "***WARNING*** Caught a SIGPIPE signal\n", 38);
  TRACE_LINE();
  return;
}
#endif

#ifndef __WIN32__
/* this will be invoked when a child will exit */
void handle_sigchld(int signo)
{
  return;
}
#endif


/* please see protocol.txt for understanding the protocol used here */
int process_function(int connfd, const char *buf)
{
//  fprintf(stderr, "<%s>\n", __func__);
  int ret = 0;

  if (!strncmp(buf, "UP ", 3)) {
    ret = proc_upload(connfd, buf+3);
  } else if (!strncmp(buf, "DOWN ", 5)) {
    ret = proc_download(connfd, buf+5);
  } else if (!strncmp(buf, "MOVE ", 5)) {
    ret = proc_move(connfd, buf+5);
  } else if (!strncmp(buf, "NONE ", 5)) {
    ret = proc_none(connfd, buf+5);
  }

  return ret;
}

/* please see protocol.txt for understanding the protocol used here */
int process_exec(int connfd, const char *buf)
{
//  fprintf(stderr, "<%s>\n", __func__);

  int ret = 0;

  ret = proc_exec(connfd, buf);

  return ret;
}

/* convert / and \ accordingly so that any notation can be used by client */
void fix_path(char *path, size_t len)
{
  int i;

  for (i = 0; i < len; i++) {
    if (path[i] == INV_DIR_DELIM) path[i] = DIR_DELIM;
  }
}
