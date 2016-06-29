#include "window_settings.h"

#include <deque>
#include <string>

#include <sigc++/sigc++.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Select_Browser.H>

#include "configuration.h"
#include "locale.h"
#include "signals.h"
#include "sync.h"
#include "util.h"
#include "window_main.h"
#include "widget/ksp_check_button.h"

using namespace std;

// static Fl_Window* window;
static Fl_Button* button_background_color;
static Fl_Button* button_selection_color;
static Fl_Button* button_text_color;
static KSP_Check_Button* button_lyrics;
static KSP_Check_Button* button_scroll_title;
static Fl_Select_Browser* browser_directories;
// static Fl_Choice* lang_choice;
static Fl_Hold_Browser* tab_selector;
static Fl_Input* input_proxy;
static Fl_Group* tabs[4] = { 0,0,0,0 };

static deque<COD_VALUE> listDir;
// static bool should_resync;

// PRIVATE SIGNALS
static sigc::signal<void> SignalClose;
static sigc::signal<void> SignalUpdateProxy;
static sigc::signal<void> SignalAddDir;
static sigc::signal<void> SignalRemoveDir;

Fl_Color edit_color(Fl_Color val)
{
  uchar r, g, b;
  Fl::get_color(val, r, g, b);

  fl_color_chooser(_("Choose Color"), r, g, b);

  return(fl_rgb_color(r, g, b));
}

WindowSettings::WindowSettings(Dao* dao)
        : Fl_Window(650, 300, _("Settings"))
{
    this->dao = dao;
    this->osSpecific = new OsSpecific();

    // should_resync = false;
    tab_selector = new Fl_Hold_Browser(10, 10, 200, h() - 20);
    tab_selector->color(Configuration::instance()->background());
    tab_selector->color2(Configuration::instance()->foreground());
    tab_selector->textcolor(Configuration::instance()->textcolor());
    tab_selector->add(_("General"));
    tab_selector->add(_("Directories"));
    tab_selector->add(_("Network"));
#ifdef WIN32
    tab_selector->add(_("Language"));
#endif

    // TAB GENERAL
    tabs[0] = new Fl_Group(215, 10, w() - 225, h() - 20, _("General"));
    tabs[0]->labelsize(16);
    tabs[0]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[0]->box(FL_UP_FRAME);
    tabs[0]->begin();

    button_lyrics = new KSP_Check_Button(220, 40, 120, 16,_("Display Lyrics"));
    button_lyrics->value(Configuration::instance()->shouldFetchLyrics());
    button_lyrics->callback([](Fl_Widget*, void*) {
        bool lyrics = Configuration::instance()->shouldFetchLyrics();
        Configuration::instance()->shouldFetchLyrics(!lyrics);
        button_lyrics->value(!lyrics);
    });

    button_scroll_title = new KSP_Check_Button(220, 65, 120, 16, _("Scroll Title"));
    button_scroll_title->value(Configuration::instance()->shouldScrollTitle());
    button_scroll_title->callback([](Fl_Widget*, void*) {
        bool scroll = Configuration::instance()->shouldScrollTitle();
        Configuration::instance()->shouldScrollTitle(!scroll);
        button_scroll_title->value(!scroll);
        if (scroll) SignalResetWindowTitle.emit();
    });

    button_background_color = new Fl_Button(220, 100, 16, 16, _("Background Color"));
    button_background_color->box(FL_DOWN_BOX);
    button_background_color->labelsize(12);
    button_background_color->clear_visible_focus();
    button_background_color->align(FL_ALIGN_RIGHT);
    button_background_color->color(Configuration::instance()->background());
    button_background_color->callback([](Fl_Widget*, void*) {
        Fl_Color c = edit_color(button_background_color->color());
        Configuration::instance()->background(c);
        SignalUpdateColors.emit();
    });

    button_selection_color = new Fl_Button(220, 125, 16, 16, _("Selection Color"));
    button_selection_color->box(FL_DOWN_BOX);
    button_selection_color->labelsize(12);
    button_selection_color->clear_visible_focus();
    button_selection_color->align(FL_ALIGN_RIGHT);
    button_selection_color->color(Configuration::instance()->foreground());
    button_selection_color->callback([](Fl_Widget*, void*) {
        Fl_Color c = edit_color(button_selection_color->color());
        Configuration::instance()->foreground(c);
        SignalUpdateColors.emit();
    });

    button_text_color = new Fl_Button(220, 150, 16, 16, _("Text Color"));
    button_text_color->box(FL_DOWN_BOX);
    button_text_color->labelsize(12);
    button_text_color->clear_visible_focus();
    button_text_color->align(FL_ALIGN_RIGHT);
    button_text_color->color(Configuration::instance()->textcolor());
    button_text_color->callback([](Fl_Widget*, void*) {
        Fl_Color c = edit_color(button_text_color->color());
        Configuration::instance()->textcolor(c);
        SignalUpdateColors.emit();
    });

    Fl_Button* button_default_colors = new Fl_Button(220, 175, 0, 22, _("Default Colors"));
    button_default_colors->labelsize(12);
    util_adjust_width(button_default_colors, 10);
    button_default_colors->clear_visible_focus();
    button_default_colors->callback([](Fl_Widget*, void*) {
        Configuration::instance()->background(DEFAULT_BACKGROUND_COLOR);
        Configuration::instance()->foreground(DEFAULT_FOREGROUND_COLOR);
        Configuration::instance()->textcolor(DEFAULT_SELECTION_COLOR);
        SignalUpdateColors.emit();
    });

    tabs[0]->end();

    // TAB DIRECTORIES
    tabs[1] = new Fl_Group(215, 10, w() - 225, h() - 20, _("Directories"));
    tabs[1]->labelsize(16);
    tabs[1]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[1]->box(FL_UP_FRAME);
    tabs[1]->begin();

    browser_directories = new Fl_Select_Browser(220, 40, w() - 235, 160, 0);
    browser_directories->type(FL_HOLD_BROWSER);
    browser_directories->color(Configuration::instance()->background());
    browser_directories->color2(Configuration::instance()->foreground());
    browser_directories->textcolor(Configuration::instance()->textcolor());

    Fl_Button* button_add = new Fl_Button(220, 205, 70, 25, _("Add"));
    util_adjust_width(button_add, 10);
    button_add->clear_visible_focus();
    button_add->callback([](Fl_Widget *w, void *u) { SignalAddDir.emit(); });

    Fl_Button* button_remove = new Fl_Button(220 + button_add->w() + 5, 205, 70, 25, _("Remove"));
    util_adjust_width(button_remove, 10);
    button_remove->clear_visible_focus();
    button_remove->callback([](Fl_Widget *w, void *u) { SignalRemoveDir.emit(); });

    tabs[1]->end();

    // TAB NETWORK
    tabs[2] = new Fl_Group(215, 10, w() - 225, h() - 20, _("Network"));
    tabs[2]->labelsize(16);
    tabs[2]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[2]->box(FL_UP_FRAME);
    tabs[2]->begin();

    Fl_Box* label_proxy = new Fl_Box(220, 40, w() - 235, 22, _("Proxy:"));
    util_adjust_width(label_proxy);

    input_proxy = new Fl_Input(215 + 8 + label_proxy->w(), 40, w() - 240 - label_proxy->w(), 22, 0);
    input_proxy->tooltip(_("Complete proxy URL, ex: http://192.168.1.1:3128"));
    input_proxy->value(dao->open_get_key("proxy").c_str());
    input_proxy->callback([](Fl_Widget*, void*) {
        SignalUpdateProxy.emit();
    });
    tabs[2]->end();

    // TAB LANGUAGE
    tabs[3] = new Fl_Group(215, 10, w() - 225, h() - 20, _("Language"));
#ifdef WIN32
    tabs[3]->labelsize(16);
    tabs[3]->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    tabs[3]->box(FL_UP_FRAME);
    tabs[3]->begin();

    lang_choice = new Fl_Choice(220, 40, 200, 25);
    lang_choice->clear_visible_focus();

    Language** languages = Locale::getDefinedLanguages();
    for (int i = 0; languages[i]; i++) {
        lang_choice->add(languages[i]->description.c_str());
    }
    lang_choice->callback(cb_change);

    dao->open_db();
    int index = util_s2i(dao->get_key("lang_index"));
    dao->close_db();
    lang_choice->value((index < 0 ? 0 : index));
#endif
    tabs[3]->end();

    // CLOSE BUTTON
    Fl_Button* button_close = new Fl_Button(0, h() - 40, 60, 25, _("Close"));
    util_adjust_width(button_close, 10);
    button_close->position(tabs[0]->x() + (tabs[0]->w() / 2) - (button_close->w() / 2), button_close->y());
    button_close->clear_visible_focus();
    button_close->callback([](Fl_Widget *w, void *u) { SignalClose.emit(); });

    updateDirList();

    tab_selector->select(1);
    tab_selector->callback([](Fl_Widget* w, void*) {
    	if (tab_selector->value() == 0) {
			return;
		}

		for (uint t = 0; t < sizeof(tabs) / sizeof(tabs[0]); t++) {
			if ((int) t == (tab_selector->value() - 1)) {
				tabs[t]->show();
			} else {
				tabs[t]->hide();
			}
		}
    });
    tab_selector->do_callback();

    set_modal();
    callback([](Fl_Widget *w, void *u) { SignalClose.emit(); });

    SignalClose.connect(sigc::mem_fun(this, &WindowSettings::close));
    SignalUpdateColors.connect(sigc::mem_fun(this, &WindowSettings::updateColors));
    SignalUpdateProxy.connect(sigc::mem_fun(this, &WindowSettings::updateProxy));
    SignalAddDir.connect(sigc::mem_fun(this, &WindowSettings::addDir));
    SignalRemoveDir.connect(sigc::mem_fun(this, &WindowSettings::removeDir));
}

void WindowSettings::show(Fl_Window* parent)
{
    util_center_window(this, parent);
    Fl_Window::show();
}

void WindowSettings::close()
{
    hide();
    /*if (should_resync) sync_execute(true);*/
}

void WindowSettings::addDir()
{
    char dir[PATH_LENGTH] = "";
    uint dirQty = listDir.size();

    osSpecific->dir_chooser(dir);
    Fl::redraw();

    if (strlen(dir) > 0) {
        // If the user didn't cancel
        string s(dir);
    	dao->insert_directory(s);
    	updateDirList();
    }

    if (listDir.size() != dirQty) {
        // If a directory was added
//        should_resync = true;
    }
}

void WindowSettings::removeDir()
{
    int index = browser_directories->value();
    if (index <= 0) return;

    string dir = browser_directories->text(index);
    for (auto d : listDir) {
        if (d.value == dir) {
            dao->delete_directory(d.cod);
            updateDirList();
            break;
        }
    }

//    should_resync = true;
}

void WindowSettings::updateProxy()
{
    dao->open_set_key("proxy", input_proxy->value());
}

void WindowSettings::updateColors()
{
    Fl_Color bg = Configuration::instance()->background();
    Fl_Color fg = Configuration::instance()->foreground();
    Fl_Color tx = Configuration::instance()->textcolor();

    button_background_color->color(bg);
    button_background_color->redraw();

    button_selection_color->color(fg);
    button_selection_color->redraw();

    button_text_color->color(tx);
    button_text_color->redraw();

    browser_directories->color(bg);
    browser_directories->color2(fg);
    browser_directories->textcolor(tx);
    browser_directories->redraw();

    tab_selector->color(bg);
    tab_selector->color2(fg);
    tab_selector->textcolor(tx);
    tab_selector->redraw();
}

void WindowSettings::updateDirList()
{
    browser_directories->clear();
    listDir.clear();
    dao->get_directories(listDir);

    for (auto s : listDir) {
        browser_directories->add(s.value.c_str());
    }
}

// void cb_change(Fl_Widget* widget, void*)
// {
//     /*Locale::setLanguage(lang_choice->value());*/
//
//     fl_beep();
//     fl_message(_("Please, restart the program to change the language."));
// }
