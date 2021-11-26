/*
 * sticky-test.c
 * gcc -o sticky-test sticky-test.c
 * sudo chown root:root sticky-test
 * sudo chmod 4755 sticky-test
 * ./sticky-test
 */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  printf("uid = %d\n", getuid());
  printf("euid = %d\n", geteuid());
  setuid(getuid());
  printf("uid = %d\n", getuid());
  printf("euid = %d\n", geteuid());

  return 0;
}
