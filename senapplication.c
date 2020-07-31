#include "sen.h"

static gboolean
window_close_requested (GtkWindow *win, GtkNotebook *nb) {
  notebook_page_close_all (nb);
  return TRUE;
}

static void
page_removed_cb (GtkNotebook *nb, GtkWidget *child, guint page_num, gpointer user_data) {
  GtkWidget *win;
  static gboolean destroyed = FALSE;

  if ((! destroyed) && (gtk_notebook_get_n_pages (nb) == 0)) {
    win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
    gtk_window_destroy (GTK_WINDOW (win));
    destroyed = TRUE;
  }
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  notebook_page_new (GTK_NOTEBOOK (nb));
}

static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  notebook_page_open (GTK_NOTEBOOK (nb));
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  notebook_page_save (GTK_NOTEBOOK (nb));
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  notebook_page_close (GTK_NOTEBOOK (nb));
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  notebook_page_saveas (GTK_NOTEBOOK (nb));
}

static void
close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  notebook_page_close_all (GTK_NOTEBOOK (nb));
}

static void
sen_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win;
  GtkWidget *boxv;
  GtkNotebook *nb;

  win = GTK_WIDGET (gtk_application_get_active_window (app));
  boxv = gtk_window_get_child (GTK_WINDOW (win));
  nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  notebook_page_new (nb);
  gtk_widget_show (GTK_WIDGET (win));
}

static void
sen_open (GApplication *application, GFile ** files, int n_files, const char *hint) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win;
  GtkWidget *boxv;
  GtkNotebook *nb;
  int i;

  win = GTK_WIDGET (gtk_application_get_active_window (app));
  boxv = gtk_window_get_child (GTK_WINDOW (win));
  nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (nb, files[i]);
  if (gtk_notebook_get_n_pages (nb) == 0)
    notebook_page_new (nb);
  gtk_widget_show (win);
}


static void
sen_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkApplicationWindow *win;
  GtkNotebook *nb;
  GtkBuilder *build;
  GtkMenuButton *btnm;
  GMenuModel *menu;
  int i;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/sen/sen.ui");
  win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));

  nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
  g_signal_connect (win, "close-request", G_CALLBACK (window_close_requested), nb);
  g_signal_connect (nb, "page-removed", G_CALLBACK (page_removed_cb), NULL);
  gtk_window_set_application (GTK_WINDOW (win), app);
  btnm = GTK_MENU_BUTTON (gtk_builder_get_object (build, "btnm"));

  g_object_unref(build);
  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/sen/menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
  gtk_menu_button_set_menu_model (btnm, menu);
  g_object_unref(build);

  const GActionEntry win_entries[] = {
    { "new", new_activated, NULL, NULL, NULL },
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "close-all", close_all_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), nb);

  struct {
    const gchar *action;
    const gchar *accels[2];
  } action_accels[] = {
    { "win.new", { "<Control>n", NULL } },
    { "win.open", { "<Control>o", NULL } },
    { "win.save", { "<Control>s", NULL } },
    { "win.close", { "<Control>w", NULL } },
    { "win.saveas", { "<Shift><Control>s", NULL } },
    { "win.close-all", { "<Control>q", NULL } },
  };

  for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);


GdkDisplay *display;

  display = gtk_widget_get_display (GTK_WIDGET (win));
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.sen", G_APPLICATION_HANDLES_OPEN);

  g_signal_connect (app, "startup", G_CALLBACK (sen_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (sen_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (sen_open), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

