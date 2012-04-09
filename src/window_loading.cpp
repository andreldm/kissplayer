#include "window_loading.h"

/**
* A Window that displays the directories scanning progress.
*/

//LOCAL WIDGETS
Fl_Progress *progress_bar_dir;
Fl_Progress *progress_bar_file;
Fl_Button *button_cancel;

//LOCAL CALLBACKS
void cb_cancel_loading(Fl_Widget*, void*);

Fl_Window* make_window_loading()
{
    //To place the window at the center of the screen
    int window_w = 350;
    int window_h = 140;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    Fl_Window *window = new Fl_Window(window_x, window_y, window_w, window_h, "Please Wait");
    window->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

    progress_bar_dir = new Fl_Progress(20, 20, 320, 30, "Scanning directories...");
    progress_bar_dir->selection_color(FL_BLUE);

    progress_bar_file = new Fl_Progress(20, 70, 320, 30, "Loading files...");
    progress_bar_file->selection_color(FL_BLUE);

    button_cancel = new Fl_Button(280, 110, 60, 25, "Cancel");
    button_cancel->callback(cb_cancel_loading);

    window->set_modal();
    window->end();

    return window;
}

void cb_cancel_loading(Fl_Widget*, void*)
{
    FLAG_CANCEL_SYNC = true;
}
