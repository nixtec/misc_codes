/*
 * cwdent.c
 * cwd entry
 */
#include "cwdent.h"

//#define __DEBUG__

static struct cwdent **cwdent_root = NULL;
static size_t cwdent_len = 0;

char *cwdent_addcwd(uint32_t host, const char *cwd)
{
  struct cwdent **root = cwdent_root;
  int len = 1;
  struct cwdent *temp = (struct cwdent *) calloc(sizeof(struct cwdent), 1);

  if (!temp) {
    perror("malloc");
    exit(1); // fatal error
  }

  temp->host = host;
  strncpy(temp->cwd, cwd, PATH_MAX);
#ifdef __DEBUG__
  fprintf(stderr, "addcwd:\n\t[%d]=>[%s]\n", temp->host, temp->cwd);
#endif

  if (!root) {
    root = (struct cwdent **) malloc(sizeof(struct cwdent *) * 2);
  } else {
    while (root[len]) {
      len++;
    }
    len++; // this must be done
    //fprintf(stderr, "len = %d\n", len);
    root = (struct cwdent **) realloc(root,
       	(sizeof(struct cwdent *) * (len+1)));
  }
  root[len-1] = temp;
  root[len] = NULL;

  cwdent_root = root;
  cwdent_len++;

  return cwdent_root[len-1]->cwd;
}

char *cwdent_getcwd(uint32_t host)
{
  int i;
  struct cwdent **root = cwdent_root;
  struct cwdent *temp;

  for (i = 0; i < cwdent_len; i++) {
    temp = root[i];
    if (temp->host == host) {
#ifdef __DEBUG__
      fprintf(stderr, "getcwd:\n\t[%d]=>[%s]\n", temp->host, temp->cwd);
#endif
      return temp->cwd;
    }
  }
  return NULL;
}

char *cwdent_update(uint32_t host, const char *cwd)
{
  int i;
  struct cwdent **root = cwdent_root;
  struct cwdent *temp;

  for (i = 0; i < cwdent_len; i++) {
    temp = root[i];
    if (temp->host == host) {
      strncpy(temp->cwd, cwd, PATH_MAX);
#ifdef __DEBUG__
      fprintf(stderr, "update:\n\t[%d]=>[%s]\n", temp->host, temp->cwd);
#endif
      return temp->cwd;
    }
  }

  return NULL;
}
