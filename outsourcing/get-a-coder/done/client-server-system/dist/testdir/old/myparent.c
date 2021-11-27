/*
 * parent.c
 */
#include <windows.h> 
#include <stdio.h> 
 
#define BUFSIZE 4096 
#define MAX_CMDLINE 4096
 
typedef struct {
  const char *cmd; // command-line
  const char *wd; // working directory
  HANDLE std_in;
  HANDLE std_out;
  HANDLE std_err;
} childarg;

typedef struct {
  int sock;
  HANDLE h;
} thread_data;
 
HANDLE CreateChildProcess(const childarg *carg); 
DWORD WINAPI read_func(LPVOID lpData);
void exec_cmd(int connfd,
    const char *cwd,
    const char *shell,
    const char *arg,
    const char *cmd,
    uid_t uid);

int main(int argc, char *argv[]) 
{
} 
 

void exec_cmd(int connfd,
    const char *cwd,
    const char *shell,
    const char *arg,
    const char *cmd,
    uid_t uid)
{
  int i, j;
  HANDLE p[2][2];
  HANDLE child = NULL;
  childarg ca;
  SECURITY_ATTRIBUTES saAttr; 
  char cmdline[MAX_CMDLINE];

  // Set the bInheritHandle flag so pipe handles are inherited. 
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
  saAttr.bInheritHandle = TRUE; 
  saAttr.lpSecurityDescriptor = NULL; 

  p[0][0] = p[0][1] = p[1][0] = p[1][1] = NULL;
  // p[0]: parent -> child
  if (!CreatePipe(&p[0][0], &p[0][1], &saAttr, 0)) {
    fprintf(stderr, "CreatePipe failed: %d\n", (int) GetLastError());
    p[0][0] = p[0][1] = NULL;
    goto cleanup;
  }
  // Ensure the write handle to the pipe for STDIN is not inherited. 
  SetHandleInformation(p[0][1], HANDLE_FLAG_INHERIT, 0);

  // p[1]: child -> parent
  if (!CreatePipe(&p[1][0], &p[1][1], &saAttr, 0))  {
    fprintf(stderr, "CreatePipe failed: %d\n", (int) GetLastError());
    p[1][0] = p[1][1] = NULL;
    goto cleanup;
  }
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  SetHandleInformation(p[1][0], HANDLE_FLAG_INHERIT, 0);

  // Now create the child process. 
  memset(&ca, 0, sizeof(ca)); // initialize
  cmdline[0] = 0; // initialize
  if (shell) {
    strcpy(cmdline, shell);
    strcat(cmdline, " ");
  }
  if (arg) {
    strcat(cmdline, arg);
    strcat(cmdline, " ");
  }
  if (cmd) {
    strcat(cmdline, arg);
  }
  ca.cmd = cmdline;
  if (cwd[0] != '-') {
    ca.wd = cwd;
  }
  ca.std_in = p[0][0]; /* parent will write to p[0][1] */
  ca.std_out = p[1][1]; /* parent will read from p[1][0] */
  ca.std_err = p[1][1]; /* parent will read from p[1][0] */

  child = CreateChildProcess(&ca);
  if (child == NULL) {
    fprintf(stderr, "CreateProcess failed: %d\n", (int) GetLastError());
    goto cleanup;
  }

  HANDLE read_thread = NULL; // thread will read data from child
  thread_data td;
  memset(&td, 0, sizeof(td));

  td.sock = connfd;
  td.h = p[1][0];

  read_thread = CreateThread(NULL, 0, read_func, &td, 0, NULL);
  if (read_thread == NULL) {
    fprintf(stderr, "Error creating receiver thread: %d\n",
       	(int) GetLastError());
    goto cleanup;
  }

  DWORD dwWritten; 
  CHAR chBuf[BUFSIZE]; 

  while ((i = recv(connfd, chBuf, sizeof(chBuf), 0)) > 0) {
    if (i <= 0) { // no more read from socket
      /* send EOF to the child */
      CloseHandle(p[0][1]);
      p[0][1] = NULL;
      break;
    }
    if (!WriteFile(p[0][1], chBuf, i, &dwWritten, NULL)) {
      break;
    }
    fprintf(stderr, "%d bytes written to pipe\n", dwWritten);
  } 

  /*
  if (TerminateThread(read_thread, 0) == 0) {
    fprintf(stderr, "Error TerminateThread: error=%d\n",
	(int) GetLastError());
  }
  */

  WaitForSingleObject(read_thread, INFINITE); // wait for thread
  CloseHandle(read_thread);

cleanup:
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      if (p[i][j] != NULL)
	CloseHandle(p[i][j]);
    }
  }

  if (child != NULL) {
    WaitForSingleObject(child, INFINITE);
    CloseHandle(child); /* we don't need this */
  }

  return 0; 
}

/* creates child and returns the child process handle */
HANDLE CreateChildProcess(const childarg *carg) 
{
  PROCESS_INFORMATION piProcInfo; 
  STARTUPINFO siStartInfo;
  BOOL bFuncRetn = FALSE; 

  // Set up members of the PROCESS_INFORMATION structure. 

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  // Set up members of the STARTUPINFO structure. 

  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO); 
  siStartInfo.hStdError = carg->std_err;
  siStartInfo.hStdOutput = carg->std_out;
  siStartInfo.hStdInput = carg->std_in;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process. 

  bFuncRetn = CreateProcess(NULL, 
      arg,           // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      carg->wd,      // working directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 

  if (bFuncRetn == 0) {
    fprintf(stderr, "CreateProcess failed: %d\n", (int) GetLastError());
    return NULL;
  } else {
    CloseHandle(piProcInfo.hThread); /* we don't need this */
  }
  return piProcInfo.hProcess;
}
 
DWORD WINAPI read_func(LPVOID lpData)
{
  DWORD dwRead; 
  CHAR chBuf[BUFSIZE]; 
  int ret;
  thread_data *ptr = (thread_data *) lpData;
  int sock = ptr->sock;
  HANDLE h = ptr->h;

  while (1) {
    if(!ReadFile(h, chBuf, BUFSIZE, &dwRead, NULL) || dwRead == 0) {
      break;
    }
    ret = sendn(sock, chBuf, dwRead);
    if (ret <= 0) {
      fprintf(stderr, "Error writing data to socket\n");
      break;
    }
  }
  ExitThread(0);
}
