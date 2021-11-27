/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

#ifdef _WIN32
#include <windows.h>
#include <wincon.h>

#define LOGFILE "manage.log"
#define ERRFILE "manage.err"
#endif	/* !_WIN32 */

void runproject(GtkWidget *window);

static GtkWidget *window1;

int
main (int argc, char *argv[])
{

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

//  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
  add_pixmap_directory (".");
  gtk_rc_add_default_file("theme.default/gtk-2.0/gtkrc");

#ifdef _WIN32
  freopen(LOGFILE, "w", stdout);
  freopen(ERRFILE, "w", stderr);
  FreeConsole();
#endif

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  window1 = create_window1 ();
  gtk_widget_show (window1);

  runproject(window1);

  gtk_main ();
  return 0;
}


void runproject(GtkWidget *window)
{
  GtkWidget *ptr = lookup_widget(window, "frame1");
  if (GTK_WIDGET_IS_SENSITIVE(ptr))
    gtk_widget_set_sensitive(ptr, FALSE);

}

void give_admin_access(void)
{
  GtkWidget *ptr;

  ptr = lookup_widget(window1, "frame1");
  if (!GTK_WIDGET_IS_SENSITIVE(ptr))
    gtk_widget_set_sensitive(ptr, TRUE);

  ptr = lookup_widget(window1, "entry1");
  gtk_entry_set_text(GTK_ENTRY(ptr), "");
  if (GTK_WIDGET_IS_SENSITIVE(ptr))
    gtk_widget_set_sensitive(ptr, FALSE);

  ptr = lookup_widget(window1, "entry2");
  gtk_widget_grab_focus(ptr);

}