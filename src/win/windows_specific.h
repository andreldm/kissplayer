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

#define     APP_ICON					0
#define     ID_THUMBNAIL_BUTTON1		40002
#define     ID_THUMBNAIL_BUTTON2        40003
#define     ID_THUMBNAIL_BUTTON3        40004
#define     ID_THUMBNAIL_BUTTONS_IMG	41001

void set_app_icon(Fl_Window *window);
void init_os_specific(Fl_Window *window);
void end_os_specific();

int windows_event_handler(int e);

void create_thumbnail_toolbar(HWND hwnd);
void update_thumbnail_toolbar(string command);

static UINT taskBarCreatedId;
static ITaskbarList3* taskbarList;
static THUMBBUTTON thumb_buttons[3];
static HWND __hwnd;

#endif
