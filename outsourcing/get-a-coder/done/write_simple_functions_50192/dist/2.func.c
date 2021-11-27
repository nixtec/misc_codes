
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

