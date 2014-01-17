#ifndef windows_specific_h
#define windows_specific_h

#include <FL/Fl_Window.H>
#include <deque>
#include <string>

#define FILE_SEPARATOR "\\"

void    os_specific_set_app_icon                (void);
int     os_specific_init                        (void);
void    os_specific_end                         (void);
void    os_specific_get_working_dir             (std::string& dir);
void    os_specific_dir_chooser                 (char* dir);
void    os_specific_maximize_window             (void);
bool    os_specific_is_window_maximized         (void);

void    os_specific_scanfolder                  (const wchar_t* dir, std::deque<std::wstring>& filelist);

void    win_specific_update_thumbnail_toolbar   (std::string command);

#endif
