#ifndef windows_specific_h
#define windows_specific_h

#include <FL/Fl_Window.H>
#include <deque>
#include <string>

#define FILE_SEPARATOR "\\"

void    os_specific_set_app_icon                (Fl_Window* window);
int     os_specific_init                        (Fl_Window* window);
void    os_specific_end                         (void);
void    os_specific_get_working_dir             (std::string& dir);
void    os_specific_dir_chooser                 (char* dir);
void    os_specific_maximize_window             (Fl_Window* window);
bool    os_specific_is_window_maximized         (Fl_Window* window);

void    os_specific_scanfolder                  (const wchar_t* dir, std::deque<std::wstring>& filelist);

void    win_specific_update_thumbnail_toolbar   (std::string command);

#endif
