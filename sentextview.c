#include "sen.h"

struct _SenTextView
{
  GtkTextView parent;
  GtkTextBuffer *tb;
  GFile *file;
  gboolean changed;
};

G_DEFINE_TYPE (SenTextView, sen_text_view, GTK_TYPE_TEXT_VIEW);

enum {
  CHANGE_FILE,
  OPEN_RESPONSE,
  SAVE_FINISHED,
  NUMBER_OF_SIGNALS
};

static guint sen_text_view_signals[NUMBER_OF_SIGNALS];

/* Signal handler */
static void
on_changed (GtkTextBuffer *tb, SenTextView *tv) {
  tv->changed = TRUE;
}

static void
sen_text_view_dispose (GObject *gobject) {
  SenTextView *tv = TFE_TEXT_VIEW (gobject);
  if (G_IS_FILE (tv->file))
    g_clear_object (&tv->file);

  G_OBJECT_CLASS (sen_text_view_parent_class)->dispose (gobject);
}

static void
sen_text_view_init (SenTextView *tv) {
  tv->tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  tv->file = NULL;
  tv->changed = FALSE;
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
  g_signal_connect (tv->tb, "changed", G_CALLBACK (on_changed), tv);
}

static void
sen_text_view_class_init (SenTextViewClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = sen_text_view_dispose;
  sen_text_view_signals[CHANGE_FILE] = g_signal_newv ("change-file",
                                 G_TYPE_FROM_CLASS (class),
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                 NULL /* closure */,
                                 NULL /* accumulator */,
                                 NULL /* accumulator data */,
                                 NULL /* C marshaller */,
                                 G_TYPE_NONE /* return_type */,
                                 0     /* n_params */,
                                 NULL  /* param_types */);
  GType param_types[] = {G_TYPE_INT}; 
  sen_text_view_signals[OPEN_RESPONSE] = g_signal_newv ("open-response",
                                 G_TYPE_FROM_CLASS (class),
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                 NULL /* closure */,
                                 NULL /* accumulator */,
                                 NULL /* accumulator data */,
                                 NULL /* C marshaller */,
                                 G_TYPE_NONE /* return_type */,
                                 1     /* n_params */,
                                 param_types);
  sen_text_view_signals[SAVE_FINISHED] = g_signal_newv ("save-finished",
                                 G_TYPE_FROM_CLASS (class),
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                 NULL /* closure */,
                                 NULL /* accumulator */,
                                 NULL /* accumulator data */,
                                 NULL /* C marshaller */,
                                 G_TYPE_NONE /* return_type */,
                                 0     /* n_params */,
                                 NULL  /* param_types */);
}

GFile *
sen_text_view_get_file (SenTextView *tv) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);

  return g_file_dup (tv->file);
}

static void
open_dialog_response(GtkWidget *dialog, int response, SenTextView *tv) {
  GFile *file;
  char *contents;
  gsize length;
  GtkWidget *message_dialog;
  GError *err = NULL;

  if (response != GTK_RESPONSE_ACCEPT)
    g_signal_emit (tv, sen_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
  else if (! G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog))))
    g_signal_emit (tv, sen_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
  else if (! g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) { /* read error */
    if (G_IS_FILE (file))
      g_object_unref (file);
    message_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog), GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                            "%s.\n", err->message);
    g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
    gtk_widget_show (message_dialog);
    g_error_free (err);
    g_signal_emit (tv, sen_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
  } else {
    gtk_text_buffer_set_text (tv->tb, contents, length);
    g_free (contents);
    tv->file = file;
    tv->changed = FALSE;
    g_signal_emit (tv, sen_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
  }
  gtk_window_destroy (GTK_WINDOW (dialog));
}

void
sen_text_view_open (SenTextView *tv) {
  g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

  GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_OPEN,
                                        "Cancel", GTK_RESPONSE_CANCEL,
                                        "Open", GTK_RESPONSE_ACCEPT,
                                        NULL);
  g_signal_connect (dialog, "response", G_CALLBACK (open_dialog_response), tv);
  gtk_widget_show (dialog);
}

static void
save_before_close_response (GtkWidget *dialog, int response, SenTextView *tv) {
  gtk_window_destroy (GTK_WINDOW (dialog));
  if (response == GTK_RESPONSE_ACCEPT)
    sen_text_view_save (tv);
  else
    g_signal_emit (tv, sen_text_view_signals[SAVE_FINISHED], 0);
}

void
sen_text_view_save_before_close (SenTextView *tv, const char *tabname) {
  g_return_if_fail (TFE_IS_TEXT_VIEW (tv));

  GtkBuilder *build;
  GtkWidget *dialog;
  GtkWidget *win;

  if (tv->changed) {
    build = gtk_builder_new_from_resource ("/com/github/ToshioCP/sen/dialog.ui");
    dialog = GTK_WIDGET (gtk_builder_get_object (build, "save_before_close_dialog"));
    win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
    gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (win));
    gtk_window_set_title (GTK_WINDOW (dialog), tabname);
    g_signal_connect (dialog, "response", G_CALLBACK (save_before_close_response), tv);
    gtk_widget_show (dialog);
    g_object_unref (build);
  } else
    g_signal_emit (tv, sen_text_view_signals[SAVE_FINISHED], 0);
}

static void
saveas_dialog_response (GtkWidget *dialog, int response, SenTextView *tv) {
  GFile *file;

  if (response == GTK_RESPONSE_ACCEPT && G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog)))) {
    tv->file = file;
    tv->changed = TRUE;
    g_signal_emit (tv, sen_text_view_signals[CHANGE_FILE], 0);
    sen_text_view_save (TFE_TEXT_VIEW (tv));
  } else
    g_signal_emit (tv, sen_text_view_signals[SAVE_FINISHED], 0);
  gtk_window_destroy (GTK_WINDOW (dialog));
}

void
sen_text_view_save (SenTextView *tv) {
  g_return_if_fail (TFE_IS_TEXT_VIEW (tv));

  GtkTextIter start_iter;
  GtkTextIter end_iter;
  char *contents;
  GtkWidget *message_dialog;
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
  GError *err = NULL;

  if (! tv->changed)
    g_signal_emit (tv, sen_text_view_signals[SAVE_FINISHED], 0);
  else if (tv->file == NULL)
    sen_text_view_saveas (tv);
  else {
    gtk_text_buffer_get_bounds (tv->tb, &start_iter, &end_iter);
    contents = gtk_text_buffer_get_text (tv->tb, &start_iter, &end_iter, FALSE);
    if (g_file_replace_contents (tv->file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err))
      tv->changed = FALSE;
    else {
/* It is possible that tv->file is broken. */
/* It is a good idea to set tv->file to NULL. */
      if (G_IS_FILE (tv->file))
        g_object_unref (tv->file);
      tv->file =NULL;
      g_signal_emit (tv, sen_text_view_signals[CHANGE_FILE], 0);
      tv->changed = TRUE;
      message_dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                              "%s.\n", err->message);
      g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_widget_show (message_dialog);
      g_error_free (err);
    }
    g_signal_emit (tv, sen_text_view_signals[SAVE_FINISHED], 0);
  }
}

void
sen_text_view_saveas (SenTextView *tv) {
  g_return_if_fail (TFE_IS_TEXT_VIEW (tv));

  GtkWidget *dialog;
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

  dialog = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_SAVE,
                                      "_Cancel", GTK_RESPONSE_CANCEL,
                                      "_Save", GTK_RESPONSE_ACCEPT,
                                      NULL);
  g_signal_connect (dialog, "response", G_CALLBACK (saveas_dialog_response), tv);
  gtk_widget_show (dialog);
}

GtkWidget *
sen_text_view_new_with_file (GFile *file) {
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  GtkWidget *tv;
  char *contents;
  gsize length;

  if (! g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) /* read error */
    return NULL;

  tv = sen_text_view_new();
  gtk_text_buffer_set_text (TFE_TEXT_VIEW (tv)->tb, contents, length);
  g_free (contents);
  TFE_TEXT_VIEW (tv)->changed = FALSE;
  TFE_TEXT_VIEW (tv)->file = g_file_dup (file);
  return tv;
}

GtkWidget *
sen_text_view_new (void) {
  return gtk_widget_new (TFE_TYPE_TEXT_VIEW, NULL);
}

