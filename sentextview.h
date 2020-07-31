#define TFE_TYPE_TEXT_VIEW sen_text_view_get_type ()
G_DECLARE_FINAL_TYPE (SenTextView, sen_text_view, TFE, TEXT_VIEW, GtkTextView)

/* "open-response" signal response */
enum
{
  TFE_OPEN_RESPONSE_SUCCESS,
  TFE_OPEN_RESPONSE_CANCEL,
  TFE_OPEN_RESPONSE_ERROR
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

