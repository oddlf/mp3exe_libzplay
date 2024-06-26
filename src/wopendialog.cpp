// WOpenDialog class
//
// Version 	1.0
// Date:    05/05/2006
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr

#define STRICT
#include <Windows.h>
#include <cstdio>
#include <CommCtrl.h>

#include "WOpenDialog.h"

BOOL WOpenDialog::Execute(HWND hwndOwner, LPCTSTR lpstrFilter, DWORD nFilterIndex,
	LPCTSTR lpstrInitialDir, LPCTSTR lpstrTitle, LPTSTR lpstrFile)
{

	InitCommonControls();

	ZeroMemory(&_ofn, sizeof(_ofn));
	_ofn.lStructSize = sizeof(_ofn);
	_ofn.hwndOwner = hwndOwner;
	_ofn.hInstance = NULL;
	_ofn.lpstrFilter = lpstrFilter;
	_ofn.lpstrCustomFilter = NULL;
	_ofn.nMaxCustFilter = 0;
	_ofn.nFilterIndex = nFilterIndex;
	if (lpstrFile)
	{
		strncpy_s(_fileName, lpstrFile, MAX_PATH);
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
	_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	_ofn.nFileOffset = 0;
	_ofn.nFileExtension = 0;
	_ofn.lpstrDefExt = 0;
	_ofn.lCustData = 0;
	_ofn.lpfnHook = 0;
	_ofn.lpTemplateName = 0;

	return GetOpenFileName(&_ofn);
}
