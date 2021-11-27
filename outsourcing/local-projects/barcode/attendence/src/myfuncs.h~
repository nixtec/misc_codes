#ifndef _MYFUNCS_H_
#define _MYFUNCS_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <openssl/sha.h>

#ifdef _WIN32
#include <windows.h>
#include <wincon.h>	/* FreeConsole() */
#include <winreg.h>
#endif

#define LICENSEFILE "attendence.lcn"
#define LOGFILE "attendence.log"
#define ERRFILE "attendence.err"

#define SALT "attendence"

#include <gtk/gtk.h>	/* gtk specific features */

#include "mybdb.h"	/* bdb database routines */
#include "mysdb.h"	/* sql database routines */

typedef struct {
  gchar *id;
  gchar *name;
  gchar *stat; /* IN / OUT */
  gchar *since; /* since xxxx (s)he is in/out */
  gchar *duration;
} lcol; /* list column */

enum {
  LCOL_ID,
  LCOL_NAME,
  LCOL_STAT,
  LCOL_SINCE,
  LCOL_DURATION,	/* how long the stuff was OUT */
  LCOL_ITEMS
};

enum {
  FILO_ID,
  FILO_NAME,
  FILO_FIN,
  FILO_LOUT,
  FILO_ITEMS
};


#define hide_widget(widget) \
  if (GTK_WIDGET_VISIBLE(GTK_WIDGET(widget))) \
    gtk_widget_hide(GTK_WIDGET(widget))

#define show_widget(widget) \
  if (!GTK_WIDGET_VISIBLE(GTK_WIDGET(widget))) \
    gtk_widget_show(GTK_WIDGET(widget))

#define present_window(window) \
  gtk_window_present(GTK_WINDOW(window))

#define get_entry(entry) \
  gtk_entry_get_text(GTK_ENTRY(entry))

#define set_entry(entry, text) \
  gtk_entry_set_text(GTK_ENTRY(entry), text)

#define clear_entry(widget) \
  gtk_entry_set_text(GTK_ENTRY(widget), "")

#define focus_widget(widget) \
  if (GTK_WIDGET_CAN_FOCUS(GTK_WIDGET(widget))) \
    gtk_widget_grab_focus(GTK_WIDGET(widget))

#define make_sensitive(widget) \
  if (!GTK_WIDGET_IS_SENSITIVE(GTK_WIDGET(widget))) \
    gtk_widget_set_sensitive(GTK_WIDGET(widget), TRUE)

#define make_insensitive(widget) \
  if (GTK_WIDGET_IS_SENSITIVE(GTK_WIDGET(widget))) \
    gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE)

#define GFree(x) if (x) g_free(x)

#define ADD_STUFF  0
#define EDIT_STUFF 1

#ifdef _WIN32
#define MKDIR(dir,mode) mkdir(dir)
#else
#define MKDIR(dir,mode) mkdir(dir,mode)
#endif

#ifdef _WIN32
#define EXPORT_QUERY_FILE "c:/att_exported.txt"
#define TMP_PREFIX "c:/att_"
#else
#define TMP_PREFIX "/tmp/att_"
#define EXPORT_QUERY_FILE "/tmp/att_exported.txt"
#endif

extern char print_handler[]; /* main() will initialize this */
extern char talk_handler[]; /* main() will initialize this */
extern int should_block; /* execution of external progs should block? */
extern int should_export_query;	/* should we export the query to text file */
extern int talk_handler_defined; /* talk handler defined? */

extern int another_window_running;
extern gboolean db_add_pass_required;	/* add password */
extern gboolean db_edit_pass_required;	/* edit password */

extern void runproject(void); /* entry point of the project */
extern void show_message(const char *msg); /* msg may be pango markup */
extern void ask_yes_no(const char *msg);
extern void update_list_window(void);
extern void update_unmindful_window(void);
extern void show_about_dialog(void);
extern void inactivate_windows(void);
extern void activate_windows(void);
extern void confirm_exit(void);
extern void exit_confirmed(gboolean confirmed);
extern void add_stuff_window(void);
extern void edit_stuff_window(void);
extern void show_barcode_entry_window(void);
extern void hide_barcode_entry_window(void);
extern void add_stuff_window_closed(void);
extern void edit_stuff_window_closed(void);

extern void load_add_file_chooser(void);
extern void load_edit_file_chooser(void);
extern void load_add_photo(void);
extern void load_edit_photo(void);
extern void save_add_stuff(void);
extern void save_edit_stuff(void);
extern void clear_window4(GtkWidget *window);
extern void clear_window4_image(GtkWidget *window);
extern void copy_photo(const char *src, const char *dest);
extern void store_photo(const char *fliename, stuff_info *sinfo);
extern void load_stuff_info(GtkWidget *widget, const char *id);
extern void display_main_window_contents(const char *id);
extern void extended_query_window(void);
extern void filo_window(void);
extern void run_extended_query(void);
extern void run_filo_query(void);
extern void show_unmindful_stuffs(void);
extern void print_list(GtkTreeView *tv, const char *suffix);
extern void shutdown_project(void);
extern void ask_for_master_pass(void);
extern void ask_for_db_add_pass(void);
extern void ask_for_db_edit_pass(void);
extern int init_env(void);
extern void just_show_message(const char *msg);
extern void say_welcome(const char *username);
extern void say_goodbye(const char *username);


#ifdef _WIN32
int getBiosDate(char *buf, size_t size); /* ret: 0 (not found), 1 (found) */
int getProductId(char *buf, size_t size);
#endif

/* LICENSE */
int isLicensed(void);
void showLicenseMessage(void);

void computeMagic(const char *info, unsigned char *md_value);

#endif
