/**
Here lives the Linux specific code.
**/

#include "linux_specific.h"

#include <X11/Xlibint.h>
#include <X11/XKBlib.h>
#include <X11/XF86keysym.h>
#include <X11/extensions/record.h>

static Display* disp_data;
static XRecordContext context;
extern Display* fl_display;

// struct taken from libxnee
typedef union {
    unsigned char           type;
    xEvent                  event;
} XRecordDatum;

void keyHookCallback(XPointer pointer, XRecordInterceptData * hook);
void keyHookTimer(void*);

void set_app_icon(Fl_Window *window)
{
    fl_open_display();
    Pixmap p;
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), const_cast<char**>(icon_xpm), &p, &mask, NULL);
    window->icon((char*)p);
}

void init_os_specific(Fl_Window *window)
{
    // To achieve icon transparency on Linux, we need this procedure.
    // Source: www.fltk.org/newsgroups.php?gfltk.general+v:14448
    XWMHints* hints = XGetWMHints(fl_display, fl_xid(window));
    hints->flags |= IconMaskHint;
    hints->icon_mask = mask;
    XSetWMHints(fl_display, fl_xid(window), hints);
    XFree(hints);

    // Setup keyboard hook
    // Try to attach to the default X11 display
    disp_data = XOpenDisplay(NULL);
    if(disp_data == NULL ) {
        printf("Error: Could not open display!\n");
        return;
    }

    // Setup XRecord range
    XRecordClientSpec clients = XRecordAllClients;
    XRecordRange* range = XRecordAllocRange();
    if (range == NULL) {
        printf("Error: Could not allocate XRecordRange!");
        return;
    }

    // Create XRecord Context
    range->device_events.first = KeyPress;
    range->device_events.last = MotionNotify;
    context = XRecordCreateContext(disp_data, 0, &clients, 1, &range, 1);
    XFree(range);
    if (context == 0) {
        printf("Error: Could not create XRecordContext!");
        return;
    }
    
    XRecordEnableContextAsync(disp_data, context, keyHookCallback, NULL);
    
    Fl::repeat_timeout(0.1, keyHookTimer);
}

void end_os_specific()
{
    // Close the data display.
    /*if(disp_data) {
        XCloseDisplay(disp_data); // Crash!
    }*/
}

string getWorkingDirectory()
{
    struct passwd *pw = getpwuid(getuid());
    string path = pw->pw_dir;
    path.append("/.kissplayer/");
    return path;
}

char* native_dir_chooser()
{
    return fl_dir_chooser("Select a folder", NULL);
}

void keyHookTimer(void*)
{
    Fl::repeat_timeout(0.3, keyHookTimer);
    XRecordProcessReplies(disp_data);
}

void keyHookCallback(XPointer pointer, XRecordInterceptData * hook) {
    static int keyPressed = false;
    // Make sure that our data come from a legitimate source.
    if (hook->category != XRecordFromServer && hook->category != XRecordFromClient) {
        XRecordFreeData(hook);
        return;
    }

    // Convert the hook data to an XRecordDatum.
    XRecordDatum* data = (XRecordDatum *) hook->data;
    
    if(data->type == KeyRelease){
        keyPressed = false;
    }
    
    if(!keyPressed && data->type == KeyPress){
        KeySym k = XkbKeycodeToKeysym(fl_display, data->event.u.u.detail, 0, 0);
        
        if(k == XF86XK_AudioPlay) cb_toggle_play(NULL, 0);
        if(k == XF86XK_AudioNext) cb_next(NULL, 0);
        if(k == XF86XK_AudioPrev) cb_previous(NULL, 0);
        
        keyPressed = true;
    }

    XRecordFreeData(hook);
}
