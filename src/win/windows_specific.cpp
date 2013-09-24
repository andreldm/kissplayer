#include "windows_specific.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define _WIN32_IE   0x0500
#include <shlobj.h>

#include <FL/x.H>

#include "mingw_fix.h"
#include "../window_main.h"

using namespace std;

#define     APP_ICON                    0
#define     ID_THUMBNAIL_BUTTON1        40002
#define     ID_THUMBNAIL_BUTTON2        40003
#define     ID_THUMBNAIL_BUTTON3        40004
#define     ID_THUMBNAIL_BUTTONS_IMG    41001

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

// Keyboard hook handler
static HHOOK handlerKeyboardHook = NULL;

// Keyboard hook callback
static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

static UINT taskBarCreatedId;
static ITaskbarList3* taskbarList;
static THUMBBUTTON thumb_buttons[3];
static HWND hwnd;

static void create_thumbnail_toolbar();
static int windows_event_handler(int e);

/**
* Sets the window icon.
*/
void os_specific_set_app_icon(Fl_Window* window)
{
    window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(APP_ICON)));
}

/**
* Starts Windows specific configuration.
*/
int os_specific_init(Fl_Window* window)
{
    Fl::add_handler(windows_event_handler);
    taskBarCreatedId = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

    // Setup the low level keyboard hook
    HINSTANCE hHandle = GetModuleHandle(NULL);
    handlerKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hHandle, 0);

    if (handlerKeyboardHook == NULL) {
        return 1;
    }

    return 0;
}

/**
* Executes the necessary clean up.
*/
void os_specific_end()
{
    // On Windows 7
    if(taskbarList != NULL) {
        taskbarList->Release();
    }

    UnhookWindowsHookEx(handlerKeyboardHook);
}

/**
* Get the directory where the database file is
*/
void os_specific_get_working_dir(string& dir)
{
    wchar_t wpath[FILENAME_MAX];
    GetModuleFileNameW(NULL, wpath, FILENAME_MAX);
    wchar_t wdir[FILENAME_MAX];
    _wsplitpath(wpath, NULL, wdir, NULL, NULL);

    char path[FILENAME_MAX];
    fl_utf8fromwc(path, FILENAME_MAX, wdir, lstrlenW(wdir));

    dir.replace(0, strlen(path), path);
}

/**
* Calls native directory chooser.
*/
void os_specific_dir_chooser(char* dir)
{
    BROWSEINFOW bi = { 0 };
    bi.lpszTitle = L"Select a folder:";
    wchar_t wpath[FILENAME_MAX];
    LPITEMIDLIST pidl = SHBrowseForFolderW (&bi);
    SHGetPathFromIDListW(pidl, wpath);

    char path[FILENAME_MAX];
    fl_utf8fromwc(path, FILENAME_MAX, wpath, lstrlenW(wpath));

    strcpy(dir, path);
}

/**
 * Set the window to maximized state.
 */
void os_specific_maximize_window(Fl_Window* window)
{
    SendMessage(fl_xid(window), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

/**
 * Check if the window is maximized.
 */
bool os_specific_is_window_maximized(Fl_Window* window)
{
    return IsZoomed(fl_xid(window));
}

/**
* Custom event handler.
*/
int windows_event_handler(int e)
{
    if(fl_msg.message == taskBarCreatedId) {
        hwnd = fl_msg.hwnd;
        create_thumbnail_toolbar();
        return 1;
    }

    if(fl_msg.message == WM_COMMAND) {
        if(LOWORD(fl_msg.wParam) == ID_THUMBNAIL_BUTTON1) {
            window_main_previous();
            return 1;
        }
        if(LOWORD(fl_msg.wParam) == ID_THUMBNAIL_BUTTON2) {
            window_main_toggle_play();
            return 1;
        }
        if(LOWORD(fl_msg.wParam) == ID_THUMBNAIL_BUTTON3) {
            window_main_next();
            return 1;
        }
    }

    return 0;
}
/**
* Creates the thumbnail toolbar.
* Called after windows registers the toolbar successfully, which only happens on Win7.
*/
void create_thumbnail_toolbar()
{
    HRESULT result = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
        reinterpret_cast<void**> (&(taskbarList)));

    if (SUCCEEDED(result)) {
        result = taskbarList->HrInit();

        if (FAILED(result)) {
            taskbarList->Release();
            taskbarList = NULL;
        }
    }

    HIMAGELIST imageList = ImageList_LoadImage(fl_display, MAKEINTRESOURCE(ID_THUMBNAIL_BUTTONS_IMG),
        16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);

    if (imageList) {
        result = taskbarList->ThumbBarSetImageList(hwnd, imageList);

        if (SUCCEEDED(result)) {
            thumb_buttons[0].dwMask = (THUMBBUTTONMASK)(THB_BITMAP | THB_TOOLTIP | THB_FLAGS);
            thumb_buttons[0].dwFlags = (THUMBBUTTONFLAGS)(THBF_ENABLED);
            thumb_buttons[0].iId = ID_THUMBNAIL_BUTTON1;
            thumb_buttons[0].iBitmap = 0;
            wcscpy(thumb_buttons[0].szTip, L"Previous");

            thumb_buttons[1].dwMask = (THUMBBUTTONMASK)(THB_BITMAP | THB_TOOLTIP | THB_FLAGS);
            thumb_buttons[1].dwFlags = (THUMBBUTTONFLAGS)(THBF_ENABLED);
            thumb_buttons[1].iId = ID_THUMBNAIL_BUTTON2;
            thumb_buttons[1].iBitmap = 1;
            wcscpy(thumb_buttons[1].szTip, L"Play");

            thumb_buttons[2].dwMask = (THUMBBUTTONMASK)(THB_BITMAP | THB_TOOLTIP | THB_FLAGS);
            thumb_buttons[2].dwFlags = (THUMBBUTTONFLAGS)(THBF_ENABLED);
            thumb_buttons[2].iId = ID_THUMBNAIL_BUTTON3;
            thumb_buttons[2].iBitmap = 2;
            wcscpy(thumb_buttons[2].szTip, L"Next");

            taskbarList->ThumbBarAddButtons(hwnd, ARRAYSIZE(thumb_buttons), thumb_buttons);
        }

        ImageList_Destroy(imageList);
    }
}

/**
* Updates the thumbnail toolbar buttons
*/
void win_specific_update_thumbnail_toolbar(string command)
{
    //Not on Windows 7
    if(taskbarList == NULL) {
        return;
    }

    if(strcmp(command.c_str(), "play") == 0) {
        thumb_buttons[1].iBitmap = 1;
        wcscpy(thumb_buttons[1].szTip, L"Play");
    } else if(strcmp(command.c_str(), "pause") == 0) {
        thumb_buttons[1].iBitmap = 3;
        wcscpy(thumb_buttons[1].szTip, L"Pause");
    }

    taskbarList->ThumbBarUpdateButtons(hwnd, ARRAYSIZE(thumb_buttons), thumb_buttons);
}

/**
* Handles the keyboard hook events
*/
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Static flag that indicates that a key is still pressed
    static bool keyPressed = false;

    // MS Keyboard Event Struct Data
    KBDLLHOOKSTRUCT* kbhook = (KBDLLHOOKSTRUCT*) lParam;

    // On Key Release
    if(keyPressed && (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)) {
        switch((unsigned int) kbhook->vkCode) {
        case VK_MEDIA_PLAY_PAUSE:
        case VK_MEDIA_STOP:
        case VK_MEDIA_NEXT_TRACK:
        case VK_MEDIA_PREV_TRACK:
            keyPressed = false;
            break;
        }
    }

    // On Key Press
    if(!keyPressed && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        switch((unsigned int) kbhook->vkCode) {
        case VK_MEDIA_PLAY_PAUSE:
            window_main_toggle_play();
            keyPressed = true;
            break;
        case VK_MEDIA_STOP:
            window_main_stop();
            keyPressed = true;
            break;
        case VK_MEDIA_NEXT_TRACK:
            window_main_next();
            keyPressed = true;
            break;
        case VK_MEDIA_PREV_TRACK:
            window_main_previous();
            keyPressed = true;
            break;
        }
    }

    return CallNextHookEx(handlerKeyboardHook, nCode, wParam, lParam);
}

void os_specific_scanfolder(const wchar_t* dir, deque<wstring>& filelist)
{
    WIN32_FIND_DATAW filedata;
    HANDLE hFind = NULL;
    wchar_t path[4096];
    wchar_t ext[50];

    wsprintfW(path, L"%s\\*.*", dir);

    // Check if it's a valid path
    if((hFind = FindFirstFileW(path, &filedata)) == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        // Ignore "." and ".."
        if(wcscmp(filedata.cFileName, L".") == 0 || wcscmp(filedata.cFileName, L"..") == 0) {
            continue;
        }

        // Join path & filename
        wsprintfW(path, L"%s\\%s", dir, filedata.cFileName);

        // If it's a folder, search it too
        if(filedata.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) {
            os_specific_scanfolder(path, filelist);
            continue;
        }
        _wsplitpath(path, NULL, NULL, NULL, ext);

        if(wcscmp(ext, L".mp3") == 0 ||
            wcscmp(ext, L".wma") == 0 ||
            wcscmp(ext, L".ogg") == 0 ||
            wcscmp(ext, L".wav") == 0 ||
            wcscmp(ext, L".flac") == 0) {
                filelist.push_back(path);
        }
    }
    while(FindNextFileW(hFind, &filedata));

    FindClose(hFind);
    return;
}
