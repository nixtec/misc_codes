/*
 * exec_cmd.winx.c
 * windows version exec_cmd()
 */
#include "exec_cmd.h"

#ifdef __NO__
/*
 * INFORMATION
 * Under Windows NT and Windows 2000, socket handles are inheritable by
 * default. This feature is often used by a process that wants to spawn a
 * child process and have the child process interact with the remote
 * application on the other end of the connection.
 *
 * It is also common practice on Windows NT to set the standard handles
 * (standard input, output, or error) of the child process to the socket
 * handle. In such cases, the child process usually does not know that its
 * standard handles are actually sockets.
 *
 * Windows 9x differs from Windows NT/Windows 2000 in the following manner:
 * •	Socket handles are not inheritable when created. To ensure that a
 * 	child process can obtain and use a socket handle created in the parent,
 * 	the handle must be explicitly duplicated using the Win32 API
 * 	DuplicateHandle. Set the bInheritHandle parameter of the API to TRUE.
 * •	Socket handles cannot be set to the standard handles of the child
 * 	process. A programmer may use other mechanisms to pass the socket
 * 	handle to the client, such as passing the handle values as command
 * 	line arguments so that the child process can simply look at its
 * 	argument vector.
 */

/* but unfortunately Windows doesn't work as they say above */
 
void exec_cmd(int connfd,
    const char *cwd,
    const char *shell,
    const char *arg,
    const char *cmd,
    uid_t uid)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  const char *wd;
  char cmdbuf[256];

  HANDLE dupsock;

  if (cwd[0] != '-')
    wd = cwd;
  else
    wd = NULL;

  /*
  if (!DuplicateHandle(GetCurrentProcess(),
	(HANDLE) connfd,
	GetCurrentProcess(),
	&dupsock,
	0,
	TRUE, // inheritable
	DUPLICATE_SAME_ACCESS)) {
    fprintf(stderr, "dup error %d!\n", (int) GetLastError());
    return;
  }
  */

  dupsock = (HANDLE) connfd;
  sprintf(cmdbuf, "%s %s %s", shell, arg, cmd);

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(STARTUPINFO); 
  si.hStdError = dupsock;
  si.hStdOutput = dupsock;
  si.hStdInput = dupsock;
  si.dwFlags |= STARTF_USESTDHANDLES;

  //fprintf(stderr, "cmdbuf=%s\n", cmdbuf);
  if (!CreateProcess(NULL,
       	cmdbuf,			// command line 
	NULL,			// process security attributes 
	NULL,			// primary thread security attributes 
	TRUE,			// handles are inherited 
	0,			// creation flags 
	NULL,			// use parent's environment 
	wd,			// working directory 
	&si,			// STARTUPINFO pointer 
	&pi)) {			// receives PROCESS_INFORMATION 
    fprintf(stderr, "CreateProcess failed: %d\n", (int) GetLastError());
    return;
  } else {
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    //CLOSE((SOCKET) dupsock);
  }
}
#endif /* __NO__ */

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
  /*
   * p[0]: parent -> child
   * p[0][0]: child will read
   * p[0][1]: parent will write
   */
  if (!CreatePipe(&p[0][0], &p[0][1], &saAttr, 0)) {
    fprintf(stderr, "CreatePipe failed: %d\n", (int) GetLastError());
    p[0][0] = p[0][1] = NULL;
    goto cleanup;
  }
  // Ensure the write handle to the pipe for STDIN is not inherited. 
  SetHandleInformation(p[0][1], HANDLE_FLAG_INHERIT, 0);

  /*
   * p[1]: child -> parent
   * p[1][0]: parent will read
   * p[1][1]: child will write
   */
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
    strcat(cmdline, cmd);
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

  CloseHandle(p[0][0]);
  CloseHandle(p[1][1]);
  p[0][0] = p[1][1] = NULL;

  HANDLE read_thread = NULL; // thread will read data from child
  thread_data td;
  memset(&td, 0, sizeof(td));

  td.sock = connfd;
  td.h = p[0][1];
  read_thread = CreateThread(NULL, 0, read_func, &td, 0, NULL);
  if (read_thread == NULL) {
    fprintf(stderr, "Error creating receiver thread: %d\n",
       	(int) GetLastError());
    goto cleanup;
  }
  p[0][1] = NULL; // thread will close the handle

  DWORD dwRead; 
  CHAR chBuf[BUFSIZE]; 

  while (1) {
    if(!ReadFile(p[1][0], chBuf, BUFSIZE, &dwRead, NULL) || dwRead == 0) {
      break;
    }
    i = sendn(connfd, chBuf, dwRead);
    if (i <= 0) {
      fprintf(stderr, "Error writing data to socket\n");
      break;
    }
  }
  shutdown(connfd, SHUT_WR);

  //fprintf(stderr, "Waiting for thread to terminate\n");
  /* not perfect way, but works :-) */
  //TerminateThread(read_thread, 0);
  WaitForSingleObject(read_thread, INFINITE); // wait for thread
  CloseHandle(read_thread);
  //fprintf(stderr, "Thread terminated\n");

cleanup:
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      if (p[i][j] != NULL)
	CloseHandle(p[i][j]);
    }
  }

  if (child != NULL) {
    //fprintf(stderr, "Waiting for child\n");
    WaitForSingleObject(child, INFINITE);
    //fprintf(stderr, "Child exited\n");
    CloseHandle(child); /* we don't need this */
  }
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

  //fprintf(stderr, "cmdline: %s\n", carg->cmd);
  bFuncRetn = CreateProcess(NULL, 
      (char *) carg->cmd,	// command line 
      NULL,			// process security attributes 
      NULL,			// primary thread security attributes 
      TRUE,			// handles are inherited 
      0,			// creation flags 
      NULL,			// use parent's environment 
      carg->wd,			// working directory 
      &siStartInfo,		// STARTUPINFO pointer 
      &piProcInfo);		// receives PROCESS_INFORMATION 

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
  DWORD dwWritten; 
  CHAR chBuf[BUFSIZE]; 
  int ret;
  thread_data *ptr = (thread_data *) lpData;
  int sock = ptr->sock;
  HANDLE h = ptr->h;

  while ((ret = recv(sock, chBuf, sizeof(chBuf), 0)) > 0) {
    if (!WriteFile(h, chBuf, ret, &dwWritten, NULL)) {
      break;
    }
    //fprintf(stderr, "%d bytes written to pipe\n", (int) dwWritten);
  }
  CloseHandle(h);

  //fprintf(stderr, "Exiting from thread\n");

  ExitThread(0);
}
