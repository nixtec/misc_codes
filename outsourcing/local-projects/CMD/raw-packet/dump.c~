/*
 * dump.c
 */
#include "common.h"

#define IP_HDRSIZ sizeof(struct ip)
#define UDP_HDRSIZ sizeof(struct udphdr)
#define TCP_HDRSIZ sizeof(struct udphdr)
#define IP_UDP_HDRSIZ (IP_HDRSIZ+UDP_HDRSIZ)
#define IP_TCP_HDRSIZ (IP_HDRSIZ+TCP_HDRSIZ)

#define MESG_BUFSIZE (1024+IP_UDP_HDRSIZ)

#define CLEAN(ptr) if (ptr) { free(ptr); ptr = NULL; }

static void handle_int(int signo);
static void dump_header(const char *header, int proto);
static void usage(const char *progname, const char *msg);

static int protocol;
static int raw_sock;

int main(int argc, char **argv)
{
  int ret;
  int n;

  char mesg[MESG_BUFSIZE];
  struct sockaddr_in servaddr;
  const int on = 1;

  struct ip *ip_ptr;
//  struct udphdr *udp_ptr;
//  struct tcphdr *tcp_ptr;

  socklen_t fromlen;
  socklen_t len;

  if (argc != 2) {
    usage(argv[0], "<tcp|udp|icmp>");
    exit(1);
  }

  if (!strcmp(argv[1], "tcp")) {
    protocol = IPPROTO_TCP;
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (raw_sock == -1) {
      perror("error:socket: RAW");
      exit(1);
    }
  } else if (!strcmp(argv[1], "udp")) {
    protocol = IPPROTO_UDP;
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_sock == -1) {
      perror("error:socket: RAW");
      exit(1);
    }
  } else if (!strcmp(argv[1], "icmp")) {
    protocol = IPPROTO_ICMP;
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (raw_sock == -1) {
      perror("error:socket: RAW");
      exit(1);
    }
  } else {
    protocol = -1;
    usage(argv[0], "<tcp|udp|icmp>");
    exit(1);
  }

  /* administrative works done... */
  setuid(getuid());

  ret = setsockopt(raw_sock, IPPROTO_IP, IP_HDRINCL, (void *) &on,
      sizeof(on));
  if (ret == -1) {
    perror("error:setsockopt");
    close(raw_sock);
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = PF_INET;
  inet_aton("192.168.0.1", &servaddr.sin_addr);
//  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//  servaddr.sin_port = htons(7860);
  /* port doesn't matter in binding a raw socket */
  /* since a raw socket is used to catch a protocol packets */

  /* bind to broadcast address so that we only get broadcast packets */
  ret = bind(raw_sock, (SA *) &servaddr, sizeof(servaddr));
  if (ret == -1) {
    perror("bind");
    close(raw_sock);
    exit(1);
  }

  signal(SIGINT, handle_int);
  signal(SIGHUP, handle_int);
#ifdef __DEBUG__
  signal(SIGQUIT, handle_int);
#else
  signal(SIGQUIT, handle_int);
#endif
  signal(SIGTERM, handle_int);

  /* for testing 'echo "hi" > /dev/udp/localhost/999' */
  fromlen = 0; /* we're getting ip header, so 'from address' not required */
  len = sizeof(struct sockaddr_in);
  while (1) {
    n = recvfrom(raw_sock, mesg, sizeof(mesg), 0,
       	(SA *) NULL, &fromlen);
    if (n <= 0) {
      perror("recvfrom");
      continue;
    }
//#ifdef __DEBUG__
    switch (protocol) {
      case IPPROTO_UDP:
	printf("+++Packet size %d [ip=%d,udphdr=%d,data=%d]\n", n,
	    IP_HDRSIZ, UDP_HDRSIZ,
	    n - IP_UDP_HDRSIZ);
	ip_ptr = (struct ip *) mesg;
	dump_header(mesg, IPPROTO_UDP);
	/*
	udp_ptr = (struct udphdr *) (mesg + IP_HDRSIZ);
	printf("   SRC %s:%d\n", inet_ntoa(ip_ptr->ip_src),
	    ntohs(udp_ptr->uh_sport));
	printf("   DST %s:%d\n", inet_ntoa(ip_ptr->ip_src),
	    ntohs(udp_ptr->uh_dport));
	    */
	break;
      default:
	fprintf(stderr, "Unknown protocol %x\n", protocol);
	break;
    }
//#endif
  }

  return 0;
}

static void handle_int(int signo)
{
//#ifdef __DEBUG__
  printf("Interrupted <%d>. Exiting...\n", signo);
//#endif
  close(raw_sock);
  exit(signo);
}

static void usage(const char *progname, const char *msg)
{
  printf("Usage: %s %s\n", progname, msg);
}

/* ************************* NULL AND VOID ***************** */




static void dump_header(const char *header, int proto)
{
  struct ip *iphdr;
  struct udphdr *udphdr;

  iphdr = (struct ip *) header;

  printf("\n**** IP HEADER INFO ***\n");
  printf("+++ip_hl\t: %d\n", iphdr->ip_hl);
  printf("+++ip_v\t\t: %d\n", iphdr->ip_v);
  printf("+++ip_tos\t: %x\n", iphdr->ip_tos);
  printf("+++ip_len\t: %d\n", ntohs(iphdr->ip_len));
  printf("+++ip_id\t: %d\n", ntohs(iphdr->ip_id));
  printf("+++ip_ttl\t: %d\n", iphdr->ip_ttl);
  printf("+++ip_p\t\t: %d\n", iphdr->ip_p);
  printf("+++ip_sum\t: %d\n", ntohs(iphdr->ip_sum));
  printf("+++ip_src\t: %s\n", inet_ntoa(iphdr->ip_src));
  printf("+++ip_dst\t: %s\n", inet_ntoa(iphdr->ip_dst));

  switch (proto) {
    case IPPROTO_UDP:
      udphdr = (struct udphdr *) (header + IP_HDRSIZ);
      printf("\n**** UDP HEADER INFO ***\n");
      printf("---uh_sport\t: %d\n", ntohs(udphdr->uh_sport));
      printf("---uh_dport\t: %d\n", ntohs(udphdr->uh_dport));
      printf("---uh_ulen\t: %d\n", ntohs(udphdr->uh_ulen));
      printf("---uh_sum\t: %d\n", ntohs(udphdr->uh_sum));
      break;
    default:
      break;
  }
}
