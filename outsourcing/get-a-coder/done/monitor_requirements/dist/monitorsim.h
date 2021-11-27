/*
 * monitorsim.h
 * header file for monitorsim.c
 */
#ifndef _MONITORSIM_H_
#define _MONITORSIM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* C doesn't have bool, creating own way */
typedef enum {
  false, true
} bool;

typedef void *element_t;
/* forward declaring node */
struct node_struct;
typedef struct node_struct node_t;
typedef node_t *position_t;
typedef node_t *list_t;

struct system_struct;
typedef struct system_struct system_t;
struct queue_struct;
typedef struct queue_struct queue_t;
struct cv_struct;
typedef struct cv_struct cv_t;
struct monitor_struct;
typedef struct monitor_struct monitor_t;
struct proc_struct;
typedef struct proc_struct proc_t;

/* node */
struct node_struct {
  element_t e;
  list_t next;
};


/* queue */
struct queue_struct {
  position_t front; /* head */
  position_t rear; /* tail */
};

struct statistics_struct {
  int nprocsmon; /* number of processes that used the monitor */
  int nprocscond; /* number of processes that spent time on condition queue */
  float avgtimecond; /* average time spent on condition queue */
  float avgtimemon; /* average time spent in monitor per process */
  time_t totaltimemon; /* total time elapsed by processes in monitor */
  time_t totaltimecond; /* total time elapsed by processes in monitor */
};
typedef struct statistics_struct statistics_t;

#define READER 0
#define WRITER 1
/* execution time */
#define READER_EXEC_TIME 1
#define WRITER_EXEC_TIME 2
struct proc_struct {
  int eip; /* instructions where the process was interrupted at */
  int type; /* READER/WRITER */
  int id; /* reader/writer sequence */
  int done; /* whether it finished doing its task, so that upon resume we don't do read/write again */
  time_t t1, t2;
};

struct monitor_struct {
  int read_locked; /* soft lock, multiple reader can read */
  int write_locked; /* exclusive lock */
  queue_t conditional; /* interrupted processes will be here */
  queue_t urgent; /* processes from conditional queue will enter here */
  queue_t running; /* running processes inside monitor (possibly multiple-reader or single writer) */
  statistics_t s;
  int res; /* the resource data */
  /* monitor methods */
  int (*wait)(monitor_t *mon, proc_t *proc); /* put the process on the queue */
  int (*signal)(monitor_t *mon, int on); /* signals a process in the queue to do work */
  int (*read)(monitor_t *mon, proc_t *proc); /* read the protected data */
  int (*write)(monitor_t *mon, proc_t *proc); /* write to the protected data */
  /* additional routines */
  int (*end)(monitor_t *mon, proc_t *proc);
  int (*interrupt)(monitor_t *mon, proc_t *proc); /* interrupted process */
  void (*statistics)(monitor_t *mon);
};

struct system_struct {
  monitor_t mon; /* monitor */
  queue_t ready; /* Ready Queue */
  bool (*schedule)(system_t *sys); /* schedule a process */
};

#endif
