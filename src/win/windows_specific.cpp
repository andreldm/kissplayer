/**
 *Here lives the Windows specific code.
 */

#include "windows_specific.h"
#include <FL/x.H>

// Keyboard hook handler
HHOOK handleKeyboardHook = NULL;

// Keyboard hook callback
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

/**
* Sets the window icon.
*/
void set_app_icon(Fl_Window *window)
{
    window->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(APP_ICON)));
}

/**
* Starts Windows specific configuration.
*/
void init_os_specific(Fl_Window *window)
{
    Fl::add_handler(windows_event_handler);
    taskBarCreatedId = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

    // Setup the low level keyboard hook
    HINSTANCE hHandle = GetModuleHandle(NULL);
    handleKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hHandle, 0);
    if (handleKeyboardHook == NULL)
        cout << "SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hHandle, 0) failed\n";
}

/**
* Executes the necessary clean up.
*/
void end_os_specific()
{
    // On Windows 7
    if(taskbarList != NULL)
        taskbarList->Release();

    // Unhook the low level keyboard hook
    UnhookWindowsHookEx(handleKeyboardHook);
}
/**
* Get the executable directory
*/
string getWorkingDirectory()
{
    TCHAR szpath[FILENAME_MAX];
    GetModuleFileName (NULL, szpath, FILENAME_MAX);
    string path(szpath);

    size_t lastSlash = path.find_last_of("/\\");
    path = path.substr(0,lastSlash+1); //include slash

    return path;
}

/**
* Calls native directory chooser.
*/
TCHAR * native_dir_chooser()
{
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = TEXT("Select a folder:");
	TCHAR *path = new TCHAR[1024];
    LPITEMIDLIST pidl = SHBrowseForFolder (&bi);
	SHGetPathFromIDList(pidl, path);
	return path;
}

/**
 * Set the window to maximized state.
 */
void maximizeWindow(Fl_Window *window)
{
	SendMessage(fl_xid(window), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	//UpdateWindow(fl_xid(window));
}

/**
 * Check if the window is maximized.
 */
bool isWindowMaximized(Fl_Window *window)
{
	return IsZoomed(fl_xid(window));
}

/**
* Custom event handler.
*/
int windows_event_handler(int e)
{
	if(fl_msg.message == taskBarCreatedId)
    {
        create_thumbnail_toolbar(fl_msg.hwnd);
        return 1;
    }
    if(fl_msg.message == WM_COMMAND)
    {
        if(LOWORD(fl_msg.wParam) == ID_THUMBNAIL_BUTTON1)
        {
            cb_previous(NULL, 0);
            return 1;
        }
        if(LOWORD(fl_msg.wParam) == ID_THUMBNAIL_BUTTON2)
        {
            cb_toggle_play(NULL, 0);
            return 1;
        }
        if(LOWORD(fl_msg.wParam) == ID_THUMBNAIL_BUTTON3)
        {
            cb_next(NULL, 0);
            return 1;
        }
    }
    return 0;
}
/**
* Creates the thumbnail toolbar.
* Called after windows registers the toolbar successfully, which happens only on Win7.
*/
void create_thumbnail_toolbar(HWND hwnd)
{
    __hwnd = hwnd;
	HRESULT result = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
        reinterpret_cast<void**> (&(taskbarList)));

	if (SUCCEEDED(result))
	{
		result = taskbarList->HrInit();

		if (FAILED(result))
		{
			taskbarList->Release();
			taskbarList = NULL;
		}
	}

	HIMAGELIST imageList = ImageList_LoadImage(fl_display, MAKEINTRESOURCE(ID_THUMBNAIL_BUTTONS_IMG),
		16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);

    if (imageList)
	{
		result = taskbarList->ThumbBarSetImageList(__hwnd, imageList);
		if (SUCCEEDED(result))
		{
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

			taskbarList->ThumbBarAddButtons(__hwnd, ARRAYSIZE(thumb_buttons), thumb_buttons);
		}

		ImageList_Destroy(imageList);
	}
}

/**
* Updates the thumbnail toolbar buttons
*/
void update_thumbnail_toolbar(string command)
{
    //Not on Windows 7
    if(taskbarList == NULL)
        return;

    if(strcmp(command.c_str(), "play") == 0)
    {
        thumb_buttons[1].iBitmap = 1;
        wcscpy(thumb_buttons[1].szTip, L"Play");
    }
    else if(strcmp(command.c_str(), "pause") == 0)
    {
        thumb_buttons[1].iBitmap = 3;
        wcscpy(thumb_buttons[1].szTip, L"Pause");
    }
    taskbarList->ThumbBarUpdateButtons(__hwnd, ARRAYSIZE(thumb_buttons), thumb_buttons);
}

/**
* Handles the keyboard hook events
*/
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Static flag that indicates that a key is still pressed
    static bool keyPressed = false;

    // MS Keyboard Event Struct Data
    KBDLLHOOKSTRUCT * kbhook = (KBDLLHOOKSTRUCT *) lParam;

    // On Key Release
    if(keyPressed && (wParam == WM_KEYUP || wParam == WM_SYSKEYUP))
    {
        switch((unsigned int) kbhook->vkCode)
        {
            case VK_MEDIA_PLAY_PAUSE:
            case VK_MEDIA_STOP:
            case VK_MEDIA_NEXT_TRACK:
            case VK_MEDIA_PREV_TRACK:
                keyPressed = false;
                break;
        }
    }

    // On Key Press
    if(!keyPressed && (wParam == WM_KEYDOWN|| wParam == WM_SYSKEYDOWN))
    {
        switch((unsigned int) kbhook->vkCode)
        {
            case VK_MEDIA_PLAY_PAUSE:
                cb_toggle_play(NULL, 0);
                keyPressed = true;
                break;
            case VK_MEDIA_STOP:
                cb_stop(NULL, 0);
                keyPressed = true;
                break;
            case VK_MEDIA_NEXT_TRACK:
                cb_next(NULL, 0);
                keyPressed = true;
                break;
            case VK_MEDIA_PREV_TRACK:
                cb_previous(NULL, 0);
                keyPressed = true;
                break;
        }
    }

    return CallNextHookEx(handleKeyboardHook, nCode, wParam, lParam);
}

/**
* Converts a string from a given encoding to UTF-8
* UTF-8 is code page: 65001
* CP-1252 is code page: 1252 :)
* Credits: http://www.chilkatsoft.com/p/p_348.asp
*/
wchar_t *CodePageToUnicode(int codePage, const char *src)
{
    if (!src) return 0;
    int srcLen = strlen(src);
    if (!srcLen)
    {
        wchar_t *w = new wchar_t[1];
        w[0] = 0;
        return w;
    }

    int requiredSize = MultiByteToWideChar(codePage, 0, src, srcLen, 0, 0);

    if (!requiredSize)
    {
        return 0;
    }

    wchar_t *w = new wchar_t[requiredSize+1];
    w[requiredSize] = 0;

    int retval = MultiByteToWideChar(codePage, 0, src, srcLen, w, requiredSize);
    if (!retval)
    {
        delete [] w;
        return 0;
    }

    return w;
}

/**
* Converts a string from UTF-8 to a given encoding
* UTF-8 is code page: 65001
* CP-1252 is code page: 1252 :)
* Credits: http://www.chilkatsoft.com/p/p_348.asp
*/
char *UnicodeToCodePage(int codePage, const wchar_t *src)
{
    if (!src) return 0;
    int srcLen = wcslen(src);
    if (!srcLen)
    {
        char *x = new char[1];
        x[0] = '\0';
        return x;
    }

    int requiredSize = WideCharToMultiByte(codePage, 0, src,srcLen, 0, 0, 0, 0);

    if (!requiredSize)
    {
        return 0;
    }

    char *x = new char[requiredSize+1];
    x[requiredSize] = 0;

    int retval = WideCharToMultiByte(codePage, 0, src, srcLen, x, requiredSize, 0, 0);
    if (!retval)
    {
        delete [] x;
        return 0;
    }

    return x;
}
