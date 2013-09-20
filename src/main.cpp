#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include "sound.h"
#include "window_main.h"
#include "os_specific.h"

// GLOBAL
Fl_Double_Window*   window_main;
bool				shouldMaximizeWindow;
bool                FLAG_CANCEL_SYNC;
bool                FLAG_LIST_CHANGED;
bool                FLAG_RANDOM;
bool                FLAG_LYRICS;
int                 FLAG_SEARCH_TYPE;

int main(int argc, char **argv)
{
    dao_start_db();

    window_main = make_window_main(argc, argv);

    set_app_icon(window_main);

    window_main->show(0, NULL);

	if(shouldMaximizeWindow) {
        maximizeWindow(window_main);
	}

    init_os_specific(window_main);

    // TODO: check if return is non-zero
    sound_initialize();

    int fl_result = Fl::run();

    end_os_specific();

    return fl_result;
}
