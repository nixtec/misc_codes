#include "myping4-gtk.h"
#include "support.h"

char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];

char hname[80];
int lookup_host = 0;

int datalen;
int nsent;
pid_t pid;
int sockfd;

static GtkTextTagTable *tagtable;
extern GtkWidget *window1;

struct proto *pr;

struct proto proto_v4 = {
  proc_v4, send_v4, NULL, NULL, 0, IPPROTO_ICMP
};

int datalen = 56;

struct addrinfo *ai;

/* timeout or interval might be less than a second
 * but it's not necessary for ping
 * and so tv.tv_usec is always set 0 in this program
 */
#define INTERVAL 1	/* seconds */
#define TIMEOUT  3	/* seconds */

int received = 0;

char msg[256];

int myping4_gtk(const char *hostname)
{
  pthread_t tid;

  fprintf(stderr, "INTERVAL=%d, TIMEOUT=%d\n", INTERVAL, TIMEOUT);
  pid = getpid();
  signal(SIGALRM, sig_alrm);
  signal(SIGINT, sig_int);

  init_tagtable();
  init_window(window1);

  pthread_create(&tid, NULL, readloop, NULL);
  pthread_detach(tid);

  return 0;
}


void *readloop(void *arg)
{
  int size;
  socklen_t len;
  ssize_t n;
  struct timeval tval;
  int ret;


  fd_set rset;
  struct timeval tv;


  size = 60 * 1024;

  ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
  if (ret == -1) {
    fprintf(stderr, "%s: setsockopt: %s\n", __func__, strerror(errno));
    exit(2);
  }
  sig_alrm(SIGALRM); /* send first packet */

  memset(&tv, 0, sizeof(tv));
  FD_ZERO(&rset);

  /*
   * linux kernel modified the tv upon return from a select()
   * after return, tv contains the time not slept
   * this technique is used to handle the timeout in this program
   * in a single thread of execution.
   * however, for this reason, this portion of code is Linux specific
   * to make your code portable, remove the
   * 'if (tv.tv_sec == 0 && tv.tv_usec == 0)'
   * line.                -AYUB
   */
  while (1) {
    if (lookup_host) {
      init_hostname(hname);
      lookup_host = 0;
    }
    if (tv.tv_sec == 0 && tv.tv_usec == 0) { /* linux specific */
      tv.tv_sec = TIMEOUT;
      received = 0;
    }
    FD_SET(sockfd, &rset);
    ret = select(sockfd+1, &rset, NULL, NULL, &tv);
    if (ret == -1) {
      if (errno == EINTR) { /* interrupted system call */
//	printf("select tv.tv_sec = %d\n", (int) tv.tv_sec);
//	printf("select tv.tv_usec = %d\n", (int) tv.tv_usec);
      }
    }
    else if (ret == 0 && !received) { /* timeout */
      sprintf(msg, "Request timedout\n");
      fprintf(stderr, msg);
      append_log("textview2", "red", msg);
      continue;
    }
    else {
      if (FD_ISSET(sockfd, &rset)) {
	len = pr->salen;
	n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
	    (struct sockaddr *) pr->sarecv, &len);
//	  printf("DATA %d bytes\n", n); /* this may be my own ECHO request (since there is no receiver address) */
	gettimeofday(&tval, NULL);
	(*pr->fproc) (recvbuf, n, &tval);
      }
    }
  }
}

void tv_sub(struct timeval *out, struct timeval *in)
{
  if ((out->tv_usec -= in->tv_usec) < 0) {
    --out->tv_sec;
    out->tv_usec += 1000000;
  }
  out->tv_sec -= in->tv_sec;
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
    if (icmp->icmp_id != pid) {
      fprintf(stderr, "ICMP ECHO reply; pid mismatch\n");
      return;
    }
    if (icmplen < 16) {
      fprintf(stderr, "%s: icmplen (%d) < 16\n", __func__, icmplen);
      exit(3);
    }

    tvsend = (struct timeval *) icmp->icmp_data;
    tv_sub(tvrecv, tvsend);
    rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

    received = 1;
    sprintf(msg, "%d bytes from %s (%s): seq=%u, ttl=%d, rtt=%.3f ms\n",
	icmplen, ai->ai_canonname, inet_ntoa(pr->sarecv->sin_addr),
	icmp->icmp_seq, ip->ip_ttl, rtt);
    append_log("textview2", "blue", msg);
  }
}

void sig_alrm(int signo)
{
  (*pr->fsend) ();

  alarm(INTERVAL);

  return;
}

void send_v4(void)
{
  int len;
  struct icmp *icmp;

  icmp = (struct icmp *) sendbuf;
  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_id = pid;
  icmp->icmp_seq = nsent++;
  gettimeofday((struct timeval *) icmp->icmp_data, NULL);

  len = 8 + datalen;
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = in_cksum((u_short *) icmp, len);

  sendto(sockfd, sendbuf, len, 0, (struct sockaddr *) pr->sasend, pr->salen);
}

unsigned short in_cksum(unsigned short *addr, int len)
{
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1) {
    *(unsigned char *) (&answer) = *(unsigned char*) w;
    sum += answer;
  }

  sum = (sum >> 16);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype)
{
  int n;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = family;
  hints.ai_socktype = socktype;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    return (struct addrinfo *) NULL;
  }

  return res;
}

void sig_int(int signo)
{
  fprintf(stderr, "Interrupted. Freeing ai\n");
  freeaddrinfo(ai);
  close(sockfd);
  exit(0);
}

void clear_log(const char *view)
{
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;

  widget = lookup_widget(window1, view);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_delete(buffer, &start, &end);
}

void append_log(const char *view, const char *tagname, const char *msg)
{
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter iter;

  widget = lookup_widget(window1, view);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_end_iter(buffer, &iter);
  gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, msg, -1,
      tagname, NULL);
}

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

  ptr = lookup_widget(window, "textview2");
  buffer = gtk_text_buffer_new(tagtable);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(ptr), buffer);

}

void init_hostname(const char *hostname)
{
  /* race condition may arise */
  struct addrinfo *tempai;
  tempai = host_serv(hostname, NULL, 0, 0);
  if (!tempai) {
    if (window1) {
      sprintf(msg, "Hostname lookup failure\n");
      append_log("textview2", "orange", msg);
    }
    return;
  }
  else {
    if (ai) {
      freeaddrinfo(ai);
    }
    ai = tempai;
    pr = &proto_v4;
    pr->sasend = (struct sockaddr_in *) ai->ai_addr;
    pr->sarecv = calloc(1, ai->ai_addrlen);
    pr->salen = ai->ai_addrlen;
  }
}
