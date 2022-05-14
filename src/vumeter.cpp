#define STRICT
#include <windows.h>


#include "vumeter.h"


WVUMeter::WVUMeter()
{
	_hbBg = 0;
	_hbFg = 0;
	_max = 100;
	_pos = 0;
	_oldpos = 0;
	_space = 1;
	_dash = 1;
	_peek = 0;
	_oldpeek = 0;
	_oldtime = 0;
	_peek_fall_step = 1;
	_peek_fall_time = 100;

}


WVUMeter::~WVUMeter()
{
	if(_oldproc) {
		SetWindowLong(Handle, GWL_WNDPROC,(LONG) _oldproc);
    }
}


int WVUMeter::Create(HWND hwndParent, HINSTANCE inst, int x, int y, int w, int h,
				HBITMAP hbBg, HBITMAP hbFg, int dash, int space)
{

	Handle = CreateWindowEx(0, "STATIC", "",
    	WS_VISIBLE|WS_CHILD,
        x, y, w, h, hwndParent, 0,
        inst, NULL);

    _hbBg = hbBg;
	_hbFg = hbFg;
	_dash = dash;
	_space = space;

	SetWindowLong(Handle, GWL_USERDATA, (LONG) this);
	_oldproc = (WNDPROC)SetWindowLong(Handle,GWL_WNDPROC,(LONG) _WindowSubclassProc);


	return 1;
}






LRESULT CALLBACK WVUMeter::_WindowSubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  	WVUMeter *control= (WVUMeter * )  GetWindowLong(hwnd, GWL_USERDATA	);
   // return CallWindowProc(bt->_oldproc, hwnd, uMsg, wParam, lParam);

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


	

     }


    return CallWindowProc(control->_oldproc, hwnd, uMsg, wParam, lParam);

}



void WVUMeter::Draw(WVUMeter *control, HDC hdc)
{
	RECT rcClient;
	GetClientRect(control->Handle,&rcClient);
	HDC hdcmem = CreateCompatibleDC(hdc);
	HDC hdcmem1 = CreateCompatibleDC(hdc);
	HBITMAP hbm = CreateCompatibleBitmap(hdc,rcClient.right,rcClient.bottom);
	HBITMAP hbm1 = CreateCompatibleBitmap(hdc,rcClient.right,rcClient.bottom);
	HBITMAP old = (HBITMAP) SelectObject(hdcmem, hbm);
	HBITMAP old1 = (HBITMAP) SelectObject(hdcmem1,control->_hbFg);

	// draw full range
	BitBlt(hdcmem,0,0,rcClient.right,rcClient.bottom,
		hdcmem1,0,0,SRCCOPY);

	SelectObject(hdcmem1,control->_hbBg);


	int res = (rcClient.bottom - control->_dash) / (control->_dash + control->_space) + 1;
	int pos = control->_max - control->_pos;
	int pix = 0;
	if(pos != 0)
		pix = pos * res / control->_max * (control->_dash + control->_space) - 1;

	// draw background ftom top to bottom to get position
	BitBlt(hdcmem,0,0,rcClient.right,
		pix,
		hdcmem1,0,0,SRCCOPY);


	// draw peek
	pos = control->_max - control->_peek;
	pix = 0;
	if(pos != 0)
		pix = pos * res / control->_max * (control->_dash + control->_space) ;
	
	SelectObject(hdcmem1, control->_hbFg);
	BitBlt(hdcmem,0,pix,rcClient.right,
		control->_dash,
		hdcmem1,0,0,SRCCOPY);


	BitBlt(hdc,0,0,rcClient.right,rcClient.bottom,
		hdcmem,0,0,SRCCOPY);

	SelectObject(hdcmem,old);
	SelectObject(hdcmem1,old1);
	DeleteDC(hdcmem);
	DeleteDC(hdcmem1);
	DeleteObject(hbm);
	DeleteObject(hbm1);


}


int WVUMeter::SetRange(DWORD max)
{

	_max = max;
	if(_max <= 0) {
		_max = 100;
		return 0;
	}
	InvalidateRect(Handle,NULL,FALSE);
	return 1;

}

int WVUMeter::SetPos(DWORD pos)
{
	_pos = pos;
	
	if(_pos == 0 && _peek == 0)
		return 1;

	
	if(_pos > _max)
		_pos = _max;

	if(_pos >= _peek)
		_peek = _pos;
	else {
		DWORD time = GetTickCount();
		if((time - _oldtime) > _peek_fall_time) {
			_oldtime = time;
			_peek -= _peek_fall_step;
			if(_peek < _pos)
				_peek = _pos;
		}
	
	}

	if(_pos == 0) {
		if(_peek > _peek_fall_step * 3)
			_peek -= _peek_fall_step * 3;
		else
			_peek = 0;
	}
		

	

	if(_oldpos != _pos || _oldpeek != _peek) {
		InvalidateRect(Handle,NULL,FALSE);
		_oldpos = _pos;
		_oldpeek = _peek;
	}

	
	return 1;

}



int WVUMeter::SetPeekFalloff(DWORD step, DWORD time_ms)
{
	_peek_fall_step = step;
	_peek_fall_time = time_ms;

	return 1;


}
