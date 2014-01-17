#include "window_loading.h"
#include "util.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>

static Fl_Window* window;
static Fl_Progress* progress_bar_dir;
static Fl_Progress* progress_bar_file;

static void cb_cancel(Fl_Widget*, void*);

void window_loading_show(void)
{
    // To place the window at the center of the screen
    int window_w = 350;
    int window_h = 140;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window = new Fl_Window(window_x, window_y, window_w, window_h, "Please Wait");
    window->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

    progress_bar_dir = new Fl_Progress(20, 20, 320, 30, "Scanning directories...");
    progress_bar_dir->selection_color(DEFAULT_FOREGROUND_COLOR);

    progress_bar_file = new Fl_Progress(20, 70, 320, 30, "Loading files...");
    progress_bar_file->selection_color(DEFAULT_FOREGROUND_COLOR);

    Fl_Button* button_cancel = new Fl_Button(280, 110, 60, 25, "Cancel");
    button_cancel->clear_visible_focus();
    button_cancel->callback(cb_cancel);

    window->set_modal();
    window->show();
}

void window_loading_close (void)
{
    if(window) {
        window->clear();
        window->hide();
        delete window;
    }
}

void window_loading_set_dir_max(int m)
{
    if(progress_bar_dir) {
        progress_bar_dir->maximum(m);
    }
}

void window_loading_set_file_max(int m)
{
    if(progress_bar_file) {
        progress_bar_file->maximum(m);
    }
}

void window_loading_set_dir_value(int v)
{
    if(progress_bar_dir) {
        progress_bar_dir->value(v);
    }
}

void window_loading_set_file_value(int v)
{
    if(progress_bar_file) {
        progress_bar_file->value(v);
    }
}

void cb_cancel(Fl_Widget*, void*)
{
    FLAG_CANCEL_SYNC = true;
}
