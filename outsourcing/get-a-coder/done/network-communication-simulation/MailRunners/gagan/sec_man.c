/*
 * sec_man.c
 * @uthor: Gagandeep Jaswal
 * Security Manager
 */


#define AUTHFILE "passwd" /* file where authentication information is written */
/*
 * username:pass:/path
 */

/* return:
 * 0 => success
 */
#include <stdio.h>
#include <string.h>

char *strsep(char **stringp, const char *delim);

int validateCredential(const char *user, const char *pass)
{
  int ret;
  FILE *fp;
  char buf[256];
  char **bp, *bufp;
  char *u, *p, *path;

  fp = fopen(AUTHFILE, "r");
  if (!fp) {
    perror("fopen");
    return -1;
  }

  ret = -1;
  bufp = buf;
  while (fgets(buf, sizeof(buf), fp)) {
    bufp = buf;
    bp = &bufp;
    u = strsep(bp, ":\n");
    p = strsep(bp, ":\n");
    path = strsep(bp, ":\n");
    if (!strcmp(u, user) && !strcmp(p, pass)) {
      ret = 0;
      break;
    }
  }
  fclose(fp);

  return ret;
}
