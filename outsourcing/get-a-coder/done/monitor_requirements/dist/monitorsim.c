/*
 * monitorsim.c
 * Monitor Simulator
 * by Ayub <mrayub@gmail.com>
 */
#include "monitorsim.h"

int monitor_wait_func(monitor_t *mon, proc_t *proc);
int monitor_signal_func(monitor_t *mon, int on);
int monitor_read_func(monitor_t *mon, proc_t *proc);
int monitor_write_func(monitor_t *mon, proc_t *proc);
int monitor_start_func(monitor_t *mon, proc_t *proc);
int monitor_end_func(monitor_t *mon, proc_t *proc);
int monitor_interrupt_func(monitor_t *mon, proc_t *proc);
void monitor_statistics_func(monitor_t *mon); 

bool system_schedule_func(system_t *sys);

typedef bool (*compare_func)(element_t, element_t);
/* queue functinos */
position_t qinsert(queue_t *q, element_t e);
element_t qdelete(queue_t *q);
element_t qextract(queue_t *q, element_t e, compare_func compare);
element_t qfetch(queue_t *q);
bool compare_elements_by_type_id(element_t e1, element_t e2);
bool compare_elements_by_type(element_t e1, element_t e2);

int do_lazy_read(monitor_t *mon);
void do_lazy_write(monitor_t *mon);

void run_simulator(void);

int main(int argc, char **argv)
{
  run_simulator();

  return 0;
}

bool system_schedule_func(system_t *sys)
{
  proc_t *proc;

  if ((proc = qfetch(&sys->mon.urgent))) { /* check if any urgent process */
    if (proc->type == READER && !sys->mon.write_locked) {
      proc = qdelete(&sys->mon.urgent);
      proc->t2 = time(NULL);
      sys->mon.s.totaltimecond += (proc->t2 - proc->t1);
      proc->t1 = time(NULL); /* monitor time calculation started */
      printf("<-- Reader [id=%d] resuming execution, eip=%d\n",
	  proc->id, proc->eip);
      printf("<<- Reader [id=%d] [interrupted] entering monitor\n", proc->id);
      sys->mon.read(&sys->mon, proc);
    } else if (proc->type == WRITER && !sys->mon.read_locked && !sys->mon.write_locked) {
      proc = qdelete(&sys->mon.urgent);
      proc->t2 = time(NULL);
      sys->mon.s.totaltimecond += (proc->t2 - proc->t1);
      proc->t1 = time(NULL); /* monitor time calculation started */
      printf("<-- Writer [id=%d] resuming execution, eip=%d\n",
	  proc->id, proc->eip);
      printf("<<- Writer [id=%d] [interrupted] entering monitor\n", proc->id);
      sys->mon.write(&sys->mon, proc);
    }
  } else if ((proc = qfetch(&sys->ready))) { /* check if we can get one from ready queue */
    if (proc->type == READER && !sys->mon.write_locked) {
      proc = qdelete(&sys->ready);
      sys->mon.s.nprocsmon++;
      proc->done = 0; /* done reading b4 interruption */
      proc->t1 = time(NULL);
      printf("<<< Reader [id=%d] entering monitor\n", proc->id);
      sys->mon.read(&sys->mon, proc);
    } else if (proc->type == WRITER && !sys->mon.read_locked && !sys->mon.write_locked) {
      proc = qdelete(&sys->ready);
      sys->mon.s.nprocsmon++;
      proc->done = 0; /* done reading b4 interruption */
      proc->t1 = time(NULL);
      printf("<<< Writer [id=%d] entering monitor\n", proc->id);
      sys->mon.write(&sys->mon, proc);
    }
  }

  if ((proc = qfetch(&sys->mon.conditional))) { /* check if processes in conditional queue can be moved to urgent queue */
    if (proc->type == READER) {
      proc = qdelete(&sys->mon.conditional);
      printf("*** Reader [id=%d] moving from conditional queue to urgent queue\n", proc->id);
      qinsert(&sys->mon.urgent, proc);
    } else if (proc->type == WRITER) {
      proc = qdelete(&sys->mon.conditional);
      printf("*** Writer [id=%d] moving from conditional queue to urgent queue\n", proc->id);
      qinsert(&sys->mon.urgent, proc);
    }
  }

  return true;
}

int monitor_wait_func(monitor_t *mon, proc_t *proc)
{
  if (proc->type == READER) {
    printf("!!! Reader [id=%d] put to wait queue\n", proc->id);
    mon->read_locked--;
  } else if (proc->type == WRITER) {
    printf("!!! Writer [id=%d] put to wait queue\n", proc->id);
    mon->write_locked--;
  }
  qinsert(&mon->conditional, proc);
  return 0;
}

/* unblocks a process waiting on condition 'on' */
int monitor_signal_func(monitor_t *mon, int on)
{
  proc_t *proc;

  if (on) {
    proc = (proc_t *) qdelete(&mon->urgent); /* use whatever queue you prefer */
    printf("[+] Process %d unblocked\n", proc->id);
    if (proc->type == READER) {
      mon->read(mon, proc);
    } else if (proc->type == WRITER) {
      mon->write(mon, proc);
    }
  }
  return 0;
}

int monitor_read_func(monitor_t *mon, proc_t *proc)
{
  mon->read_locked++;
  qinsert(&mon->running, proc);
  if (proc->done == 0) { /* will not read twice */
    proc->done = 1;
    do_lazy_read(mon); /* reader will call this function to read data */
  }

  return 0;
}

int monitor_write_func(monitor_t *mon, proc_t *proc)
{
  mon->write_locked++;
  qinsert(&mon->running, proc);
  if (proc->done == 0) { /* will not read twice */
    proc->done = 1;
    do_lazy_write(mon); /* reader will call this function to read data */
  }

  return 0;
}

/* cleanup process from running queue, will match against 'type' and 'id' */
int monitor_end_func(monitor_t *mon, proc_t *proc)
{
  proc_t *tmproc;

  tmproc = qextract(&mon->running, proc, compare_elements_by_type_id);
  if (tmproc) {
    if (tmproc->type == READER) {
      printf("--- Reader [id=%d] done execution, cleaning up\n", tmproc->id);
      mon->read_locked--;
    } else {
      printf("--- Writer [id=%d] done execution, cleaning up\n", tmproc->id);
      mon->write_locked--;
    }
    tmproc->t2 = time(NULL);
    mon->s.totaltimemon += (tmproc->t2 - tmproc->t1);
    free(tmproc);
  }
  return 0;
}

int monitor_interrupt_func(monitor_t *mon, proc_t *proc)
{
  proc_t *tmproc;

  tmproc = qextract(&mon->running, proc, compare_elements_by_type_id);
  if (tmproc) {
    if (proc->type == READER) {
      printf("+++ Reader [id=%d] interrupted\n", tmproc->id);
    } else {
      printf("+++ Writer [id=%d] interrupted\n", tmproc->id);
    }
    mon->s.nprocscond++;
    tmproc->t2 = time(NULL);
    mon->s.totaltimemon += (tmproc->t2 - tmproc->t1);
    tmproc->eip = __LINE__; /* it will be instruction location in real */
    mon->wait(mon, tmproc);
  }

  return 0;
}

void monitor_statistics_func(monitor_t *mon)
{
  mon->s.avgtimemon = (float) (mon->s.totaltimemon / mon->s.nprocsmon);
  mon->s.avgtimecond = (float) (mon->s.totaltimecond / mon->s.nprocscond);
  printf("\n*** Monitor Execution Summary ***\n");
  printf("Number of processes that used the monitors: %d\n", mon->s.nprocsmon);
  printf("Number of processes that spent time on conditional variable queue: %d\n", mon->s.nprocscond);
  printf("Average time spent on conditional variable queue: %f seconds\n", mon->s.avgtimecond);
  printf("Average time spent in monitor per process: %f seconds\n", mon->s.avgtimemon);
}

system_t *system_init(void)
{
  system_t *sys;
  sys = calloc(1, sizeof(system_t));
  sys->schedule = system_schedule_func;
  sys->mon.read = monitor_read_func;
  sys->mon.write = monitor_write_func;
  sys->mon.wait = monitor_wait_func;
  sys->mon.signal = monitor_signal_func;
  sys->mon.end = monitor_end_func;
  sys->mon.interrupt = monitor_interrupt_func;
  sys->mon.statistics = monitor_statistics_func;

  printf("Initialized monitor resource to value: %d\n", sys->mon.res);

  return sys;
}

void run_simulator(void)
{
  char buf[80];
  char who[10];
  char status[10];
  int id;
  proc_t *proc;
  system_t *sys;

  sys = system_init();
  while (fgets(buf, sizeof(buf), stdin)) {
    if (buf[0] == '\n') { /* skip empty lines */
      continue;
    }
    memset(who, 0, sizeof(who));
    memset(status, 0, sizeof(status));
    id = 0;
    sscanf(buf, "%s%s%d", status, who, &id);
    if (id <= 0) {
      printf("id must be grater than 0, Skipping the rest of the line\n");
      continue;
    }

    proc = calloc(1, sizeof(proc_t));
    proc->id = id;
    if (!strcmp(who, "reader")) {
      proc->type = READER;
    } else if (!strcmp(who, "writer")) {
      proc->type = WRITER;
    }

    if (!strcmp(status, "come")) {
      if (proc->type == READER) {
	printf(">>> Reader [id=%d] put to ready queue\n", proc->id);
      } else if (proc->type == WRITER) {
	printf(">>> Writer [id=%d] put to ready queue\n", proc->id);
      }
      qinsert(&sys->ready, proc);
    } else if (!strcmp(status, "done")) {
      sys->mon.end(&sys->mon, proc);
      free(proc);
    } else if (!strcmp(status, "interrupt")) {
      sys->mon.interrupt(&sys->mon, proc);
    } else {
      printf("I don't understand what you really want me to do!\n");
    }
    sys->schedule(sys); /* schedule a process from ready queue */
  }
  sys->mon.statistics(&sys->mon);

  free(sys);
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

  if (q->front != NULL) {
    delnode = q->front; /* saving the pointer to free() it */
    e = delnode->e;
    q->front = q->front->next; /* update the front of the queue */
    if (q->front == NULL) {
      q->rear = NULL;
    }
    free(delnode);
  }

  return e;
}

/* returns the first node from queue, doesn't modify the queue */
element_t qfetch(queue_t *q)
{
  element_t e = NULL;

  if (q && q->front) {
    e = q->front->e;
  }

  return e;
}


/* extract a node matching 'e' {type,id} from queue */
element_t qextract(queue_t *q, element_t e, compare_func compare)
{
  position_t prevnode, curnode, nextnode;
  element_t tmpe = NULL;

  if (!q) {
    return NULL;
  }

  prevnode = curnode = q->front;
  nextnode = curnode->next;

  while (curnode) {
    tmpe = curnode->e;
    if (compare(tmpe, e)) {
      if (curnode == q->front) {
	q->front = nextnode;
	if (q->front == NULL) {
	  q->rear = NULL;
	}
      } else if (curnode == q->rear) {
	q->rear = prevnode;
      } else {
	prevnode->next = curnode->next;
      }
      free(curnode);
      break;
    }
    prevnode = curnode;
    curnode = curnode->next;
    if (curnode) {
      nextnode = curnode->next;
    }
    tmpe = NULL;
  }

  return tmpe;
}

bool compare_elements_by_type_id(element_t e1, element_t e2)
{
  proc_t *p1, *p2;
  p1 = (proc_t *) e1;
  p2 = (proc_t *) e2;

  return (p1->type == p2->type && p1->id == p2->id);
}

bool compare_elements_by_type(element_t e1, element_t e2)
{
  proc_t *p1, *p2;
  p1 = (proc_t *) e1;
  p2 = (proc_t *) e2;

  return (p1->type == p2->type);
}

void do_lazy_write(monitor_t *mon)
{
  /* writer will do whatever it wants here */
  mon->res++;
  sleep(WRITER_EXEC_TIME);
  printf("<=< Writer: incremented, value of resource = %d\n", mon->res);
}

int do_lazy_read(monitor_t *mon)
{
  /* reader will do whatever it wants here */
  printf(">=> Reader: current value of resource = %d\n", mon->res);
  sleep(READER_EXEC_TIME);
  return mon->res;
}

