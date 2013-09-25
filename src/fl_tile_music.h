#ifndef fl_tile_music_h
#define fl_tile_music_h

#include <FL/Fl_Tile.H>

/**
* A modified version of the Fl_Tile that when resized,
* the Browser_Music grows/shrinks and the Lyrics_Pane remains the same size.
* While this implementation is not perfect, is far more "confortable" to the
* user. The only glitch is if the user resize the window so that the Browser_Music
* width is zero from there the Lyrics_Pane won't shrink together.
*/

class FL_EXPORT Fl_Tile_Music : public Fl_Tile
{
public:

    // Attention: This method is hardcoded, only suitable for this project!!!
    // FLTK's Tile doesn't have a gravity concept, so it'll always take as
    // the reference the widget at right-bottom position... If a resizable
    // is set, the user cannot move the slider.
    void resize(int X,int Y,int W,int H)
    {
        int dx = X-x();
        int dy = Y-y();
        int dw = W-w();
        int dh = H-h();
        int *p = sizes();

        Fl_Widget::resize(X,Y,W,H);

        int OR = p[5];
        int NR = X+W-(p[1]-OR);
        int OB = p[7];
        int NB = Y+H-(p[3]-OB);

        Fl_Widget*const* a = array();

        // The Music Browser
        {
            Fl_Widget* o = a[0];
            int xx = o->x() + dx;
            int yy = o->y() + dy;
            int R = xx + o->w() + dw;
            int B = yy + o->h() + dh;

            o->resize(xx, yy, R-xx, B-yy);
        }

        // The Lyrics Pane
        {
            Fl_Widget* o = a[1];
            int xx = o->x() + dx;
            int yy = o->y() + dy;
            int R = xx + o->w();
            int B = yy + o->h() + dh;

            o->resize(xx + dw, yy, R-xx, B-yy);
        }
    }

    Fl_Tile_Music(int x, int y, int w, int h)
        : Fl_Tile(x, y, w, h, NULL) { }
};

#endif
