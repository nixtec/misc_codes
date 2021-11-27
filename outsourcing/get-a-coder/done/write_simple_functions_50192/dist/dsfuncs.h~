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

/* LAST UPDATED HERE <START> */
#define YES	1
#define NO	0
/* LAST UPDATED HERE <END> */

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

#define MAXBUF 1024	/* maximum buffer size for sections */
/* if you want to use character string as result, you can remove the following
 * definition
 */
/* encapsulated type for cpu */
struct cpu_struct {
  int cid; /* cpu id [0 indexed] */
  list_t jobs; /* list of jobs on this cpu */
  int njobs; /* how many jobs the CPU has */
  /* LAST UPDATED HERE <START> */
  int complete; /* a job complete? YES/NO */
  char result[MAXBUF]; /* result of the last finished job */
  /* LAST UPDATED HERE <END> */
};
typedef struct cpu_struct cpu_t;

/* encapsulated type for job */
struct job_struct {
  int jid; /* job identifier */
  cpu_t *cpu; /* the CPU the job is assigned in */
  /* LAST UPDATED HERE <START> */
  char sec2[MAXBUF]; /* operations string */
  char sec3[MAXBUF]; /* some information string */
  char sec5[MAXBUF]; /* input string */
  char sec6[MAXBUF]; /* output [initially blank] string [or int!] */
  /* LAST UPDATED HERE <END> */
};
typedef struct job_struct job_t;

struct system_struct {
  list_t cpus; /* list of the system's  CPUs  */
  /* LAST UPDATED HERE <START> */
  list_t running_jobs; /* running job queue [list] */
  /* if you want character string like "yes", then declare the following as
   * character arrays. however in that case you will have to use strcmp() like
   * functions to check the values
   */
  /* transfer should be system-wise, because running_procs queue will either
   * contain all jobs or no jobs. otherwise during decrement_cpu() we'll get
   * wrong sequence of jobs to free. Because Queue allows only removal from
   * front
   */
  /* LAST UPDATED HERE <END> */
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

#define element_compare(e1, e2) (((job_t *)(e1))->jid - ((job_t *)(e2))->jid)

/* ********** prototypes ************ */
int create_new_jobid(bool init); /* new job id */
cpuinfo_t *get_cid(system_t *sys);
int least_loaded_cpu(system_t *sys);
/* assign job to cpu_id */
job_t *increment_cpu(system_t *sys, int cpuid, int jobid);
/* jobid is finished, remove it */
job_t *decrement_cpu(system_t *sys, int jobid, char *result);
/* transfer job to running queue */
job_t *transfer_job(system_t *sys, job_t *job);
int num_cpus(void); /* number of CPUs to be used for a system */


/* linked list methods */
list_t list_insert(list_t l, element_t e);
list_t list_delete(list_t l, element_t e, bool free_element);
position_t list_find(list_t l, element_t e);
list_t list_destroy(list_t l, bool free_element);
int list_length(list_t l);

#endif /* !DSFUNCS_H */
