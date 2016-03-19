#include "window_main.h"

#include <deque>
#include <string>

#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Tile.H>
#include <FL/filename.H>

#include "images.h"
#include "locale.h"
#include "os_specific.h"
#include "widget/ksp_menu_item.h"
#include "sound.h"
#include "util.h"
#include "signals.h"
#include "sync.h"

using namespace std;

// VARIABLES
static string windowTitle;
static int windowTitlePosition;
static string lastSearch;
static bool skipLastSearch;

// FUNCTIONS
static void timer_title_scrolling(void*);
static void timer_check_music(void*);
static void timer_play_at_start(void*);
static int handler(int, Fl_Window*);

// CALLBACKS
static void cb_close_window(Fl_Widget*, void*);
static void cb_menu(Fl_Widget*, void*);
static void cb_sync(Fl_Widget*, void*);
static void cb_random(Fl_Widget*, void*);
static void cb_repeat(Fl_Widget*, void*);
static void cb_slider_music(Fl_Widget*, void*);

// PRIVATE SIGNALS
static sigc::signal<void> SignalMusicBrowser;
static sigc::signal<void, Fl_Widget*> SignalClose;
static sigc::signal<void> SignalShowAbout;
static sigc::signal<void> SignalShowSettings;
static sigc::signal<void> SignalClearSearch;

// MENU ITEMS INDEX
enum {
    MENU_ITEM_SYNC = 0,
    MENU_ITEM_RANDOM,
    MENU_ITEM_REPEAT,
    MENU_ITEM_SETTINGS,
    MENU_ITEM_ABOUT,
    MENU_ITEM_NONE
};

// MENU ITEMS
static KSP_Menu_Item menu_items[] = {
    KSP_Menu_Item(0, 0, cb_sync, NULL),
    KSP_Menu_Item(0, 0, cb_random, NULL, FL_MENU_TOGGLE),
    KSP_Menu_Item(0, 0, cb_repeat, NULL, FL_MENU_TOGGLE),
    KSP_Menu_Item(0, 0, [](Fl_Widget*, void*) { SignalShowSettings.emit(); }, NULL),
    KSP_Menu_Item(0, 0, [](Fl_Widget*, void*) { SignalShowAbout.emit(); }, NULL),
    KSP_Menu_Item(0)
};

WindowMain::WindowMain(Sound* sound, Dao* dao)
    : Fl_Double_Window(0, 0, "KISS Player")
{
    this->sound = sound;
    this->dao = dao;
}

void WindowMain::init(int argc, char** argv)
{
    // Place the window at the center of the screen
    int window_w = 770;
    int window_h = 465;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    xclass("KISS Player");
    resize(window_x, window_y, window_w, window_h);
    size_range(420, 370);
    callback([](Fl_Widget* w, void*) {
        SignalClose.emit(w);
    });

    Configuration::instance()->load(dao);

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
    input_search->callback([](Fl_Widget*, void*) {
        SignalSearch.emit();
    });

    choice_search_type = new Fl_Choice(598, 10, 80, 22);
    choice_search_type->add(_("All"));
    choice_search_type->add(_("Title"));
    choice_search_type->add(_("Artist"));
    choice_search_type->add(_("Album"));
    choice_search_type->value(0);
    choice_search_type->clear_visible_focus();
    choice_search_type->callback([](Fl_Widget*, void*) {
        SignalSearchType.emit();
    });
    searchType = SEARCH_TYPE_ALL;

    button_clear = new Fl_Button(684, 10, 22, 22);
    button_clear->image(img_icon_clear);
    button_clear->clear_visible_focus();
    button_clear->callback([](Fl_Widget*, void*) {
        SignalClearSearch.emit();
    });

    button_search = new Fl_Button(711, 10, 22, 22);
    button_search->image(img_icon_search);
    button_search->clear_visible_focus();
    button_search->callback([](Fl_Widget*, void*) {
        SignalSearch.emit();
    });

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
    browser_music->callback([](Fl_Widget*, void*) {
        SignalMusicBrowser.emit();
    });

    lyrics_text_buffer = new Fl_Text_Buffer();

    lyrics_pane = new Fl_Text_Display (560, 40, 205, 360, NULL);
    lyrics_pane->buffer(lyrics_text_buffer);
    lyrics_pane->box(FL_DOWN_BOX);
    lyrics_pane->textfont(FL_HELVETICA);
    lyrics_pane->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
    lyrics_pane->scrollbar_width(15);
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
    button_play->callback([](Fl_Widget*, void*) {
        SignalPlay.emit();
    });

    button_stop = new Fl_Button(47, 420, 25, 25);
    button_stop->clear_visible_focus();
    button_stop->image(img_icon_stop);
    button_stop->tooltip(_("Stop"));
    button_stop->callback([](Fl_Widget* w, void* u) {
        SignalStop.emit();
    });

    button_previous = new Fl_Button(82, 420, 25, 25);
    button_previous->clear_visible_focus();
    button_previous->image(img_icon_previous);
    button_previous->tooltip(_("Previous"));
    button_previous->callback([](Fl_Widget* w, void* u) {
        SignalPrevious.emit();
    });

    button_next = new Fl_Button(117, 420, 25, 25);
    button_next->clear_visible_focus();
    button_next->image(img_icon_next);
    button_next->tooltip(_("Next"));
    button_next->callback([](Fl_Widget* w, void* u) {
        SignalNext.emit();
    });

    slider_music = new KSP_Slider(this, 157, 424, 527, 19, "00:00");
    slider_music->callback(cb_slider_music);

    volume_controller = new KSP_Volume_Controller(699, 418, 60, 30);
    volume_controller->value(8);
    volume_controller->tooltip(_("Volume"));
    volume_controller->callback([](Fl_Widget* w, void* u) {
        SignalVolume.emit(((KSP_Volume_Controller *) w)->value2());
    });

    group_controls->resizable(slider_music);
    group_controls->end();

    // END OF WIDGET'S SETUP

    // Set menu labels and icons
    menu_items[MENU_ITEM_SYNC].set_label_icon(_("Synchronize Library"), &img_icon_sync);
    menu_items[MENU_ITEM_RANDOM].set_label_icon(_("Randomize"), 0);
    menu_items[MENU_ITEM_REPEAT].set_label_icon(_("Repeat Playlist"),0);
    menu_items[MENU_ITEM_SETTINGS].set_label_icon(_("Settings"), &img_icon_settings);
    menu_items[MENU_ITEM_ABOUT].set_label_icon(_("About"), &img_icon_about);

    // Fl::event_dispatch(handler);
    // Fl::add_timeout(0.5, timer_check_music);
    // Fl::add_timeout(0.2, timer_title_scrolling);

    resizable(tile_center);
    end();

    SignalClose.connect(sigc::mem_fun(this, &WindowMain::close_window));
    SignalPlay.connect(sigc::mem_fun(this, &WindowMain::toggle_play));
    SignalStop.connect(sigc::mem_fun(this, &WindowMain::stop));
    SignalPrevious.connect(sigc::mem_fun(this, &WindowMain::previous));
    SignalNext.connect(sigc::mem_fun(this, &WindowMain::next));
    SignalSearch.connect(sigc::mem_fun(this, &WindowMain::search));
    SignalSearchType.connect(sigc::mem_fun(this, &WindowMain::search_type));
    SignalVolume.connect(sigc::mem_fun(this, &WindowMain::volume_changed));
    SignalMusicBrowser.connect(sigc::mem_fun(this, &WindowMain::music_browser_event));
    SignalShowAbout.connect(sigc::mem_fun(this, &WindowMain::show_about));
    SignalShowSettings.connect(sigc::mem_fun(this, &WindowMain::show_settings));
    SignalUpdateColors.connect(sigc::mem_fun(this, &WindowMain::update_colors));
    SignalClearSearch.connect(sigc::mem_fun(this, &WindowMain::clear_search));
    SignalUpdateMusicPlaying.connect(sigc::mem_fun(this, &WindowMain::update_music_playing));

    // Init other components
    this->playlist = new Playlist(dao, sound, browser_music);
    this->lyricsFetcher = new LyricsFetcher(dao, sound, lyrics_text_buffer);

    // Check for music files on arguments
    skipLastSearch = playlist->parse_args(argc, argv);

    SignalUpdateColors.emit();

    // FIXME: Remove this
    search();
}

void WindowMain::close_window(Fl_Widget* widget)
{
    // ignore Escape key
    if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
        return;
    }

    SaveData data;
    data.x = x();
    data.y = y();
    data.w = w();
    data.h = h();

    Configuration::instance()->save(data, dao);

    widget->~Fl_Widget();
}

void cb_menu(Fl_Widget* w, void*)
{
    const KSP_Menu_Item* m = menu_items[0].popup(w->x(), w->y() + w->h());
    if (m && m->callback()) m->do_callback(w, m->user_data());
}

void cb_slider_music(Fl_Widget* widget, void*)
{
    // If there's no music playing, do not change
    // The slider's changes are handled at timer_check_music and my_handler
    /*if(!sound_is_loaded()) {
        slider_music->value(0);
    }*/
}

void WindowMain::clear_search()
{
    input_search->value(NULL);
    input_search->do_callback();
}

void cb_random(Fl_Widget* widget, void*)
{
    /*Configuration::instance()->shouldRandomize(Configuration::instance()->shouldRandomize());

    if(Configuration::instance()->shouldRandomize()) {
        menu_items[MENU_ITEM_RANDOM].set();
    } else {
        menu_items[MENU_ITEM_RANDOM].clear();
    }*/
}

void cb_repeat(Fl_Widget* widget, void*)
{
    /*Configuration::instance()->shouldRepeatSong(Configuration::instance()->shouldRepeatSong());

    if(Configuration::instance()->shouldRepeatSong()) {
        menu_items[MENU_ITEM_REPEAT].set();
    } else {
        menu_items[MENU_ITEM_REPEAT].clear();
    }*/
}

void WindowMain::show_settings()
{
    delete windowSettings;
    windowSettings = new WindowSettings(this->dao);
    windowSettings->show(this);
}

void WindowMain::show_about()
{
    delete windowAbout;
    windowAbout = new WindowAbout();
    windowAbout->show(this);
}

void cb_sync(Fl_Widget* widget, void*)
{
    /*sync_execute();*/
}
/*
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
        case FL_Up: // Cycle search type
            if(Fl::event_ctrl() && input_search == Fl::focus()) {
                int v = choice_search_type->value() -1;
                if(v >= 0) choice_search_type->value(v);
                cb_search_type(choice_search_type, NULL);
                return 0;
            }
        case FL_Down: // Cycle search type
            if(Fl::event_ctrl() && input_search == Fl::focus()) {
                int v = choice_search_type->value() +1;
                if(v < choice_search_type->size()) choice_search_type->value(v);
                cb_search_type(choice_search_type, NULL);
                return 0;
            }
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
}*/

void WindowMain::music_browser_event()
{
    /*if(browser_music->dnd_evt) {
        browser_music->dnd_evt = false;
        bool list_changed = util_parse_dnd(browser_music->evt_txt, listMusic);
        if(list_changed) {
            update_playlist();
        }
        return;
    }*/

    if(Fl::event_clicks() > 0 && Fl::event_button() == FL_LEFT_MOUSE) {
        play(true);
    }
}

void WindowMain::toggle_play()
{
    if(sound->isLoaded()) {
        // It has to be done before the togglePause
        if(sound->isPlaying()) {
            button_play->image(img_icon_play);
            label(_("KISS Player - Paused"));
// #ifdef WIN32
//             win_specific_update_thumbnail_toolbar("play");
// #endif
        } else {
            button_play->image(img_icon_pause);
            if(!Configuration::instance()->shouldScrollTitle())
                copy_label(windowTitle.c_str());

// #ifdef WIN32
//             win_specific_update_thumbnail_toolbar("pause");
// #endif
        }

        button_play->redraw();
        sound->togglePaused();
    } else if(!playlist->isEmpty()) {
        play(false);
    }
}

void WindowMain::play(bool playAtMusicBrowser)
{
    float volume = volume_controller->value2();
    if(!playlist->play(volume, playAtMusicBrowser)) return;
}

void WindowMain::stop()
{
    if(!playlist->stop()) {
        return;
    }

    label(_("KISS Player"));
    lyrics_text_buffer->text("");
    button_play->image(img_icon_play);
    button_play->redraw();

    slider_music->reset();

    browser_music->clear_highlighted();
    browser_music->redraw();
}

void WindowMain::next()
{
    playlist->next();
}

void WindowMain::previous()
{
    playlist->previous();
}

void WindowMain::search_type()
{
    const char* choice = choice_search_type->text();

    if(strcmp(choice, _("All")) == 0) searchType = SEARCH_TYPE_ALL;
    else if(strcmp(choice, _("Title")) == 0) searchType = SEARCH_TYPE_TITLE;
    else if(strcmp(choice, _("Artist")) == 0) searchType = SEARCH_TYPE_ARTIST;
    else if(strcmp(choice, _("Album")) == 0) searchType = SEARCH_TYPE_ALBUM;

    input_search->take_focus();
    input_search->do_callback();
}

void WindowMain::search()
{
    lastSearch = input_search->value();
    playlist->search(lastSearch, searchType);
}

void WindowMain::volume_changed(float volume)
{
    sound->setVolume(volume);
}

void WindowMain::reset_title()
{
    if (!sound->isLoaded() || !sound->isPlaying()) {
        return;
    }

    windowTitlePosition = 0;

    string title = windowTitle.substr(windowTitlePosition, windowTitle.size());
    title.append(windowTitle.substr(0, windowTitlePosition));

    copy_label(title.c_str());
}

void WindowMain::update_colors()
{
    browser_music->color(Configuration::instance()->background());
    browser_music->color2(Configuration::instance()->foreground());
    lyrics_pane->color(Configuration::instance()->background());
    lyrics_pane->color2(Configuration::instance()->foreground());
    input_search->color2(Configuration::instance()->foreground());
    choice_search_type->color2(Configuration::instance()->foreground());
    browser_music->textcolor(Configuration::instance()->textcolor());
    lyrics_pane->textcolor(Configuration::instance()->textcolor());

    browser_music->redraw();
    lyrics_pane->redraw();
    input_search->redraw();
    choice_search_type->redraw();
}

void WindowMain::update_music_playing (int index) {
    index += 1;

    browser_music->value(index);
    browser_music->set_highlighted(index);
    browser_music->redraw();

    windowTitle = "KISS Player :: ";
    windowTitle.append(browser_music->text(index));
    windowTitlePosition = 0;
    copy_label(windowTitle.c_str());

    button_play->image(img_icon_pause);
    button_play->redraw();
    // #ifdef WIN32
    //     win_specific_update_thumbnail_toolbar("pause");
    // #endif

    slider_music->realise_new_sound(sound->length());

    if(Configuration::instance()->shouldFetchLyrics()) {
        lyrics_text_buffer->text(_("Fetching..."));
        Music music = playlist->getCurrentMusic();
        lyricsFetcher->fetch(&music);
        lyrics_pane->scroll(0, 0);
    } else {
        lyrics_text_buffer->text(_("Lyrics Disabled"));
    }
}
