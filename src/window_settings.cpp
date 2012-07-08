#include "window_settings.h"

/**
 * A Window where the user sets the app configuration.
 */

//LOCAL WIDGETS
Fl_Window *window_settings;


Fl_Window* make_window_settings()
{
    Fl_Button *button_close;
Fl_Button *button_add;
Fl_Button *button_remove;
Fl_Select_Browser *browser_directories;

    //To place the window at the center of the screen
    int window_w = 400;
    int window_h = 170;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    Fl_Window *window = new Fl_Window(window_x, window_y, window_w, window_h, "Settings");
    window->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

    browser_directories = new Fl_Select_Browser(5, 5, 390, 120, 0);
    browser_directories->color(43);
    browser_directories->type(FL_HOLD_BROWSER);

    button_close = new Fl_Button(335, 140, 60, 25, "Close");

    button_add = new Fl_Button(5, 130, 40, 25, "Add");

    button_remove = new Fl_Button(50, 130, 65, 25, "Remove");


    window->set_modal();
    window->end();

    return window;
}
