// WBmpFont class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004

#ifndef _WBMPFONT_
#define _WBMPFONT_






class WBmpFont {
public:
	WBmpFont();
	~WBmpFont();
	int Open(HBITMAP hbFont, char* lpCharMap, int dwFontHeight);
	int SetText(char* lpText);
	int Draw(HDC hdc, RECT* rcDest,
    		BOOL fStretch, BOOL fTransparent, COLORREF crTransparent);

private:
	HBITMAP _hbFont;
	int _dwHeight;
	char* _lpCharMap;
	char* _lpText;
	int _dwTextLen;
	int _dwCharMapLen;

	static int _Draw(WBmpFont* font,HDC hdc, RECT* rcDest,
    		BOOL fStretch, BOOL fTransparent, COLORREF crTransparent);

};



#endif
