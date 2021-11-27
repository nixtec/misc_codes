#include <stdio.h>

int main(void)
{
  char buf[256];

  while (fgets(buf, sizeof(buf), stdin)) {
    fputs(buf, stdout);
    /* if i don't flush, the data is not sent immediately through pipe :( */
    fflush(stdout);
  }

  return 0;
}
