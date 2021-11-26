/*
 * ping-gtk.c
 * ping program with gtk interface
 * GTK is thread-aware but NOT thread safe
 * that's why i have used mutex so that two threads may not clobber the
 * gtk runtime library
 */
#include "ping.h"
#include <gtk/gtk.h>

#define BUFSIZE 1500

#define TIMEOUT 3	/* request timeout */

void proc_v4(char *, ssize_t, struct timeval *);
void send_v4(void);
void readloop(void);
void sig_int(int signo); /* handle interrupt */
void sig_alrm(int signo);
void tv_sub(struct timeval *, struct timeval *);
unsigned short in_cksum(unsigned short *addr, int len);
void *do_ping_send(void *); /* sender thread */
void *do_ping_recv(void *); /* receiver thread */
void show_error(const char *, const char *func);
int ping_gtk(const char *hostname);

void append_log(const char *view, const char *tagname, const char *msg);
void clear_log(const char *view);
void toggle_sender(void);
void toggle_receiver(void);
void init_tagtable(void);
void init_window(GtkWidget *window);
void update_hostname(void);
void init_hostname(const char *hname);

static GtkTextTagTable *tagtable;

char recvbuf[BUFSIZE], sendbuf[BUFSIZE];

extern GtkWidget *window1; /* defined in main.c */

int datalen;
const char *host;
struct hostent *hent;
char hostbuf[80]; /* hostname */
int nsent;
int sockfd;
pid_t pid;

int newhost = 0; /* if a new host is to be pinged */

int datalen = 56;

static int disable_sender, disable_recver;

#ifdef _DEBUG_
#define SHOWFUNCNAME printf("<%s>\n", __func__)
#else
#define SHOWFUNCNAME printf("")
#endif

#include "support.h"
#include "callbacks.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct sockaddr_in sendaddr, recvaddr;

char send_msg[256]; /* message to be displayed */
char recv_msg[256]; /* message to be displayed */

void init_tagtable(void)
{
  GtkTextTag *tag;

  tagtable = gtk_text_tag_table_new();

  tag = gtk_text_tag_new("bold");
  g_object_set(G_OBJECT(tag), "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("italic");
  g_object_set(G_OBJECT(tag), "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("typewriter");
  g_object_set(G_OBJECT(tag), "family", "monospace", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("red");
  g_object_set(G_OBJECT(tag), "foreground", "#FF0000", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("red-background");
  g_object_set(G_OBJECT(tag), "background", "#FF0000", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("blue");
  g_object_set(G_OBJECT(tag), "foreground", "#0000FF", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("blue-background");
  g_object_set(G_OBJECT(tag), "background", "#0000FF", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("orange");
  g_object_set(G_OBJECT(tag), "foreground", "#FFA500", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("orange-background");
  g_object_set(G_OBJECT(tag), "background", "#FFA500", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("pink");
  g_object_set(G_OBJECT(tag), "foreground", "#FFC0CB", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("pink-background");
  g_object_set(G_OBJECT(tag), "background", "#FFC0CB", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("lavender");
  g_object_set(G_OBJECT(tag), "foreground", "#E6E6FA", NULL);
  gtk_text_tag_table_add(tagtable, tag);

  tag = gtk_text_tag_new("lavender-background");
  g_object_set(G_OBJECT(tag), "background", "#E6E6FA", NULL);
  gtk_text_tag_table_add(tagtable, tag);

}

void init_window(GtkWidget *window)
{
  GtkWidget *ptr;
  GtkTextBuffer *buffer;

  if (!tagtable)
    init_tagtable();

  ptr = lookup_widget(window, "textview1");
  buffer = gtk_text_buffer_new(tagtable);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(ptr), buffer);

  ptr = lookup_widget(window, "textview2");
  buffer = gtk_text_buffer_new(tagtable);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(ptr), buffer);

}

void init_hostname(const char *hname)
{
  if (!hname) {
    strcpy(hostbuf, "localhost");
  }
  else {
    strncpy(hostbuf, hname, sizeof(hostbuf));
  }

  hent = gethostbyname(hostbuf);
  if (!hent) {
    fprintf(stderr, "%s: gethostbyanme: %s\n", __func__, hstrerror(h_errno));
//    exit(1);
    return;
  }
  sendaddr.sin_family = PF_INET;
  memcpy(&sendaddr.sin_addr, hent->h_addr_list[0], hent->h_length);

  host = hent->h_name;
  printf("PING %s (%s) : %d data bytes\n", hent->h_name,
      inet_ntoa(sendaddr.sin_addr), datalen);

}

int ping_gtk(const char *hostname)
{
  int ret;
  pthread_t tid;

  init_window(window1);
  signal(SIGINT, sig_int);
  pid = getpid();

  init_hostname(hostname);

  ret = pthread_create(&tid, NULL, do_ping_recv, NULL);
  if (ret != 0) {
    errno = ret;
    show_error(__func__, "pthread_create");
    exit(2);
  }
  pthread_detach(tid);

  usleep(100000); /* 100ms; let the receiver initialize socket */

  ret = pthread_create(&tid, NULL, do_ping_send, NULL);
  if (ret != 0) {
    errno = ret;
    show_error(__func__, "pthread_create");
    exit(2);
  }
  pthread_detach(tid);

  return 0;
}

void *do_ping_recv(void *arg)
{
  int size;
  int ret;
  fd_set rset;
  struct timeval tv;
  struct timeval tval;
  int len;
  ssize_t n;

  size = 60 * 1024; /* document it */

  ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
  if (ret == -1) {
    show_error(__func__, "setsockopt");
    pthread_exit(NULL);
  }

  FD_ZERO(&rset);

  while (1) {
    if (disable_recver) {
      sleep(1);
      continue;
    }
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    FD_SET(sockfd, &rset);
    ret = select(sockfd+1, &rset, NULL, NULL, &tv);
    if (ret == -1) { /* error */
      show_error(__func__, "select");
      continue;
    }
    if (ret == 0) { /* timeout */
      sprintf(recv_msg, "Request timeout\n");
      append_log("textview2", "red", recv_msg);
    }
    else { /* something happened */
      if (FD_ISSET(sockfd, &rset)) { /* socket is readable */
	len = sizeof(recvaddr);
	n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
	    (struct sockaddr *) &recvaddr, &len);
	if (n == -1) {
	  if (errno == EINTR) { /* interrupted system call */
	  }
	  else {
	    show_error(__func__, "recvfrom");
	  }
	  continue;
	}
	gettimeofday(&tval, NULL);
	proc_v4(recvbuf, n, &tval);
      }
    }
  }

  pthread_exit(NULL);
}

void proc_v4(char *ptr, ssize_t len, struct timeval *tvrecv)
{
  int hlen1, icmplen;
  double rtt;
  struct ip *ip;
  struct icmp *icmp;
  struct timeval *tvsend;

  ip = (struct ip *) ptr;
  hlen1 = ip->ip_hl << 2;
  icmp = (struct icmp *) (ptr + hlen1);
  if ((icmplen = len - hlen1) < 8) {
    fprintf(stderr, "%s: icmplen (%d) < 8\n", __func__, icmplen);
    exit(3);
  }

  if (icmp->icmp_type == ICMP_ECHOREPLY) {
    if (icmp->icmp_id != pid) { /* not my reply */
      fprintf(stderr, "Not for me\n");
      return;
    }
    if (icmplen < 16) {
      fprintf(stderr, "%s: icmplen (%d) < 16\n", __func__, icmplen);
      exit(3);
    }

    tvsend = (struct timeval *) icmp->icmp_data;
    tv_sub(tvrecv, tvsend); /* count difference */
    rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0; /* miliseconds */

    sprintf(recv_msg, "%d bytes from %s (%s): seq=%u, ttl=%d, rtt=%.3f ms\n",
	icmplen, host, inet_ntoa(recvaddr.sin_addr),
       	icmp->icmp_seq, ip->ip_ttl, rtt);
    append_log("textview2", "blue", recv_msg);
  }
}

void show_error(const char *func, const char *errfunc)
{
  fprintf(stderr, "%s: %s: %s\n", func, errfunc, strerror(errno));
}

void tv_sub(struct timeval *out, struct timeval *in)
{
  if ((out->tv_usec -= in->tv_usec) < 0) {
    --out->tv_sec;
    out->tv_usec += 1000000;
  }
  out->tv_sec -= in->tv_sec;
}

void *do_ping_send(void *arg)
{
  if (sockfd == -1) { /* socket creation failed */
    pthread_exit(NULL);
  }

  while (1) {
    if (newhost) { /* new hostname to be used */
      usleep(100000);
      continue;
    }
    if (disable_sender) {
      sleep(1);
      continue;
    }
    send_v4();
    sleep(1);
  }

  pthread_exit(NULL);
}

void send_v4(void)
{
  int len;
  struct icmp *icmp;
  int ret;

  icmp = (struct icmp *) sendbuf;
  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_id = pid;
  icmp->icmp_seq = nsent++;
  gettimeofday((struct timeval *) icmp->icmp_data, NULL);

  len = 8 + datalen;
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = in_cksum((u_short *) icmp, len);

  sprintf(send_msg, "Sending ICMP ECHO request\n");
  append_log("textview1", "orange", send_msg);

  ret = sendto(sockfd, sendbuf, len, 0, (struct sockaddr *) &sendaddr, sizeof(sendaddr));
  if (ret == -1) {
    show_error(__func__, "sendto");
  }
}

unsigned short in_cksum(unsigned short *addr, int len)
{
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1) {
    *(unsigned char *) (&answer) = *(unsigned char *) w;
    sum += answer;
  }

  sum = (sum >> 16);
  sum += (sum >> 16);
  answer = ~sum;

  return answer;
}

void sig_int(int signo)
{
  fprintf(stderr, "Interrupted, exiting...\n");
  close(sockfd);
  exit(signo);
}

void sig_alrm(int signo)
{
  return;
}

/* append_log("textview1", "color", "hi there\n"); */
/* append_log("textview2", "color", "hi ayub\n"); */
void append_log(const char *view, const char *tagname, const char *msg)
{
  pthread_mutex_lock(&mutex);
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter iter;

  widget = lookup_widget(window1, view);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_end_iter(buffer, &iter);
  gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, msg, -1,
      tagname, NULL);
  pthread_mutex_unlock(&mutex);
}

void clear_log(const char *view)
{
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;

  pthread_mutex_lock(&mutex);
  widget = lookup_widget(window1, view);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_delete(buffer, &start, &end);
  pthread_mutex_unlock(&mutex);
}

void toggle_sender(void)
{
  disable_sender ^= 1;
}


void toggle_receiver(void)
{
  disable_recver ^= 1;
}

void update_hostname(void)
{
  newhost = 1; /* race condition is not handled */
  GtkWidget *ptr = lookup_widget(window1, "entry1");
  if (strlen(gtk_entry_get_text(GTK_ENTRY(ptr))) == 0) {
    init_hostname(NULL);
  }
  else {
    strncpy(hostbuf, gtk_entry_get_text(GTK_ENTRY(ptr)), sizeof(hostbuf));
    init_hostname(hostbuf);
  }
  newhost = 0; /* race condition is not handled */
}
