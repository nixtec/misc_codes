/* include unlock */
#include "pthread.h"

#define err_dump printf

int
pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
  int result;

  if (rw->rw_magic != RW_MAGIC)
    return(EINVAL);

  if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
    return(result);

  if (rw->rw_refcount > 0)
    rw->rw_refcount--;			/* releasing a reader */
  else if (rw->rw_refcount == -1)
    rw->rw_refcount = 0;		/* releasing a reader */
  else
    err_dump("rw_refcount = %d\n", rw->rw_refcount);

  /* give preference to waiting writers over waiting readers */
  if (rw->rw_nwaitwriters > 0) {
    if (rw->rw_refcount == 0)
      result = pthread_cond_signal(&rw->rw_condwriters);
  } else if (rw->rw_nwaitreaders > 0)
    result = pthread_cond_broadcast(&rw->rw_condreaders);

  pthread_mutex_unlock(&rw->rw_mutex);
  return(result);
}
/* end unlock */
