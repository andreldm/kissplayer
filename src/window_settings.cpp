#include "window_settings.h"

#include <deque>
#include <string>

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Select_Browser.H>

#include "widget/ksp_check_button.h"
#include "locale.h"
#include "os_specific.h"
#include "window_main.h"
#include "dao.h"
#include "util.h"
#include "sync.h"

using namespace std;

static Fl_Window* window;
static Fl_Button* button_background_color;
static Fl_Button* button_selection_color;
static Fl_Button* button_text_color;
static KSP_Check_Button* button_lyrics;
static KSP_Check_Button* button_scroll_title;
static Fl_Select_Browser* browser_directories;
static Fl_Choice* lang_choice;

static deque<COD_VALUE> listDir;
static bool should_resync;

static void update_dir_list();
static Fl_Color edit_color(Fl_Color val);

static void cb_close(Fl_Widget*, void*);
static void cb_add_dir(Fl_Widget*, void*);
static void cb_remove_dir(Fl_Widget*, void*);
static void cb_background_color(Fl_Widget*, void*);
static void cb_selection_color(Fl_Widget*, void*);
static void cb_text_color(Fl_Widget*, void*);
static void cb_default_colors(Fl_Widget*, void*);
static void cb_lyrics(Fl_Widget*, void*);
static void cb_scroll_title(Fl_Widget*, void*);
static void cb_change(Fl_Widget*, void*);

void window_settings_show(void)
{
    should_resync = false;

    // To place the window at the center of the screen
    int window_w = 450;
    int window_h = 415;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window = new Fl_Window(window_x, window_y, window_w, window_h, _("Settings"));

    // GENERAL GROUP AND ITS WIDGETS
    int group_offset = 22;

    Fl_Group* group_general = new Fl_Group(5, group_offset, window_w - 10, 105, _("General & Appearance"));
    group_general->align(FL_ALIGN_TOP_LEFT);
    group_general->box(FL_UP_FRAME);
    group_general->begin();

    button_lyrics = new KSP_Check_Button(15, group_offset + 5, 120, 16,_("Display Lyrics"));
    button_lyrics->value(FLAG_LYRICS);
    button_lyrics->callback((Fl_Callback*)cb_lyrics);

    button_scroll_title = new KSP_Check_Button(15, group_offset + 30, 120, 16, _("Scroll Title"));
    button_scroll_title->value(FLAG_SCROLL_TITLE);
    button_scroll_title->callback((Fl_Callback*)cb_scroll_title);

    button_background_color = new Fl_Button(150, group_offset + 5, 16, 16, _("Background Color"));
    button_background_color->box(FL_DOWN_BOX);
    button_background_color->labelsize(12);
    button_background_color->clear_visible_focus();
    button_background_color->color(window_main_get_browser_music_color(1));
    button_background_color->callback((Fl_Callback*)cb_background_color);
    button_background_color->align(FL_ALIGN_RIGHT);

    button_selection_color = new Fl_Button(150, group_offset + 30, 16, 16, _("Selection Color"));
    button_selection_color->box(FL_DOWN_BOX);
    button_selection_color->labelsize(12);
    button_selection_color->clear_visible_focus();
    button_selection_color->color(window_main_get_browser_music_color(2));
    button_selection_color->callback((Fl_Callback*)cb_selection_color);
    button_selection_color->align(FL_ALIGN_RIGHT);

    button_text_color = new Fl_Button(150, group_offset + 55, 16, 16, _("Text Color"));
    button_text_color->box(FL_DOWN_BOX);
    button_text_color->labelsize(12);
    button_text_color->clear_visible_focus();
    button_text_color->color(window_main_get_browser_music_color(3));
    button_text_color->callback((Fl_Callback*)cb_text_color);
    button_text_color->align(FL_ALIGN_RIGHT);

    Fl_Button* button_default_colors = new Fl_Button(150, group_offset + 80, 0, 22, _("Default Colors"));
    button_default_colors->labelsize(12);
    util_adjust_width(button_default_colors, 10);
    button_default_colors->clear_visible_focus();
    button_default_colors->callback((Fl_Callback*)cb_default_colors);

    group_general->end();

    // DIRECTORIES GROUP AND ITS WIDGETS
    group_offset = 150;

    Fl_Group* group_directories = new Fl_Group(5, group_offset, window_w - 10, 162, _("Directories"));
    group_directories->align(FL_ALIGN_TOP_LEFT);
    group_directories->box(FL_UP_FRAME);
    group_directories->begin();

    browser_directories = new Fl_Select_Browser(10, group_offset + 5, window_w - 20, 120, 0);
    browser_directories->type(FL_HOLD_BROWSER);
    browser_directories->color(window_main_get_browser_music_color(1));
    browser_directories->color2(window_main_get_browser_music_color(2));
    browser_directories->textcolor(window_main_get_browser_music_color(3));

    Fl_Button* button_add = new Fl_Button(10, group_offset + 130, 70, 25, _("Add"));
    util_adjust_width(button_add, 10);
    button_add->clear_visible_focus();
    button_add->callback((Fl_Callback*)cb_add_dir);

    Fl_Button* button_remove = new Fl_Button(15 + button_add->w(), group_offset + 130, 70, 25, _("Remove"));
    util_adjust_width(button_remove, 10);
    button_remove->clear_visible_focus();
    button_remove->callback((Fl_Callback*)cb_remove_dir);

    group_directories->end();

    group_offset = 335;

    // LANGUAGE GROUP AND ITS WIDGETS
    Fl_Group* group_language = new Fl_Group(6, group_offset, window_w -10, 40, _("Language"));
    group_language->align(FL_ALIGN_TOP_LEFT );
    group_language->box(FL_UP_FRAME);
    group_language->begin();

    lang_choice = new Fl_Choice(10, group_offset + 8, 200, 25);
    lang_choice->clear_visible_focus();

    Language** languages = Locale::getDefinedLanguages();
    for (int i = 0; languages[i]; i++) {
        lang_choice->add(languages[i]->description.c_str());
    }
    lang_choice->callback(cb_change);

    dao_open_db();
    int index = util_s2i(dao_get_key("lang_index"));
    if (index < 0) index = 0;
    lang_choice->value(index);
    dao_close_db();

    group_language->end();

    // CLOSE BUTTON
    Fl_Button* button_close = new Fl_Button(0, window_h - 32, 60, 25, _("Close"));
    util_adjust_width(button_close, 10);
    button_close->position((window_w / 2) - (button_close->w() / 2), button_close->y());

    button_close->clear_visible_focus();
    button_close->callback((Fl_Callback*)cb_close);

    update_dir_list();

    window->set_modal();
    window->callback((Fl_Callback*)cb_close);
    window->show();
}

void cb_close(Fl_Widget* widget, void*)
{
    window->clear();
    window->hide();
    window_main_get_instance()->show();
    delete window;
    if(should_resync) sync_execute(true);
}

void cb_add_dir(Fl_Widget* widget, void*)
{
    char dir[PATH_LENGTH];
    int dirQty = listDir.size();

    os_specific_dir_chooser(dir);
    Fl::redraw();

    if(dir != NULL && strlen(dir) > 0) {
        // If the user didn't cancel
        dao_insert_directory(dir);
        update_dir_list();
    }

    if(listDir.size() != dirQty) {
        // If a directory was added
        should_resync = true;
    }
}

void cb_remove_dir(Fl_Widget* widget, void*)
{
    int index = browser_directories->value();
    if(index <= 0) return;

    string dir = browser_directories->text(index);
    for(int i = 0; i < listDir.size(); i++) {
        if(listDir.at(i).value == dir) {
            dao_delete_directory(listDir.at(i).cod);
            update_dir_list();
        }
    }

    should_resync = true;
}

void cb_background_color(Fl_Widget* widget, void*)
{
    Fl_Color val = edit_color(button_background_color->color());

    button_background_color->color(val);
    button_background_color->redraw();

    browser_directories->color(val);
    browser_directories->redraw();

    window_main_set_browser_music_color(val, -1, -1);
    window_main_set_lyrics_pane_color(val, -1, -1);
}

void cb_selection_color(Fl_Widget* widget, void*)
{
    Fl_Color val = edit_color(button_selection_color->color());

    button_selection_color->color(val);
    button_selection_color->redraw();

    browser_directories->color2(val);
    browser_directories->redraw();

    window_main_set_input_search_type_color(val);
    window_main_set_browser_music_color(-1, val, -1);
    window_main_set_lyrics_pane_color(-1, val, -1);
    window_main_set_choice_search_type_color(val);
}

void cb_text_color(Fl_Widget* widget, void*)
{
    Fl_Color val = edit_color(button_text_color->color());

    button_text_color->color(val);
    button_text_color->redraw();

    browser_directories->textcolor(val);
    browser_directories->redraw();

    window_main_set_browser_music_color(-1, -1, val);
    window_main_set_lyrics_pane_color(-1, -1, val);
}

void cb_default_colors(Fl_Widget* widget, void*)
{
    window_main_set_input_search_type_color(DEFAULT_SELECTION_COLOR);
    window_main_set_choice_search_type_color(DEFAULT_SELECTION_COLOR);
    window_main_set_browser_music_color(DEFAULT_BACKGROUND_COLOR, DEFAULT_SELECTION_COLOR, DEFAULT_FOREGROUND_COLOR);
    window_main_set_lyrics_pane_color(DEFAULT_BACKGROUND_COLOR, DEFAULT_SELECTION_COLOR, DEFAULT_FOREGROUND_COLOR);

    button_text_color->color(DEFAULT_FOREGROUND_COLOR);
    button_background_color->color(DEFAULT_BACKGROUND_COLOR);
    button_selection_color->color(DEFAULT_SELECTION_COLOR);
    browser_directories->color(DEFAULT_BACKGROUND_COLOR);
    browser_directories->color2(DEFAULT_SELECTION_COLOR);
    browser_directories->textcolor(DEFAULT_FOREGROUND_COLOR);

    button_text_color->redraw();
    button_background_color->redraw();
    button_selection_color->redraw();
    browser_directories->redraw();
}

void cb_lyrics(Fl_Widget* widget, void*)
{
    FLAG_LYRICS = !FLAG_LYRICS;
}

void cb_scroll_title(Fl_Widget* widget, void*)
{
    FLAG_SCROLL_TITLE = !FLAG_SCROLL_TITLE;

    if(!FLAG_SCROLL_TITLE) {
        window_main_reset_title();
    }
}

Fl_Color edit_color(Fl_Color val)
{
  uchar r, g, b;
  Fl::get_color(val, r, g, b);

  fl_color_chooser(_("Choose Color"), r, g, b);

  return(fl_rgb_color(r, g, b));
}

void update_dir_list()
{
    browser_directories->clear();
    listDir.clear();
    dao_get_directories(listDir);

    for(int i = 0; i < listDir.size(); i++) {
        string s = listDir.at(i).value;
        browser_directories->add(s.c_str());
    }
}

void cb_change(Fl_Widget* widget, void*)
{
    Locale::setLanguage(lang_choice->value());

    fl_beep();
    fl_message_title(_("Warning"));
    fl_message(_("Please, restart the program to change the language."));
}
