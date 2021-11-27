/*
 * cmdmap.h
 */
#ifndef CMDMAP_H
#define CMDMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

#include "config.h"		// for config file name

/* syntax of file:
 * CMD:UNIXCMD:WINCMD
 * ListDir:ls:dir
 */
#ifdef __WIN32__
#define CMD_INDEX 2
#else
#define CMD_INDEX 1
#endif

struct cmdmap {
  char *cmd; // ListDir
  char *mapcmd; // 'ls' or 'dir'
};

typedef int (*compfunc)(const void *, const void *);

void cmdmap_init(void);
const char *cmdmap_getmap(const char *cmd);
void cmdmap_destroy(void);

#endif
