#include "ksp_browser.h"

#include <Fl/fl.H>
#include <Fl/fl_draw.H>

#define SELECTED 1

using namespace std;

struct FL_BLINE {
    FL_BLINE* prev;
    FL_BLINE* next;
    void* data;
    Fl_Image* icon;
    short length;
    char flags;
    char txt[1];
};

KSP_Browser::KSP_Browser(int x, int y, int w, int h)
    : Fl_Select_Browser(x, y, w, h, NULL)
{
    type(FL_HOLD_BROWSER);
    box(FL_DOWN_BOX);
}

int KSP_Browser::handle(int e)
{
    switch(e) {
    case FL_DND_ENTER:
    case FL_DND_RELEASE:
    case FL_DND_LEAVE:
    case FL_DND_DRAG:
        return 1;

    case FL_PASTE:
        dnd_evt = true;

        // make a copy of the DND payload
        evt_txt.clear();
        evt_txt += Fl::event_text();

        if(callback() && ((when() & FL_WHEN_RELEASE) || (when() & FL_WHEN_CHANGED))) {
            Fl::add_timeout(0.0, callback_deferred, (void*)this);
        }
        return 1;
    case FL_KEYBOARD:
        // ignore left and right keys
        int key = Fl::event_original_key();
        if(key == FL_Right || key == FL_Left) {
            return 1;
        }
        break;
    }

    return Fl_Select_Browser::handle(e);
}

void KSP_Browser::set_highlighted(int line)
{
    FL_BLINE* item = find_line(line);
    if(item == NULL) return;

    highlighted_line = item;
}

void KSP_Browser::clear_highlighted()
{
    highlighted_line = NULL;
}

/**
What changes is that the highlighted line has a bold font.
Also some code not used by the player was removed.
*/
void KSP_Browser::item_draw(void* item, int X, int Y, int W, int H) const
{
    FL_BLINE* l = (FL_BLINE*) item;
    char* str = l->txt;
    const int* i = column_widths();

    while (W > 6) {
        int w1 = W;
        char* e = 0;
        if (*i) {
            e = strchr(str, column_char());
            if (e) {
                *e = 0;
                w1 = *i++;
            }
        }

        int tsize = textsize();
        Fl_Font font = textfont();
        Fl_Color lcol = textcolor();
        Fl_Align talign = FL_ALIGN_LEFT;

        if (l == highlighted_line) {
            font = (Fl_Font)(font | FL_BOLD);
        }
        if (l->flags & SELECTED) {
            lcol = fl_contrast(lcol, selection_color());
        }
        if (!active_r()) {
            lcol = fl_inactive(lcol);
        }

        fl_font(font, tsize);
        fl_color(lcol);
        fl_draw(str, X + 3, Y, w1 - 6, H, e ? Fl_Align(talign|FL_ALIGN_CLIP) : talign, 0, 0);
        if (!e) break;
        *e = column_char();
        X += w1;
        W -= w1;
        str = e + 1;
    }
}

void KSP_Browser::callback_deferred(void* v)
{
    KSP_Browser* w = (KSP_Browser*)v;
    w->do_callback();
}
