
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

