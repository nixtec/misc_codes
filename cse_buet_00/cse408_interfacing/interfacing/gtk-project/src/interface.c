/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window1 (void)
{
  GtkWidget *window1;
  GdkPixbuf *window1_icon_pixbuf;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *alignment1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *vbox4;
  GtkWidget *label3;
  GtkWidget *hseparator1;
  GtkWidget *hbox1;
  GtkWidget *vbox2;
  GtkWidget *radiobutton1;
  GSList *radiobutton1_group = NULL;
  GtkWidget *radiobutton2;
  GtkWidget *radiobutton3;
  GtkWidget *radiobutton4;
  GtkWidget *radiobutton5;
  GtkWidget *radiobutton6;
  GtkWidget *radiobutton7;
  GtkWidget *radiobutton8;
  GtkWidget *vbox3;
  GtkWidget *radiobutton9;
  GtkWidget *radiobutton10;
  GtkWidget *radiobutton11;
  GtkWidget *radiobutton12;
  GtkWidget *radiobutton13;
  GtkWidget *hseparator2;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window1, 300, -1);
  gtk_container_set_border_width (GTK_CONTAINER (window1), 5);
  gtk_tooltips_set_tip (tooltips, window1, _("Written by Ayub <mrayub@gmail.com>"), NULL);
  gtk_window_set_title (GTK_WINDOW (window1), _("Parallel Pin Status Check"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  window1_icon_pixbuf = create_pixbuf ("red.png");
  if (window1_icon_pixbuf)
    {
      gtk_window_set_icon (GTK_WINDOW (window1), window1_icon_pixbuf);
      gdk_pixbuf_unref (window1_icon_pixbuf);
    }

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame1, -1, 80);
  gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);

  image1 = create_pixmap (window1, NULL);
  gtk_widget_show (image1);
  gtk_container_add (GTK_CONTAINER (alignment1), image1);

  label1 = gtk_label_new (_("<b>Signal Status</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox4, FALSE, FALSE, 5);

  label3 = gtk_label_new (_("<b>Interval:</b>"));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox4), label3, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label3), TRUE);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox4), hseparator1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 9);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);

  radiobutton1 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 2"));
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton1, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton1, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton1), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton1));

  radiobutton2 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 3"));
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton2, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton2, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton2), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton2));

  radiobutton3 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 4"));
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton3, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton3, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton3), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton3));

  radiobutton4 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 5"));
  gtk_widget_show (radiobutton4);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton4, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton4, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton4), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton4));

  radiobutton5 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 6"));
  gtk_widget_show (radiobutton5);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton5, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton5, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton5), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton5));

  radiobutton6 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 7"));
  gtk_widget_show (radiobutton6);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton6, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton6, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton6), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton6));

  radiobutton7 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 8"));
  gtk_widget_show (radiobutton7);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton7, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton7, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton7), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton7));

  radiobutton8 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 9"));
  gtk_widget_show (radiobutton8);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton8, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton8, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton8), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton8));

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox3, TRUE, TRUE, 0);

  radiobutton9 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 10"));
  gtk_widget_show (radiobutton9);
  gtk_box_pack_start (GTK_BOX (vbox3), radiobutton9, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton9, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton9), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton9));

  radiobutton10 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 11"));
  gtk_widget_show (radiobutton10);
  gtk_box_pack_start (GTK_BOX (vbox3), radiobutton10, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton10, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton10), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton10));

  radiobutton11 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 12"));
  gtk_widget_show (radiobutton11);
  gtk_box_pack_start (GTK_BOX (vbox3), radiobutton11, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton11, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton11), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton11));

  radiobutton12 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 13"));
  gtk_widget_show (radiobutton12);
  gtk_box_pack_start (GTK_BOX (vbox3), radiobutton12, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton12, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton12), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton12));

  radiobutton13 = gtk_radio_button_new_with_mnemonic (NULL, _("Pin 15"));
  gtk_widget_show (radiobutton13);
  gtk_box_pack_start (GTK_BOX (vbox3), radiobutton13, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobutton13, _("Check this pin"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton13), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton13));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton13), TRUE);

  hseparator2 = gtk_hseparator_new ();
  gtk_widget_show (hseparator2);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator2, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 5);
  gtk_container_set_border_width (GTK_CONTAINER (hbuttonbox1), 2);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  button1 = gtk_button_new_from_stock ("gtk-quit");
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  gtk_container_set_border_width (GTK_CONTAINER (button1), 2);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, button1, _("Terminate application"), NULL);

  g_signal_connect ((gpointer) window1, "delete_event",
                    G_CALLBACK (on_window1_delete_event),
                    NULL);
  g_signal_connect ((gpointer) radiobutton1, "toggled",
                    G_CALLBACK (on_radiobutton1_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton2, "toggled",
                    G_CALLBACK (on_radiobutton2_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton3, "toggled",
                    G_CALLBACK (on_radiobutton3_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton4, "toggled",
                    G_CALLBACK (on_radiobutton4_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton5, "toggled",
                    G_CALLBACK (on_radiobutton5_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton6, "toggled",
                    G_CALLBACK (on_radiobutton6_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton7, "toggled",
                    G_CALLBACK (on_radiobutton7_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton8, "toggled",
                    G_CALLBACK (on_radiobutton8_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton9, "toggled",
                    G_CALLBACK (on_radiobutton9_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton10, "toggled",
                    G_CALLBACK (on_radiobutton10_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton11, "toggled",
                    G_CALLBACK (on_radiobutton11_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton12, "toggled",
                    G_CALLBACK (on_radiobutton12_toggled),
                    NULL);
  g_signal_connect ((gpointer) radiobutton13, "toggled",
                    G_CALLBACK (on_radiobutton13_toggled),
                    NULL);
  g_signal_connect ((gpointer) button1, "clicked",
                    G_CALLBACK (on_button1_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window1, window1, "window1");
  GLADE_HOOKUP_OBJECT (window1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (window1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (window1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (window1, image1, "image1");
  GLADE_HOOKUP_OBJECT (window1, label1, "label1");
  GLADE_HOOKUP_OBJECT (window1, vbox4, "vbox4");
  GLADE_HOOKUP_OBJECT (window1, label3, "label3");
  GLADE_HOOKUP_OBJECT (window1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (window1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (window1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (window1, radiobutton1, "radiobutton1");
  GLADE_HOOKUP_OBJECT (window1, radiobutton2, "radiobutton2");
  GLADE_HOOKUP_OBJECT (window1, radiobutton3, "radiobutton3");
  GLADE_HOOKUP_OBJECT (window1, radiobutton4, "radiobutton4");
  GLADE_HOOKUP_OBJECT (window1, radiobutton5, "radiobutton5");
  GLADE_HOOKUP_OBJECT (window1, radiobutton6, "radiobutton6");
  GLADE_HOOKUP_OBJECT (window1, radiobutton7, "radiobutton7");
  GLADE_HOOKUP_OBJECT (window1, radiobutton8, "radiobutton8");
  GLADE_HOOKUP_OBJECT (window1, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (window1, radiobutton9, "radiobutton9");
  GLADE_HOOKUP_OBJECT (window1, radiobutton10, "radiobutton10");
  GLADE_HOOKUP_OBJECT (window1, radiobutton11, "radiobutton11");
  GLADE_HOOKUP_OBJECT (window1, radiobutton12, "radiobutton12");
  GLADE_HOOKUP_OBJECT (window1, radiobutton13, "radiobutton13");
  GLADE_HOOKUP_OBJECT (window1, hseparator2, "hseparator2");
  GLADE_HOOKUP_OBJECT (window1, hbuttonbox1, "hbuttonbox1");
  GLADE_HOOKUP_OBJECT (window1, button1, "button1");
  GLADE_HOOKUP_OBJECT_NO_REF (window1, tooltips, "tooltips");

  return window1;
}
