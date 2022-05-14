// WMem class
//
// Version 	1.0
// Date:    10/05/2004
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr



#define STRICT
#include <windows.h>

#include "wmem.h"


WMem::WMem()
{
	_hMem = 0;
    Buffer = 0;
}

WMem::~WMem()
{
	if(_hMem) {
    	GlobalUnlock(_hMem);
		GlobalFree(_hMem);
    }
}

BOOL WMem::Allocate(DWORD  dwBytes)
{
	_hMem = GlobalAlloc(GPTR, dwBytes);
    if(!_hMem)
    	return FALSE;

    Buffer = (char*) GlobalLock(_hMem);
    if(! Buffer) {
    	GlobalFree(_hMem);
        return FALSE;
    }

    Size = GlobalSize(_hMem);
    return TRUE;
}

void WMem::Free()
{
    GlobalUnlock(_hMem);
	GlobalFree(_hMem);
    Buffer = 0;
    _hMem = 0;
}


DWORD WMem::GetSize()
{
	return GlobalSize(_hMem);
}



