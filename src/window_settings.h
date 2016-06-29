#ifndef window_settings_h
#define window_settings_h

#include <FL/Fl_Window.H>

#include "dao.h"
#include "os_specific.h"

class WindowSettings : public Fl_Window
{
private:
    Dao* dao;
    OsSpecific* osSpecific;

    void    updateColors                (void);
    void    updateProxy                 (void);

public:
            WindowSettings              (Dao*);
    void    show                        (Fl_Window* parent);
    void    close                       (void);
    void    addDir                      (void);
    void    removeDir					(void);
    void    updateDirList               (void);
};

#endif
