// WBmpButton class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2007

#ifndef _WBMPBUTTONEX_Z_
#define _WBMPBUTTONEX_Z_


#include "wcontrol.h"


#define PUSH_BUTTON 1
#define CHECK_BUTTON 2



class WBmpButton : public WControl {

public:
// variable


// function
	WBmpButton();
    ~WBmpButton();


	 int Create(DWORD dwType,
				int nX,
				int nY,
				int nWidth,
				int nHeight,
				HWND hWndParent,
				UINT hMenu,
				HINSTANCE hInstance,
                HBITMAP hbitmap);
               


    void SetCheck(BOOL fCheck);
    BOOL GetCheck();
    void Redraw();

	void SetBitmap(HBITMAP hbitmap);


private:

    int _type;	// button type
    WNDPROC _oldproc;

	HBITMAP _hbitmap;


    int _state;
    BOOL _hover;
    BOOL _pushed;
    BOOL _checked;
	int _bmpWidth;
	int _bmpHeight;
	char* _caption;
	BOOL _checkbutton;





    static void MouseMove(WBmpButton *bt, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK _WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
     static void Draw(WBmpButton *bt, HDC hdc);
     static void ButtonDown(WBmpButton *bt);
    static void ButtonUp(WBmpButton *bt, LPARAM lParam);

};







#endif




