/*
 * lockdemo-mt.c
 * testing flock() in multithreaded environment
 *
 * in linux flock() is working in multithreaded environment too
 * here each threads opens the file, so file descriptors are different
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <pthread.h>
#include <unistd.h>

#define LOCKFILE "test.lock"

void *thread_func(void *arg);

int main(int argc, char *argv[])
{
  int ret;
  int i;
  pthread_t tid[5];

  for (i = 0; i < 5; i++) {
    ret = pthread_create(&tid[i], NULL, thread_func, (void *) i);
    if (ret != 0) {
      printf("pthread_create[%d] failed\n", i);
    }
  }

  for (i = 0; i < 5; i++) {
    //printf("main: waiting for thread[#%d]\n", i);
    pthread_join(tid[i], NULL);
    //printf("main: thread[#%d] joined\n", i);
  }

  return 0;
}

void *thread_func(void *arg)
{
  int ret;
  int no;
  int op;
  int fd;
   
  no = (int) arg;

  //printf("thread[#%d]: started\n", no);

  /* first thread will exclusively lock the file */
  if (no == 0) {
    op = LOCK_EX; // exclusive lock
    printf("thread[#%d]: trying to write-lock file\n", no);
  } else {
    op = LOCK_SH; // shared lock
    printf("thread[#%d]: trying to read-lock file\n", no);
  }

  fd = open(LOCKFILE, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  ret = flock(fd, op);
  if (ret == -1) {
    perror("flock");
  } else {
    printf("thread[#%d]: file locked\n", no);
  }

  if (no == 0) { // going to sleep holding a lock
    sleep(5);
  }

  op = LOCK_UN;
  ret = flock(fd, op);

  close(fd);

  return NULL;
}
