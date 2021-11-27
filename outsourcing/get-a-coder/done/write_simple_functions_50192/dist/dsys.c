/*
 * dsys.c
 * distributed system functions
 * Copyright (C) Ayub <mrayub@gmail.com> <ayub@technovilla.com>
 */
#include "dsfuncs.h"

system_t *init_system(int ncpus);
void destroy_system(system_t *sys);

/* main function for testing the functions */
int main(int argc, char **argv)
{
  int i;
  int njobs; /* create 'n' number of jobs */
  int ncpus; /* system with 'ncpus' CPUs */
  job_t *job;
  system_t *sys;
  cpu_t *cpu;
  list_t temp;

  ncpus = 5;
  sys = init_system(ncpus);
  if (!sys) {
    fprintf(stderr, "failed to initialize system!!!\n");
    exit(1);
  }

  njobs = 17;
  for (i = 0; i < njobs; i++) {
    job = malloc(sizeof(job_t));
    if (!job) {
      perror("malloc");
      exit(1);
    }
    job->jid = create_new_jobid(false); /* create new job */
    increment_cpu(sys, job); /* assign the job to the lest loaded cpu */
  }

  /* let's see what happened really so far */
  printf("... ... ...\n");
  temp = sys->cpus;
  while (temp) {
    cpu = (cpu_t *) temp->e;
    printf("CPU#: %-4d, #jobs: %-4d\n", cpu->cid+1, cpu->njobs);
    temp = temp->next;
  }
  printf("... ... ...\n");

  /* least_loaded_cpu() returnes cpu id, 0 indexed, so adding 1 */
  printf("Least loaded cpu is CPU# %d\n", least_loaded_cpu(sys)+1);

  fprintf(stderr, "\n\n*** Shutting down system\n");
  /* shutdown the jobs */
  for (i = 0; i < njobs; i++) {
    job = decrement_cpu(sys); /* free CPU, heavily loaded one, return the job */
    free(job); /* we allocated it */
  }

  fprintf(stderr, "*** Destroying system\n");
  /* destroy the system */
  destroy_system(sys);

  return 0;
}

system_t *init_system(int ncpus)
{
  system_t *sys;
  int i;
  cpu_t *cpu;

  sys = malloc(sizeof(system_t));
  if (!sys) {
    perror("malloc");
    exit(1);
  }
  memset(sys, 0, sizeof(system_t));

  for (i = 0; i < ncpus; i++) {
    cpu = calloc(1, sizeof(cpu_t));
    cpu->cid = i;
    sys->cpus = list_insert(sys->cpus, cpu);
  }

  return sys;
}

void destroy_system(system_t *sys)
{
  list_t cpus = sys->cpus;
  cpu_t *cpu;

  while (cpus) {
    cpu = (cpu_t *) cpus->e;
    qdestroy(&cpu->jobs, false); /* destroy the job queue */
    cpus = cpus->next;
  }
  /* LAST UPDATED HERE <START> */
  qdestroy(&sys->running_procs, true); /* now free the job really */
  /* LAST UPDATED HERE <END> */
  list_destroy(sys->cpus, true);

}

/* **************** CODE *************** */
/* get list of cpus and their lengths */
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
    cinfo->cpu = realloc(cinfo->cpu, sizeof(cpu_t *) * (n+1));
    cpu = (cpu_t *) cur->e; /* get the CPU element */
    cinfo->cpu[n] = cpu;
    cinfo->cid[n] = cpu->cid; /* get the cpu id */
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

  cinfo->n = n;

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

  printf("Job# %-4d assigned to CPU# %-4d\n", job->jid, n_cpu+1);
  qinsert(&cinfo->cpu[n_cpu]->jobs, job);
  cinfo->cpu[n_cpu]->njobs++;

  /* LAST UPDATED HERE <START> */
  sys->transfer = YES; /* we want to transfer job to running queue too */
  /* if 'transfer == 'yes', the information will be queued to the running Q */
  if (sys->transfer == YES) {
    /*
    fprintf(stderr,
       	"you have to transfer the information to the running Queue\n");
	*/
    /* transfer [add] the information to the running queue */
    /* NOTE: we are adding the job to the running queue, so sys->transfer value
     * will have to be the same value all the time. Because otherwise we'll get
     * wrong sequence of jobs in two queues, that is, the queues won't hold
     * same list of jobs. It's not a problem during increment_cpu(), but will
     * be a problem during decrement_cpu(). Because decrement_cpu() will try
     * to remove the job from both queue, so if they don't contain same
     * sequence of jobs, two qdelete() operation will delete wrong jobs. So
     * throughout the program either the sys->transfer will hold only YES or
     * only NO. This restriction applies because we're using Queue data
     * structure here for jobs. Because qdelete() deletes the earliest entered
     * entry in the queue.
     */
    qinsert(&sys->running_procs, job);
  }
  /* LAST UPDATED HERE <END> */

  free(cinfo->cpu);
  free(cinfo->len);
  free(cinfo->cid);
  free(cinfo);
}

/* remove a job from the most loaded CPU */
void decrement_cpu(system_t *sys)
{
  int max_cid;
  int max_len;
  int i, n_cpu;
  struct cpuinfo *cinfo = NULL;
  job_t *job = NULL;

  cinfo = get_cid(sys);

  /* find max loaded CPU */
  max_cid = cinfo->cid[0];
  max_len = cinfo->len[0];
  n_cpu = 0;
  for (i = 0; i < cinfo->n; i++) {
    if (cinfo->len[i] > max_len) {
      max_cid = cinfo->cid[i];
      max_len = cinfo->len[i];
      n_cpu = i; /* n_cpu contains index of the least loaded CPU */
    }
  }

  job = qdelete(&cinfo->cpu[n_cpu]->jobs);
  cinfo->cpu[n_cpu]->njobs--;

  /* LAST UPDATED HERE <START> */
  cinfo->cpu[n_cpu]->complete = YES; /* job is completed */
#ifdef _USE_INT_RESULT_
  cinfo->cpu[n_cpu]->result = RESULT_OK; /* result is OK */
#else
  strcpy(cinfo->cpu[n_cpu]->result, "OK"); /* do whatever you want */
#endif
  if (cinfo->cpu[n_cpu]->complete == YES) {
#ifdef _USE_INT_RESULT_
    job->sec6 = cinfo->cpu[n_cpu]->result;
#else
    strcpy(job->sec6, cinfo->cpu[n_cpu]->result);
#endif
    /*
    fprintf(stderr,
	"section id's information are transferred back to where you got!\n");
	*/
    /* do whatever you want with the job here b4 freeing it */
  }
  if (sys->transfer == YES) { /* we inserted new jobs to the running queue */
    job = qdelete(&sys->running_procs); /* delete job from running queue */
  }

  if (job != NULL) {
    free(job); /* we no longer need this job */
  }
  /* LAST UPDATED HERE <END> */

  free(cinfo->cpu);
  free(cinfo->len);
  free(cinfo->cid);
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

/* insert a job to the queue */
position_t qinsert(queue_t *q, element_t e)
{
  if (!q) {
    fprintf(stderr, "No queue supplied!\n");
    return NULL;
  }

  node_t *newnode; /* New node to be inserted */
  newnode = malloc(sizeof(node_t));
  newnode->e = e;
  newnode->next = NULL;
  if (q->front == NULL && q->rear == NULL) {
    q->front = newnode;
    q->rear = newnode;
  } else {
    q->rear->next = newnode;
    q->rear = newnode;
  }

  return q->rear; /* always rear */
}

/* delete node from queue, return the element */
element_t qdelete(queue_t *q)
{
  position_t delnode; /* Node to be deleted */
  element_t e = NULL;

  if (!q) {
    return NULL;
  }

  if (q->front == NULL && q->rear == NULL) {
    printf ("Empty queue!\n");
  } else {
    delnode = q->front; /* saving the pointer to free() it */
    e = delnode->e;
    q->front = q->front->next; /* update the front of the queue */
    free(delnode);
  }

  return e;
}

/* destroy the Queue, free element if 'free_element' is true */
void qdestroy(queue_t *q, bool free_element)
{
  position_t delnode; /* Node to be deleted */

  if (!q) {
    return;
  }

  while (q->front) {
    delnode = q->front; /* saving the pointer to free() it */
    q->front = q->front->next; /* update the front of the queue */
    if (free_element == true && delnode->e != NULL) {
      free(delnode->e);
    }
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


/* ******************* LINKED LIST FUNCTIONS ************** */
/* inserts an element at the head of the list: O(1) */
list_t list_insert(list_t l, element_t e)
{
  list_t node;

  node = malloc(sizeof(list_t));
  if (!node) {
    perror("malloc");
    exit(1);
  }
  node->e = e;
  node->next = l;
  l = node;

  return l;
}

/* deletes e from the list, fee if 'free_element' is true: O(n) */
list_t list_delete(list_t l, element_t e, bool free_element)
{
  list_t prev, cur;

  if (!l) {
    fprintf(stderr, "List empty!\n");
    return l;
  }

  prev = NULL;
  cur = l;
  while (cur) {
    if (element_compare(e, cur->e) == 0) {
      if (!prev) { /* no previous, head to be deleted */
	l = cur->next;
      } else {
	prev->next = cur->next;
      }
      if (free_element) {
	free(cur->e);
      }
      free(cur);
      break;
    }
    prev = cur;
    cur = cur->next;
  }

  return l;
}

/* find an element in the list */
position_t list_find(list_t l, element_t e)
{
  if (!l)
    return NULL;

  while (l) {
    if (element_compare(e, l->e) == 0)
      break; /* found */
    l = l->next;
  }

  return l;
}

/* destroy the list, free element if 'free_element' is true */
list_t list_destroy(list_t l, bool free_element)
{
  list_t temp1, temp2;

  temp2 = l;

  while (temp2) {
    temp1 = temp2;
    temp2 = temp1->next;
    if (free_element == true) {
      free(temp1->e);
    }
    free(temp1);
  }

  return NULL;
}
