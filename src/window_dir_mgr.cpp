#include "window_dir_mgr.h"

/**
* A Window where the user manages the directories to be scaned.
*/

//LOCAL WIDGETS
Fl_Button *button_close;
Fl_Button *button_add;
Fl_Button *button_remove;
Fl_Window *dir_mgr_window;
Fl_Select_Browser *browser_directories;

//LOCAL VARIABLES
vector<NameCod *> *listDir;

//LOCAL FUNCTIONS
void update_list();

//LOCAL CALLBACKS
void cb_close(Fl_Widget*, void*);
void cb_add(Fl_Widget*, void*);
void cb_remove(Fl_Widget*, void*);

Fl_Window* make_window_dir_mgr()
{
    //To place the window at the center of the screen
    int window_w = 400;
    int window_h = 170;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    Fl_Window *window = new Fl_Window(window_x, window_y, window_w, window_h, "Directory Manager");
    window->callback((Fl_Callback*)cb_close);
    window->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

    browser_directories = new Fl_Select_Browser(5, 5, 390, 120, 0);
    browser_directories->color(43);
    browser_directories->type(FL_HOLD_BROWSER);

    button_close = new Fl_Button(335, 140, 60, 25, "Close");
    button_close->callback((Fl_Callback*)cb_close);

    button_add = new Fl_Button(5, 130, 40, 25, "Add");
    button_add->callback((Fl_Callback*)cb_add);

    button_remove = new Fl_Button(50, 130, 65, 25, "Remove");
    button_remove->callback((Fl_Callback*)cb_remove);

    update_list();

    window->set_modal();
    window->end();

    return window;
}

void cb_close(Fl_Widget* widget, void*)
{
    window_dir_mgr->clear();
    window_dir_mgr->hide();
    window_dir_mgr->~Fl_Window();
}

void cb_add(Fl_Widget* widget, void*)
{
    char * dir = fl_dir_chooser("Choose a directory", NULL);
    if(dir != NULL)//If the user didn't cancel;
    {
        insertDirectory(dir);
        update_list();
    }
}

void cb_remove(Fl_Widget* widget, void*)
{
    int index = browser_directories->value();
    if(index <= 0) return;

    string dir = browser_directories->text(index);
    for(int i = 0; i < listDir->size(); i++)
    {
        if(listDir->at(i)->name == dir)
        {
            deleteDirectory(listDir->at(i)->cod);
            update_list();
        }
    }
}

void update_list()
{
    browser_directories->clear();
    listDir = getAllDirectories();

    for(int i = 0; i < listDir->size(); i++)
    {
        string s = listDir->at(i)->name;
        browser_directories->add(s.c_str());
    }
}
