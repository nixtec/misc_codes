#include "support.h"
#include "myfuncs.h"


void computeMagic(const char *info, unsigned char *md_value)
{
  SHA_CTX shactx;

  SHA1_Init(&shactx);
  SHA1_Update(&shactx, info, strlen(info));
  SHA1_Final(md_value, &shactx);
}


void buildLicense(GtkWidget *widget)
{
  static char name1[40];

  static char buffer[128];
  static unsigned char magic[SHA_DIGEST_LENGTH];

  FILE *fp = NULL;

  char *name = NULL;
  char *code = NULL;

  GtkWidget *ptr = NULL;

  GtkWidget *entry1 = lookup_widget(widget, "entry1");
  name = strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
  if (strlen(name) <= 0) {
    ptr = lookup_widget(widget, "label3");
    gtk_label_set_text(GTK_LABEL(ptr), "Name NIL not allowed!");
    goto cleanup;
  }
  gtk_entry_set_text(GTK_ENTRY(entry1), "");

  ptr = lookup_widget(widget, "label3");
  gtk_label_set_text(GTK_LABEL(ptr), "");

  GtkWidget *entry2 = lookup_widget(widget, "entry2");
  code = strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
  if (strlen(code) <= 0) {
    ptr = lookup_widget(widget, "label3");
    gtk_label_set_text(GTK_LABEL(ptr), "Code NIL not allowed!");
    goto cleanup;
  }
  gtk_entry_set_text(GTK_ENTRY(entry2), "");
  gtk_widget_grab_focus(entry1);

  ptr = lookup_widget(widget, "label3");
  gtk_label_set_text(GTK_LABEL(ptr), "");

  memset(buffer, 0, sizeof(buffer));

  strcpy(buffer, name);
  strcat(buffer, code);

//  fprintf(stderr, "Computing magic for |%s|\n", buffer);

  memset(magic, 0, sizeof(magic));
  computeMagic(buffer, magic);

  fp = fopen("license", "wb");

  if (!fp) {
    perror("fopen");
    ptr = lookup_widget(widget, "label3");
    gtk_label_set_text(GTK_LABEL(ptr), "Error opening output file!");
    goto cleanup;
  }
  ptr = lookup_widget(widget, "label3");
  gtk_label_set_text(GTK_LABEL(ptr), "");

  strcpy(name1, name);
  strcat(name1, "\n");
  if (fwrite(name1, strlen(name1), 1, fp) == 0) {
    perror("fwrite");
    ptr = lookup_widget(widget, "label3");
    gtk_label_set_text(GTK_LABEL(ptr), "Error writing on output file!");
    goto cleanup;
  }
  if (fwrite(magic, sizeof(magic), 1, fp) == 0) {
    perror("fwrite");
    ptr = lookup_widget(widget, "label3");
    gtk_label_set_text(GTK_LABEL(ptr), "Error writing on output file!");
    goto cleanup;
  }
  ptr = lookup_widget(widget, "label3");
  gtk_label_set_text(GTK_LABEL(ptr), "Information written successfully");

cleanup:
  if (fp)
    fclose(fp);
  if (name)
    free(name);
  if (code)
    free(code);
}

