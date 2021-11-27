/*
 * dsfuncs.h
 * functions and type declarations for dsfuncs.c
 */
#ifndef DSFUNCS_H
#define DSFUNCS_H

#include <stdio.h>		/* what to say about it ? */
#include <string.h>
#include <stdlib.h>		/* malloc(), etc */

typedef enum {
  false, true
} bool;

/* linked list types */
typedef void *element_t;

struct node;
typedef struct node node_t;
typedef node_t *position_t;
typedef node_t *list_t;

/* a single node */
struct node {
  element_t e;
  list_t next;
};

/* queue data structure */
struct queue_struct {
  position_t front;
  position_t rear;
};
typedef struct queue_struct queue_t;

/* encapsulated type for job */
struct job_struct {
  int jid; /* job identifier */
  int secid5;
  int secid2;
  int secid3;
  int secid6;
};
typedef struct job_struct job_t;

/* encapsulated type for cpu */
struct cpu_struct {
  int cid; /* cpu id [0 indexed] */
  queue_t jobs; /* list of jobs on this cpu */
  int njobs; /* how many jobs the CPU has */
};
typedef struct cpu_struct cpu_t;

struct system_struct {
  list_t cpus; /* list of the system's  CPUs  */
};
typedef struct system_struct system_t;

/* *************** CODE **************** */

/* data type to be returned by get_cid() */
struct cpuinfo {
  cpu_t **cpu; /* the CPU pointer */
  int *cid; /* list of CPUs */
  int *len; /* lengths of the CPU list */
  int n; /* length of each of the above the lists */
};
typedef struct cpuinfo cpuinfo_t;

#define element_compare(e1, e2) (((cpu_t *)(e1))->cid - ((cpu_t *)(e2))->cid)

/* ********** prototypes ************ */
int create_new_jobid(bool init);
cpuinfo_t *get_cid(system_t *sys);
int least_loaded_cpu(system_t *sys);
void increment_cpu(system_t *sys, job_t *job);
void decrement_cpu(system_t *sys);

/* queue methods */
void qinsert(queue_t *q, element_t e);
void qdelete(queue_t *q);
void qdestroy(queue_t *q);
int qlength(queue_t *q);

/* linked list methods */
list_t list_insert(list_t l, element_t e);
list_t list_delete(list_t l, element_t e, bool free_element);
position_t list_find(list_t l, element_t e);
list_t list_destroy(list_t l, bool free_element);

#endif /* !DSFUNCS_H */
