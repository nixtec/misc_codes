/*
 * comm_man.h
 * @uthor: Gagandeep Jaswal
 * Communication Manager header file
 */


#ifndef _COMM_MAN_H_
#define _COMM_MAN_H_

#include <stdint.h>
#include "sec_man.h"


typedef enum {
  false, true
} bool;

extern int comm_man_init(const char *ip, const char *user, const char *pass);

/* socket domain */
#define PF_INET		0	/* IPv4 socket */
#define AF_INET		PF_INET
/* right now only PF_INET supported */
#define PF_INET6	1	/* IPv4 socket */
#define AF_INET6	PF_INET6

/* socket types */
#define SOCK_STREAM	0
#define SOCK_DGRAM	1

/* socket protocols */
#define IPPROTO_TCP	0

struct socket_struct;
typedef struct socket_struct socket_t;
struct conn_struct;
typedef struct conn_struct conn_t;

typedef uint16_t in_port_t;
typedef uint8_t sa_family_t;
typedef uint32_t socklen_t;

/* listening socket */
#define SOCK_LISTEN	0x04

/* struct sockaddr is really struct sockaddr_in in our comm manager */
struct sockaddr {
  in_port_t		sin_port;
  char			sin_addr[16]; /* for simplicity not using int */
};

struct socket_struct {
  int sock_id; /* socket ID (unique for a system) */
  int sockfd_cs; /* client to server fifo */
  int sockfd_sc; /* server to client fifo */
  char sockname_cs[256];
  char sockname_sc[256];
  struct sockaddr address;
  int type; /* SOCK_LISTEN or not */
  int status; /* to be set when shutdown() is called */
  conn_t *conn; /* after connect() or accept() it will be initialized */
  bool dirty; /* is the socket buffer dirty? */
};

/* active connection */
struct conn_struct {
  int conn_id;
  socket_t *local_sock; /* which local socket the connection is is holding */
  char local_addr[16];
  in_port_t local_port;
  char foreign_addr[16];
  in_port_t foreign_port;
};

/* communication manager */
struct comm_man_struct {
  char cmdbuf[256]; /* command buffer, to know which action to take place */
  char local_host_name[16]; /* ip of the host */
  char username[32];
  char password[16];
  /* maximum 256 sockets may be opened at a time */
#define MAX_SOCKETS 256
  socket_t *sock_table[MAX_SOCKETS];
  in_port_t free_port;
#define SERVER 0x00
#define CLIENT 0x01
  uint8_t c_or_s; /* is it client or server? */
};
typedef struct comm_man_struct comm_man_t;

extern int accept(int socket, struct sockaddr *address, socklen_t *address_len);
extern int bind(int socket, const struct sockaddr *address,
    socklen_t address_len);
extern int closesocket(int sockfd);
extern int connect(int socket, const struct sockaddr *address,
    socklen_t address_len);
extern int listen(int socket, int backlog);
extern int recv(int socket, void *buffer, size_t length, int flags);
extern int send(int socket, const void *buffer, size_t length, int flags);
extern int socket(int domain, int type, int protocol);
extern int shutdown(int socket, int how);
extern int server_handle_file_service(int sockfd);
extern int client_handle_file_service(int sockfd);

#endif
