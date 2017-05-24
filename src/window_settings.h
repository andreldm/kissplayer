#ifndef window_settings_h
#define window_settings_h

#include <FL/Fl_Window.H>

#include "context.h"

class WindowSettings : public Fl_Window
{
private:
    Context* context;
    bool     shouldSync;

    void     updateColors                (void);
    void     updateProxy                 (void);

public:
             WindowSettings              (Context*);
    void     show                        (Fl_Window* parent);
    void     close                       (void);
    void     addDir                      (void);
    void     removeDir                   (void);
    void     updateDirList               (void);
    void     changeLanguage              (void);
    using Fl_Window::show;
};

#endif
