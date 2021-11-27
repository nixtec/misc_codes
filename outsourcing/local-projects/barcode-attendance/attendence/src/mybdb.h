/*
 * mybdb.h
 * berkeley DB related routines
 */
#ifndef _MYBDB_H_
#define _MYBDB_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>

#ifdef _WIN32
#include <win_db.h>
#endif
#include <db.h>



#define REG_CREATE_MODE 0644

/* don't include trailing slash here */
extern char DB_DIR[];

#define STUFFDB "stuff.bdb"

typedef struct {
  char *bcode_id;
  char *name;
  char *meta;
  unsigned char *photo;
  size_t photo_len; /* size of photo buffer */
} stuff_info;

extern int stuff_exists(const char *bcode_id);
extern int store_stuff_info(const stuff_info *sinfo);
extern int get_stuff_info(const char *bcode_id, stuff_info *sinfo);

#endif
