/*
 * server.c
 * @uthor: Gagandeep Jaswal
 * Server Interface along with Sockets
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "comm_man.h"

#include <signal.h>		/* to handle Ctrl-C */

typedef void (*sig_t)(int);

void handle_sig(int signo);

/* global so that from signal handler we can access */
int listenfd; /* listening socket */
int connfd; /* connection to client */

int main(int argc, char **argv)
{
  struct sockaddr addr;
  struct sockaddr connaddr;
  socklen_t connaddr_len;
  char selfip[16];

  fprintf(stderr, "Please identify your own IP Address (x.x.x.x): ");
  scanf("%s", selfip);
  if (comm_man_init(selfip, NULL, NULL) == -1) {
    exit(1);
  }

  listenfd = socket(PF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
    exit(1);

  addr.sin_port = 21; /* ftp service */
  strcpy(addr.sin_addr, selfip);

  signal(SIGINT, handle_sig);

  bind(listenfd, &addr, sizeof(addr));
  listen(listenfd, 5);

  connfd = -1;
  while (1) {
    connfd = accept(listenfd, &connaddr, &connaddr_len);
    server_handle_file_service(connfd);
    closesocket(connfd);
    connfd = -1;
  }
  closesocket(listenfd);

  return 0;
}

void handle_sig(int signo)
{
  fprintf(stderr, "\nCaught signal <%d>, cleaning up\n", signo);
  if (listenfd >= 0) {
    closesocket(listenfd);
  }
  if (connfd >= 0) {
    closesocket(connfd);
  }
  fprintf(stderr, "Done, Exiting.\n");
  exit(0);
}
