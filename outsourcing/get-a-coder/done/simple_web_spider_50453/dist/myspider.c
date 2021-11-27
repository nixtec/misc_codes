/*
 * myspider.c
 */
#include "myspider.h"

#define INFINITE -1
static int count;
static int n_all_links;
static int n_internal_links;
static int n_external_links;
static int n_links; /* number of links in the current page */
static int nlinks = INFINITE; /* how many links to visit [INFINITE] */
static list_t adj_list;
static queue_t global_queue;
static tree_t global_tree;
static rankinfo_t *global_rank;
static int global_rank_size;
static int n_ranks;
static regex_t preg;
static char *baseurl; /* base hostname */
static int baseurl_len;
static int global_count; /* how many links visited */
static int **global_matrix; /* the connectivity matrix */
static FILE *qfp; /* queue file pointer, global so that we can close in SIG */
static FILE *lfp; /* link file pointer, global so that we can close in SIG */

/* we're interested on html links */
static const char *regex = "(/|\\.htm(l)?|\\.php(\\?.*)?)$";

static bool link_visited(char *url); /* whether the link was visited */
static char *get_hostname(const char *url);
static void build_matrix(tree_t t);
static void dump_matrix(const char *file);
static void dump_rank(const char *file);
static void sort_rank(void);
static void write_qfile(nodeinfo_t *ninfo);

//#define nodeinfo_urlcmp(n1, n2) strcmp(((nodeinfo_t *)(n1))->url, ((nodeinfo_t *)(n2))->url)
int nodeinfo_urlcmp(void *n1, void *n2);
int nodeinfo_idcmp(void *n1, void *n2);

static void handle_int(int signo);

#define QFILE "queue.txt"
#define MFILE "matrix.txt"
#define ERRLOG "spider.log"
#define LFILE "links.txt"		/* contains all the links */
#define RFILE "ranks.txt"		/* contains the ranking */

int main(int argc, char **argv)
{
  char *url = NULL;
  nodeinfo_t *ninfo = NULL;
  int i;
  lfp = fopen(LFILE, "w");
  if (!lfp) {
    perror(LFILE);
    exit(1);
  }

  if (argc < 2) {
    fprintf(stderr, "Usage: %s url [max_no_of_links_to_visit]\n", argv[0]);
    exit(0);
  }

  freopen(ERRLOG, "w", stdout); /* messages from library [go there] */

  /*
  baseurl = strdup(argv[1]);
  url = strrchr(baseurl, '/');
  *(url+1) = '\0';
  */

  baseurl = get_hostname(argv[1]);
  if (!baseurl) {
    fprintf(stderr, "Malformed Hostname <%s>\n", argv[1]);
    exit(1);
  }

  qinsert(&global_queue, strdup(argv[1])); /* link to be visited */

  if (argc > 2) { /* number of links to visit supplied */
    nlinks = atoi(argv[2]);
  }

  init_spider();

  signal(SIGINT, handle_int);

  if (nlinks != INFINITE)
    fprintf(stderr, "\t*** Maximum %d links will be visited\n", nlinks);
  fprintf(stderr, "\t*** Crawling <%s>\n\n", argv[1]);
  while ((url = qextract(&global_queue)) != NULL) {
    fprintf(stderr, "Fetching URL from queue: <%s>\n", url);
    if (link_visited(url) == false) {
      if (nlinks != INFINITE &&  count >= nlinks)
	break;
      count++;
      global_count++;
      adj_list = NULL; /* new list to be created, because new url */
      fprintf(stderr, "Visiting %s\n", url);
      n_links = 0; /* initialize number of links in the current page */
      getlinks(url);
      fprintf(stderr, "# of links in <%s> : %d\n", url, n_links);
      global_rank_size += sizeof(rankinfo_t);
      global_rank = realloc(global_rank, global_rank_size);
      if (!global_rank) {
	perror("realloc");
	exit(1);
      }
      global_rank[n_ranks].nlinks = n_links;
      global_rank[n_ranks].url = strdup(url);
      n_ranks++;

      ninfo = malloc(sizeof(nodeinfo_t)); /* tree node information */
      if (!ninfo) {
	fprintf(stderr, "malloc failed!\n");
	exit(1);
      }
      ninfo->id = get_new_id();
      ninfo->url = strdup(url);
      ninfo->adj = adj_list;
      global_tree = tree_insert(global_tree, ninfo, nodeinfo_urlcmp);
      write_qfile(ninfo);
      //fprintf(fp, "%d %s\n", ninfo->id, ninfo->url);
      /* debugging */
      /*
      fprintf(stderr, "id=%d, url=%s\n", ninfo->id, ninfo->url);
      adj_list = ninfo->adj;
      while (adj_list) {
	fprintf(stderr, "\turl=%s\n", (char *) adj_list->e);
	adj_list = adj_list->next;
      }
      */
    } else {
      fprintf(stderr, "Link already visited, skipping <%s>\n", url);
    }
    free(url); /* it is strduped */
  }
  fprintf(lfp, "Total Links: %d\n", n_all_links);
  fprintf(lfp, "External Links: %d\n", n_external_links);
  fprintf(lfp, "Internal HTML Links: %d\n", n_internal_links);
  fclose(lfp);
  lfp = NULL;
  fclose(qfp);
  qfp = NULL;

  fprintf(stderr, "Done.\n");

  global_matrix = malloc(global_count * sizeof(int *));
  for (i = 0; i < global_count; i++) {
    global_matrix[i] = calloc(1, global_count * sizeof(int));
  }
  fprintf(stderr, "Building matrix ... ");
  fflush(stderr);
  build_matrix(global_tree);
  fprintf(stderr, "Done.\n");
  fprintf(stderr, "Writing matrix to <%s> ... ", MFILE);
  fflush(stderr);
  dump_matrix(MFILE);
  fprintf(stderr, "Done.\n");
  fprintf(stderr, "Building ranking ... ");
  fflush(stderr);
  sort_rank();
  dump_rank(RFILE);
  fprintf(stderr, "Done.\n");

  destroy_spider();

  return 0;
}

void write_qfile(nodeinfo_t *ninfo)
{
  fprintf(qfp, "%d %s\n", ninfo->id, ninfo->url);
}

void do_something_with_link(const char *link)
{
  char *url = NULL;

  fprintf(stderr, "\t%s\n", link);
  fprintf(lfp, "%s\n", link);
  n_all_links++;
  n_links++;
  if (strncmp(baseurl, link, baseurl_len)) {
    fprintf(stderr, "\t\tExternal link, skipping <%s>\n", link);
    n_external_links++;
    return; /* silently ignore extenal links */
  }
  size_t len = strlen(link);
  size_t l = strcspn(link, "#"); /* it specifies some para of same page */
  if (len == l) { /* not found */
    url = strdup(link);
  } else { /* skip from # */
    url = malloc(l+1); /* +1 for terminating NUL char */
    strncpy(url, link, l);
    url[l] = '\0';
    len = l;
  }
  if (match_regex(&preg, regex, url) == 0) { /* matched */
    qinsert(&global_queue, url); /* to be visited later */
    adj_list = list_insert(adj_list, strdup(url)); /* save a copy of URL */
    n_internal_links++;
    //fprintf(stderr, "%s\n", url);
  } else {
    fprintf(stderr, "\t\tSkipping %s\n", url);
  }
}

void init_spider(void)
{
  int cflags = REG_EXTENDED|REG_NOSUB;
  if (regcomp(&preg, regex, cflags) != 0) {
    fprintf(stderr, "regcomp error!\n");
    exit(1);
  }

  fprintf(stderr, "Opening queue file <%s> ... ", QFILE);
  qfp = fopen(QFILE, "w");
  if (!qfp) {
    perror(QFILE);
    exit(0);
  }
  fprintf(stderr, "Done.\n");

  init_libwww("myspider", "1.0");
}

void destroy_spider(void)
{
  regfree(&preg);
  destroy_libwww();
  tree_destroy(global_tree, true);
}

int get_new_id(void)
{
  static int id;

  return ++id;
}

static bool link_visited(char *url)
{
  nodeinfo_t ninfo;

  ninfo.url = url;

  if (tree_find(global_tree, &ninfo, nodeinfo_urlcmp)) {
    return true;
  }
  return false;
}

/* compare two nodes by URL */
int nodeinfo_urlcmp(void *n1, void *n2)
{
  nodeinfo_t *tmp1 = n1;
  nodeinfo_t *tmp2 = n2;

  return (strcmp(tmp1->url, tmp2->url));
}

/* compare two nodes by ID */
int nodeinfo_idcmp(void *n1, void *n2)
{
  nodeinfo_t *tmp1 = n1;
  nodeinfo_t *tmp2 = n2;

  return (tmp1->id - tmp2->id);
}

/* signal handler */
static void handle_int(int signo)
{
  int i;

  if (qfp)
    fclose(qfp);

  if (lfp) {
    fprintf(lfp, "Total Links: %d\n", n_all_links);
    fprintf(lfp, "External Links: %d\n", n_external_links);
    fprintf(lfp, "Internal HTML Links: %d\n", n_internal_links);
    fclose(lfp);
  }

  if (!global_matrix) {
    global_matrix = malloc(global_count * sizeof(int *));
    for (i = 0; i < global_count; i++) {
      global_matrix[i] = calloc(1, global_count * sizeof(int));
    }
  }
  build_matrix(global_tree);
  dump_matrix(MFILE);
  sort_rank();
  dump_rank(RFILE);

  destroy_spider();
  exit(signo);
}

/* get the main hostname from the link
 * for example:
 * if the link to visit is http://www.website.com/visit/this/link/
 * then the hostname is 'http://www.website.com/'
 * This is needed so that we don't visit external links
 */
static char *get_hostname(const char *url)
{
  char *base = NULL;
  char proto[] = "http://"; /* we accept only http */
  int protolen = strlen(proto);
  if (strncmp(url, proto, protolen)) {
    return NULL;
  }
  int len = strlen(url);
  int n = strcspn(url+protolen, "/");
  if (n == (len-protolen)) { /* no '/' found!!! */
    return NULL;
  }

  base = malloc(n+protolen+1);
  if (!base) { /* fatal error */
    return NULL;
  }
  strncpy(base, url, n+protolen);
  base[n+protolen] = '\0';
  baseurl_len = n+protolen;

  return base;
}

/* build the connectivity matrix from the 'visited' binary search tree */
static void build_matrix(tree_t t)
{
  int row, col;
  nodeinfo_t *ninfo = NULL;
  list_t alist = NULL;
  nodeinfo_t info, *pinfo;
  tpos_t pos;

  if (t) {
    build_matrix(t->left);
    build_matrix(t->right);
    ninfo = t->e;
    row = ninfo->id - 1;
    //fprintf(stderr, "id=%d, url=%s\n", row, ninfo->url);
    alist = ninfo->adj;
    while (alist) {
      info.url = alist->e;
      pos = tree_find(global_tree, &info, nodeinfo_urlcmp);
      if (pos) { /* found it in the tree */
	pinfo = pos->e;
	col = pinfo->id - 1;
	//fprintf(stderr, "\tid=%d, url=%s\n", col, pinfo->url);
	global_matrix[row][col] = 1;
      }
#if 0
      else {
	fprintf(stderr, "Oops! No such node found!\n");
      }
#endif
      alist = alist->next;
    }
  }
}


static void dump_matrix(const char *file)
{
  int i, j;
  FILE *fp = fopen(file, "w");
  if (!fp) {
    perror("fopen");
    return;
  }

  fprintf(fp, "\t\t\t***** CONNECTIVITY MATRIX *****\n\n");
  fprintf(fp, "ID is according to the queue file\n\n");
  fprintf(fp, "ID +->");
  for (i = 0; i < global_count; i++) {
    fprintf(fp, "%4d", i+1);
  }
  fprintf(fp, "\n   v\n");

  fprintf(fp, "========");
  for (i = 0; i < global_count; i++) {
    fprintf(fp, "====");
  }
  fprintf(fp, "\n");

  for (i = 0; i < global_count; i++) {
    fprintf(fp, "%4d |", i+1);
    for (j = 0; j < global_count; j++) {
      fprintf(fp, "%4d", global_matrix[i][j]);
    }
    fprintf(fp, "\n");
  }

  fclose(fp);
}

static void dump_rank(const char *file)
{
  int i;
  FILE *fp = fopen(file, "w");
  if (!fp) {
    perror("fopen");
    return;
  }

  for (i = 0; i < n_ranks; i++) {
    fprintf(fp, "%4d %s\n", global_rank[i].nlinks, global_rank[i].url);
  }

  fclose(fp);
}

static void sort_rank(void)
{
  int i, j;
  rankinfo_t tmprank;

  for (i = 0; i < n_ranks; i++) {
    for (j = i+1; j < n_ranks; j++) {
      if (global_rank[i].nlinks < global_rank[j].nlinks) {
	tmprank = global_rank[i];
	global_rank[i].nlinks = global_rank[j].nlinks;
	global_rank[i].url = global_rank[j].url;
	global_rank[j] = tmprank;
      }
    }
  }
}
