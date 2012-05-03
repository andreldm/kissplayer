#define KISS_MAJOR_VERSION 0
#define KISS_MINOR_VERSION 5
#define KISS_PATCH_VERSION 2

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include "sound.h"
#include "window_main.h"

#if defined WIN32
#define APP_ICON 0
#else
#include <X11/xpm.h>
#include "icon.xpm"
#endif

/**
 * GLOBALS
 */
Sound*   sound;
bool     FLAG_CANCEL_SYNC;
bool     FLAG_LIST_CHANGED;
bool     FLAG_RANDOM;
int      FLAG_SEARCH_TYPE;
float    INITIAL_VOLUME = 0.8; //between 0 and 1

int main(int argc, char **argv)
{
    startDB();

    Fl_Double_Window *window_main = make_window_main();

    /**
     * SETS THE ICON FOR WINDOWS OR LINUX
     */
#if defined WIN32
    window_main->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(APP_ICON)));
#else
    Pixmap p, mask;
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), icon_xpm, &p, &mask, NULL);
    window_main->icon((char *)p);
#endif

    window_main->show(argc, argv);

    //To archieve icon transparency on Linux, we need this operation.
    //Source: www.fltk.org/newsgroups.php?gfltk.general+v:14448
#if not defined WIN32
    XWMHints* hints = XGetWMHints(fl_display, fl_xid(window_main));
    hints->flags |= IconMaskHint;
    hints->icon_mask = mask;
    XSetWMHints(fl_display, fl_xid(window_main), hints);
    XFree(hints);
#endif

    sound = new Sound();
    sound->initialise();
    return Fl::run();
}
