#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "myping4-gtk.h"

extern char hname[];
extern int lookup_host;

gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();

  return FALSE;
}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_main_quit();

}


void
on_button12_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  clear_log("textview2");

}


void
on_entry1_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
  strncpy(hname, gtk_entry_get_text(entry), 80);
  lookup_host = 1;
}


void
on_button13_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *entry = lookup_widget(GTK_WIDGET(button), "entry1");
  strncpy(hname, gtk_entry_get_text(GTK_ENTRY(entry)), 80);
  lookup_host = 1;
}

