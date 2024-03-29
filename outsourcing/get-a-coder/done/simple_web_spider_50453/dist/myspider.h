/*
 * myspider.h
 */
#ifndef MYSPIDER_H
#define MYSPIDER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "mygetlinks.h"
#include "myregex.h"
#include "bst.h"
#include "list.h"
#include "queue.h"

/* each telement_t will be nodeinfo_t * */
struct nodeinfo_struct {
  int id;
  char *url;
  list_t adj;
};
typedef struct nodeinfo_struct nodeinfo_t;

struct rankinfo_struct {
  int nlinks;
  char *url;
};
typedef struct rankinfo_struct rankinfo_t;



/* public functions */
void do_something_with_link(const char *link);
void init_spider(void);
void destroy_spider(void);
int get_new_id(void); /* uses static data, returns incremental ID */

#endif
