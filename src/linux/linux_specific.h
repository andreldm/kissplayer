#ifndef linux_specific_h
#define linux_specific_h

#include <deque>
#include <string>

#include <FL/Fl_Window.H>

#define FILE_SEPARATOR "/"

void    os_specific_set_app_icon            (Fl_Window* window);
int     os_specific_init                    (Fl_Window* window);
void    os_specific_end                     (void);
void    os_specific_get_working_dir         (std::string& dir);
void    os_specific_dir_chooser             (char* dir);
void    os_specific_maximize_window         (Fl_Window* window);
bool    os_specific_is_window_maximized     (Fl_Window* window);

void    os_specific_scanfolder              (const char* dir, std::deque<std::string>& filelist);

#endif
