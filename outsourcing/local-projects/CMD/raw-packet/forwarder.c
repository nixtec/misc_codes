/*
 * forwarder.c
 * cisco ip-helper simulation that forwards only UDP BROADCASTS
 * actually this type of program is necessary for supporting softwares
 * that broadcasts on LAN and if we want to run this software in WAN
 * environment where LAN broadcasts don't reach
 *
 * you can do something like 'iptables' do here in this program since we are
 * getting the IP Header
 *
 * declaring a variable static just ensures that the memory is allocated only
 * once and remains in next calls. Intel manual also says to use static
 * variables whenever possible.
 */
#include "config.h"
#include "common.h"

#define IP_HDRSIZ sizeof(struct ip)
#define UDP_HDRSIZ sizeof(struct udphdr)
#define IP_UDP_HDRSIZ (IP_HDRSIZ+UDP_HDRSIZ)

#define MESG_BUFSIZE (1024+IP_UDP_HDRSIZ)

#define CLEAN(ptr) if (ptr) { free(ptr); ptr = NULL; }

static void handle_int(int signo);
static void handle_hup(int signo);
void dump_header(const char *header);
void usage(const char *progname, const char *msg);
static void update_addr_list(void);
static void update_port_list(void);


static struct in_addr bindaddr; /* I won't forward to my binding address */


static int raw_sock;
static struct sockaddr_in *g_addrs; /* global address list to forward */
static int num_addrs; /* number of address to forward to */
static u_short *g_ports;
static int num_ports; /* number of ports to forward */


int main(int argc, char **argv)
{
  int ret;
  int n;
  int i, j;

  struct sockaddr_in *temp_addr;
  u_short *temp_port;

  int forward_port_ok;

  char mesg[MESG_BUFSIZE];
  struct sockaddr_in servaddr;
  const int on = 1;

  struct ip *ip_ptr;
  struct udphdr *udp_ptr;

  socklen_t fromlen;
  socklen_t len;

  if (argc != 2) { /* broadcast address not supplied */
    usage(argv[0], "<broadcast-address>");
    exit(1);
  }

  /* I want all udp packets to be passed to me */
  raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (raw_sock == -1) {
    perror("error:socket: RAW");
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

  /* allow sending broadcasts */
  ret = setsockopt(raw_sock, SOL_SOCKET, SO_BROADCAST, (void *) &on,
      sizeof(on));
  if (ret == -1) {
    perror("error:setsockopt");
  }

  memset(&servaddr, 0x00, sizeof(servaddr));
  servaddr.sin_family = PF_INET;
  inet_aton(argv[1], &servaddr.sin_addr);
  bindaddr = servaddr.sin_addr;
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
  signal(SIGHUP, handle_hup);
#ifdef __DEBUG__
  signal(SIGQUIT, handle_hup);
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
#ifdef __DEBUG__
    printf("+++Packet size %d [ip=%d,udphdr=%d,data=%d]\n", n,
	IP_HDRSIZ, UDP_HDRSIZ,
	n - IP_UDP_HDRSIZ);
    ip_ptr = (struct ip *) mesg;
    udp_ptr = (struct udphdr *) (mesg + IP_HDRSIZ);
    printf("   SRC %s:%d\n", inet_ntoa(ip_ptr->ip_src),
	ntohs(udp_ptr->uh_sport));
    printf("   DST %s:%d\n", inet_ntoa(ip_ptr->ip_src),
	ntohs(udp_ptr->uh_dport));
#endif

    /* if no addresses, no need to check for ports */
    if (num_addrs == 0)
      continue;

    ip_ptr = (struct ip *) mesg;
    udp_ptr = (struct udphdr *) (mesg + IP_HDRSIZ);

    forward_port_ok = 0; /* by default don't forward ports */
    temp_port = g_ports;
    for (j = 0; j < num_ports; j++) {
      if (udp_ptr->uh_dport == *temp_port) {
	forward_port_ok = 1; /* OK, forward [sendto()] */
	break;
      }
      temp_port++;
    }
    if (!forward_port_ok) /* port doesn't match. Give up */
      continue;

//#ifdef __NODELAY__
    /* dont use __NODELAY__ */
    /* disable nagle algorithm */
//    ip_ptr->ip_tos = (ip_ptr->ip_tos | IPTOS_LOWDELAY);
//#endif

    /* now forward to given addresses */
    temp_addr = g_addrs;
    for (i = 0; i < num_addrs; i++) {
      temp_addr->sin_port = udp_ptr->uh_dport; /* to-be-forwarded port */
      ip_ptr->ip_dst = temp_addr->sin_addr; /* to-be-forwarded address */
      /* packet building complete, now send it to interested addresses */

#ifdef __DEBUG__
      printf("^^^Forwarding to %s:%d", inet_ntoa(temp_addr->sin_addr),
	  ntohs(temp_addr->sin_port));
#endif
      ret = sendto(raw_sock, mesg, n, 0, (SA *) temp_addr, len);
      if (ret < n) {
	perror("sendto");
      }
#ifdef __DEBUG__
      printf("\t[%d/%d]\n", ret, n);
#endif
      temp_addr++;
    }
  }

  return 0;
}

void handle_hup(int signo)
{
  update_addr_list();
  update_port_list();
  return;
}

void handle_int(int signo)
{
#ifdef __DEBUG__
  printf("Interrupted <%d>. Exiting...\n", signo);
#endif
  close(raw_sock);
  exit(signo);
}

void usage(const char *progname, const char *msg)
{
  printf("Usage: %s %s\n", progname, msg);
}

static void update_addr_list(void)
{
  static FILE *fp;
  static char straddr[20]; /* ipv4 address [dotted quad] */
  static char buf[20];
  static int i;
  static struct sockaddr_in *temp;

  CLEAN(g_addrs);
  num_addrs = 0;

  fp = fopen(ADDRLIST, "r");
  if (!fp)
    return;

  if (!fgets(buf, sizeof(buf), fp)) { /* empty file */
    fclose(fp);
    return;
  }
  num_addrs = (int) strtol(buf, (char **) NULL, 10);
  if (num_addrs <= 0) {
    num_addrs = 0;
    fclose(fp);
    return;
  }

  temp = g_addrs = (struct sockaddr_in *) calloc(num_addrs,
      sizeof(struct sockaddr_in));
  i = 0;
  while (i < num_addrs) {
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%s", straddr);
    inet_aton(straddr, &(temp->sin_addr));
    if (temp->sin_addr.s_addr == bindaddr.s_addr) {
      printf("Skipping %s [oops! i'm bounded here]\n", straddr);
      num_addrs--; /* valid address decreased */
      continue;
    }
    temp->sin_family = AF_INET;
    /* port will be filled before calling sendto() */
    printf("%s\n", straddr);
    i++;
    temp++;
  }
  fclose(fp);

  fflush(stdout);
  fflush(stderr);
}

static void update_port_list(void)
{
  static FILE *fp;
  static char buf[10];
  static int i;
  static u_short *temp;

  CLEAN(g_ports);
  num_ports = 0;

  fp = fopen(PORTLIST, "r");
  if (!fp)
    return;

  if (!fgets(buf, sizeof(buf), fp)) {
    fclose(fp);
    return;
  }
  num_ports = (int) strtol(buf, (char **) NULL, 10);
  if (num_ports <= 0) {
    num_ports = 0;
    fclose(fp);
    return;
  }

  temp = g_ports = (u_short *) calloc(num_ports, sizeof(u_short));
  for (i = 0; i < num_ports; i++) {
    fgets(buf, sizeof(buf), fp);
    printf("%s", buf);
    *temp = htons((u_short) strtol(buf, (char **) NULL, 10));
    temp++;
  }
  fclose(fp);

  fflush(stdout);
  fflush(stderr);
}




/* ************************* NULL AND VOID ***************** */




#ifdef ____NO____
static void dump_header(const char *header)
{
  struct ip *iphdr;
  struct udphdr *udphdr;

  iphdr = (struct ip *) header;
  udphdr = (struct udphdr *) (header + sizeof(struct ip));

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

  printf("\n**** UDP HEADER INFO ***\n");
  printf("---uh_sport\t: %d\n", ntohs(udphdr->uh_sport));
  printf("---uh_dport\t: %d\n", ntohs(udphdr->uh_dport));
  printf("---uh_ulen\t: %d\n", ntohs(udphdr->uh_ulen));
  printf("---uh_sum\t: %d\n", ntohs(udphdr->uh_sum));
}
#endif

