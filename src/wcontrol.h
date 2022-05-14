// WControl class
//
// Version 	1.0
// Date:    10/05/2004
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr


#ifndef _WCONTROL_
#define _WCONTROL_

#include <tchar.h>
//#include <mem.h>
#include "wtooltip.h"
#include "wfont.h"
//#include "werror.h"

#define MAKECOLORREF(rgb) ( (DWORD) ((DWORD)(BYTE)(rgb)<<16 ) | \
				                ((WORD) (BYTE)(rgb >> 8)<<8 ) | \
                                ( (BYTE)(rgb >> 16)) )






void BringWindowToForeground(HWND hwnd, BOOL fShow);


class WControl {
public:
	HWND Handle;
    WTooltip *Tooltip;
    WFont *Font;

    static void *GetClassPointer(HWND hwnd);
    
    void SetClassPointer();
    BOOL SetClassPointer(HWND hwnd, HANDLE handle);
    void RemoveClassPointer();
   
    WControl();
    ~WControl();

   

    void SetFocus();

    
    DWORD SetFont(WFont *font, BOOL fRedraw = TRUE);

    BOOL AddTooltip(WTooltip *Tooltip, LPTSTR lpszText); // add button to tooltip control
    void SetTooltipText(LPTSTR lpszText);
    void SetPos(int nX, int nY, int nWidth, int nHeight,BOOL  bRepaint = TRUE);
    void ResizeTo(int nWidth, int nHeight, BOOL  bRepaint = TRUE);
    void MoveTo(int nX, int nY);
    void SetPos(RECT *rcPos ,BOOL  bRepaint = TRUE);
    void UpdateToolRect();
    void Enable(BOOL bEnable = TRUE);
    void Show(BOOL fShow);
    void Close();
    void Destroy();
    void Minimize();
    void Redraw();

    DWORD SendCommand(HWND hwnd, WORD NotificationCode, WORD Identifier, HWND hwndControl = NULL);

    DWORD SetRedraw(BOOL fRedraw = TRUE);


    void GetPos(RECT *rcPos);
    void GetClientPos(RECT *rcPos);
        /*
    // window API wrapper
    BOOL SetWindowPos(HWND  hwndInsertAfter,	// placement-order handle
    					int  x,	// horizontal position
    					int  y,	// vertical position
    					int  cx,	// width
    					int  cy,	// height
    					UINT  fuFlags  // window-positioning flags
                        );
    */

    BOOL SetWindowPos(HWND hwndInsertAfter, RECT *rcPos, UINT fuFlags);
    void AlwaysOnTop(BOOL bOnTop = TRUE);

    BOOL MoveWindow(
    	int  X,	// horizontal position
    	int  Y,	// vertical position
    	int  nWidth,	// width
    	int  nHeight,	// height
    	BOOL  bRepaint 	// repaint flag
   		);

    BOOL MoveWindow(RECT *rcPos,BOOL  bRepaint );

    BOOL IsVisible();
    BOOL IsMinimized();
             
    static TCHAR ClassPointer[];


//    WError* _error;
private:
	UINT ToolID;









};





#endif




