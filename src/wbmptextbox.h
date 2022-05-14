// WBmpTextBox class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004


#ifndef _WBMPTEXTBOX_Z_
#define _WBMPTEXTBOX_Z_

#include "wcontrol.h"
#include "wbmpfont.h"



class WBmpTextBox : public WControl {
public:
	WBmpTextBox();
	~WBmpTextBox();
	int Create(char* text,
				int nX,
				int nY,
				int nWidth,
				int nHeight,
				HWND hWndParent,
				UINT hMenu,
				HINSTANCE hInstance,
                HBITMAP hBgBitmap,
				WBmpFont* font);

	int SetText(char* text, BOOL fRedraw);
	int SetScrollTextDelimiter(char* delimiter);
	void EnableScroll(BOOL Enable,DWORD dwTimeout);


private:
	WBmpFont* _font;
	HBITMAP _hbBg;
	WNDPROC _oldproc;
	char* _lpText;
	int _nTextLen;
	UINT _timer_timeout;
	BOOL _enable_scroll;
	char* _scroll_text_delimiter;
	char* _scroll_text;


	static LRESULT CALLBACK _WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    static void Draw(WBmpTextBox *control, HDC hdc);
	



protected:








};


#endif
