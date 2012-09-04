/**
Here lives the Linux specific code.
**/

#ifndef linux_specific_h
#define linux_specific_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/x.H>
#include <X11/xpm.h>
#include "icon.xpm"

void set_app_icon(Fl_Window *window);
void init_os_specific(Fl_Window *window);
void end_os_specific();
char * native_dir_chooser();

static Pixmap mask;

#endif
