#include "ksp_volume_controller.h"

#include <FL/Fl.H>
#include <FL/Fl_draw.H>

KSP_Volume_Controller::KSP_Volume_Controller(int xx, int yy, int ww, int hh)
    : Fl_Valuator(xx, yy, ww, hh, 0)
{
    minimum(0);
    maximum(10);
    box(FL_FLAT_BOX);
}

float KSP_Volume_Controller::value2()
{
    return value() / 10;
}

void KSP_Volume_Controller::increase(int v)
{
    value(clamp(value() + v));
}

void KSP_Volume_Controller::decrease(int v)
{
    value(clamp(value() - v));
}

void KSP_Volume_Controller::draw()
{
    draw_box();

    int xx = x();
    int yy = y();
    int ww = w();
    int hh = h();
    int barw = (int) (ww / maximum());
    int barh = (int) (hh / maximum());

    fl_color(FL_WHITE);
    for(int i = 1; i <= value(); i++) {
        fl_rectf(xx + (barw * (i - 1)), yy + hh - (barh * i), barw, (barh * i));
    }

    fl_color(fl_darker(FL_WHITE));
    for(int i = 1; i <= maximum(); i++) {
        fl_rect(xx + (barw * (i - 1)), yy + hh - (barh * i), barw, (barh * i));
    }
}

int KSP_Volume_Controller::handle(int event)
{
    switch (event) {
    case FL_PUSH: {
        Fl_Widget_Tracker wp(this);
        handle_push();
        if (wp.deleted()) return 1;
    }
    case FL_DRAG: {
        int mx = Fl::event_x() - x();
        if (mx < 0 || mx > w()) return 1;

        int v = (int) (mx * maximum()) / w(); // Just a cross multiplication
        handle_drag(clamp(v));
        return 1;
    }
    case FL_RELEASE:
        handle_release();
        return 1;
    case FL_ENTER :
    case FL_LEAVE :
        return 1;
    default:
        return 0;
    }
    return 0;
}
