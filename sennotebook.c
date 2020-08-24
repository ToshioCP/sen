#include "sen.h"

/* The returned string should be freed with g_free() when no longer needed. */
static char*
get_untitled () {
  static int c = -1;
  if (++c == 0) 
    return g_strdup_printf("Untitled");
  else
    return g_strdup_printf ("Untitled%u", c);
}

static void
file_changed (SenTextView *tv, GtkNotebook *nb) {
  GFile *file;
  char *filename;
  GtkWidget *boxh;
  GtkWidget *scr;
  GtkWidget *label;

  file = sen_text_view_get_file (tv);
  if (G_IS_FILE (file))
    filename = g_file_get_basename (file);
  else
    filename = get_untitled ();
  scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  boxh = gtk_widget_get_parent (GTK_WIDGET (scr));
  label = gtk_label_new (filename);
  gtk_notebook_set_tab_label (nb, boxh, label);
  g_object_unref (file);
  g_free (filename);
}

static void
page_close_all_cb (GtkNotebook *nb, GtkWidget *child, guint page_num, gpointer user_data) {

  if (gtk_notebook_get_n_pages (nb) > 0)
    notebook_page_close (nb);
}

SenTextView *
notebook_page_get_current_text_view (GtkNotebook *nb) {
  int i;
  GtkWidget *boxh;
  GList *list, *list_end;
  GtkWidget *scr, *tv;

  if ((i = gtk_notebook_get_current_page (nb)) < 0)
    return NULL;
  else {
    boxh = gtk_notebook_get_nth_page (nb, i);
    list = gtk_container_get_children (GTK_CONTAINER (boxh));
    list_end = g_list_last (list);
    scr = GTK_WIDGET (list_end->data);
    tv = gtk_bin_get_child (GTK_BIN (scr));
    return SEN_TEXT_VIEW (tv);
  }
}

void
notebook_page_close_all (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  g_signal_connect (nb, "page-removed", G_CALLBACK (page_close_all_cb), NULL);
  notebook_page_close (nb);
}

static void
page_close_cb (SenTextView *tv) {
  GtkWidget *boxh;
  GtkNotebook *nb;
  GtkWidget *scr;
  int i;

  nb = GTK_NOTEBOOK (gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_NOTEBOOK));
  scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  boxh = gtk_widget_get_parent (GTK_WIDGET (scr));
  i = gtk_notebook_page_num (nb, GTK_WIDGET (boxh));
  gtk_notebook_remove_page (nb, i);
}

void
notebook_page_close(GtkNotebook *nb) {
  int i;
  GtkWidget *boxh;
  GList *list, *list_end;
  GtkWidget *scr;
  GtkWidget *tv;
  const char *tabname;

  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  if ((i = gtk_notebook_get_current_page (nb)) < 0)
    return;
  boxh = gtk_notebook_get_nth_page (nb, i);
  list = gtk_container_get_children (GTK_CONTAINER (boxh));
  list_end = g_list_last (list);
  scr = GTK_WIDGET (list_end->data);

  tv = gtk_bin_get_child (GTK_BIN (scr));
  g_signal_connect (SEN_TEXT_VIEW (tv), "save-finished", G_CALLBACK (page_close_cb), NULL);
  tabname = gtk_notebook_get_tab_label_text (nb, boxh);
  sen_text_view_save_before_close (SEN_TEXT_VIEW (tv), tabname);
}

/* Save the contents in the current page */
void
notebook_page_save(GtkNotebook *nb) {
  int i;
  GtkWidget *boxh;
  GList *list, *list_end;
  GtkWidget *scr;
  GtkWidget *tv;

  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  if ((i = gtk_notebook_get_current_page (nb)) < 0)
    return;
  boxh = gtk_notebook_get_nth_page (nb, i);
  list = gtk_container_get_children (GTK_CONTAINER (boxh));
  list_end = g_list_last (list);
  scr = GTK_WIDGET (list_end->data);
  tv = gtk_bin_get_child (GTK_BIN (scr));
  sen_text_view_save (SEN_TEXT_VIEW (tv));
}

void
notebook_page_saveas(GtkNotebook *nb) {
  int i;
  GtkWidget *boxh;
  GList *list, *list_end;
  GtkWidget *scr;
  GtkWidget *tv;

  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  if ((i = gtk_notebook_get_current_page (nb)) < 0)
    return;
  boxh = gtk_notebook_get_nth_page (nb, i);
  list = gtk_container_get_children (GTK_CONTAINER (boxh));
  list_end = g_list_last (list);
  scr = GTK_WIDGET (list_end->data);
  tv = gtk_bin_get_child (GTK_BIN (scr));
  sen_text_view_saveas (SEN_TEXT_VIEW (tv));
}

static void
notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
  GtkWidget *boxh;
  GtkWidget *line;
  GtkWidget *scr;
  GtkWidget *lab;
  int i;

  boxh = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  line = gtk_drawing_area_new();
  scr = gtk_scrolled_window_new (NULL, NULL);
  gtk_box_pack_start (GTK_BOX (boxh), line, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (boxh), scr, TRUE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (scr), tv);
  lab = gtk_label_new (filename);
  i = gtk_notebook_append_page (nb, boxh, lab);
  gtk_container_child_set (GTK_CONTAINER (nb), boxh, "tab-expand", TRUE, NULL);
  g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed), nb);
  gtk_widget_show_all (GTK_WIDGET (boxh));
  gtk_notebook_set_current_page (nb, i);

/* The following constructor must be called after the SenTextView instance is set to GtkScrolledWindow as a child. */
  sen_text_view_construct (SEN_TEXT_VIEW (tv), GTK_DRAWING_AREA (line));
}

static void
open_response (SenTextView *tv, int response, GtkNotebook *nb) {
  GFile *file;
  char *filename;

  if (response != SEN_OPEN_RESPONSE_SUCCESS) {
    if (g_object_is_floating (tv))
      g_object_ref_sink (tv);
    g_object_unref (tv);
  } else if (! G_IS_FILE (file = sen_text_view_get_file (tv)))
    g_object_unref (tv);
  else {
    filename = g_file_get_basename (file);
    g_object_unref (file);
    notebook_page_build (nb, GTK_WIDGET (tv), filename);
  }
}

void
notebook_page_open (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  GtkWidget *tv;

  tv = sen_text_view_new ();
  g_signal_connect (SEN_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
  sen_text_view_open (SEN_TEXT_VIEW (tv), GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_APPLICATION_WINDOW)));
}

void
notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(G_IS_FILE (file));

  GtkWidget *tv;
  char *filename;

  if ((tv = sen_text_view_new_with_file (file)) == NULL)
    return; /* read error */
  filename = g_file_get_basename (file);
  notebook_page_build (nb, tv, filename);
}

void
notebook_page_new (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  GtkWidget *tv;
  char *filename;

  tv = sen_text_view_new ();
  filename = get_untitled ();
  notebook_page_build (nb, tv, filename);
}

