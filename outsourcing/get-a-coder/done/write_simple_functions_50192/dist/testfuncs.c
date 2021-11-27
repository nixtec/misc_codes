/*
 * testfuncs.c
 */
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
  int id; /* job identifier */
  int secid5;
  int secid2;
  int secid3;
  int secid6;
};
typedef struct job_struct job_t;

/* encapsulated type for cpu */
struct cpu_struct {
  int id; /* cpu id [0 indexed] */
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
  cpu_t *cpu;
  int *cid; /* list of CPUs */
  int *len; /* lengths of the CPU list */
  int n; /* length of each of the above the lists */
};
typedef struct cpuinfo cpuinfo_t;


/* ********** prototypes ************ */
int create_new_jobid(bool init);
cpuinfo_t *get_cid(system_t *sys);
int least_loaded_cpu(system_t *sys);
void increment_cpu(system_t *sys, job_t *job);
void decrement_cpu(system_t *sys);

/* queue methods */
void qinsert(queue_t *q, element_t e);
void qdelete(queue_t *q);
int qlength(queue_t *q);



/* **************** CODE *************** */
cpuinfo_t *get_cid(system_t *sys)
{
  list_t cur;
  cpu_t *cpu;
  int n;
  struct cpuinfo *cinfo;

  cinfo = malloc(sizeof(struct cpuinfo));
  if (!cinfo) {
    perror("malloc");
    exit(1);
  }

  n = 0;
  memset(cinfo, 0, sizeof(cpuinfo_t));
  cur = sys->cpus; /* get list of CPUs in the system */
  while (cur) {
    cinfo->cid = realloc(cinfo->cid, sizeof(int) * (n+1));
    cinfo->len = realloc(cinfo->len, sizeof(int) * (n+1));
    cinfo->cpu = realloc(cinfo->cpu, sizeof(cpu_t) * (n+1));
    cpu = (cpu_t *) cur->e; /* get the CPU element */
    cinfo->cid[n] = cpu->id; /* get the cpu id */
    /* we can call qlength(&cpu->jobs) to calculate the number of jobs.
     * but if get_cid() is called several times, it will be inefficient. We
     * have the number of jobs running under a cpu saved in cpu->njobs.
     * we can use it. whenever a new job is added/removed, that variable is
     * updated accordingly. so we can safely use that variable direcly without
     * calling the qlength(). However, if you want to calculate the
     * length of queue, you can call it. Just it will slow down the system
     */
    /* cinfo->len[n] = qlength(&cpu->jobs); */ /* inefficient */
    cinfo->len[n] = cpu->njobs; /* number of jobs currently under this CPU */
    n++;
    cur = cur->next;
  }

  return cinfo;
}

/* returns least loaded cpu id. 0 indexed */
int least_loaded_cpu(system_t *sys)
{
  int min_cid; /* cid of CPU with minimum length of queue */
  int min_len; /* minimum length of queue */
  int i, n_cpu;
  struct cpuinfo *cinfo = NULL;

  /* get list of CPUs and their lengths */
  cinfo = get_cid(sys);

  min_cid = cinfo->cid[0];
  min_len = cinfo->len[0];
  n_cpu = 0;
  for (i = 0; i < cinfo->n; i++) {
    if (cinfo->len[i] < min_len) { /* less than min_len found */
      min_cid = cinfo->cid[i];
      min_len = cinfo->len[i];
      n_cpu = i;
    }
  }

  free(cinfo->cpu);
  free(cinfo->len);
  free(cinfo->cid);
  free(cinfo);

  return min_cid;
}

/* distribute the new job to the least loaded CPU */
void increment_cpu(system_t *sys, job_t *job)
{
  int min_cid; /* cid of CPU with minimum length of queue */
  int min_len; /* minimum length of queue */
  int i, n_cpu;
  struct cpuinfo *cinfo = NULL;

  cinfo = get_cid(sys);

  /* find least loaded CPU */
  min_cid = cinfo->cid[0];
  min_len = cinfo->len[0];
  n_cpu = 0;
  for (i = 0; i < cinfo->n; i++) {
    if (cinfo->len[i] < min_len) { /* less than min_len found */
      min_cid = cinfo->cid[i];
      min_len = cinfo->len[i];
      n_cpu = i; /* n_cpu contains index of the least loaded CPU */
    }
  }

  qinsert(&cinfo->cpu[n_cpu].jobs, job);
  cinfo->cpu[n_cpu].njobs++;

  for (i = 0; i < cinfo->n; i++) {
    free(cinfo->cpu);
    free(cinfo->len);
    free(cinfo->cid);
  }
  free(cinfo);
}

/* remove a job from the most loaded CPU */
void decrement_cpu(system_t *sys)
{
  int max_cid;
  int max_len;
  int i, n_cpu;
  struct cpuinfo *cinfo = NULL;

  cinfo = get_cid(sys);

  /* find max loaded CPU */
  max_cid = cinfo->cid[0];
  max_len = cinfo->len[0];
  n_cpu = 0;
  for (i = 0; i < cinfo->n; i++) {
    if (cinfo->cid[i] > max_cid) {
      max_cid = cinfo->cid[i];
      max_len = cinfo->len[i];
      n_cpu = i; /* n_cpu contains index of the least loaded CPU */
    }
  }

  qdelete(&cinfo->cpu[n_cpu].jobs);
  cinfo->cpu[n_cpu].njobs--;

  for (i = 0; i < cinfo->n; i++) {
    free(cinfo->cpu);
    free(cinfo->len);
    free(cinfo->cid);
  }
  free(cinfo);
}

/* create a new job, return the job id */
int create_new_jobid(bool init)
{
  static int jid;

  if (init) {
    jid = 0;
  }

  return ++jid;
}


/* ***************** QUEUE USING LINKED LIST **************** */
void qinsert(queue_t *q, element_t e)
{
  if (!q) {
    fprintf(stderr, "No queue supplied!\n");
    return;
  }

  node_t *newnode; /* New node to be inserted */
  newnode = malloc(sizeof(node_t));
  newnode->next = NULL;
  if (q->front == NULL && q->rear == NULL) {
    q->front = newnode;
    q->rear = newnode;
  } else {
    q->rear->next = newnode;
    q->rear = newnode;
  }
}

/* delete job from queue */
void qdelete(queue_t *q)
{
  position_t delnode; /* Node to be deleted */

  if (!q) {
    return;
  }

  if (q->front == NULL && q->rear == NULL) {
    printf ("Empty queue!\n");
  } else {
    delnode = q->front; /* saving the pointer to free() it */
    q->front = q->front->next; /* update the front of the queue */
    free(delnode);
  }
}

/* calculate length of a queue */
int qlength(queue_t *q)
{
  int len;
  position_t temp;

  if (!q)
    return 0;

  temp = q->front; /* we start from front and iterate until we get NULL */
  len  = 0;
  while (temp) {
    len++;
    temp = temp->next;
  }

  return len;
}
