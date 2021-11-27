/*
 * exec_cmd.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 */
#include "exec_cmd.h"

void exec_cmd(int connfd,
    const char *cwd,
    const char *shell,
    const char *arg,
    const char *cmd,
    uid_t uid)
{
  pid_t pid;
  int status;
  const char *cp;

  if ((pid = fork()) == 0) {
#ifdef linux
    if (geteuid() == 0) { // root
      if (setuid(uid)) { // run as user uid
	perror("setuid");
	exit(1); // not safe to execute
      }
    }
#endif
    if (cwd[0] != '-') {
      chdir(cwd);
    }
    cp = strrchr(shell, DIR_DELIM);
    if (cp)
      cp++;
    else
      cp = shell;
    dup2(connfd, STDIN_FILENO);
    dup2(connfd, STDOUT_FILENO);
    dup2(connfd, STDERR_FILENO);
    close(connfd);
    execl(shell, cp, arg, cmd, NULL);
    exit(1); /* if we reach here, there's something wrong */
  }

  if (pid > 0) {
    if (waitpid(pid, &status, 0) == pid) {
      fprintf(stderr, "Child [%d] exited with status %d\n", (int) pid, status);
    } else {
      TRACE_LINE();
      perror("waitpid");
    }
  }
}
