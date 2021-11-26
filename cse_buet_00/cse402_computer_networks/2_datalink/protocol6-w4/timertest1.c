#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

static void int_handler(int signo)
{
  fprintf(stderr, "Interrupted\n");
  exit(1);
}

int main(int argc, char **argv)
{
  int ret;

  int signo;
  sigset_t newset;
  static struct itimerval value;

  sigemptyset(&newset);
  sigaddset(&newset, SIGALRM);
  pthread_sigmask(SIG_BLOCK, &newset, NULL);

  signal(SIGINT, int_handler);

  value.it_interval.tv_usec = 100000;
  value.it_value.tv_usec = 100000;

  ret = setitimer(ITIMER_REAL, &value, NULL);
  if (ret < 0) {
    perror("setitimer");
    return 2;
  }

  while (1) {
    sigwait(&newset, &signo);
    if (signo == SIGALRM) {
      fprintf(stderr, "Alarmed\n");
    }
  }

  return 0;
}
