#include "window_about.h"

/**
 * The About Window :D
 */

//LOCAL WIDGETS
Fl_Window *window_about;
Fl_Button *button_about_close;
Fl_Help_View *browser_about_info;
Fl_Box *box_about_title;

//LOCAL CALLBACKS
void cb_about_close(Fl_Widget*, void*);

Fl_Window* make_window_about()
{
    //To place the window at the center of the screen
    int window_w = 380;
    int window_h = 265;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    Fl_Window *window = new Fl_Window(window_x, window_y, window_w, window_h, "About");

    stringstream title;
    title << "KISS Player v" << KISS_MAJOR_VERSION << "." << KISS_MINOR_VERSION << "." << KISS_PATCH_VERSION;

    box_about_title = new Fl_Box(5, 5, 240, 35);
    box_about_title->copy_label(title.str().c_str());
    box_about_title->labelfont(FL_BOLD);
    box_about_title->labelsize(22);
    box_about_title->labeltype(FL_SHADOW_LABEL);
    box_about_title->labelcolor(0xDDEEFF00);
    box_about_title->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

    browser_about_info = new Fl_Help_View (5, 40, window_w-10, 185);
    browser_about_info->textsize(14);
    browser_about_info->value("<b>A Simple and Lightweight Music Player</b><br><br>\
        <p>Author: André Miranda<br>\
        Contact: <u>andreldm1989@gmail.com</u><br>\
        Website: <u>https://sourceforge.net/projects/kissplayer</u><br><br></p>\
        <p>This program is licensed under the terms<br>\
        of the GNU General Public License version 2<br>\
        Available online under:<br>\
        <u>http://gnu.org/licenses/gpl-2.0.html</u></p>");

    button_about_close = new Fl_Button(window_w-65, window_h-30, 60, 25, "Close");
    button_about_close->callback((Fl_Callback*)cb_about_close);

    window->set_modal();
    window->end();

    return window;
}

void cb_about_close(Fl_Widget* widget, void*)
{
    window_about->clear();
    window_about->hide();
    window_about->~Fl_Window();
}
