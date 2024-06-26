#define STRICT
#include <Windows.h>
#include <CommCtrl.h> // must link with  comctl32 library

#include <libzplay/libzplay.h>

#include "MainForm.h"

#include "Main.h"
#include "resource.h"
#include "WBmpTextBox.h"

extern BOOL Start();

WApp* myApp;
MainForm* mainForm;

libZPlay::ZPlay* player;

WMixer* mixer;

BOOL bAutostart = FALSE;

extern char mp3filename[MAX_PATH];

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[] = "ZC_Mp3ExePlayer";

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{

	if (lpszArgument && *lpszArgument)
	{

		if (lpszArgument[0] == '\"')
			lpszArgument++;

		size_t size = strlen(lpszArgument);
		if (size)
		{
			if (lpszArgument[size - 1] == '\"')
				lpszArgument[size - 1] = 0;
		}

		strcpy_s(mp3filename, lpszArgument);

		HWND hwnd;
		hwnd = FindWindow("ZC_Mp3ExePlayer", NULL);
		if (hwnd)
		{
			COPYDATASTRUCT data;
			data.cbData = (DWORD)strlen(mp3filename) + 1;
			data.lpData = mp3filename;
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&data);
			// terminate this instance of application
			return 0;
		}
	}

	InitCommonControls();

	player = libZPlay::CreateZPlay();

	myApp = new WApp(hThisInstance, lpszArgument, nFunsterStil);
	mainForm = new MainForm;
	mixer = new WMixer;

	mainForm->Create(szClassName, LoadIcon(hThisInstance, MAKEINTRESOURCE(LARGE_ICON)),
		LoadIcon(hThisInstance, MAKEINTRESOURCE(SMALL_ICON)),
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)COLOR_BACKGROUND, NULL,
		0, "Mp3-Exe Player", WS_POPUP | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 275, 388,
		NULL);

	mainForm->Show(TRUE);

	if (lpszArgument && *lpszArgument)
	{
		SendMessage(mainForm->Handle, WM_COMMAND, MAKEWPARAM(MESSAGE_PLAY_ARGUMENT, 0), 0);
	}
	else
	{
		SendMessage(mainForm->Handle, WM_COMMAND, MAKEWPARAM(MESSAGE_PLAY_EMBEDED, 0), 0);
	}

	myApp->Run();

	delete mixer;
	delete mainForm;
	player->Release();
	delete myApp;

	return 0;
}
