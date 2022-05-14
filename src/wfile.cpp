#define STRICT
#include <windows.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "WFile.h"


WFile::WFile()
{
	_whandle = 0;
    _size = 0;
}

BOOL WFile::Open(const char *filename, DWORD DesiredAccess, DWORD ShareMode, DWORD CreationDistribution)
{
	_whandle = ::CreateFile(filename,DesiredAccess,ShareMode,NULL,CreationDistribution,0,NULL);
    if( _whandle == INVALID_HANDLE_VALUE)
    	return FALSE;

    _size = ::GetFileSize(_whandle, NULL);
    if(_size ==  0xFFFFFFFF) {
    	::CloseHandle(_whandle);
        _whandle = 0;
        _size = 0;
        return FALSE;
    }

    return TRUE;
}


BOOL WFile::Close()
{
    if(_whandle)
    {
		BOOL ret =  ::CloseHandle(_whandle);
    	_whandle = NULL;
    	return ret;
    }
    return 0;
}

WFile::~WFile()
{
 	if(_whandle) Close();
}




BOOL WFile::Read(void *lpBuffer, DWORD nNumberOfBytesToRead, DWORD& nNumberOfBytesRead)
{
	return ::ReadFile(_whandle, lpBuffer, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);

}

BOOL WFile::Write(void *lpBuffer,DWORD nNumberOfBytesToRead, DWORD& nNumberOfBytesRead)
{
    nNumberOfBytesRead = 0;
	return ::WriteFile(_whandle, lpBuffer, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);

}



BOOL WFile::Seek(LONG lDistanceToMove, DWORD dwMoveMethod)
{
 	if( ::SetFilePointer(_whandle, lDistanceToMove, NULL, dwMoveMethod) ==  0xFFFFFFFF)
    	return FALSE;

	return TRUE;
}


