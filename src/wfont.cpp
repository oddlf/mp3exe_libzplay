#define STRICT
#include <windows.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wfont.h"

WFont::WFont()
{
	Handle = ( HFONT ) GetStockObject(ANSI_VAR_FONT);

}

WFont::WFont(int  nSize,DWORD fnStyle, LPCTSTR  lpszFace)
{
 	long lfHeight;
    HDC hdc = GetDC(NULL);
    lfHeight = -MulDiv(nSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(NULL, hdc);

    Handle = CreateFont(lfHeight,
    			 0,
                 0,
                 0,
                 ( fnStyle & FS_BOLD ) ? 700:400,
                 fnStyle & FS_ITALIC,
                 fnStyle & FS_UNDERLINE,
                 fnStyle & FS_STRIKEOUT,
                 DEFAULT_CHARSET, 0, 0, 0, 0,lpszFace);


    if (Handle == NULL)
    	WFont();

}

void WFont::Select(int  nSize,DWORD fnStyle, LPCTSTR  lpszFace)
{
    if (Handle != NULL)
    	DeleteObject(Handle);

 	long lfHeight;
    HDC hdc = GetDC(NULL);
    lfHeight = -MulDiv(nSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(NULL, hdc);

    Handle = CreateFont(lfHeight,
    			 0,
                 0,
                 0,
                 ( fnStyle & FS_BOLD ) ? 700:400,
                 fnStyle & FS_ITALIC,
                 fnStyle & FS_UNDERLINE,
                 fnStyle & FS_STRIKEOUT,
                 DEFAULT_CHARSET, 0, 0, 0, 0,lpszFace);



    if (Handle == NULL)
    	WFont();

}

WFont::~WFont()
{
	DeleteObject(Handle);
}


