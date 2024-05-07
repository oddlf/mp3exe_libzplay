// WApp class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004

#ifndef _WAPP_Z_
#define _WAPP_Z_

class WApp
{
public:
	// Variables
	HINSTANCE Instance; // instance of module
	LPSTR CmdLine;		// command line
	int CmdShow;
	char FileDir[MAX_PATH]; //
	char FileName[MAX_PATH];

	// functions
	WApp();
	WApp(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);
	void Run();
	void Terminate();
	void ProcessThisDlgMsg(HWND Handle);
	void DeleteThisDlgMsg(HWND Handle);

private:
	// variables

	HWND DlgHandle[50];
	int DlgNum;
	int DlgCounter;
	MSG msg;

	// functions
	BOOL DlgMessage(MSG* message); // is message dialog message ???

	/*


	int RegisterNewClass(LPCTSTR lpszClassName, UINT style, HICON hIcon, HICON hIconSm,
						HCURSOR hCursor,HBRUSH hbrBackground,LPCTSTR lpszMenuName);

	void Initialize(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);



	LPSTR CmdLine;
	int CmdShow;
	//char FileName[MAX_PATH];
	char FileDir[MAX_PATH];
	//char FilePath[MAX_PATH];







	HINSTANCE Hinstance;






private:
static LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



*/
};

/*
class WApp {
	public:

		void Run();
		void Terminate();
		int RegisterNewClass(LPCTSTR lpszClassName, UINT style, HICON hIcon, HICON hIconSm,
							HCURSOR hCursor,HBRUSH hbrBackground,LPCTSTR lpszMenuName);

		void Initialize(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);






		WApp();
		WApp(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);

		void ProcessDlgMsg(HWND Handle);

		HINSTANCE Hinstance;




	MSG msg;

private:
	static LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND DlgHandle[50];
	int DlgNum;
	int DlgCounter;
	BOOL DlgMessage(MSG *message);

};

*/
#endif
