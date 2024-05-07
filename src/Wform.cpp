#define STRICT
#include <Windows.h>

#include <tchar.h>

#include "WForm.h"

#define TRAY_ICON_CALLBACKMESSAGE WM_USER + 0x7FFF

WForm* WForm::instance = NULL;

WForm::WForm()
{
	instance = this;
}

WForm::~WForm()
{
	RemoveClassPointer();
}

void WForm::Create(LPCTSTR lpszClassName, HICON hIcon, HICON hIconSm, HCURSOR hCursor,
	HBRUSH hbrBackground, LPCTSTR lpszMenuName, DWORD dwExStyle,
	LPCTSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HINSTANCE hInstance)
{

	instance = this;
	// 1. register new class

	WNDCLASSEX wincl;

	if (!hInstance)
		wincl.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	else
		wincl.hInstance = (HINSTANCE)hInstance;

	wincl.lpszClassName = lpszClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);

	wincl.hIcon = hIcon;
	wincl.hIconSm = hIconSm;
	wincl.hCursor = hCursor;
	wincl.lpszMenuName = lpszMenuName;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = hbrBackground;

	if (!RegisterClassEx(&wincl))
	{
		throw "Can't register class";
	}

	// 2. create window
	Handle = CreateWindowEx(dwExStyle, lpszClassName, lpWindowName, dwStyle,
		x,
		y,
		nWidth,
		nHeight,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL);

	if (!Handle)
		throw "Can't create window";
}

LRESULT CALLBACK WForm::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	WForm* form = (WForm*)GetClassPointer(hwnd);
	if (form == NULL)
	{
		form = instance;
	}

	form->formMessage.hwnd = hwnd;
	form->formMessage.message = msg;
	form->formMessage.wParam = wParam;
	form->formMessage.lParam = lParam;

	switch (form->OnMessage(hwnd, msg, wParam, lParam, form->_return))
	{
	case -1:
		return DefWindowProc(hwnd, msg, wParam, lParam);

	case 1:
		return form->_return;
	}

	switch (msg)
	{

	case WM_CONTEXTMENU:
		return form->OnContextMenu(LOWORD(lParam), HIWORD(lParam), (HWND)wParam);

	case WM_DEVICECHANGE:
		return form->OnDeviceChange((UINT)wParam, lParam);

	case WM_KEYDOWN:
		return form->OnKeyDown((int)wParam, (int)lParam);

	case WM_KEYUP:
		return form->OnKeyUp((int)wParam, (int)lParam);

	case WM_CANCELMODE:
		ReleaseCapture();
		return 0;

	case TRAY_ICON_CALLBACKMESSAGE:
		return form->OnTrayIconNotify(lParam, wParam);

	case WM_TIMER:
		return form->OnTimer(wParam);

	case WM_SETFOCUS:
		return form->OnActivate();

	case WM_KILLFOCUS:
		return form->OnDeactivate();

		/*
		case WM_ACTIVATE:
			MessageBeep(0);
		return 0;
		  */

	case WM_CREATE: {
		form->Handle = hwnd;
		int ret = form->OnCreate();
		if (ret >= 0)
		{
			form->SetClassPointer(hwnd, (HANDLE)form);
		}
		return ret;
	}

	case WM_PAINT:
		return form->OnPaint();

	case WM_HSCROLL:
		return form->OnHScroll((DWORD)GetWindowLongPtr((HWND)lParam, GWLP_ID), (int)LOWORD(wParam), int HIWORD(wParam), (HWND)lParam);

	case WM_VSCROLL:
		return form->OnVScroll((DWORD)GetWindowLongPtr((HWND)lParam, GWLP_ID), (int)LOWORD(wParam), int HIWORD(wParam), (HWND)lParam);

	case WM_COMMAND:
		return form->OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		return form->OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
		return form->OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));

	case WM_RBUTTONUP:
		DefWindowProc(hwnd, msg, wParam, lParam);
		return form->OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));

	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		return form->OnDblClick(wParam, LOWORD(lParam), HIWORD(lParam));

	case WM_MOUSEMOVE:
		return form->OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));

	case WM_DESTROY:
		form->RemoveClassPointer();
		return form->OnDestroy();

	default: /* for messages that we don't deal with */
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WForm::OnDblClick(DWORD fwKeys, int X, int Y)
{
	return DefProc();
}

LRESULT WForm::OnMouseDown(DWORD fwKeys, int X, int Y)
{
	return DefProc();
}

LRESULT WForm::OnMouseUp(DWORD fwKeys, int X, int Y)
{
	return DefProc();
}

LRESULT WForm::OnMouseMove(DWORD fwKeys, int X, int Y)
{
	return DefProc();
}

LRESULT WForm::OnCreate()
{
	return DefProc();
}

LRESULT WForm::OnPaint()
{
	return DefProc();
}

LRESULT WForm::OnHScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle)
{
	return DefProc();
}

LRESULT WForm::OnVScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle)
{
	return DefProc();
}

LRESULT WForm::OnButtonClick(int ButtonID, void* Sender)
{
	return DefProc();
}

LRESULT WForm::OnDestroy()
{
	return DefProc();
}

LRESULT WForm::OnTrayIconNotify(DWORD Message, UINT iconID)
{
	return DefProc();
}

LRESULT WForm::OnTimer(UINT idTimer)
{
	return DefProc();
}

BOOL WForm::SetTimer(UINT idTimer, UINT uTimeout)
{
	return ::SetTimer(Handle, idTimer, uTimeout, (TIMERPROC)NULL);
}

BOOL WForm::KillTimer(UINT idTimer)
{
	return ::KillTimer(Handle, idTimer);
}

LRESULT WForm::OnCommand(int wNotifyCode, int ControlID, HWND hWndControl)
{
	return DefProc();
}

LRESULT WForm::OnActivate()
{
	return DefProc();
}
LRESULT WForm::OnDeactivate()
{
	return DefProc();
}

LRESULT WForm::OnKeyDown(int VirtKey, int KeyData)
{
	return DefProc();
}

LRESULT WForm::OnKeyUp(int VirtKey, int KeyData)
{
	return DefProc();
}

LRESULT WForm::OnDeviceChange(UINT Event, DWORD Data)
{
	return DefProc();
}

LRESULT WForm::DefProc()
{
	return DefWindowProc(formMessage.hwnd, formMessage.message,
		formMessage.wParam, formMessage.lParam);
}

void WForm::MinimizeToTray(UINT uID, HICON hIcon, LPSTR lpszTip)
{

	NOTIFYICONDATA tnid;
	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = Handle;

	tnid.uID = uID;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = TRAY_ICON_CALLBACKMESSAGE;
	tnid.hIcon = hIcon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
		tnid.szTip[0] = '\0';

	Shell_NotifyIcon(NIM_ADD, &tnid);
	if (hIcon)
		DestroyIcon(hIcon);

	Show(FALSE);
}

void WForm::RestoreFromTray(UINT uID)
{

	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = Handle;
	tnid.uID = uID;

	Shell_NotifyIcon(NIM_DELETE, &tnid);

	Show(TRUE);
}

BOOL WForm::TaskBarAddIcon(UINT uID, HICON hicon, LPSTR lpszTip)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = Handle;

	tnid.uID = uID;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = TRAY_ICON_CALLBACKMESSAGE;
	tnid.hIcon = hicon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
		tnid.szTip[0] = '\0';

	res = Shell_NotifyIcon(NIM_ADD, &tnid);

	if (hicon)
		DestroyIcon(hicon);

	return res;
}

BOOL WForm::TaskBarDeleteIcon(UINT uID)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = Handle;
	tnid.uID = uID;

	res = Shell_NotifyIcon(NIM_DELETE, &tnid);
	return res;
}

LRESULT WForm::OnContextMenu(int X, int Y, HWND Handle)
{
	return DefProc();
}

int WForm::OnMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD& dwReturn)
{

	return 0;
}