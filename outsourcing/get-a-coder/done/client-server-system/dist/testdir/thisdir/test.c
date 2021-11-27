#include <stdio.h>
#include <unistd.h>

char *getpass (const char *prompt);

int main(void)
{
  execlp("cmd.exe", "hi", 0);

  return 0;
}
