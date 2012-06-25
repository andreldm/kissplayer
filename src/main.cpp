#define KISS_MAJOR_VERSION 0
#define KISS_MINOR_VERSION 5
#define KISS_PATCH_VERSION 3

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include "sound.h"
#include "window_main.h"

#if defined WIN32
#include "win/windows_specific.h"
#else
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
int                 FLAG_SEARCH_TYPE;
float               INITIAL_VOLUME = 0.8;

int main(int argc, char **argv)
{
    startDB();

    window_main = make_window_main();

    set_app_icon(window_main);

    window_main->show(argc, argv);

    set_app_icon_transparency(window_main);

    sound = new Sound();
    sound->initialise();

    init_os_specific();

    int fl_result = Fl::run();

    if(fl_result == 0)
        save_config();

    end_os_specific();

    return fl_result;
}
