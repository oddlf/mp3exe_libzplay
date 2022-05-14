#define STRICT
#include <windows.h>


#pragma hdrstop



#include <stdio.h>
#include <assert.h>

#include "wbmptextbox.h"


#define TB_SCROLL_TIMER WM_USER + 1

void ScrollCharArray(char *text);

WBmpTextBox::WBmpTextBox()
{
	_font = 0;
	_hbBg = 0;
	_oldproc = 0;
	_lpText = 0;
	_nTextLen = 0;
	_timer_timeout = 500;
	_enable_scroll = TRUE;
	_scroll_text_delimiter = 0;
	_scroll_text = 0;


}

WBmpTextBox::~WBmpTextBox()
{

	if(_lpText)
		free(_lpText);

	if(_hbBg)
		DeleteObject(_hbBg);

	if(_scroll_text_delimiter)
		free(_scroll_text_delimiter);

	if(_scroll_text)
		free(_scroll_text);


		// restore original window procedure
		
	if(_oldproc)
	{
		SetWindowLong(Handle, GWL_WNDPROC,(LONG) _oldproc);
    }
	
	
	DestroyWindow(Handle);

}


int WBmpTextBox::SetText(char* text, BOOL fRedraw)
{
	// prevent from invalid input
	if(!text)
		return 0;
	// free old text buffer
	if(_lpText)
		free(_lpText);

	_lpText = 0;


	// stop timer, free old scrolling text buffer

	if(_scroll_text)
	{
		KillTimer(Handle, TB_SCROLL_TIMER);
		free(_scroll_text);
		_scroll_text = 0;
		
	}

// all buffers are now free, scrolling is stoped


// allocate new text buffer
	_nTextLen = strlen(text);
	_lpText = (char*) malloc(_nTextLen + 1);
	if(!_lpText)
	{
		_nTextLen = 0;
		return 0;
	}

// copy text into new text buffer
	strcpy(_lpText, text);

// redraw window, Draw function is responsible for text scrolling
	if(fRedraw)
		InvalidateRect(Handle,NULL,FALSE);

	return 1;
}



int WBmpTextBox::Create(char* text,
				int nX,
				int nY,
				int nWidth,
				int nHeight,
				HWND hWndParent,
				UINT hMenu,
				HINSTANCE hInstance,
                HBITMAP hBgBitmap,
				WBmpFont* font)
{

	if(!text)
		return 0;

	_scroll_text = 0;

	_nTextLen = strlen(text);
	_lpText = (char*) malloc(_nTextLen + 1);
	if(!_lpText)
	{
		return 0;
	}

	_scroll_text_delimiter = (char*) malloc(5);

	if(!_scroll_text_delimiter)
	{
		free(_lpText);
		_lpText = 0;
		_nTextLen = 0;
		return 0;	
	}

	strcpy(_scroll_text_delimiter, " - ");

	strcpy(_lpText, text);


	HDC hdcMem1, hdcMem2;
    
  




    if(hBgBitmap)
	{
		hdcMem1 = CreateCompatibleDC(0);
		hdcMem2 = CreateCompatibleDC(0);

		BITMAP bm = {0};
		GetObject(hBgBitmap, sizeof(BITMAP), &bm);
		_hbBg = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel,NULL);

		HBITMAP hbmOld1 = (HBITMAP)SelectObject(hdcMem1, hBgBitmap);
		HBITMAP hbmOld2 = (HBITMAP)SelectObject(hdcMem2, _hbBg);
    
		BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY);
  
		SelectObject(hdcMem1, hbmOld1);
		SelectObject(hdcMem2, hbmOld2);

		 DeleteDC(hdcMem1);
		DeleteDC(hdcMem2);
	}
	else
	{
		hdcMem1 = CreateCompatibleDC(0);
		_hbBg = CreateCompatibleBitmap(hdcMem1, nWidth, nHeight);
		HBITMAP hbmOld1 = (HBITMAP)SelectObject(hdcMem1, _hbBg);
    

 
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = nWidth;
		rc.bottom = nHeight;
		FillRect(hdcMem1, &rc,  (HBRUSH) GetStockObject(BLACK_BRUSH) );
    
		SelectObject(hdcMem1, hbmOld1);

		DeleteDC(hdcMem1);

	

	}


  


	Handle = CreateWindowEx(0, "STATIC", "",
    	WS_VISIBLE|WS_CHILD|SS_NOTIFY,
        nX, nY, nWidth, nHeight, hWndParent, (HMENU) hMenu,
        hInstance, NULL);

    
	_font = font;






	SetWindowLong(Handle, GWL_USERDATA, (LONG) this);


	_oldproc = (WNDPROC)SetWindowLong(Handle,GWL_WNDPROC,(LONG) _WindowSubclassProc);




	return 1;

}


LRESULT CALLBACK WBmpTextBox::_WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	
  	WBmpTextBox *control= (WBmpTextBox * )  GetWindowLong(hwnd, GWL_USERDATA);


  	switch(uMsg)
   	{

		case WM_TIMER:
        {
        	switch (wParam)
			{
             	case TB_SCROLL_TIMER:
				{
                	ScrollCharArray(control->_scroll_text);
					InvalidateRect(control->Handle, NULL,FALSE);
					
       					
                 }
                break;
            }
        }
        break;



		case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
    	{
        	HWND hwndParent;
            hwndParent = GetParent(hwnd);
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            MapWindowPoints(hwnd, hwndParent, &pt,1);
            SendMessage(hwndParent, uMsg, wParam, MAKELPARAM(pt.x, pt.y));

        }
        return 0;

		case WM_LBUTTONDOWN:
        {
        	HWND hwndParent;
            hwndParent = GetParent(hwnd);
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            MapWindowPoints(hwnd, hwndParent, &pt,1);

        	LONG ButtonID = GetWindowLong(control->Handle,GWL_ID);
        	SendMessage(hwndParent,WM_COMMAND,MAKEWPARAM(ButtonID,BN_CLICKED),
       				(LPARAM)control->Handle);

            SendMessage(hwndParent, uMsg, wParam, MAKELPARAM(pt.x, pt.y));
        }
        return 0;


		case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
		
        	HWND hwndParent;
            hwndParent = GetParent(hwnd);
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            MapWindowPoints(hwnd, hwndParent, &pt,1);
            SendMessage(hwndParent, uMsg, wParam, MAKELPARAM(pt.x, pt.y));

        }
        return 0;


        
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        {
        	HWND hwndParent;
            hwndParent = GetParent(hwnd);
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            MapWindowPoints(hwnd, hwndParent, &pt,1);
            SendMessage(hwndParent, uMsg, wParam, MAKELPARAM(pt.x, pt.y));

        }
        return 0;

		case WM_LBUTTONDBLCLK:
        	PostMessage(hwnd,WM_LBUTTONDOWN,wParam,lParam);
        return 0;

    
         
        case WM_ENABLE:
        	control->Redraw();
        break;



        case WM_MOUSEMOVE:
		{
        	HWND hwndParent;
            hwndParent = GetParent(hwnd);
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            MapWindowPoints(hwnd, hwndParent, &pt,1);
            SendMessage(hwndParent, uMsg, wParam, MAKELPARAM(pt.x, pt.y));

        }
        return 0;

		case WM_KEYDOWN:
        return 0;

        case WM_KEYUP:
        return 0;

        case WM_SETTEXT:
        return 0;

       	case WM_PAINT:
        {
        	PAINTSTRUCT ps;
    		HDC hdc;
        	hdc = BeginPaint(hwnd, &ps);
           	Draw(control, hdc);
            EndPaint(hwnd,&ps);
			
        }
        return 0;


		case WM_DESTROY:
    		SetWindowLong(control->Handle, GWL_WNDPROC,(LONG) control->_oldproc);

        return CallWindowProc(control->_oldproc, hwnd, uMsg, wParam, lParam);


	

     }


    return CallWindowProc(control->_oldproc, hwnd, uMsg, wParam, lParam);

}





void WBmpTextBox::Draw(WBmpTextBox *control, HDC hdc)
{
	
	if(control->_scroll_text)
		control->_font->SetText(control->_scroll_text);
	else
		control->_font->SetText(control->_lpText);

	RECT rcClient;
	GetClientRect(control->Handle,&rcClient);

	HDC hdcmem = CreateCompatibleDC(hdc);
	HBITMAP hbm = CreateCompatibleBitmap(hdc,rcClient.right,rcClient.bottom);
	HBITMAP hbmOldMem = (HBITMAP) SelectObject(hdcmem, hbm);
	
	HDC hdcBg = CreateCompatibleDC(hdc);
	HBITMAP hbmOldBg = (HBITMAP) SelectObject(hdcBg, control->_hbBg);


	// draw background
	BitBlt(hdcmem,0,0,rcClient.right,rcClient.bottom,
		hdcBg,0,0,SRCCOPY);

	SelectObject(hdcBg,hbmOldBg);
	DeleteDC(hdcBg);

	// draw bitmap text into memory DC and get size of text bitmap
	int ret = control->_font->Draw(hdcmem,&rcClient,0,1,RGB(0,0,0));
	// check if need scroll text
	if(ret > rcClient.right - 1 && control->_enable_scroll)
	{
		// allocate memory for scrolling text and create scrolling text
		if(!control->_scroll_text)
		{ // prevent from multiple allocation
			control->_scroll_text = (char*) malloc (control->_nTextLen + strlen(control->_scroll_text_delimiter) + 1);
			// memory is allocated, create scrolling text and create timer
			if(control->_scroll_text)
			{
				sprintf(control->_scroll_text,"%s%s", control->_lpText, control->_scroll_text_delimiter);
				SetTimer(control->Handle,TB_SCROLL_TIMER, control->_timer_timeout,NULL); 
			}
		}
	}
	else if(control->_scroll_text)
	{ // no need for scrolling text
	// if allocated memory for scrolling text, free this memory, stop timer
		KillTimer(control->Handle, TB_SCROLL_TIMER); 
		free(control->_scroll_text);
		control->_scroll_text = 0;
	}	

	BitBlt(hdc,0,0,rcClient.right,rcClient.bottom,
		hdcmem,0,0,SRCCOPY);


	SelectObject(hdcmem,hbmOldMem);
	DeleteDC(hdcmem);
	DeleteObject(hbm);

	SelectObject(hdcBg,hbmOldBg);
	DeleteDC(hdcBg);
	 


}


int WBmpTextBox::SetScrollTextDelimiter(char* delimiter)
{
	if(!delimiter)
		return 0;

// stop scrolling		
	if(_scroll_text)
	{
		KillTimer(Handle,TB_SCROLL_TIMER);
		free(_scroll_text);
		_scroll_text = 0;
	}
// free memory for delimiter
	if(_scroll_text_delimiter)
		free(_scroll_text_delimiter);

	_scroll_text_delimiter = 0;

// scrolling is stoped, memory for delimiter is free

// allocate new memory for delimiter
	_scroll_text_delimiter = (char*) malloc(strlen(delimiter) + 1);

	if(!_scroll_text_delimiter) {	
		return 0;	
	}
// copy new delimitert into new allocated memory
	strcpy(_scroll_text_delimiter, delimiter);

// redraw window, Draw function is responsible for new scrolling		
	InvalidateRect(Handle, NULL,FALSE);

	return 1;

}

void WBmpTextBox::EnableScroll(BOOL Enable,DWORD dwTimeout)
{
	_enable_scroll = Enable;
	_timer_timeout = dwTimeout;
// stop current scrolling	
	if(_scroll_text)
	{
		KillTimer(Handle,TB_SCROLL_TIMER);
		free(_scroll_text);
		_scroll_text = 0;
	}

// redraw window, Draw function is responsible for new text scrolling
	InvalidateRect(Handle, NULL,FALSE);

}


void ScrollCharArray(char *text)
{
	
	char first = *text;
	if(first == 0)
		return;

	text++;

	while(*text)
	{
		*(text - 1) = *text;
		text++;	
	}

	*(text - 1) = first;



/*

	if(strlen(text) > 1)
	{
		char c = *text;
		memmove(text,


		char first[2];
    	first[0] = *text;
    	first[1]=0;
		
    	sprintf(text,"%s%s",text+1,first);
    }
	*/
}
