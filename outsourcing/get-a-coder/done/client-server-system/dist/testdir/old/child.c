/*
 * child.c
 */
#include <windows.h> 
#include <stdio.h>

#define BUFSIZE 4096 
 
int main(int argc, char **argv) 
{ 
  CHAR chBuf[BUFSIZE]; 
  DWORD dwRead, dwWritten; 
  HANDLE hStdin, hStdout; 
  BOOL fSuccess; 

  fprintf(stderr, "I am child, entered\n");
  fflush(stderr);

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
  hStdin = GetStdHandle(STD_INPUT_HANDLE); 
  if ((hStdout == INVALID_HANDLE_VALUE) || 
      (hStdin == INVALID_HANDLE_VALUE))
    ExitProcess(1); 

  for (;;) 
  { 
    // Read from standard input. 
    fSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL); 
    if (! fSuccess || dwRead == 0) 
      break; 

    // Write to standard output. 
    fSuccess = WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL); 
    if (! fSuccess) 
      break; 
  } 

  fprintf(stderr, "I am child, exiting\n");
  fflush(stderr);

  return 0;
}
