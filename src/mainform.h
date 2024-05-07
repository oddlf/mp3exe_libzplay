#ifndef _MAINFORM_
#define _MAINFORM_

#include "WForm.h"
#include "WBmpButton.h"
#include "WBmpTextBox.h"

#define INTER_APP_MESSAGE WM_USER + 1

#define MAIN_TIMER 500
#define VU_TIMER 501

#define MESSAGE_PLAY_EMBEDED 115
#define MESSAGE_PLAY_ARGUMENT 116
#define MESSAGE_PLAY_FILE 117

int StrToRect(char* str, RECT* rect);
int StrToRect(char* str, RECT* rc, int num);

extern int PlayTrack;

class MainForm : public WForm
{
public:
	int OnMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD& dwReturn);
	LRESULT OnMouseMove(DWORD fwKeys, int X, int Y);
	LRESULT OnCreate();
	LRESULT OnDestroy();
	LRESULT OnMouseDown(DWORD fwKeys, int X, int Y);
	LRESULT OnMouseUp(DWORD fwKeys, int X, int Y);
	LRESULT OnPaint();
	LRESULT OnTimer(UINT idTimer);
	LRESULT OnHScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle);
	LRESULT OnVScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle);
	LRESULT OnCommand(int wNotifyCode, int ControlID, HWND hWndControl);
	LRESULT OnActivate();
	LRESULT OnDeactivate();
	LRESULT OnKeyDown(int VirtKey, int KeyData);
	LRESULT OnKeyUp(int VirtKey, int KeyData);
	LRESULT OnTrayIconNotify(DWORD Message, UINT iconID);
	LRESULT OnContextMenu(int X, int Y, HWND Handle);

private:
	HBITMAP hbMain;
	HBITMAP hbTitlebar;

	WBmpButton* btSpectrumNext;
	WBmpButton* btSpectrumPrev;
	WBmpTextBox* tbSpectrumMode;

	WBmpButton* btLinear;

	WBmpButton* btFFTWindowNext;
	WBmpButton* btFFTWindowPrev;
	WBmpTextBox* tbFFTWindow;

	int c_nSpectrumType;
	int c_nSpectrumWindow;
};

#endif
