/*
 * proc-exec.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 */
#include "server-funcs.h"
#include "exec_cmd.h"

#ifdef linux
#include <strings.h>
#include "authenticate.h"
#endif

#ifdef linux
int authenticate_client(int connfd, char *shell, uid_t *uid);
#endif

#ifdef __NO__
int parse_command_line(const char *line, struct cmd **cmdp);
#endif

int fix_command_line(const char *line, char *cmdp);

/* 500 means success */
/* please read error-codes.txt for values used in protocol */

/* please read 'protocol.txt' for understanding the protocol used */
/* handle 'execute' */

#define MAX_CMDLINE 256
#define CMD_NOSUPPORT -256
int proc_exec(int connfd, const char *buf)
{
//  fprintf(stderr, "<%s>\n", __func__);

  int ret = -1;
  int n;
  uid_t uid = 0; // in Windows this will be ignored

  char buffer[512];
  char cs[MAX_CMDLINE]; /* command to execute */
  char cwd[PATH_MAX+1];
  char shell_path[PATH_MAX+1]; // full path of shell

  (void) uid;

  /* <cwd>:<cmd> */
  n = strcspn(buf, DELIM); /* DELIM is #define'd in config.h */
  strncpy(cwd, buf, n);
  cwd[n] = 0;
  n++; /* skip the DELIM */
  buf += n; /* buf now points to <cmd> */

  printf("<%s:%d> cwd=<%s> cmd=%s", __FILE__, __LINE__, cwd, buf);

#ifdef linux
  uid = geteuid();
  if (uid != 0) { // not root, authentication is NOT possible [/etc/shadow]
    sprintf(buffer, "400 Server NOT excuted as root. Execution denied!\n");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    goto cleanup;
  }

  if (authenticate_client(connfd, shell_path, &uid) != 0) {
    goto cleanup;
  }
#else /* windows */
  strcpy(shell_path, global_shell_path);
#endif

//  fprintf(stderr, "%s", buf);
  if ((ret = fix_command_line(buf, cs)) >= 0) {
    sprintf(buffer, "500 OK\n"); /* command supported */
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n) {
      perror("sendn");
      goto cleanup;
    }
  } else if (ret == CMD_NOSUPPORT) {
    /* command not supported */
    sprintf(buffer, "400 Command execution not supported\n");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n) {
      perror("sendn");
    }
    goto cleanup;
  } else {
    /* command not supported */
    sprintf(buffer, "400 Command parsing error\n");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n) {
      perror("sendn");
    }
    goto cleanup;
  }

  //fprintf(stderr, "proc-exec.c: shell:\n<%s>\n", shell_path);

#ifdef linux
  exec_cmd(connfd, cwd, shell_path, "-c", cs, uid);
#else
  exec_cmd(connfd, cwd, shell_path, "/c", cs, uid);
#endif

  ret = 0;
cleanup:

  return ret;
}

/*
 * return -256 to indicate command not supported
 */
int fix_command_line(const char *cmdline, char *cmdp)
{
  int i, j;
  int ch;
  int special_retval = 0;
  int find_map = 1; // map finding should be started
  int safe = 0;

  char tmpbuf[256];
  const char *ptr = NULL;

  i = j = 0;
  while (1) {
    ch = *cmdline++;

    if (find_map) {
      if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\0') {
	if (safe) {
	  tmpbuf[j] = '\0';

	  ptr = cmdmap_getmap(tmpbuf);
	  if (!ptr) {
	    special_retval = CMD_NOSUPPORT;
	    break;
	  }
	  j = strlen(ptr);
	  memcpy(cmdp+i, ptr, j); // strncat
	  i += j;
	  cmdp[i++] = ch; // append the current character read
	  j = 0;
	  find_map = 0;
	  safe = 0;
	} else { // not safe
	  cmdp[i++] = ch;
	}
      } else {
	tmpbuf[j++] = ch;
	safe = 1;
      }
    } else {
      cmdp[i++] = ch;
    }

    if (ch == '\0') {
      break;
    } else if (ch == '|') {
      find_map = 1; // next command should be searched for mapping
    }
  }

  if (cmdp[i-2] == '\n') { // -2 is because we copied '\0' too
    cmdp[i-2] = '\0';
  }

  if (special_retval != 0)
    return special_retval;

  return 0;
}

#ifdef linux
//static int auth_success = 0;

int authenticate_client(int connfd, char *shell, uid_t *uid)
{
//  fprintf(stderr, "<%s>\n", __func__);

  int ret;
  int n;

  char buffer[100];
  char username[40];
  char password[40];

  sprintf(buffer, "300 Login: \n");
  n = strlen(buffer);
  ret = sendn(connfd, buffer, n);
  if (ret < n) {
    DEBUG("sendn", errno);
    ret = -1;
    goto cleanup;
  }

  n = recvline(connfd, buffer, sizeof(buffer));
  if (buffer[n-1] == '\n')
    buffer[n-1] = 0; /* disard newline */
  strcpy(username, buffer);

  struct passwd pd;
  struct passwd *pwdptr = &pd;
  struct passwd *tempPwdPtr = NULL;
  char pwdbuf[200]; // adjust size according to your need
  int pwdlinelen = sizeof(pwdbuf);

  //errno = 0;
  ret = getpwnam_r(username, pwdptr, pwdbuf, pwdlinelen, &tempPwdPtr);
  if (tempPwdPtr == NULL) { // not found or error
    if (ret == 0) { // not found
      sprintf(buffer, "200 No such user '%s'!\n", username);
    } else {
      fprintf(stderr, "getpwnam_r: %s\n", strerror(ret));
      sprintf(buffer, "200 No such user '%s'!\n", username);
    }
    n = strlen(buffer);
    sendn(connfd, buffer, n);
    ret = -1;
    goto cleanup;
  }
  *uid = pd.pw_uid;
  strcpy(shell, pd.pw_shell);

  sprintf(buffer, "350 Password: \n");
  n = strlen(buffer);
  ret = sendn(connfd, buffer, n);
  if (ret < n) {
    DEBUG("sendn", errno);
    ret = -1;
    goto cleanup;
  }

  n = recvline(connfd, buffer, sizeof(buffer));
  if (buffer[n-1] == '\n')
    buffer[n-1] = 0; /* disard newline */
  strcpy(password, buffer);
  ret = authenticate(username, password);
  if (ret == UPAP_AUTHACK) { // successful ack
    sprintf(buffer, "100 Login Successful!\n");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    ret = 0;
  } else {
    sprintf(buffer, "200 Login Failure!\n");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    ret = -1;
  }

cleanup:
  return ret;
}

#endif /* linux */
