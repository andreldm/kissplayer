#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include "sound.h"
#include "window_main.h"

#ifdef WIN32
#include "win/windows_specific.h"
#endif

#ifdef __linux__
#include "linux/linux_specific.h"
#endif

/**
 * GLOBALS
 */
Sound*              sound;
Fl_Double_Window*   window_main;
bool                FLAG_CANCEL_SYNC;
bool                FLAG_LIST_CHANGED;
bool                FLAG_RANDOM;
bool                FLAG_LYRICS;
int                 FLAG_SEARCH_TYPE;

int main(int argc, char **argv)
{
    startDB();

    window_main = make_window_main(argc, argv);

    set_app_icon(window_main);

    window_main->show(0, NULL);

    init_os_specific(window_main);

    sound = new Sound();
    sound->initialise();

    int fl_result = Fl::run();

    end_os_specific();

    return fl_result;
}
