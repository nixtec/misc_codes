#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "parapin.h"

extern int pin;

gboolean
on_window1_delete_event                (GtkWidget       *widget,
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
on_radiobutton1_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN02;

}


void
on_radiobutton2_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN03;

}


void
on_radiobutton3_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN04;

}


void
on_radiobutton4_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN05;

}


void
on_radiobutton5_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN06;

}


void
on_radiobutton6_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN07;

}


void
on_radiobutton7_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN08;

}


void
on_radiobutton8_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN09;

}


void
on_radiobutton9_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN10;

}


void
on_radiobutton10_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN11;

}


void
on_radiobutton11_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN12;

}


void
on_radiobutton12_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN13;

}


void
on_radiobutton13_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
#ifdef _DEBUG_
  g_print("%s\n", __func__);
#endif

  if (gtk_toggle_button_get_active(togglebutton) == FALSE)
    return;

  pin = LP_PIN15;

}

