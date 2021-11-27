/*
 * mysdb.h
 * sql related routines
 */
#ifndef _MYSDB_H_
#define _MYSDB_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sqlite3.h>	/* sqlite3 specific features */

/* errors to be returned */
#define E_NO_STUFF	1	/* no such stuff */
#define E_NO_DB		2	/* no such database */
#define E_EXEC_SQL	3	/* error executing sql statement */

#define TYPE_PASS_ADMIN	1	/* password administration */
#define TYPE_PASS_ENTRY	2	/* entry login */
#define TYPE_PASS_ADD	3	/* add stuff */
#define TYPE_PASS_EDIT	4	/* edit stuff */

extern int verify_pass(const char *pass, int type);
extern int update_pass(const char *pass, int type);

#endif
