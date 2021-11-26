#ifndef _PROTOCOL6_H_
#define _PROTOCOL6_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>		/* fixed sized int's */
#include <pthread.h>		/* thread routines */
#include <sys/types.h>
/* socket declarations */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* end socket declarations */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>		/* setitimer() */
#include <errno.h>
#include <unistd.h>

#include "crc16.h"		/* compute_crc16() */
#include "support.h"		/* stuffing(), destuffing() */

/* define number of bits used for sequence number */
#ifndef SEQ_BITS
#define SEQ_BITS 3		/* MAX_SEQ will be 2^SEQ_BITS - 1 */
#endif

#define PREAMBLE  0x7e		/* 01111110 */
#define POSTAMBLE 0x7e		/* 01111110 */

#ifndef ACK_BITS
#define ACK_BITS SEQ_BITS
#endif

#ifndef TYPE_BITS
#define TYPE_BITS 2		/* frame type: data, ack, nak */
#endif


#define MAX_SEQ ((1 << SEQ_BITS) - 1)	/* should be 2^n - 1 */
#define NR_BUFS ((MAX_SEQ + 1) / 2)

/* events that may arise */
typedef enum {
  frame_arrival,	/* 0 */
  cksum_err,		/* 1 */
  timeout,		/* 2 */
  network_layer_ready,	/* 3 */
  ack_timeout,		/* 4 */
  nr_events		/* number of events */
} event_type;

/* define MAX_PKT if not defined in command line */
#ifndef MAX_PKT
#define MAX_PKT 80		/* packet size in bytes */
#endif

typedef enum {
  false, /* 0 */
  true /* 1 */
} boolean;

typedef unsigned int seq_nr;	/* seq or ack numbers */
typedef struct {
  uint8_t data[MAX_PKT];
  uint8_t len; /* length of a packet */
} packet;			/* packet definition */

typedef enum {
  data,		/* 0: is this a data frame */
  ack,		/* 1: is this acknowledgement frame */
  nak,		/* 2: is this nak frame */
  nr_kind
} frame_kind;	/* frame_kind */

typedef struct {
  uint8_t seq  : SEQ_BITS; /* sequence number */
  uint8_t ack  : ACK_BITS; /* acknowledge number */
  uint8_t type : TYPE_BITS; /* frame type: data, ack, nak */
} control_byte;

/* here data link layer is not doing any fragmentation */
typedef struct {
//  uint8_t preamble;
//  frame_kind kind;		/* what kind of frame is it? */
  control_byte ctrl;
  packet info;			/* network layer packet (payload) */
//  uint16_t crc;
//  uint8_t postamble;
//  seq_nr seq;			/* sequence number */
//  seq_nr ack;			/* ack number */
} frame;

/* 'event_type' will be predefined in the header included before
 * including this header
 */

/* wait for an event to happen; return its type in event */
void wait_for_event(event_type *event);

/* fetch a packet from the network layer for transmission on the channel */
void from_network_layer(packet *p);

/* deliver information from an inbound frame to the network layer */
void to_network_layer(packet *p);

/* go get an inbound frame from the physical layer and copy it to r */
void from_physical_layer(frame *r);

/* pass the frame to the physical layer for transmission */
void to_physical_layer(frame *s);

/* start the clock running and enable the timeout event */
void start_timer(seq_nr k);

/* stop the clock and disable the timeout event */
void stop_timer(seq_nr k);

/* start an auxiliary timer and enable the ack_timeout event */
void start_ack_timer(void);

/* stop the auxiliary timer and disable the ack_timeout event */
void stop_ack_timer(void);

/* allow the network layer to cause a network_layer_ready event */
void enable_network_layer(void);

/* forbid the network layer from causing a network_layer_ready event */
void disable_network_layer(void);

/* macro inc is expanded in-line: increment k circularly */
#define inc(k) if (k < MAX_SEQ) k = k + 1; else k = 0

void raise_event(event_type event);

/* SIGALRM handler for timer */
void alarm_handler(int signo);

void *physical_layer(void *arg);
void *datalink_layer(void *arg);
void *network_layer(void *arg);
/* physical transmission (simulated with socket) */
int init_connection(int server);
/* physical transmission end */
void init_timer(void);

void *physical_layer_writer(void *arg);
void *physical_layer_reader(void *arg);
void *datalink_layer(void *arg);
void *network_layer_reader(void *arg);
void *network_layer_writer(void *arg);




#endif /* !_PROTOCOL6_H_ */
