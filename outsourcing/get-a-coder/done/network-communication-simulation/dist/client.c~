/*
 * client.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "comm_man.h"

int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr addr;
  char selfip[16];
  char serverip[16];
  char username[32];
  char password[16];

  fprintf(stderr, "Please identify your own IP Address (x.x.x.x): ");
  scanf("%s", selfip);

  fprintf(stderr, "Please identify Server IP Address (x.x.x.x): ");
  scanf("%s", serverip);

  fprintf(stderr, "Please enter your username (max length: 31) ");
  scanf("%s", username);

  fprintf(stderr, "Please enter your password (max length: 15) ");
  scanf("%s", password);

  if (comm_man_init(selfip, username, password) == -1) {
    exit(1);
  }

  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    exit(1);

  strcpy(addr.sin_addr, serverip);
  addr.sin_port = 21; /* ftp service */
  connect(sockfd, &addr, sizeof(addr));
  client_handle_file_service(sockfd);
  closesocket(sockfd);

  return 0;
}
