#ifndef window_dir_mgr_h
#define window_dir_mgr_h

#include <iostream>
#include <vector>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Button.H>

#include "dao.h"
#include "name_cod.h"

//GLOBALS
extern Fl_Window *window_dir_mgr;
Fl_Window* make_window_dir_mgr();

#endif
