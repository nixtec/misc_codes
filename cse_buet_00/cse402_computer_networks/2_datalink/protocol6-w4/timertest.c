#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>


void handler(int signo);

int main(void)
{
  int ret;
  struct itimerval value;
  memset(&value, 0, sizeof(value));

  signal(SIGALRM, handler);
  signal(SIGINT, handler);

  value.it_interval.tv_usec = 100000;
  value.it_interval.tv_sec = 0;
  value.it_value.tv_usec = 100000;
  value.it_value.tv_sec = 0;

  ret = setitimer(ITIMER_REAL, &value, NULL);
  if (ret < 0) {
    perror("setitimer");
    return 2;
  }

  while (1) {
    pause();
  }

  return 0;
}

void handler(int signo)
{
  printf("signal handled : %d\n", signo);
  if (signo == SIGINT) {
    printf("Interrupted\n");
    exit(signo);
  }
  return;
}
