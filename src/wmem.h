// WMemclass
//
// Version 	1.0
// Date:    10/05/2004
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr

#ifndef _W_MEM_
#define _W_MEM_

class WMem {
public:
	WMem();
    ~WMem();

    char* Buffer;
    DWORD Size;

    BOOL Allocate(DWORD  dwBytes);
    void Free();
    DWORD GetSize();

private:
	HGLOBAL _hMem;


};


#endif


