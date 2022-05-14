#define STRICT
#include <windows.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <string.h>
#include <stdlib.h>



#include "wapp.h"

// constructor

WApp::WApp()
{
 	Instance = ( HINSTANCE ) GetModuleHandle(NULL);
    DlgNum = 0;
}

WApp::WApp(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    DlgNum = 0;
	Instance = hInstance;
    CmdLine = lpszCmdLine;
    CmdShow = nCmdShow;
    char tmp[MAX_PATH];
    GetModuleFileName(Instance, tmp,MAX_PATH);
    char *end = strrchr(tmp,'\\');
    char *tmp1;
    if(end) {
    	*end = 0;
        strcpy(FileDir, tmp);
        tmp1 = end + 1;
    }
    strcpy(FileName, tmp1);
}


BOOL WApp::DlgMessage(MSG *message)
{
// return TRUE if message is dialog message
	for( DlgCounter = 0; DlgCounter < DlgNum; DlgCounter++ ) {
    	if (IsWindow(DlgHandle[DlgCounter]) && IsDialogMessage(DlgHandle[DlgCounter], message))
        	return TRUE;

    }
    return FALSE;
}


void WApp::Run()
{
// maim message loop
    while (GetMessage(&msg, (HWND) NULL, 0, 0)) {
		if(!DlgMessage(&msg)) {
        	TranslateMessage(&msg);
        	DispatchMessage(&msg);
        }
    }
}


void WApp::Terminate()
{
// terminate message loop
	PostQuitMessage (0);
}


void WApp::ProcessThisDlgMsg(HWND Handle)
{
	DlgHandle[DlgNum] = Handle;
    DlgNum++;
}

void WApp::DeleteThisDlgMsg(HWND Handle)
{
 	for(int i = 0; i < DlgNum; i++) {
    	if(DlgHandle[i] == Handle) {
        	for( int j = i; j < DlgNum - 1; j++ )
            	DlgHandle[j] = DlgHandle[j + 1];

            DlgNum--;
            return;
        }
    }
}
