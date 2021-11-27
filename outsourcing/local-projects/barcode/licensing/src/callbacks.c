#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "myfuncs.h"


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
  buildLicense(GTK_WIDGET(button));

}

