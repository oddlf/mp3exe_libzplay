// WScrollbar class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004


#ifndef _WSCROLLBAR_Z_
#define _WSCROLLBAR_Z_


#define HORZ 1
#define VERT 2



#include "wcontrol.h"



class WScrollbar : public WControl {
public:

    WScrollbar();
    ~WScrollbar();


	int Create( int nType,int nX, int nY, int nWidth, int nHeight,
						HWND hWndParent, DWORD hMenu, HINSTANCE hInstance, HBITMAP hBg, HBITMAP hThumb );

  
    void SetPos(int pos, BOOL fRedraw = TRUE);
    void SetRange(int max, BOOL fRedraw = TRUE);

 
    int GetPos();


    void Redraw();

private:
	HBITMAP _hbg;
	HBITMAP _hthumb;
	BOOL _horz;
	WNDPROC _oldproc;
	int _interpos;
	int _thumbsize;
	int _thumbstate;
	BOOL _hover;
	BOOL _pushed;
	DWORD _indent;
	int _max;
	int _oldpos;

	static LRESULT CALLBACK _WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	static void Draw(WScrollbar *control, HDC hdc);
	static void MouseMoveSC(WScrollbar *control, WPARAM wParam, LPARAM lParam);
    static void GetThumbRect(WScrollbar* control, RECT* rcthumb);
	static BOOL IsPointOnThumb(WScrollbar* control, POINT* pt);
	static void InvalidateThumb(WScrollbar* control);
	


    static void ButtonDownSC(WScrollbar *sc, LPARAM lParam);
    static void ButtonUpSC(WScrollbar *sc, LPARAM lParam);



};






#endif
