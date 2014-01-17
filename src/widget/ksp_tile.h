#ifndef ksp_tile_h
#define ksp_tile_h

#include <FL/Fl_Tile.H>

/**
 * A modified version of the Fl_Tile that when resized,
 * the Browser_Music grows/shrinks and the Lyrics_Pane remains the same size.
 * While this implementation is not perfect, is far more "confortable" to the
 * user. The only glitch is if the user resize the window so that the Browser_Music
 * width is zero, now the Lyrics_Pane won't shrink together.
 */
class KSP_Tile : public Fl_Tile
{
public:
    KSP_Tile(int x, int y, int w, int h);
    void resize(int X,int Y,int W,int H);
};

#endif
