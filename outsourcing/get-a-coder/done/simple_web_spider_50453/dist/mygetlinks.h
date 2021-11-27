/*
 * mygetlinks.h
 */
#ifndef MYGETLINKS_H
#define MYGETLINKS_H

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWHTML.h"
#include "myspider.h"	/* do_something_with_link() */

int getlinks(const char *uri);
PUBLIC void init_libwww(const char *client, const char *version);
PUBLIC void destroy_libwww(void);

#endif
