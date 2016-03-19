#ifndef window_about_h
#define window_about_h

#include <FL/Fl_Window.H>

class WindowAbout : public Fl_Window
{
public:
            WindowAbout         (void);
    void    show                (Fl_Window* parent);
    void    close               (void);
};

#endif
