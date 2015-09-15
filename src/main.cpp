#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include "sound.h"
#include "locale.h"
#include "window_main.h"
#include "os_specific.h"
#include "dao.h"

// GLOBAL
bool    FLAG_MAXIMIZE_WINDOW;
bool    FLAG_CANCEL_SYNC;
bool    FLAG_LIST_CHANGED;
bool    FLAG_RANDOM;
bool    FLAG_REPEAT;
bool    FLAG_LYRICS;
bool    FLAG_SCROLL_TITLE;
int     FLAG_SEARCH_TYPE;

int main(int argc, char** argv)
{
    Fl::scheme("GTK+");

    Fl::background(50, 50, 50);
    Fl::background2(90, 90, 90);
    Fl::foreground(255, 255, 255);

    fl_message_title_default(_("Warning"));

    if(dao_start_db() != 0) {
        fl_alert(_("Error while initializing database"));
        return -1;
    }

    Locale::init();

    window_main_init(argc, argv);
    Fl_Double_Window* window_main = window_main_get_instance();

    os_specific_set_app_icon();

    window_main->show(0, NULL);

    if(FLAG_MAXIMIZE_WINDOW) {
        os_specific_maximize_window();
    }

    if(os_specific_init() != 0) {
        fl_alert(_("Error while registering keyboard hook!"));
        return -1;
    }

    if(sound_initialize() != 0) {
        fl_alert(_("Error while initializing sound system!"));
        return -1;
    }

    Fl::lock();

    int fl_result = Fl::run();

    os_specific_end();

    return fl_result;
}
