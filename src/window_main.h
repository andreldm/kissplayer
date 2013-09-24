#ifndef window_main_h
#define window_main_h

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

void                window_main_toggle_play                     (void);
void                window_main_stop                            (void);
void                window_main_next                            (void);
void                window_main_previous                        (void);
void                window_main_set_choice_search_type_color    (Fl_Color c);
void                window_main_set_input_search_type_color     (Fl_Color c);
void                window_main_set_lyrics_pane_color           (Fl_Color c1, Fl_Color c2, Fl_Color t);
void                window_main_set_browser_music_color         (Fl_Color c1, Fl_Color c2, Fl_Color t);
Fl_Color            window_main_get_browser_music_color         (int c);
Fl_Double_Window*   make_window_main                            (int argc, char** argv);

// GLOBAL IMPORT
extern bool     shouldMaximizeWindow;
extern bool     FLAG_RANDOM;
extern bool     FLAG_LYRICS;
extern int      FLAG_SEARCH_TYPE;
extern float    INITIAL_VOLUME;

#endif
