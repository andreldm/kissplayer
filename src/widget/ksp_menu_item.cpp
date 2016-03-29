#include "ksp_menu_item.h"

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Multi_Label.H>

/*
 * Lots of code had to be imported from FLTK because the way
 * it's written, so we can't just override Fl_Menu_Item::draw
 * in order to draw a white selector :(
 *
 * Also code from fldigi was borrowed in order to use Fl_Multi_Label correctly.
 * Source: http://git.berlios.de/cgi-bin/gitweb.cgi?p=fldigi;a=blob;f=src/misc/icons.cxx;hb=HEAD#l65
 */

#define LEADING 4

// values for menustate.state:
#define INITIAL_STATE   0   // no mouse up or down since popup() called
#define PUSH_STATE      1   // mouse has been pushed on a normal item
#define DONE_STATE      2   // exit the popup, the current item was picked
#define MENU_PUSH_STATE 3   // mouse has been pushed on a menu title

// This code has to be isolated to avoid conflicts with FLTK's code
namespace KSP_MENU
{
    static const Fl_Menu_* button=0;

    class menutitle : public Fl_Menu_Window
    {
        void draw();
        public:
        const KSP_Menu_Item* menu;
        menutitle(int X, int Y, int W, int H, const KSP_Menu_Item*);
    };

    class menuwindow : public Fl_Menu_Window
    {
        void draw();
        void drawentry(const KSP_Menu_Item*, int i, int erase);
        public:
        menutitle* title;
        int handle(int);
        int itemheight;	// zero == menubar
        int numitems;
        int selected;
        int drawn_selected;	// last redraw has this selected
        int shortcutWidth;
        const KSP_Menu_Item* menu;
        menuwindow(const KSP_Menu_Item* m, int X, int Y, int W, int H,
             const KSP_Menu_Item* picked, const KSP_Menu_Item* title,
             int menubar = 0, int menubar_title = 0, int right_edge = 0);
        ~menuwindow();
        void set_selected(int);
        int find_selected(int mx, int my);
        int titlex(int);
        void autoscroll(int);
        void position(int x, int y);
        int is_inside(int x, int y);
    };

    struct menustate
    {
        const KSP_Menu_Item* current_item; // what mouse is pointing at
        int menu_number; // which menu it is in
        int item_number; // which item in that menu, -1 if none
        menuwindow* p[20]; // pointers to menus
        int nummenus;
        int menubar; // if true p[0] is a menubar
        int state;
        menuwindow* fakemenu; // kludge for buttons in menubar
        int is_inside(int mx, int my);
    };
    static menustate* p=0;

    static inline void setitem(const KSP_Menu_Item* i, int m, int n)
    {
        p->current_item = i;
        p->menu_number = m;
        p->item_number = n;
    }

    static void setitem(int m, int n)
    {
        menustate &pp = *p;
        pp.current_item = (n >= 0) ? (KSP_Menu_Item*) pp.p[m]->menu->next(n) : 0;
        pp.menu_number = m;
        pp.item_number = n;
    }

    static int forward(int menu) // go to next item in menu menu if possible
    {
        menustate &pp = *p;
        // Fl_Menu_Button can generate menu=-1. This line fixes it and selectes the first item.
        if (menu==-1) menu = 0;
        menuwindow &m = *(pp.p[menu]);
        int item = (menu == pp.menu_number) ? pp.item_number : m.selected;
        while (++item < m.numitems) {
            const KSP_Menu_Item* m1 = (KSP_Menu_Item*) m.menu->next(item);
            if (m1->activevisible()) {setitem(m1, menu, item); return 1;}
        }
        return 0;
    }

    static int backward(int menu) // previous item in menu menu if possible
    {
        menustate &pp = *p;
        menuwindow &m = *(pp.p[menu]);
        int item = (menu == pp.menu_number) ? pp.item_number : m.selected;
        if (item < 0) item = m.numitems;
        while (--item >= 0) {
            const KSP_Menu_Item* m1 = (KSP_Menu_Item*) m.menu->next(item);
            if (m1->activevisible()) {setitem(m1, menu, item); return 1;}
        }
        return 0;
    }

    menutitle::menutitle(int X, int Y, int W, int H, const KSP_Menu_Item* L)
        : Fl_Menu_Window(X, Y, W, H, 0)
    {
        end();
        set_modal();
        clear_border();
        set_menu_window();
        menu = L;
        if (L->labelcolor_ || Fl::scheme() || L->labeltype_ > FL_NO_LABEL) clear_overlay();
    }

    void menutitle::draw()
    {
        menu->draw(0, 0, w(), h(), button, 2);
    }

    menuwindow::menuwindow(const KSP_Menu_Item* m, int X, int Y, int Wp, int Hp,
        const KSP_Menu_Item* picked, const KSP_Menu_Item* t,
        int menubar, int menubar_title, int right_edge): Fl_Menu_Window(X, Y, Wp, Hp, 0)
    {
        int scr_x, scr_y, scr_w, scr_h;
        int tx = X, ty = Y;

        Fl::screen_work_area(scr_x, scr_y, scr_w, scr_h);
        if (!right_edge || right_edge > scr_x+scr_w) right_edge = scr_x+scr_w;

        end();
        set_modal();
        clear_border();
        set_menu_window();
        menu = m;
        if (m) m = (KSP_Menu_Item*) m->first(); // find the first item that needs to be rendered
        drawn_selected = -1;

        box(FL_UP_BOX);
        color(FL_GRAY);
        selected = -1;
        {
            int j = 0;
            if (m){
                for (const KSP_Menu_Item* m1=m; ; m1 = (KSP_Menu_Item*) m1->next(), j++) {
                    if (picked) {
                        if (m1 == picked) {selected = j; picked = 0;}
                        else if (m1 > picked) {selected = j-1; picked = 0; Wp = Hp = 0;}
                    }
                    if (!m1->text) break;
                }
            }
            numitems = j;
        }

        if (menubar) {
            itemheight = 0;
            title = 0;
            return;
        }

        itemheight = 1;

        int hotKeysw = 0;
        int hotModsw = 0;
        int Wtitle = 0;
        int Htitle = 0;
        int W = 0;

        if (m) {
            for (; m->text; m = (KSP_Menu_Item*) m->next()) {
                int hh;
                int w1 = m->measure(&hh, button);
                if (hh+LEADING>itemheight) itemheight = hh+LEADING;
                if (m->flags&(FL_SUBMENU|FL_SUBMENU_POINTER))
                w1 += FL_NORMAL_SIZE;
                if (w1 > W) W = w1;
                // calculate the maximum width of all shortcuts
                if (m->shortcut_) {
                    // s is a pointerto the utf8 string for the entire shortcut
                    // k points only to the key part (minus the modifier keys)
                    const char *k, *s = fl_shortcut_label(m->shortcut_, &k);
                    if (fl_utf_nb_char((const unsigned char*)k, (int) strlen(k))<=4) {
                        // a regular shortcut has a right-justified modifier followed by a left-justified key
                        w1 = int(fl_width(s, (int) (k-s)));
                        if (w1 > hotModsw) hotModsw = w1;
                        w1 = int(fl_width(k))+4;
                        if (w1 > hotKeysw) hotKeysw = w1;
                    } else {
                        // a shortcut with a long modifier is right-justified to the menu
                        w1 = int(fl_width(s))+4;
                        if (w1 > (hotModsw+hotKeysw)) {
                            hotModsw = w1-hotKeysw;
                        }
                    }
                }
                if (m->labelcolor_ || Fl::scheme() || m->labeltype_ > FL_NO_LABEL) clear_overlay();
            }
        }

        shortcutWidth = hotKeysw;
        if (selected >= 0 && !Wp) X -= W/2;
        int BW = Fl::box_dx(box());
        W += hotKeysw+hotModsw+2*BW+7;
        if (Wp > W) W = Wp;
        if (Wtitle > W) W = Wtitle;

        if (X < scr_x) X = scr_x; if (X > scr_x+scr_w-W) X = right_edge-W;
        x(X); w(W);
        h((numitems ? itemheight*numitems-LEADING : 0)+2*BW+3);

        if (selected >= 0) {
            Y = Y+(Hp-itemheight)/2-selected*itemheight-BW;
        } else {
            Y = Y+Hp;
            // if the menu hits the bottom of the screen, we try to draw
            // it above the menubar instead. We will not adjust any menu
            // that has a selected item.
            if (Y+h()>scr_y+scr_h && Y-h()>=scr_y) {
                if (Hp>1) {
                    // if we know the height of the Fl_Menu_, use it
                    Y = Y-Hp-h();
                } else if (t) {
                    // assume that the menubar item height relates to the first
                    // menuitem as well
                    Y = Y-itemheight-h()-Fl::box_dh(box());
                } else {
                    // draw the menu to the right
                    Y = Y-h()+itemheight+Fl::box_dy(box());
                }
            }
        }

        if (m) y(Y); else {y(Y-2); w(1); h(1);}

        if (t) {
            if (menubar_title) {
                int dy = Fl::box_dy(button->box())+1;
                int ht = button->h()-dy*2;
                title = new menutitle(tx, ty-ht-dy, Wtitle, ht, t);
            } else {
                int dy = 2;
                int ht = Htitle+2*BW+3;
                title = new menutitle(X, Y-ht-dy, Wtitle, ht, t);
            }
        } else {
            title = 0;
        }
    }

    int menuwindow::handle(int e)
    {
        menustate& pp = *p;
        switch (e) {
        case FL_KEYBOARD:
            switch (Fl::event_key()) {
            case FL_BackSpace:
                BACKTAB:
                if (!backward(pp.menu_number)) {pp.item_number = -1;backward(pp.menu_number);}
                return 1;
            case FL_Up:
                if (pp.menubar && pp.menu_number == 0) {
                    // Do nothing...
                } else if (backward(pp.menu_number)) {
                    // Do nothing...
                } else if (pp.menubar && pp.menu_number==1) {
                    setitem(0, pp.p[0]->selected);
                }
                return 1;
            case FL_Tab:
                if (Fl::event_shift()) goto BACKTAB;
            case FL_Down:
                if (pp.menu_number || !pp.menubar) {
                    if (!forward(pp.menu_number) && Fl::event_key()==FL_Tab) {
                        pp.item_number = -1;
                        forward(pp.menu_number);
                    }
                } else if (pp.menu_number < pp.nummenus-1) {
                    forward(pp.menu_number+1);
                }
                return 1;
            case FL_Right:
                if (pp.menubar && (pp.menu_number<=0 || (pp.menu_number==1 && pp.nummenus==2)))
                    forward(0);
                else if (pp.menu_number < pp.nummenus-1) forward(pp.menu_number+1);
                return 1;
            case FL_Left:
                if (pp.menubar && pp.menu_number<=1) backward(0);
                else if (pp.menu_number>0)
                    setitem(pp.menu_number-1, pp.p[pp.menu_number-1]->selected);
                return 1;
            case FL_Enter:
            case FL_KP_Enter:
            case ' ':
                pp.state = DONE_STATE;
                return 1;
            case FL_Escape:
                setitem(0, -1, 0);
                pp.state = DONE_STATE;
                return 1;
            }
            break;

        case FL_SHORTCUT:
        {
            for (int mymenu = pp.nummenus; mymenu--;) {
                menuwindow &mw = *(pp.p[mymenu]);
                int item; const KSP_Menu_Item* m = (KSP_Menu_Item*) mw.menu->find_shortcut(&item);
                if (m) {
                    setitem(m, mymenu, item);
                    if (!m->submenu()) pp.state = DONE_STATE;
                    return 1;
                }
            }
            break;
        }

        case FL_MOVE:
            #if ! (defined(WIN32) || defined(__APPLE__))
            if (pp.state == DONE_STATE) {
                return 1; // Fix for STR #2619
            }
            /* FALLTHROUGH */
            #endif
        case FL_ENTER:
        case FL_PUSH:
        case FL_DRAG:
        {
            int mx = Fl::event_x_root();
            int my = Fl::event_y_root();
            int item=0; int mymenu = pp.nummenus-1;
            // Clicking or dragging outside menu cancels it...
            if ((!pp.menubar || mymenu) && !pp.is_inside(mx, my)) {
                setitem(0, -1, 0);
                if (e==FL_PUSH)
                    pp.state = DONE_STATE;
                return 1;
            }
            for (mymenu = pp.nummenus-1; ; mymenu--) {
                item = pp.p[mymenu]->find_selected(mx, my);
                if (item >= 0)
                    break;
                if (mymenu <= 0) {
                    // buttons in menubars must be deselected if we move outside of them!
                    if (pp.menu_number==-1 && e==FL_PUSH) {
                        pp.state = DONE_STATE;
                        return 1;
                    }
                    if (pp.current_item && pp.menu_number==0 && !pp.current_item->submenu()) {
                        if (e==FL_PUSH)
                            pp.state = DONE_STATE;
                        setitem(0, -1, 0);
                        return 1;
                    }
                    // all others can stay selected
                    return 0;
                }
            }

            if (my == 0 && item > 0) setitem(mymenu, item - 1);
            else setitem(mymenu, item);
            if (e == FL_PUSH) {
                if (pp.current_item && pp.current_item->submenu() // this is a menu title
                    && item != pp.p[mymenu]->selected // and it is not already on
                    && !pp.current_item->callback_) { // and it does not have a callback
                    pp.state = MENU_PUSH_STATE;
                } else {
                    pp.state = PUSH_STATE;
                }
            }
            return 1;
        }

        case FL_RELEASE:
            // Mouse must either be held down/dragged some, or this must be
            // the second click (not the one that popped up the menu):
            if (!Fl::event_is_click()
                    || pp.state == PUSH_STATE
                    || (pp.menubar && pp.current_item && !pp.current_item->submenu())) { // button
                    // do nothing if they try to pick inactive items
                    if (!pp.current_item || pp.current_item->activevisible())
                    pp.state = DONE_STATE;
            }
            return 1;
        }
        return Fl_Window::handle(e);
    }

    void menuwindow::draw()
    {
        if (damage() != FL_DAMAGE_CHILD) {	// complete redraw
            fl_draw_box(box(), 0, 0, w(), h(), button ? button->color() : color());
            if (menu) {
                const KSP_Menu_Item* m; int j;
                for (m=(KSP_Menu_Item*) menu->first(), j=0; m->text; j++, m = (KSP_Menu_Item*) m->next()) drawentry(m, j, 0);
            }
        } else {
            if (damage() & FL_DAMAGE_CHILD && selected!=drawn_selected) { // change selection
                drawentry((KSP_Menu_Item*) menu->next(drawn_selected), drawn_selected, 1);
                drawentry((KSP_Menu_Item*) menu->next(selected), selected, 1);
            }
        }
        drawn_selected = selected;
    }

    void menuwindow::set_selected(int n)
    {
        if (n != selected) {selected = n; damage(FL_DAMAGE_CHILD);}
    }

    int menuwindow::find_selected(int mx, int my)
    {
        if (!menu || !menu->text) return -1;
        mx -= x();
        my -= y();
        if (my < 0 || my >= h()) return -1;
        if (!itemheight) { // menubar
            int xx = 3; int n = 0;
            const KSP_Menu_Item* m = menu ? (KSP_Menu_Item*) menu->first() : 0;
            for (; ; m = (KSP_Menu_Item*) m->next(), n++) {
                if (!m->text) return -1;
                xx += m->measure(0, button) + 16;
                if (xx > mx) break;
            }
            return n;
        }
        if (mx < Fl::box_dx(box()) || mx >= w()) return -1;
        int n = (my-Fl::box_dx(box())-1)/itemheight;
        if (n < 0 || n>=numitems) return -1;

        return n;
    }

    // return horizontal position for item n in a menubar:
    int menuwindow::titlex(int n)
    {
        const KSP_Menu_Item* m;
        int xx = 3;
        for (m=(KSP_Menu_Item*) menu->first(); n--; m = (KSP_Menu_Item*) m->next()) {
            xx += m->measure(0, button) + 16;
        }
        return xx;
    }

    // return 1, if the given root coordinates are inside the window
    int menuwindow::is_inside(int mx, int my)
    {
        if (mx < x_root() || mx >= x_root() + w() ||
            my < y_root() || my >= y_root() + h()) {
            return 0;
        }
        if (itemheight == 0 && find_selected(mx, my) == -1) {
            // in the menubar but out from any menu header
            return 0;
        }
        return 1;
    }

    menuwindow::~menuwindow()
    {
        hide();
        delete title;
    }

    void menuwindow::position(int X, int Y)
    {
        if (title) {title->position(X, title->y()+Y-y());}
        Fl_Menu_Window::position(X, Y);
    }

    // scroll so item i is visible on screen
    void menuwindow::autoscroll(int n)
    {
        int scr_y, scr_h;
        int Y = y()+Fl::box_dx(box())+2+n*itemheight;

        int xx, ww;
        Fl::screen_work_area(xx, scr_y, ww, scr_h);
        if (Y <= scr_y) {
            Y = scr_y-Y+10;
        } else {
            Y = Y+itemheight-scr_h-scr_y;
            if (Y < 0) return;
            Y = -Y-10;
        }
        Fl_Menu_Window::position(x(), y()+Y);
    }

    void menuwindow::drawentry(const KSP_Menu_Item* m, int n, int eraseit)
    {
        if (!m) return; // this happens if -1 is selected item and redrawn

        int BW = Fl::box_dx(box());
        int xx = BW;
        int W = w();
        int ww = W-2*BW-1;
        int yy = BW+1+n*itemheight;
        int hh = itemheight - LEADING;

        if (eraseit && n != selected) {
            fl_push_clip(xx+1, yy-(LEADING-2)/2, ww-2, hh+(LEADING-2));
            draw_box(box(), 0, 0, w(), h(), button ? button->color() : color());
            fl_pop_clip();
        }

        m->draw(xx, yy, ww, hh, button, n==selected);

        // the shortcuts and arrows assume fl_color() was left set by draw():
        if (m->submenu()) {
            int sz = (hh-7)&-2;
            int y1 = yy+(hh-sz)/2;
            int x1 = xx+ww-sz-3;
            fl_polygon(x1+2, y1, x1+2, y1+sz, x1+sz/2+2, y1+sz/2);
        } else if (m->shortcut_) {
            Fl_Font f = m->labelsize_ || m->labelfont_ ? (Fl_Font)m->labelfont_ :
            button ? button->textfont() : FL_HELVETICA;
            fl_font(f, m->labelsize_ ? m->labelsize_ :
            button ? button->textsize() : FL_NORMAL_SIZE);
            const char *k, *s = fl_shortcut_label(m->shortcut_, &k);
            if (fl_utf_nb_char((const unsigned char*)k, (int) strlen(k))<=4) {
                // righ-align the modifiers and left-align the key
                char buf[32]; strcpy(buf, s); buf[k-s] = 0;
                fl_draw(buf, xx, yy, ww-shortcutWidth, hh, FL_ALIGN_RIGHT);
                fl_draw(  k, xx+ww-shortcutWidth, yy, shortcutWidth, hh, FL_ALIGN_LEFT);
            } else {
                // right-align to the menu
                fl_draw(s, xx, yy, ww-4, hh, FL_ALIGN_RIGHT);
            }
        }

        if (m->flags & FL_MENU_DIVIDER) {
            fl_color(FL_DARK3);
            fl_xyline(BW-1, yy+hh+(LEADING-2)/2, W-2*BW+2);
            fl_color(FL_LIGHT3);
            fl_xyline(BW-1, yy+hh+((LEADING-2)/2+1), W-2*BW+2);
        }
    }

    int menustate::is_inside(int mx, int my)
    {
        int i;
        for (i = nummenus - 1; i >= 0; i--) {
            if (p[i]->is_inside(mx, my))
                return 1;
        }
        return 0;
    }
}

using namespace KSP_MENU;

KSP_Menu_Item::KSP_Menu_Item (const char* label, int shortcut1,
    Fl_Callback* callback, void* data, int flags1)
{
    text = label;
    shortcut(shortcut1);
    callback_ = callback;
    user_data(data);
    flags |= flags1;
}

void KSP_Menu_Item::draw(int x, int y, int w, int h, const Fl_Menu_* m, int selected) const
{
    Fl_Label l;
    l.value   = text;
    l.image   = 0;
    l.deimage = 0;
    l.type    = labeltype_;
    l.font    = labelsize_ || labelfont_ ? labelfont_ : (m ? m->textfont() : FL_HELVETICA);
    l.size    = labelsize_ ? labelsize_ : m ? m->textsize() : FL_NORMAL_SIZE;
    l.color   = labelcolor_ ? labelcolor_ : m ? m->textcolor() : int(FL_FOREGROUND_COLOR);
    if (!active()) l.color = fl_inactive((Fl_Color)l.color);
    Fl_Color color = m ? m->color() : FL_GRAY;

    if (selected) {
        Fl_Color r = m ? m->selection_color() : FL_SELECTION_COLOR;
        Fl_Boxtype b = m && m->down_box() ? m->down_box() : FL_FLAT_BOX;
        if (fl_contrast(r,color)!=r) { // back compatibility boxtypes
            if (selected == 2) { // menu title
                r = color;
                b = m ? m->box() : FL_UP_BOX;
            } else {
                r = (Fl_Color)(FL_COLOR_CUBE-1); // white
                l.color = fl_contrast((Fl_Color)labelcolor_, r);
            }
        } else {
            l.color = fl_contrast((Fl_Color)labelcolor_, r);
        }

        if (selected == 2) { // menu title
            fl_draw_box(b, x, y, w, h, r);
            x += 3;
            w -= 8;
        } else {
            fl_draw_box(b, x+1, y-(LEADING-2)/2, w-2, h+(LEADING-2), r);
        }
    }

    if (flags & (FL_MENU_TOGGLE)) {
        int d = (h - FL_NORMAL_SIZE + 1) / 2;
        int W = h - 2 * d;

        fl_draw_box(FL_DOWN_BOX, x+2, y+d, W, W, FL_BACKGROUND2_COLOR);
        if (value()) {
            fl_color(FL_WHITE);

            int tx = x + 5;
            int tw = W - 6;
            int d1 = tw/3;
            int d2 = tw-d1;
            int ty = y + d + (W+d2)/2-d1-2;
            for (int n = 0; n < 3; n++, ty++) {
                fl_line(tx, ty, tx+d1, ty+d1);
                fl_line(tx+d1, ty+d1, tx+tw-1, ty+d1-d2+1);
            }
        }
        x += W + 3;
        w -= W + 3;
    }

    if (!fl_draw_shortcut) fl_draw_shortcut = 1;
    l.draw(x+3, y, w>6 ? w-6 : 0, h, FL_ALIGN_LEFT);
    fl_draw_shortcut = 0;
}

const KSP_Menu_Item* KSP_Menu_Item::pulldown(int X, int Y) const
{
    Fl_Group::current(0); // fix possible user error...

    button = 0;

    X += Fl::event_x_root()-Fl::event_x();
    Y += Fl::event_y_root()-Fl::event_y();

    menuwindow mw(this, X, Y, 0, 0, 0, 0, 0);
    Fl::grab(mw);
    menustate pp; p = &pp;
    pp.p[0] = &mw;
    pp.nummenus = 1;
    pp.menubar = 0;
    pp.state = INITIAL_STATE;
    pp.fakemenu = 0; // kludge for buttons in menubar
    pp.current_item = 0;
    pp.menu_number = 0;
    pp.item_number = -1;

    const KSP_Menu_Item* initial_item = pp.current_item;
    if (initial_item) goto STARTUP;

    // the main loop, runs until p.state goes to DONE_STATE:
    for (;;) {

        // make sure all the menus are shown:
        {
            for (int k = 0; k < pp.nummenus; k++) {
                if (!pp.p[k]->shown()) {
                    if (pp.p[k]->title) pp.p[k]->title->show();
                        pp.p[k]->show();
                }
            }
        }

        // get events:
        {
          const KSP_Menu_Item* oldi = pp.current_item;
          Fl::wait();
          if (pp.state == DONE_STATE) break; // done.
          if (pp.current_item == oldi) continue;
        }

        // only do rest if item changes:
        if (pp.fakemenu) {delete pp.fakemenu; pp.fakemenu = 0;} // turn off "menubar button"

        if (!pp.current_item) { // pointing at nothing
            // turn off selection in deepest menu, but don't erase other menus:
            pp.p[pp.nummenus-1]->set_selected(-1);
            continue;
        }

        if (pp.fakemenu) {delete pp.fakemenu; pp.fakemenu = 0;}
        initial_item = 0; // stop the startup code
        pp.p[pp.menu_number]->autoscroll(pp.item_number);

        STARTUP:
        menuwindow& cw = *pp.p[pp.menu_number];
        const KSP_Menu_Item* m = pp.current_item;
        if (!m->activevisible()) { // pointing at inactive item
            cw.set_selected(-1);
            initial_item = 0; // turn off startup code
            continue;
        }
        cw.set_selected(pp.item_number);

        if (m==initial_item) initial_item=0; // stop the startup code if item found
        if (m->submenu()) {
            const KSP_Menu_Item* title = m;
            const KSP_Menu_Item* menutable;
            if (m->flags&FL_SUBMENU) menutable = m+1;
            else menutable = (KSP_Menu_Item*)(m)->user_data_;
            // figure out where new menu goes:
            int nX, nY;
            if (!pp.menu_number && pp.menubar) {	// menu off a menubar:
                nX = cw.x() + cw.titlex(pp.item_number);
                nY = cw.y() + cw.h();
                initial_item = 0;
            } else {
                nX = cw.x() + cw.w();
                nY = cw.y() + pp.item_number * cw.itemheight;
                title = 0;
            }

            if (initial_item) { // bring up submenu containing initial item:
                menuwindow* n = new menuwindow(menutable,X,Y,0,0,initial_item,title,0,0,cw.x());
                pp.p[pp.nummenus++] = n;
                // move all earlier menus to line up with this new one:
                if (n->selected>=0) {
                    int dy = n->y()-nY;
                    int dx = n->x()-nX;
                    for (int menu = 0; menu <= pp.menu_number; menu++) {
                        menuwindow* tt = pp.p[menu];
                        int nx = tt->x()+dx; if (nx < 0) {nx = 0; dx = -tt->x();}
                        int ny = tt->y()+dy; if (ny < 0) {ny = 0; dy = -tt->y();}
                        tt->position(nx, ny);
                    }
                    setitem(pp.nummenus-1, n->selected);
                    goto STARTUP;
                }
            } else if (pp.nummenus > pp.menu_number+1 && pp.p[pp.menu_number+1]->menu == menutable) {
                // the menu is already up:
                while (pp.nummenus > pp.menu_number+2) delete pp.p[--pp.nummenus];
                pp.p[pp.nummenus-1]->set_selected(-1);
            } else {
                // delete all the old menus and create new one:
                while (pp.nummenus > pp.menu_number+1) delete pp.p[--pp.nummenus];
                pp.p[pp.nummenus++]= new menuwindow(menutable, nX, nY,
                                  title?1:0, 0, 0, title, 0, 0,
                                    (title ? 0 : cw.x()) );
            }
        } else { // !m->submenu():
            while (pp.nummenus > pp.menu_number+1) delete pp.p[--pp.nummenus];
                if (!pp.menu_number && pp.menubar) {
                // kludge so "menubar buttons" turn "on" by using menu title:
                pp.fakemenu = new menuwindow(0,
                          cw.x()+cw.titlex(pp.item_number),
                          cw.y()+cw.h(), 0, 0,
                          0, m, 0, 1);
                pp.fakemenu->title->show();
            }
        }
    }
    const KSP_Menu_Item* m = pp.current_item;
    delete pp.fakemenu;
    while (pp.nummenus>1) delete pp.p[--pp.nummenus];
    mw.hide();
    Fl::grab(0);
    return m;
}

const KSP_Menu_Item* KSP_Menu_Item::popup(int X, int Y) const
{
    return pulldown(X, Y);
}

void KSP_Menu_Item::set_label_icon(const char* text, Fl_Image* icon, bool prepend_space)
{
    if (!text) text = "";
    char* text2;

    if (prepend_space) {
        size_t len = strlen(text);
        text2 = new char[len + 2];
        text2[0] = ' ';
        memcpy(text2 + 1, text, len + 1);
    } else {
        text2 = (char *)text;
    }

    if (!icon) {
        label(text2);
        return;
    }

    // Code simplified from fldigi
    Fl_Multi_Label* mlabel = new Fl_Multi_Label();
    mlabel->labela = (const char*)icon;
    mlabel->typea = _FL_IMAGE_LABEL;

    mlabel->labelb = text2;
    mlabel->typeb = FL_NORMAL_LABEL;

    mlabel->label(this);

    image(icon);
    label((const char*)mlabel);
    labeltype(_FL_MULTI_LABEL);
}

void KSP_Menu_Item::set_toggled(bool toggled)
{
    if (toggled) set();
    else clear();
}
