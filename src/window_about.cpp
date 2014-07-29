#include "window_about.h"

#include <sstream>

#include <FL/Fl_Help_View.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Menu_Item.H>

#include "util.h"
#include "locale.h"

static Fl_Window* window;

static void cb_close(Fl_Widget*, void*);

void window_about_show(Fl_Window* parent)
{
    std::stringstream title;
    Fl_Help_View* text;
    Fl_Group* group;

    int window_w = 380;
    int window_h = 250;
    int window_x = 0;
    int window_y = 0;

    // Place this window at the center of the parent window or screen
    if(parent) {
        window_x = parent->x() + (parent->w()/2)-(window_w/2);
        window_y = parent->y() + (parent->h()/2)-(window_h/2);
    } else {
        int screen_w = Fl::w();
        int screen_h = Fl::h();
        window_x = (screen_w/2)-(window_w/2);
        window_y = (screen_h/2)-(window_h/2);
    }

    window = new Fl_Window(window_x, window_y, window_w, window_h, _("About"));

    title << "KISS Player v" << KISS_MAJOR_VERSION << "." << KISS_MINOR_VERSION << "." << KISS_PATCH_VERSION;

    Fl_Box* box_title = new Fl_Box(5, 5, 240, 35);
    box_title->copy_label(title.str().c_str());
    box_title->labelfont(FL_BOLD);
    box_title->labelsize(22);
    box_title->labeltype(FL_SHADOW_LABEL);
    box_title->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

    Fl_Tabs* tabs = new Fl_Tabs(5, 40, window_w-10, window_h-80);
    tabs->clear_visible_focus();

    /** INFO **/
    group = new Fl_Group(5, 60, window_w-10, window_h-80, _("Info"));

    text = new Fl_Help_View(10, 65, window_w-20, 140);
    text->textsize(14);
    text->value("<b>A Simple and Lightweight Music Player</b><br><br>\
        Contact: <u>andreldm1989@gmail.com</u><br>\
        Website: <u>https://sourceforge.net/projects/kissplayer</u><br><br></p>");

    group->end();

    /** CREDITS **/
    group = new Fl_Group(5, 60, window_w-10, 210, _("Credits"));

    text = new Fl_Help_View(10, 65, window_w-20, 140);
    text->textsize(14);
    text->value("<b>Developers:</b><br>\
        André Miranda - Project Leader<br>\
        Maykon Silva - Developer<br>\
        <p></p>\
        <b>Contributors:</b><br>\
        Herman Polloni");

    group->end();

    /** LICENSE **/
    group = new Fl_Group(5, 60, window_w-10, 210, _("License"));

    text = new Fl_Help_View(10, 65, window_w-20, 140);
    text->textsize(14);
    text->value("This program is licensed under the terms<br>\
        of the GNU General Public License version 2<br>\
        Available online under:<br>\
        <u>http://gnu.org/licenses/gpl-2.0.html</u>");

    group->end();

    tabs->end();

    Fl_Button* button_close = new Fl_Button((window_w/2)-30, window_h-32, 60, 25, _("Close"));
    util_adjust_width(button_close, 15);
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
