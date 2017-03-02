#ifndef os_specific_h
#define os_specific_h

#include <deque>
#include <string>

#include <FL/Fl_Window.H>

class OsSpecific {
private:

public:
    void    set_app_icon            (Fl_Window* window);
    int     init                    (Fl_Window* window);
    void    end                     (void);
    int     get_working_dir         (std::string& dir);
    void    dir_chooser             (char* dir);
    void    maximize_window         (Fl_Window* window);
    bool    is_window_maximized     (Fl_Window* window);

#ifdef WIN32
    void    scanfolder              (const wchar_t* dir, std::deque<std::wstring>& filelist);
#else
    void    scanfolder              (const char* dir, std::deque<std::string>& filelist);
#endif
};

#endif
