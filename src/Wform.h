// WForm class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004

#ifndef _WFORM_Z_
#define _WFORM_Z_

#include "WControl.h"
#include "WApp.h"

class WForm : public WControl
{
public:
	// variable
	static WForm* instance;

	// function
	WForm();
	~WForm();
	void Create(LPCTSTR lpszClassName, HICON hIcon, HICON hIconSm, HCURSOR hCursor,
		HBRUSH hbrBackground, LPCTSTR lpszMenuName, DWORD dwExStyle,
		LPCTSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight, HINSTANCE hInstance = NULL);

	void MinimizeToTray(UINT uID, HICON hIcon, LPSTR lpszTip);
	void RestoreFromTray(UINT uID);
	BOOL TaskBarAddIcon(UINT uID, HICON hicon, LPSTR lpszTip);
	BOOL TaskBarDeleteIcon(UINT uID);

	LRESULT DefProc();

	virtual int OnMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD& dwReturn);

	virtual LRESULT OnCreate();
	virtual LRESULT OnMouseMove(DWORD fwKeys, int X, int Y);
	virtual LRESULT OnMouseDown(DWORD fwKeys, int X, int Y);
	virtual LRESULT OnMouseUp(DWORD fwKeys, int X, int Y);
	virtual LRESULT OnKeyDown(int VirtKey, int KeyData);
	virtual LRESULT OnKeyUp(int VirtKey, int KeyData);
	virtual LRESULT OnDeviceChange(UINT Event, DWORD Data);
	virtual LRESULT OnActivate();
	virtual LRESULT OnDeactivate();
	virtual LRESULT OnTrayIconNotify(DWORD Message, UINT iconID);
	virtual LRESULT OnDblClick(DWORD fwKeys, int X, int Y);
	virtual LRESULT OnDestroy();
	virtual LRESULT OnCommand(int wNotifyCode, int ControlID, HWND hWndControl);
	virtual LRESULT OnButtonClick(int ButtonID, void* Sender);
	virtual LRESULT OnPaint();
	virtual LRESULT OnTimer(UINT idTimer);
	virtual LRESULT OnHScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle);
	virtual LRESULT OnVScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle);
	virtual LRESULT OnContextMenu(int X, int Y, HWND Handle);

	BOOL SetTimer(UINT idTimer, UINT uTimeout);
	BOOL KillTimer(UINT idTimer);

private:
	// variable
	MSG formMessage;

	// function
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	DWORD _return;
};

#endif
