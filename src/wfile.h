#ifndef _WFILE_Z_
#define _WFILE_Z_

#define ACCESS_READ 			GENERIC_READ
#define ACCESS_WRITE 			GENERIC_WRITE

#define NO_SHARE		0
#define SHARE_READ 		FILE_SHARE_READ
#define SHARE_WRITE		FILE_SHARE_WRITE

class WFile {
	public:

    WFile();
    ~WFile();
    BOOL Open(const char *filename, DWORD DesiredAccess, DWORD ShareMode, DWORD CreationDistribution);
    BOOL Close();
    BOOL Read(void *lpBuffer, DWORD nNumberOfBytesToRead, DWORD& nNumberOfBytesRead);
    BOOL Seek(LONG lDistanceToMove, DWORD dwMoveMethod);
    DWORD GetFileSize() { return _size; };
    BOOL Write(void *lpBuffer,DWORD nNumberOfBytesToRead, DWORD& nNumberOfBytesRead);


    private:
    	HANDLE _whandle;
    	DWORD _size;





};

#endif
