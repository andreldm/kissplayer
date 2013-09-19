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
#include <pwd.h>
#include "icon.xpm"
#include "../window_main.h"


#define FILE_SEPARATOR "/"

void set_app_icon(Fl_Window *window);
void init_os_specific(Fl_Window *window);
void end_os_specific();
string getWorkingDirectory();
char * native_dir_chooser();
bool isWindowMaximized(Fl_Window *window);
void maximizeWindow(Fl_Window *window);

void os_specific_scanfolder(const char* dir, deque<string>& filelist);

static Pixmap mask;

#endif
