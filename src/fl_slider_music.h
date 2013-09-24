#ifndef fl_slider_music_h
#define fl_slider_music_h

#include <FL/Fl_Slider.H>

/**
* A modified version of the Fl_Slider that ignores the Left and Right key events.
*/

class Fl_Slider_Music : public Fl_Slider
{
private:
    float slider_size_;

    int handle(int event, int X, int Y, int W, int H)
    {
        // Fl_Widget_Tracker wp(this);
        switch (event) {
        case FL_PUSH:
        {
            Fl_Widget_Tracker wp(this);
            if (!Fl::event_inside(X, Y, W, H)) return 0;
            handle_push();
            if (wp.deleted()) return 1;
        }
        // fall through ...
        case FL_DRAG:
        {

            double val;
            if (minimum() == maximum()) {
                val = 0.5;
            } else {
                val = (value() - minimum()) / (maximum() - minimum());
                if (val > 1.0) val = 1.0;
                else if (val < 0.0) val = 0.0;
            }

            int ww = (horizontal() ? W : H);
            int mx = (horizontal() ? Fl::event_x()-X : Fl::event_y()-Y);
            int S;
            static int offcenter;

            if (type() == FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) {

                S = 0;
                if (event == FL_PUSH) {
                    int xx = int(val * ww + .5);
                    offcenter = mx - xx;
                    if (offcenter < -10 || offcenter > 10) offcenter = 0;
                    else return 1;
                }

            } else {
                S = int(slider_size_ * ww + .5);
                if (S >= ww) return 0;
                int T = (horizontal() ? H : W) / 2 + 1;
                if (type() == FL_VERT_NICE_SLIDER || type() == FL_HOR_NICE_SLIDER) T += 4;
                if (S < T) S = T;
                if (event == FL_PUSH) {
                    int xx = int(val * (ww - S) + .5);
                    offcenter = mx - xx;
                    if (offcenter < 0) offcenter = 0;
                    else if (offcenter > S) offcenter = S;
                    else return 1;
                }
            }

            int xx = mx-offcenter;
            double v;
            char tryAgain = 1;
            while (tryAgain) {
                tryAgain = 0;
                if (xx < 0) {
                    xx = 0;
                    offcenter = mx;
                    if (offcenter < 0) offcenter = 0;
                }
                else if (xx > (ww-S)) {
                    xx = ww-S;
                    offcenter = mx-xx;
                    if (offcenter > S) offcenter = S;
                }
                v = round(xx * (maximum() - minimum()) / (ww - S) + minimum());
                // make sure a click outside the sliderbar moves it:
                if (event == FL_PUSH && v == value()) {
                    offcenter = S / 2;
                    event = FL_DRAG;
                    tryAgain = 1;
                }
            }
            handle_drag(clamp(v));
        }
        return 1;
        case FL_RELEASE:
            handle_release();
            return 1;
        case FL_KEYBOARD:
        {
            Fl_Widget_Tracker wp(this);
            switch (Fl::event_key())
            {
            case FL_Up:
                if (horizontal()) return 0;
                handle_push();
                if (wp.deleted()) return 1;
                handle_drag(clamp(increment(value(), -1)));
                if (wp.deleted()) return 1;
                handle_release();
                return 1;
            case FL_Down:
                if (horizontal()) return 0;
                handle_push();
                if (wp.deleted()) return 1;
                handle_drag(clamp(increment(value(), 1)));
                if (wp.deleted()) return 1;
                handle_release();
                return 1;
            case FL_Left:
            case FL_Right:
                return 1;
            default:
                return 0;
            }
        }
        case FL_FOCUS :
        case FL_UNFOCUS :
            if (Fl::visible_focus()) {
                redraw();
                return 1;
            }
            return 0;
        case FL_ENTER :
        case FL_LEAVE :
            return 1;
        default:
            return 0;
        }
    }

public:
    int handle(int event)
    {
        if (event == FL_PUSH && Fl::visible_focus()) {
            Fl::focus(this);
            redraw();
        }

        return handle(event,
                      x()+Fl::box_dx(box()),
                      y()+Fl::box_dy(box()),
                      w()-Fl::box_dw(box()),
                      h()-Fl::box_dh(box()));
    }

    Fl_Slider_Music(int x,int y,int w,int h,const char* l = NULL)
        : Fl_Slider(x,y,w,h,l)
    {
        type(FL_HOR_NICE_SLIDER);
        box(FL_FLAT_BOX);
    }
};

#endif
