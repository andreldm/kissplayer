#ifndef window_main_h
#define window_main_h

#include <iostream>
#include <vector>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Help_View.H>

#include "sound.h"
#include "lyrics_fetcher.h"
#include "window_dir_mgr.h"
#include "dao.h"
#include "misc.h"

//GLOBALS
extern Sound *sound;

extern bool     FLAG_RANDOM;
extern int      FLAG_SEARCH_TYPE;
extern float    INITIAL_VOLUME;

Fl_Double_Window* make_window_main();

#endif
