#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
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
    dao_start_db();

    Locale::init();

    window_main_init(argc, argv);
    Fl_Double_Window* window_main = window_main_get_instance();

    os_specific_set_app_icon();

    window_main->show(0, NULL);

    if(FLAG_MAXIMIZE_WINDOW) {
        os_specific_maximize_window();
    }

    // TODO: Check if returns 0, else "Keyboard hooker failed!";
    os_specific_init();

    // TODO: Check if returns 0, else "Could not initialize sound system!"
    sound_initialize();

    Fl::lock();

    int fl_result = Fl::run();

    os_specific_end();

    return fl_result;
}
