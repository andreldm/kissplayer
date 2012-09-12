#ifndef window_settings_h
#define window_settings_h

#include <vector>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>

#include "dao.h"
#include "name_cod.h"
#include "misc.h"

#ifdef WIN32
#include "win/windows_specific.h"
#endif

#ifdef __linux__
#include "linux/linux_specific.h"
#endif

//GLOBALS
extern Fl_Window *window_settings;
Fl_Window* make_window_settings();

//IMPORTED
extern Fl_Select_Browser *browser_music;
extern Fl_Text_Display *lyrics_pane;
extern Fl_Input *input_search;
extern Fl_Choice *choice_search_type;
extern bool FLAG_NO_LYRICS;

#endif
