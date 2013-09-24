#ifndef window_loading_h
#define window_loading_h

void    window_loading_show             (void);
void    window_loading_close            (void);
void    window_loading_set_dir_max      (int m);
void    window_loading_set_file_max     (int m);
void    window_loading_set_dir_value    (int v);
void    window_loading_set_file_value   (int v);

// GLOBAL IMPORT
extern bool     FLAG_CANCEL_SYNC;

#endif
