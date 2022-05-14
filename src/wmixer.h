// WMixer class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004


#ifndef _WMIXER_Z_
#define _WMIXER_Z_


#include <mmsystem.h>



class WMixer {
public:
	HMIXER Handle;

    WMixer();
    ~WMixer();
    UINT GetNumDevs();
    BOOL Open(UINT uMxId);
    void Close();
    BOOL GetControlID(DWORD ComponentType, DWORD ControlType,DWORD *ControlID);
    BOOL GetVolume(DWORD ControlID, DWORD Channels,
					DWORD MultipleItems,LPDWORD lChannel, LPDWORD rChannel);

    void SetVolume(DWORD ControlID, DWORD Channels,
			DWORD MultipleItems,DWORD lChannel, DWORD rChannel);

    void SetMute(DWORD ControlID,BOOL mute);
    BOOL GetMute(DWORD ControlID);






};






#endif

