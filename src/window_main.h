#ifndef window_main_h
#define window_main_h

#include <iostream>
#include <vector>
#include <deque>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include "fl_slider_music.h"
#include "sound.h"
#include "lyrics_fetcher.h"
#include "window_settings.h"
#include "window_about.h"
#include "dao.h"
#include "util.h"

// GLOBAL
extern bool     shouldMaximizeWindow;
extern bool     FLAG_RANDOM;
extern bool     FLAG_LYRICS;
extern int      FLAG_SEARCH_TYPE;
extern float    INITIAL_VOLUME;

void        window_main_set_choice_search_type_color    (Fl_Color c);
void        window_main_set_input_search_type_color     (Fl_Color c);
void        window_main_set_lyrics_pane_color           (Fl_Color c1, Fl_Color c2, Fl_Color t);
void        window_main_set_browser_music_color         (Fl_Color c1, Fl_Color c2, Fl_Color t);
Fl_Color    window_main_get_browser_music_color         (int c);

// EXPORTED
Fl_Double_Window* make_window_main(int argc, char **argv);
void cb_toggle_play(Fl_Widget*, void*);
void cb_stop(Fl_Widget*, void*);
void cb_next(Fl_Widget*, void*);
void cb_previous(Fl_Widget*, void*);

#endif
