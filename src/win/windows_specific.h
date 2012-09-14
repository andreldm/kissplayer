/**
 *Here lives the Windows specific code.
 */

#ifndef windows_specific_h
#define windows_specific_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <windows.h>
#include <string.h>
#include "mingw_fix.h"
#include "../window_main.h"

#define _WIN32_IE	0x0500
#include <shlobj.h>

#define FILE_SEPARATOR "\\"

#define     APP_ICON					0
#define     ID_THUMBNAIL_BUTTON1		40002
#define     ID_THUMBNAIL_BUTTON2        40003
#define     ID_THUMBNAIL_BUTTON3        40004
#define     ID_THUMBNAIL_BUTTONS_IMG	41001

#ifndef VK_MEDIA_PLAY_PAUSE
    #define VK_MEDIA_PLAY_PAUSE     0xB3
#endif

#ifndef VK_MEDIA_STOP
    #define VK_MEDIA_STOP           0xB2
#endif

#ifndef VK_MEDIA_PREV_TRACK
    #define VK_MEDIA_PREV_TRACK     0xB1
#endif

#ifndef VK_MEDIA_NEXT_TRACK
    #define VK_MEDIA_NEXT_TRACK     0xB0
#endif

void set_app_icon(Fl_Window *window);
void init_os_specific(Fl_Window *window);
void end_os_specific();
string getWorkingDirectory();
TCHAR *native_dir_chooser();

wchar_t *CodePageToUnicode(int codePage, const char *src);
char *UnicodeToCodePage(int codePage, const wchar_t *src);

int windows_event_handler(int e);

void create_thumbnail_toolbar(HWND hwnd);
void update_thumbnail_toolbar(string command);

static UINT taskBarCreatedId;
static ITaskbarList3* taskbarList;
static THUMBBUTTON thumb_buttons[3];
static HWND __hwnd;

#endif
