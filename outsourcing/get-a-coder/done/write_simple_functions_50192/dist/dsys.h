/*
 * dsys.h
 * header for dsys.c
 */
#ifndef WSF_H
#define WSF_H

#include <stdio.h>		/* what to say about it ? */
#include <string.h>
#include <stdlib.h>		/* malloc(), etc */

/* this is simulation project, so a very flexible type system is used */

typedef enum {
  false, true
} bool;

/* linked list types */
typedef void *element_t;

struct node;
typedef struct node node_t;
typedef node_t *position_t;
typedef node_t *list_t;

struct node {
  element_t e;
  list_t next;
};

/* distributed system related types */

/* type for job */
#define NSECS 4		/* section IDs as specified in the problem document */
typedef int jid_t; /* encapsulated job identifier */
typedef int sid_t; /* encapsulated section identifier */
#define JOB_ACTIVE	1
#define JOB_SUSPENDED	0
typedef int stat_t; /* status of the job [active,suspended] */
struct job_struct {
  jid_t jid; /* job identifier */
  sid_t secs[NSECS]; /* section identifiers for the job */
  stat_t status; /* status of the job */
  int n_cpu; /* in which CPU this job is attached */
};
typedef struct job_struct job_t;

#define CPU_UP		1		/* CPU status */
#define CPU_DOWN	0		/* CPU status */
#define INVALID_CPU	-1		/* CPU status, uninitizlied */
/* type for cpu */
struct cpu_struct {
  int cid; /* cpu id [0 indexed] */
  stat_t status; /* status of the CPU */
  list_t jobs; /* list of jobs on this cpu */
  int njobs; /* how many jobs currently attached with this cpu */
};
typedef struct cpu_struct cpu_t;

/* the whole distributed system */
#define NCPUS 6
#define INVALID_SYSTEM -1
struct system_struct {
  int sid; /* system identifier, we may have several systems [0 indexed] */
  int ncpus; /* # of cpus for the system, O(1)  */
  list_t cpus; /* the system has 'ncpus' CPUs  */
  list_t job_list; /* job list for the network */
  /* for load balancing */
  int cpu_to_use; /* when new job arrives, attach it with this CPU */
};
typedef struct system_struct system_t;

/* types for input */

/* job initialization */
const char INIT[] = "INIT";		/*  job initiation  */
#define INIT_VAL 0
const char COMP[] = "COMP";		/*  job completion  */
#define COMP_VAL 1
const char DOWN[] = "DOWN";		/*  CPU is down        */
#define DOWN_VAL 2
const char REST[] = "REST";		/*  CPU is restored    */
#define REST_VAL 3
const char SHOW[] = "SHOW";		/*  show system status */
#define SHOW_VAL 4
const char EXIT[] = "EXIT";		/*  exit from program   */
#define EXIT_VAL 4
#define BADT_VAL 5			/*  bad type val        */
struct input_struct {
  char type[10]; /* type of input [one of the above] */
  int typeval; /* integer equivalent of the string type */
  /* data for possible inputs */
  jid_t jid; /* job ID for initiation */
  sid_t secs[NSECS]; /* section identifiers for the job for initiation */
  int n_cpu; /* which CPU is down/restored [0 indexed] */
};
typedef struct input_struct input_t;


/* public functions */
void run_simulator(void);
/* first obtain a system */
/* uses static variable, not thread-safe */
system_t *init_system(void);
/* take actoin on network according to input, return the result */
int action_on_system(system_t *sys, input_t *input);
/* we're done, destroy the network */
void destroy_system(system_t *sys);
void show_system_status(system_t *sys);
void dump_job_list(list_t l, bool verbose);
void print_job_info(job_t *job, bool verbose);
void update_cpu_to_use(system_t *sys);

/* for thread-safety, use the following routines */
void system_init(system_t *sys, int sid, int ncpus); /* initialize system */
void system_free(system_t *sys);
/* now manage the obtained network */
/* high level routine that calls other functions */

int job_init(system_t *sys, job_t *job); /* initialize job in sys */
int job_comp(system_t *sys, jid_t jid); /* process completed */
int cpu_down(system_t *sys, int n_cpu); /* cpu is down */
int cpu_rest(system_t *sys, int n_cpu); /* cpu is restored */
void trigger_interrupt(list_t l, stat_t status); /* set status for all in l */
/*
 * macro equivalent of this function is available
 * int element_compare(element_t *e1, element_t *e2);
 */

/* ********* LIST FUNCTIONS ********* */
list_t		list_destroy(list_t t, bool free_element); /* free element? */
position_t	list_find(element_t e, list_t t);
position_t	list_find_min(list_t l);
position_t	list_find_max(list_t l);
list_t		list_insert(element_t e, list_t l);
list_t		list_delete(element_t e, list_t l, bool free_element);
/*
 * macro equivalent of this function is available
 * element_t	list_get_element(position_t p);
 */


/* these functions will be called several times, so using macros */
#define element_compare(e1, e2) (((job_t *)(e1))->jid - ((job_t *)(e2))->jid)
#define find_suitable_cpu(sys) ((sys)->cpu_to_use)
#define list_get_element(p) ((p)->e)


#endif /* !WSF_H */
