/*
 * libwmp3 - multimedia library for playing mp3, mp2 files
 *
 *	NOTE: wrapper functions using __stdcall calling convention
 *        for compatibility with Visual Basic.
 *
 * Copyright (C) 2003-2009 Zoran Cindori ( zcindori@inet.hr )
 *
 * ver: 2.4
 * date: 01 April, 2009.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * or see:
 *  <http://www.gnu.org/licenses/>
 */


#include <windows.h>


#include "wmp3.h"



// =============================================================================================================

CWMp3 * __stdcall  CreateCWMp3() 
{ 
    return new WMp3() ; 
}

// =============================================================================================================

char * __stdcall Mp3_Initialize()
{
	WMp3* mp3 = new WMp3; // create instance of WMp3 class
	return (char*) mp3;
}

// =============================================================================================================

int __stdcall Mp3_Destroy(char *cp)
{
	WMp3* mp3 = (WMp3*) cp;
	if(!mp3)
		return 0;
		
	delete mp3;
	return 1;	
}

// =============================================================================================================

int __stdcall Mp3_GetError(char *objptr, char *pBuffer, int nSize)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	char *msg = mp3->GetError();
	int size = strlen(msg);

	if(pBuffer == 0 || nSize == 0)
		return size + 1;

	if(nSize < size + 1)
		size = nSize - 1;

	memcpy(pBuffer, msg, size);
	pBuffer[size] = 0;
	return size;	
}

// =============================================================================================================


int __stdcall Mp3_OpenFile(
			char *objptr,					// Pointer to class instance.
			const char *sFileName,			// pointer to name of the file 
			int nWaveBufferLengthMs,		// size of wave buffer ( milliseconds ), minimal 200
			unsigned int nSeekFromStart,	// number of bytes to move file pointer from file beginning	 
			unsigned int nFileSize			// size of file counting from nSeekFromStart position
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->OpenFile(sFileName, nWaveBufferLengthMs, nSeekFromStart, nFileSize);
}

// =============================================================================================================



int __stdcall Mp3_OpenStream(
			char *objptr,				// Pointer to class instance.
			int fBuffered,				// flag specifying buffered stream
			int fManaged,				// flag flag for enabling or disabling managed stream
			const void *sMemStream,		// pointer to first memory block
			unsigned int nStreamSize,	// size of memory block in bytes
			int nWaveBufferLengthMs		// size of wave buffer ( milliseconds ), minimal 200
)
{

	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->OpenStream(fBuffered, fManaged, sMemStream, nStreamSize, nWaveBufferLengthMs);

}



// =============================================================================================================

int __stdcall Mp3_OpenResource(
			char *objptr,				// Pointer to class instance.
			HMODULE hModule,				// resource-module handle 
			const char *sResName,			// pointer to resource name 
			const char *sResType,			// pointer to resource type 
			int nWaveBufferLengthMs,		// size of wave buffer ( milliseconds ), minimal 200
			unsigned int nSeekFromStart,	// number of bytes to move file pointer from beginning of resource block	
			unsigned int nResourceSize		// size of resource block counting from nSeekFromStart position
										)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->OpenResource(hModule, sResName, sResType, nWaveBufferLengthMs, nSeekFromStart, nResourceSize);


}


// =============================================================================================================

		
int __stdcall Mp3_Close(
				char *objptr
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->Close();

}


// =============================================================================================================



int __stdcall Mp3_EnumerateWaveOut(
										char *objptr	// Pointer to class instance.
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->EnumerateWaveOut();
}



int __stdcall Mp3_GetWaveOutInfo(
							char *objptr,				// Pointer to class instance.
							unsigned int nIndex,		// index of wave out device
							WAVEOUT_INFO *pWaveOutInfo	// pointer to  WAVEOUT_INFO structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetWaveOutInfo(nIndex, pWaveOutInfo);
}



int __stdcall Mp3_SetWaveOutDevice(
							char *objptr,				// Pointer to class instance.
							unsigned int nIndex	// wave out index
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetWaveOutDevice(nIndex);
}


int __stdcall Mp3_PushDataToStream(
			char *objptr,				// Pointer to class instance.
			const void *sMemNewData,	// pointer to memory block
			unsigned int nNewDataize	// size of memory block in bytes
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->PushDataToStream(sMemNewData, nNewDataize );
}






// =============================================================================================================

		// check if sMemNewData is out of stream

int __stdcall Mp3_IsStreamDataFree(
			char *objptr,				// Pointer to class instance.
			const void *sMemNewData		// pointer to memory block
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->IsStreamDataFree(sMemNewData);
}


// =============================================================================================================

		// return number of buffers in  stream, works only on managed streams
    	
int __stdcall Mp3_GetStreamLoad(
			char *objptr,						// Pointer to class instance.
			STREAM_LOAD_INFO *pStreamLoadInfo	// pointer to STREAM_LOAD_INFO structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetStreamLoad(pStreamLoadInfo);
}


// ----------------------------------------------------------------------------------------

		// start playing from current position

int __stdcall Mp3_Play(
			char *objptr	// Pointer to class instance.
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->Play();
}


// =============================================================================================================

		// play loop

int __stdcall Mp3_PlayLoop(
			char *objptr,				// Pointer to class instance.
			int fFormatStartTime,		// flag specifying start time format
			MP3_TIME *pStartTime,		// pointer to MP3_TIME structure specifying start time
			int fFormatEndTime,			// flag specifying end time format
			MP3_TIME *pEndTime,			// pointer to MP3_TIME structure specifying end time
			unsigned int nNumOfRepeat	// number of loop iterations
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->PlayLoop(fFormatStartTime, pStartTime, fFormatEndTime, pEndTime, nNumOfRepeat);
}



// =============================================================================================================


		// seek position counting from song start

int __stdcall Mp3_Seek(
			char *objptr,				// Pointer to class instance.
			int fFormat,		// flag specifying time format
			MP3_TIME *pTime,	// pointer to MP3_TIME structure specifying time
			int nMoveMethod	// how to move 
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->Seek(fFormat, pTime, nMoveMethod);
}


// =============================================================================================================

		// pause playing

int __stdcall Mp3_Pause(
			char *objptr		// Pointer to class instance.
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->Pause();
}


// =============================================================================================================

		// continue paused playing

int __stdcall Mp3_Resume(
			char *objptr	// Pointer to class instance.
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->Resume();
}



// =============================================================================================================
		// get current position


int __stdcall Mp3_GetPosition(
			char *objptr,	// Pointer to class instance.
			MP3_TIME *pTime	// pointer to MP3_TIME structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetPosition(pTime);
}

		

// =============================================================================================================

		// use internal volume control, modify wave samples, don't change wave out mixer volume

int __stdcall Mp3_InternalVolume(
			char *objptr,	// Pointer to class instance.
			int fEnable		// flag for enabling or disabling internal volume control ( disabled by default )
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->InternalVolume(fEnable);
}


// =============================================================================================================

        // Set output volume

int __stdcall Mp3_SetVolume(
			char *objptr,				// Pointer to class instance.
			unsigned int nLeftVolume,	// left channel volume
			unsigned int nRightVolume	// right channel volume
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetVolume(nLeftVolume, nRightVolume);
}


// =============================================================================================================-

		// get output volume

int __stdcall Mp3_GetVolume(
			char *objptr,				// Pointer to class instance.
			unsigned int *pnLeftVolume,	// pointer to variable receiving left volume
			unsigned int *pnRightVolume	// pointer to variable receiving right volume
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetVolume(pnLeftVolume, pnRightVolume);
}

		//	PARAMETERS:
		//		objptr
		//			Pointer to class instance.
		//
		//		pnLeftVolume
		//			Pointer to variable receiving left volume. Volume range is from 0 to 100
		//
		//		pnRightVolume
		//			Pointer to variable receiving right volume. Volume range is from 0 to 100
		//
		//	RETURN VALUES:
		//		1 - all OK
		//		0 - error, call GetError() to get error string
		//
		//	REMARKS:
		//		If internal volume control is enabled, you get internal volume levels.
		//		If internal volume control is disabled, you get volume level of wave out device.

// =============================================================================================================

        // get MPEG version

int __stdcall Mp3_GetMp3Info(
			char *objptr,			// Pointer to class instance.
			MP3_STREAM_INFO *pInfo	// pointer to MP3_STREAM_INFO structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetMp3Info(pInfo);
}


// =============================================================================================================

		// get song length
	
int  __stdcall Mp3_GetSongLength(
			char *objptr,		// Pointer to class instance.
			MP3_TIME *pLength	// pointer to MP3_TIME structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetSongLength(pLength);
}
	

// =============================================================================================================

		// get bitrate 

int __stdcall Mp3_GetBitrate(
			char *objptr,	// Pointer to class instance.
			int fAverage	// specifies average bitrate of decoded frame  or current bitrate of decoded frame
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetBitrate(fAverage);
}

	
// =============================================================================================================

		// get current status 

int __stdcall Mp3_GetStatus(
			char *objptr,		// Pointer to class instance.
			MP3_STATUS *pStatus	// pointer to MP3_STATUS structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetStatus(pStatus);
}



// =============================================================================================================

		// mix left and right channel into one mono channel
	
int __stdcall Mp3_MixChannels(
			char *objptr,				// Pointer to class instance.
			int fEnable,				// flag for enabling or disabling channel mixing
			unsigned int nLeftPercent,	// percent of left channel in resulting sound ( 0 - 100 )
			unsigned int nRightPercent	// percent of right channel in resulting sound ( 0 - 100 )
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->MixChannels(fEnable, nLeftPercent, nRightPercent);
}


// =============================================================================================================

		// return VU data for left and right channel

int __stdcall Mp3_GetVUData(
			char *objptr,					// Pointer to class instance.
			unsigned int *pnLeftChannel,	// pointer to integer receiving left channel data
			unsigned int *pnRightChannel	// pointer to integer receiving right channel data
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetVUData(pnLeftChannel, pnRightChannel);
}

// =============================================================================================================

		// enable simple vocal cut

int __stdcall Mp3_VocalCut(
			char *objptr,	// Pointer to class instance.
			int fEnable		// flag for enabling or disabling vocal cut
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->VocalCut(fEnable);
}

	

// =============================================================================================================

		// fade volume

int __stdcall Mp3_FadeVolume(
			char *objptr,		// Pointer to class instance.
			int fMode,			// specifies fade mode ( fade in ot fade out )
			int fFormat,		// flag specifying time format
			MP3_TIME *pTime	// pointer to MP3_TIME structure specifying fade interval
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->FadeVolume(fMode, fFormat, pTime);
}

// =============================================================================================================

		// fade volume

int __stdcall Mp3_FadeVolumeEx(
			char *objptr,		// Pointer to class instance.
			int fMode,			// specifies fade mode ( fade in ot fade out )
			int fFormat,		// flag specifying time format
			MP3_TIME *pTime	// pointer to MP3_TIME structure specifying fade interval
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->FadeVolumeEx(fMode, fFormat, pTime);
}

// =============================================================================================================


		// enable equalizer or disable equalizer

int __stdcall Mp3_EnableEQ(
			char *objptr,	// Pointer to class instance.
			int fEnable,	//  flag for enabling or disabling equalizer
			int fExternal	//  flag for internal or external equalizer
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->EnableEQ(fEnable,fExternal );
}



// =============================================================================================================


		// create new equalizer bands for external equalizer

int __stdcall Mp3_CreateEqBands(
			char *objptr,		// Pointer to class instance.
			int *pnFreqPoint,	// pointer to array of integers
			int nNumOfPoints	// number of elements in pnFreqPoint array( min value is 2)
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->CreateEqBands(pnFreqPoint, nNumOfPoints);
}


// =============================================================================================================

		// get eq bands limit frequencies for external equelizer

int __stdcall Mp3_GetEqBands(
			char *objptr,		// Pointer to class instance.
			int *pnFreqPoint	// pointer to array of integers
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetEqBands(pnFreqPoint);
}

	

// =============================================================================================================

		// set equalizer data

int __stdcall Mp3_SetEQParam(
			char *objptr,		// Pointer to class instance.
			int fExternal,		// flag specifying internal or external equalizer
			int nPreAmpGain,	// preamp gain
			int *pnBandGain,	// array of gain for each band
			int nNumberOfBands	// number of frequency bands
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetEQParam(fExternal, nPreAmpGain, pnBandGain, nNumberOfBands);
}


// =============================================================================================================

		// set equalizer data

int __stdcall Mp3_SetEQParamEx(
			char *objptr,		// Pointer to class instance.
			int fExternal,		// flag specifying internal or external equalizer
			int nPreAmpGain,	// preamp gain
			int *pnBandGain,	// array of gain for each band
			int nNumberOfBands	// number of frequency bands
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetEQParamEx(fExternal, nPreAmpGain, pnBandGain, nNumberOfBands);
}


// =============================================================================================================

		// get equalizer data

int __stdcall Mp3_GetEQParam(
			char *objptr,		// Pointer to class instance.
			int fExternal,		// flag specifying internal or external equalizer
			int *pnPreAmpGain,	// pointer to integer receiving preamp gain
			int *pnBandGain		// pointer to variable receiving gain for each band
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetEQParam(fExternal, pnPreAmpGain, pnBandGain);
}



// =============================================================================================================

		// load ID3 data
	
int __stdcall Mp3_LoadID3(
			char *objptr,		// Pointer to class instance.
			int nId3Version,	// ID3 version
			ID3_INFO *pId3Info	// pointer to ID3_INFO structure
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->LoadID3(nId3Version, pId3Info);
}

	

// =============================================================================================================	



		// sound processing echo and reverb
		// enable echo and set echo parematers

		
int __stdcall Mp3_SetSfxParam(
			char *objptr,		// Pointer to class instance.
			int fEnable,		// flag for enabling or disabling sfx processing
			int nMode,			// echo mode
			int nDelayMs,		// delay
			int nInputGain,		// gain of input sound
			int nEchoGain,		// gain of added echo sound
			int nOutputGain		// gain of output sound
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetSfxParam(fEnable, nMode, nDelayMs, nInputGain, nEchoGain, nOutputGain);
}


// =============================================================================================================

		// get sfx param

		
int __stdcall Mp3_GetSfxParam(
			char *objptr,		// Pointer to class instance.
			int *pnMode,		// pointer to variable receiving current mode
			int *pnDelayMs,		// pointer to variable receiving echo delay in millisecond
			int *pnInputGain,	// pointer to variable receiving gain of input sound in decibels
			int *pnEchoGain,	// pointer to variable receiving  gain of added echo sound in decibels
			int *pnOutputGain	// pointer to variable receiving gain of output sound in decibels
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetSfxParam(pnMode, pnDelayMs, pnInputGain, pnEchoGain, pnOutputGain);
}




// =============================================================================================================

		// set sfx mode data

int __stdcall Mp3_SetSfxModeData(
			char *objptr,			// Pointer to class instance.
			int nMode,				// specifies sfx mode
			int *pnModeDataLeft,	// pointer to sfx mode data for left channel
			int *pnModeDataRight,	// pointer to sfx mode data for right channel
			int nSize				// must be ECHO_MAX_DELAY_NUMBER ( 20 )
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetSfxModeData(nMode, pnModeDataLeft, pnModeDataRight, nSize);
}



// =============================================================================================================	


		// get sfx mode data
		// 
		//
		// INPUT:
		//			int nMode				- identify sfx mode ( 0 - 9 ) 
		//			int *pnModeDataLeft		- pointer to variable receiving mode data for left channel
		//			int *pnModeDataRight	- pointer to variable receiving mode data for right channel
		// RETURN:
		//		number of elements in pnModeDataLeft array ( ECHO_MAX_DELAY_NUMBER )
		//
		// REMARKS:
		//		If pnModeDataLeft is 0, function return number of elements needed for pnModeDataLeft array
		//		Read Remarks section for SetSfxModeData function

		
int __stdcall Mp3_GetSfxModeData(
			char *objptr,			// Pointer to class instance.
			int nMode,				// identify sfx mode
			int *pnModeDataLeft,	// pointer to variable receiving mode data for left channel
			int *pnModeDataRight	// pointer to variable receiving mode data for right channel
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetSfxModeData(nMode, pnModeDataLeft, pnModeDataRight);
}


int __stdcall Mp3_GetID3Field(
							char *objptr,		// Pointer to class instance.
							int nId3Version,	// ID3 version
							int nFieldID,		// field ID
							char *pBuffer,		// Pointer to buffer receiving NULL terminated string.
							int nSize			// Size of receiving buffer
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	ID3_INFO id_info;
	if(mp3->LoadID3(nId3Version, &id_info) == 0)
		return 0;

	char *msg;
	int size;

	switch(nFieldID)
	{
		case ID3_TITLE:
			msg = id_info.Title;
		break;

		case ID3_ARTIST:
			msg = id_info.Artist;
		break;

		case ID3_ALBUM:
			msg = id_info.Album;
		break;

		case ID3_GENRE:
			msg = id_info.Genre;
		break;

		case ID3_COMMENT:
			msg = id_info.Comment;
		break;

		case ID3_TRACKNUM:
			msg = id_info.TrackNum;
		break;

		case ID3_YEAR:
			msg = id_info.Year;
		break;

		default:
		return 0;
	}


	size = strlen(msg);

	if(pBuffer == 0 || nSize == 0)
		return size + 1;

	if(nSize < size + 1)
		size = nSize - 1;

	memcpy(pBuffer, msg, size);
	pBuffer[size] = 0;
	return size;	

}




int __stdcall Mp3_Stop(
			char *objptr	// Pointer to class instance.
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->Stop();
}


int __stdcall Mp3_ReverseMode(
			char *objptr,	// Pointer to class instance.
			int fEnable	//  flag for enabling or disabling reverse mode
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->ReverseMode(fEnable);

}



int __stdcall Mp3_GetVersion(
			char *objptr	// Pointer to class instance.
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetVersion();

}


int __stdcall Mp3_SetCallbackFunc(
							char *objptr,
							t_mp3_callback pCallbackFunc, // pointert to callback function
							unsigned int nMessages
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetCallbackFunc(pCallbackFunc, nMessages);

}



int __stdcall Mp3_GetEQParamEx(
			char *objptr,		// Pointer to class instance.
			int fExternal,		// flag specifying internal or external equalizer
			int *pnPreAmpGain,	// pointer to integer receiving preamp gain ( millidecibels)
			int *pnBandGain		// pointer to variable receiving gain for each band ( millidecibels)
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetEQParamEx(fExternal, pnPreAmpGain, pnBandGain);

}


int __stdcall Mp3_SetMasterVolume(
			char *objptr,	// Pointer to class instance.
			unsigned int nLeftVolume,	// left channel volume
			unsigned int nRightVolume	// right channel volume
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetMasterVolume(nLeftVolume, nRightVolume);

}


int __stdcall Mp3_SetPlayerVolume(
			char *objptr,	// Pointer to class instance.
			unsigned int nLeftVolume,	// left channel volume
			unsigned int nRightVolume	// right channel volume
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->SetPlayerVolume(nLeftVolume, nRightVolume);

}

int __stdcall Mp3_GetMasterVolume(
			char *objptr,	// Pointer to class instance.
			unsigned int *pnLeftVolume,	// pointer to variable receiving left volume
			unsigned int *pnRightVolume	// pointer to variable receiving right volume
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetMasterVolume(pnLeftVolume, pnRightVolume);

}


int __stdcall Mp3_GetPlayerVolume(
			char *objptr,	// Pointer to class instance.
			unsigned int *pnLeftVolume,	// pointer to variable receiving left volume
			unsigned int *pnRightVolume	// pointer to variable receiving right volume
)
{
	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;

	return mp3->GetPlayerVolume(pnLeftVolume, pnRightVolume);

}


   	

int __stdcall Mp3_GetFFTValues(
								char *objptr,	// Pointer to class instance.
								int nFFTPoints,			// specify number of fft points. Set this value to:  32, 64, 128, 256, 512, 1024, 2048, 8192, 16384
								int nFFTWindow,		// specify FFT window (	FFT_RECTANGULAR, FFT_HAMMING, FFT_HANN, FFT_COSINE, ... )  	   
								int fFlag,				// flags specifing data we need from this function
								int *pnHarmonicNumber, // pointer to variable receiving number of harmonics
								int *pnHarmonicFreq,
								int *pnLeftAmplitude,	// address of buffer for amplitude of left channel harmonics ( can be NULL )
								int *pnRightAmplitude,	// address of buffer for amplitude of right channel harmonics ( can be NULL )
								int *pnLeftPhase,		// address of buffer for phases of left channel harmonics ( can be NULL )
								int *pnRightPhase		// address of buffer for phases of right channel harmonics ( can be NULL )
)
{

	WMp3* mp3 = (WMp3*) objptr;
	if(!mp3) return 0;


	FFT_STRUCT fftStruct;

	fftStruct.nFFTPoints = nFFTPoints;
	fftStruct.nFFTWindow = nFFTWindow;

	fftStruct.nHarmonicNumber = 0;
	fftStruct.pnHarmonicFreq = 0;
	fftStruct.pnLeftAmplitude = 0;
	fftStruct.pnRightAmplitude = 0;
	fftStruct.pnLeftPhase = 0;
	fftStruct.pnRightPhase = 0;

	if(fFlag & FFT_HARMONIC_FREQ)
		fftStruct.pnHarmonicFreq = pnHarmonicFreq;

	if(fFlag & FFT_LEFT_AMPLITUDE)
		fftStruct.pnLeftAmplitude = pnLeftAmplitude;

	if(fFlag & FFT_RIGHT_AMPLITUDE)
		fftStruct.pnRightAmplitude = pnRightAmplitude;

	if(fFlag & FFT_LEFT_PHASE)
		fftStruct.pnLeftPhase = pnLeftPhase;

	if(fFlag & FFT_RIGHT_PHASE)
		fftStruct.pnRightPhase = pnRightPhase;


	int ret = mp3->GetFFTValues(&fftStruct);

	if(fFlag & FFT_HARMONIC_NUM)
		*pnHarmonicNumber = fftStruct.nHarmonicNumber;

	return ret;

}

