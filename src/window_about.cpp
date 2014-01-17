#include "window_about.h"

#include <sstream>

#include <FL/Fl_Window.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

#include "util.h"

using namespace std;

static Fl_Window* window;

static void cb_close(Fl_Widget*, void*);

void window_about_show(void)
{
    // To place the window at the center of the screen
    int window_w = 380;
    int window_h = 265;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window = new Fl_Window(window_x, window_y, window_w, window_h, "About");

    stringstream title;
    title << "KISS Player v" << KISS_MAJOR_VERSION << "." << KISS_MINOR_VERSION << "." << KISS_PATCH_VERSION;

    Fl_Box* box_title = new Fl_Box(5, 5, 240, 35);
    box_title->copy_label(title.str().c_str());
    box_title->labelfont(FL_BOLD);
    box_title->labelsize(22);
    box_title->labeltype(FL_SHADOW_LABEL);
    box_title->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

    Fl_Help_View* browser_info = new Fl_Help_View(5, 40, window_w-10, 185);
    browser_info->textsize(14);
    browser_info->value("<b>A Simple and Lightweight Music Player</b><br><br>\
        <p>Author: André Miranda<br>\
        Contact: <u>andreldm1989@gmail.com</u><br>\
        Website: <u>https://sourceforge.net/projects/kissplayer</u><br><br></p>\
        <p>This program is licensed under the terms<br>\
        of the GNU General Public License version 2<br>\
        Available online under:<br>\
        <u>http://gnu.org/licenses/gpl-2.0.html</u></p>");

    Fl_Button* button_close = new Fl_Button((window_w/2)-30, window_h-32, 60, 25, "Close");
    button_close->callback((Fl_Callback*)cb_close);

    window->set_modal();
    window->show();
}

void cb_close(Fl_Widget* widget, void*)
{
    window->clear();
    window->hide();
    delete window;
}
