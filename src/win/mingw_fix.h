/**
Well, it was tricky to accomplish the Thumbnail Toolbar Buttons due MinGW slow development pace.
At the beginning, I was able to compile a simple test on MSVC, but I wish to keep it MinGW compatible.
Then I tried MinGW64, after a whole night of hard work, extensive search and this blog post:
	nicug.blogspot.com.br/2011/03/windows-7-taskbar-extensions-in-qt.html
Finally I could build the Thumbnail Buttons Test.
But I wasn't ready to change my toolchain and rebuild lots of libs, so I managed to pick the missings
definitions of the original MinGW.

The code below is unnecessary when using MinGW64, just include the "shlobj.h";
**/

#ifndef mingw_fix_h
#define mingw_fix_h

#include <windows.h>
#include <commctrl.h>

const GUID IID_ITaskbarList3 = {0xea1afb91, 0x9e28, 0x4b86, 0x90,0xe9, 0x9e,0x9f,0x8a,0x5e,0xef,0xaf};
const GUID CLSID_TaskbarList = {0x56fdf344,0xfd6d,0x11d0,0x95,0x8a,0x0,0x60,0x97,0xc9,0xa0,0x90};

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) \
	((sizeof(a) / sizeof(*(a))) / \
	static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

typedef enum TBPFLAG
{
	TBPF_NOPROGRESS = 0,
	TBPF_INDETERMINATE = 0x1,
	TBPF_NORMAL = 0x2,
	TBPF_ERROR = 0x4,
	TBPF_PAUSED = 0x8
} TBPFLAG;

typedef enum THUMBBUTTONFLAGS {
	THBF_ENABLED = 0x0,
	THBF_DISABLED = 0x1,
	THBF_DISMISSONCLICK = 0x2,
	THBF_NOBACKGROUND = 0x4,
	THBF_HIDDEN = 0x8,
	THBF_NONINTERACTIVE = 0x10
} THUMBBUTTONFLAGS;

typedef enum THUMBBUTTONMASK {
	THB_BITMAP = 0x1,
	THB_ICON = 0x2,
	THB_TOOLTIP = 0x4,
	THB_FLAGS = 0x8
} THUMBBUTTONMASK;

typedef struct THUMBBUTTON {
	THUMBBUTTONMASK dwMask;
	UINT iId;
	UINT iBitmap;
	HICON hIcon;
	WCHAR szTip[260];
	THUMBBUTTONFLAGS dwFlags;
} THUMBBUTTON;

typedef struct THUMBBUTTON *LPTHUMBBUTTON;

struct ITaskbarList : public IUnknown {
public:
	virtual HRESULT WINAPI HrInit(void) = 0;
	virtual HRESULT WINAPI AddTab(HWND hwnd) = 0;
	virtual HRESULT WINAPI DeleteTab(HWND hwnd) = 0;
	virtual HRESULT WINAPI ActivateTab(HWND hwnd) = 0;
	virtual HRESULT WINAPI SetActiveAlt(HWND hwnd) = 0;
};

struct ITaskbarList2 : public ITaskbarList {
public:
	virtual HRESULT WINAPI MarkFullscreenWindow(HWND hwnd,WINBOOL fFullscreen) = 0;
};

interface ITaskbarList3 : public ITaskbarList2
{
	virtual HRESULT STDMETHODCALLTYPE SetProgressValue(
	HWND hwnd,
	ULONGLONG ullCompleted,
	ULONGLONG ullTotal) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetProgressState(
	HWND hwnd,
	TBPFLAG tbpFlags) = 0;

	virtual HRESULT STDMETHODCALLTYPE RegisterTab(
	HWND hwndTab,
	HWND hwndMDI) = 0;

	virtual HRESULT STDMETHODCALLTYPE UnregisterTab(
	HWND hwndTab) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetTabOrder(
	HWND hwndTab,
	HWND hwndInsertBefore) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetTabActive(
	HWND hwndTab,
	HWND hwndMDI,
	DWORD dwReserved) = 0;

	virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons(
	HWND hwnd,
	UINT cButtons,
	LPTHUMBBUTTON pButton) = 0;

	virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons(
	HWND hwnd,
	UINT cButtons,
	LPTHUMBBUTTON pButton) = 0;

	virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList(
	HWND hwnd,
	HIMAGELIST himl) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon(
	HWND hwnd,
	HICON hIcon,
	LPCWSTR pszDescription) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip(
	HWND hwnd,
	LPCWSTR pszTip) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip(
	HWND hwnd,
	RECT *prcClip) = 0;
};

#endif
