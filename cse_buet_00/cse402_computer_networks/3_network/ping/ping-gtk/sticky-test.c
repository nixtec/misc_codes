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
  printf("Before setting uid\n");
  printf("uid = %d\n", getuid());
  printf("euid = %d %s\n", geteuid(), (geteuid() == 0) ? "superuser!" : "");
  printf("gid = %d\n", getgid());
  printf("egid = %d\n", getegid());
  printf("Setting uid to current user id\n");
  setuid(getuid());
  printf("After setting uid\n");
  printf("uid = %d\n", getuid());
  printf("euid = %d\n", geteuid());
  printf("gid = %d\n", getgid());
  printf("egid = %d\n", getegid());

  return 0;
}
