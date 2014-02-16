#include "window_main.h"

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
#include <FL/filename.H>

#include "images.h"
#include "locale.h"
#include "os_specific.h"
#include "widget/ksp_slider.h"
#include "widget/ksp_browser.h"
#include "widget/ksp_tile.h"
#include "widget/ksp_volume_controller.h"
#include "widget/ksp_menu_item.h"
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
static Fl_Button* button_search;
static Fl_Button* button_menu;
static Fl_Button* button_clear;
static KSP_Volume_Controller* volume_controller;
static KSP_Browser* browser_music;
static KSP_Slider* slider_music;
static Fl_Input* input_search;
static Fl_Choice* choice_search_type;
static Fl_Text_Display* lyrics_pane;
static Fl_Text_Buffer* lyrics_text_buffer;
static Fl_Group* group_search;
static Fl_Group* group_controls;
static KSP_Tile* tile_center;

// VARIABLES
static Fl_Double_Window* window;
static deque<Music> listMusic;
static deque<int> listRandom;
static string windowTitle;
static int windowTitlePosition;
static int musicIndex;
static int musicIndexRandom;
static int musicPlayingCod;
static string lastSearch;
static bool doNotLoadLastSearch;

// FUNCTIONS
static void play_music();
static void update_playlist();
static void timer_title_scrolling(void*);
static void timer_check_music(void*);
static void timer_play_at_start(void*);
static void save_config();
static void load_config();
static bool hasNextMusic();
static int handler(int, Fl_Window*);

// CALLBACKS
static void cb_close_window(Fl_Widget*, void*);
static void cb_menu(Fl_Widget*, void*);
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

// MENU ITEMS INDEX
enum {
    MENU_ITEM_SYNC = 0,
    MENU_ITEM_RANDOM,
    MENU_ITEM_SETTINGS,
    MENU_ITEM_ABOUT,
    MENU_ITEM_NONE
};

// MENU ITEMS
static KSP_Menu_Item menu_items[] = {
    KSP_Menu_Item(0, 0, cb_sync, NULL),
    KSP_Menu_Item(0, 0, cb_random, NULL, FL_MENU_TOGGLE),
    KSP_Menu_Item(0, 0, cb_settings, NULL),
    KSP_Menu_Item(0, 0, cb_about, NULL),
    KSP_Menu_Item(0)
};

Fl_Double_Window* window_main_get_instance()
{
    return window;
}

void window_main_init(int argc, char** argv)
{
    // Place the window at the center of the screen
    int window_w = 770;
    int window_h = 465;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window = new Fl_Double_Window(window_x, window_y, window_w, window_h, "KISS Player");
    window->xclass("KISS Player");
    window->size_range(420, 370);
    window->callback((Fl_Callback*)cb_close_window);

    // SEARCH GROUP AND ITS WIDGETS
    group_search = new Fl_Group(5, 5, 760, 30);
    group_search->box(FL_UP_FRAME);
    group_search->begin();

    Fl_Box* label_search = new Fl_Box(8, 10, 10, 22, _("Search:"));
    label_search->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    util_adjust_width(label_search);

    input_search = new Fl_Input(8 + label_search->w(), 10, 585 - label_search->w(), 22);
    input_search->maximum_size(50);
    input_search->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
    input_search->callback(cb_search);

    choice_search_type = new Fl_Choice(598, 10, 80, 22);
    choice_search_type->add(_("All"));
    choice_search_type->add(_("Title"));
    choice_search_type->add(_("Artist"));
    choice_search_type->add(_("Album"));
    choice_search_type->value(0);
    choice_search_type->callback(cb_search_type);
    choice_search_type->clear_visible_focus();
    FLAG_SEARCH_TYPE = SEARCH_TYPE_ALL;

    button_clear = new Fl_Button(684, 10, 22, 22);
    button_clear->image(img_icon_clear);
    button_clear->clear_visible_focus();
    button_clear->callback(cb_clear);

    button_search = new Fl_Button(711, 10, 22, 22);
    button_search->image(img_icon_search);
    button_search->clear_visible_focus();
    button_search->callback(cb_search);

    button_menu = new Fl_Button(738, 10, 22, 22);
    button_menu->clear_visible_focus();
    button_menu->image(img_icon_menu);
    button_menu->callback(cb_menu);

    group_search->resizable(input_search);
    group_search->end();

    // CENTER TILE AND ITS WIDGETS
    tile_center = new KSP_Tile(5, 40, 760, 360);
    tile_center->begin();

    browser_music = new KSP_Browser(5, 40, 555, 360);
    browser_music->color(DEFAULT_BACKGROUND_COLOR);
    browser_music->callback(cb_music_browser);

    lyrics_text_buffer = new Fl_Text_Buffer();

    lyrics_pane = new Fl_Text_Display (560, 40, 205, 360, NULL);
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
    group_controls = new Fl_Group(5, 405, 760, 55);
    group_controls->box(FL_UP_FRAME);
    group_controls->begin();

    button_play = new Fl_Button(12, 420, 25, 25);
    button_play->clear_visible_focus();
    button_play->image(img_icon_play);
    button_play->tooltip(_("Play/Pause"));
    button_play->callback(cb_toggle_play);

    button_stop = new Fl_Button(47, 420, 25, 25);
    button_stop->clear_visible_focus();
    button_stop->image(img_icon_stop);
    button_stop->tooltip(_("Stop"));
    button_stop->callback(cb_stop);

    button_previous = new Fl_Button(82, 420, 25, 25);
    button_previous->clear_visible_focus();
    button_previous->image(img_icon_previous);
    button_previous->tooltip(_("Previous"));
    button_previous->callback(cb_previous);

    button_next = new Fl_Button(117, 420, 25, 25);
    button_next->clear_visible_focus();
    button_next->image(img_icon_next);
    button_next->tooltip(_("Next"));
    button_next->callback(cb_next);

    slider_music = new KSP_Slider(157, 424, 527, 19, "00:00");
    slider_music->callback(cb_slider_music);

    volume_controller = new KSP_Volume_Controller(699, 418, 60, 30);
    volume_controller->value(8);
    volume_controller->tooltip(_("Volume"));
    volume_controller->callback(cb_volume);

    group_controls->resizable(slider_music);
    group_controls->end();

    // END OF WIDGET'S SETUP

    // Set menu labels and icons
    menu_items[MENU_ITEM_SYNC].set_label_icon(_("Synchronize Library"), 0, false);
    menu_items[MENU_ITEM_RANDOM].set_label_icon(_("Randomize"), 0, false);
    menu_items[MENU_ITEM_SETTINGS].set_label_icon(_("Settings"), 0, false);
    menu_items[MENU_ITEM_ABOUT].set_label_icon(_("About"), 0, false);

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
        Fl::add_timeout(0.1, timer_play_at_start);
    }

    Fl::event_dispatch(handler);
    Fl::add_timeout(0.5, timer_check_music);
    Fl::add_timeout(0.2, timer_title_scrolling);

    Fl::scheme("GTK+");

    Fl::background(50, 50, 50);
    Fl::background2(90, 90, 90);
    Fl::foreground(255, 255, 255);

    window->resizable(tile_center);
    window->end();

    load_config();
}

void cb_close_window(Fl_Widget* widget, void*)
{
    // ignore Escape key
    if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
        return;
    }

    sound_unload();
    save_config();

    window->~Fl_Window();
}

void cb_menu(Fl_Widget* w, void*)
{
    const KSP_Menu_Item* m = menu_items[0].popup(w->x(), w->y() + w->h());
    if (m && m->callback()) m->do_callback(w, m->user_data());
}

void cb_toggle_play(Fl_Widget* widget, void*)
{
    if(sound_is_loaded()) {
        // It has to be done before the togglePause
        if(sound_is_playing()) {
            window->label(_("KISS Player - Paused"));
            button_play->image(img_icon_play);
            button_play->redraw();
#ifdef WIN32
            win_specific_update_thumbnail_toolbar("play");
#endif
        } else {
            button_play->image(img_icon_pause);
            button_play->redraw();
            if(!FLAG_SCROLL_TITLE) {
                window->copy_label(windowTitle.c_str());
            }
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

    musicPlayingCod = 0;
    sound_active(false);
    window->label(_("KISS Player"));
    lyrics_text_buffer->text("");
    button_play->image(img_icon_play);
    button_play->redraw();

    slider_music->reset();

    browser_music->clear_highlighted();
    browser_music->redraw();
}

void cb_previous(Fl_Widget* widget, void*)
{
    // If there's no music on the list or there's no music playing, do not continue
    if(listMusic.empty() || !sound_is_loaded()) {
        return;
    }

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

        play_music();
    }
}

void cb_music_browser(Fl_Widget* widget, void*)
{
    if(browser_music->dnd_evt) {
        browser_music->dnd_evt = false;
        bool list_changed = util_parse_dnd(browser_music->evt_txt, listMusic);
        if(list_changed) {
            update_playlist();
        }
        return;
    }

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
    sound_volume(volume_controller->value2());
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

    if(strcmp(choice, _("All")) == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ALL;
    else if(strcmp(choice, _("Title")) == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_TITLE;
    else if(strcmp(choice, _("Artist")) == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ARTIST;
    else if(strcmp(choice, _("Album")) == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ALBUM;

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
        menu_items[MENU_ITEM_RANDOM].set();
    } else {
        menu_items[MENU_ITEM_RANDOM].clear();
    }
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

int handler(int e, Fl_Window* w)
{
    // When the slider is released, set the sound's position
    if(e == FL_RELEASE && Fl::belowmouse() == slider_music && sound_is_loaded() && !Fl::event_buttons()) {
        // Needed to update without delay
        slider_music->update_time();

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
            volume_controller->decrease();
            cb_volume(volume_controller, NULL);
            return 0;
        case FL_Volume_Up: // Decrease Volume
            volume_controller->increase();
            cb_volume(volume_controller, NULL);
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
        case 'k': // Search bar gets focused
            if(Fl::event_ctrl()) {
                input_search->take_focus();
                input_search->position(0,input_search->size());
                input_search->redraw();
            }
            return 0;
        case FL_F + 6: // Toggles seach bar focus
            if(input_search == Fl::focus()) {
                browser_music->take_focus();
            } else {
                input_search->take_focus();
                input_search->position(0,input_search->size());
                input_search->redraw();
            }
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
        (Fl_Window::current() == window || slider_music->is_tipwin_current())) {
            volume_controller->increase(Fl::event_dy() * -1);
            cb_volume(volume_controller, NULL);
            return 0;
    }

    return Fl::handle_(e, w);
}

void play_music()
{
    Music music = listMusic.at(musicIndex);
    musicPlayingCod = music.cod;
    const char* filepath = music.filepath.c_str();

    // TODO: First we need to check if the file exists
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

    browser_music->value(musicIndex + 1);
    browser_music->set_highlighted(musicIndex + 1);
    browser_music->redraw();

    sound_load(filepath);
    sound_play();
    sound_volume(volume_controller->value2());

    windowTitle = "KISS Player :: ";
    windowTitle.append(browser_music->text(musicIndex + 1));
    windowTitlePosition = 0;
    window->copy_label(windowTitle.c_str());

    button_play->image(img_icon_pause);
    button_play->redraw();
#ifdef WIN32
    win_specific_update_thumbnail_toolbar("pause");
#endif

    slider_music->realise_new_sound();

    if(FLAG_LYRICS) {
        lyrics_text_buffer->text(_("Fetching..."));
        lyrics_fetcher_run(lyrics_text_buffer, music.artist, music.title);
        lyrics_pane->scroll(0, 0);
    } else {
        lyrics_text_buffer->text(_("Lyrics Disabled"));
    }
}

void timer_title_scrolling(void*)
{
    Fl::repeat_timeout(0.2, timer_title_scrolling);
    if (!FLAG_SCROLL_TITLE || !sound_is_loaded() || !sound_is_playing()) {
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

    //box_current_time->copy_label(util_format_time(sound_position()));
    //box_current_time->damage();

    if(Fl::pushed() != slider_music) {
        slider_music->value(sound_position());
        slider_music->update_time();
    }
}

void timer_play_at_start(void*)
{
    musicIndex = -1;
    musicIndexRandom = 0;

    if(FLAG_RANDOM) {
        musicIndex = listRandom.at(musicIndexRandom++);
    } else {
        musicIndex++;
    }

    play_music();
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
    dao_set_key("window_maximized", util_i2s(os_specific_is_window_maximized()));
    dao_set_key("browser_music_width", util_i2s(browser_music->w()));
    dao_set_key("volume_level", util_i2s(volume_controller->value()));
    dao_set_key("random_button", util_i2s(FLAG_RANDOM));
    dao_set_key("search_input", lastSearch);
    dao_set_key("search_type", util_i2s(FLAG_SEARCH_TYPE));
    dao_set_key("music_index", util_i2s(musicIndex));
    dao_set_key("music_index_random", util_i2s(musicIndexRandom));
    dao_set_key("color_background", util_i2s(browser_music->color()));
    dao_set_key("color_selection", util_i2s(browser_music->color2()));
    dao_set_key("color_text", util_i2s(browser_music->textcolor()));
    dao_set_key("lyrics", util_i2s(FLAG_LYRICS));
    dao_set_key("scroll_title", util_i2s(FLAG_SCROLL_TITLE));

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
    float volume = util_s2i(dao_get_key("volume_level"));
    if(volume != -1) {
        volume_controller->value(volume);
        cb_volume(volume_controller, NULL);
    }

    // TOGGLE RANDOMIZE
    int random = util_s2i(dao_get_key("random_button"));
    if(random != -1) {
        // It should be inverted, because...
        FLAG_RANDOM = !random;
        // ...here we toggle the Random Button
        cb_random(NULL, 0);
    }

    // SET SEARCH TYPE
    int search_type = util_s2i(dao_get_key("search_type"));
    if(search_type != -1) {
        FLAG_SEARCH_TYPE = search_type;
        choice_search_type->value(FLAG_SEARCH_TYPE);
    }

    // SET SEARCH STRING
    if(!doNotLoadLastSearch) {
        string search_input = dao_get_key("search_input");
        if(search_input.empty()) {
            dao_get_all_music(listMusic);
            update_playlist();
        } else {
            input_search->value(search_input.c_str());
            cb_search(NULL, 0);
        }
    }

    // Open DB again, update_playlist and cb_search close it
    dao_open_db();

    // SET LYRICS FLAG
    int lyrics = util_s2i(dao_get_key("lyrics"));
    if(lyrics != -1) {
        FLAG_LYRICS = lyrics;
    } else {
        FLAG_LYRICS = true;
    }

    // SET SCROLL TITLE FLAG
    int scroll_title = util_s2i(dao_get_key("scroll_title"));
    if(scroll_title != -1) {
        FLAG_SCROLL_TITLE = scroll_title;
    } else {
        FLAG_SCROLL_TITLE = true;
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
        color_selection = DEFAULT_SELECTION_COLOR;
    if(color_text == -1)
        color_text = DEFAULT_FOREGROUND_COLOR;

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
    browser_music->clear_highlighted();

    for(int i = 0; i < listMusic.size(); i++) {
        Music m = listMusic.at(i);
        browser_music->add(m.getDesc().c_str());
        if(m.cod != 0 && m.cod == musicPlayingCod) {
            browser_music->set_highlighted(i + 1);
            browser_music->value(i + 1);
        }
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

void window_main_reset_title()
{
    if (!sound_is_loaded() || !sound_is_playing()) {
        return;
    }

    windowTitlePosition = 0;

    string title = windowTitle.substr(windowTitlePosition, windowTitle.size());
    title.append(windowTitle.substr(0, windowTitlePosition));

    window->copy_label(title.c_str());
}
