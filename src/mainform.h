#ifndef _MAINFORM_
#define _MAINFORM_

#include "wform.h"
#include "wbmpbutton.h"
#include "wbmptextbox.h"



#define FFT_SIZE 512
#define FFT_HARMONICS_NUMBER FFT_SIZE/2 + 1


#define INTER_APP_MESSAGE WM_USER + 1

#define MAIN_TIMER 500
#define VU_TIMER 501

#define MESSAGE_PLAY_EMBEDED 115
#define MESSAGE_PLAY_ARGUMENT 116
#define MESSAGE_PLAY_FILE 117







int StrToRect(char *str, RECT *rect);
int StrToRect(char *str, RECT *rc, int num);





extern int PlayTrack;



class MainForm : public WForm {
public:
	int OnMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD &dwReturn);
	LRESULT OnMouseMove(DWORD fwKeys, int X, int Y);
    LRESULT OnCreate();
    LRESULT OnDestroy();
    LRESULT OnMouseDown(DWORD fwKeys, int X, int Y);
    LRESULT OnMouseUp(DWORD fwKeys, int X, int Y);
    LRESULT OnPaint();
    LRESULT OnTimer(UINT idTimer);
    LRESULT OnHScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle);
    LRESULT OnVScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle);
    LRESULT OnCommand(int wNotifyCode, int ControlID, HWND hWndControl);
    LRESULT OnActivate();
    LRESULT OnDeactivate();
    LRESULT OnKeyDown(int VirtKey, int KeyData);
    LRESULT OnKeyUp(int VirtKey, int KeyData);
    LRESULT OnTrayIconNotify(DWORD Message, UINT iconID);
    LRESULT OnContextMenu(int X, int Y, HWND Handle);

	WBmpTextBox *tbF1;
	WBmpTextBox *tbF2;
	WBmpTextBox *tbF3;
	WBmpTextBox *tbF4;
	WBmpTextBox *tbF5;
	WBmpTextBox *tbF6;
	WBmpTextBox *tbF7;
	WBmpTextBox *tbF8;

	int c_LeftAmplitude[FFT_HARMONICS_NUMBER];
	int c_RightAmplitude[FFT_HARMONICS_NUMBER];
	int c_HarmonicFreq[FFT_HARMONICS_NUMBER];

private:
	



HBITMAP hbMain;
HBITMAP hbTitlebar;
HBITMAP hbFFT;
HBITMAP hbFFTBg;


WBmpButton *btSpectrumNext;
WBmpButton *btSpectrumPrev;
WBmpTextBox *tbSpectrumMode;



WBmpButton* btLinear;


WBmpButton *btFFTWindowNext;
WBmpButton *btFFTWindowPrev;
WBmpTextBox *tbFFTWindow;

HBRUSH hBrushFFTBackground;
HBRUSH hBrushFFTLeft;
HBRUSH hBrushFFTLeftOverlap;
HBRUSH hBrushFFTRight;
HBRUSH hBrushFFTRightOverlap;

HPEN hPenBgLine;
HPEN hPenBgLineStrong;
HPEN hPenLeft; 
HPEN hPenRight;

HPEN hPenLeftOverlap; 
HPEN hPenRightOverlap;

RECT FFTDisplayRect;
int c_nSpectrumType;


};



#endif


