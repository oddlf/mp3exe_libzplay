// WOpenDialog class
//
// Version 	1.0
// Date:    05/05/2008
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr


#ifndef _WSAVEDIALOG_
#define _WSAVEDIALOG_



class WSaveDialog
{
public:
	BOOL Execute(HWND hwndOwner, LPCTSTR lpstrFilter, DWORD nFilterIndex,
						LPCTSTR lpstrInitialDir, LPCTSTR lpstrTitle, LPTSTR lpstrFile,LPCTSTR lpstrDefExt);


    char* GetFileName() { return _ofn.lpstrFile;};

private:
	OPENFILENAME _ofn;
    char _fileName[MAX_PATH + 1];


protected:




}
;



#endif


