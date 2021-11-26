#include "myping4.h"

struct proto proto_v4 = {
  proc_v4, send_v4, NULL, NULL, 0, IPPROTO_ICMP
};

int datalen = 56;

struct addrinfo *ai;

/* timeout or interval might be less than a second
 * but it's not necessary for ping
 * and so tv.tv_usec is always set 0 in this program
 */
#ifndef INTERVAL
#define INTERVAL 1	/* seconds */
#endif
#ifndef TIMEOUT
#define TIMEOUT  3	/* seconds */
#endif

int received = 0;

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
    exit(2);
  }

  fprintf(stderr, "INTERVAL=%d, TIMEOUT=%d\n", INTERVAL, TIMEOUT);

  pid = getpid();
  signal(SIGALRM, sig_alrm);
  signal(SIGINT, sig_int);

  ai = host_serv(argv[1], NULL, 0, 0);
  if (!ai) {
    printf("Hostname lookup failure\n");
    exit(1);
  }
  pr = &proto_v4;
  pr->sasend = (struct sockaddr_in *) ai->ai_addr;
  pr->sarecv = calloc(1, ai->ai_addrlen);
  pr->salen = ai->ai_addrlen;

  printf("PING %s (%s) : %d data bytes\n", ai->ai_canonname,
     inet_ntoa(pr->sasend->sin_addr), datalen);
  readloop();

  return 0;
}


void readloop(void)
{
  int size;
  socklen_t len;
  ssize_t n;
  struct timeval tval;
  int ret;


  fd_set rset;
  struct timeval tv;

  sockfd = socket(pr->sasend->sin_family, SOCK_RAW, pr->icmpproto);
  if (sockfd == -1) {
    fprintf(stderr, "%s: socket: %s\n", __func__, strerror(errno));
    exit(1);
  }
  setuid(getuid()); /* we need not be root from now */

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
      fprintf(stderr, "Request timedout\n");
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
    printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n",
	icmplen, inet_ntoa(pr->sarecv->sin_addr),
	icmp->icmp_seq, ip->ip_ttl, rtt);
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

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    return (struct addrinfo *) NULL;

  return res;
}

void sig_int(int signo)
{
  fprintf(stderr, "Interrupted. Freeing ai\n");
  freeaddrinfo(ai);
  close(sockfd);
  exit(0);
}
