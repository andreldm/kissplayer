/**
Here lives the linux specific code.
**/

#include "linux_specific.h"

void set_app_icon(Fl_Window *window)
{
    fl_open_display();
    Pixmap p;
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), icon_xpm, &p, &mask, NULL);
    window->icon((char *)p);
}

//To archieve icon transparency on Linux, we need this operation.
//Source: www.fltk.org/newsgroups.php?gfltk.general+v:14448
void set_app_icon_transparency(Fl_Window *window)
{
	XWMHints* hints = XGetWMHints(fl_display, fl_xid(window));
	hints->flags |= IconMaskHint;
	hints->icon_mask = mask;
    XSetWMHints(fl_display, fl_xid(window), hints);
    XFree(hints);
}

void init_os_specific(){ }
