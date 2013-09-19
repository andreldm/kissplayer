/**
Here lives the Linux specific code.
**/

#include "linux_specific.h"

#include <X11/Xlibint.h>
#include <X11/XKBlib.h>
#include <X11/XF86keysym.h>
#include <X11/extensions/record.h>
#include <unistd.h>

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

/**
 * Set the window to maximized state.
 * Source: www.mail-archive.com/xfree86@xfree86.org/msg21266.html
 */
void maximizeWindow(Fl_Window *window)
{
    static Atom atomState = XInternAtom (fl_display, "_NET_WM_STATE", True );
    static Atom atomMaxVert = XInternAtom (fl_display, "_NET_WM_STATE_MAXIMIZED_VERT", True );
    static Atom atomMaxHorz = XInternAtom (fl_display, "_NET_WM_STATE_MAXIMIZED_HORZ", True );
    
    XEvent xev;
    memset(&xev,0,sizeof(xev));;
    xev.xclient.type=ClientMessage;
    xev.xclient.serial=0;
    xev.xclient.send_event=True;
    xev.xclient.window=fl_xid(window);
    xev.xclient.format=32;
    xev.xclient.message_type=atomState;
    xev.xclient.data.l[0]=(unsigned long)1;
    xev.xclient.data.l[1]=atomMaxVert;
    xev.xclient.data.l[2]=atomMaxHorz;
    XSendEvent(fl_display,DefaultRootWindow(fl_display),False,SubstructureRedirectMask|SubstructureNotifyMask,&xev);
}

/**
 * Check if the window is maximized.
 * Source: SDL_x11window.c
 */
bool isWindowMaximized(Fl_Window *window)
{
    static Atom atomState = XInternAtom (fl_display, "_NET_WM_STATE", True );
    static Atom atomMaxVert = XInternAtom (fl_display, "_NET_WM_STATE_MAXIMIZED_VERT", True );
    static Atom atomMaxHorz = XInternAtom (fl_display, "_NET_WM_STATE_MAXIMIZED_HORZ", True );
    
    Atom actualType;
    int actualFormat;
    unsigned long i, numItems, bytesAfter;
    unsigned char *propertyValue = NULL;
    long maxLength = 1024;
    int maximized = 0;
    
    if ( Success == XGetWindowProperty(fl_display, fl_xid(window), atomState,
                           0l, maxLength, False, XA_ATOM, &actualType,
                           &actualFormat, &numItems, &bytesAfter,
                           &propertyValue)) {
        Atom *atoms = (Atom *) propertyValue;
        
        for (i = 0; i < numItems; ++i) {
            if (atoms[i] == atomMaxVert) {
                maximized |= 1;
            } else if (atoms[i] == atomMaxHorz) {
                maximized |= 2;
            }
        }
    }
    
    return (maximized == 3);
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

void os_specific_scanfolder(const char* dir, deque<string>& filelist)
{
    dirent** list;
    int fileQty = fl_filename_list(dir, &list);
    
    for (int i = 0; i < fileQty; i++) {
        const char* filename = list[i]->d_name;
        
        if(filename[0] == '.') {
            continue;
        }
        
        char buffer[4096];
        sprintf(buffer, "%s/%s", dir, filename);
        
        if(fl_filename_isdir(buffer)) {
            os_specific_scanfolder(buffer, filelist);
            continue;
        }
        
        const char* ext = fl_filename_ext(filename);
        
        if(strcmp(ext, ".mp3") == 0 ||
           strcmp(ext, ".wma") == 0 ||
           strcmp(ext, ".ogg") == 0 ||
           strcmp(ext, ".wav") == 0 ||
           strcmp(ext, ".flac") == 0) {
               filelist.push_back(buffer);
        }
    }
    
    fl_filename_free_list(&list, fileQty);
}
