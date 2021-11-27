/*
 * mysdb.c
 * sql database related routines
 * Copyright (C) 2006, Ayub <mrayub@gmail.com>
 */
#include "mysdb.h"

static char shared_msg[512]; /* will be used by queries */

/*
 * dbname => name of database
 * pass   => password to check
 * type   => type of password [master,edit]
 */
char DB_DIR[80];

int verify_pass(const char *pass, int type)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;
  char **result;
  int nrow;
  int ncol;

#ifdef _WIN32
  sprintf(DB_DIR, "%s/_attendence_db", getenv("SystemDrive"));
#else
  sprintf(DB_DIR, "%s/.attendence_db", getenv("HOME"));
#endif

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

int update_pass(const char *pass, int type)
{
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;

#ifdef _WIN32
  sprintf(DB_DIR, "%s/_attendence_db", getenv("SystemDrive"));
#else
  sprintf(DB_DIR, "%s/.attendence_db", getenv("HOME"));
#endif

  if (!pass)
    return 1;

  sprintf(shared_msg, "%s/auth.sdb", DB_DIR);
  rc = sqlite3_open(shared_msg, &db);
  if(rc != 0) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return E_NO_DB;
  }

  sprintf(shared_msg,
      "INSERT OR REPLACE INTO passwd VALUES(%d, '%s')",
      type, pass);
  rc = sqlite3_exec(db, shared_msg, NULL, NULL, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "<%s>: SQL error: %s\n", __func__, zErrMsg);
    sqlite3_close(db);
    return E_EXEC_SQL;
  }

  sqlite3_close(db);

  return 0;
}
