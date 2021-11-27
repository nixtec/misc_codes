/*
 * mysdb.h
 * sql related routines
 */
#ifndef _MYSDB_H_
#define _MYSDB_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sqlite3.h>	/* sqlite3 specific features */

#include <glib.h>	/* for GSList */

#include "myfuncs.h"	/* say_xxx() routines */
#include "mybdb.h"	/* get_stuff_info */

#ifdef IN
#undef IN
#endif
#define IN  1
#ifdef OUT
#undef OUT
#endif
#define OUT 0

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DB_YEAR "db_%Y.sdb"
#define DB_LAST_STAT "last_status.sdb"
#define TBL_ATTENDENCE "attendence_%d_%m"
#define TBL_FAULT "fault_%d_%m"
#define TBL_FILO "filo_%d_%m"

/* errors to be returned */
#define E_NO_STUFF	1	/* no such stuff */
#define E_NO_DB		2	/* no such database */
#define E_EXEC_SQL	3	/* error executing sql statement */

#define TYPE_PASS_ADMIN	1	/* password administration */
#define TYPE_PASS_ENTRY	2	/* entry login */
#define TYPE_PASS_ADD	3	/* add stuff */
#define TYPE_PASS_EDIT	4	/* edit stuff */

typedef struct {
  char *id;
  char *name;
  char *stat_time;
  int duration; /* how long the stuff was out/in */
  int status; /* 0 = out, 1 = in */
} stuff_status;

typedef struct {
  char *id;
  char *name;
  char *fin;
  char *lout;
} stuff_status_filo;

typedef struct {
  char *bcode_id;
  int last_stat;
  int times;
} last_status;

struct last_stat_info {
  int last_stat;
  int duration;
};

extern int init_databases(void);
extern int toggle_stuff_status(const char *bcode_id);
extern char *get_current_year(void);
extern char *get_current_month(void);
extern char *get_current_day(void);
extern int dump_list(void); /* dump attendence_dd_mm table */
extern int fill_list_unmindful_today(GSList **slist);
extern int fill_list_attendence_today(GSList **slist);
extern int fill_stuff_status_list_query(const char *dbname, const char *qry,
    GSList **list);
extern int fill_filo_list_query(const char *dbname, const char *qry,
    GSList **list);
extern int get_last_status(const char *id, last_status *lstatus);
extern int verify_pass(const char *pass, int type);


#define Free(x) if (x) free(x)


#endif