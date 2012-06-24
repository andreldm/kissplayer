/**
 *Here lives the windows specific code.
 */

#include "windows_specific.h"

void set_app_icon(Fl_Window *window)
{
    window->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(APP_ICON)));
}

//Not necessary on Windows
void set_app_icon_transparency(Fl_Window *window)
{

}

void init_os_specific()
{
    Fl::add_handler(windows_event_handler);

    taskBarCreatedId = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));
}

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
			thumb_buttons[0].dwFlags = (THUMBBUTTONFLAGS)(THBF_ENABLED | THBF_DISMISSONCLICK);
			thumb_buttons[0].iId = ID_THUMBNAIL_BUTTON1;
			thumb_buttons[0].iBitmap = 0;
			wcscpy(thumb_buttons[0].szTip, L"Previous");

			thumb_buttons[1].dwMask = (THUMBBUTTONMASK)(THB_BITMAP | THB_TOOLTIP | THB_FLAGS);
			thumb_buttons[1].dwFlags = (THUMBBUTTONFLAGS)(THBF_ENABLED | THBF_DISMISSONCLICK);
			thumb_buttons[1].iId = ID_THUMBNAIL_BUTTON2;
			thumb_buttons[1].iBitmap = 1;
			wcscpy(thumb_buttons[1].szTip, L"Play");

			thumb_buttons[2].dwMask = (THUMBBUTTONMASK)(THB_BITMAP | THB_TOOLTIP | THB_FLAGS);
			thumb_buttons[2].dwFlags = (THUMBBUTTONFLAGS)(THBF_ENABLED | THBF_DISMISSONCLICK);
			thumb_buttons[2].iId = ID_THUMBNAIL_BUTTON3;
			thumb_buttons[2].iBitmap = 2;
			wcscpy(thumb_buttons[2].szTip, L"Next");

			taskbarList->ThumbBarAddButtons(__hwnd, ARRAYSIZE(thumb_buttons), thumb_buttons);
		}
		//ImageList_Destroy(imageList);
	}
	//taskbarList->Release();
}

void update_thumbnail_toolbar(string command)
{
    if(strcmp(command.c_str(), "pause"))
    {
        thumb_buttons[1].iBitmap = 1;
        wcscpy(thumb_buttons[1].szTip, L"Play");
    }
    else if(strcmp(command.c_str(), "play"))
    {
        thumb_buttons[1].iBitmap = 3;
        wcscpy(thumb_buttons[1].szTip, L"Pause");
    }
    taskbarList->ThumbBarUpdateButtons(__hwnd, ARRAYSIZE(thumb_buttons), thumb_buttons);
}
