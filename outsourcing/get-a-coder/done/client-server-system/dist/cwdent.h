/*
 * cwdent.h
 */
#ifndef CWDENT_H
#define CWDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "config.h"

struct cwdent {
  uint32_t host; // network byte ordered
  char cwd[PATH_MAX+1];
};

/* add {host,cwd} to the list, return cwd */
char *cwdent_addcwd(uint32_t host, const char *cwd);
/* return the cwd associated with host */
char *cwdent_getcwd(uint32_t host);
/* update 'cwd' corresponding to 'host', return cwd */
char *cwdent_update(uint32_t host, const char *cwd);

#endif /* !CWDENT_H */
