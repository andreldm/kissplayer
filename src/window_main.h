#ifndef window_main_h
#define window_main_h

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>

#include "constants.h"
#include "context.h"
#include "lyrics_fetcher.h"
#include "playlist.h"
#include "sync.h"
#include "window_about.h"
#include "window_settings.h"

#include "widget/ksp_slider.h"
#include "widget/ksp_volume_controller.h"
#include "widget/ksp_browser.h"
#include "widget/ksp_tile.h"

class KSP_Slider;

class WindowMain : public Fl_Double_Window
{
private:
    Context* context;
    LyricsFetcher* lyricsFetcher;
    Sync* sync;
    Playlist* playlist;
    WindowAbout* windowAbout;
    WindowSettings* windowSettings;

    Fl_Button* button_play;
    Fl_Button* button_stop;
    Fl_Button* button_previous;
    Fl_Button* button_next;
    Fl_Button* button_search;
    Fl_Button* button_menu;
    Fl_Button* button_clear;
    KSP_Volume_Controller* volume_controller;
    KSP_Browser* browser_music;
    KSP_Slider* slider_music;
    Fl_Input* input_search;
    Fl_Choice* choice_search_type;
    Fl_Text_Display* lyrics_pane;
    Fl_Text_Buffer* lyrics_text_buffer;
    Fl_Group* group_search;
    Fl_Group* group_controls;
    KSP_Tile* tile_center;

    SearchType searchType;

    void        toggle_play                     (void);
    void        play                            (bool);
    void        stop                            (void);
    void        next                            (void);
    void        previous                        (void);
    void        reset_title                     (void);
    void        search                          (void);
    void        search_type                     (void);
    void        volume_changed                  (float);
    void        music_browser_event             (void);
    void        show_about                      (void);
    void        show_settings                   (void);
    void        update_colors                   (void);
    void        close_window                    (Fl_Widget*);
    void        clear_search                    (void);
    void        update_music_playing            (int);
    void        slide_changed                   (void);
    void        save_config                     (void);
    void        load_config                     (void);
    void        execute_sync                    (void);
    int         handle_event                    (int, Fl_Window*);
    void        check_title_scroll              (void);
    void        check_music_end                 (void);
    void        toggle_randomize                (void);
    void        toggle_repeat                   (void);

public:
                WindowMain                      (Context*);
    void        init                            (int argc, char** argv);
};

// GLOBAL IMPORT
extern float    INITIAL_VOLUME;

#endif
