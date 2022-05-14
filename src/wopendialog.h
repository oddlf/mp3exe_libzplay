// WOpenDialog class
//
// Version 	1.0
// Date:    05/05/2006
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr


#ifndef _WOPENDIALOG_
#define _WOPENDIALOG_



class WOpenDialog
{
public:
	BOOL Execute(HWND hwndOwner, LPCTSTR lpstrFilter, DWORD nFilterIndex,
						LPCTSTR lpstrInitialDir, LPCTSTR lpstrTitle, LPTSTR lpstrFile);


    char* GetFileName() { return _ofn.lpstrFile;};

private:
	OPENFILENAME _ofn;
    char _fileName[MAX_PATH + 1];


protected:




}
;



#endif


