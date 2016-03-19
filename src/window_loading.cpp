#include "window_loading.h"

#include <sigc++/sigc++.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>

#include "constants.h"
#include "locale.h"
#include "util.h"
#include "signals.h"

static Fl_Progress* progress_bar_dir;
static Fl_Progress* progress_bar_file;

static void cb_cancel(Fl_Widget*, void*);

WindowLoading::WindowLoading(Configuration* config)
        : Fl_Window(0, 0, _("Please Wait"))
{
    this->config = config;
}

void WindowLoading::show(void)
{
    // To place the window at the center of the screen
    int window_w = 350;
    int window_h = 140;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    resize(window_x, window_y, window_w, window_h);
    this->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

    progress_bar_dir = new Fl_Progress(20, 20, 320, 30, _("Scanning directories..."));
    progress_bar_dir->selection_color(DEFAULT_FOREGROUND_COLOR);

    progress_bar_file = new Fl_Progress(20, 70, 320, 30, _("Loading files..."));
    progress_bar_file->selection_color(DEFAULT_FOREGROUND_COLOR);

    Fl_Button* button_cancel = new Fl_Button(280, 110, 70, 25, _("Cancel"));
    util_adjust_width(button_cancel, 10);

    button_cancel->clear_visible_focus();
    button_cancel->callback([](Fl_Widget *w, void *u) {
        SignalCancelSync.emit();
    });

    SignalCancelSync.connect(sigc::mem_fun(this, &WindowLoading::cancel));

    set_modal();
    show();
}

void WindowLoading::close (void)
{
    // if(window) {
        clear();
        hide();
        // delete window;
    // }
}

void WindowLoading::set_dir_max(int m)
{
    if(progress_bar_dir) {
        progress_bar_dir->maximum(m);
    }
}

void WindowLoading::set_file_max(int m)
{
    if(progress_bar_file) {
        progress_bar_file->maximum(m);
    }
}

void WindowLoading::set_dir_value(int v)
{
    if(progress_bar_dir) {
        progress_bar_dir->value(v);
    }
}

void WindowLoading::set_file_value(int v)
{
    if(progress_bar_file) {
        progress_bar_file->value(v);
    }
}

void WindowLoading::cancel()
{
    config->isCancelSync(true);
}
