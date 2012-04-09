#ifndef window_loading_h
#define window_loading_h

#include <iostream>
#include <vector>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>

#include "dao.h"

//GLOBALS
extern Fl_Progress *progress_bar_dir;
extern Fl_Progress *progress_bar_file;
extern bool FLAG_CANCEL_SYNC;
Fl_Window* make_window_loading();

#endif
