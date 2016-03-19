#ifndef window_settings_h
#define window_settings_h

#include <FL/Fl_Window.H>

#include "dao.h"

class WindowSettings : public Fl_Window
{
private:
    Dao* dao;

    void    toogleLyrics        (void);
    void    toogleScrollTitle   (void);

public:
            WindowSettings      (Dao*);
    void    show                (Fl_Window* parent);
    void    close               (void);
};

#endif
