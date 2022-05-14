// WOpenDialog class
//
// Version 	1.0
// Date:    05/05/2008
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr



#define STRICT
#include <windows.h>
#pragma hdrstop

#ifdef _MSC_VER
#pragma comment( lib, "comctl32")
#endif

#include <stdio.h>
#include <commctrl.h>

#include "wsavedialog.h"


BOOL WSaveDialog::Execute(HWND hwndOwner, LPCTSTR lpstrFilter, DWORD nFilterIndex,
						LPCTSTR lpstrInitialDir, LPCTSTR lpstrTitle, LPTSTR lpstrFile,
						LPCTSTR lpstrDefExt)
{

    InitCommonControls();

    ZeroMemory( &_ofn, sizeof(_ofn));
	_ofn.lStructSize = sizeof(_ofn);
    _ofn.hwndOwner = hwndOwner;
    _ofn.hInstance = NULL;
    _ofn.lpstrFilter = lpstrFilter;
    _ofn.lpstrCustomFilter = NULL;
    _ofn.nMaxCustFilter = 0;
    _ofn.nFilterIndex = nFilterIndex;
    if(lpstrFile)
    {
    	strncpy(_fileName, lpstrFile, MAX_PATH);
        _fileName[MAX_PATH] = 0;
    }
	else
		*_fileName = 0;
    _ofn.lpstrFile = _fileName;
    _ofn.nMaxFile = MAX_PATH;
    _ofn.lpstrFileTitle = NULL;
    _ofn.nMaxFileTitle = 0;
    _ofn.lpstrInitialDir = lpstrInitialDir;
    _ofn.lpstrTitle = lpstrTitle;
    _ofn.Flags = OFN_OVERWRITEPROMPT|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST;
    _ofn.nFileOffset = 0;
    _ofn.nFileExtension = 0;
    _ofn.lpstrDefExt = lpstrDefExt;
    _ofn.lCustData = 0;
    _ofn.lpfnHook = 0;
    _ofn.lpTemplateName = 0;


    return GetSaveFileName(&_ofn);




}




