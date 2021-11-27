#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <stdio.h>
#include <string.h>

#include "mysdb.h"

extern void give_admin_access(void); /* main.c */
extern int verify_pass(const char *pass, int type);

gboolean
on_window10_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();

  return FALSE;
}


void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_main_quit();

}


void
on_button2_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *ptr, *ptr1;
  gchar *pass;
  int type;

  ptr = lookup_widget(GTK_WIDGET(button), "entry2");
  pass = g_strdup(gtk_entry_get_text(GTK_ENTRY(ptr)));
  if (strlen(pass) == 0) {
    g_free(pass);
  } else {
    type = TYPE_PASS_ADMIN;
    if (update_pass(pass, type) != 0) {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Error performing request!</b>");
    } else {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Operating successful</b>");
      ptr1 = lookup_widget(GTK_WIDGET(button), "entry2");
      gtk_entry_set_text(GTK_ENTRY(ptr1), "");
    }
    g_free(pass);
  }

  ptr = lookup_widget(GTK_WIDGET(button), "entry3");
  pass = g_strdup(gtk_entry_get_text(GTK_ENTRY(ptr)));
  if (strlen(pass) == 0) {
    g_free(pass);
  } else {
    type = TYPE_PASS_ENTRY;
    if (update_pass(pass, type) != 0) {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Error performing request!</b>");
    } else {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Operating successful</b>");
      ptr1 = lookup_widget(GTK_WIDGET(button), "entry3");
      gtk_entry_set_text(GTK_ENTRY(ptr1), "");
    }
    g_free(pass);
  }

  ptr = lookup_widget(GTK_WIDGET(button), "entry4");
  pass = g_strdup(gtk_entry_get_text(GTK_ENTRY(ptr)));
  if (strlen(pass) == 0) {
    g_free(pass);
  } else {
    type = TYPE_PASS_ADD;
    if (update_pass(pass, type) != 0) {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Error performing request!</b>");
    } else {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Operating successful</b>");
      ptr1 = lookup_widget(GTK_WIDGET(button), "entry4");
      gtk_entry_set_text(GTK_ENTRY(ptr1), "");
    }
    g_free(pass);
  }

  ptr = lookup_widget(GTK_WIDGET(button), "entry5");
  pass = g_strdup(gtk_entry_get_text(GTK_ENTRY(ptr)));
  if (strlen(pass) == 0) {
    g_free(pass);
  } else {
    type = TYPE_PASS_EDIT;
    if (update_pass(pass, type) != 0) {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Error performing request!</b>");
    } else {
      ptr1 = lookup_widget(GTK_WIDGET(button), "label6");
      gtk_label_set_markup(GTK_LABEL(ptr1), "<b>Operating successful</b>");
      ptr1 = lookup_widget(GTK_WIDGET(button), "entry5");
      gtk_entry_set_text(GTK_ENTRY(ptr1), "");
    }
    g_free(pass);
  }

}


void
on_entry1_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
  GtkWidget *widget;

  gchar *pass = g_strdup(gtk_entry_get_text(entry));
  if (strlen(pass) == 0) {
    g_free(pass);
    return;
  } else {
    if (verify_pass(pass, TYPE_PASS_ADMIN) == 0) {
      widget = lookup_widget(GTK_WIDGET(entry), "label6");
      gtk_label_set_markup(GTK_LABEL(widget),
	  "<b>Authentication Successful!</b>");
      give_admin_access();
    } else {
      widget = lookup_widget(GTK_WIDGET(entry), "label6");
      gtk_label_set_markup(GTK_LABEL(widget), "<b>Authentication Failure!</b>");
      gtk_widget_grab_focus(GTK_WIDGET(entry));
    }
    g_free(pass);
  }

}

