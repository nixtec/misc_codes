
/* create a new job, return the job id */
int create_new_jobid(bool init)
{
  static int jid;

  if (init) {
    jid = 0;
  }

  return ++jid;
}

