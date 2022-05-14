#define STRICT
#include <windows.h>

#pragma hdrstop


#include <commctrl.h>
#include <tchar.h>

#include "wscrollbar.h"

WScrollbar::WScrollbar()
{

	_max = 100;
	_interpos = 0;
	_thumbstate = 0;
	_hover = FALSE;
	_pushed = FALSE;
	_indent = 0;
	_oldpos = 0;
	
	


}

WScrollbar::~WScrollbar()
{
// restore original window procedure
	if(_oldproc) {
		SetWindowLong(Handle, GWL_WNDPROC,(LONG) _oldproc);
    }
	
	
	
	DestroyWindow(Handle);
	DeleteObject(_hbg);
	DeleteObject(_hthumb);

}




int WScrollbar::Create( int nType,int nX, int nY, int nWidth, int nHeight,
						HWND hWndParent, DWORD hMenu, HINSTANCE hInstance, HBITMAP hBg, HBITMAP hThumb )
{

	// copy bitmap into internal memory
	HDC hdcMem1, hdcMem2;
    HBITMAP hbmDest = 0;
	HBITMAP hbmDest1 = 0;
    BITMAP bm = {0};
  
	hdcMem1 = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

    GetObject(hBg, sizeof(BITMAP), &bm);
	
    hbmDest = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes,
			bm.bmBitsPixel,NULL);

    
    HBITMAP hbmOld1 = (HBITMAP)SelectObject(hdcMem1, hBg);
    HBITMAP hbmOld2 = (HBITMAP)SelectObject(hdcMem2, hbmDest);
    
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY);
  
    
	_hbg = hbmDest;

	GetObject(hThumb, sizeof(BITMAP), &bm);
	
	 hbmDest1 = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes,
			bm.bmBitsPixel,NULL);

	SelectObject(hdcMem1, hThumb);
    SelectObject(hdcMem2, hbmDest1);
    
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY);
  
    
	_hthumb = hbmDest1;

	SelectObject(hdcMem1, hbmOld1);
    SelectObject(hdcMem2, hbmOld2);

    DeleteDC(hdcMem1);
    DeleteDC(hdcMem2);

	if(nType == HORZ) {
		_horz = TRUE;
		_thumbsize = bm.bmWidth / 4;
	}
	else {
		_horz = FALSE;
		_thumbsize = bm.bmHeight;
	}

	DWORD style;

    
    if(!_horz)
    	style = WS_CHILD | WS_VISIBLE |SBS_VERT;
    else
    	style = WS_CHILD | WS_VISIBLE |SBS_HORZ;

	// create BUTTON control
	Handle = CreateWindowEx(0, "SCROLLBAR", "",
    	style,
        nX, nY, nWidth, nHeight, hWndParent, (HMENU)hMenu,
        hInstance, NULL);

	 SetWindowLong(Handle, GWL_USERDATA, (LONG) this);
		_oldproc = (WNDPROC)SetWindowLong(Handle,GWL_WNDPROC,(LONG) _WindowSubclassProc);


	return 1;
}






void WScrollbar::MouseMoveSC(WScrollbar *control, WPARAM wParam, LPARAM lParam)
{

	POINT pt;
	GetCursorPos(&pt);
	if(control->_pushed) {
		RECT rc;
		GetWindowRect(control->Handle, &rc);
		int pos;
		int min;
		int max;

		if(control->_horz) {
			pos = pt.x;
			min = rc.left;
			max = rc.right;
		}
		else {
			pos = pt.y;
			min = rc.top;
			max = rc.bottom;
		}

		control->_interpos = ( pos -  control->_indent);
		if(control->_interpos < 0) control->_interpos = 0;	
		if(control->_interpos > (max - min - control->_thumbsize)) control->_interpos = max - min - control->_thumbsize;
		HDC hdc = GetDC(control->Handle);
		Draw(control,hdc);
		ReleaseDC(control->Handle, hdc);

		WORD message;
		GetClientRect(control->Handle, &rc);
		int inter_range = 0;
		if(control->_horz) {
			message= WM_HSCROLL;
			inter_range = rc.right - control->_thumbsize;
		} 
		else {
    		message= WM_VSCROLL;
			inter_range = rc.bottom - control->_thumbsize;
		}

		
		pos = MulDiv(control->_interpos,control->_max,inter_range);
		if(control->_oldpos != pos) {

   			SendMessage(GetParent(control->Handle),message,
            			 MAKEWPARAM(SB_THUMBTRACK,
						 pos),
        	 			(LPARAM)control->Handle);


			control->_oldpos = pos;
		}



	}
	else {
    	if(IsPointOnThumb(control,&pt)) {
			if(!control->_hover) {
				SetCapture(control->Handle);
				InvalidateRect(control->Handle,NULL,FALSE);
				control->_hover = TRUE;
			}
		}
		else {
			if(control->_hover) {
				InvalidateRect(control->Handle,NULL,FALSE);
				control->_hover = FALSE;
				ReleaseCapture();
			}

		}
	}
}



int WScrollbar::GetPos()
{
	RECT rc;
	GetClientRect(Handle, &rc);
	int inter_range = 0;
	if(_horz) 
		inter_range = rc.right -_thumbsize;
	else 
		inter_range = rc.bottom - _thumbsize;


		
	return  MulDiv(_interpos, _max,inter_range);
	
}




void WScrollbar::ButtonDownSC(WScrollbar *control, LPARAM lParam)
{
	
    SetForegroundWindow(GetParent(control->Handle));
	SetCapture(control->Handle);
	RECT rc;
	GetWindowRect(control->Handle, &rc);

	POINT pt;
	int pos;
	int min;
	int max;
	GetCursorPos(&pt);

	if(control->_horz) {
		pos = pt.x;
		min = rc.left;
		max = rc.right;
	}
	else {
		pos = pt.y;
		min = rc.top;
		max = rc.bottom;
	}

	if(IsPointOnThumb(control, &pt) ) {
		control->_indent= pos - control->_interpos;	
	}
	else { // jump at new position
		control->_interpos = ( pos - min - control->_thumbsize / 2);
		if(control->_interpos < 0) control->_interpos = 0;	
		if(control->_interpos > (max - min - control->_thumbsize)) control->_interpos = max - min - control->_thumbsize;
		control->_indent=  min + control->_thumbsize / 2;
	}

	control->_pushed = TRUE;
	//InvalidateRect(control->Handle,NULL,FALSE);
	HDC hdc = GetDC(control->Handle);
	Draw(control,hdc);
	ReleaseDC(control->Handle, hdc);
	
	WORD message;
	GetClientRect(control->Handle, &rc);
	

	if(control->_horz) {
		message= WM_HSCROLL;
		max = rc.right - control->_thumbsize;
	} 
	else {
    	message= WM_VSCROLL;
		max = rc.bottom - control->_thumbsize;
	}

	pos = MulDiv(control->_interpos , control->_max , max);
   	SendMessage(GetParent(control->Handle),message,
            			 MAKEWPARAM(SB_THUMBTRACK,
						 pos),
        	 			(LPARAM)control->Handle);


	control->_oldpos = pos;

}



void WScrollbar::ButtonUpSC(WScrollbar *control, LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	
//	if(!IsPointOnThumb(control,&pt)) {
		control->_hover = FALSE;
		ReleaseCapture();
		
	//}
	
	//ReleaseCapture();
	control->_pushed = FALSE;
	RECT rc;
	GetClientRect(control->Handle, &rc);


	 WORD message;
	 int max;
    if(control->_horz) {
		message= WM_HSCROLL;
		max = rc.right - control->_thumbsize;
	} 
    else {
    	message= WM_VSCROLL;
		max = rc.bottom - control->_thumbsize;
	}

	int pos = MulDiv(control->_interpos,control->_max,max);


	control->_interpos = MulDiv(pos ,max , control->_max);
	if(control->_interpos < 0) control->_interpos = 0;

	HDC hdc = GetDC(control->Handle);
	Draw(control,hdc);
	ReleaseDC(control->Handle, hdc);


   	SendMessage(GetParent(control->Handle),message,
            		 MAKEWPARAM(SB_THUMBPOSITION,
					 pos),
        	 		(LPARAM)control->Handle);
	

}


void WScrollbar::SetRange(int max, BOOL fRedraw)
{
	_max = max;

    if (fRedraw)
    	InvalidateRect(Handle,NULL,FALSE);
}


void WScrollbar::SetPos(int pos, BOOL fRedraw)
{
	if(pos == 0) {
		_interpos = 0;
		if (fRedraw)
			InvalidateRect(Handle, NULL,FALSE);
		return;
	}
 
	RECT rc;
	GetClientRect(Handle, &rc);
	int _position = pos;
	if(_position < 0) _position = 0;
	if(_position > _max) _position = _max;

	int max;
	if(_horz)
		max = rc.right -  _thumbsize;
	else
		max = rc.bottom - _thumbsize;

	//_interpos = _position * max / _max;

	_interpos = MulDiv(_position ,max , _max);
//
    if (fRedraw)
    	InvalidateRect(Handle, NULL,FALSE);


}







LRESULT CALLBACK WScrollbar::_WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  	WScrollbar *control= (WScrollbar * )  GetWindowLong(hwnd, GWL_USERDATA	);
 
  	switch(uMsg)
   	{


       	case WM_PAINT:
        {
			
        	PAINTSTRUCT ps;
    		HDC hdc;
        	hdc = BeginPaint(hwnd, &ps);
           	Draw(control, hdc);
            EndPaint(hwnd,&ps);
			
        }
        return 0;

		case WM_MOUSEMOVE:
		   MouseMoveSC(control,wParam,lParam);
		return 0;

		 case WM_MOUSELEAVE:
        	control->_hover = FALSE;
			control->_thumbstate = 0;
            InvalidateRect(control->Handle, NULL, FALSE);
        return 0;

		case WM_LBUTTONDOWN:
   		   	ButtonDownSC(control,lParam);
   		return 0; //must block this message


		 case  WM_LBUTTONUP:
   		   	ButtonUpSC(control,lParam);
   		return 0; //must block this message

		 case WM_MBUTTONDOWN :

        return 0;

        case WM_MBUTTONUP:

        return 0;

        case WM_RBUTTONDOWN:

        return 0;

        case WM_RBUTTONUP:

        return 0;
   	

   		case  WM_LBUTTONDBLCLK:
        	PostMessage(hwnd,WM_LBUTTONDOWN,wParam,lParam);
   		return 0; //must block this message


        case WM_DESTROY:
    		SetWindowLong(control->Handle, GWL_WNDPROC,(LONG)control->_oldproc);


     }


    return CallWindowProc(control->_oldproc, hwnd, uMsg, wParam, lParam);

}




void WScrollbar::Draw(WScrollbar *control, HDC hdc)
{
	
	DWORD state = 0;
	if(control->_pushed)
		state = 1;
	else {
		POINT pt;
		GetCursorPos(&pt);	
		if(IsPointOnThumb(control,&pt))
			state = 2;
	}
		
	RECT rc;
	GetClientRect(control->Handle,&rc);



	HDC hdcdest = CreateCompatibleDC(hdc);
	HDC hdcsrc = CreateCompatibleDC(hdc);
	HBITMAP hbmdest = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hbold = (HBITMAP) SelectObject(hdcsrc,control->_hbg);
	HBITMAP hbold1 = (HBITMAP) SelectObject(hdcdest, hbmdest);

// draw background
	BitBlt(hdcdest,0,0,rc.right,rc.bottom,hdcsrc,0,0,SRCCOPY);

// draw thumn
	SelectObject(hdcsrc,control->_hthumb);
	if(control->_horz)
		BitBlt(hdcdest,control->_interpos,0,control->_thumbsize,rc.bottom,hdcsrc,control->_thumbsize * state,0,SRCCOPY);
	else
		BitBlt(hdcdest,0,control->_interpos,rc.right, control->_thumbsize,hdcsrc,rc.right * state,0,SRCCOPY);
			

// draw to screen
	BitBlt(hdc,0,0,rc.right, rc.bottom,
			hdcdest,0,0,SRCCOPY);

	SelectObject(hdcsrc,hbold);
	SelectObject(hdcdest,hbold1);
	DeleteDC(hdcdest);
	DeleteDC(hdcsrc);
	DeleteObject(hbmdest);
	




}



void WScrollbar::GetThumbRect(WScrollbar* control, RECT* rcthumb)
{
	
	GetWindowRect(control->Handle, rcthumb);
	if(control->_horz) {
		rcthumb->left  += control->_interpos;
		rcthumb->right = rcthumb->left + control->_thumbsize;
	}
	else {
		rcthumb->top +=  control->_interpos;
		rcthumb->bottom = rcthumb->top + control->_thumbsize; 

	}

}


BOOL WScrollbar::IsPointOnThumb(WScrollbar* control, POINT* pt)
{
	RECT rc;
	GetThumbRect(control,&rc);
	return PtInRect(&rc, *pt);

}



void WScrollbar::InvalidateThumb(WScrollbar* control)
{
	RECT rc;
	GetThumbRect(control, &rc);
	InvalidateRect(control->Handle,&rc,FALSE);

}
