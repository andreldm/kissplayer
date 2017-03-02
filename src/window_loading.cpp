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

WindowLoading::WindowLoading(Context* context)
        : Fl_Window(350, 140, _("Please Wait"))
{
    this->context = context;
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
}

void WindowLoading::show(void)
{
    util_center_window(this);
    Fl_Window::show();
}

void WindowLoading::close (void)
{
    clear();
    hide();
}

void WindowLoading::set_dir_max(int m)
{
    if (progress_bar_dir) {
        progress_bar_dir->maximum(m);
    }
}

void WindowLoading::set_file_max(int m)
{
    if (progress_bar_file) {
        progress_bar_file->maximum(m);
    }
}

void WindowLoading::set_dir_value(int v)
{
    if (progress_bar_dir) {
        progress_bar_dir->value(v);
    }
}

void WindowLoading::set_file_value(int v)
{
    if (progress_bar_file) {
        progress_bar_file->value(v);
    }
}

void WindowLoading::cancel()
{
    context->configuration->isCancelSync(true);
}
