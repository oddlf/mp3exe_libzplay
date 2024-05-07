// WTooltip class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004

#ifndef _WTOOLTIP_Z_
#define _WTOOLTIP_Z_

#include <tchar.h>

class WTooltip
{
public:
	WTooltip();

	static TCHAR PropTooltipStr[];
	static UINT toolNumber;
	HWND x_hwnd;
	HWND Handle;
	static HHOOK x_hhk;
	WTooltip(HWND hwnd);   // constructor
	~WTooltip(); // destructor
	void SetDelayTime(int iDelay);
	void SetDelayTime(int iInitial, int iAutopop, int iReshow);
	void Activate(BOOL fActivate); // activate/deactivate tooltip control
	int AddTool(HWND hwnd, LPTSTR lpszText);
	void DelTool(HWND hwnd, UINT uId);
	void SetText(HWND hwnd, UINT uID, LPTSTR lpszText);
	void UpdateToolRect(HWND hwnd, UINT uId);

	static WTooltip* GetTooltip(HWND hwnd); // get tooltip hwnd for control
	static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif
