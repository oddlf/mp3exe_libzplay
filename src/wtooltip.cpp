#define STRICT
#include <Windows.h>
#include <CommCtrl.h>
#include <cstring>

#include "WTooltip.h"

TCHAR WTooltip::PropTooltipStr[] = _T("TOOLTIP_CLASS_POINTER");
UINT WTooltip::toolNumber = 0; // initialize static member of class

HHOOK WTooltip::x_hhk = (HHOOK)NULL; // initialize static member of class

WTooltip::WTooltip(HWND hwnd)
{

	InitCommonControls();
	x_hwnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, (LPSTR)NULL,
		TTS_ALWAYSTIP | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, hwnd, (HMENU)NULL, GetModuleHandle(NULL), NULL);

	if (!x_hhk) // prevent multiple windows hooks
		x_hhk = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)&GetMsgProc,
			(HINSTANCE)NULL, GetCurrentThreadId());

	Handle = x_hwnd;
	toolNumber = 0;
}

WTooltip::~WTooltip()
{
	UnhookWindowsHookEx(x_hhk);
}

WTooltip* WTooltip::GetTooltip(HWND hwnd)
{
	return (WTooltip*)GetProp(hwnd, PropTooltipStr);
}

void WTooltip::Activate(BOOL fActivate)
{
	SendMessage(x_hwnd, TTM_ACTIVATE, (WPARAM)fActivate, 0);
}

void WTooltip::SetDelayTime(int iDelay)
{
	SendMessage(x_hwnd, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOMATIC, (LPARAM)iDelay);
}

void WTooltip::SetDelayTime(int iInitial, int iAutopop, int iReshow)
{
	SendMessage(x_hwnd, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOPOP, (LPARAM)iAutopop);
	SendMessage(x_hwnd, TTM_SETDELAYTIME, (WPARAM)TTDT_INITIAL, (LPARAM)iInitial);
	SendMessage(x_hwnd, TTM_SETDELAYTIME, (WPARAM)TTDT_RESHOW, (LPARAM)iReshow);
}

// GetMsgProc - monitors the message stream for mouse messages intended
//     for a control window in the dialog box.
// Returns a message-dependent value.
// nCode - hook code
// wParam - message flag (not used)

// lParam - address of an MSG structure
LRESULT CALLBACK WTooltip::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode < 0)
		return (CallNextHookEx(x_hhk, nCode, wParam, lParam));

	MSG* lpmsg;
	lpmsg = (MSG*)lParam;

	WTooltip* Tooltip;

	switch (lpmsg->message)
	{

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:

		Tooltip = GetTooltip(lpmsg->hwnd);

		if (Tooltip == NULL)
		{

			POINT pt;
			pt.x = LOWORD(lpmsg->lParam);
			pt.y = HIWORD(lpmsg->lParam);
			HWND hwnd = ChildWindowFromPoint(lpmsg->hwnd, pt);
			Tooltip = GetTooltip(hwnd);
			if (Tooltip == NULL)
				return (CallNextHookEx(x_hhk, nCode, wParam, lParam));
		}

		MSG msg;

		msg.lParam = lpmsg->lParam;
		msg.wParam = lpmsg->wParam;
		msg.message = lpmsg->message;
		msg.hwnd = lpmsg->hwnd;

		SendMessage(Tooltip->x_hwnd, TTM_RELAYEVENT, 0,
			(LPARAM)(LPMSG)&msg);

		break;

	default:
		break;
	}
	return (CallNextHookEx(x_hhk, nCode, wParam, lParam));
}

int WTooltip::AddTool(HWND hwnd, LPTSTR lpszText)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);

	toolNumber++;

	// for disabled control
	ti.uFlags = 0;
	ti.hwnd = GetParent(hwnd);
	ti.uId = toolNumber;

	ti.hinst = 0;
	GetClientRect(hwnd, &ti.rect);
	MapWindowPoints(hwnd, ti.hwnd, (POINT*)&ti.rect, 2);

	ti.lpszText = lpszText;

	SendMessage(Handle, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	// for enabled control

	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = 0;
	ti.uId = (UINT_PTR)hwnd;

	SendMessage(Handle, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	return toolNumber;
}

void WTooltip::DelTool(HWND hwnd, UINT uId)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);

	ti.uFlags = 0;
	ti.hwnd = GetParent(hwnd);
	ti.uId = uId;
	ti.hinst = 0;
	SendMessage(Handle, TTM_DELTOOL, 0,
		(LPARAM)(LPTOOLINFO)&ti);

	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = 0;
	ti.uId = (UINT_PTR)hwnd;

	SendMessage(Handle, TTM_DELTOOL, 0,
		(LPARAM)(LPTOOLINFO)&ti);
}

void WTooltip::UpdateToolRect(HWND hwnd, UINT uId)
{
	// for disabled control
	TOOLINFO ti;

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = 0;
	ti.hwnd = GetParent(hwnd);
	ti.uId = uId;
	ti.hinst = 0;
	GetClientRect(hwnd, &ti.rect);
	MapWindowPoints(hwnd, ti.hwnd, (POINT*)&ti.rect, 2);

	SendMessage(Handle, TTM_NEWTOOLRECT, 0,
		(LPARAM)(LPTOOLINFO)&ti);
}

void WTooltip::SetText(HWND hwnd, UINT uID, LPTSTR lpszText)
{

	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);

	// for disabled control
	ti.uFlags = 0;
	ti.hwnd = GetParent(hwnd);
	ti.uId = uID;

	ti.hinst = 0;
	GetClientRect(hwnd, &ti.rect);
	MapWindowPoints(hwnd, ti.hwnd, (POINT*)&ti.rect, 2);

	ti.lpszText = lpszText;

	SendMessage(Handle, TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&ti);

	// for enabled control

	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = 0;
	ti.uId = (UINT_PTR)hwnd;

	SendMessage(Handle, TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&ti);
}
