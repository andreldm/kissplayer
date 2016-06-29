#ifndef window_loading_h
#define window_loading_h

#include <FL/Fl_Window.H>

#include "context.h"

class WindowLoading : public Fl_Window
{
private:
    Context* context;
public:
    WindowLoading               (Context* context);
    void    show                (void);
    void    close               (void);
    void    set_dir_max         (int m);
    void    set_file_max        (int m);
    void    set_dir_value       (int v);
    void    set_file_value      (int v);
    void    cancel              (void);
};

#endif
