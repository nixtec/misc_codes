/*
 * myfuncs.c
 * my custom routines
 * Copyright (C) 2006, Ayub <mrayub@gmail.com>
 */
#include "myfuncs.h"
#include "interface.h"		/* interface functions */
#include "support.h"

static GtkWidget *window1;
static GtkWidget *window2;
static GtkWidget *window3; /* yes/no window */
static GtkWidget *add_window; /* add stuff window */
static GtkWidget *edit_window; /* edit stuff window */
static GtkWidget *window5; /* extended query window */
static GtkWidget *window6; /* unmindful stuff window */
static GtkWidget *window8; /* master password window */
static GtkWidget *window9; /* db add password window */
static GtkWidget *window11; /* db edit password window */
static GtkWidget *window12; /* first in last out window */
static GtkWidget *dialog1; /* message window */
static GtkWidget *aboutdialog1;
static GtkWidget *add_filechooserdialog1; /* for add stuff window */
static GtkWidget *edit_filechooserdialog1; /* for edit stuff window */
int another_window_running = 0;
static char tmpbuf[1024]; /* temporary buffer */
static char license_holder[50];
char print_handler[50]; /* main() will initialize this */
char talk_handler[50]; /* main() will initialize this */
int should_block = 0;	/* execution of external progs should block? */
int should_export_query = 0;	/* should we export the query to text file */
int talk_handler_defined = 0; /* talk handler defined? */

static GtkWidget *default_focus_widget; /* widget to be focused on activate */

static int dump_photo(stuff_info *sinfo, char *tmpphoto);
static GtkTreeModel *create_model(void);
static GtkTreeModel *create_unmindful_model(void);
static GtkTreeModel *create_extended_query_model(void);
static GtkTreeModel *create_filo_model(void);
static void add_columns(GtkTreeView *treeview, gboolean clickable);
static void add_extended_query_columns(GtkTreeView *tv, gboolean clickable);
static void add_filo_columns(GtkTreeView *tv, gboolean clickable);
static void add_unmindful_columns(GtkTreeView *tv);
static void init_list_window(void);
static void init_unmindful_window(void);
static void init_extended_query_window(void);
static void init_filo_window(void);
static void free_list(GSList *list);
static void free_filo_list(GSList *list);
static void append_stuff_status_list(GtkListStore *store, GSList *slist);
static void append_filo_list(GtkListStore *store, GSList *slist);

char DB_DIR[80];

gboolean db_add_pass_required;
gboolean db_edit_pass_required;

static GHashTable *month_hash;

//static gboolean pass_ok = FALSE;

void init_month_hash(void)
{
  if (!month_hash)
    month_hash = g_hash_table_new(g_str_hash, g_str_equal);

  g_hash_table_insert(month_hash, "Jan", "01");
  g_hash_table_insert(month_hash, "Feb", "02");
  g_hash_table_insert(month_hash, "Mar", "03");
  g_hash_table_insert(month_hash, "Apr", "04");
  g_hash_table_insert(month_hash, "May", "05");
  g_hash_table_insert(month_hash, "Jun", "06");
  g_hash_table_insert(month_hash, "Jul", "07");
  g_hash_table_insert(month_hash, "Aug", "08");
  g_hash_table_insert(month_hash, "Sep", "09");
  g_hash_table_insert(month_hash, "Oct", "10");
  g_hash_table_insert(month_hash, "Nov", "11");
  g_hash_table_insert(month_hash, "Dec", "12");
}

int init_env(void)
{
  DB_DIR[sizeof(DB_DIR)-1] = '\0';
#ifdef _WIN32
  snprintf(DB_DIR, sizeof(DB_DIR)-1,
      "%s/_attendence_db", getenv("SystemDrive"));
#else
  snprintf(DB_DIR, sizeof(DB_DIR)-1,
      "%s/.attendence_db", getenv("HOME"));
#endif
#ifdef _DEBUG_
  fprintf(stderr, "DB_DIR = %s\n", DB_DIR);
#endif

  struct stat st;
  if (stat(DB_DIR, &st) == -1) { /* not exists */
    if (MKDIR(DB_DIR, 0755) == -1) {
      fprintf(stderr, "Error creating directory %s\n", DB_DIR);
      return 1;
    }
  }
  return 0;
}

void runproject(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
//  GdkCursor *cursor = gdk_cursor_new(GDK_HAND2);

  if (!isLicensed()) {
    showLicenseMessage();
  } else {

    if (!window1) {
      window1 = create_window1();
      gtk_window_maximize(GTK_WINDOW(window1));
    }
    show_widget(window1);


    //  gdk_window_set_cursor(window1->window, cursor);

    if (!window2)
      window2 = create_window2();
    show_widget(window2);

    if (!window5)
      window5 = create_window5();

    if (!window6)
      window6 = create_window6();
//    show_widget(window6); /* don't show it right now */

    if (!window12)
      window12 = create_window12();

#ifdef _DEBUG_
    fprintf(stderr, "Initializing databases\n");
#endif

    if (init_databases() != 0) {
      show_message(_("Error Initializing Databases\n"));
    }

    init_list_window();
    update_list_window();

    init_unmindful_window();
    update_unmindful_window();

    init_month_hash();
    init_extended_query_window();
    init_filo_window();

    db_add_pass_required = TRUE;
    db_edit_pass_required = TRUE;
  }
}

void ask_for_master_pass(void)
{
  GtkWidget *ptr;
  ptr = create_window7();
  show_widget(ptr);

  while (gtk_events_pending())
    gtk_main_iteration();

#ifdef _WIN32
  Sleep(3000);
#else
  sleep(3);
#endif
  gtk_widget_destroy(ptr);

  if (!window8) {
    window8 = create_window8();
  }
  present_window(window8);
}

void ask_for_db_add_pass(void)
{
  if (!window9) {
    window9 = create_window9();
  }
  present_window(window9);
}

void ask_for_db_edit_pass(void)
{
  if (!window11) {
    window11 = create_window11();
  }
  present_window(window11);
}

void show_message(const char *msg)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (!dialog1)
    dialog1 = create_dialog1();

  GtkWidget *label = lookup_widget(dialog1, "label8");
  gtk_label_set_markup(GTK_LABEL(label), msg);

  inactivate_windows();

  show_widget(dialog1);

}

void just_show_message(const char *msg)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (!dialog1)
    dialog1 = create_dialog1();

  GtkWidget *label = lookup_widget(dialog1, "label8");
  gtk_label_set_markup(GTK_LABEL(label), msg);

  present_window(dialog1);

}

void ask_yes_no(const char *msg)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (!window3)
    window3 = create_window3();

  GtkWidget *label = lookup_widget(window3, "label9");
  gtk_label_set_markup(GTK_LABEL(label), msg);

  inactivate_windows();

  present_window(window3);

}

void show_about_dialog(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (!aboutdialog1)
    aboutdialog1 = create_aboutdialog1();

  present_window(aboutdialog1);

}

/* redraw the list by reading database */
void update_unmindful_window(void)
{
  GSList *list = NULL;
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  fill_list_unmindful_today(&list);

  GtkTreeIter iter;
  GtkWidget *tv = lookup_widget(window6, "treeview3");
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tv));
  gtk_list_store_clear(GTK_LIST_STORE(model));

  GSList *temp = list;
  char *bcode_id;
  stuff_info sinfo;
  while (temp) {
    bcode_id = (char *) temp->data;
    memset(&sinfo, 0, sizeof(sinfo));
    if (get_stuff_info(bcode_id, &sinfo) != 0) {
      temp = temp->next;
      continue;
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
	0, sinfo.name,
	-1);
    temp = temp->next;
  }

  temp = list;
  while (temp) {
    g_free(temp->data);
    temp = temp->next;
  }
  g_slist_free(list);

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
}

/* redraw the list by reading database */
void update_list_window(void)
{
  GtkWidget *w;
  GSList *list = NULL;
  char duration_msg[50];
//  char hh_mm_ss[15]; /* duration */
  int hh;
  int mm;
//  int ss;
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

#ifdef _DEBUG_
  dump_list();
#endif

  fill_list_attendence_today(&list);

  GtkTreeIter iter;
  GtkWidget *tv = lookup_widget(window1, "treeview1");
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tv));
  gtk_list_store_clear(GTK_LIST_STORE(model));

  GSList *temp = list;
  stuff_status *sstat;
  while (temp) {
    sstat = (stuff_status *) temp->data;
//    sprintf(hh_mm_ss, "%d", sstat->duration);
    /* convert duration to HH:MM:SS here */
//    fprintf(stderr, "Hi There, debugging msg\n");
    hh = (int) (sstat->duration / 3600);
    mm = (int) ((sstat->duration % 3600) / 60);
//    ss = (int) (sstat->duration % 60);
//    sprintf(hh_mm_ss, "%02d hours %02d minutes", hh, mm);
    duration_msg[sizeof(duration_msg)-1] = '\0';
    snprintf(duration_msg, sizeof(duration_msg)-1,
       	"Was %s for %d hours and %d minutes",
	((sstat->status == 0) ? "IN" : "OUT"),
	hh,
	mm);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
	LCOL_ID, sstat->id,
	LCOL_NAME, sstat->name,
	LCOL_STAT, ((sstat->status == 0) ? "OUT" : "IN"),
	LCOL_SINCE, sstat->stat_time,
	LCOL_DURATION, duration_msg,
	-1);
    temp = temp->next;
  }

  free_list(list);
  g_slist_free(list);

  w = lookup_widget(window1, "image6");
  gtk_image_set_from_file(GTK_IMAGE(w), NULL);

  w = lookup_widget(window1, "label16");
  gtk_label_set_text(GTK_LABEL(w), "");

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
}

static void append_stuff_status_list(GtkListStore *store, GSList *slist)
{
  GtkTreeIter iter;
  GSList *temp = NULL;
  stuff_status *sstat;


  temp = slist;
  while (temp) {
    sstat = (stuff_status *) temp->data;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
	LCOL_ID, sstat->id,
	LCOL_NAME, sstat->name,
	LCOL_STAT, ((sstat->status == 0) ? "OUT" : "IN"),
	LCOL_SINCE, sstat->stat_time,
	-1);
    temp = temp->next;
  }

}

static void append_filo_list(GtkListStore *store, GSList *slist)
{
  GtkTreeIter iter;
  GSList *temp = NULL;
  stuff_status_filo *sstat;


  temp = slist;
  while (temp) {
    sstat = (stuff_status_filo *) temp->data;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
	LCOL_ID, sstat->id,
	LCOL_NAME, sstat->name,
	LCOL_STAT, sstat->fin,
	LCOL_SINCE, sstat->lout,
	-1);
    temp = temp->next;
  }

}

/* frees stuff_status list */
static void free_list(GSList *list)
{
  stuff_status *stemp;

  GSList *temp = list;
  while (temp) {
    stemp = (stuff_status *) temp->data;
    GFree(stemp->id);
    GFree(stemp->name);
    GFree(stemp->stat_time);
    GFree(temp->data);
    temp = temp->next;
  }
}

/* frees stuff_status_filo list */
static void free_filo_list(GSList *list)
{
  stuff_status_filo *stemp;

  GSList *temp = list;
  while (temp) {
    stemp = (stuff_status_filo *) temp->data;
    GFree(stemp->id);
    GFree(stemp->name);
    GFree(stemp->fin);
    GFree(stemp->lout);
    GFree(temp->data);
    temp = temp->next;
  }
}

static void init_unmindful_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

#ifdef _DEBUG_
//  dump_list();
#endif

//  GtkWidget *ptr;

  GtkTreeModel *model;
  GtkTreeView *tv;

  model = create_unmindful_model();

  tv = GTK_TREE_VIEW(lookup_widget(window6, "treeview3"));
  gtk_tree_view_set_model(tv, model);
  g_object_unref(model); /* tv already holds a reference */

  add_unmindful_columns(tv);

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
}

static void init_list_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

#ifdef _DEBUG_
//  dump_list();
#endif

//  GtkWidget *ptr;

  GtkTreeModel *model;
  GtkTreeView *tv;

  model = create_model();

  tv = GTK_TREE_VIEW(lookup_widget(window1, "treeview1"));
  gtk_tree_view_set_model(tv, model);
  g_object_unref(model); /* tv already holds a reference */

//  add_columns(tv, TRUE); /* because it may confuse the user to see list */
  add_columns(tv, FALSE);

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
}

static void init_extended_query_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkTreeModel *model;
  GtkTreeView *tv;

  model = create_extended_query_model();

  tv = GTK_TREE_VIEW(lookup_widget(window5, "treeview2"));
  gtk_tree_view_set_model(tv, model);
  g_object_unref(model); /* tv already holds a reference */

  add_extended_query_columns(tv, FALSE); /* columns will not be clickable */

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
}

static void init_filo_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Entering <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkTreeModel *model;
  GtkTreeView *tv;

  model = create_filo_model();

  tv = GTK_TREE_VIEW(lookup_widget(window12, "treeview4"));
  gtk_tree_view_set_model(tv, model);
  g_object_unref(model); /* tv already holds a reference */

  add_filo_columns(tv, FALSE); /* columns will not be clickable */

#ifdef _DEBUG_
  fprintf(stderr, "Leaving <%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
}


static GtkTreeModel *create_extended_query_model(void)
{
//  gint i = 0;
  GtkListStore *model;
//  GtkTreeIter iter;

  /* create list store */
  model = gtk_list_store_new(LCOL_ITEMS,
      G_TYPE_STRING, G_TYPE_STRING,
      G_TYPE_STRING, G_TYPE_STRING,
      G_TYPE_BOOLEAN);

  return GTK_TREE_MODEL(model);
}

static GtkTreeModel *create_filo_model(void)
{
//  gint i = 0;
  GtkListStore *model;
//  GtkTreeIter iter;

  /* create list store */
  model = gtk_list_store_new(FILO_ITEMS,
      G_TYPE_STRING, G_TYPE_STRING,
      G_TYPE_STRING, G_TYPE_STRING);

  return GTK_TREE_MODEL(model);
}

static GtkTreeModel *create_model(void)
{
//  gint i = 0;
  GtkListStore *model;
//  GtkTreeIter iter;

  /* create list store */
  model = gtk_list_store_new(LCOL_ITEMS,
      G_TYPE_STRING, G_TYPE_STRING,	/* LCOL_ID, LCOL_NAME */
      G_TYPE_STRING, G_TYPE_STRING,	/* LCOL_STAT, LCOL_SINCE */
      G_TYPE_STRING, G_TYPE_BOOLEAN);	/* LCOL_DURATION, NONE */

  return GTK_TREE_MODEL(model);
}

static GtkTreeModel *create_unmindful_model(void)
{
//  gint i = 0;
  GtkListStore *model;
//  GtkTreeIter iter;

  /* create list store */
  model = gtk_list_store_new(1, G_TYPE_STRING);

  return GTK_TREE_MODEL(model);
}


static void add_extended_query_columns(GtkTreeView *tv, gboolean clickable)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
//  GtkTreeModel *model = gtk_tree_view_get_model(tv);

#ifdef ____NO____
  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_ID));
  column = gtk_tree_view_column_new_with_attributes("Stuff ID",
      renderer,
      "text", LCOL_ID,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_ID);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);
#endif

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_NAME));
  column = gtk_tree_view_column_new_with_attributes("Stuff Name",
      renderer,
      "text", LCOL_NAME,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 250);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_NAME);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_STAT));
  column = gtk_tree_view_column_new_with_attributes("Status",
      renderer,
      "text", LCOL_STAT,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 100);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_STAT);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_SINCE));
  column = gtk_tree_view_column_new_with_attributes("Since",
      renderer,
      "text", LCOL_SINCE,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_SINCE);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

//  gtk_tree_view_set_search_column (tv, LCOL_ID);
  gtk_tree_view_set_search_column (tv, LCOL_NAME);

//  g_object_set(G_OBJECT(renderer), "background", "lavender", NULL);

}

static void add_filo_columns(GtkTreeView *tv, gboolean clickable)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
//  GtkTreeModel *model = gtk_tree_view_get_model(tv);

#ifdef ____NO____
  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(FILO_ID));
  column = gtk_tree_view_column_new_with_attributes("Stuff ID",
      renderer,
      "text", FILO_ID,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, FILO_ID);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);
#endif

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(FILO_NAME));
  column = gtk_tree_view_column_new_with_attributes("Stuff Name",
      renderer,
      "text", FILO_NAME,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 250);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, FILO_NAME);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(FILO_FIN));
  column = gtk_tree_view_column_new_with_attributes("First IN",
      renderer,
      "text", FILO_FIN,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 100);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, FILO_FIN);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_SINCE));
  column = gtk_tree_view_column_new_with_attributes("Last OUT",
      renderer,
      "text", FILO_LOUT,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, FILO_LOUT);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

//  gtk_tree_view_set_search_column (tv, LCOL_ID);
  gtk_tree_view_set_search_column (tv, FILO_NAME);

//  g_object_set(G_OBJECT(renderer), "background", "lavender", NULL);

}

static void add_columns(GtkTreeView *tv, gboolean clickable)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
//  GtkTreeModel *model = gtk_tree_view_get_model(tv);

#ifdef ____NO____
  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_ID));
  column = gtk_tree_view_column_new_with_attributes("Stuff ID",
      renderer,
      "text", LCOL_ID,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_ID);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);
#endif

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_NAME));
  column = gtk_tree_view_column_new_with_attributes("Stuff Name",
      renderer,
      "text", LCOL_NAME,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 250);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_NAME);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_STAT));
  column = gtk_tree_view_column_new_with_attributes("Status",
      renderer,
      "text", LCOL_STAT,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 100);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_STAT);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(LCOL_SINCE));
  column = gtk_tree_view_column_new_with_attributes("Time [HH:MM:SS]",
      renderer,
      "text", LCOL_SINCE,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_SINCE);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column",
      GINT_TO_POINTER(LCOL_DURATION));
  column = gtk_tree_view_column_new_with_attributes("Duration [HH:MM:SS]",
      renderer,
      "text", LCOL_DURATION,
      NULL);
  gtk_tree_view_column_set_fixed_width (column, 150);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, LCOL_DURATION);
  gtk_tree_view_column_set_clickable(column, clickable);
  gtk_tree_view_append_column(tv, column);
  g_object_set(G_OBJECT(renderer), "background", "lavender", NULL);

//  gtk_tree_view_set_search_column (tv, LCOL_ID);
  gtk_tree_view_set_search_column (tv, LCOL_NAME);


}

static void add_unmindful_columns(GtkTreeView *tv)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
//  GtkTreeModel *model = gtk_tree_view_get_model(tv);

  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(0));
  column = gtk_tree_view_column_new_with_attributes("Stuff Name",
      renderer,
      "text", 0,
      NULL);
//  gtk_tree_view_column_set_fixed_width (column, 150);
//  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_sort_column_id(column, 0);
  gtk_tree_view_append_column(tv, column);
  gtk_tree_view_set_search_column (tv, 0);

//  g_object_set(G_OBJECT(renderer), "background", "lavender", NULL);

}

void activate_windows(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *window;
  /* user must take a decision
   * so set other windows non-sensitive
   */
  window = window1;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == FALSE)
    gtk_widget_set_sensitive(window, TRUE);
  window = add_window;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == FALSE)
    gtk_widget_set_sensitive(window, TRUE);
  window = edit_window;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == FALSE)
    gtk_widget_set_sensitive(window, TRUE);
  window = window5;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == FALSE)
    gtk_widget_set_sensitive(window, TRUE);
  window = window12;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == FALSE)
    gtk_widget_set_sensitive(window, TRUE);
  /*
  if (GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
    */

  if (default_focus_widget) {
    focus_widget(default_focus_widget);
    default_focus_widget = NULL;
  }

  show_barcode_entry_window();
}

void inactivate_windows(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *window;
  /* user must take a decision
   * so set other windows non-sensitive
   */
  window = window1;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
  window = add_window;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
  window = edit_window;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
  window = window5;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
  window = window12;
  if (window && GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
  /*
  if (GTK_WIDGET_IS_SENSITIVE(window) == TRUE)
    gtk_widget_set_sensitive(window, FALSE);
    */

  hide_barcode_entry_window();
}

void confirm_exit(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  ask_yes_no(_("<b>Are you sure you want to exit?\n</b>"));
}

void exit_confirmed(gboolean confirmed)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (confirmed == TRUE) { /* exit confirmed */
    shutdown_project();
  } else {
    activate_windows();
  }
}

/* barcode id will be editable */
void add_stuff_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
//  claer_stuff_window();
  if (!add_window) {
    add_window = create_window4();
    g_object_set_data(G_OBJECT(add_window), "type",
	GINT_TO_POINTER(ADD_STUFF));
    gtk_window_set_title(GTK_WINDOW(add_window),
	_("Add Stuff"));
  }
  if (!GTK_WIDGET_VISIBLE(add_window))
    another_window_running++;
  present_window(add_window);
  hide_widget(window2);
}

/* barcode id will be non-editable */
void edit_stuff_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
//  claer_stuff_window();
  if (!edit_window) {
    edit_window = create_window4();
    g_object_set_data(G_OBJECT(edit_window), "type",
	GINT_TO_POINTER(EDIT_STUFF));
    gtk_window_set_title(GTK_WINDOW(edit_window),
	_("Edit Stuff"));
    /*
    GtkWidget *editable = lookup_widget(edit_window, "entry2");
    gtk_widget_set_sensitive(editable, FALSE);
    */
//    gtk_editable_set_editable(GTK_EDITABLE(editable), FALSE);
  }
  if (!GTK_WIDGET_VISIBLE(edit_window))
    another_window_running++;
  present_window(edit_window);
  hide_widget(window2);
}

void extended_query_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
//  claer_stuff_window();
  if (!GTK_WIDGET_VISIBLE(window5))
    another_window_running++;
  present_window(window5);
  hide_barcode_entry_window();
}

void filo_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
//  claer_stuff_window();
  if (!GTK_WIDGET_VISIBLE(window12))
    another_window_running++;
  present_window(window12);
  hide_barcode_entry_window();
}

void hide_barcode_entry_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (window2)
    hide_widget(window2);
}

void show_barcode_entry_window(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *entry;

  if (another_window_running)
    return;

  if (window2) {
    present_window(window2); /* will unhide if necessary */
    entry = lookup_widget(window2, "entry1");
    focus_widget(entry);
  }
}

void load_add_file_chooser(void)
{
  GtkFileFilter *filter;

#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (!add_filechooserdialog1) {
    add_filechooserdialog1 = create_filechooserdialog1();
    g_object_set_data(G_OBJECT(add_filechooserdialog1), "type",
	GINT_TO_POINTER(ADD_STUFF));
    gtk_window_set_title(GTK_WINDOW(add_filechooserdialog1),
	_("Add Stuff: Load Image File"));
    filter = gtk_file_filter_new();
    /* following is buggy in windows */
//    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name(filter, "JPEG Image");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    /*
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(add_filechooserdialog1),
	filter);
	*/
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(add_filechooserdialog1),
	filter);
//    printf("DEBUGGING: %s:%d\n", __FILE__, __LINE__);
  }
  present_window(add_filechooserdialog1);

}

void load_edit_file_chooser(void)
{
  GtkFileFilter *filter;

#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (!edit_filechooserdialog1) {
    edit_filechooserdialog1 = create_filechooserdialog1();
    g_object_set_data(G_OBJECT(edit_filechooserdialog1), "type",
	GINT_TO_POINTER(EDIT_STUFF));
    gtk_window_set_title(GTK_WINDOW(edit_filechooserdialog1),
	_("Edit Stuff: Load Image File"));
    filter = gtk_file_filter_new();
    /* following is buggy in windows */
//    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name(filter, "JPEG Image");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    /*
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(add_filechooserdialog1),
	filter);
	*/
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(edit_filechooserdialog1),
	filter);
//    printf("DEBUGGING: %s:%d\n", __FILE__, __LINE__);
  }
  present_window(edit_filechooserdialog1);
}

void load_add_photo(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  char *filename;
  char *prevfile = NULL;
  GtkWidget *image;

  filename = gtk_file_chooser_get_filename(
      GTK_FILE_CHOOSER(add_filechooserdialog1));
  hide_widget(add_filechooserdialog1);
#ifdef _DEBUG_
  fprintf(stderr, "Filename: %s\n", filename);
#endif
  if (filename) {
    prevfile = g_object_get_data(G_OBJECT(add_window), "photo_loc");
    if (prevfile && !strncmp(TMP_PREFIX, prevfile, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
      fprintf(stderr, "removing %s\n", prevfile);
#endif
      remove(prevfile);
    }
    image = lookup_widget(add_window, "image5");
    gtk_image_set_from_file(GTK_IMAGE(image), filename);
    g_object_set_data_full(G_OBJECT(add_window), "photo_loc",
       	(gpointer) filename, g_free);
  }
  /* photo_loc is attached with the object, so don't free it */
}

void load_edit_photo(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  char *filename;
  char *prevfile = NULL;
  GtkWidget *image;

  filename = gtk_file_chooser_get_filename(
      GTK_FILE_CHOOSER(edit_filechooserdialog1));
  hide_widget(edit_filechooserdialog1);
#ifdef _DEBUG_
  fprintf(stderr, "Filename: %s\n", filename);
#endif
  if (filename) {
    prevfile = g_object_get_data(G_OBJECT(edit_window), "photo_loc");
    if (prevfile && !strncmp(TMP_PREFIX, prevfile, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
      fprintf(stderr, "removing %s\n", prevfile);
#endif
      remove(prevfile);
    }
    image = lookup_widget(edit_window, "image5");
    gtk_image_set_from_file(GTK_IMAGE(image), filename);
    g_object_set_data_full(G_OBJECT(edit_window), "photo_loc",
       	(gpointer) filename, g_free);
  }
}

/* take action when add stuff window closed */
void add_stuff_window_closed(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (add_filechooserdialog1)
    hide_widget(add_filechooserdialog1);
}

/* take action when edit stuff window closed */
void edit_stuff_window_closed(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (edit_filechooserdialog1)
    hide_widget(edit_filechooserdialog1);
}

void save_add_stuff(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  stuff_info *sinfo;
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  char *filename;

  sinfo = (stuff_info *) g_malloc0(sizeof(stuff_info));

  widget = lookup_widget(add_window, "entry2");
  sinfo->bcode_id = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  if (strlen(sinfo->bcode_id) == 0) {
    show_message(_("Please fill stuff ID entry!"));
    default_focus_widget = widget;
    goto cleanup;
  }
  widget = lookup_widget(add_window, "entry3");
  sinfo->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  if (strlen(sinfo->name) == 0) {
    show_message(_("Please fill in stuff Name!"));
    default_focus_widget = widget;
    goto cleanup;
  }

  widget = lookup_widget(add_window, "textview2");
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  sinfo->meta = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

//  widget = lookup_widget(add_window, "image5");
  filename = (char *) g_object_get_data(G_OBJECT(add_window), "photo_loc");
  if (filename && (strlen(filename) > 0)) {
#ifdef _DEBUG_
    fprintf(stderr, "Filename: %s\n", filename);
#endif
    store_photo(filename, sinfo);
  }

  if (stuff_exists(sinfo->bcode_id)) {
    show_message(_("Stuff already exists!"));
    widget = lookup_widget(add_window, "entry2");
    default_focus_widget = widget;
    goto cleanup;
  }

  if (store_stuff_info(sinfo) == 0) {
    show_message(_("Stuff info saved successfully."));
    clear_window4(add_window);
  } else {
    show_message(_("Error saving stuff info!!!"));
    goto cleanup;
  }

  if (filename && !strncmp(TMP_PREFIX, filename, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
    fprintf(stderr, "removing %s\n", filename);
#endif
    remove(filename);
  }

cleanup:
  GFree(sinfo->bcode_id);
  GFree(sinfo->name);
  GFree(sinfo->meta);
  GFree(sinfo->photo); /* free byte array data now */
  GFree(sinfo);
}


void save_edit_stuff(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  stuff_info *sinfo;
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  char *filename;

  sinfo = (stuff_info *) g_malloc0(sizeof(stuff_info));

  widget = lookup_widget(edit_window, "entry2");
  sinfo->bcode_id = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  if (strlen(sinfo->bcode_id) == 0) {
    show_message(_("Please fill stuff ID entry!"));
    default_focus_widget = widget;
    goto cleanup;
  }
  widget = lookup_widget(edit_window, "entry3");
  sinfo->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  if (strlen(sinfo->name) == 0) {
    show_message(_("Please fill in stuff Name!"));
    default_focus_widget = widget;
    goto cleanup;
  }

  widget = lookup_widget(edit_window, "textview2");
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  sinfo->meta = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

//  widget = lookup_widget(edit_window, "image5");
  filename = (char *) g_object_get_data(G_OBJECT(edit_window), "photo_loc");
  if (filename && (strlen(filename) > 0)) {
#ifdef _DEBUG_
    fprintf(stderr, "Filename: %s\n", filename);
#endif
    store_photo(filename, sinfo);
  }

  if (!stuff_exists(sinfo->bcode_id)) {
    show_message(_("No such Stuff!"));
    widget = lookup_widget(edit_window, "entry2");
    default_focus_widget = widget;
    goto cleanup;
  }

  if (store_stuff_info(sinfo) == 0) {
    show_message(_("Stuff info saved successfully."));
    clear_window4(edit_window);
  } else {
    show_message(_("Error saving stuff info!!!"));
    goto cleanup;
  }

  if (filename && !strncmp(TMP_PREFIX, filename, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
    fprintf(stderr, "removing %s\n", filename);
#endif
    remove(filename);
  }

cleanup:
  GFree(sinfo->bcode_id);
  GFree(sinfo->name);
  GFree(sinfo->meta);
  GFree(sinfo->photo);
  GFree(sinfo);
}

void clear_window4(GtkWidget *window)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;

  char *prevfile;

//  int data;

  /*
  data = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "type"));
  if (data == ADD_STUFF) {
    widget = lookup_widget(window, "entry2");
    clear_entry(widget);
  }
  */
  widget = lookup_widget(window, "entry2");
  clear_entry(widget);
  widget = lookup_widget(window, "entry3");
  clear_entry(widget);

  widget = lookup_widget(window, "textview2");
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_delete(buffer, &start, &end);

  widget = lookup_widget(window, "image5");
  gtk_image_set_from_file(GTK_IMAGE(widget), NULL);
  /* clears previous filename */
  prevfile = g_object_get_data(G_OBJECT(window), "photo_loc");
  if (prevfile && !strncmp(TMP_PREFIX, prevfile, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
    fprintf(stderr, "removing %s\n", prevfile);
#endif
    remove(prevfile);
  }
  g_object_set_data(G_OBJECT(window), "photo_loc", NULL);

  widget = lookup_widget(window, "entry2");
  focus_widget(widget);
  /*
  if (data == ADD_STUFF) {
    widget = lookup_widget(window, "entry2");
    focus_widget(widget);
  } else {
    widget = lookup_widget(window, "entry3");
    focus_widget(widget);
  }
  */
}

void clear_window4_image(GtkWidget *window)
{
  char *prevfile;
  GtkWidget *image;

  image = lookup_widget(window, "image5");
  gtk_image_set_from_file(GTK_IMAGE(image), NULL);

  prevfile = g_object_get_data(G_OBJECT(window), "photo_loc");
  if (prevfile && !strncmp(TMP_PREFIX, prevfile, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
    fprintf(stderr, "removing %s\n", prevfile);
#endif
    remove(prevfile);
  }
  g_object_set_data(G_OBJECT(window), "photo_loc", NULL);
}


/* just an assumption of the size of the picture */
# define MIN_PHOTO_SIZE 4096
/*
 * store photo to the sinfo's photo field
 * memory must be freed with g_free() by caller
 *
 * always use 'fopen()' in windows and then get file descriptor with 'fileno'
 * otherwise unexpected results may occur
 */
void store_photo(const char *filename, stuff_info *sinfo)
{
  size_t nread;
  int fd;
  GByteArray *gbarray;

  gbarray = g_byte_array_sized_new(MIN_PHOTO_SIZE);
#ifdef _WIN32
  FILE *fp;
  fp = fopen(filename, "rb");
  fd = fileno(fp);
#else
  fd = open(filename, O_RDONLY);
#endif
  if (fd == -1) {
    fprintf(stderr, "Error opening photo %s\n", filename);
    return;
  }

  while ((nread = read(fd, tmpbuf, sizeof(tmpbuf))) > 0) {
#ifdef _DEBUG_
    fprintf(stderr, "%d bytes read in a single loop\n", nread);
#endif
    g_byte_array_append(gbarray, (unsigned char *) tmpbuf, nread);
  }
  close(fd);
#ifdef _WIN32
  fclose(fp);
#endif

  sinfo->photo = (unsigned char *) gbarray->data;
  sinfo->photo_len = gbarray->len;
#ifdef _DEBUG_
  fprintf(stderr, "store_photo: photo_len = %d\n", sinfo->photo_len);
#endif

  g_byte_array_free(gbarray, FALSE); /* don't free real byte data */
}

/*
 * load info from database */
void load_stuff_info(GtkWidget *widget, const char *id)
{
  GtkWidget *w;
  stuff_info sinfo;
  char tmpphoto[256];

  memset(&sinfo, 0, sizeof(sinfo));

  if (get_stuff_info(id, &sinfo) != 0) {
    w = lookup_widget(widget, "window4");
    clear_window4(w);
    w = lookup_widget(widget, "entry2");
    gtk_entry_set_text(GTK_ENTRY(w), id);
//    focus_widget(w);
    return;
  }

  strcpy(tmpphoto, TMP_PREFIX);
  strcat(tmpphoto, "XXXXXX");

//  strcpy(tmpphoto, "c:/test.jpg");
  if (sinfo.photo_len > 0) {
    if (dump_photo(&sinfo, tmpphoto) != 0) {
      memset(tmpphoto, 0, sizeof(tmpphoto));
    }
  }

#ifdef _DEBUG_
  fprintf(stderr, "bcode_id = %s\n", sinfo.bcode_id);
  fprintf(stderr, "name = %s\n", sinfo.name);
  fprintf(stderr, "meta = %s\n", sinfo.meta);
  fprintf(stderr, "photo_len = %d\n", sinfo.photo_len);
  fprintf(stderr, "tmpphoto = %s\n", tmpphoto);
#endif

//  w = lookup_widget(widget, "window4");
  w = lookup_widget(widget, "entry2");
  set_entry(w, sinfo.bcode_id);
  w = lookup_widget(widget, "entry3");
  set_entry(w, sinfo.name);
  w = lookup_widget(widget, "textview2");

  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w));
  gtk_text_buffer_set_text(buffer, sinfo.meta, -1);

  if (sinfo.photo_len > 0) {
    w = lookup_widget(widget, "image5");
    gtk_image_set_from_file(GTK_IMAGE(w), tmpphoto);
    w = lookup_widget(widget, "window4");
    g_object_set_data_full(G_OBJECT(w), "photo_loc",
       	(gpointer) g_strdup(tmpphoto), g_free);
  } else {
    w = lookup_widget(widget, "image5");
    gtk_image_set_from_file(GTK_IMAGE(w), NULL);
    g_object_set_data(G_OBJECT(w), "photo_loc", NULL);
  }

}

/*
 * return 0 = success
 * return 1 = failure
 */
static int dump_photo(stuff_info *sinfo, char *tmpname)
{
  int fd;
  ssize_t ret;

#ifdef _WIN32
  FILE *fp;
  fp = fopen(mktemp(tmpname), "wb");
  fd = fileno(fp);
#else
  fd = mkstemp(tmpname);
#endif
  if (fd == -1)
    return 1;


#ifdef _DEBUG_
  fprintf(stderr, "Dumping photo [%d bytes] to %s\n",
      sinfo->photo_len, tmpname);
#endif
  ret = write(fd, sinfo->photo, sinfo->photo_len);
  if (ret < sinfo->photo_len) {
    fprintf(stderr, "Error writing to %s: %s\n", tmpname, strerror(errno));
    ret = 1;
  } else {
    ret = 0;
  }

  close(fd);
#ifdef _WIN32
  fclose(fp);
#endif

  return (int) ret;
}

void display_main_window_contents(const char *id)
{
  stuff_info sinfo;
  last_status lstatus;
  char tmpphoto[256];
  char timebuf[32];
  GtkWidget *widget;


  memset(&sinfo, 0, sizeof(sinfo));
  memset(&lstatus, 0, sizeof(lstatus));

  strcpy(tmpphoto, TMP_PREFIX);
  strcat(tmpphoto, "XXXXXX");

//  strcpy(tmpphoto, "c:/test.jpg");

  if (get_stuff_info(id, &sinfo) != 0)
    return;

  widget = lookup_widget(window1, "image6");
  if ((sinfo.photo_len > 0) && (dump_photo(&sinfo, tmpphoto) == 0)) {
    gtk_image_set_from_file(GTK_IMAGE(widget), tmpphoto);
    if (remove(tmpphoto) != 0) {
      fprintf(stderr, "Error removing file %s\n", tmpphoto);
    }
  } else {
    gtk_image_set_from_file(GTK_IMAGE(widget), NULL);
  }

  if (get_last_status(id, &lstatus) != 0)
    return;

  strcpy(tmpbuf, "<span size=\"xx-large\"><b>Last Status</b>\n");
  strcat(tmpbuf, "  <b>Name : </b> <span weight=\"heavy\" "
      "foreground=\"purple\"><tt>");
  strcat(tmpbuf, sinfo.name);
  strcat(tmpbuf, "</tt></span>\n  <b>Status : </b> ");
  if (lstatus.last_stat == IN) {
    strcat(tmpbuf, "<span foreground=\"blue\" weight=\"heavy\">"
	"<tt>IN</tt></span>");
  } else {
    strcat(tmpbuf, "<span foreground=\"red\" weight=\"heavy\">"
	"<tt>OUT</tt></span>");
  }
  strcat(tmpbuf, " since <b>");
//  strcat(tmpbuf, ctime((time_t *) &lstatus.times));
  (void) strftime(timebuf, sizeof(timebuf), "%H:%M:%S %d-%m-%Y",
		  localtime((time_t *) &lstatus.times));
  strcat(tmpbuf, timebuf);
  strcat(tmpbuf, "</b></span>");

  widget = lookup_widget(window1, "label16");
  gtk_label_set_markup(GTK_LABEL(widget), _(tmpbuf));

//cleanup:
  GFree(lstatus.bcode_id);
}


#define INVALID_QUERY 0
#define DAILY_QUERY   1
#define MONTHLY_QUERY 2
#define YEARLY_QUERY  3
void run_extended_query(void)
{
  char dbname[256];
  char date_dd_mm[80];
  int qtype = INVALID_QUERY;
  int i;

  GtkWidget *w;
  GtkListStore *store;
  GtkTreeIter iter;

#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  /*
  show_message(_("Not yet implemented!"));
  default_focus_widget = NULL;
  return;
  */

  FILE *fp; /* to be exported file pointer */
  GSList *temp = NULL;
  stuff_status *sstat = NULL;

  char *query = NULL;
  GSList *list = NULL;
  char *dd_str = NULL;
  char *mm_str = NULL;
  char *mon = NULL;
  char *yyyy_str = NULL;
//  int dd, mm, yyyy;
  char *bcode_id = NULL;


  /* build query here */
  w = lookup_widget(window5, "radiobutton1");
  /* gtk_radio_button inherits gtk_toggle_button */
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
    qtype = DAILY_QUERY;
//    fprintf(stderr, "Daily Query:\n");
    w = lookup_widget(window5, "comboboxentry3");
    dd_str = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
    if (strlen(dd_str) == 0) {
      show_message(_("Please fill the day field!"));
      focus_widget(w);
      goto cleanup;
    }
//    fprintf(stderr, "*** day = %s\n", dd_str);
    w = lookup_widget(window5, "comboboxentry4");
    mm_str = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
    if (strlen(mm_str) == 0) {
      show_message(_("Please fill the month field!"));
      focus_widget(w);
      goto cleanup;
    }
//    fprintf(stderr, "*** month = %s\n", mm_str);
  }
  w = lookup_widget(window5, "radiobutton2");
  /* gtk_radio_button inherits gtk_toggle_button */
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
    qtype = MONTHLY_QUERY;
//    fprintf(stderr, "Monthly Query:\n");
    w = lookup_widget(window5, "comboboxentry5");
    mm_str = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
    if (strlen(mm_str) == 0) {
      show_message(_("Please fill the month field!"));
      focus_widget(w);
      goto cleanup;
    }
//    fprintf(stderr, "*** month = %s\n", mm_str);
  }

  w = lookup_widget(window5, "entry7");
  bcode_id = g_strdup(get_entry(w));
  clear_entry(w);
  focus_widget(w);
//  fprintf(stderr, "bcode_id = %s\n", bcode_id);

  /*
  show_message(_("Not yet implemented!"));
  default_focus_widget = NULL;
  goto cleanup;
  */

  yyyy_str = g_strdup(get_current_year());
  dbname[sizeof(dbname)-1] = '\0';
  snprintf(dbname, sizeof(dbname)-1,
      "%s/db_%s.sdb", DB_DIR, yyyy_str);

  mon = (char *) g_hash_table_lookup(month_hash, mm_str);
  if (qtype == DAILY_QUERY) {
    if (strlen(bcode_id) > 0) {
      snprintf(tmpbuf, sizeof(tmpbuf)-1,
	  "SELECT * from attendence_%s_%s WHERE bcode_id='%s'",
	  dd_str,
	  mon,
	  bcode_id);
    } else {
      sprintf(tmpbuf, "SELECT * from attendence_%s_%s", dd_str, mon);
    }
    query = tmpbuf;
    fill_stuff_status_list_query(dbname, query, &list);
    /* now show the data to list */
    w = lookup_widget(window5, "treeview2");
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w)));
    gtk_list_store_clear(store);
    sprintf(date_dd_mm, "Query Output for %s-%s", dd_str, mm_str);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
	LCOL_ID, "---",
	LCOL_NAME, date_dd_mm,
	LCOL_STAT, "------",
	LCOL_SINCE, "------",
	-1);

    append_stuff_status_list(store, list);

    /* export it */
    if (should_export_query) {
      fp = fopen(EXPORT_QUERY_FILE, "w");
      if (!fp) {
	fprintf(stderr, "Error opening export file!\n");
      } else {
	fprintf(fp, ":%s-%s\n", dd_str, mon);
	temp = list;
	while (temp) {
	  sstat = (stuff_status *) temp->data;
	  fprintf(fp, "%s|%s|%d|%s\n",
	      sstat->id,
	      sstat->name,
	      sstat->status,
	      sstat->stat_time);
	  temp = temp->next;
	}
	fclose(fp);
      }
    }


    free_list(list);
    g_slist_free(list);
    list = NULL;
  }
  else if (qtype == MONTHLY_QUERY) {
    w = lookup_widget(window5, "treeview2");
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w)));
    gtk_list_store_clear(store);
    fp = NULL;
    if (should_export_query) {
      fp = fopen(EXPORT_QUERY_FILE, "w");
      if (!fp) {
	fprintf(stderr, "Error opening export file!\n");
      }
    }
    for (i = 1; i <= 31; i++) {
      if (strlen(bcode_id) > 0) {
	sprintf(tmpbuf, "SELECT * from attendence_%02d_%s WHERE bcode_id='%s'",
	    i, mon, bcode_id);
      } else {
	sprintf(tmpbuf, "SELECT * from attendence_%02d_%s", i, mon);
      }
      query = tmpbuf;
      fill_stuff_status_list_query(dbname, query, &list);

      if (list) {
	sprintf(date_dd_mm, "Query Output for %02d-%s", i, mm_str);
//	sprintf(date_dd_mm, "*** %02d-%s ***", i, mm_str);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
	    LCOL_ID, "---",
	    LCOL_NAME, date_dd_mm,
	    LCOL_STAT, "------",
	    LCOL_SINCE, "------",
	    -1);
	append_stuff_status_list(store, list);
	/* export it */
	if (fp) {
	  fprintf(fp, ":%02d-%s\n", i, mon);
	  temp = list;
	  while (temp) {
	    sstat = (stuff_status *) temp->data;
	    fprintf(fp, "%s|%s|%d|%s\n",
		sstat->id,
		sstat->name,
		sstat->status,
		sstat->stat_time);
	    temp = temp->next;
	  }
	}

	free_list(list);
	g_slist_free(list);
	list = NULL;
      }
    }
    if (fp) {
      fclose(fp);
    }
  }
  else if (qtype == YEARLY_QUERY) {
    fprintf(stderr, "Yearly query not available!\n");
  }
  else {
    fprintf(stderr, "Unknown query type %d\n", qtype);
  }

//  fprintf(stderr, "QUERY: %s\n", query);

cleanup:
  GFree(dd_str);
  GFree(mm_str);
  GFree(yyyy_str);
  GFree(bcode_id);
}

void run_filo_query(void)
{
  char dbname[256];
  char date_dd_mm[80];
  int qtype = INVALID_QUERY;
  int i;

  GtkWidget *w;
  GtkListStore *store;
  GtkTreeIter iter;

#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  /*
  show_message(_("Not yet implemented!"));
  default_focus_widget = NULL;
  return;
  */

  FILE *fp; /* to be exported file pointer */
  GSList *temp = NULL;
  stuff_status_filo *sstat = NULL;

  char *query = NULL;
  GSList *list = NULL;
  char *dd_str = NULL;
  char *mm_str = NULL;
  char *mon = NULL;
  char *yyyy_str = NULL;
//  int dd, mm, yyyy;
  char *bcode_id = NULL;


  /* build query here */
  w = lookup_widget(window12, "radiobutton3");
  /* gtk_radio_button inherits gtk_toggle_button */
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
    qtype = DAILY_QUERY;
//    fprintf(stderr, "Daily Query:\n");
    w = lookup_widget(window12, "comboboxentry6");
    dd_str = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
    if (strlen(dd_str) == 0) {
      show_message(_("Please fill the day field!"));
      focus_widget(w);
      goto cleanup;
    }
//    fprintf(stderr, "*** day = %s\n", dd_str);
    w = lookup_widget(window12, "comboboxentry7");
    mm_str = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
    if (strlen(mm_str) == 0) {
      show_message(_("Please fill the month field!"));
      focus_widget(w);
      goto cleanup;
    }
//    fprintf(stderr, "*** month = %s\n", mm_str);
  }
  w = lookup_widget(window12, "radiobutton4");
  /* gtk_radio_button inherits gtk_toggle_button */
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
    qtype = MONTHLY_QUERY;
//    fprintf(stderr, "Monthly Query:\n");
    w = lookup_widget(window12, "comboboxentry8");
    mm_str = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
    if (strlen(mm_str) == 0) {
      show_message(_("Please fill the month field!"));
      focus_widget(w);
      goto cleanup;
    }
//    fprintf(stderr, "*** month = %s\n", mm_str);
  }

  w = lookup_widget(window12, "entry11");
  bcode_id = g_strdup(get_entry(w));
  clear_entry(w);
  focus_widget(w);
//  fprintf(stderr, "bcode_id = %s\n", bcode_id);

  /*
  show_message(_("Not yet implemented!"));
  default_focus_widget = NULL;
  goto cleanup;
  */

  yyyy_str = g_strdup(get_current_year());
  dbname[sizeof(dbname)-1] = '\0';
  snprintf(dbname, sizeof(dbname)-1,
      "%s/db_%s.sdb", DB_DIR, yyyy_str);

  mon = (char *) g_hash_table_lookup(month_hash, mm_str);
  if (qtype == DAILY_QUERY) {
    if (strlen(bcode_id) > 0) {
      snprintf(tmpbuf, sizeof(tmpbuf)-1,
	  "SELECT * from filo_%s_%s WHERE bcode_id='%s'",
	  dd_str,
	  mon,
	  bcode_id);
    } else {
      sprintf(tmpbuf, "SELECT * from filo_%s_%s", dd_str, mon);
    }
    query = tmpbuf;
    fill_filo_list_query(dbname, query, &list);
    /* now show the data to list */
    w = lookup_widget(window12, "treeview4");
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w)));
    gtk_list_store_clear(store);
    sprintf(date_dd_mm, "Query Output for %s-%s", dd_str, mm_str);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
	LCOL_ID, "---",
	LCOL_NAME, date_dd_mm,
	LCOL_STAT, "------",
	LCOL_SINCE, "------",
	-1);

    append_filo_list(store, list);

    /* export it */
    if (should_export_query) {
      fp = fopen(EXPORT_QUERY_FILE, "w");
      if (!fp) {
	fprintf(stderr, "Error opening export file!\n");
      } else {
	fprintf(fp, ":%s-%s\n", dd_str, mon);
	temp = list;
	while (temp) {
	  sstat = (stuff_status_filo *) temp->data;
	  fprintf(fp, "%s|%s|%s|%s\n",
	      sstat->id,
	      sstat->name,
	      sstat->fin,
	      sstat->lout);
	  temp = temp->next;
	}
	fclose(fp);
      }
    }


    free_filo_list(list);
    g_slist_free(list);
    list = NULL;
  }
  else if (qtype == MONTHLY_QUERY) {
    w = lookup_widget(window12, "treeview4");
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w)));
    gtk_list_store_clear(store);
    fp = NULL;
    if (should_export_query) {
      fp = fopen(EXPORT_QUERY_FILE, "w");
      if (!fp) {
	fprintf(stderr, "Error opening export file!\n");
      }
    }
    for (i = 1; i <= 31; i++) {
      if (strlen(bcode_id) > 0) {
	sprintf(tmpbuf, "SELECT * from filo_%02d_%s WHERE bcode_id='%s'",
	    i, mon, bcode_id);
      } else {
	sprintf(tmpbuf, "SELECT * from filo_%02d_%s", i, mon);
      }
      query = tmpbuf;
      fill_filo_list_query(dbname, query, &list);

      if (list) {
	sprintf(date_dd_mm, "Query Output for %02d-%s", i, mm_str);
//	sprintf(date_dd_mm, "*** %02d-%s ***", i, mm_str);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
	    LCOL_ID, "---",
	    LCOL_NAME, date_dd_mm,
	    LCOL_STAT, "------",
	    LCOL_SINCE, "------",
	    -1);
	append_filo_list(store, list);
	/* export it */
	if (fp) {
	  fprintf(fp, ":%02d-%s\n", i, mon);
	  temp = list;
	  while (temp) {
	    sstat = (stuff_status_filo *) temp->data;
	    fprintf(fp, "%s|%s|%s|%s\n",
		sstat->id,
		sstat->name,
		sstat->fin,
		sstat->lout);
	    temp = temp->next;
	  }
	}

	free_filo_list(list);
	g_slist_free(list);
	list = NULL;
      }
    }
    if (fp) {
      fclose(fp);
    }
  }
  else if (qtype == YEARLY_QUERY) {
    fprintf(stderr, "Yearly query not available!\n");
  }
  else {
    fprintf(stderr, "Unknown query type %d\n", qtype);
  }

//  fprintf(stderr, "QUERY: %s\n", query);

cleanup:
  GFree(dd_str);
  GFree(mm_str);
  GFree(yyyy_str);
  GFree(bcode_id);
}
void show_unmindful_stuffs(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
//  inactivate_windows();
  present_window(window6);
}

static char print_cmd[256];
void print_list(GtkTreeView *tv, const char *suffix)
{

#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkTreeIter iter;
  GtkTreeModel *model;

  gchar *s_name = NULL;
  gchar *s_stat = NULL;
  gchar *s_sinc = NULL;

  model = gtk_tree_view_get_model(tv);
  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE) {
#ifdef _DEBUG_
    fprintf(stderr, "Tree is empty!!!\n");
#endif
    return;
  }

  char filename[256];
  FILE *fp;
  filename[sizeof(filename)-1] = '\0';
  snprintf(filename, sizeof(filename)-1,
      "%s%s", TMP_PREFIX, suffix);
  fp = fopen(filename, "w");
  if (!fp) {
    fprintf(stderr, "fopen: %s: %s\n", filename, strerror(errno));
    return;
  }

  time_t curtime;
  (void) time(&curtime);
  fprintf(fp, "Title: %s\nPrinted on %s\n", suffix, ctime(&curtime));
  fprintf(fp, "%-30s %-15s %s\n", "Stuff Name", "Stuff Status", "Since");
  fprintf(fp, "------------------------------ --------------- --------\n");

  do {
    gtk_tree_model_get(model, &iter,
       	LCOL_NAME, &s_name,
	LCOL_STAT, &s_stat,
	LCOL_SINCE, &s_sinc,
	-1);
    fprintf(fp, "%-30s %-15s %s\n",
	s_name, s_stat, s_sinc);
  } while (gtk_tree_model_iter_next(model, &iter));

  fclose(fp);

  snprintf(print_cmd, sizeof(print_cmd)-1, "%s %s", print_handler, filename);

#ifdef _WIN32
  WinExec(print_cmd, SW_SHOW);
#else
  fflush(stdout);
  fflush(stderr);
  if (should_block == 1) {
    (void) pclose(popen(print_cmd, "r"));
  } else {
    (void) popen(print_cmd, "r");
  }
#endif

  /*
  fflush(stdout);
  fflush(stderr);
  if (should_block == 1) {
    (void) pclose(popen(print_cmd, "r"));
  } else {
    (void) popen(print_cmd, "r");
  }
  */

}

static char talk_cmd[256];
void say_welcome(const char *user)
{
  if (!talk_handler_defined) {
    return;
  }
  snprintf(talk_cmd, sizeof(talk_cmd)-1, "%s Welcome %s", talk_handler, user);
#ifdef _WIN32
  WinExec(talk_cmd, SW_SHOW);
#else
  fflush(stdout);
  fflush(stderr);
  if (should_block == 1) {
    (void) pclose(popen(talk_cmd, "r"));
  } else {
    (void) popen(talk_cmd, "r");
  }
#endif
}

void say_goodbye(const char *user)
{
  if (!talk_handler_defined) {
    return;
  }
  snprintf(talk_cmd, sizeof(talk_cmd)-1, "%s Goodbye %s", talk_handler, user);
#ifdef _WIN32
  WinExec(talk_cmd, SW_SHOW);
#else
  fflush(stdout);
  fflush(stderr);
  if (should_block == 1) {
    (void) pclose(popen(talk_cmd, "r"));
  } else {
    (void) popen(talk_cmd, "r");
  }
#endif
}

void shutdown_project(void)
{
  char *tmpfile;

  /* remove temporary image files */
  if (add_window) {
    tmpfile = g_object_get_data(G_OBJECT(add_window), "photo_loc");
    if (tmpfile && !strncmp(TMP_PREFIX, tmpfile, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
      fprintf(stderr, "Removing %s\n", tmpfile);
#endif
      remove(tmpfile);
    }
  }
  if (edit_window) {
    tmpfile = g_object_get_data(G_OBJECT(edit_window), "photo_loc");
    if (tmpfile && !strncmp(TMP_PREFIX, tmpfile, strlen(TMP_PREFIX))) {
#ifdef _DEBUG_
      fprintf(stderr, "Removing %s\n", tmpfile);
#endif
      remove(tmpfile);
    }
  }
  gtk_main_quit();
  /* and we're done */
}


/* ******************************* LICENSE ***************** */

/* licensing */
void showLicenseMessage(void)
{
  GtkWidget *window = create_window10();

  show_widget(window);

}

int isLicensed(void)
{
  FILE *fp = NULL;
  unsigned char magic[SHA_DIGEST_LENGTH];
  unsigned char filemagic[SHA_DIGEST_LENGTH];

  if (access(LICENSEFILE, F_OK)) { /* file not exists */
    return 0;
  }
  else {
#ifndef _WIN32 /* for linux */
    return 1;
#endif
    char name[50]; /* client's license name */
#ifdef _USE_BIOS_DATE
    char biosdate[12]; /* date */
#else
    char product_id[50];
#endif
    fp = fopen(LICENSEFILE, "rb");
    if (!fp) {
      fprintf(stderr, "fopen failed: %s\n", LICENSEFILE);
      return 0;
    }

    memset(name, 0, sizeof(name));
    fgets(name, sizeof(name)-1, fp);
    name[strlen(name)-1] = '\0'; /* discard newline */
    strcpy(license_holder, name);

    memset(filemagic, 0, sizeof(filemagic));
    if (fread(filemagic, sizeof(filemagic), 1, fp) == 0) { /* less data */
      fprintf(stderr, "fread error\n");
      fclose(fp);
      return 0;
    }
//    fprintf(stderr, "SHA_DIGEST_LENGTH = %d\n", SHA_DIGEST_LENGTH);
//    fprintf(stderr, "filemagic = %s[%d]\n", filemagic, strlen(filemagic));

    fclose(fp);

#ifdef _USE_BIOS_DATE
    memset(biosdate, 0, sizeof(biosdate));
#else
    memset(product_id, 0, sizeof(product_id));
#endif

#ifdef _WIN32
#ifdef _USE_BIOS_DATE
    if (!getBiosDate(biosdate, sizeof(biosdate)-1)) { /* not found */
#else /* NOT _USE_BIOS_DATE */
    if (!getProductId(product_id, sizeof(product_id)-1)) { /* not found */
#endif /* !_USE_BIOS_DATE */
      fprintf(stderr, "getProductId error\n");
      return 0;
    }
#endif /* !_WIN32 */

//    fprintf(stderr, "biosdate = |%s|[%d]\n", biosdate, strlen(biosdate));
    strcpy(tmpbuf, name);
    strcat(tmpbuf, SALT);

#ifdef _USE_BIOS_DATE
    strcat(tmpbuf, biosdate);
#else
    strcat(tmpbuf, product_id);
#endif

#ifdef _DEBUG_
    fprintf(stderr, "name&license: %s\n", tmpbuf);
#endif

    memset(magic, 0, sizeof(magic));
    computeMagic(tmpbuf, magic);
    /*
    fprintf(stderr, "computed magic: |");
    fwrite(magic, sizeof(magic), 1, stderr);
    fprintf(stderr, "|\n");
    */

    if (memcmp(magic, filemagic, sizeof(magic))) { /* mismatch */
      fprintf(stderr, "magic mismatch\n");
      return 0;
    }
    return 1;
  }

}

void computeMagic(const char *info, unsigned char *md_value)
{
  SHA_CTX shactx;

  SHA1_Init(&shactx);
  SHA1_Update(&shactx, info, strlen(info));
  SHA1_Final(md_value, &shactx);
}


#ifdef _WIN32
#ifdef _USE_BIOS_DATE
int getBiosDate(char *buf, size_t size)
{
  HKEY key;
  DWORD type;

  int len;
  int i;
  int j;

  DWORD bufsize = size;
  char lbuf[20]; /* date */

  LONG errorcode;

  if ((errorcode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
	  "SYSTEM\\CurrentControlSet\\Control\\Biosinfo",
	  0,
	  KEY_READ,
	  &key)) != ERROR_SUCCESS) {
    return 0; /* not found, error */
  }
  if ((errorcode = RegQueryValueEx(key, "SystemBiosDate",
	  NULL,
	  &type,
	  lbuf, &bufsize)) != ERROR_SUCCESS) {
    RegCloseKey(key);
    return 0;
  }

  len = strlen(lbuf);
  j = 0;
  for (i = 0; i <= len; i++) {
    if (lbuf[i] != '/') {
      buf[j] = lbuf[i];
      j++;
    }
  }


  RegCloseKey(key);

  return 1;

}
#else /* product id */
int getProductId(char *buf, size_t size)
{
  HKEY key;
  char buffer[50];
  char client_id[50];

  DWORD bufsize = sizeof(buffer);
  DWORD type;

  LONG errorcode;

  char *token;

  if ((errorcode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
       	"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
       	0,
       	KEY_READ,
       	&key)) == ERROR_SUCCESS) {
//    fprintf(stderr, "Key opened\n");
    if ((errorcode = RegQueryValueEx(key, "ProductId",
	  NULL,
	  &type,
	  buffer, &bufsize)) == ERROR_SUCCESS) {
//      fprintf(stderr, "value found: %s\n", buffer);
      memset(client_id, 0, sizeof(client_id));

      token = strtok(buffer, "-");
      while (token != NULL) {
	strcat(client_id, token);
	token = strtok(NULL, "-"); /* get next token */
      }
      strncpy(buf, client_id, size);
//      fprintf(stderr, "client_id : %s (%d)\n", buf, strlen(buf));
    }
    else {
      fprintf(stderr, "value query failed, error %ld\n", errorcode);
      return 0;
    }
    RegCloseKey(key);
  }
  else {
    fprintf(stderr, "Key open failed, error %ld\n", errorcode);
    return 0;
  }

  return 1;
}
#endif /* !_USE_BIOS_DATE */
#endif /* !_WIN32 */
