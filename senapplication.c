#include "sen.h"

static GtkNotebook *sen_notebook = NULL;

static GtkNotebook *
get_notebook() {
  return sen_notebook;
}

static SenTextView *
get_current_tv () {
  return (notebook_page_get_current_text_view (get_notebook()));
}

void
regexp_cb (GtkToggleButton *btnre) {
  sen_text_view_set_regexp (get_current_tv (), gtk_toggle_button_get_active (btnre));
}

void
search_entry_activate_cb (GtkSearchEntry *search_entry) {
  const char *text;

  text = gtk_editable_get_text (GTK_EDITABLE (search_entry));
  sen_text_view_search (get_current_tv (), text);
}

void
search_entry_next_match_cb (GtkSearchEntry *search_entry) {
  sen_text_view_search_next (get_current_tv ());
}

void
search_entry_previous_match_cb (GtkSearchEntry *search_entry) {
  sen_text_view_search_prev (get_current_tv ());
}

void
search_entry_stop_search_cb (GtkSearchEntry *search_entry) {
  sen_text_view_search_stop (get_current_tv ());
}

static void
replace_entry_changed_cb (GtkEditable *editable) {
  GtkEntry *entry = GTK_ENTRY (editable);

  if (gtk_entry_get_text_length (entry) > 0 && (gtk_entry_get_icon_name (entry, GTK_ENTRY_ICON_SECONDARY) == NULL))
    gtk_entry_set_icon_from_icon_name (entry, GTK_ENTRY_ICON_SECONDARY, "edit-clear-all-symbolic");
  else if  (gtk_entry_get_text_length (entry) == 0 && (gtk_entry_get_icon_name (entry, GTK_ENTRY_ICON_SECONDARY) != NULL))
    gtk_entry_set_icon_from_icon_name (entry, GTK_ENTRY_ICON_SECONDARY, NULL);
}
    
void
replace_entry_clear_cb (GtkEntry *entry) {
  gtk_editable_delete_text (GTK_EDITABLE (entry), 0, -1);
}

void
replace_cb (GtkButton *btnrp, gpointer user_data) {
  const char *text;

  text = gtk_editable_get_text (GTK_EDITABLE (user_data));
  sen_text_view_replace (get_current_tv (), text);
}

void
replace_all_cb (GtkButton *btnrpall, gpointer user_data) {
  const char *text;

  text = gtk_editable_get_text (GTK_EDITABLE (user_data));
  sen_text_view_replace_all (get_current_tv (), text);
}

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
  GtkEntry *entry;
  GMenuModel *menu;
  int i;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/sen/sen.ui");
  win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));

  nb = sen_notebook = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
  g_signal_connect (win, "close-request", G_CALLBACK (window_close_requested), nb);
  g_signal_connect (nb, "page-removed", G_CALLBACK (page_removed_cb), NULL);
  gtk_window_set_application (GTK_WINDOW (win), app);
  btnm = GTK_MENU_BUTTON (gtk_builder_get_object (build, "btnm"));
  entry = GTK_ENTRY (gtk_builder_get_object (build, "replace_entry"));

  g_object_unref(build);

  g_signal_connect (GTK_EDITABLE (entry), "changed", G_CALLBACK (replace_entry_changed_cb), NULL);

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
  gtk_css_provider_load_from_data (provider, "textview {font-family: monospace; font-size: 12pt;}", -1);
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

