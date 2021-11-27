#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "myfuncs.h"
#include "mysdb.h"
#include "mybdb.h"

//static gboolean add_window_requested;
//static gboolean edit_window_requested;

gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  confirm_exit();
//  gtk_main_quit();

  return TRUE;
}


void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  confirm_exit();

}


void
on_button2_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  update_list_window();

}


void
on_button3_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

}


void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  confirm_exit();
//  gtk_main_quit();

}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  show_about_dialog();

}


gboolean
on_aboutdialog1_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (GTK_WIDGET_VISIBLE(widget))
    gtk_widget_hide(widget);

  return TRUE;
}


gboolean
on_window2_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  hide_widget(widget);
//  return FALSE;
  /* ignore closing this window */
  return TRUE;
}


void
on_entry1_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  char *stuff_id;
  int errval;

  stuff_id = strdup(gtk_entry_get_text(entry));
  if (strlen(stuff_id) > 0) {
    if ((errval = toggle_stuff_status(stuff_id)) == 0) {
      /* if database updated, update the list in window */
      update_list_window(); /* update entry in window1 */
      display_main_window_contents(stuff_id);
    } else {
      switch (errval) {
	case E_NO_STUFF:
	  show_message(_("Error: No such Stuff!"));
	  break;
	case E_NO_DB:
	  show_message(_("Error: Database error!"));
	  break;
	case E_EXEC_SQL:
	  show_message(_("Error: Executing query!"));
	  break;
	default:
	  show_message(_("Error: unknown!"));
	  break;
      }
    }
  } else {
#ifdef _DEBUG_
    fprintf(stderr, "Entry = NULL, ignoring. %s:%d\n", __FILE__, __LINE__);
#endif
  }
  free(stuff_id);

  clear_entry(entry);

}


void
on_toolbutton1_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  confirm_exit();
//  ask_yes_no(_("Are you sure?"));

}


void
on_closebutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "dialog1");
  hide_widget(widget);

  activate_windows();

}


/* never be called */
void
on_dialog1_close                       (GtkDialog       *dialog,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  hide_widget(dialog);

  activate_windows();
}


gboolean
on_dialog1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  hide_widget(widget);

  activate_windows();

  return TRUE;
}


gboolean
on_window3_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  hide_widget(widget);

  exit_confirmed(FALSE);

  return TRUE;
}


void
on_button4_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "window3");
  hide_widget(widget);
  exit_confirmed(FALSE);

}


void
on_button5_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "window3");
  hide_widget(widget);
  exit_confirmed(TRUE);

}


gboolean
on_window4_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  int data = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "type"));
  if (data == ADD_STUFF) {
    add_stuff_window_closed(); /* filechooser should be hidden too */
  } else {
    edit_stuff_window_closed(); /* filechooser should be hidden too */
  }
  another_window_running--;

  hide_widget(widget);
  show_barcode_entry_window(); /* window2 */

  return TRUE;
}


void
on_entry2_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  char *id = strdup(get_entry(entry));
  if (strlen(id) != 0) {
    load_stuff_info(GTK_WIDGET(entry), id);
  }
  free(id);

}


void
on_button6_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "window4");
  int data = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "type"));
  if (data == ADD_STUFF) {
    add_stuff_window_closed(); /* filechooser should be hidden too */
  } else {
    edit_stuff_window_closed(); /* filechooser should be hidden too */
  }
  another_window_running--;

  hide_widget(widget);
  show_barcode_entry_window(); /* window2 */

}


void
on_button7_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "window4");
  int data = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "type"));
  if (data == ADD_STUFF) {
    save_add_stuff();
  } else {
    save_edit_stuff();
  }

}


void
on_toolbutton3_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (db_add_pass_required == TRUE) {
    ask_for_db_add_pass();
  } else {
    add_stuff_window();
  }

}


void
on_toolbutton4_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  if (db_edit_pass_required == TRUE) {
    ask_for_db_edit_pass();
  } else {
    edit_stuff_window();
  }

}


void
on_entry3_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

}


gboolean
on_filechooserdialog1_delete_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  hide_widget(widget);

  return TRUE;
}


void
on_button9_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *dialog = lookup_widget(GTK_WIDGET(button), "filechooserdialog1");
  hide_widget(dialog);

}


void
on_button10_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkWidget *dialog = lookup_widget(GTK_WIDGET(button), "filechooserdialog1");
  int data = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(dialog), "type"));
  if (data == ADD_STUFF)
    load_add_photo();
  else
    load_edit_photo();

}


void
on_button8_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *window = lookup_widget(GTK_WIDGET(button), "window4");
  int data = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "type"));
  if (data == ADD_STUFF)
    load_add_file_chooser();
  else
    load_edit_file_chooser();

}


void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkWidget *window = lookup_widget(GTK_WIDGET(button), "window4");
  clear_window4(window);

}


void
on_button12_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkWidget *window = lookup_widget(GTK_WIDGET(button), "window4");
  clear_window4_image(window);

}


void
on_toolbutton5_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  extended_query_window();

}


gboolean
on_window5_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  hide_widget(widget);
  another_window_running--;
  show_barcode_entry_window(); /* window2 */

  return TRUE;
}


void
on_treeview1_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  /* path is zero indexed [the index of the row]*/
#ifdef _DEBUG_
  fprintf(stderr, "path = %s\n", gtk_tree_path_to_string(path));
#endif

  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  char *id;

  selection = gtk_tree_view_get_selection(treeview);
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, LCOL_ID, &id, -1);
#ifdef _DEBUG_
    fprintf(stderr, "%s selected\n", id);
#endif
    display_main_window_contents(id);
    g_free(id);
  }


}


void
on_button13_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkWidget *window = lookup_widget(GTK_WIDGET(button), "window5");
  hide_widget(window);
  another_window_running--;
  show_barcode_entry_window(); /* window2 */

}


void
on_button14_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  run_extended_query();

}


void
on_toolbutton6_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  show_barcode_entry_window();

}


void
on_toolbutton7_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  show_unmindful_stuffs();
}


gboolean
on_window6_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  hide_widget(widget);

  return TRUE;
}


void
on_treeview3_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{

}


void
on_button15_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "window6");
  hide_widget(widget);

}


void
on_button16_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  update_unmindful_window();

}


void
on_entry7_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *window = lookup_widget(GTK_WIDGET(entry), "window5");
  gtk_window_activate_default(GTK_WINDOW(window));

}


void
on_button17_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  char *suffix = "today_history.txt";
  GtkWidget *tv = lookup_widget(GTK_WIDGET(button), "treeview1");
  print_list(GTK_TREE_VIEW(tv), suffix); /* tmp file will have this suffix */

}


void
on_button18_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  char *suffix = "extended_query.txt";
  GtkWidget *tv = lookup_widget(GTK_WIDGET(button), "treeview2");
  print_list(GTK_TREE_VIEW(tv), suffix); /* tmp file will have this suffix */

}



gboolean
on_window8_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return TRUE;
}


void
on_button19_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  gtk_main_quit();

}

void
on_button20_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *ptr;
  char *pass;
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  /* check for master password here */
  ptr = lookup_widget(GTK_WIDGET(button), "entry8");
  pass = g_strdup(get_entry(ptr));
  if (strlen(pass) == 0) {
    g_free(pass);
    return;
  }

  if (verify_pass(pass, TYPE_PASS_ENTRY) == 0) {
    clear_entry(ptr);
    ptr = lookup_widget(GTK_WIDGET(button), "window8");
    hide_widget(ptr);
    runproject();
  } else {
    ptr = lookup_widget(GTK_WIDGET(button), "entry8");
    focus_widget(ptr);
    show_message(_("Authentication failure!"));
  }
}


gboolean
on_window9_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  hide_widget(widget);

  return TRUE;
}


void
on_button21_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *ptr = lookup_widget(GTK_WIDGET(button), "window9");
  hide_widget(ptr);

}


void
on_button22_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *ptr;
  char *pass;
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  /* check for master password here */
  ptr = lookup_widget(GTK_WIDGET(button), "entry9");
  pass = g_strdup(get_entry(ptr));
  if (strlen(pass) == 0) {
    g_free(pass);
    return;
  }

  if (verify_pass(pass, TYPE_PASS_ADD) == 0) {
    db_add_pass_required = FALSE;
    clear_entry(ptr);
    ptr = lookup_widget(GTK_WIDGET(button), "window9");
    hide_widget(ptr);
    add_stuff_window();
  } else {
    db_add_pass_required = TRUE;
    ptr = lookup_widget(GTK_WIDGET(button), "entry9");
    focus_widget(ptr);
    just_show_message(_("Authentication failure!"));
  }

}


void
on_button23_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  GtkWidget *ptr = lookup_widget(GTK_WIDGET(button), "window11");
  hide_widget(ptr);

}


void
on_button24_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *ptr;
  char *pass;
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  /* check for master password here */
  ptr = lookup_widget(GTK_WIDGET(button), "entry10");
  pass = g_strdup(get_entry(ptr));
  if (strlen(pass) == 0) {
    g_free(pass);
    return;
  }

  if (verify_pass(pass, TYPE_PASS_EDIT) == 0) {
    db_edit_pass_required = FALSE;
    clear_entry(ptr);
    ptr = lookup_widget(GTK_WIDGET(button), "window11");
    hide_widget(ptr);
    edit_stuff_window();
  } else {
    db_edit_pass_required = TRUE;
    ptr = lookup_widget(GTK_WIDGET(button), "entry10");
    focus_widget(ptr);
    just_show_message(_("Authentication failure!"));
  }

}


gboolean
on_window11_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif
  hide_widget(widget);

  return TRUE;
}


void
on_toolbutton8_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  filo_window();


}


gboolean
on_window12_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  hide_widget(widget);
  another_window_running--;
  show_barcode_entry_window(); /* window2 */

  return TRUE;
}


void
on_button25_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  run_filo_query();


}


void
on_button26_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  GtkWidget *window = lookup_widget(GTK_WIDGET(button), "window12");
  hide_widget(window);
  another_window_running--;
  show_barcode_entry_window(); /* window2 */


}


void
on_button27_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  fprintf(stderr, "<%s> %s:%d\n", __func__, __FILE__, __LINE__);
#endif

  char *suffix = "filo_query.txt";
  GtkWidget *tv = lookup_widget(GTK_WIDGET(button), "treeview4");
  print_list(GTK_TREE_VIEW(tv), suffix); /* tmp file will have this suffix */


}

