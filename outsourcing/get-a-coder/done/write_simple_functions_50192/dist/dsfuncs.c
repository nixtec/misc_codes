/*
 * dsfuncs.c
 * distributed system functions
 * Copyright (C) Ayub <mrayub@gmail.com> <ayub@technovilla.com>
 */
#include "dsfuncs.h"

system_t *init_system(int ncpus);
void destroy_system(system_t *sys);

/* main function for testing the functions */
int main(int argc, char **argv)
{
  int new_jobid = 0; /* jobid for new job */
  int new_cpuid = 0; /* id of CPU to use */
  job_t *job = NULL; /* new allocated job or finished job */
  system_t *sys = NULL; /* our distributed system */
  int ncpus = 0; /* system with 'ncpus' CPUs */
  int transfer = YES; /* we want to transfer job to running_jobs */
  int complete = YES; /* job is complete */
  char *result = "SAMPLE RESULT";

  /* HERE'S THE FLOW OF PROGRAM */
  ncpus = num_cpus(); /* how many CPUs to use for the system */
  sys = init_system(ncpus);
  new_jobid = create_new_jobid(false);
  new_cpuid = least_loaded_cpu(sys);
  job = increment_cpu(sys, new_cpuid, new_jobid);
  if (job) { /* increment_cpu() succeeded [returned NOT NULL] */
    fprintf(stderr, "job# %d allocated to CPU# %d\n", new_jobid, new_cpuid);
    if (transfer == YES) {
      transfer_job(sys, job); /* use the job returned by increment_cpu() */
    }
  }

  /* job is complete */
  if (complete == YES) {
    job = decrement_cpu(sys, new_jobid, result); /* this job is finished */
    if (job) {
      fprintf(stderr, "job# %d finished\n", job->jid);
      /* whenever decrement_cpu() will be called, _must_ free the job [!NULL] */
      free(job); /* we _must_ free it, it was allocated by increment_cpu() */
    }
  }

  /* when we're sure all jobs we created are done, we can shut down system */
  fprintf(stderr, "*** Destroying system ***\n");
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
    list_destroy(cpu->jobs, true); /* destroy the job queue [list] */
    cpus = cpus->next;
  }
  /* LAST UPDATED HERE <START> */
  list_destroy(sys->running_jobs, false); /* previous list_destroy() freed */
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

  return (min_cid+1); /* cid is 0 indexed, so to count from 1 we add 1 */
}

/* distribute the new job to the least loaded CPU */
job_t *increment_cpu(system_t *sys, int cpuid, int jobid)
{
  list_t temp = NULL; /* just for iteration through the list */
  cpu_t *cpu = NULL; /* CPU going to be assigned the job to */
  job_t *job = NULL;

  temp = sys->cpus; /* get list of CPUs in the system */
  while (temp) { /* now find out the CPU with 'cpuid' */
    cpu = (cpu_t *) temp->e; /* get the cpu element */
    if (cpu->cid == (cpuid-1)) { /* cid starts from 0, so cpuid=1, means cid=0 */
      job = calloc(1, sizeof(job_t)); /* allocate memory for new job */
      if (!job) {
	perror("malloc");
	exit(1);
      }
      job->jid = jobid; /* juse this job id */
      job->cpu = cpu; /* the job is attached to this CPU */
      /* now add the job to the CPU's job queue [list] */
      cpu->jobs = list_insert(cpu->jobs, job);
      cpu->njobs++; /* increment the #job counter */
      break;
    }
    temp = temp->next;
  }

  return job;
}

/* remove the job from system and returns the job, caller _must_ free() it */
job_t *decrement_cpu(system_t *sys, int jobid, char *result)
{
  list_t temp, prev; /* just for iteration through the list */
  cpu_t *cpu = NULL; /* CPU the job was assigned to */
  job_t *job = NULL;
  list_t tmpjob = NULL;

  prev = NULL;
  temp = sys->running_jobs; /* first we find the job in the running queue */
  while (temp) {
    job = (job_t *) temp->e;
    if (job->jid == jobid) { /* found, remove it from running queue [list] */
      if (prev == NULL) { /* found in the head */
	sys->running_jobs = temp->next;
      } else { /* some element in the middle of the list */
	prev->next = temp->next;
      }
      free(temp);
      break;
    }
    prev = temp;
    temp = temp->next;
    job = NULL;
  }
  
  if (job) { /* we found job in the running queue */
    cpu = job->cpu;
    cpu->jobs = list_delete(cpu->jobs, job, false); /* remove job from list */
    cpu->njobs--;
  } else { /* job not found in the running queue, find it in CPU queues */
    temp = sys->cpus;
    while (temp) { /* iterate to all CPUs */
      job = NULL;
      prev = NULL;
      cpu = (cpu_t *) temp->e;
      tmpjob = cpu->jobs;
      prev = NULL;
      while (tmpjob) {
	job = (job_t *) tmpjob->e;
	if (job->jid == jobid) { /* found job */
	  if (prev == NULL) { /* first job in the list */
	    cpu->jobs = tmpjob->next;
	  } else { /* somwhere in the middle of the list */
	    prev->next = tmpjob->next;
	  }
	  cpu->njobs--; /* decrement # of jobs */
	  free(tmpjob);
	  break; /* we found the job, no more iteration through job list */
	}
	prev = tmpjob;
	tmpjob = tmpjob->next;
	job = NULL;
      }
      if (job) { /* job found */
	break; /* no more iteration through the CPU list */
      }
      temp = temp->next;
    }
  }

  return job;
}

/* transfer job to sys->running_jobs */
job_t *transfer_job(system_t *sys, job_t *job)
{
  sys->running_jobs = list_insert(sys->running_jobs, job);
  return job;
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

#define NCPUS 5		/* just for example i'm using this value */
int num_cpus(void)
{
  return NCPUS;
}


/* ******************* LINKED LIST FUNCTIONS ************** */
/* inserts an element at the head of the list: O(1) */
list_t list_insert(list_t l, element_t e)
{
  node_t *node;

  node = malloc(sizeof(node_t));
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

int list_length(list_t l)
{
  int len = 0;
  list_t temp = l;

  while (temp) {
    len++;
    temp = temp->next;
  }

  return len;
}