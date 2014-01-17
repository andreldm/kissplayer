#include "ksp_check_button.h"

#include <FL/Fl.H>
#include <FL/fl_draw.H>

KSP_Check_Button::KSP_Check_Button (int X, int Y, int W, int H, const char* L)
    : Fl_Light_Button(X, Y, W, H, L)
    {
        box(FL_NO_BOX);
        down_box(FL_DOWN_BOX);
    }

void KSP_Check_Button::draw()
{
    int W  = labelsize();
    int dx = Fl::box_dx(box()) + 2;
    int dy = (h() - W) / 2;

    draw_box(down_box(), x() + dx, y() + dy, W, W, FL_BACKGROUND2_COLOR);
    if(value()) {
        fl_color(FL_WHITE);

        int tx = x() + dx + 3;
        int tw = W - 6;
        int d1 = tw / 3;
        int d2 = tw - d1;
        int ty = y() + dy + (W + d2) / 2 - d1 - 2;
        for(int n = 0; n < 3; n++, ty++) {
            fl_line(tx, ty, tx + d1, ty + d1);
            fl_line(tx + d1, ty + d1, tx + tw - 1, ty + d1 - d2 + 1);
        }
    }

    draw_label(x() + W + 2 * dx, y(), w() - W - 2  * dx, h());
}
