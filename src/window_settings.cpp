#include "window_settings.h"

#include <deque>
#include <string>

#include <sigc++/sigc++.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Select_Browser.H>

#include "configuration.h"
#include "widget/ksp_check_button.h"
#include "locale.h"
#include "os_specific.h"
#include "window_main.h"
#include "dao.h"
#include "util.h"
#include "sync.h"

using namespace std;

// static Fl_Window* window;
static Fl_Button* button_background_color;
static Fl_Button* button_selection_color;
static Fl_Button* button_text_color;
static KSP_Check_Button* button_lyrics;
static KSP_Check_Button* button_scroll_title;
static Fl_Select_Browser* browser_directories;
// static Fl_Choice* lang_choice;
static Fl_Hold_Browser* tab_selector;
static Fl_Input* input_proxy;
static Fl_Group* tabs[4] = { 0,0,0,0 };

static deque<COD_VALUE> listDir;
// static bool should_resync;

static void update_dir_list();
// static Fl_Color edit_color(Fl_Color val);

static void cb_add_dir(Fl_Widget*, void*);
static void cb_remove_dir(Fl_Widget*, void*);
static void cb_background_color(Fl_Widget*, void*);
static void cb_selection_color(Fl_Widget*, void*);
static void cb_text_color(Fl_Widget*, void*);
static void cb_default_colors(Fl_Widget*, void*);
// static void cb_lyrics(Fl_Widget*, void*);
// static void cb_scroll_title(Fl_Widget*, void*);
// static void cb_change(Fl_Widget*, void*);
static void cb_tab_select(Fl_Widget*, void*);
static void cb_proxy(Fl_Widget*, void*);

static sigc::signal<void> SignalClose;

WindowSettings::WindowSettings(Dao* dao)
        : Fl_Window(650, 300, _("Settings"))
{
    this->dao = dao;

    // should_resync = false;
    tab_selector = new Fl_Hold_Browser(10, 10, 200, h() - 20);
    tab_selector->color(Configuration::instance()->background());
    tab_selector->color2(Configuration::instance()->foreground());
    tab_selector->textcolor(Configuration::instance()->textcolor());
    tab_selector->add(_("General"));
    tab_selector->add(_("Directories"));
    tab_selector->add(_("Network"));
#ifdef WIN32
    tab_selector->add(_("Language"));
#endif

    // TAB GENERAL
    tabs[0] = new Fl_Group(215, 10, w() - 225, h() - 20, _("General"));
    tabs[0]->labelsize(16);
    tabs[0]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[0]->box(FL_UP_FRAME);
    tabs[0]->begin();

    button_lyrics = new KSP_Check_Button(220, 40, 120, 16,_("Display Lyrics"));
    // button_lyrics->value(FLAG_LYRICS);
    // button_lyrics->callback((Fl_Callback*)cb_lyrics);

    button_scroll_title = new KSP_Check_Button(220, 65, 120, 16, _("Scroll Title"));
    // button_scroll_title->value(FLAG_SCROLL_TITLE);
    // button_scroll_title->callback((Fl_Callback*)cb_scroll_title);

    button_background_color = new Fl_Button(220, 100, 16, 16, _("Background Color"));
    button_background_color->box(FL_DOWN_BOX);
    button_background_color->labelsize(12);
    button_background_color->clear_visible_focus();
    button_background_color->color(Configuration::instance()->background());
    button_background_color->callback((Fl_Callback*)cb_background_color);
    button_background_color->align(FL_ALIGN_RIGHT);

    button_selection_color = new Fl_Button(220, 125, 16, 16, _("Selection Color"));
    button_selection_color->box(FL_DOWN_BOX);
    button_selection_color->labelsize(12);
    button_selection_color->clear_visible_focus();
    button_selection_color->color(Configuration::instance()->foreground());
    button_selection_color->callback((Fl_Callback*)cb_selection_color);
    button_selection_color->align(FL_ALIGN_RIGHT);

    button_text_color = new Fl_Button(220, 150, 16, 16, _("Text Color"));
    button_text_color->box(FL_DOWN_BOX);
    button_text_color->labelsize(12);
    button_text_color->clear_visible_focus();
    button_text_color->color(Configuration::instance()->textcolor());
    button_text_color->callback((Fl_Callback*)cb_text_color);
    button_text_color->align(FL_ALIGN_RIGHT);

    Fl_Button* button_default_colors = new Fl_Button(220, 175, 0, 22, _("Default Colors"));
    button_default_colors->labelsize(12);
    util_adjust_width(button_default_colors, 10);
    button_default_colors->clear_visible_focus();
    button_default_colors->callback((Fl_Callback*)cb_default_colors);

    tabs[0]->end();

    // TAB DIRECTORIES
    tabs[1] = new Fl_Group(215, 10, w() - 225, h() - 20, _("Directories"));
    tabs[1]->labelsize(16);
    tabs[1]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[1]->box(FL_UP_FRAME);
    tabs[1]->begin();

    browser_directories = new Fl_Select_Browser(220, 40, w() - 235, 160, 0);
    browser_directories->type(FL_HOLD_BROWSER);
    browser_directories->color(Configuration::instance()->background());
    browser_directories->color2(Configuration::instance()->foreground());
    browser_directories->textcolor(Configuration::instance()->textcolor());

    Fl_Button* button_add = new Fl_Button(220, 205, 70, 25, _("Add"));
    util_adjust_width(button_add, 10);
    button_add->clear_visible_focus();
    button_add->callback((Fl_Callback*)cb_add_dir);

    Fl_Button* button_remove = new Fl_Button(220 + button_add->w() + 5, 205, 70, 25, _("Remove"));
    util_adjust_width(button_remove, 10);
    button_remove->clear_visible_focus();
    button_remove->callback((Fl_Callback*)cb_remove_dir);

    tabs[1]->end();

    // TAB NETWORK
    tabs[2] = new Fl_Group(215, 10, w() - 225, h() - 20, _("Network"));
    tabs[2]->labelsize(16);
    tabs[2]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[2]->box(FL_UP_FRAME);
    tabs[2]->begin();

    Fl_Box* label_proxy = new Fl_Box(220, 40, w() - 235, 22, _("Proxy:"));
    util_adjust_width(label_proxy);

    input_proxy = new Fl_Input(215 + 8 + label_proxy->w(), 40, w() - 240 - label_proxy->w(), 22, 0);
    input_proxy->tooltip(_("Complete proxy URL, ex: http://192.168.1.1:3128"));
    dao->open_db();
    input_proxy->value(dao->get_key("proxy").c_str());
    dao->close_db();
    input_proxy->callback(cb_proxy);
    tabs[2]->end();

    // TAB LANGUAGE
    tabs[3] = new Fl_Group(215, 10, w() - 225, h() - 20, _("Language"));
#ifdef WIN32
    tabs[3]->labelsize(16);
    tabs[3]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[3]->box(FL_UP_FRAME);
    tabs[3]->begin();

    lang_choice = new Fl_Choice(220, 40, 200, 25);
    lang_choice->clear_visible_focus();

    Language** languages = Locale::getDefinedLanguages();
    for (int i = 0; languages[i]; i++) {
        lang_choice->add(languages[i]->description.c_str());
    }
    lang_choice->callback(cb_change);

    dao->open_db();
    int index = util_s2i(dao->get_key("lang_index"));
    dao->close_db();
    lang_choice->value((index < 0 ? 0 : index));
#endif
    tabs[3]->end();

    // CLOSE BUTTON
    Fl_Button* button_close = new Fl_Button(0, h() - 40, 60, 25, _("Close"));
    util_adjust_width(button_close, 10);
    button_close->position(tabs[0]->x() + (tabs[0]->w() / 2) - (button_close->w() / 2), button_close->y());
    button_close->clear_visible_focus();
    button_close->callback([](Fl_Widget *w, void *u) {
        SignalClose.emit();
    });

    update_dir_list();

    tab_selector->select(1);
    tab_selector->callback(cb_tab_select);
    tab_selector->do_callback();

    set_modal();
    callback([](Fl_Widget *w, void *u) {
        SignalClose.emit();
    });

    SignalClose.connect(sigc::mem_fun(this, &WindowSettings::close));
}

void WindowSettings::show(Fl_Window* parent)
{
    util_center_window(this, parent);
    Fl_Window::show();
}

void WindowSettings::close()
{
    clear();
    hide();
    /*window_main_get_instance()->show();*/
    /*if(should_resync) sync_execute(true);*/
}

void cb_add_dir(Fl_Widget* widget, void*)
{
    /*char dir[PATH_LENGTH] = "";
    int dirQty = listDir.size();

    os_specific_dir_chooser(dir);
    Fl::redraw();

    if(strlen(dir) > 0) {
        // If the user didn't cancel
        dao_insert_directory(dir);
        update_dir_list();
    }

    if(listDir.size() != dirQty) {
        // If a directory was added
        should_resync = true;
    }*/
}

void cb_remove_dir(Fl_Widget* widget, void*)
{
    /*int index = browser_directories->value();
    if(index <= 0) return;

    string dir = browser_directories->text(index);
    for(int i = 0; i < listDir.size(); i++) {
        if(listDir.at(i).value == dir) {
            dao_delete_directory(listDir.at(i).cod);
            update_dir_list();
        }
    }

    should_resync = true;*/
}

void cb_background_color(Fl_Widget* widget, void*)
{
    /*Fl_Color val = edit_color(button_background_color->color());

    button_background_color->color(val);
    button_background_color->redraw();

    browser_directories->color(val);
    browser_directories->redraw();

    tab_selector->color(val);
    tab_selector->redraw();

    window_main_set_browser_music_color(val, -1, -1);
    window_main_set_lyrics_pane_color(val, -1, -1);*/
}

void cb_selection_color(Fl_Widget* widget, void*)
{
    /*Fl_Color val = edit_color(button_selection_color->color());

    button_selection_color->color(val);
    button_selection_color->redraw();

    browser_directories->color2(val);
    browser_directories->redraw();

    tab_selector->color2(val);
    tab_selector->redraw();

    window_main_set_input_search_type_color(val);
    window_main_set_browser_music_color(-1, val, -1);
    window_main_set_lyrics_pane_color(-1, val, -1);
    window_main_set_choice_search_type_color(val);*/
}

void cb_text_color(Fl_Widget* widget, void*)
{
    /*Fl_Color val = edit_color(button_text_color->color());

    button_text_color->color(val);
    button_text_color->redraw();

    browser_directories->textcolor(val);
    browser_directories->redraw();

    tab_selector->textcolor(val);
    tab_selector->redraw();

    window_main_set_browser_music_color(-1, -1, val);
    window_main_set_lyrics_pane_color(-1, -1, val);*/
}

void cb_default_colors(Fl_Widget* widget, void*)
{
    /*window_main_set_input_search_type_color(DEFAULT_SELECTION_COLOR);
    window_main_set_choice_search_type_color(DEFAULT_SELECTION_COLOR);
    window_main_set_browser_music_color(DEFAULT_BACKGROUND_COLOR, DEFAULT_SELECTION_COLOR, DEFAULT_FOREGROUND_COLOR);
    window_main_set_lyrics_pane_color(DEFAULT_BACKGROUND_COLOR, DEFAULT_SELECTION_COLOR, DEFAULT_FOREGROUND_COLOR);

    button_text_color->color(DEFAULT_FOREGROUND_COLOR);
    button_background_color->color(DEFAULT_BACKGROUND_COLOR);
    button_selection_color->color(DEFAULT_SELECTION_COLOR);
    browser_directories->color(DEFAULT_BACKGROUND_COLOR);
    browser_directories->color2(DEFAULT_SELECTION_COLOR);
    browser_directories->textcolor(DEFAULT_FOREGROUND_COLOR);
    tab_selector->color(DEFAULT_BACKGROUND_COLOR);
    tab_selector->color2(DEFAULT_SELECTION_COLOR);
    tab_selector->textcolor(DEFAULT_FOREGROUND_COLOR);

    button_text_color->redraw();
    button_background_color->redraw();
    button_selection_color->redraw();
    browser_directories->redraw();
    tab_selector->redraw();*/
}

void WindowSettings::toogleLyrics()
{
    Configuration::instance()->shouldFetchLyrics(Configuration::instance()->shouldFetchLyrics());
}

void WindowSettings::toogleScrollTitle()
{
    Configuration::instance()->shouldScrollTitle(Configuration::instance()->shouldScrollTitle());

    /*if(!FLAG_SCROLL_TITLE) {
        window_main_reset_title();
    }*/
}

// Fl_Color edit_color(Fl_Color val)
// {
//   uchar r, g, b;
//   Fl::get_color(val, r, g, b);
//
//   fl_color_chooser(_("Choose Color"), r, g, b);
//
//   return(fl_rgb_color(r, g, b));
// }

void update_dir_list()
{
    /*browser_directories->clear();
    listDir.clear();
    dao_get_directories(listDir);

    for(int i = 0; i < listDir.size(); i++) {
        string s = listDir.at(i).value;
        browser_directories->add(s.c_str());
    }*/
}

// void cb_change(Fl_Widget* widget, void*)
// {
//     /*Locale::setLanguage(lang_choice->value());*/
//
//     fl_beep();
//     fl_message(_("Please, restart the program to change the language."));
// }

void cb_tab_select(Fl_Widget*, void*) {
    if (tab_selector->value() == 0) {
        return;
    }

    for (uint t = 0; t < sizeof(tabs) / sizeof(tabs[0]); t++) {
        if ((int) t == (tab_selector->value() - 1)) {
            tabs[t]->show();
        } else {
            tabs[t]->hide();
        }
    }
}

void cb_proxy(Fl_Widget*, void*) {
    /*dao_open_db();
    dao_set_key("proxy", input_proxy->value());
    dao_close_db();*/
}
