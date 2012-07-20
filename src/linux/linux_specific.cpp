/**
Here lives the Linux specific code.
**/

#include "linux_specific.h"

void set_app_icon(Fl_Window *window)
{
    fl_open_display();
    Pixmap p;
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), const_cast<char**>(icon_xpm), &p, &mask, NULL);
    window->icon((char *)p);
}

void init_os_specific(Fl_Window *window)
{
	//To archieve icon transparency on Linux, we need this operation.
	//Source: www.fltk.org/newsgroups.php?gfltk.general+v:14448

	XWMHints* hints = XGetWMHints(fl_display, fl_xid(window));
	hints->flags |= IconMaskHint;
	hints->icon_mask = mask;
    XSetWMHints(fl_display, fl_xid(window), hints);
    XFree(hints);
}

void end_os_specific(){ }
