/*
 * getpass.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 * implementation of getpass() for Windows
 */

#ifdef __WIN32__

#include <stdio.h>
#include <string.h>
#include <limits.h>			/* For PASS_MAX. */
#include <conio.h>

#ifndef PASS_MAX
#define PASS_MAX 512
#endif

/* unix style getpass() for Windows */
char *getpass (const char *prompt)
{
  static char getpassbuf[PASS_MAX + 1]; // static buffer
  size_t i = 0;
  int c;

  if (prompt) {
    fputs(prompt, stderr);
    fflush(stderr);
  }

  while (1) {
    c = _getch();
    if (c == '\r') { // ENTER
      getpassbuf[i] = '\0';
      break;
    }
    else if (c == '\b') { // backspace
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

  return getpassbuf;
}

#endif /* __WIN32__ */
