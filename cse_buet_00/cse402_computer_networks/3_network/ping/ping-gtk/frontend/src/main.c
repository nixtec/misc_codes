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

#include "myping4-gtk.h"

GtkWidget *window1;

extern struct addrinfo *ai; /* defined in myping4-gtk.c */
extern struct proto *pr; /* ,, */
extern struct proto proto_v4; /* ,, */
extern int datalen;


extern int sockfd; /* defined in ping-gtk.c */

int
main (int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
    exit(1);
  }

  init_hostname(argv[1]);
  if (!ai) {
    fprintf(stderr, "Hostname lookup failure\n");
    exit(2);
  }

  printf("PING %s (%s) : %d data bytes\n", ai->ai_canonname,
     inet_ntoa(pr->sasend->sin_addr), datalen);

  sockfd = socket(pr->sasend->sin_family, SOCK_RAW, pr->icmpproto);
  if (sockfd == -1) {
    fprintf(stderr, "%s: socket: %s\n", __func__, strerror(errno));
    exit(1);
  }
  setuid(getuid()); /* we need not be root from now */

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
  add_pixmap_directory ("../pixmaps");

  gtk_rc_add_default_file("theme/gtk-2.0/gtkrc");

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  window1 = create_window1 ();
  gtk_widget_show (window1);

  myping4_gtk(argv[1]);

  gtk_main ();

  return 0;
}
