#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void)
{
  char path[PATH_MAX+1];

  path[PATH_MAX] = 0;
  char *p = realpath("../../../", path);
  if (p) {
    fprintf(stderr, "%s\n", path);
  }

  return 0;
}
