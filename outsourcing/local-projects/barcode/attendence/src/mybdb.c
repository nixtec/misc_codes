/*
 * mybdb.c
 * berkeley DB related routines
 * Copyright (C) 2006, Ayub <mrayub@gmail.com>
 */
#include "mybdb.h"

/* private routines */
static DB *bdb_initdb(DB **dbpp, const char *dbname);
static int bdb_insertdb(DB *dbp, char *key, void *data, size_t len,
    int confirm);
//static int bdb_deletedb(DB *dbp, char *key);
static void bdb_closedb(DB *dbp);

static char shared_name[40];

/*
 * return 0 = success
 * return 1 = failure
 * caller must NOT free data
 * */
int get_stuff_info(const char *key, stuff_info *sinfo)
{
  DB *dbp;

  int ret;
  DBT k, d;
  u_int32_t flags;
  char *buffer = NULL; /* data buffer (don't free it here) */
  int len;
  char *temp;

  flags = 0;
  memset(&k, 0, sizeof(DBT));
  memset(&d, 0, sizeof(DBT));
  k.data = (char *) key;
  k.size = strlen(key) + 1;

  sprintf(shared_name, "%s/%s", DB_DIR, STUFFDB);
  if (bdb_initdb(&dbp, shared_name) == NULL) {
    ret = 1;
    goto cleanup;
  }

  /* get information stored in database */
  if ((ret = dbp->get(dbp, NULL, &k, &d, flags)) == 0) {
#ifdef _DEBUG_
    fprintf(stderr, "db: %s: key retrieved\n", (char *) k.data);
#endif
    temp = buffer = (char *) d.data; /* raw data */
#ifdef _DEBUG_
    fprintf(stderr, "db: data size %d\n", d.size);
#endif

    sinfo->bcode_id = buffer;
    len = strlen(sinfo->bcode_id) + 1;
    sinfo->name = buffer + len; /* including NUL */
    len += strlen(sinfo->name) + 1;
    sinfo->meta = buffer + len;
    len += strlen(sinfo->meta) + 1;
    sinfo->photo = (unsigned char *) (buffer + len);
    sinfo->photo_len = d.size - len;

    ret = 0;
  }
  else {
    dbp->err(dbp, ret, "DB->get");
    ret = 1;
  }

cleanup:

  return ret;
}

/* return 0 = success
 * return 1 = failure
 */
int store_stuff_info(const stuff_info *sinfo)
{
  unsigned char *buffer;
  size_t bufsize;
  size_t buflen;
  size_t len;

  DB *dbp;
  int ret;
  int confirm = 1; /* overwrite */

  bufsize = strlen(sinfo->bcode_id)+1;
  if (sinfo->name)
    bufsize += strlen(sinfo->name)+1;
  if (sinfo->meta)
    bufsize += strlen(sinfo->meta)+1;
  if (sinfo->photo_len)
    bufsize += sinfo->photo_len;

  buffer = (unsigned char *) malloc(bufsize);

  len = strlen(sinfo->bcode_id) + 1;
  memcpy(buffer, sinfo->bcode_id, len);
  buflen = len;

  if (sinfo->name) {
    len = strlen(sinfo->name)+1;
    memcpy(buffer+buflen, sinfo->name, len);
    buflen += len;
  } else {
    *(buffer+buflen) = 0;
    buflen += 1;
  }

  if (sinfo->meta) {
    len = strlen(sinfo->meta)+1;
    memcpy(buffer+buflen, sinfo->meta, len);
    buflen += len;
  } else {
    *(buffer+buflen) = 0;
    buflen += 1;
  }

  if (sinfo->photo && (sinfo->photo_len > 0)) {
    len = sinfo->photo_len;
    memcpy(buffer+buflen, sinfo->photo, len);
    buflen += len;
  }

  (void) sprintf(shared_name, "%s/%s", DB_DIR, STUFFDB);
  if (bdb_initdb(&dbp, shared_name) == NULL) {
    ret = 1;
    goto cleanup;
  }

  /* now store the data to database */
#ifdef _DEBUG_
  fprintf(stderr, "db: storing %d bytes\n", buflen);
#endif
  ret = bdb_insertdb(dbp, sinfo->bcode_id, buffer, buflen, confirm);

  bdb_closedb(dbp);

cleanup:
  free(buffer);
  return ret;
}
/* check if stuff exists
 * returns TRUE [1] if exists
 * returns FALSE [0] otherwise
 */
int stuff_exists(const char *key)
{
  int ret;
  DB *dbp;
  DBT k, d;
  u_int32_t flags = 0;

  flags = 0;
  memset(&k, 0, sizeof(DBT));
  memset(&d, 0, sizeof(DBT));
  k.data = (char *) key;
  k.size = strlen(key)+1;

  (void) sprintf(shared_name, "%s/%s", DB_DIR, STUFFDB);
  if (bdb_initdb(&dbp, shared_name) == NULL)
    return 0;

  /* get information stored in database */
  if ((ret = dbp->get(dbp, NULL, &k, &d, flags)) == 0) {
#ifdef _DEBUG_
    fprintf(stderr, "db: %s: key retrieved\n", (char *) k.data);
#endif
    ret = 1; /* found */
  }
  else {
    dbp->err(dbp, ret, "DB->get");
    ret = 0;
  }

  bdb_closedb(dbp);

  return ret;
}


/* private routines */
static DB *bdb_initdb(DB **dbpp, const char *dbname)
{
  int ret;
  DB *dbp;

  if ((ret = db_create(dbpp, NULL, 0)) != 0) {
    fprintf(stderr, "db_create: %s\n", db_strerror(ret));
    return (NULL);
  }
  dbp = *dbpp;

  if ((ret = dbp->open(dbp, NULL, dbname, NULL, DB_BTREE, DB_CREATE,
	  REG_CREATE_MODE)) != 0) {
    dbp->err(dbp, ret, "%s", dbname);
    return (NULL);
  }

  return (*dbpp);
}

/* 'data' must be pre filled up */
/* len => size of data */
static int bdb_insertdb( DB *dbp, char *key, void *data, size_t len,
    int confirm)
{
  DBT k, d; /* key/data */
  int ret;
  u_int32_t flags = 0;

  /* now store the data */
  memset(&k, 0, sizeof(DBT));
  memset(&d, 0, sizeof(DBT));
  k.data = key;
  k.size = strlen(key)+1;
  d.data = data;
  d.size = len;


  if (confirm)
    flags = 0;
  else
    flags = DB_NOOVERWRITE;

  switch (ret = dbp->put(dbp, NULL, &k, &d, flags)) {
    case 0:
#ifdef _DEBUG_
      fprintf(stderr, "db: %s: key stored.\n", (char *) k.data);
#endif
      break;
    case DB_KEYEXIST:
#ifdef _DEBUG_
      fprintf(stderr, "db: %s: key already present!\n", (char *) k.data);
#endif
      break;
    default:
      dbp->err(dbp, ret, "DB->put");
      break;
  }

  return ret;
}

static void bdb_closedb(DB *dbp)
{
  dbp->close(dbp, 0);
}



#ifdef ____NO____
/* delete key/data from dbp */
static int bdb_deletedb(DB *dbp, char *key)
{
  int ret;
  DBT k;
  u_int32_t flags;

  flags = 0;

  memset(&k, 0, sizeof(DBT));
  k.data = key;
  k.size = strlen(key)+1;

  if ((ret = dbp->del(dbp, NULL, &k, flags)) == 0) {
#ifdef _DEBUG_
    fprintf(stderr, "db: %s: key deleted.\n", (char *) k.data);
#endif
  } else {
    dbp->err(dbp, ret, "DB->del");
  }

  return ret;
}
#endif

