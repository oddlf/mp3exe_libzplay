#define STRICT
#include <Windows.h>

#include <cstdio>

#include "WIniKey.h"

WIniKey::WIniKey(const char* szFileName)
{
	strcpy_s(FileName, szFileName);
}

int WIniKey::GetKey(LPCTSTR lpszSection, LPCTSTR lpszKey, LPTSTR lpReturnedString, DWORD nSize)
{
	if (!lpszSection || !lpszKey)
	{
		*lpReturnedString = 0;
		return 0;
	}
	return GetPrivateProfileString(lpszSection, lpszKey, "\0", lpReturnedString,
		nSize, FileName);
}

int WIniKey::EnumSections(LPTSTR lpReturnedString, DWORD nSize)
{
	return GetPrivateProfileString(NULL, NULL, "\0", lpReturnedString,
		nSize, FileName);
}

int WIniKey::EnumKeys(LPCTSTR lpszSection, LPTSTR lpReturnedString, DWORD nSize)
{
	if (!lpszSection)
	{
		*lpReturnedString = 0;
		return 0;
	}
	return GetPrivateProfileString(lpszSection, NULL, "\0", lpReturnedString,
		nSize, FileName);
}

int WIniKey::WriteKey(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszString)
{
	if (!lpszSection || !lpszKey || !lpszString)
	{
		return 0;
	}

	return WritePrivateProfileString(lpszSection, lpszKey, lpszString, FileName);
}

int WIniKey::DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszKey)
{
	if (!lpszSection || !lpszKey)
	{
		return 0;
	}

	return WritePrivateProfileString(lpszSection, lpszKey, NULL, FileName);
}

int WIniKey::DeleteSection(LPCTSTR lpszSection)
{
	if (!lpszSection)
	{
		return 0;
	}

	return WritePrivateProfileString(lpszSection, NULL, NULL, FileName);
}

/*
BOOL WIniKey::GetKeyInt(LPCTSTR lpszSection,LPCTSTR lpszKey, int *nDefault)
{

	int value = GetPrivateProfileInt(lpszSection, lpszKey, -1, FileName);
	if(value == -1)
		return FALSE;
	else {
		*nDefault = value;
		return TRUE;
	}
}
*/

BOOL WIniKey::GetKeyInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int* nDefault)
{
	char lpReturnedString[100];
	if (GetPrivateProfileString(lpszSection, lpszKey, "\0", lpReturnedString, 100, FileName))
	{

		*nDefault = atoi(lpReturnedString);
		return TRUE;
	}

	return FALSE;
}

int WIniKey::WriteKeyInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int value)
{
	if (!lpszSection || !lpszKey)
	{
		return 0;
	}

	char lpszString[100];
	sprintf_s(lpszString, "%i", value);

	return WritePrivateProfileString(lpszSection, lpszKey, lpszString, FileName);
}
