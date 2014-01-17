#ifndef ksp_browser_h
#define ksp_browser_h

#include <string>

#include <FL/Fl_Select_Browser.H>

struct FL_BLINE;

/**
 * A custom select browser that highlights one specific line.
 * Also reacts to Drag and Drop.
 */
class KSP_Browser : public Fl_Select_Browser
{
public:
    std::string evt_txt;
    bool dnd_evt;

    KSP_Browser(int x, int y, int w, int h);
    int handle(int e);
    void set_highlighted(int line);
    void clear_highlighted();

private:
    FL_BLINE* highlighted_line;

    void item_draw(void* item, int X, int Y, int W, int H) const;
    static void callback_deferred(void* v);
};

#endif
