#include "window_settings.h"

/**
 * A Window where the user sets the app configuration.
 */

//LOCAL WIDGETS
Fl_Window *window_settings;
Fl_Button *button_settings_close;
Fl_Button *button_settings_add;
Fl_Button *button_settings_remove;
Fl_Select_Browser *browser_settings_directories;
Fl_Group *group_settings_directories;

//LOCAL VARIABLES
vector<NameCod *> *listDir;

//LOCAL FUNCTIONS
void update_dir_list();

//LOCAL CALLBACKS
void cb_settings_close(Fl_Widget*, void*);
void cb_settings_add(Fl_Widget*, void*);
void cb_settings_remove(Fl_Widget*, void*);

Fl_Window* make_window_settings()
{
    //To place the window at the center of the screen
    int window_w = 400;
    int window_h = 250;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    Fl_Window *window = new Fl_Window(window_x, window_y, window_w, window_h, "Settings");

    int group_offset = 25;
    group_settings_directories = new Fl_Group(5, group_offset, 390, 160, "Directories");
    group_settings_directories->box(FL_UP_FRAME);
    group_settings_directories->begin();

    browser_settings_directories = new Fl_Select_Browser(10, group_offset+5, 380, 120, 0);
    browser_settings_directories->color(43);
    browser_settings_directories->type(FL_HOLD_BROWSER);

    button_settings_add = new Fl_Button(10, group_offset+130, 40, 25, "Add");
    button_settings_add->callback((Fl_Callback*)cb_settings_add);

    button_settings_remove = new Fl_Button(55, group_offset+130, 65, 25, "Remove");
    button_settings_remove->callback((Fl_Callback*)cb_settings_remove);

    group_settings_directories->end();

    button_settings_close = new Fl_Button(335, window_h-30, 60, 25, "Close");
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

void cb_settings_add(Fl_Widget* widget, void*)
{
    char * dir = fl_dir_chooser("Choose a directory", NULL);
    if(dir != NULL)//If the user didn't cancel;
    {
        insertDirectory(dir);
        update_dir_list();
    }
}

void cb_settings_remove(Fl_Widget* widget, void*)
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
