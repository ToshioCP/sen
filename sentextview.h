#define SEN_TYPE_TEXT_VIEW sen_text_view_get_type ()
G_DECLARE_FINAL_TYPE (SenTextView, sen_text_view, SEN, TEXT_VIEW, GtkTextView)

/* "open-response" signal response */
enum
{
  SEN_OPEN_RESPONSE_SUCCESS,
  SEN_OPEN_RESPONSE_CANCEL,
  SEN_OPEN_RESPONSE_ERROR
};

GFile *
sen_text_view_get_file (SenTextView *tv);

void
sen_text_view_open (SenTextView *tv);

void
sen_text_view_save (SenTextView *tv);

void
sen_text_view_saveas (SenTextView *tv);

void
sen_text_view_save_before_close (SenTextView *tv, const char *tabname);

GtkWidget *
sen_text_view_new_with_file (GFile *file);

GtkWidget *
sen_text_view_new (void);

void
sen_text_view_set_regexp (SenTextView *tv, gboolean button_status);

void
sen_text_view_search (SenTextView *tv, const char *text);

void
sen_text_view_search_next (SenTextView *tv);

void
sen_text_view_search_prev (SenTextView *tv);

void
sen_text_view_search_stop (SenTextView *tv);

void
sen_text_view_replace (SenTextView *tv, const char *text);

void
sen_text_view_replace_all (SenTextView *tv, const char *text);

