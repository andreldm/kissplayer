#ifndef window_main_h
#define window_main_h

#include <iostream>
#include <vector>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "fl_slider_music.h"
#include "sound.h"
#include "lyrics_fetcher.h"
#include "window_settings.h"
#include "window_about.h"
#include "dao.h"
#include "misc.h"

#ifdef WIN32
    #include "win/windows_specific.h"
#endif

//GLOBALS
extern Sound *sound;

extern bool     FLAG_RANDOM;
extern int      FLAG_SEARCH_TYPE;
extern float    INITIAL_VOLUME;

// EXPORTEDS
Fl_Double_Window* make_window_main();
void cb_next(Fl_Widget*, void*);
void cb_previous(Fl_Widget*, void*);
void cb_toggle_play(Fl_Widget*, void*);

#endif
