#ifndef fl_browser_music_h
#define fl_browser_music_h

#include <FL/Fl_Select_Browser.H>

/**
* A modified version of the Fl_Select_Browser that highlights one specific line.
*/

struct FL_BLINE {
    FL_BLINE* prev;
    FL_BLINE* next;
    void* data;
    Fl_Image* icon;
    short length;
    char flags;
    char txt[1];
};

#define SELECTED 1

class FL_EXPORT Fl_Browser_Music : public Fl_Select_Browser
{
private:
    FL_BLINE* highlighted_line;

    /**
    Based on FL_Browser.
    What changes is that the highlighted line has a bold font.
    Also some code not used by the player is removed.
    */
    void item_draw(void* item, int X, int Y, int W, int H) const
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

public:
    void setHighlighted(int line)
    {
        FL_BLINE* item = find_line(line);
        if(item == NULL) return;

        highlighted_line = item;
    }

    void clearHighlighted()
    {
        highlighted_line = NULL;
    }

    Fl_Browser_Music(int x, int y, int w, int h)
        : Fl_Select_Browser(x, y, w, h, NULL)
    {
        type(FL_HOLD_BROWSER);
        box(FL_DOWN_BOX);
    }
};

#endif
