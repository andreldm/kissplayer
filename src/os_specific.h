#ifndef os_utils_h
#define os_utils_h

#include <deque>
#include <string>

#include <FL/Fl_Window.H>

class OsUtils {
private:

public:
    // OsUtils();

    void    set_app_icon            (Fl_Window* window);
    int     init                    (void);
    void    end                     (void);
    int     get_working_dir         (std::string& dir);
    void    dir_chooser             (char* dir);
    void    maximize_window         (void);
    bool    is_window_maximized     (void);

    void    scanfolder              (const char* dir, std::deque<std::string>& filelist);
};

#endif
