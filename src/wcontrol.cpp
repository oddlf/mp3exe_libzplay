// WControl class
//
// Version 	1.0
// Date:    10/05/2004
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr

#define STRICT
#include <Windows.h>

#include <cstdio>
#include <CommCtrl.h>

#include "WControl.h"

TCHAR WControl::ClassPointer[] = _T("W_CLASS_POINTER");

WControl::WControl()
{
	Tooltip = NULL;
	ToolID = 0;

	//_error = new WError();
}

WControl::~WControl()
{
	if (Tooltip)
		RemoveProp(Handle, Tooltip->PropTooltipStr);

	// delete _error;
}

BOOL WControl::SetClassPointer(HWND hwnd, HANDLE handle)
{
	return SetProp(hwnd, ClassPointer, handle);
}

void WControl::AlwaysOnTop(BOOL bOnTop)
{
	if (bOnTop)
		::SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	else
		::SetWindowPos(Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void* WControl::GetClassPointer(HWND hwnd)
{
	return GetProp(hwnd, ClassPointer);
}

BOOL WControl::AddTooltip(WTooltip* lpTooltip, LPTSTR lpszText)
{

	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);

	if (Tooltip != NULL)
	{
		Tooltip->DelTool(Handle, ToolID);

		RemoveProp(Handle, Tooltip->PropTooltipStr);
		Tooltip = NULL;
	}

	Tooltip = lpTooltip;

	ToolID = Tooltip->AddTool(Handle, lpszText);

	SetProp(Handle, Tooltip->PropTooltipStr, Tooltip);
	return TRUE;
}

void WControl::SetTooltipText(LPTSTR lpszText)
{
	if (Tooltip)
		Tooltip->SetText(Handle, ToolID, lpszText);
}

void WControl::SetPos(int nX, int nY, int nWidth, int nHeight, BOOL bRepaint)
{
	::MoveWindow(Handle, nX, nY, nWidth, nHeight, bRepaint);
	/*
	if(bRepaint)
		InvalidateRect(Handle,NULL,FALSE);
	*/
}

void WControl::SetPos(RECT* rcPos, BOOL bRepaint)
{
	::MoveWindow(Handle, rcPos->left, rcPos->top, rcPos->right, rcPos->bottom, FALSE);
	if (bRepaint)
		InvalidateRect(Handle, NULL, FALSE);
}

void WControl::UpdateToolRect()
{

	if (Tooltip)
		Tooltip->UpdateToolRect(Handle, ToolID);
}

void WControl::Enable(BOOL bEnable)
{
	EnableWindow(Handle, bEnable);
}

void WControl::Show(BOOL fShow)
{
	if (fShow)
		ShowWindow(Handle, SW_SHOW);
	else
		ShowWindow(Handle, SW_HIDE);
}

void WControl::ResizeTo(int nWidth, int nHeight, BOOL bRepaint)
{
	::SetWindowPos(Handle, NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE);
	if (bRepaint)
		InvalidateRect(Handle, NULL, FALSE);
}

void WControl::Close()
{
	SendMessage(Handle, WM_CLOSE, 0, 0);
}

void WControl::Destroy()
{
	DestroyWindow(Handle);
}

void WControl::SetClassPointer()
{
	SetProp(Handle, ClassPointer, (HANDLE)this);
}

void WControl::RemoveClassPointer()
{
	RemoveProp(Handle, ClassPointer);
}

void WControl::MoveTo(int nX, int nY)
{
	::SetWindowPos(Handle, NULL, nX, nY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WControl::Minimize()
{
	ShowWindow(Handle, SW_MINIMIZE);
	// CloseWindow(Handle);
}

BOOL WControl::IsVisible()
{
	return IsWindowVisible(Handle);
}

BOOL WControl::IsMinimized()
{
	return IsIconic(Handle);
}

void WControl::GetPos(RECT* rcPos)
{
	GetWindowRect(Handle, rcPos);
}

void WControl::GetClientPos(RECT* rcPos)
{
	GetClientRect(Handle, rcPos);
}

/*
BOOL WControl::SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags)
{
	return ::SetWindowPos(Handle, hwndInsertAfter, x, y, cx, cy, fuFlags);
}
*/

BOOL WControl::SetWindowPos(HWND hwndInsertAfter, RECT* rcPos, UINT fuFlags)
{
	return ::SetWindowPos(Handle, hwndInsertAfter, rcPos->left,
		rcPos->top, rcPos->right, rcPos->bottom, fuFlags);
}

BOOL WControl::MoveWindow(int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
	return ::MoveWindow(Handle, X, Y, nWidth, nHeight, bRepaint);
}

BOOL WControl::MoveWindow(RECT* rcPos, BOOL bRepaint)
{
	return ::MoveWindow(Handle, rcPos->left, rcPos->top, rcPos->right, rcPos->bottom, bRepaint);
}

void WControl::Redraw()
{
	InvalidateRect(Handle, NULL, TRUE);
}

DWORD WControl::SetFont(WFont* font, BOOL fRedraw)
{
	Font = font;
	return SendMessage(Handle, WM_SETFONT, (WPARAM)font->Handle, MAKELPARAM(fRedraw, 0));
}

void WControl::SetFocus()
{
	::SetFocus(Handle);
}

DWORD WControl::SendCommand(HWND hwnd, WORD NotificationCode, WORD Identifier, HWND hwndControl)
{
	return ::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(Identifier, NotificationCode), (LPARAM)hwndControl);
}

DWORD WControl::SetRedraw(BOOL fRedraw)
{
	return ::SendMessage(Handle, WM_SETREDRAW, (WPARAM)fRedraw, 0);
}

void BringWindowToForeground(HWND hwnd, BOOL fShow)
{
	HWND hForeground = GetForegroundWindow();
	DWORD piForeground;
	DWORD piMe;
	DWORD tForeground = GetWindowThreadProcessId(hForeground, &piForeground);
	DWORD tMe = GetWindowThreadProcessId(hwnd, &piMe);

	if (tForeground == tMe)
	{
		SetForegroundWindow(hwnd);
	}
	else
	{
		AttachThreadInput(tMe, tForeground, TRUE);
		SetForegroundWindow(hwnd);
		AttachThreadInput(tMe, tForeground, FALSE);
	}

	if (!fShow)
		return;

	if (IsIconic(hwnd))
	{
		ShowWindow(hwnd, SW_RESTORE);
	}
	else
	{
		ShowWindow(hwnd, SW_SHOW);
	}
}
