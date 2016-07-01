#include "ksp_slider.h"

#include <string>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Window.H>

#include "../util.h"

using namespace std;

/**
 * Floating Tooltip Window
 * Credits: http://seriss.com/people/erco/fltk/#SliderTooltip
 */
class TipWin : public Fl_Menu_Window
{
public:
    TipWin(WindowMain* window_main, Context* context);
    void draw();
    void update(float f, int parent_y);

private:
    WindowMain* window_main;
    Context* context;
    std::string tip;
};

/**
 * A box to display the song's current time.
 * Ignores resizing its width and height.
 */
class TimeDisplay : public Fl_Box
{
public:
    TimeDisplay(Fl_Boxtype b, int X, int Y, int W, int H, const char *l=0);
    void resize(int x, int y, int ww, int hh);
};

TipWin::TipWin(WindowMain* window_main, Context* context) : Fl_Menu_Window(1,1)
{
    this->window_main = window_main;
    this->context = context;
    clear_visible_focus();
    set_override();
    end();
}

void TipWin::draw()
{
    draw_box(FL_BORDER_BOX, 0, 0, w(), h(), context->configuration->background());
    fl_color(context->configuration->textcolor());
    fl_draw(tip.c_str(), tip.length(), 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP));
}

void TipWin::update(float f, int parent_y)
{
    int xx = Fl::event_x_root();
    int yy = window_main->y() + parent_y - 17;

    if (xx < window_main->x() + 14) {
        xx = window_main->x() + 14;
    } else if (xx > window_main->x() + window_main->w() - 14) {
        xx = window_main->x() + window_main->w() - 14;
    }

    tip = util_format_time((int)f);
    // Recalc size of window
    fl_font(labelfont(), labelsize());
    int W = w(), H = h();
    fl_measure(tip.c_str(), W, H, 0);
    W += 8;
    size(W, H);
    position(xx - (w()/2), yy);
    redraw();
}

TimeDisplay::TimeDisplay(Fl_Boxtype b, int X, int Y, int W, int H, const char *l)
    : Fl_Box(b, X, Y, W, H, l)
{
    // Nothing
}

void TimeDisplay::resize(int x, int y, int ww, int hh)
{
    Fl_Box::resize(x, y, w(), h());
}

double KSP_Slider::calc_value(int event, int X, int Y, int W, int H)
{
    int mx = Fl::event_x() - X;
    int T = H / 2;
    if (event == FL_PUSH) {
        double val = 0.5;
        if (minimum() != maximum()) {
            val = (value() - minimum()) / (maximum() - minimum());
            if (val > 1.0) val = 1.0;
            else if (val < 0.0) val = 0.0;
        }
        // int xx = int(val * (W - T) + .5);
    }

    int xx = mx - 7;

    if (xx < 0) {
        xx = 0;
    } else if (xx > (W - T)) {
        xx = W - T;
    }
    double v = round(xx * (maximum() - minimum()) / (W - T) + minimum());

    return clamp(v);
}

int KSP_Slider::handle(int event, int X, int Y, int W, int H)
{
    double v;
    // int yy;

    switch (event) {
    case FL_PUSH:
    {
        Fl_Widget_Tracker wp(this);
        if (!Fl::event_inside(X, Y, W, H)) {
            return 0;
        }
        handle_push();
        if (wp.deleted()) return 1;
    }
    case FL_DRAG:
        v = calc_value(event, X, Y, W, H);
        tipwin->update(v, y());
        handle_drag(v);
        return 1;
    case FL_RELEASE:
        handle_release();
        return 1;
    case FL_KEYBOARD:
        return 1;
    case FL_FOCUS:
    case FL_UNFOCUS:
        Fl::focus(parent()->parent());
        return 0;
    case FL_ENTER:
        // Don't show the tipwin if the slider is "reseted"
        if (minimum() + maximum() <= 1) {
            return 0;
        }
        tipwin->show();
        return 1;
    case FL_LEAVE:
        tipwin->hide();
        return 1;
    case FL_MOVE:
        v = calc_value(event, X, Y, W, H);
        tipwin->update(v, y());
        return 0;
    default:
        return 0;
    }
}

KSP_Slider::KSP_Slider(WindowMain* window_main, Context* context, int x, int y, int w, int h, const char* l)
    : Fl_Slider(x, y, w, h, l)
{
    type(FL_HOR_NICE_SLIDER);
    box(FL_FLAT_BOX);
    color2(0xDDDDDD00);
    clear_visible_focus();
    align(FL_ALIGN_BOTTOM_RIGHT);

    time_display = new TimeDisplay(FL_FLAT_BOX, x - 3, y + h, 50, 15, "00:00");
    time_display->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    reset();

    Fl_Group* save = Fl_Group::current();
    tipwin = new TipWin(window_main, context);
    tipwin->hide();
    Fl_Group::current(save);
}

KSP_Slider::~KSP_Slider()
{
    tipwin->hide();
}

void KSP_Slider::update_time()
{
    string time = util_format_time(value());
    time_display->copy_label(time.c_str());
    redraw();
}

void KSP_Slider::realise_new_sound(int length)
{
    string time = util_format_time(length);
    copy_label(time.c_str());
    time_display->label("00:00");
    maximum(length);
    value(0);
}

void KSP_Slider::reset()
{
    copy_label("00:00");
    time_display->label("00:00");
    value(0);
    minimum(0);
    maximum(1);
}

int KSP_Slider::handle(int event)
{
    if (event == FL_PUSH && Fl::visible_focus()) {
        Fl::focus(this);
        redraw();
    }

    return handle(event,
                  x() + Fl::box_dx(box()),
                  y() + Fl::box_dy(box()),
                  w() - Fl::box_dw(box()),
                  h() - Fl::box_dh(box()));
}

bool KSP_Slider::is_tipwin_current()
{
    return (tipwin == Fl_Window::current());
}
