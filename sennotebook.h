SenTextView *
notebook_page_get_current_text_view (GtkNotebook *nb);

void
notebook_page_save (GtkNotebook *nb);

void
notebook_page_saveas (GtkNotebook *nb);

void
notebook_page_open (GtkNotebook *nb);

void
notebook_page_close (GtkNotebook *nb);

void
notebook_page_close_all (GtkNotebook *nb);

void
notebook_page_new_with_file (GtkNotebook *nb, GFile *file);

void
notebook_page_new (GtkNotebook *nb);

