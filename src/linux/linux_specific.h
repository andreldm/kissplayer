#ifndef linux_specific_h
#define linux_specific_h

#include <deque>
#include <string>

#include <FL/Fl_Window.H>

#define FILE_SEPARATOR "/"

void    os_specific_set_app_icon            (void);
int     os_specific_init                    (void);
void    os_specific_end                     (void);
int     os_specific_get_working_dir         (std::string& dir);
void    os_specific_dir_chooser             (char* dir);
void    os_specific_maximize_window         (void);
bool    os_specific_is_window_maximized     (void);

void    os_specific_scanfolder              (const char* dir, std::deque<std::string>& filelist);

#endif
