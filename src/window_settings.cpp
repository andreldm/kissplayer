#include "window_settings.h"

#include <deque>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Select_Browser.H>

#include "widget/ksp_check_button.h"
#include "os_specific.h"
#include "window_main.h"
#include "dao.h"
#include "util.h"

using namespace std;


static Fl_Window* window;
static Fl_Button* button_background_color;
static Fl_Button* button_selection_color;
static Fl_Button* button_text_color;
static KSP_Check_Button* button_lyrics;
static KSP_Check_Button* button_scroll_title;
static Fl_Select_Browser* browser_directories;

static deque<COD_VALUE> listDir;

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

void window_settings_show(void)
{
    // To place the window at the center of the screen
    int window_w = 450;
    int window_h = 355;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window = new Fl_Window(window_x, window_y, window_w, window_h, "Settings");

    // GENERAL GROUP AND ITS WIDGETS
    int group_offset = 22;

    Fl_Group* group_general = new Fl_Group(5, group_offset, window_w - 10, 105, "General & Appearance");
    group_general->align(FL_ALIGN_TOP_LEFT);
    group_general->box(FL_UP_FRAME);
    group_general->begin();

    button_lyrics = new KSP_Check_Button(15, group_offset + 5, 120, 16, "Display Lyrics");
    button_lyrics->value(FLAG_LYRICS);
    button_lyrics->callback((Fl_Callback*)cb_lyrics);

    button_scroll_title = new KSP_Check_Button(15, group_offset + 30, 120, 16, "Scroll Title");
    button_scroll_title->value(FLAG_SCROLL_TITLE);
    button_scroll_title->callback((Fl_Callback*)cb_scroll_title);

    button_background_color = new Fl_Button(150, group_offset + 5, 16, 16, "Background Color");
    button_background_color->box(FL_DOWN_BOX);
    button_background_color->labelsize(12);
    button_background_color->clear_visible_focus();
    button_background_color->color(window_main_get_browser_music_color(1));
    button_background_color->callback((Fl_Callback*)cb_background_color);
    button_background_color->align(FL_ALIGN_RIGHT);

    button_selection_color = new Fl_Button(150, group_offset + 30, 16, 16, "Selection Color");
    button_selection_color->box(FL_DOWN_BOX);
    button_selection_color->labelsize(12);
    button_selection_color->clear_visible_focus();
    button_selection_color->color(window_main_get_browser_music_color(2));
    button_selection_color->callback((Fl_Callback*)cb_selection_color);
    button_selection_color->align(FL_ALIGN_RIGHT);

    button_text_color = new Fl_Button(150, group_offset + 55, 16, 16, "Text Color");
    button_text_color->box(FL_DOWN_BOX);
    button_text_color->labelsize(12);
    button_text_color->clear_visible_focus();
    button_text_color->color(window_main_get_browser_music_color(3));
    button_text_color->callback((Fl_Callback*)cb_text_color);
    button_text_color->align(FL_ALIGN_RIGHT);

    Fl_Button* button_default_colors = new Fl_Button(150, group_offset + 80, 95, 22, "Default Colors");
    button_default_colors->labelsize(12);
    button_default_colors->clear_visible_focus();
    button_default_colors->callback((Fl_Callback*)cb_default_colors);

    group_general->end();

    // DIRECTORIES GROUP AND ITS WIDGETS
    group_offset = 150;

    Fl_Group* group_directories = new Fl_Group(5, group_offset, window_w - 10, 162, "Directories");
    group_directories->align(FL_ALIGN_TOP_LEFT);
    group_directories->box(FL_UP_FRAME);
    group_directories->begin();

    browser_directories = new Fl_Select_Browser(10, group_offset + 5, window_w - 20, 120, 0);
    browser_directories->type(FL_HOLD_BROWSER);
    browser_directories->color(window_main_get_browser_music_color(1));
    browser_directories->color2(window_main_get_browser_music_color(2));
    browser_directories->textcolor(window_main_get_browser_music_color(3));

    Fl_Button* button_add = new Fl_Button(10, group_offset + 130, 40, 25, "Add");
    button_add->clear_visible_focus();
    button_add->callback((Fl_Callback*)cb_add_dir);

    Fl_Button* button_remove = new Fl_Button(55, group_offset + 130, 65, 25, "Remove");
    button_remove->clear_visible_focus();
    button_remove->callback((Fl_Callback*)cb_remove_dir);

    group_directories->end();

    // CLOSE BUTTON
    Fl_Button* button_close = new Fl_Button((window_w/2) - 30, window_h - 32, 60, 25, "Close");
    button_close->clear_visible_focus();
    button_close->callback((Fl_Callback*)cb_close);

    update_dir_list();

    window->set_modal();
    window->show();
}

void cb_close(Fl_Widget* widget, void*)
{
    window->clear();
    window->hide();
    window_main_get_instance()->show();
    delete window;
}

void cb_add_dir(Fl_Widget* widget, void*)
{
    char dir[8196]; // Seems enough
    os_specific_dir_chooser(dir);
    Fl::redraw();

    if(dir != NULL && strlen(dir) > 0) { // If the user didn't cancel;
        dao_insert_directory(dir);
        update_dir_list();
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

  fl_color_chooser("Choose Color", r, g, b);

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
