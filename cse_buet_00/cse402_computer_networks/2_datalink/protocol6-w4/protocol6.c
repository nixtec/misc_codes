/*
 * protocol6.c
 * protocol 6 (selective repeat) accepts frmes out of order but passes packets
 * to the network layer in order. associated with each outstanding frame is a
 * timer. when the timer expires, only that frame is retransmitted, not all
 * the outstanding frames
 *
 * structure assignments are valid in C
 */

/*
 * routines are implemented by Ayub <mrayub@gmail.com>
 * this program is supposed to work in all UN*X systems
 * with pthread support
 */
#include "protocol6.h"

#define PORT 7860

/* main program will execute this routine */
/* timer interval (microseconds) */
#define INTERVAL 500000		/* 500 ms */

/*
 * LOOK AT THIS
 */
/* NTICKS must be greater than NTICKS_ACK
 * otherwise the sender will timeout first and keep sending
 * packets to the receiver...
 */
#define NTICKS     100		/* frame timout time */
#define NTICKS_ACK 1		/* ack timeout time */

#define READ_BYTES 64		/* how many bytes to read in a single call */

#ifdef _PROTOCOL6_DEBUG_
#define PRINTF printf
#else
#define PRINTF NOOP
#endif

void int_handler(int signo);

void NOOP(const char *str, ...)
{
}

void *timer(void *arg);


static int senderror; /* whether we will introduce error in outgoint frames */
static int sockfd; /* physical layer transmission line (socket) */
static char ipaddr[INET_ADDRSTRLEN];

static int ackhead; /* ack timer */

struct timer_node {
  int ticks; /* number of ticks (intervals) */
  seq_nr n; /* frame sequence number */
  struct timer_node *next;
};
struct timer_node *thead;
struct timer_node *tail;

char nl_output_file[128]; /* output filename */
char nl_input_file[128]; /* input filename */

int network_to_fd;
int network_from_fd;

struct timerthread {
  pthread_mutex_t mutex;
} tthread = { PTHREAD_MUTEX_INITIALIZER };

pthread_mutex_t e_mutex = PTHREAD_MUTEX_INITIALIZER; /* mutex for raise_event */

/* global variables, mutex, conditional variables, etc */
/* all three threads will access this */
struct common { /* wait for event struct */
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  event_type event;
} s_common = {
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, nr_events
};

/* synchronize datalink and network layer */
/* at first network might not be locked */
struct nl_sync {
  pthread_mutex_t r_mutex;
  pthread_mutex_t w_mutex;
  pthread_cond_t r_cond; /* to_network_layer will use this */
  pthread_cond_t w_cond;
  packet from; /* network layer will fill this */
  packet to; /* datalink layer will fill this */
} s_nl = {
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER
};

/* synchronize datalink and physical layer */
struct ph_sync {
  pthread_mutex_t r_mutex;
  pthread_mutex_t w_mutex;
  pthread_mutex_t mutex; /* common mutex; should be locked before a read/write on socket */
  pthread_cond_t r_cond; /* to_physical_layer will use this */
  pthread_cond_t w_cond; /* condition variable for the physical_layer_writer */
  frame from; /* network layer will fill this */
  frame to; /* datalink layer will fill this */
} s_ph = {
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER
};

boolean no_nak = true;			/* no nak has been sent yet */
seq_nr oldest_frame = MAX_SEQ + 1;	/* initial value for the simulator */

/* these strings are for debugging */
char ftype[][5] = {
  "data",
  "ack",
  "nak"
};

char etype[][20] = {
  "frame_arrival",
  "cksum_err",
  "timeout",
  "network_layer_ready",
  "ack_timeout"
};

void disable_physical_layer(void);
void enable_physical_layer(void); /* let the physical layer know that we have received the data */
void disable_event(void); /* because dll may be busy in doing something and others must respect it */
void enable_event(void);

#define NTHREADS 6

int main(int argc, char **argv)
{
  pthread_t tid[NTHREADS];
  int ret;

  int server = 0; /* if this listens or connects */

  signal(SIGINT, int_handler);

  PRINTF("PREAMBLE/POSTAMBLE : %x\n", PREAMBLE);
  PRINTF("MAX_SEQ : %d\n", MAX_SEQ);
  PRINTF("NR_BUFS : %d\n", NR_BUFS);
  PRINTF("Control byte size : %lu\n", sizeof(control_byte));
  PRINTF("Frame size : %lu\n", sizeof(frame));
  PRINTF("Packet size : %lu\n", sizeof(packet));

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <server|client>\n", argv[0]);
    exit(0);
  }
  if (!strcmp(argv[1], "server"))
    server = 1;
  else {
    if (argc < 3) {
      strcpy(ipaddr, "127.0.0.1");
    }
    else {
      strncpy(ipaddr, argv[2], sizeof(ipaddr));
    }
    server = 0;
  }

  if (server) {
    strcpy(nl_output_file, "protocol6.out.server");
  }
  else {
    strcpy(nl_output_file, "protocol6.out.client");
  }

  PRINTF("Initializing connection\n");
  sockfd = init_connection(server);
  if (sockfd < 0) {
    fprintf(stderr, "init_connection failure\n");
    exit(1);
  }

  PRINTF("%s: creating threads...\n", __func__);
  ret = pthread_create(&tid[0], NULL, physical_layer_writer, NULL);
  if (ret != 0) {
    fprintf(stderr, "%s: pthread_create: %s\n", __func__, strerror(ret));
    exit(1);
  }
  ret = pthread_create(&tid[1], NULL, physical_layer_reader, NULL);
  if (ret != 0) {
    fprintf(stderr, "%s: pthread_create: %s\n", __func__, strerror(ret));
    exit(1);
  }

//  PRINTF("%s: initializing timer\n", __func__);
  ret = pthread_create(&tid[2], NULL, timer, NULL);
  if (ret != 0) {
    fprintf(stderr, "%s: pthread_create: %s\n", __func__, strerror(ret));
    exit(1);
  }

  ret = pthread_create(&tid[3], NULL, datalink_layer, NULL);
  if (ret != 0) {
    fprintf(stderr, "%s: pthread_create: %s\n", __func__, strerror(ret));
    exit(1);
  }

  ret = pthread_create(&tid[4], NULL, network_layer_reader, NULL);
  if (ret != 0) {
    fprintf(stderr, "%s: pthread_create: %s\n", __func__, strerror(ret));
    exit(1);
  }
  ret = pthread_create(&tid[5], NULL, network_layer_writer, NULL);
  if (ret != 0) {
    fprintf(stderr, "%s: pthread_create: %s\n", __func__, strerror(ret));
    exit(1);
  }

//  pthread_mutex_lock(&s_nl.w_mutex); /* initially locked */

  /*
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
  pthread_join(tid[2], NULL);
  pthread_join(tid[3], NULL);
  pthread_join(tid[4], NULL);
  pthread_join(tid[5], NULL);
  */
  for (ret = 0; ret < NTHREADS; ret++) {
    pthread_detach(tid[ret]);
  }

  int signo;
  sigset_t newset;

  sigemptyset(&newset);
  sigaddset(&newset, SIGQUIT);
  pthread_sigmask(SIG_BLOCK, &newset, NULL); /* block signals */
  while (1) {
    sigwait(&newset, &signo);
    if (signo == SIGQUIT) {
      fprintf(stderr, "main: toggle error\n");
      senderror ^= 1; /* toggle senderror */
    }
  }

  return 0;
}

int init_connection(int server)
{

  int ret;
  int listenfd, fd;
  struct sockaddr_in addr;

  fd = 0;

  listenfd = fd = socket(PF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);

  if (server) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(listenfd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
      fprintf(stderr, "%s: bind: %s\n", __func__, strerror(errno));
      return ret;
    }

    PRINTF("listening ...\n");
    listen(listenfd, 5); /* queue size is 5 */
    PRINTF("Waiting for connection\n");
    fd = accept(listenfd, (struct sockaddr *) NULL, NULL); /* don't care about clients */
    PRINTF("Connection accepted\n");
  }
  else {
    inet_pton(AF_INET, ipaddr, &addr.sin_addr);
    PRINTF("Connecting to %s ...", ipaddr);
    fflush(stdout);
    ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
      PRINTF("[FAILED]\n");
      fd = ret;
    }
    else {
      PRINTF("[  OK  ]\n");
    }
  }

  return fd;
}

static boolean between(seq_nr a, seq_nr b, seq_nr c)
{
  PRINTF("<%s>\n", __func__);
  /* return true if a <= b < c circularly; false otherwise */
  return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
}

static void send_frame(frame_kind fk, seq_nr frame_nr,
   seq_nr frame_expected, packet *buffer)
{

  PRINTF("<%s> <%s>\n", __func__, ftype[fk]);

  /* construct and send a data, ack, or nak frame */
  frame s;		/* scratch variable */

//  s.preamble = PREAMBLE;
//  s.postamble = POSTAMBLE;

//  s.kind = fk;				/* kind == data, ack, or nak */
  s.ctrl.type = fk;				/* type == data, ack, or nak */
  if (fk == data)
    s.info = buffer[frame_nr % NR_BUFS];
  s.ctrl.seq = frame_nr;		/* only meaningful for data frames */
  s.ctrl.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
  if (fk == nak)
    no_nak = false;

  to_physical_layer(&s);
  if (fk == data)
    start_timer(frame_nr % NR_BUFS);
  stop_ack_timer();			/* no need to separate ack frame */
}

void protocol6(void)
{
  PRINTF("<%s>\n", __func__);
  seq_nr ack_expected;			/* lower edge of sender's window */
  seq_nr next_frame_to_send;		/* upper edge of sender's window+1 */
  seq_nr frame_expected;		/* lower edge of receiver's window */
  seq_nr too_far;			/* upper edge of receiver's window+1 */
  int i;				/* index into buffer pool */
  frame r;				/* scratch variable */
  packet out_buf[NR_BUFS];		/* buffers for outbound stream */
  packet in_buf[NR_BUFS];		/* buffers for inbound stream */
  boolean arrived[NR_BUFS];		/* inbound bit map */
  seq_nr nbuffered;			/* # of output buffers used */
  event_type event;

  pthread_mutex_lock(&s_nl.w_mutex); /* lock the network_layer_writer() */

  ack_expected = 0;			/* next ack expected on inbound */
  next_frame_to_send = 0;		/* next outgoing frame */
  frame_expected = 0;
  too_far = NR_BUFS;
  nbuffered = 0;			/* initially no packets are buffered */

  for (i = 0; i < NR_BUFS; i++)
    arrived[i] = false;

  pthread_mutex_unlock(&s_nl.w_mutex);		/* unlock the network_layer_writer() */

  while (true) {
    wait_for_event(&event);
    disable_event(); /* we will process data here, so no more event allowed */
    switch (event) {
      case network_layer_ready:		/* accept, save, and transmit */
	PRINTF("%s: %s\n", __func__, etype[event]);
	nbuffered += 1;
	from_network_layer(&out_buf[next_frame_to_send % NR_BUFS]);
	send_frame(data, next_frame_to_send, frame_expected, out_buf);
	inc(next_frame_to_send);
	break;
      case frame_arrival:
//	disable_physical_layer();
	/* not needed, physical layer will be disabled after each recv */
	PRINTF("%s: %s\n", __func__, etype[event]);
	/* get data put by physical layer */
	from_physical_layer(&r);
	/* physical layer can put new data now */
	enable_physical_layer();
	if (r.ctrl.type == data) {
	  PRINTF("%s: data frame\n", __func__);
	  if ((r.ctrl.seq != frame_expected) && no_nak) {
	    PRINTF("%s: but not the expected frame\n", __func__);
	    send_frame(nak, 0, frame_expected, out_buf);
	  }
	  else {
	    start_ack_timer();
	  }
	  if (between(frame_expected, r.ctrl.seq, too_far) &&
	      (arrived[r.ctrl.seq % NR_BUFS] == false)) {
	    PRINTF("%s: frames may be accepted in any order\n", __func__);
	    /* frames may be accepted in any order */
	    arrived[r.ctrl.seq % NR_BUFS] = true;
	    in_buf[r.ctrl.seq % NR_BUFS] = r.info; /* insert data into buffer */
	    while (arrived[frame_expected % NR_BUFS]) {
	      /* pass frames and advance window */
	      to_network_layer(&in_buf[frame_expected % NR_BUFS]);
	      no_nak = true;
	      arrived[frame_expected % NR_BUFS] = false;
	      inc(frame_expected); /* advance lower edge of receiver */
	      inc(too_far);
	      start_ack_timer(); /* to see if a separate ack is needed */
	    }
	  }
	}
	if ((r.ctrl.type == nak) && between(ack_expected,
	      (r.ctrl.ack + 1) % (MAX_SEQ + 1), next_frame_to_send)) {
	  PRINTF("%s: nak frame\n", __func__);
	  send_frame(data, (r.ctrl.ack + 1) % (MAX_SEQ + 1), frame_expected,
	      out_buf);
	}

	while (between(ack_expected, r.ctrl.ack, next_frame_to_send)) {
	  nbuffered -= 1; /* handle piggybacked ack */
	  stop_timer(ack_expected % NR_BUFS); /* frame arrived intact */
	  inc(ack_expected); /* advance lower edge of sender's window */
	}
	break;
      case cksum_err:
	PRINTF("%s: %s\n", __func__, etype[event]);
	if (no_nak)
	  send_frame(nak, 0, frame_expected, out_buf); /* damaged frame */
	break;
      case timeout:
	PRINTF("%s: %s\n", __func__, etype[event]);
	send_frame(data, oldest_frame, frame_expected, out_buf); /* timeout */
	break;
      case ack_timeout:
	PRINTF("%s: %s\n", __func__, etype[event]);
	send_frame(ack, 0, frame_expected, out_buf); /* ack timer expired */
	break;
      default:
	fprintf(stderr, "%s: Unknown event : %d\n", __func__, event);
	break;
    }
    enable_event();
    if (nbuffered < NR_BUFS)
      enable_network_layer();
    else
      disable_network_layer();
  }
}




/* low level codes should be optimized,
 * need not be portable to all OSes -- ayub :-)
 * but should be standardized
 */

/* implemented routines */

/* man 3 pthread_cond_wait
 * pthread_cond_wait() shall block on a condition variable. this should be
 * called with 'mutex' locked by the calling thread, otherwise undefined
 * behavior results
 */
void wait_for_event(event_type *event)
{
  PRINTF("<%s>\n", __func__);
  int ret = 0;

  ret = pthread_mutex_lock(&s_common.mutex); /* lock the mutex */
  if (ret != 0) {
    PRINTF("%s: pthread_mutex_lock: %s\n", __func__, strerror(ret));
    *event = s_common.event = nr_events + 1; /* unknown event */
    return; /* lock not acquired */
  }
  ret = pthread_cond_wait(&s_common.cond, &s_common.mutex);
  if (ret != 0) {
    PRINTF("%s: pthread_cond_wait: %s\n", __func__, strerror(ret));
    *event = s_common.event = nr_events + 2; /* unknown event */
    pthread_mutex_unlock(&s_common.mutex); /* unlock the locked mutex */
    return; /* lock not acquired */
  }
  /* conditional wait successful */
  /* somebody signaled me, assuming he/she filled the s_event.event */
  *event = s_common.event;
  pthread_mutex_unlock(&s_common.mutex); /* release the mutex held */
}

void from_network_layer(packet *p)
{
  PRINTF("<%s>\n", __func__);
  *p = s_nl.from;
}

void to_network_layer(packet *p)
{
  PRINTF("<%s>\n", __func__);
  s_nl.to = *p;
//  memcpy(&s_nl.to, p, sizeof(packet));
  pthread_cond_signal(&s_nl.r_cond); /* let the reader know this */
}

void from_physical_layer(frame *r)
{
  PRINTF("<%s>\n", __func__);
//  memcpy(r, &s_ph.from, sizeof(frame));
  *r = s_ph.from;
}

void to_physical_layer(frame *s)
{
  PRINTF("<%s>\n", __func__);
  s_ph.to = *s;
//  memcpy(&s_ph.to, s, sizeof(frame));
  pthread_cond_signal(&s_ph.r_cond); /* let the reader know this */
}


void enable_physical_layer(void)
{
  PRINTF("<%s>\n", __func__);

//  pthread_mutex_unlock(&s_ph.w_mutex);
  pthread_cond_signal(&s_ph.w_cond);
}

/*
void disable_physical_layer(void)
{
  int ret;
  PRINTF("<%s>\n", __func__);

  if ((ret = pthread_mutex_trylock(&s_ph.w_mutex)) != 0) {
    PRINTF("%s: pthread_mutex_trylock: %s\n", __func__, strerror(ret));
    return;
  }
}
*/

void enable_network_layer(void)
{
  PRINTF("<%s>\n", __func__);

//  pthread_mutex_unlock(&s_nl.w_mutex);
  pthread_cond_signal(&s_nl.w_cond);
}

void disable_network_layer(void)
{
//  int ret;
  PRINTF("<%s>\n", __func__);

  /*
//  if ((ret = pthread_mutex_trylock(&s_nl.w_mutex)) != 0) {
  if ((ret = pthread_mutex_lock(&s_nl.w_mutex)) != 0) {
    PRINTF("%s: pthread_mutex_trylock: %s\n", __func__, strerror(ret));
  }
  */
}

void disable_event(void)
{
  int ret;
  PRINTF("<%s>\n", __func__);
//  ret = pthread_mutex_trylock(&e_mutex);
  ret = pthread_mutex_lock(&e_mutex);
  if (ret != 0) {
    PRINTF("%s: %s\n", __func__, strerror(ret));
  }
}

void enable_event(void)
{
  int ret;
  PRINTF("<%s>\n", __func__);
  ret = pthread_mutex_unlock(&e_mutex);
}

#ifdef ____NO____
typedef void sigfunc_rt(int, siginfo_t *, void *);
sigfunc_rt *signal_rt(int signo, sigfunc_rt *func)
{
  struct sigaction act, oact;

  act.sa_sigaction = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO; /* must specify this for realtime */
  if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT; /* sunos 4.x */
#endif
  }
  else {
#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART; /* svr4, 4.4 bsd */
#endif
  }
  if (sigaction(signo, &act, &oact) < 0)
    return ((sigfunc_rt *) SIG_ERR);
  return (oact.sa_sigaction);
}

static void
sig_rt(int signo, siginfo_t *info, void *context)
{
  PRINTF("%s: received signal #%d, code = %d, ival = %d\n",
      __func__, signo, info->si_code, info->si_value.sival_int);
}
#endif /* !____NO____ */

void init_timer(void)
{
  PRINTF("<%s>\n", __func__);
  int ret;
  struct itimerval value;
  struct timer_node *temp;

  int signo;
  sigset_t newset;

  sigemptyset(&newset);
  sigaddset(&newset, SIGALRM);
  pthread_sigmask(SIG_BLOCK, &newset, NULL); /* block signals */

  /* now establish signal handler with SA_SIGINFO set */
//  signal_rt(SIGALRM, sig_rt);

//  pthread_sigmask(SIG_UNBLOCK, &newset, NULL); /* unblock signals */

  memset(&value, 0, sizeof(value));

//  signal(SIGALRM, alarm_handler);

  value.it_interval.tv_usec = INTERVAL;
//  value.it_interval.tv_sec = 0;
  value.it_value.tv_usec = INTERVAL;
//  value.it_value.tv_sec = 0;

  ret = setitimer(ITIMER_REAL, &value, NULL);
  if (ret < 0) {
    PRINTF("%s: setitimer: %s\n", __func__, strerror(errno));
    return;
  }

  thead = tail = (struct timer_node *) NULL;

  while (1) {
    sigwait(&newset, &signo);
    if (signo == SIGALRM) {
      pthread_mutex_lock(&tthread.mutex);
      if (thead) { /* first node */
	thead->ticks = thead->ticks - 1;
	if (thead->ticks == 0) { /* expired */
	  temp = thead->next;
	  free(thead);
	  thead = temp;
	  if (thead == NULL) {
	    tail = NULL;
	  }
	  raise_event(timeout);
	}
      }
      if (ackhead) {
	ackhead -= 1;
	if (ackhead <= 0) {
	  ackhead = 0;
	  raise_event(ack_timeout);
	}
      }
      pthread_mutex_unlock(&tthread.mutex);
    }
  }
}

void start_timer(seq_nr k)
{
  PRINTF("<%s> entered\n", __func__);
  struct timer_node *temp;

  pthread_mutex_lock(&tthread.mutex);
  PRINTF("<%s> lock acquired\n", __func__);

  temp = thead;
  while (temp) {
    if (temp->n == k) {
      temp->ticks = NTICKS; /* reinitialize */
      break;
    }
    temp = temp->next;
  }
  if (temp) /* found timer and set up */
    goto cleanup;

  PRINTF("new node\n");
  temp = (struct timer_node *) malloc(sizeof(struct timer_node));
  if (thead == NULL) {
    thead = temp;
    tail = thead;
  }
  else { /* head not null */
    tail->next = temp;
    tail = temp;
  }
  temp->ticks = NTICKS;
  temp->n = k;
  temp->next = NULL;

cleanup:
  pthread_mutex_unlock(&tthread.mutex);
  PRINTF("<%s> lock released\n", __func__);
  PRINTF("<%s> left\n", __func__);
}

void stop_timer(seq_nr k)
{
  PRINTF("<%s> entered\n", __func__);
  struct timer_node *temp;
  struct timer_node *temp1;

  pthread_mutex_lock(&tthread.mutex);
  PRINTF("<%s> lock acquired\n", __func__);

  temp1 = temp = thead;
  while (temp) {
    if (temp->n == k) { /* remove the node */
//      temp1 = temp->next;
      if (thead == temp) { /* this is the head node */
	thead = temp->next;
      }
      if (!temp->next && (thead == NULL)) { /* this was the only node */
	tail = NULL;
      }
      else { /* this is the last node and thead not null */
	tail = temp1;
	tail->next = NULL;
      }
      free(temp);
      break;
    }
    temp1 = temp;
    temp = temp->next;
  }

  pthread_mutex_unlock(&tthread.mutex);
  PRINTF("<%s> lock released\n", __func__);
  PRINTF("<%s> left\n", __func__);
}

void start_ack_timer(void)
{
  PRINTF("<%s> entered\n", __func__);
  int ret;

  ret = pthread_mutex_lock(&tthread.mutex);
  ackhead = NTICKS_ACK;
  pthread_mutex_unlock(&tthread.mutex);
  PRINTF("<%s> entered\n", __func__);
}

void stop_ack_timer(void)
{
  PRINTF("<%s> entered\n", __func__);
  int ret;

  ret = pthread_mutex_lock(&tthread.mutex);
  ackhead = 0;
  pthread_mutex_unlock(&tthread.mutex);
  PRINTF("<%s> left\n", __func__);
}

void raise_event(event_type event)
{
  static int count[nr_events];
  int ret;

  ret = pthread_mutex_lock(&e_mutex);

  count[event]++;
  PRINTF("<%s> <%s> [%d]\n", __func__, etype[event], count[event]);
  s_common.event = event;
  pthread_cond_signal(&s_common.cond);

  pthread_mutex_unlock(&e_mutex);
}


/* handle timer interrupt */
void alarm_handler(int signo)
{
//  PRINTF("<%s>\n", __func__);
  return;
}

void int_handler(int signo)
{
  fprintf(stderr, "Interrupted, exiting\n");
  close(sockfd);
  exit(signo);
}


void *datalink_layer(void *arg)
{
  PRINTF("<%s>\n", __func__);
  protocol6();
  pthread_exit(NULL);
}

/* writer thread: read from input file and send data to datalink layer */
void *network_layer_writer(void *arg)
{
  PRINTF("<%s>\n", __func__);
  int ret;

  /* main function will lock s_nl.r_mutex and will unlock
   * it when a filename is supplied by user
   */
  /*
   * nl_output_file is predefined
   * */

  /*
  network_to_fd = open(nl_output_file, O_WRONLY);
  if (network_to_fd == -1) {
    perror("open: nl_output_file");
    pthread_exit(NULL);
  }
  */


  while (1) {
    PRINTF("%s: need another input file\n", __func__);
//    pthread_mutex_lock(&s_nl.w_mutex); /* initially locked by protocol6() */
    printf("Enter input filename : ");
    scanf("%s", nl_input_file);
    if (strlen(nl_input_file) == 0) {
      continue;
    }
//    pthread_mutex_unlock(&s_nl.w_mutex);
#ifdef ____NO____
    ret = pthread_mutex_lock(&s_nl.w_mutex); /* lock the mutex */
    if (ret != 0) {
      PRINTF("%s: pthread_mutex_lock: %s\n", __func__, strerror(ret));
      continue;
    }
    ret = pthread_cond_wait(&s_nl.w_cond, &s_nl.w_mutex); /* waiting for data to arrive */
    if (ret != 0) {
      PRINTF("%s: pthread_cond_wait: %s\n", __func__, strerror(ret));
    }
    pthread_mutex_unlock(&s_nl.w_mutex);
#endif /* !____NO____ */

    network_from_fd = open(nl_input_file, O_RDONLY);
    if (network_from_fd == -1) {
      PRINTF("%s: open: %s\n", __func__, strerror(errno));
      continue;
    }

    while (1) {
      ret = pthread_mutex_lock(&s_nl.w_mutex); /* disable_network_layer will block this */
      if (ret != 0) {
	PRINTF("%s: pthread_mutex_lock: %s\n", __func__, strerror(ret));
	continue;
      }
      ret = read(network_from_fd, s_nl.from.data, READ_BYTES);
      if (ret <= 0) {
	PRINTF("%s: read: %s\n", __func__, strerror(errno));
	pthread_mutex_unlock(&s_nl.w_mutex);
	close(network_from_fd);
	break;
      }
      PRINTF("%s: %d bytes read from %s\n", __func__, ret, nl_input_file);
      s_nl.from.len = ret;
//      pthread_mutex_unlock(&s_nl.w_mutex); /* let the protocol6 process data */
      /*
       * if network layer is too fast, then network_layer_ready event may be
       * lost */
      /* that's why we might need to sleep here */
//      usleep(1000); /* sleep 1 ms */
      raise_event(network_layer_ready);

//      ret = pthread_mutex_lock(&s_nl.w_mutex); /* disable_network_layer will block this */
//      if (ret != 0) {
//	PRINTF("%s: pthread_mutex_lock: %s\n", __func__, strerror(ret));
//	continue;
//      }

      PRINTF("%s: waiting for signal from protocol6\n", __func__);
      ret = pthread_cond_wait(&s_nl.w_cond, &s_nl.w_mutex); /* waiting for data to arrive */
      if (ret != 0) {
	PRINTF("%s: pthread_cond_wait: %s\n", __func__, strerror(ret));
      }
      PRINTF("%s: signal received from protocol6\n", __func__);
      pthread_mutex_unlock(&s_nl.w_mutex);
    }
    close(network_from_fd); /* done sending */
  }

  pthread_exit(NULL);

}

/* reader thread: gets data from datalink layer */
void *network_layer_reader(void *arg)
{
  PRINTF("<%s>\n", __func__);
  int ret;

  /* main function will lock s_nl.r_mutex and will unlock
   * it when a filename is supplied by user
   */
  /*
   * nl_output_file is predefined
   * */

  network_to_fd = open(nl_output_file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (network_to_fd == -1) {
    PRINTF("%s: open: %s\n", __func__, strerror(errno));
    pthread_exit(NULL);
  }

  while (1) {
    ret = pthread_mutex_lock(&s_nl.r_mutex); /* lock the mutex */
    if (ret != 0) {
      PRINTF("%s: pthread_mutex_lock: %s\n", __func__, strerror(ret));
      continue;
    }
    ret = pthread_cond_wait(&s_nl.r_cond, &s_nl.r_mutex); /* waiting for data to arrive */
    if (ret != 0) {
      PRINTF("%s: pthread_cond_wait: %s\n", __func__, strerror(ret));
    }
    ret = write(network_to_fd, s_nl.to.data, s_nl.to.len);
    if (ret <= 0) {
      PRINTF("%s: write: %s\n", __func__, strerror(errno));
    }
    pthread_mutex_unlock(&s_nl.r_mutex);
  }
  close(network_to_fd); /* done sending */

  pthread_exit(NULL);

}

/* writer thread: read from socket and send data to datalink layer */
void *physical_layer_writer(void *arg)
{
  PRINTF("<%s>\n", __func__);
  int i;
  int ret;
  uint8_t src[MAX_PKT+MAX_PKT/5+1+2];
  uint8_t dest[MAX_PKT+1+2];

  int src_cur; /* current location of 'src' */
  uint8_t buf[READ_BYTES+READ_BYTES/5+2+1+2+1]; /* data+stuffed+(pre+post)+ctrl+crc+padding */
  int pre_found, post_found;

  uint16_t crc, crcval;

  pre_found = post_found = 0;

  src_cur = 0;

  while (1) {
    while (1) {
      PRINTF("%s: Waiting for receiving data from socket\n", __func__);
      /* socket is blocking by default */
//      pthread_mutex_lock(&s_ph.mutex);
//      ret = recv(sockfd, buf, READ_BYTES+2+1+2, 0);
      ret = recv(sockfd, buf, sizeof(buf), 0);
      if (ret <= 0) {
	PRINTF("%s: recv: %s; Exiting...\n", __func__, strerror(errno));
//	pthread_mutex_unlock(&s_ph.mutex);
	pthread_exit(NULL); /* peer broken/closed */
	break;
      }
//      pthread_mutex_unlock(&s_ph.mutex);

      PRINTF("%s: %d bytes received\n", __func__, ret);
      /* remove preamble and postamble and copy data to buffer */
      for (i = 0; i < ret; i++) {
	if (pre_found && post_found)
	  break;
	if (buf[i] == PREAMBLE) {
	  if (!pre_found) {
	    PRINTF("%s: preamble found\n", __func__);
	    pre_found = 1;
	  }
	  else {
	    PRINTF("%s: postamble found\n", __func__);
	    post_found = 1;
	  }
	  continue;
	}
	src[src_cur++] = buf[i];
      }
      if (!pre_found || !post_found) {
	continue;
      }
      else {
	pre_found = post_found = 0;
	ret = destuffing(src, src_cur, dest, sizeof(dest));
	PRINTF("%s: destuffed buffer size (%d)\n", __func__, ret);
	PRINTF("%s: Received Data\n+++|", __func__);
	fflush(stdout);
	write(1, dest, ret-2); /* last 2 bytes are crc */
	PRINTF("|+++\n");
	fflush(stdout);

	src_cur = 0; /* reinitialize */
	memcpy(&crc, &dest[ret-2], 2);
	crcval = compute_crc16(dest, ret-2);
	if (crc != crcval) {
	  raise_event(cksum_err);
	}
	else {
	  s_ph.from.info.len = ret-3; /* without {ctrl,crc} */
	  memcpy(&s_ph.from.ctrl, dest, 1); /* control byte */
	  memcpy(&s_ph.from.info.data, &dest[1], ret-2-1);
	  raise_event(frame_arrival);
	}
	src_cur = 0;
      }
      pthread_mutex_lock(&s_ph.w_mutex);
      PRINTF("%s: lock acquired\n", __func__);
      pthread_cond_wait(&s_ph.w_cond, &s_ph.w_mutex); /* datalink layer has processed incoming data */
      PRINTF("%s: signaled\n", __func__);
      pthread_mutex_unlock(&s_ph.w_mutex);
    }
//    pthread_mutex_unlock(&s_ph.mutex);
  }

  pthread_exit(NULL);

}

/* reader thread: gets data from datalink layer */
void *physical_layer_reader(void *arg)
{
  PRINTF("<%s>\n", __func__);
  int ret;
  uint8_t src[MAX_PKT+1+2]; /* ctrl+crc+payload */
  uint8_t buf[sizeof(src)+sizeof(src)/5+1];
  int len;
  uint16_t crc;
  int counter = 0; /* how many times called */
  int pos; /* position of error bit byte */
  int MASK = 0x0010;

  /* main function will lock s_nl.r_mutex and will unlock
   * it when a filename is supplied by user
   */
  /*
   * nl_output_file is predefined
   * */

  while (1) {
    counter++;
    PRINTF("%s: counter = %d\n", __func__, counter);
    ret = pthread_mutex_lock(&s_ph.r_mutex); /* lock the mutex */
    PRINTF("%s: Waiting for writing data to socket\n", __func__);
    ret = pthread_cond_wait(&s_ph.r_cond, &s_ph.r_mutex); /* waiting for data to arrive */
    if (ret != 0) {
      PRINTF("%s: pthread_cond_wait: %s\n", __func__, strerror(ret));
    }
    pthread_mutex_unlock(&s_ph.r_mutex);

    PRINTF("%s: frame content:\n", __func__);
    len = 0;
    memcpy(&src[len], &s_ph.to.ctrl, sizeof(s_ph.to.ctrl));
    len += sizeof(s_ph.to.ctrl);
    if (s_ph.to.ctrl.type == data) {
      memcpy(&src[len], s_ph.to.info.data, s_ph.to.info.len);
      len += s_ph.to.info.len;
    }
    PRINTF("%s: Data to send\n+++|", __func__);
    fflush(stdout);
    write(1, src, len);
    PRINTF("|++\n");
    fflush(stdout);
    crc = compute_crc16(src, len);
    PRINTF("%s: CRC = %d\n", __func__, crc);
    memcpy(&src[len], &crc, 2); /* crc */
    len += 2;

    buf[0] = PREAMBLE;
    ret = stuffing(src, len, &buf[1], sizeof(buf)-2); /* pre+post{amble} */
    PRINTF("%s: stuffed buffer size (%d)\n", __func__, ret);
    len = ret;
    buf[len+1] = POSTAMBLE;
    len += 2; /* {pre+post}amble */
    /* socket is blocking by default */
//    pthread_mutex_lock(&s_ph.mutex); /* lock the socket */
    if (senderror) {
      pos = (int) (rand() % len-2) + 1;
      buf[pos] = buf[pos] ^ MASK;
    }
//    ret = write(sockfd, buf, len);
    ret = writen(sockfd, buf, len);
    if (ret < len) {
      PRINTF("%s: write: %s; Exiting...\n", __func__, strerror(errno));
//      pthread_mutex_unlock(&s_ph.mutex);
      pthread_exit(NULL); /* peer broken/closed */
    }
    PRINTF("%s: %d bytes written to socket\n", __func__, ret);
//    pthread_mutex_unlock(&s_ph.mutex);
  }

  pthread_exit(NULL);

}

void *timer(void *arg)
{
  PRINTF("<%s>\n", __func__);
  init_timer();
  pthread_exit(NULL);
}
