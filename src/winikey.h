// WIniKey class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004

#ifndef _WINIKEY_
#define _WINIKEY_


class WIniKey {
public:
	char FileName[MAX_PATH];
    WIniKey(const char *szFileName);
    //~WIniKey();

    int GetKey(LPCTSTR lpszSection,LPCTSTR lpszKey, LPTSTR lpReturnedString,DWORD nSize);
    int WriteKey(LPCTSTR lpszSection,LPCTSTR lpszKey, LPCTSTR lpszString);
    int WriteKeyInt(LPCTSTR lpszSection,LPCTSTR lpszKey, int value);
    int DeleteKey(LPCTSTR lpszSection,LPCTSTR lpszKey);
    int DeleteSection(LPCTSTR lpszSection);
    int EnumSections(LPTSTR lpReturnedString,DWORD nSize);
    int EnumKeys(LPCTSTR lpszSection, LPTSTR lpReturnedString,DWORD nSize);

    BOOL GetKeyInt(LPCTSTR lpszSection,LPCTSTR lpszKey, int *nDefault);





};




#endif

