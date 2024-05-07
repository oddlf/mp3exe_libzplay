// WFontclass
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004

#ifndef _WFONT_Z_
#define _WFONT_Z_

#define FS_NORMAL 0x00
#define FS_BOLD 0x01
#define FS_ITALIC 0x02
#define FS_UNDERLINE 0x4
#define FS_STRIKEOUT 0x8

class WFont
{
public:
	HFONT Handle;
	TCHAR Name[LF_FACESIZE];

	long lfHeight;

	WFont();
	WFont(int nSize, DWORD fnStyle, LPCTSTR lpszFace);
	~WFont();

	void Select(int nSize, DWORD fnStyle, LPCTSTR lpszFace);
};

#endif
