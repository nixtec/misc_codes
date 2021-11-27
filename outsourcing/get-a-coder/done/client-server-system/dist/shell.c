/*
 * shell.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 * main file for client
 */
#include <stdio.h>
#include "functions.h"
#include "parser.h"		/* command line parser */

#define NONE		0
#define LOCAL		1
#define UPLOAD		2
#define DOWNLOAD	3
#define MOVE		4

#define FALSE		0
#define TRUE 		1


int do_copy(
    char *srcaddr,
    char *src,
    char *dstaddr,
    char *dst,
    int del); /* true/false */
int do_delete(
    char *dstaddr, /* server */
    char *path); /* filename/pattern */
int do_list(
    char *dstaddr, /* server */
    char *path); /* filename/pattern */
int do_create_folder(
    char *dstaddr, /* server */
    char *path); /* filename/pattern */
int do_delete_folder(
    char *dstaddr, /* server */
    char *path); /* filename/pattern */
int do_change_folder(
    char *dstaddr, /* server */
    char *path); /* filename/pattern */
int do_get_current_dir(char *srcaddr);
int do_handle_execute(char *srcaddr, char *src);

void prompt(void);


/* globals */
char redir_in[PATH_MAX+1];
int redir_in_size = sizeof(redir_in);
char redir_out[PATH_MAX+1];
int redir_out_size = sizeof(redir_out);

static char prompt_buf[PATH_MAX+1];

int main(int argc, char **argv)
{
  char buf[PATH_MAX*2+80];
  int del;

  char command[32];
  char src[PATH_MAX+1], dst[PATH_MAX+1];
  char srcaddr[128], dstaddr[128];

#ifdef __WIN32__
  /* initialize dll */
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
    printf("Socket Initialization Error. Program aborted\n");
    return 1;
  }
#endif

  prompt();
  while (fgets(buf, sizeof(buf), stdin)) {
    /* initialize, so that we don't get mixed with old values */
    command[0] = srcaddr[0] = src[0] = dstaddr[0] = dst[0] = redir_in[0]
      = redir_out[0] = 0;
    /* parse command line and fill the appropriate buffers */
    if (parse_command_line(buf,
	  command,
	  srcaddr, src,
	  dstaddr, dst,
	  redir_in, redir_out) != 0) {
      fprintf(stderr, "Parse error\n");
      prompt();
      continue;
    }
    fix_path(redir_in, strlen(redir_in));
    fix_path(redir_out, strlen(redir_out));

    /*
    fprintf(stderr, "command=%s\n\t"
	"srcaddr=%s\n\tsrc=%s\n\t"
	"dstaddr=%s\n\tdst=%s\n", command, srcaddr, src, dstaddr, dst);
	*/

    /* now process the commands */
    if (!strcmp(command, "copy")) {
      del = FALSE;
      do_copy(srcaddr, src, dstaddr, dst, del);
    } else if (!strcmp(command, "move")) {
      del = TRUE;
      do_copy(srcaddr, src, dstaddr, dst, del);
    } else if (!strcmp(command, "delete")) {
      do_delete(srcaddr, src);
    } else if (!strcmp(command, "list")) {
      do_list(srcaddr, src);
    } else if (!strcmp(command, "create_folder")) {
      do_create_folder(srcaddr, src);
    } else if (!strcmp(command, "delete_folder")) {
      do_delete_folder(srcaddr, src);
    } else if (!strcmp(command, "change_folder")) {
      do_change_folder(srcaddr, src);
    } else if (!strcmp(command, "pwd")) {
      do_get_current_dir(srcaddr);
    } else if (!strcmp(command, "execute")) {
      do_handle_execute(srcaddr, src);
    } else if (!strcmp(command, "exit")) {
      break;
    } else if (!strcmp(command, "")) {
      // do nothing
    } else {
      fprintf(stderr, "Unknown command: %s\n", command);
    }

    prompt();
  } /* while () */

  printf("Exiting\n");

#ifdef __WIN32__
  WSACleanup();
#endif

  return 0;
}

int do_copy(
    char *srcaddr,
    char *src,
    char *dstaddr,
    char *dst,
    int del)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  /* copy|move file1 server:file2 */
  if (!srcaddr[0] && dstaddr[0])
    type = UPLOAD;
  /* copy|move server:file1 file2 */
  else if (srcaddr[0] && !dstaddr[0])
    type = DOWNLOAD;
  /* copy|move server:file1 server:file2 */
  else if (srcaddr[0] && dstaddr[0]) {
    if (strcmp(srcaddr, dstaddr)) {
      fprintf(stderr, "src and dst servers must be the same\n");
      return -1;
    }
    type = MOVE; /* ignore dstaddr */
  }
  else
    /* copy|move file1 file2 */
    type = LOCAL; /* local copy */

  switch (type) {
    case UPLOAD:
      fix_path(src, strlen(src));
      ret = upload_file(src, dstaddr, dst, del);
      break;
    case DOWNLOAD:
      fix_path(dst, strlen(dst));
      ret = download_file(srcaddr, src, dst, del);
      break;
    case MOVE:
      ret = move_file(srcaddr, src, dst, del); /* srcaddr = dstaddr */
      break;
    case LOCAL:
      fix_path(src, strlen(src));
      fix_path(dst, strlen(dst));
      ret = copy_file(src, dst, del); /* local copy */
      break;
    default:
      break;
  }

  return ret;
}

/* delete file */
int do_delete(char *srcaddr, char *path)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  if (!srcaddr[0])
    type = LOCAL;
  else
    type = NONE;

  switch (type) {
    case NONE:
      ret = delete_file(srcaddr, path);
      break;
    case LOCAL:
      fix_path(path, strlen(path));
      ret = delete_local_file(path);
      break;
    default:
      break;
  }

  return ret;
}

/* list files */
int do_list(char *srcaddr, char *path)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  if (!srcaddr[0])
    type = LOCAL;
  else
    type = NONE;

  switch (type) {
    case NONE:
      ret = list_file(srcaddr, path);
      break;
    case LOCAL:
      fix_path(path, strlen(path));
      ret = list_local_file(path);
      break;
    default:
      break;
  }

  return ret;
}

/* create folder */
int do_create_folder(char *srcaddr, char *path)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  if (!srcaddr[0])
    type = LOCAL;
  else
    type = NONE;

  switch (type) {
    case NONE:
      ret = create_folder(srcaddr, path);
      break;
    case LOCAL:
      fix_path(path, strlen(path));
      ret = create_local_folder(path);
      break;
    default:
      break;
  }

  return ret;
}

/* delete folder */
int do_delete_folder(char *srcaddr, char *path)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  if (!srcaddr[0])
    type = LOCAL;
  else
    type = NONE;

  switch (type) {
    case NONE:
      ret = delete_folder(srcaddr, path);
      break;
    case LOCAL:
      fix_path(path, strlen(path));
      ret = delete_local_folder(path);
      break;
    default:
      break;
  }

  return ret;
}

/* change_folder */
int do_change_folder(char *srcaddr, char *path)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  if (!srcaddr[0])
    type = LOCAL;
  else
    type = NONE;

  switch (type) {
    case NONE:
      ret = change_folder(srcaddr, path);
      break;
    case LOCAL:
      fix_path(path, strlen(path));
      ret = change_local_folder(path);
      break;
    default:
      break;
  }

  return ret;
}

/* pwd */
int do_get_current_dir(char *srcaddr)
{
  int type = NONE; /* type of transfer */
  int ret = -1;

  if (!srcaddr[0])
    type = LOCAL;
  else
    type = NONE;

  switch (type) {
    case NONE:
      ret = get_current_dir(srcaddr);
      break;
    case LOCAL:
      ret = get_local_current_dir();
      break;
    default:
      break;
  }

  return ret;
}

/* execute ... */
int do_handle_execute(char *srcaddr, char *src)
{
  int ret;

  /*
  fprintf(stderr, "server=<%s>, servcmd=<%s>, redirect=<%s>\n",
      server, servcmd, redirect_out);
      */

  ret = execute_command(srcaddr, src);

  return 0;
}

void prompt(void)
{
  fprintf(stderr, "%s $ ",
      strrchr(getcwd(prompt_buf, sizeof(prompt_buf)), DIR_DELIM)+1);
  fflush(stderr);
}
