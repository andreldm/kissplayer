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
    std::stringstream sstream;
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

    sstream << "KISS Player v" << VERSION;

    Fl_Box* box_title = new Fl_Box(5, 5, 240, 35);
    box_title->copy_label(sstream.str().c_str());
    box_title->labelfont(FL_BOLD);
    box_title->labelsize(22);
    box_title->labeltype(FL_SHADOW_LABEL);
    box_title->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

    Fl_Tabs* tabs = new Fl_Tabs(5, 40, window_w-10, window_h-80);
    tabs->clear_visible_focus();

    /** INFO **/
    group = new Fl_Group(5, 60, window_w-10, window_h-80, _("Info"));

    sstream.str(std::string());
    sstream.clear();
    sstream << "<b>" << _("A Simple and Lightweight Music Player") << "</b><br>";
    sstream << "<p></p>";
    sstream << _("Contact:") << " <u>" << PACKAGE_BUGREPORT << "</u><br>";
    sstream << _("Website:") << " <u>" << PACKAGE_URL << "</u>";

    text = new Fl_Help_View(10, 65, window_w-20, 140);
    text->textsize(14);
    text->value(sstream.str().c_str());

    group->end();

    /** CREDITS **/
    group = new Fl_Group(5, 60, window_w-10, 210, _("Credits"));

    sstream.str(std::string());
    sstream.clear();
    sstream << "<b>" << _("Developers:") << "</b><br>";
    sstream << "André Miranda - " << _("Project Leader") << "<br>";
    sstream << "Maykon Silva - " << _("Developer") << "<br>";
    sstream << "<p></p>";
    sstream << "<b>" << _("Contributors:") << "</b><br>";
    sstream << "Carlos Sanchez<br>";
    sstream << "Herman Polloni<br>";

    text = new Fl_Help_View(10, 65, window_w-20, 140);
    text->textsize(14);
    text->value(sstream.str().c_str());

    group->end();

    /** LICENSE **/
    group = new Fl_Group(5, 60, window_w-10, 210, _("License"));

    sstream.str(std::string());
    sstream.clear();
    sstream << _("This program is licensed under the terms of the GNU General Public License version 2.") << "<br>";
    sstream << _("Available online under:") << "<br>";
    sstream << "<u>http://gnu.org/licenses/gpl-2.0.html</u>";

    text = new Fl_Help_View(10, 65, window_w-20, 140);
    text->textsize(14);
    text->value(sstream.str().c_str());

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
