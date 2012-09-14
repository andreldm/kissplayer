#include "window_settings.h"

/**
 * A Window where the user sets the app configuration.
 */

//LOCAL WIDGETS
Fl_Window *window_settings;
Fl_Check_Button *button_settings_lyrics;
Fl_Button *button_settings_background_color;
Fl_Button *button_settings_selection_color;
Fl_Button *button_settings_text_color;
Fl_Button *button_settings_default_colors;
Fl_Button *button_settings_close;
Fl_Button *button_settings_add;
Fl_Button *button_settings_remove;
Fl_Select_Browser *browser_settings_directories;
Fl_Group *group_settings_appearance;
Fl_Group *group_settings_general;
Fl_Group *group_settings_directories;

//LOCAL VARIABLES
vector<NameCod *> *listDir;

//LOCAL FUNCTIONS
void update_dir_list();
Fl_Color edit_color(Fl_Color val);

//LOCAL CALLBACKS
void cb_settings_close(Fl_Widget*, void*);
void cb_settings_add_dir(Fl_Widget*, void*);
void cb_settings_remove_dir(Fl_Widget*, void*);
void cb_settings_background_color(Fl_Widget*, void*);
void cb_settings_selection_color(Fl_Widget*, void*);
void cb_settings_text_color(Fl_Widget*, void*);
void cb_settings_default_colors(Fl_Widget*, void*);
void cb_settings_lyrics(Fl_Widget*, void*);

Fl_Window* make_window_settings()
{
    //To place the window at the center of the screen
    int window_w = 450;
    int window_h = 340;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    Fl_Window *window = new Fl_Window(window_x, window_y, window_w, window_h, "Settings");

    //GENERAL GROUP AND ITS WIDGETS
    int group_offset = 22;

    group_settings_general = new Fl_Group(5, group_offset, window_w - 10, 30, "General");
    group_settings_general->align(FL_ALIGN_TOP_LEFT);
    group_settings_general->box(FL_UP_FRAME);
    group_settings_general->begin();

    button_settings_lyrics = new Fl_Check_Button(15, group_offset, 32, 32, "Display Lyrics");
    button_settings_lyrics->value(FLAG_LYRICS);
    button_settings_lyrics->clear_visible_focus();
    button_settings_lyrics->callback((Fl_Callback*)cb_settings_lyrics);

    group_settings_general->end();

    //APPEARANCE GROUP AND ITS WIDGETS
    group_offset = 75;

    group_settings_appearance = new Fl_Group(5, group_offset, window_w - 10, 30, "Appearance");
    group_settings_appearance->align(FL_ALIGN_TOP_LEFT);
    group_settings_appearance->box(FL_UP_FRAME);
    group_settings_appearance->begin();

    button_settings_background_color = new Fl_Button(115, group_offset + 5, 16, 16, "Background Color:");
    button_settings_background_color->box(FL_DOWN_BOX);
    button_settings_background_color->labelsize(12);
    button_settings_background_color->clear_visible_focus();
    button_settings_background_color->color(browser_music->color());
    button_settings_background_color->callback((Fl_Callback*)cb_settings_background_color);
    button_settings_background_color->align(FL_ALIGN_LEFT);

    button_settings_selection_color = new Fl_Button(245, group_offset + 5, 16, 16, "Selection Color:");
    button_settings_selection_color->box(FL_DOWN_BOX);
    button_settings_selection_color->labelsize(12);
    button_settings_selection_color->clear_visible_focus();
    button_settings_selection_color->color(browser_music->color2());
    button_settings_selection_color->callback((Fl_Callback*)cb_settings_selection_color);
    button_settings_selection_color->align(FL_ALIGN_LEFT);

    button_settings_text_color = new Fl_Button(348, group_offset + 5, 16, 16, "Text Color:");
    button_settings_text_color->box(FL_DOWN_BOX);
    button_settings_text_color->labelsize(12);
    button_settings_text_color->clear_visible_focus();
    button_settings_text_color->color(browser_music->textcolor());
    button_settings_text_color->callback((Fl_Callback*)cb_settings_text_color);
    button_settings_text_color->align(FL_ALIGN_LEFT);

    button_settings_default_colors = new Fl_Button(380, group_offset + 5, 56, 16, "Defaults");
    button_settings_default_colors->labelsize(12);
    button_settings_default_colors->clear_visible_focus();
    button_settings_default_colors->callback((Fl_Callback*)cb_settings_default_colors);

    group_settings_appearance->end();

    //DIRECTORIES GROUP AND ITS WIDGETS
    group_offset = 135;
    group_settings_directories = new Fl_Group(5, group_offset, window_w - 10, 162, "Directories");
    group_settings_directories->align(FL_ALIGN_TOP_LEFT);
    group_settings_directories->box(FL_UP_FRAME);
    group_settings_directories->begin();

    browser_settings_directories = new Fl_Select_Browser(10, group_offset + 5, window_w - 20, 120, 0);
    browser_settings_directories->color(43);
    browser_settings_directories->type(FL_HOLD_BROWSER);

    button_settings_add = new Fl_Button(10, group_offset + 130, 40, 25, "Add");
    button_settings_add->callback((Fl_Callback*)cb_settings_add_dir);

    button_settings_remove = new Fl_Button(55, group_offset + 130, 65, 25, "Remove");
    button_settings_remove->callback((Fl_Callback*)cb_settings_remove_dir);

    group_settings_directories->end();

    //CLOSE BUTTON
    button_settings_close = new Fl_Button((window_w/2)-30, window_h-32, 60, 25, "Close");
    button_settings_close->callback((Fl_Callback*)cb_settings_close);

    window->set_modal();
    window->end();

    update_dir_list();

    return window;
}

void cb_settings_close(Fl_Widget* widget, void*)
{
    window_settings->clear();
    window_settings->hide();
    window_settings->~Fl_Window();
}

void cb_settings_add_dir(Fl_Widget* widget, void*)
{
    const char *dir = native_dir_chooser();
    if(dir != NULL)//If the user didn't cancel;
    {
        insertDirectory(dir);
        update_dir_list();
    }
    delete[] dir;
}

void cb_settings_remove_dir(Fl_Widget* widget, void*)
{
    int index = browser_settings_directories->value();
    if(index <= 0) return;

    string dir = browser_settings_directories->text(index);
    for(int i = 0; i < listDir->size(); i++)
    {
        if(listDir->at(i)->name == dir)
        {
            deleteDirectory(listDir->at(i)->cod);
            update_dir_list();
        }
    }
}

void cb_settings_background_color(Fl_Widget* widget, void*)
{
    Fl_Color val = edit_color(button_settings_background_color->color());

    button_settings_background_color->color(val);
    browser_music->color(val);
    lyrics_pane->color(val);

    button_settings_background_color->redraw();
    browser_music->redraw();
    lyrics_pane->redraw();
}

void cb_settings_selection_color(Fl_Widget* widget, void*)
{
    Fl_Color val = edit_color(button_settings_selection_color->color());

    button_settings_selection_color->color(val);
    browser_music->color2(val);
    lyrics_pane->color2(val);
    input_search->color2(val);
    choice_search_type->color2(val);

    button_settings_selection_color->redraw();
    browser_music->redraw();
    lyrics_pane->redraw();
    input_search->redraw();
    choice_search_type->redraw();
}

void cb_settings_text_color(Fl_Widget* widget, void*)
{
    Fl_Color val = edit_color(button_settings_text_color->color());

    button_settings_text_color->color(val);
    browser_music->textcolor(val);
    lyrics_pane->textcolor(val);

    button_settings_text_color->redraw();
    browser_music->redraw();
    lyrics_pane->redraw();
}

void cb_settings_default_colors(Fl_Widget* widget, void*)
{
    browser_music->color(DEFAULT_BACKGROUND_COLOR);
    browser_music->color2(FL_SELECTION_COLOR);
    lyrics_pane->color(DEFAULT_BACKGROUND_COLOR);
    lyrics_pane->color2(FL_SELECTION_COLOR);
    input_search->color2(FL_SELECTION_COLOR);
    choice_search_type->color2(FL_SELECTION_COLOR);
    browser_music->textcolor(FL_FOREGROUND_COLOR);
    lyrics_pane->textcolor(FL_FOREGROUND_COLOR);

    button_settings_text_color->color(FL_FOREGROUND_COLOR);
    button_settings_background_color->color(DEFAULT_BACKGROUND_COLOR);
    button_settings_selection_color->color(FL_SELECTION_COLOR);

    browser_music->redraw();
    lyrics_pane->redraw();
    input_search->redraw();
    choice_search_type->redraw();
    button_settings_text_color->redraw();
    button_settings_background_color->redraw();
    button_settings_selection_color->redraw();
}
void cb_settings_lyrics(Fl_Widget* widget, void*)
{
    FLAG_LYRICS = !FLAG_LYRICS;
}

Fl_Color edit_color(Fl_Color val)
{
  uchar r,g,b;

  Fl::get_color(val,r,g,b);

  fl_color_chooser("Choose Color",r,g,b);

  return(fl_rgb_color(r,g,b));
}

void update_dir_list()
{
    browser_settings_directories->clear();
    listDir = getAllDirectories();

    for(int i = 0; i < listDir->size(); i++)
    {
        string s = listDir->at(i)->name;
        browser_settings_directories->add(s.c_str());
    }
}
