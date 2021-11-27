/*
 * cmdmap.c
 * command mapping
 */
#include "cmdmap.h"

static struct cmdmap **cmdmap_root;
static size_t cmdmap_maplen;

static struct cmdmap **cmdmap_build(const char *mapfile);
static void cmdmap_freefunc(struct cmdmap **root);
static const char *cmdmap_find(struct cmdmap **root, const char *cmd);

void cmdmap_init(void)
{
  if (cmdmap_root) // already initialized
    return;
  cmdmap_root = cmdmap_build(CMDMAPFILE);
  if (cmdmap_root == NULL) {
    fprintf(stderr, "Command map tree creation failed!\n");
  }
}

/* get from command to mapped command */
const char *cmdmap_getmap(const char *cmd)
{
  if (cmdmap_root) {
    return (cmdmap_find(cmdmap_root, cmd));
  }

  return NULL;
}

void cmdmap_destroy(void)
{
  if (cmdmap_root) {
    cmdmap_freefunc(cmdmap_root);
    cmdmap_root = NULL;
  }
}

// qsort sends address of the values
static int cmdmap_cmp(const struct cmdmap **c1, const struct cmdmap **c2)
{
  //fprintf(stderr, "comparing: <%s><%s>\n", (*c1)->cmd, (*c2)->cmd);

  return strcmp((*c1)->cmd, (*c2)->cmd);
}

static struct cmdmap **cmdmap_add_map(struct cmdmap **root, struct cmdmap *map)
{
  int len = 1;

  if (!root) {
    root = (struct cmdmap **) malloc(sizeof(struct cmdmap *) * 2);
  } else {
    while (root[len]) {
      len++;
    }
    len++; // this must be done
    //fprintf(stderr, "len = %d\n", len);
    root = (struct cmdmap **) realloc(root,
       	(sizeof(struct cmdmap *) * (len+1)));
  }
  root[len-1] = map;
  //fprintf(stderr, "added: <%s><%s>\n", root[len-1]->cmd, root[len-1]->mapcmd);
  root[len] = NULL;

  return root;
}

static struct cmdmap **cmdmap_build(const char *mapfile)
{
  FILE *fp;
  char line[80];
  int i;
  char *token, *str;
  struct cmdmap **root = NULL;
  struct cmdmap *temp = NULL;

  fp = fopen(mapfile, "r");
  if (!fp) {
    perror(mapfile);
    return ((struct cmdmap **) NULL);
  }

  while (fgets(line, sizeof(line), fp)) {
    if (line[0] == '#') { // skip comments
      continue;
    }
    i = strlen(line);
    if (line[i-1] == '\n')
      line[i-1] = 0;
    temp = (struct cmdmap *) malloc(sizeof(struct cmdmap));
    if (!temp) {
      fprintf(stderr, "fatal error: malloc failed!\n");
      exit(1);
    }
    for (i = 0, str = line; ; i++, str = NULL) {
      token = strtok(str, ":");
      if (token == NULL)
	break;

      if (i == 0) { // main command
	temp->cmd = strdup(token);
      }
      else if (i == CMD_INDEX) { // entry for Linux/Windows command
	temp->mapcmd = strdup(token);
	root = cmdmap_add_map(root, temp);
	cmdmap_maplen++;
	break; // we've got what we need
      }
    }
  }

  fclose(fp);

  qsort(root, cmdmap_maplen, sizeof(struct cmdmap *), (compfunc) cmdmap_cmp);

  return root;
}

static const char *cmdmap_find(struct cmdmap **root, const char *cmd)
{
  struct cmdmap *temp = (struct cmdmap *) malloc(sizeof(struct cmdmap));;
  temp->cmd = (char *) cmd;
  struct cmdmap **result;

  result = (struct cmdmap **) bsearch(&temp, root, cmdmap_maplen,
      sizeof(struct cmdmap *), (compfunc) cmdmap_cmp);

  free(temp);

  if (result)
    return ((*result)->mapcmd);

  return NULL;
}

static void cmdmap_freefunc(struct cmdmap **root)
{
  struct cmdmap **item1, **item2;

  item1 = item2 = root;
  while (*item2) {
    item1 = item2++;
    free((*item1)->cmd);
    free((*item1)->mapcmd);
    free(*item1);
  }

  free(root);
}

