#define STRICT
#include <windows.h>
#pragma hdrstop




#include <stdio.h>


#include <commctrl.h>
#include <tchar.h>

#include "wbmpbutton.h"






// button states
#define NORMAL 0
#define PUSHED 1
#define DISABLED 2
#define HIGHLIGHT 3
#define CHECKED 4
#define CHECKEDHIGHLIGHT 5
#define CHECKEDDISABLED 6



WBmpButton::WBmpButton()
{
	//InitCommonControls();
	_oldproc = 0;
    _checked = FALSE;
	_checkbutton = FALSE;

	Handle = 0;



}

WBmpButton::~WBmpButton()
{

// restore original window procedure
	if(_oldproc) {
		SetWindowLong(Handle, GWL_WNDPROC,(LONG) _oldproc);
    }
	
	
	
	DestroyWindow(Handle);
	DeleteObject(_hbitmap);
}






void WBmpButton::Redraw()
{
 	InvalidateRect(Handle,NULL,FALSE);
}



void WBmpButton::SetCheck(BOOL fCheck)
{
    if(fCheck) {
    	_checked = TRUE;	
    }
    else {
    	_checked = FALSE;
    }

	InvalidateRect(Handle,NULL,FALSE);
}


BOOL WBmpButton::GetCheck()
{
    return _checked;
}









LRESULT CALLBACK WBmpButton::_WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  	WBmpButton *bt= (WBmpButton * )  GetWindowLong(hwnd, GWL_USERDATA	);

	switch(uMsg)
   	{

       	case WM_PAINT:
        {
		
        	PAINTSTRUCT ps;
    		HDC hdc;
        	hdc = BeginPaint(hwnd, &ps);
           	Draw(bt, hdc);
            EndPaint(hwnd,&ps);
        }
        return 0;

        case WM_MOUSEMOVE:
			MouseMove(bt,wParam,lParam);
        return 0;


        case WM_MOUSELEAVE:
        	bt->_hover = FALSE;
            InvalidateRect(bt->Handle, NULL, FALSE);
        return 0;

        case WM_MOVE:
         	bt->UpdateToolRect();

        return 0;

        case  WM_SIZE:
        	bt->UpdateToolRect();
        break;

        case WM_LBUTTONDOWN:
              ButtonDown(bt);
        return 0;

        case WM_LBUTTONUP:
            ButtonUp(bt,lParam);
        return 0;

      

    	case BM_SETCHECK:
        	bt->Redraw();
        return 0;

        case WM_KEYDOWN :

        return 0;

        case WM_KEYUP :

        return 0;

        case WM_SETTEXT:
        	DefWindowProc(hwnd,uMsg,wParam,lParam);
            InvalidateRect(hwnd,NULL,FALSE);
        return 0;




        case WM_LBUTTONDBLCLK:
        	PostMessage(hwnd,WM_LBUTTONDOWN,wParam,lParam);
        return 0;

        case WM_MBUTTONDOWN :

        return 0;

        case WM_MBUTTONUP:

        return 0;

        case WM_RBUTTONDOWN:

        return 0;

        case WM_RBUTTONUP:

        return 0;

        case WM_ENABLE:
        	InvalidateRect(hwnd,NULL,FALSE);
        break;
        

        case WM_DESTROY:
    		SetWindowLong(bt->Handle, GWL_WNDPROC,(LONG)bt->_oldproc);

        return CallWindowProc(bt->_oldproc, hwnd, uMsg, wParam, lParam);

		

     }


    return CallWindowProc(bt->_oldproc, hwnd, uMsg, wParam, lParam);

}



void WBmpButton::MouseMove(WBmpButton *bt, WPARAM wParam, LPARAM lParam)
{
	
	POINT pt;
    pt.x= LOWORD(lParam);
    pt.y= HIWORD(lParam);
	RECT rect;
    GetClientRect(bt->Handle,&rect);

    if(PtInRect(&rect,pt)) {
		if(GetCapture() != bt->Handle) {
			SetCapture(bt->Handle);
		}
		
			if(wParam == MK_LBUTTON) {
				if(!bt->_pushed) {
					bt->_pushed = TRUE;
					InvalidateRect(bt->Handle, NULL, FALSE);
				}
			}
			else {
				if(!bt->_hover) {
					bt->_hover = TRUE;
					InvalidateRect(bt->Handle, NULL, FALSE);
				
				}
			}
	}
	else {
		bt->_hover = FALSE;
		if(wParam != MK_LBUTTON) {
			ReleaseCapture();
			InvalidateRect(bt->Handle, NULL, FALSE);
		}
		else {
			if(bt->_pushed) {
				bt->_pushed = FALSE;
				InvalidateRect(bt->Handle, NULL, FALSE);
			}
		}
	}	
}


void WBmpButton::Draw(WBmpButton *bt, HDC hdc)
{

	
    RECT rc;
   	GetClientRect(bt->Handle,&rc);

	DWORD x = 0;

	

   	bt->_state = NORMAL;

    if(!IsWindowEnabled(bt->Handle))
		bt->_state = DISABLED;
    else if (bt->GetCheck())
			bt->_state = CHECKED;
	else {
		RECT rc;
		GetWindowRect(bt->Handle, &rc);
		POINT pt;
		GetCursorPos(&pt);
		if(PtInRect(&rc,pt))
			bt->_state = HIGHLIGHT;
	}
	
	if(bt->_pushed)
        bt->_state = PUSHED;


	HDC hdcmem = CreateCompatibleDC(hdc);
	HBITMAP hbmold = (HBITMAP) SelectObject(hdcmem,bt->_hbitmap);

    switch(bt->_state)
    {
     	case HIGHLIGHT:
        {
			x = bt->_bmpWidth / 2;
			BitBlt(hdc, rc.left, rc.top, rc.right, rc.bottom,
					hdcmem,x,0,SRCCOPY);
        }
        break;

        case NORMAL:
        {
			x = 0;
			BitBlt(hdc, rc.left, rc.top, rc.right, rc.bottom,
					hdcmem,x,0,SRCCOPY);
        }
        break;


        case PUSHED:
		case CHECKED:
        {
			x = bt->_bmpWidth / 4;
			BitBlt(hdc, rc.left, rc.top, rc.right, rc.bottom,
					hdcmem,x,0,SRCCOPY);
        }
        break; 

         case DISABLED:
        {
			x = bt->_bmpWidth / 4 * 3;
			BitBlt(hdc, rc.left, rc.top, rc.right, rc.bottom,
					hdcmem,x,0,SRCCOPY);	
        }
        break;
    }

	SelectObject(hdcmem,hbmold);
	DeleteDC(hdcmem);

	
   
}


void WBmpButton::ButtonDown(WBmpButton *bt)
{
    SetForegroundWindow(GetParent(bt->Handle));
	SetCapture(bt->Handle);
    bt->_pushed = TRUE;
    InvalidateRect(bt->Handle,NULL,FALSE);

}



void WBmpButton::ButtonUp(WBmpButton *bt, LPARAM lParam)
{

    RECT rect;
    GetClientRect(bt->Handle,&rect);

	POINT pt;
    pt.x= LOWORD(lParam);
    pt.y= HIWORD(lParam);

	ReleaseCapture();
	
    if(PtInRect(&rect,pt)) { // in button rect
		//bt->_hover = TRUE;
    	if(bt->_pushed) { // button was pushed
        	if(bt->_checkbutton)
            {

                bt->SetCheck(!bt->GetCheck());
            }

			bt->_hover = FALSE;
			bt->_pushed = FALSE;
			InvalidateRect(bt->Handle,NULL,FALSE);

        	LONG ButtonID = GetWindowLong(bt->Handle,GWL_ID);
        	SendMessage(GetParent(bt->Handle),WM_COMMAND,MAKEWPARAM(ButtonID,BN_CLICKED),
       				(LPARAM)bt->Handle);
			InvalidateRect(bt->Handle,NULL,FALSE);

        }

    }
	else {
		bt->_hover = FALSE;
		bt->_pushed = FALSE;
		InvalidateRect(bt->Handle,NULL,FALSE);	
	}


}




int WBmpButton::Create(DWORD  dwType,
				int nX,
				int nY,
				int nWidth,
				int nHeight,
				HWND hWndParent,
				UINT hMenu,
				HINSTANCE hInstance,
                HBITMAP hbitmap)
{

// copy bitmap to internal structure

	HDC hdcMem1, hdcMem2;
    HBITMAP hbmDest = 0;
    BITMAP bm = {0};
  
	hdcMem1 = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

    GetObject(hbitmap, sizeof(BITMAP), &bm);
	
    hbmDest = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes,
			bm.bmBitsPixel,NULL);

    
    HBITMAP hbmOld1 = (HBITMAP)SelectObject(hdcMem1, hbitmap);
    HBITMAP hbmOld2 = (HBITMAP)SelectObject(hdcMem2, hbmDest);
    
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY);
  
    SelectObject(hdcMem1, hbmOld1);
    SelectObject(hdcMem2, hbmOld2);

    DeleteDC(hdcMem1);
    DeleteDC(hdcMem2);

	_bmpWidth = bm.bmWidth;
	_bmpHeight = bm.bmHeight;

	_hbitmap = hbmDest;
	
	
	DWORD style = WS_VISIBLE|WS_CHILD;
	if(dwType == CHECK_BUTTON)
		_checkbutton = TRUE;
	else
		_checkbutton = FALSE;


	Handle = CreateWindowEx(0, "BUTTON", "",
    	style,
        nX, nY, nWidth, nHeight, hWndParent, (HMENU)hMenu,
        hInstance, NULL);

   
        _state = NORMAL;
        _hover = FALSE;
        _pushed = FALSE;



   

	  SetWindowLong(Handle, GWL_USERDATA, (LONG) this);
		_oldproc = (WNDPROC)SetWindowLong(Handle,GWL_WNDPROC,(LONG) _WindowSubclassProc);


   return 1;


}


void WBmpButton::SetBitmap(HBITMAP hbitmap)
{
	DeleteObject(_hbitmap);

	HDC hdcMem1, hdcMem2;
    HBITMAP hbmDest = 0;
    BITMAP bm = {0};
  
	hdcMem1 = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

    GetObject(hbitmap, sizeof(BITMAP), &bm);
	
    hbmDest = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes,
			bm.bmBitsPixel,NULL);

    
    HBITMAP hbmOld1 = (HBITMAP)SelectObject(hdcMem1, hbitmap);
    HBITMAP hbmOld2 = (HBITMAP)SelectObject(hdcMem2, hbmDest);
    
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY);
  
    SelectObject(hdcMem1, hbmOld1);
    SelectObject(hdcMem2, hbmOld2);

    DeleteDC(hdcMem1);
    DeleteDC(hdcMem2);

	_bmpWidth = bm.bmWidth;
	_bmpHeight = bm.bmHeight;

	_hbitmap = hbmDest;

}

