/*
 * mysdb.c
 * sql database related routines
 * Copyright (C) 2006, Ayub <mrayub@gmail.com>
 */
#include "mysdb.h"

static char shared_msg[512]; /* will be used by queries */

static int init_yearly_db(void);
static int init_last_status_db(void);
/* init_fault_table will also modify last_status */
static int init_fault_table(sqlite3 *db, const char *name);
static int init_filo_table(sqlite3 *db, const char *tblname);
static int fill_fault_table(sqlite3 *db, const char *tblname);
static int fill_fault_table_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName);
static int fill_list_unmindful_today_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName);

//static int callback(void *unused, int argc, char **argv, char **azColName);
static int get_stuff_status_callback(
    void *last_stat,
    int argc,
    char **argv,
    char **azColName);
static int dump_list_callback(
    void *NotUsed,
    int argc,
    char **argv,
    char **azColName);
static int fill_list_attendence_today_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName);
static int fill_stuff_status_list_query_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName);
static int fill_filo_list_query_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName);
static int get_last_status_callback(
    void *lstatus,
    int argc,
    char **argv,
    char **azColName);

static int init_yearly_db(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  sqlite3 *db;
  char *errmsg = 0;
  char name[32];
  char *query;
  int rc;
  time_t curtime;

  sprintf(name, "%s/%s", DB_DIR, DB_YEAR);
  (void) time(&curtime);
  (void) strftime(shared_msg, sizeof(shared_msg),
		  name, localtime(&curtime));
//  printf("yearly db initialized: %s\n", shared_msg);

#ifdef ____NO____
  /* we should try creating table */
  struct stat st;
  if (stat(shared_msg, &st) == 0)
    return 0;
#endif

  rc = sqlite3_open(shared_msg, &db);
  if (rc != 0) {
    fprintf(stderr, "<%s>: sqlite3_open: %s: %s\n", __func__,
       	shared_msg, sqlite3_errmsg(db));
    return 1;
  }

  /* now create the attendence_dd_mm table */
  (void) strftime(name, sizeof(name), TBL_ATTENDENCE, localtime(&curtime));
  (void) sprintf(shared_msg,
      "CREATE TABLE %s (bcode_id TEXT NOT NULL, "
      "name TEXT NOT NULL, "
      "stat_time TEXT NOT NULL, "
      "duration INTEGER NOT NULL, "
      "status INTEGER NOT NULL)",
      name);
  query = shared_msg;
  rc = sqlite3_exec(db, query, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, errmsg);
  }
  (void) strftime(name, sizeof(name), TBL_FAULT, localtime(&curtime));
  if (init_fault_table(db, name) == 0) {
    fill_fault_table(db, name); /* it will also modify last_status table */
  }
  (void) strftime(name, sizeof(name), TBL_FILO, localtime(&curtime));
  init_filo_table(db, name);

  rc = 0;

  sqlite3_close(db);

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  return rc;
}

/* sqlite3 callback function */
/*
static int callback(void *unused, int argc, char **argv, char **azColName)
{
  return 0;
}
*/

/*
 * initialize databases
 * returns:
 *   0 => success
 *   1 => failure
 */
int init_databases(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  int ret;

  ret = init_last_status_db(); /* this must be at the first */
  if (ret != 0)
    return ret;

  ret = init_yearly_db();
  if (ret != 0)
    return ret;

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  return ret;
}


/*
 * first check if stuff exists
 * returns following:
 * E_NO_STUFF	| no such stuff |
 * E_NO_DB	| no such db |
 * E_EXEC_SQL	| error executing sql statement |
 */
int toggle_stuff_status(const char *bcode_id)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char *query;
  struct last_stat_info lstatinfo;
//  int last_stat = OUT;
  time_t curtime;
  char current_time[10]; /* HH:MM:SS */
  char name[32];
  stuff_info sinfo;

  memset(&lstatinfo, 0, sizeof(lstatinfo));
  memset(&sinfo, 0, sizeof(sinfo));

  lstatinfo.last_stat = OUT;


  if (!stuff_exists(bcode_id)) {
#ifdef _DEBUG_
    fprintf(stderr, "%s does not exist in stuff database!!!\n", bcode_id);
#endif
    return E_NO_STUFF;
  }

#ifdef ____NO____
  /* begin transaction [operation must be atomic] */
  rc = sqlite3_exec(db, "BEGIN", NULL, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }
#endif

  sprintf(shared_msg, "%s/%s", DB_DIR, DB_LAST_STAT);
  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "<%s>: Can't open database: %s\n", __func__,
       	sqlite3_errmsg(db));
    return E_NO_DB;
  }

  (void) sprintf(shared_msg,
		 "SELECT last_stat, times FROM last_status WHERE bcode_id='%s'",
		 bcode_id);
  query = shared_msg;
  rc = sqlite3_exec(db, query, get_stuff_status_callback,
      &lstatinfo, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }
  lstatinfo.last_stat ^= 1;
  (void) sprintf(shared_msg,
		 "INSERT OR REPLACE INTO last_status VALUES ('%s', %d, %d)",
		 bcode_id, lstatinfo.last_stat, (int) time(NULL));
  query = shared_msg;
  rc = sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }
  sqlite3_close(db);
  /* last_status table update complete */

//  sprintf(name, "%s/%s", DB_DIR, DB_YEAR);
  sprintf(shared_msg, "%s/db_%s.sdb", DB_DIR, get_current_year());

  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "<%s>: Can't open database: %s\n", __func__,
       	sqlite3_errmsg(db));
    return E_NO_DB;
  }

  sprintf(name, "attendence_%s_%s", get_current_day(), get_current_month());

  (void) time(&curtime);
  (void) strftime(current_time, sizeof(current_time),
		  "%H:%M:%S", localtime(&curtime));
//  fprintf(stderr, "previous duration = %d\n", lstatinfo.duration);
  if (lstatinfo.duration != 0) {
    lstatinfo.duration = (int) curtime - lstatinfo.duration;
  }
//  fprintf(stderr, "current duration = %d\n", lstatinfo.duration);
  get_stuff_info(bcode_id, &sinfo);
  if (lstatinfo.last_stat == 0) { /* going OUT */
    say_goodbye(sinfo.name);
  } else {
    say_welcome(sinfo.name);
  }
  (void) sprintf(shared_msg,
		 "INSERT INTO %s VALUES ('%s', '%s', '%s', %d, %d)",
		 name,
		 bcode_id,
		 sinfo.name ? sinfo.name : "NULL",
		 current_time,
		 lstatinfo.duration,
		 lstatinfo.last_stat);
  query = shared_msg;
  rc = sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }

  /* now update filo_dd_mm table */
  sprintf(name, "filo_%s_%s", get_current_day(), get_current_month());
  if (lstatinfo.last_stat == IN) { /* this may be the first IN */
    (void) sprintf(shared_msg,
		   "INSERT INTO %s VALUES ('%s', '%s', '%s', '%s')",
		   name,
		   bcode_id,
		   sinfo.name ? sinfo.name : "NULL",
		   current_time,
		   "N/A");
    query = shared_msg;
    rc = sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
    /* ignore errors here */
    /*
    if(rc != SQLITE_OK) {
      fprintf(stderr, "<%s>: SQL error [INSERT]: %s\n", __func__, zErrMsg);
      sqlite3_close(db);
      return E_EXEC_SQL;
    }
    */
  } else {
    (void) sprintf(shared_msg,
		   "UPDATE %s SET lout='%s' WHERE bcode_id='%s'",
		   name,
		   current_time,
		   bcode_id);
    query = shared_msg;
    rc = sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
    if(rc != SQLITE_OK) {
      fprintf(stderr, "<%s>: SQL error [UPDATE]: %s\n", __func__, zErrMsg);
      sqlite3_close(db);
      return E_EXEC_SQL;
    }
  }


#ifdef ____NO____
  /* commit changes */
  rc = sqlite3_exec(db, "COMMIT", NULL, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }
#endif

  sqlite3_close(db);
  return 0;
}

static int get_stuff_status_callback(
    void *lstatinfo,
    int argc,
    char **argv,
    char **azColName)
{
  int i;
  struct last_stat_info *linfo;
  linfo = (struct last_stat_info *) lstatinfo;

  for(i=0; i<argc; i++){
    if (!strcmp(azColName[i], "last_stat")) {
      linfo->last_stat = (int) strtol(argv[i], (char **) NULL, 10);
    } else if (!strcmp(azColName[i], "times")) {
      linfo->duration = (int) strtol(argv[i], (char **) NULL, 10);
    }
  }

  return 0;
}

/*
 *
 */
static int init_fault_table(sqlite3 *db, const char *tblname)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  char *errmsg = 0;
  char *query;
  int rc;

  /* now create the fault_dd_mm table */
  (void) sprintf(shared_msg,
      "CREATE TABLE %s (bcode_id TEXT PRIMARY KEY)",
      tblname);
  query = shared_msg;
  rc = sqlite3_exec(db, query, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, errmsg);
    rc = E_EXEC_SQL;
    goto cleanup;
  }
  rc = 0;

cleanup:

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  return rc;
}

/*
 *
 */
static int init_filo_table(sqlite3 *db, const char *tblname)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  char *errmsg = 0;
  char *query;
  int rc;

  /* now create the fault_dd_mm table */
  (void) sprintf(shared_msg,
      "CREATE TABLE %s (bcode_id TEXT PRIMARY KEY, "
      "name TEXT NOT NULL, "
      "fin TEXT NOT NULL, lout TEXT NOT NULL)",
      tblname);
  query = shared_msg;
  rc = sqlite3_exec(db, query, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, errmsg);
    rc = E_EXEC_SQL;
    goto cleanup;
  }
  rc = 0;

cleanup:

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  return rc;
}


/*
 * returns following:
 * E_NO_STUFF	| no such stuff |
 * E_NO_DB	| no such db |
 * E_EXEC_SQL	| error executing sql statement |
 */
int fill_fault_table(sqlite3 *db, const char *tblname)
{
  sqlite3 *last_db;
  char *zErrMsg = NULL;
  int rc;
  char *query;
  GSList *slist = NULL;
  GSList *temp = NULL;
  char *bcode_id;
  time_t curtime;

  sprintf(shared_msg, "%s/%s", DB_DIR, DB_LAST_STAT);
  rc = sqlite3_open(shared_msg, &last_db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(last_db));
    return E_NO_DB;
  }

  sprintf(shared_msg,
      "SELECT bcode_id FROM last_status WHERE last_stat=%d",
      IN);
  query = shared_msg;
  rc = sqlite3_exec(last_db, query, fill_fault_table_callback,
      &slist, &zErrMsg); /* fill the list with bcode_id */
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    goto cleanup;
    rc = E_EXEC_SQL;
  }
  /* the 'slist' is now filled up with bcode_id who didn't go OUT */

  temp = slist;
  while (temp) {
    bcode_id = (char *) temp->data;
    (void) sprintf(shared_msg,
		   "INSERT INTO %s VALUES ('%s')",
		   tblname, bcode_id);
    query = shared_msg;
    rc = sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
    if(rc != SQLITE_OK) {
      (void) fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
      rc = E_EXEC_SQL;
      goto cleanup;
    }
    temp = temp->next;
  }

  (void) time(&curtime);
  /* now set all last_stat to OUT [0] */
  (void) sprintf(shared_msg,
		 "UPDATE last_status SET last_stat=%d, "
		 "times=%u WHERE last_stat=%d",
		 OUT, (unsigned int) curtime, IN);
  query = shared_msg;
  rc = sqlite3_exec(last_db, query, NULL, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    (void) fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    rc = E_EXEC_SQL;
    goto cleanup;
  }

  rc = 0;

cleanup:
  sqlite3_close(last_db);
  /* free list here */
  temp = slist;
  while (temp) {
    g_free(temp->data);
    temp = temp->next;
  }
  g_slist_free(slist);

  return rc;
}

static int fill_fault_table_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName)
{
  char *bcode_id;

  bcode_id = g_strdup(argv[0] ? argv[0] : "NULL");
  /* this is the curprit
   * when the list gets long, we have to load bdb database unnecessarily
   * just for name
   */
  /*
  get_stuff_info(sstat->id, &sinfo);
  if (sinfo.name) {
    sstat->name = g_strdup(sinfo.name);
  }
  */
  *(GSList **) slist = g_slist_append(*(GSList **) slist, bcode_id);

  return 0;
}


int init_last_status_db(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  sqlite3 *db;
  char *errmsg = 0;
//  char name[32]; /* database/table name */
  char *query;
  int rc;

  sprintf(shared_msg, "%s/%s", DB_DIR, DB_LAST_STAT);
  /* skip if the database already exists */
  struct stat st;
  if (stat(shared_msg, &st) == 0)
    return 0;

  rc = sqlite3_open(shared_msg, &db);
  if (rc != 0) {
    fprintf(stderr, "sqlite3_open: %s: %s\n", shared_msg, sqlite3_errmsg(db));
    return 1;
  }

  query = "CREATE TABLE last_status (bcode_id TEXT PRIMARY KEY, "
    "last_stat INTEGER NOT NULL, times INTEGER NOT NULL)";
  /* now create the attendence_dd_mm_yyyy table */
  rc = sqlite3_exec(db, query, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, errmsg);
    rc = E_EXEC_SQL;
  } else {
    rc = 0;
  }

  sqlite3_close(db);

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  return rc;
}



int dump_list(void)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char *query;

  char name[32];
//  sprintf(name, "%s/%s", DB_DIR, DB_YEAR);
  sprintf(shared_msg, "%s/db_%s.sdb", DB_DIR, get_current_year());

  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  sprintf(name, "attendence_%s_%s", get_current_day(), get_current_month());

  (void) sprintf(shared_msg, "SELECT * FROM %s", name);
  query = shared_msg;
  rc = sqlite3_exec(db, query, dump_list_callback, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}

static int dump_list_callback(
    void *NotUsed,
    int argc,
    char **argv,
    char **azColName)
{
  int i;
  for(i=0; i<argc; i++){
    fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  return 0;
}

int fill_list_attendence_today(GSList **slist)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char *query;

  char name[32];
//  sprintf(name, "%s/%s", DB_DIR, DB_YEAR);
  sprintf(shared_msg, "%s/db_%s.sdb", DB_DIR, get_current_year());

  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  sprintf(name, "attendence_%s_%s", get_current_day(), get_current_month());

  (void) sprintf(shared_msg, "SELECT * FROM %s", name);
  query = shared_msg;
  rc = sqlite3_exec(db, query, fill_list_attendence_today_callback,
      slist, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}

static int fill_list_attendence_today_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName)
{
  int i;

//  stuff_info sinfo;
//  memset(&sinfo, 0, sizeof(sinfo));
  stuff_status *sstat = (stuff_status *) g_malloc0(sizeof(stuff_status));

  for(i=0; i<argc; i++){
//    fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if (!strcmp(azColName[i], "bcode_id"))
      sstat->id = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "name"))
      sstat->name = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "stat_time"))
      sstat->stat_time = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "duration"))
      sstat->duration = (int) strtol(argv[i] ? argv[i] : "0",
	  (char **) NULL, 10);
    else if (!strcmp(azColName[i], "status"))
      sstat->status = (int) strtol(argv[i] ? argv[i] : "0",
	  (char **) NULL, 10);
  }
  /* this is the curprit
   * when the list gets long, we have to load bdb database unnecessarily
   * just for name
   */
  /*
  get_stuff_info(sstat->id, &sinfo);
  if (sinfo.name) {
    sstat->name = g_strdup(sinfo.name);
  }
  */
  *(GSList **) slist = g_slist_append(*(GSList **) slist, sstat);

  return 0;
}

int fill_list_unmindful_today(GSList **slist)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char *query;

  char name[32];
//  sprintf(name, "%s/%s", DB_DIR, DB_YEAR);
  sprintf(shared_msg, "%s/db_%s.sdb", DB_DIR, get_current_year());

  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  sprintf(name, "fault_%s_%s", get_current_day(), get_current_month());

  (void) sprintf(shared_msg, "SELECT bcode_id FROM %s", name);
  query = shared_msg;
  rc = sqlite3_exec(db, query, fill_list_unmindful_today_callback,
      slist, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}

static int fill_list_unmindful_today_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName)
{
  char *bcode_id;

  bcode_id = g_strdup(argv[0] ? argv[0] : "NULL");
  *(GSList **) slist = g_slist_append(*(GSList **) slist, bcode_id);

  return 0;
}

int fill_stuff_status_list_query(const char *dbname, const char *qry,
    GSList **slist)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;

  if (!qry)
    return 0;

//  sprintf(shared_msg, "%s/db_%s.sdb", DB_DIR, get_current_year());

  rc = sqlite3_open(dbname, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return E_NO_DB;
  }

  rc = sqlite3_exec(db, qry, fill_stuff_status_list_query_callback,
      slist, &zErrMsg);
  if(rc != SQLITE_OK) {
//    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }

  sqlite3_close(db);
  return 0;
}

static int fill_stuff_status_list_query_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName)
{
  int i;

//  stuff_info sinfo;
//  memset(&sinfo, 0, sizeof(sinfo));
  stuff_status *sstat = (stuff_status *) g_malloc0(sizeof(stuff_status));

  for(i=0; i<argc; i++){
//    fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if (!strcmp(azColName[i], "bcode_id"))
      sstat->id = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "name"))
      sstat->name = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "stat_time"))
      sstat->stat_time = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "status"))
      sstat->status = atoi(argv[i] ? argv[i] : "0");
  }
  /* this is the curprit
   * when the list gets long, we have to load bdb database unnecessarily
   * just for name
   */
  /*
  get_stuff_info(sstat->id, &sinfo);
  if (sinfo.name) {
    sstat->name = g_strdup(sinfo.name);
  }
  */
  *(GSList **) slist = g_slist_append(*(GSList **) slist, sstat);

  return 0;
}

int fill_filo_list_query(const char *dbname, const char *qry,
    GSList **slist)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;

  if (!qry)
    return 0;

//  sprintf(shared_msg, "%s/db_%s.sdb", DB_DIR, get_current_year());

  rc = sqlite3_open(dbname, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return E_NO_DB;
  }

  rc = sqlite3_exec(db, qry, fill_filo_list_query_callback,
      slist, &zErrMsg);
  if(rc != SQLITE_OK) {
//    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }

  sqlite3_close(db);
  return 0;
}

static int fill_filo_list_query_callback(
    void *slist,
    int argc,
    char **argv,
    char **azColName)
{
  int i;

//  stuff_info sinfo;
//  memset(&sinfo, 0, sizeof(sinfo));
  stuff_status_filo *sstat =
    (stuff_status_filo *) g_malloc0(sizeof(stuff_status));

  for(i=0; i<argc; i++){
//    fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if (!strcmp(azColName[i], "bcode_id"))
      sstat->id = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "name"))
      sstat->name = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "fin"))
      sstat->fin = g_strdup(argv[i] ? argv[i] : "NULL");
    else if (!strcmp(azColName[i], "lout"))
      sstat->lout = g_strdup(argv[i] ? argv[i] : "NULL");
  }
  /* this is the curprit
   * when the list gets long, we have to load bdb database unnecessarily
   * just for name
   */
  /*
  get_stuff_info(sstat->id, &sinfo);
  if (sinfo.name) {
    sstat->name = g_strdup(sinfo.name);
  }
  */
  *(GSList **) slist = g_slist_append(*(GSList **) slist, sstat);

  return 0;
}

int get_last_status(const char *id, last_status *lstatus)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char *query;

//  char name[32];
  sprintf(shared_msg, "%s/%s", DB_DIR, DB_LAST_STAT);

  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "<%s>: Can't open database: %s\n", __func__,
       	sqlite3_errmsg(db));
    return 1;
  }

  (void) sprintf(shared_msg,
		 "SELECT * FROM last_status WHERE bcode_id='%s'", id);
  query = shared_msg;
  rc = sqlite3_exec(db, query, get_last_status_callback, lstatus, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}

static int get_last_status_callback(
    void *lstatus,
    int argc,
    char **argv,
    char **azColName)
{
  last_status *lst = lstatus;
  int i;
  for(i=0; i<argc; i++){
#ifdef _DEBUG_
    fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
#endif
    if (!strcmp("bcode_id", azColName[i])) {
      lst->bcode_id = g_strdup(argv[i]);
    }
    else if (!strcmp("last_stat", azColName[i])) {
      lst->last_stat = atoi(argv[i]);
    }
    else if (!strcmp("times", azColName[i])) {
      lst->times = atoi(argv[i]);
    }
  }
  return 0;
}

/*
 * dbname => name of database
 * pass   => password to check
 * type   => type of password [master,edit]
 */
int verify_pass(const char *pass, int type)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char **result;
  int nrow;
  int ncol;

  if (!pass)
    return 1;

  sprintf(shared_msg, "%s/auth.sdb", DB_DIR);
  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return E_NO_DB;
  }

  sprintf(shared_msg,
      "SELECT pass FROM passwd WHERE type=%d AND pass='%s'",
      type, pass);
  rc = sqlite3_get_table(db, shared_msg, &result, &nrow, &ncol, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }
  if (nrow == 0) {
#ifdef _DEBUG_
    fprintf(stderr, "nrow = 0\n");
#endif
    sqlite3_free_table(result);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_free_table(result);
  sqlite3_close(db);

  return 0;
}

/* utility functions
 * NOT reentrant
 *
 * NOTE:
 * these routines return (char *) value and NOT 'int'
 * you may convert the value as you require using 'atoi', etc.
 */
char *get_current_year(void)
{
  static char yyyy[5];
  time_t curtime;
  (void) time(&curtime);
  (void) strftime(yyyy, sizeof(yyyy), "%Y", localtime(&curtime));

  return yyyy;
}

char *get_current_month(void)
{
  static char mm[3];
  time_t curtime;
  (void) time(&curtime);
  (void) strftime(mm, sizeof(mm), "%m", localtime(&curtime));

  return mm;
}

char *get_current_day(void)
{
  static char dd[3];
  time_t curtime;
  (void) time(&curtime);
  (void) strftime(dd, sizeof(dd), "%d", localtime(&curtime));

  return dd;
}

