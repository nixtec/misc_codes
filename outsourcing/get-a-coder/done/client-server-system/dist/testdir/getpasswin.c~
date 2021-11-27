/* Windows implementation by Martin Lambers <marlam@marlam.de>,
   improved by Simon Josefsson. */

#ifdef __WIN32__

#include <stdio.h>
#include <string.h>
#include <limits.h>			/* For PASS_MAX. */
#include <conio.h>

#ifndef PASS_MAX
# define PASS_MAX 512
#endif

char *getpass (const char *prompt)
{
  char getpassbuf[PASS_MAX + 1];
  size_t i = 0;
  int c;

  if (prompt) {
    fputs(prompt, stderr);
    fflush(stderr);
  }

  for (;;) {
    c = _getch();
    if (c == '\r') {
      getpassbuf[i] = '\0';
      break;
    }
    else if (c == '\b') {
       if (i > 0)
	  i--;
    }
    else if (i < PASS_MAX) {
      getpassbuf[i++] = c;
      //fputc('*', stderr);
    }

    if (i >= PASS_MAX) {
      getpassbuf[i] = '\0';
      break;
    }
  }

  if (prompt) {
    fputs ("\r\n", stderr);
    fflush (stderr);
  }

  /* caller will have to free it */
  return strdup(getpassbuf);
}
#endif
