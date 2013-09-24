#include "window_main.h"

#include <iostream>
#include <deque>
#include <string>

#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "images.h"
#include "os_specific.h"
#include "fl_slider_music.h"
#include "fl_browser_music.h"
#include "sound.h"
#include "lyrics_fetcher.h"
#include "window_settings.h"
#include "window_about.h"
#include "dao.h"
#include "util.h"

using namespace std;

// WIDGETS
static Fl_Button* button_play;
static Fl_Button* button_stop;
static Fl_Button* button_previous;
static Fl_Button* button_next;
static Fl_Button* button_sync;
static Fl_Button* button_search;
static Fl_Button* button_clear;
static Fl_Button* button_settings;
static Fl_Button* button_about;
static Fl_Button* button_random;
static Fl_Dial* dial_volume;
static Fl_Browser_Music* browser_music;
static Fl_Slider_Music* slider_music;
static Fl_Box* box_current_time;
static Fl_Input* input_search;
static Fl_Choice* choice_search_type;
static Fl_Text_Display* lyrics_pane;
static Fl_Text_Buffer* lyrics_text_buffer;
static Fl_Group* group_search;
static Fl_Group* group_controls;
static Fl_Tile* tile_center;

// VARIABLES
static Fl_Double_Window* window;
static deque<Music> listMusic;
static deque<int> listRandom;
static string windowTitle;
static int windowTitlePosition;
static int musicIndex;
static int musicIndexRandom;
static string lastSearch;
static bool doNotLoadLastSearch;

// FUNCTIONS
static void play_music();
static void update_playlist();
static void timer_title_scrolling(void*);
static void timer_check_music(void*);
static void save_config();
static void load_config();
static bool hasNextMusic();
static int main_handler(int, Fl_Window*);

// CALLBACKS
static void cb_close_window(Fl_Widget*, void*);
static void cb_toggle_play(Fl_Widget*, void*);
static void cb_previous(Fl_Widget*, void*);
static void cb_stop(Fl_Widget*, void*);
static void cb_next(Fl_Widget*, void*);
static void cb_volume(Fl_Widget*, void*);
static void cb_music_browser(Fl_Widget*, void*);
static void cb_sync(Fl_Widget*, void*);
static void cb_search(Fl_Widget*, void*);
static void cb_search_type(Fl_Widget*, void*);
static void cb_random(Fl_Widget*, void*);
static void cb_settings(Fl_Widget*, void*);
static void cb_about(Fl_Widget*, void*);
static void cb_clear(Fl_Widget*, void*);
static void cb_slider_music(Fl_Widget*, void*);

Fl_Double_Window* make_window_main(int argc, char** argv)
{
    // To place the window at the center of the screen
    int window_w = 420;
    int window_h = 370;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window = new Fl_Double_Window(window_x, window_y, window_w, window_h, "KISS Player");
    window->size_range(window_w, window_h);
    window->callback((Fl_Callback*)cb_close_window);

    // SEARCH GROUP AND ITS WIDGETS
    group_search = new Fl_Group(5, 5, 410, 30);
    group_search->box(FL_UP_FRAME);
    group_search->begin();

    input_search = new Fl_Input(67, 10, 205, 22,"Search:");
    input_search->maximum_size(50);
    input_search->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
    input_search->callback(cb_search);

    choice_search_type = new Fl_Choice(275, 10, 80, 22);
    choice_search_type->add("All");
    choice_search_type->add("Title");
    choice_search_type->add("Artist");
    choice_search_type->add("Album");
    choice_search_type->value(0);
    choice_search_type->callback(cb_search_type);
    FLAG_SEARCH_TYPE = SEARCH_TYPE_ALL;

    button_clear = new Fl_Button(361, 10, 22, 22);
    button_clear->image(img_icon_clear);
    button_clear->callback(cb_clear);

    button_search = new Fl_Button(388, 10, 22, 22);
    button_search->image(img_icon_search);
    button_search->callback(cb_search);

    group_search->resizable(input_search);
    group_search->end();

    // CENTER TILE AND ITS WIDGETS
    tile_center = new Fl_Tile(5, 40, 410, 230);
    tile_center->begin();

    browser_music = new Fl_Browser_Music(5, 40, 205, 230);
    browser_music->callback(cb_music_browser);

    lyrics_text_buffer = new Fl_Text_Buffer();

    lyrics_pane = new Fl_Text_Display (210, 40, 205, 230, NULL);
    lyrics_pane->buffer(lyrics_text_buffer);
    lyrics_pane->box(FL_DOWN_BOX);
    lyrics_pane->textfont(FL_HELVETICA);
    lyrics_pane->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
    lyrics_pane->scrollbar_width(15);
    lyrics_pane->color(DEFAULT_BACKGROUND_COLOR);
    lyrics_pane->callback(cb_slider_music);
    lyrics_pane->scrollbar_align(FL_ALIGN_RIGHT);

    tile_center->box(FL_DOWN_BOX);
    tile_center->end();

    // CONTROLS GROUP AND ITS WIDGETS
    group_controls = new Fl_Group(5, 275, 410, 90);
    group_controls->box(FL_UP_FRAME);
    group_controls->begin();

    slider_music = new Fl_Slider_Music(10, 280, 403, 20, "00:00");
    slider_music->callback(cb_slider_music);
    slider_music->color2(0x018BFD00);
    slider_music->align(FL_ALIGN_BOTTOM_RIGHT);

    box_current_time = new Fl_Box(10, 300, 40, 15, "00:00");

    button_play = new Fl_Button(10, 330, 25, 25);
    button_play->clear_visible_focus();
    button_play->image(img_icon_play);
    button_play->tooltip("Play/Pause");
    button_play->callback(cb_toggle_play);

    button_stop = new Fl_Button(45, 330, 25, 25);
    button_stop->clear_visible_focus();
    button_stop->image(img_icon_stop);
    button_stop->tooltip("Stop");
    button_stop->callback(cb_stop);

    button_previous = new Fl_Button(80, 330, 25, 25);
    button_previous->clear_visible_focus();
    button_previous->image(img_icon_previous);
    button_previous->tooltip("Previous");
    button_previous->callback(cb_previous);

    button_next = new Fl_Button(115, 330, 25, 25);
    button_next->clear_visible_focus();
    button_next->image(img_icon_next);
    button_next->tooltip("Next");
    button_next->callback(cb_next);

    Fl_Box* separator = new Fl_Box(FL_FLAT_BOX, 150, 330, 1, 25,"");
    separator->color(FL_DARK3);

    button_sync = new Fl_Button(160, 330, 25, 25);
    button_sync->clear_visible_focus();
    button_sync->callback(cb_sync);
    button_sync->tooltip("Synchronize Music Library");
    button_sync->image(img_icon_sync);

    button_random = new Fl_Button(195, 330, 25, 25);
    button_random->clear_visible_focus();
    button_random->image(img_icon_random_off);
    button_random->tooltip("Randomize");
    button_random->callback(cb_random);

    button_settings = new Fl_Button(230, 330, 25, 25);
    button_settings->clear_visible_focus();
    button_settings->image(img_icon_settings);
    button_settings->callback(cb_settings);
    button_settings->tooltip("Settings");

    button_about = new Fl_Button(265, 330, 25, 25);
    button_about->clear_visible_focus();
    button_about->image(img_icon_about);
    button_about->tooltip("About");
    button_about->callback(cb_about);

    // It won't be visible, it's just for the resize work nicely
    Fl_Button* spacer = new Fl_Button(330, 330, 10, 10);
    spacer->hide();

    dial_volume = new Fl_Dial(370, 321, 38, 38, NULL);
    dial_volume->value(0.8);
    dial_volume->callback(cb_volume);

    group_controls->resizable(spacer);
    group_controls->end();

    // END OF WIDGET'S SETUP

    // Check for music files on arguments
    util_parse_args(argc, argv, listMusic);

    // If there is one or more on music files as argument, display it/them
    if(listMusic.size() > 0) {
        for(int i = 0; i < listMusic.size(); i++) {
            Music m = listMusic.at(i);
            browser_music->add(m.getDesc().c_str());
        }
        doNotLoadLastSearch = true;
        util_randomize(listRandom, listMusic.size());
    }

    Fl::event_dispatch(main_handler);
    Fl::add_timeout(0.5, timer_check_music);
    Fl::add_timeout(0.2, timer_title_scrolling);

    Fl::scheme("GTK+");
    window->resizable(tile_center);
    window->end();

    load_config();

    return window;
}

void cb_close_window(Fl_Widget* widget, void*)
{
    // ignore Escape key
    if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
        return;
    }

    sound_unload();
    save_config();

    window->~Fl_Window();
}

void cb_toggle_play(Fl_Widget* widget, void*)
{
    if(sound_is_loaded()) {
        //It has to be done before the togglePause
        if(sound_is_playing()) {
            window->label("KISS Player - Paused");
            button_play->image(img_icon_play);
            button_play->redraw();
#ifdef WIN32
            win_specific_update_thumbnail_toolbar("play");
#endif
        } else {
            button_play->image(img_icon_pause);
            button_play->redraw();
#ifdef WIN32
            win_specific_update_thumbnail_toolbar("pause");
#endif
        }

        sound_toggle_pause();
    } else if(!listMusic.empty()) {
        musicIndex = (browser_music->value() == 0) ? 0 : browser_music->value()-1;
        play_music();
    }
}

void cb_stop(Fl_Widget* widget, void*)
{
    if(!sound_is_playing()) {
        return;
    }

    sound_active(false);
    window->label("KISS Player");
    button_play->image(img_icon_play);
    button_play->redraw();

    box_current_time->label("00:00");
    slider_music->label("00:00");
    slider_music->value(0);

    browser_music->clearHighlighted();
    browser_music->redraw();
}

void cb_previous(Fl_Widget* widget, void*)
{
    // If there's no music on the list or there's no music playing, do not continue
    if(listMusic.empty() || !sound_is_loaded()) {
        return;
    }

    sound_unload();

    if(FLAG_RANDOM) {
        if(musicIndexRandom >= 1) {
            musicIndex = listRandom.at(--musicIndexRandom);
            play_music();
        }
    } else if(musicIndex > 0) {
        musicIndex--;
        play_music();
    }
}

void cb_next(Fl_Widget* widget, void*)
{
    if(hasNextMusic()) {
        if(FLAG_RANDOM) {
            musicIndex = listRandom.at(++musicIndexRandom);
        } else {
            musicIndex++;
        }

        sound_unload();
        play_music();
    }
}

void cb_music_browser(Fl_Widget* widget, void*)
{
    if(Fl::event_clicks() > 0 && Fl::event_button() == FL_LEFT_MOUSE) {
        if(browser_music->value() == 0) {
            return;
        }
        musicIndex = browser_music->value() - 1;
        play_music();
    }
}

void cb_volume(Fl_Widget* widget, void*)
{
    sound_volume(dial_volume->value());
}

void cb_slider_music(Fl_Widget* widget, void*)
{
    // If there's no music playing, do not change
    // The slider's changes are handled at timer_check_music and my_handler
    if(!sound_is_loaded()) {
        slider_music->value(0);
    }
}

void cb_search(Fl_Widget* widget, void*)
{
    lastSearch = input_search->value();
    dao_search_music(lastSearch.c_str(), listMusic);
    update_playlist();
}

void cb_search_type(Fl_Widget* widget, void*)
{
    const char* choice = choice_search_type->text();

    if(strcmp(choice, "All") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ALL;
    else if(strcmp(choice, "Title") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_TITLE;
    else if(strcmp(choice, "Artist") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ARTIST;
    else if(strcmp(choice, "Album") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ALBUM;

    input_search->take_focus();
    input_search->do_callback();
}

void cb_clear(Fl_Widget* widget, void*)
{
    input_search->value(NULL);
    input_search->do_callback();
}

void cb_random(Fl_Widget* widget, void*)
{
    FLAG_RANDOM = !FLAG_RANDOM;

    if(FLAG_RANDOM) {
        button_random->image(img_icon_random_on);
    } else {
        button_random->image(img_icon_random_off);
    }

    button_random->redraw();
}

void cb_settings(Fl_Widget* widget, void*)
{
    window_settings_show();
}

void cb_about(Fl_Widget* widget, void*)
{
    window_about_show();
}

void cb_sync(Fl_Widget* widget, void*)
{
    button_stop->do_callback();
    util_sync_library();
    dao_get_all_music(listMusic);
    update_playlist();
}

int main_handler(int e, Fl_Window* w)
{
    // When the slider is released, set the sound's position
    if(e == FL_RELEASE && Fl::belowmouse() == slider_music && !Fl::event_buttons()) {
        // Needed to update without delay
        box_current_time->label(util_format_time(slider_music->value()));

        // We need to store the volume, because FMOD will reset this value
        float volume = sound_volume();

        // If the user is holding the slider and the music reaches its end,
        // we need to play the sound, otherwise we get a strange sound loop.
        if(sound_is_playing()) {
            sound_play();
        }

        sound_position(slider_music->value());
        sound_volume(volume);
    }

    if(e == FL_KEYDOWN) {
        int key = Fl::event_original_key();
        float v;

        switch(key) {
        case FL_Volume_Down: // Increase Volume
            v = dial_volume->value() - 0.05f;
            if (v < 0) v = 0;
            dial_volume->value(v);
            sound_volume(v);
            return 0;
        case FL_Volume_Up: // Decrease Volume
            v = dial_volume->value() + 0.05f;
            if (v > 1) v = 1;
            dial_volume->value(v);
            sound_volume(v);
            return 0;
        }
    }

    if(e == FL_KEYUP) {
        int key = Fl::event_original_key();
        int pos;

        switch(key) {
        case FL_Right: // Go 5s foward
            // If the input search is focused or the slider is being dragged, do not continue
            if(input_search == Fl::focus() || Fl::pushed() == slider_music) {
                return Fl::handle_(e, w);
            }
            Fl::focus(slider_music);

            pos = sound_position();
            if(pos < sound_length() + 5000) {
                sound_position(pos + 5000);
            }

            return 1;
        case FL_Left: // Go 5s backward
            // If the input search is focused or the slider is being dragged, do not continue
            if(input_search == Fl::focus() || Fl::pushed() == slider_music) {
                return Fl::handle_(e, w);
            }
            Fl::focus(slider_music);

            pos = sound_position();
            if(sound_length() - 5000 > 0)
                sound_position(pos - 5000);

            return 1;
        case 'k':
            if(Fl::event_ctrl()) {
                Fl::focus(input_search);
                input_search->position(0,input_search->size());
                input_search->redraw();
            }
            return 0;
        case FL_F + 6:
            Fl::focus(input_search);
            input_search->position(0,input_search->size());
            input_search->redraw();
            return 0;
        }
        return 0;
    }

    // MOUSEWHEEL HANDLER FOR VOLUME DIAL
    if(e == FL_MOUSEWHEEL &&
        Fl::belowmouse() != NULL &&
        Fl::belowmouse() != browser_music &&
        Fl::belowmouse() != lyrics_pane &&
        Fl::belowmouse()->parent() != browser_music &&
        Fl_Window::current() == window) {
        float valor = 0.1 * Fl::event_dy();
        valor = dial_volume->value()-valor;
        if(valor < 0) valor = 0;
        if(valor > 1) valor = 1;
        dial_volume->value(valor);
        sound_volume(valor);
        return 0;
    }

    return Fl::handle_(e, w);
}

void play_music()
{
    const char* filepath = listMusic.at(musicIndex).filepath.c_str();

    /*while(fl_access(filepath, 0)) { // 0 = F_OK
        if(!hasNextMusic()) {
            cb_stop(NULL, NULL);
            return;
        }

        if(FLAG_RANDOM) {
            musicIndex = listRandom.at(++musicIndexRandom);
        } else {
            musicIndex++;
        }

        filepath = listMusic.at(musicIndex).filepath.c_str();
        browser_music->value(musicIndex+1);
        browser_music->redraw();
    }*/

    browser_music->value(musicIndex+1);
    browser_music->setHighlighted(musicIndex+1);
    browser_music->redraw();

    sound_load(listMusic.at(musicIndex).filepath.c_str());
    sound_play();
    sound_volume(dial_volume->value());

    windowTitle = "KISS Player :: ";
    windowTitle.append(browser_music->text(musicIndex+1));
    windowTitlePosition = 0;

    button_play->image(img_icon_pause);
    button_play->redraw();
#ifdef WIN32
    win_specific_update_thumbnail_toolbar("pause");
#endif

    box_current_time->label("00:00");
    slider_music->copy_label(util_format_time(sound_length()));
    slider_music->maximum(sound_length());
    slider_music->value(0);
    if(FLAG_LYRICS) {
        lyrics_fetcher_run(lyrics_text_buffer, listMusic.at(musicIndex).artist, listMusic.at(musicIndex).title);
        lyrics_pane->scroll(0,0);
    }
    else {
        lyrics_text_buffer->text("");
    }
}

void timer_title_scrolling(void*)
{
    Fl::repeat_timeout(0.2, timer_title_scrolling);
    if (!sound_is_loaded() || !sound_is_playing()) {
        return;
    }

    if (windowTitlePosition == windowTitle.size()) {
        windowTitlePosition = 0;
    } else {
        windowTitlePosition++;
    }

    string title = windowTitle.substr(windowTitlePosition, windowTitle.size());
    title.append(" - ");
    title.append(windowTitle.substr(0, windowTitlePosition));

    window->copy_label(title.c_str());

    // if the current char is multibyte
    // we should "jump" those extra bytes
    const char* str = windowTitle.c_str();
    int charLen = fl_utf8len(str[windowTitlePosition]);
    if(charLen > 1) {
        windowTitlePosition += (charLen - 1);
    }
}

void timer_check_music(void*)
{
    Fl::repeat_timeout(0.25, timer_check_music);

    // If there's no music playing, do not continue
    if(!sound_is_loaded()) {
        return;
    }

    // If the music reached its end, so advance to the next one.
    // Note that we check if any mouse button is pressed, so if
    // the user is dragging the slider it won't advance the music.
    if(slider_music->value() >= (slider_music->maximum() - 1) && Fl::pushed() != slider_music) {
        if(hasNextMusic()) {
            cb_next(NULL, 0);
        } else {
            cb_stop(NULL, 0);
        }
        return;
    }

    box_current_time->copy_label(util_format_time(sound_position()));
    box_current_time->redraw();

    if(Fl::pushed() != slider_music) {
        slider_music->value(sound_position());
    }
}

void save_config()
{
    dao_begin_transaction();

    // TODO: On Windows, when the window is minimized its location becomes -32000, -32000.
    //       We need to save the location before it becomes minimized.
    if(window->x() >= 0) {
        dao_set_key("window_main_x", util_i2s(window->x()));
    }

    if(window->y() >= 0) {
        dao_set_key("window_main_y", util_i2s(window->y()));
    }

    dao_set_key("window_main_width", util_i2s(window->w()));
    dao_set_key("window_main_height", util_i2s(window->h()));

    dao_set_key("window_maximized", util_i2s(os_specific_is_window_maximized(window)));

    dao_set_key("browser_music_width", util_i2s(browser_music->w()));

    dao_set_key("volume_level", util_f2s(dial_volume->value()));

    dao_set_key("random_button", util_i2s(FLAG_RANDOM));

    dao_set_key("search_input", lastSearch);

    dao_set_key("search_type", util_i2s(FLAG_SEARCH_TYPE));

    dao_set_key("music_index", util_i2s(musicIndex));

    dao_set_key("music_index_random", util_i2s(musicIndexRandom));

    dao_set_key("color_background", util_i2s(browser_music->color()));

    dao_set_key("color_selection", util_i2s(browser_music->color2()));

    dao_set_key("color_text", util_i2s(browser_music->textcolor()));

    dao_set_key("lyrics", util_i2s(FLAG_LYRICS));

    dao_commit_transaction();
}

void load_config()
{
    dao_open_db();

    // SET WINDOW POSITION
    int x = util_s2i(dao_get_key("window_main_x"));
    if(x != -1) window->position(x, window->y());

    int y = util_s2i(dao_get_key("window_main_y"));
    if(y != -1) window->position(window->x(), y);

    // SET WINDOW SIZE
    int width = util_s2i(dao_get_key("window_main_width"));
    if(width != -1) window->size(width, window->h());

    int height = util_s2i(dao_get_key("window_main_height"));
    if(height != -1) window->size(window->w(), height);

    // SET WINDOW MAXIMIZED STATE
    int maximized = util_s2i(dao_get_key("window_maximized"));
    if(maximized == 1) {
        FLAG_MAXIMIZE_WINDOW = true;
    }

    // SET VOLUME
    float volume = util_s2f(dao_get_key("volume_level"));
    if(volume != -1) {
        dial_volume->value(volume);
        sound_volume(dial_volume->value());
    }

    // TOGGLE RANDOMIZE
    int random = util_s2i(dao_get_key("random_button"));
    if(random != -1) {
        // It should be inverted, because...
        FLAG_RANDOM = !random;
        // ...here toggle the Random Button
        cb_random(NULL, 0);
    }

    // SET SEARCH TYPE
    int search_type = util_s2i(dao_get_key("search_type"));
    if(search_type != -1) {
        FLAG_SEARCH_TYPE = search_type;
        choice_search_type->value(FLAG_SEARCH_TYPE);
    }

    // SET SEARCH STRING
    string search_input = dao_get_key("search_input");
    if(!search_input.empty()) {
        input_search->value(search_input.c_str());
        if(!doNotLoadLastSearch)
            cb_search(NULL, 0);
    }
    // If the search string is empty, load all the musics
    else if(!doNotLoadLastSearch) {
        dao_get_all_music(listMusic);
        update_playlist();
    }

    // Open DB again, update_playlist and cb_search close it
    dao_open_db();

    // SET LYRICS FLAG
    int lyrics = util_s2i(dao_get_key("lyrics"));
    if(lyrics != -1) {
        FLAG_LYRICS = lyrics;
    }
    else {
        FLAG_LYRICS = true;
    }

    // SET MUSIC INDEX
    int mi = util_s2i(dao_get_key("music_index"));
    if(mi != -1) musicIndex = mi;

    int mir = util_s2i(dao_get_key("music_index_random"));
    if(mir != -1) musicIndexRandom = mir;

    browser_music->value(musicIndex+1);

    // SET MUSIC BROWSER WIDTH
    int browser_music_width = util_s2i(dao_get_key("browser_music_width"));

    if(browser_music_width > 0) {
        browser_music->size(browser_music_width, browser_music->h());
        int x = tile_center->x() + browser_music_width;
        int y = tile_center->y();
        int w = tile_center->w() - browser_music_width;
        int h = lyrics_pane->h();
        lyrics_pane->resize(x, y, w, h);
    }

    // SET WIDGETS COLORS
    int color_background = util_s2i(dao_get_key("color_background"));
    int color_selection = util_s2i(dao_get_key("color_selection"));
    int color_text = util_s2i(dao_get_key("color_text"));

    if(color_background == -1)
        color_background = DEFAULT_BACKGROUND_COLOR;
    if(color_selection == -1)
        color_selection = FL_SELECTION_COLOR;
    if(color_text == -1)
        color_text = FL_FOREGROUND_COLOR;

    browser_music->color(color_background);
    browser_music->color2(color_selection);
    lyrics_pane->color(color_background);
    lyrics_pane->color2(color_selection);
    input_search->color2(color_selection);
    choice_search_type->color2(color_selection);
    browser_music->textcolor(color_text);
    lyrics_pane->textcolor(color_text);

    browser_music->redraw();
    lyrics_pane->redraw();
    input_search->redraw();
    choice_search_type->redraw();

    dao_close_db();
}

bool hasNextMusic()
{
    // There's no music on the list or no music playing
    if(listMusic.empty() || !sound_is_loaded()) {
        return false;
    }

    // '-1' is the beginning of the playlist
    if(FLAG_RANDOM) {
        if(musicIndexRandom == -1 || musicIndexRandom + 2 <= listRandom.size()) {
            return true;
        }
    } else if(musicIndex + 2 <= listMusic.size()) {
        return true;
    }

    return false;
}

void update_playlist()
{
    browser_music->clear();

    for(int i = 0; i < listMusic.size(); i++) {
        Music m = listMusic.at(i);
        browser_music->add(m.getDesc().c_str());
    }

    musicIndexRandom = -1;
    util_randomize(listRandom, listMusic.size());
}

void window_main_toggle_play()
{
    cb_toggle_play(NULL, NULL);
}

void window_main_stop()
{
    cb_stop(NULL, NULL);
}

void window_main_next()
{
    cb_next(NULL, NULL);
}

void window_main_previous()
{
    cb_previous(NULL, NULL);
}

void window_main_set_choice_search_type_color(Fl_Color c)
{
    if(c != -1) choice_search_type->color2(c);
    choice_search_type->redraw();
}

void window_main_set_input_search_type_color(Fl_Color c)
{
    if(c != -1) input_search->color2(c);
    input_search->redraw();
}

void window_main_set_lyrics_pane_color(Fl_Color c1, Fl_Color c2, Fl_Color t)
{
    if(c1 != -1) lyrics_pane->color(c1);
    if(c2 != -1) lyrics_pane->color2(c2);
    if(t != -1) lyrics_pane->textcolor(t);
    lyrics_pane->redraw();
}

void window_main_set_browser_music_color(Fl_Color c1, Fl_Color c2, Fl_Color t)
{
    if(c1 != -1) browser_music->color(c1);
    if(c2 != -1) browser_music->color2(c2);
    if(t != -1) browser_music->textcolor(t);
    browser_music->redraw();
}

Fl_Color window_main_get_browser_music_color(int c)
{
    switch(c) {
    case 1:
        return browser_music->color();
    case 2:
        return browser_music->color2();
    case 3:
        return browser_music->textcolor();
    }

    return 0;
}
