#include "window_about.h"

#include <sstream>

#include <sigc++/sigc++.h>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Menu_Item.H>

#include "util.h"
#include "locale.h"

static sigc::signal<void> SignalClose;

WindowAbout::WindowAbout()
        : Fl_Window(380, 250, _("About"))
{
    std::stringstream sstream;
    Fl_Help_View* text;
    Fl_Group* group;

    sstream << "KISS Player v" << VERSION;

    Fl_Box* box_title = new Fl_Box(5, 5, 240, 35);
    box_title->copy_label(sstream.str().c_str());
    box_title->labelfont(FL_BOLD);
    box_title->labelsize(22);
    box_title->labeltype(FL_SHADOW_LABEL);
    box_title->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

    Fl_Tabs* tabs = new Fl_Tabs(5, 40, w() - 10, h() - 80);
    tabs->clear_visible_focus();

    /** INFO **/
    group = new Fl_Group(5, 60, w() - 10, h() - 80, _("Info"));

    sstream.str(std::string());
    sstream.clear();
    sstream << "<b>" << _("A Simple and Lightweight Music Player") << "</b><br>";
    sstream << "<p></p>";
    sstream << _("Contact:") << " <u>" << PACKAGE_BUGREPORT << "</u><br>";
    sstream << _("Website:") << " <u>" << PACKAGE_URL << "</u>";

    text = new Fl_Help_View(10, 65, w() - 20, 140);
    text->textsize(14);
    text->value(sstream.str().c_str());

    group->end();

    /** CREDITS **/
    group = new Fl_Group(5, 60, w() - 10, 210, _("Credits"));

    sstream.str(std::string());
    sstream.clear();
    sstream << "<b>" << _("Developers:") << "</b><br>";
    sstream << "André Miranda - " << _("Project Leader") << "<br>";
    sstream << "Maykon Silva - " << _("Developer") << "<br>";
    sstream << "<p></p>";
    sstream << "<b>" << _("Collaborators:") << "</b><br>";
    sstream << "Carlos Sanchez<br>";
    sstream << "Herman Polloni<br>";
    sstream << "Nuno Jesus (New Logo)";

    std::string translators_credits = _("<translators_credits>");
    if (translators_credits.compare("<translators_credits>") != 0) {
        sstream << "<br><p></p><b>" << _("Translators:") << "</b><br>";
        sstream << translators_credits;
    }

    text = new Fl_Help_View(10, 65, w() - 20, 140);
    text->textsize(14);
    text->value(sstream.str().c_str());

    group->end();

    /** LICENSE **/
    group = new Fl_Group(5, 60, w() - 10, 210, _("License"));

    sstream.str(std::string());
    sstream.clear();
    sstream << _("This program is licensed under the terms of the GNU General Public License version 2.") << "<br>";
    sstream << _("Available online under:") << "<br>";
    sstream << "<u>http://gnu.org/licenses/gpl-2.0.html</u>";

    text = new Fl_Help_View(10, 65, w() - 20, 140);
    text->textsize(14);
    text->value(sstream.str().c_str());

    group->end();
    tabs->end();

    Fl_Button* button_close = new Fl_Button((w() / 2) - 30, h() - 32, 60, 25, _("Close"));
    util_adjust_width(button_close, 15);
    button_close->callback([](Fl_Widget *w, void *u) {
        SignalClose.emit();
    });

    SignalClose.connect(sigc::mem_fun(this, &WindowAbout::close));

    set_modal();
}

void WindowAbout::show(Fl_Window* parent)
{
    util_center_window(this, parent);
    Fl_Window::show();
}

void WindowAbout::close()
{
    hide();
}
