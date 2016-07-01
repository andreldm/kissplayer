#ifndef ksp_slider_h
#define ksp_slider_h

#include <FL/Fl_Slider.H>

#include "../context.h"
#include "../window_main.h"

class TipWin;
class TimeDisplay;
class WindowMain;

/**
 * A modified version of the Fl_Slider that features:
 * - Ignores the Left and Right key events.
 * - A Tip Window that displays the song's position while the mouse is hovering.
 */
class KSP_Slider : public Fl_Slider
{
public:
    KSP_Slider(WindowMain* window_main, Context* context, int x, int y, int w, int h, const char* l = 0);
    ~KSP_Slider();
    void update_time();
    void realise_new_sound(int length);
    void reset();
    int handle(int event);
    bool is_tipwin_current();

private:
    TipWin* tipwin;
    TimeDisplay* time_display;

    double calc_value(int event, int X, int Y, int W, int H);
    int handle(int event, int X, int Y, int W, int H);
};

#endif
