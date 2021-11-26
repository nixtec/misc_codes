#ifndef _MYPING4_GTK_H_
#define _MYPING4_GTK_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h> /* according to POSIX.xxx */
#include <errno.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <pthread.h>

#define BUFSIZE 1500


void proc_v4(char *, ssize_t, struct timeval *);
void send_v4(void);

void* readloop(void *);
void sig_alrm(int);
void sig_int(int signo);
void tv_sub(struct timeval *, struct timeval *);
struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype);
unsigned short in_cksum(unsigned short *addr, int len);

void init_tagtable(void);
void init_window(GtkWidget *window);
void clear_log(const char *view);
void append_log(const char *view, const char *tagname, const char *msg);
void init_hostname(const char *hostname);
int myping4_gtk(const char *hostname);


struct proto {
  void (*fproc) (char *, ssize_t, struct timeval *);
  void (*fsend) (void);
  struct sockaddr_in *sasend;
  struct sockaddr_in *sarecv;
  socklen_t salen;
  int icmpproto;
};

#endif /* !_MYPING4_GTK_H_ */
