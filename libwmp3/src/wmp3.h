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


#ifndef _WMP3_Z_
#define _WMP3_Z_


#include <windows.h>
#include "libwmp3.h"
#include "wwave.h"
#include "wmp3decoder.h"
#include "wequ.h"
#include "wqueue.h"





typedef struct {
	unsigned char *ptr;
	//unsigned int len;

} MP3_FRAME_PTR;


struct tag_xing {
	long flags;		   /* valid fields (see above) */
	unsigned long frames;	   /* total number of frames */
	unsigned long bytes;	   /* total number of bytes */
	unsigned char toc[100];  /* 100-point seek table */
	long scale;		   /* VBR quality indicator (0 best - 100 worst) */
};


enum rgain_name {
  RGAIN_NAME_NOT_SET    = 0x0,
  RGAIN_NAME_RADIO      = 0x1,
  RGAIN_NAME_AUDIOPHILE = 0x2
};

enum rgain_originator {
  RGAIN_ORIGINATOR_UNSPECIFIED = 0x0,
  RGAIN_ORIGINATOR_PRESET      = 0x1,
  RGAIN_ORIGINATOR_USER        = 0x2,
  RGAIN_ORIGINATOR_AUTOMATIC   = 0x3
};


struct rgain {
  enum rgain_name name;			/* profile (see above) */
  enum rgain_originator originator;	/* source (see above) */
  signed short adjustment;		/* in units of 0.1 dB */
};



enum tag_lame_vbr {
	TAG_LAME_VBR_CONSTANT      = 1,
	TAG_LAME_VBR_ABR           = 2,
	TAG_LAME_VBR_METHOD1       = 3,
	TAG_LAME_VBR_METHOD2       = 4,
	TAG_LAME_VBR_METHOD3       = 5,
	TAG_LAME_VBR_METHOD4       = 6,
	TAG_LAME_VBR_CONSTANT2PASS = 8,
	TAG_LAME_VBR_ABR2PASS      = 9
};

enum tag_lame_source {
	TAG_LAME_SOURCE_32LOWER  = 0x00,
	TAG_LAME_SOURCE_44_1     = 0x01,
	TAG_LAME_SOURCE_48       = 0x02,
	TAG_LAME_SOURCE_HIGHER48 = 0x03
};

enum tag_lame_mode {
	TAG_LAME_MODE_MONO      = 0x00,
	TAG_LAME_MODE_STEREO    = 0x01,
	TAG_LAME_MODE_DUAL      = 0x02,
	TAG_LAME_MODE_JOINT     = 0x03,
	TAG_LAME_MODE_FORCE     = 0x04,
	TAG_LAME_MODE_AUTO      = 0x05,
	TAG_LAME_MODE_INTENSITY = 0x06,
	TAG_LAME_MODE_UNDEFINED = 0x07
};

enum tag_lame_surround {
	TAG_LAME_SURROUND_NONE      = 0,
	TAG_LAME_SURROUND_DPL       = 1,
	TAG_LAME_SURROUND_DPL2      = 2,
	TAG_LAME_SURROUND_AMBISONIC = 3
};

enum tag_lame_preset {
	TAG_LAME_PRESET_NONE          =    0,
	TAG_LAME_PRESET_V9            =  410,
	TAG_LAME_PRESET_V8            =  420,
	TAG_LAME_PRESET_V7            =  430,
	TAG_LAME_PRESET_V6            =  440,
	TAG_LAME_PRESET_V5            =  450,
	TAG_LAME_PRESET_V4            =  460,
	TAG_LAME_PRESET_V3            =  470,
	TAG_LAME_PRESET_V2            =  480,
	TAG_LAME_PRESET_V1            =  490,
	TAG_LAME_PRESET_V0            =  500,
	TAG_LAME_PRESET_R3MIX         = 1000,
	TAG_LAME_PRESET_STANDARD      = 1001,
	TAG_LAME_PRESET_EXTREME       = 1002,
	TAG_LAME_PRESET_INSANE        = 1003,
	TAG_LAME_PRESET_STANDARD_FAST = 1004,
	TAG_LAME_PRESET_EXTREME_FAST  = 1005,
	TAG_LAME_PRESET_MEDIUM        = 1006,
	TAG_LAME_PRESET_MEDIUM_FAST   = 1007,
	TAG_LAME_PRESET_PORTABLE      = 1010,
	TAG_LAME_PRESET_RADIO         = 1015
};


struct tag_lame {
	unsigned char revision;
	unsigned char flags;

	enum tag_lame_vbr vbr_method;
	unsigned short lowpass_filter;

	mad_fixed_t peak;
	struct rgain replay_gain[2];

	unsigned char ath_type;
	unsigned char bitrate;

	int start_delay;
	int end_padding;

	enum tag_lame_source source_samplerate;
	enum tag_lame_mode stereo_mode;
	unsigned char noise_shaping;

	signed char gain;
	enum tag_lame_surround surround;
	enum tag_lame_preset preset;

	unsigned long music_length;
	unsigned short music_crc;
};



struct tag_xl {
	int flags;
	struct tag_xing xing;
	struct tag_lame lame;
	char encoder[21];
};


#define ERRORMESSAGE_SIZE 512 // max error message string size


#define ECHO_BUFFER_NUMBER 40




#define real double


typedef real SFX_MODE[ECHO_MAX_DELAY_NUMBER];



#define mad_fixed_t signed int

typedef struct audio_dither {
  mad_fixed_t error[3];
  mad_fixed_t random;
} AUTODITHER_STRUCT;











typedef struct
{
	char *artist;
	char *album;
	char *title;
	char *comment;
	char *genre;
	char *year;
    char *tracknum;
} ID3STRUCT;



typedef struct {
	unsigned char *stream_start;
	unsigned char *stream_end;
	unsigned char *pos;
	unsigned int stream_size;
	unsigned char *unsync_buf;
	unsigned int MajorVersion;
	unsigned int RevisionNumber;
	unsigned int nFlags;
    struct {
    	BOOL Unsync;
        BOOL Extended;
        BOOL Experimental;
        BOOL Footer;
    } Flags;

	unsigned int TagSize;
	unsigned int TotalSize;
} ID3v2Header;




// WMP3 CLASS  ------------------------------------------------------------------------


class  WMp3 : public CWMp3 {
	public:
        WMp3();  // constructor
        ~WMp3(); // destructor
	
		int __stdcall GetVersion();
		
		char * __stdcall GetError();
		int __stdcall EnumerateWaveOut();
		int __stdcall OpenFile(const char *sFileName, int nWaveBufferLengthMs, unsigned int nSeekFromStart, unsigned int nFileSize);
		int __stdcall OpenStream(int fBuffered, int fManaged, const void *sMemStream, unsigned int nStreamSize,  int nWaveBufferLengthMs);
		int __stdcall PushDataToStream(const void *sMemNewData, unsigned int nNewDataize);
		int __stdcall IsStreamDataFree(const void *sMemNewData);
		int __stdcall GetStreamLoad(STREAM_LOAD_INFO *pStreamLoadInfo);
		int __stdcall OpenResource(HMODULE hModule, const char *sResName, const char *sResType,  int nWaveBufferLengthMs, unsigned int nSeekFromStart, unsigned int nStreamSize);
		int __stdcall Close();
		int __stdcall Play();
		int __stdcall GetWaveOutInfo(unsigned int nIndex,WAVEOUT_INFO *pWaveOutInfo);
		int __stdcall PlayLoop(int fFormatStartTime, MP3_TIME* pStartTime,int fFormatEndTime, MP3_TIME* pEndTime, unsigned int nNumOfRepeat);
		int __stdcall SetWaveOutDevice(unsigned int nIndex);
		int __stdcall Seek(int fFormat,	MP3_TIME *pTime,int nMoveMethod);
        int __stdcall Stop();
        int __stdcall Pause();
        int __stdcall Resume();
        int __stdcall GetPosition(MP3_TIME *pTime);
		int __stdcall GetSongLength(MP3_TIME *pLength);
		int __stdcall InternalVolume(int fEnable);
        int __stdcall SetVolume(unsigned int nLeftVolume, unsigned int nRightVolume);
		int __stdcall GetVolume(unsigned int *pnLeftVolume, unsigned int *pnRightVolume);
        int __stdcall GetMp3Info(MP3_STREAM_INFO *pInfo); // return mpeg version string
        int __stdcall GetBitrate(int fAverage);
        int __stdcall GetStatus(MP3_STATUS *pStatus);
		int __stdcall MixChannels(int fEnable, unsigned int nLeftPercent, unsigned int nRightPercent) ;
		int __stdcall GetVUData(unsigned int *pnLeftChannel, unsigned int *pnRightChannel);
		int __stdcall VocalCut(int fEnable);
		int __stdcall FadeVolume(int fMode, int fFormat,MP3_TIME *pTime);
		int __stdcall EnableEQ(int fEnable, int fExternal);
		int __stdcall CreateEqBands(int *pnFreqPoint, int nNumOfPoints);
		int __stdcall GetEqBands(int *pnFreqPoint);
		int __stdcall SetEQParam(int fExternal, int nPreAmpGain, int *pnBandGain, int nNumberOfBands);
		int __stdcall GetEQParam(int fExternal, int *pnPreAmpGain, int *pnBandGain);
        int __stdcall LoadID3(int nId3Version, ID3_INFO *pId3Info);
		int __stdcall SetSfxParam(int fEnable, int nMode,  int nDelayMs, int nInputGain, int nEchoGain, int nOutputGain);
		int __stdcall GetSfxParam(int *pnMode, int *pnDelayMs, int *pnInputGain, int *pnEchoGain, int *pnOutputGain);
		int __stdcall SetSfxModeData(int nMode,  int *pnModeDataLeft, int *pnModeDataRight, int nSize);
		int __stdcall GetSfxModeData(int nMode,  int *pnModeDataLeft, int *pnModeDataRight);
		int __stdcall ReverseMode(int fEnable);
		void __stdcall Release();
		int __stdcall FadeVolumeEx(int fMode, int fFormat, MP3_TIME *pTime);
		int __stdcall SetEQParamEx(int fExternal,int nPreAmpGain,int *pnBandGain,int nNumberOfBands);
		int __stdcall SetCallbackFunc(t_mp3_callback pCallbackFunc, unsigned int nMessages);
		int __stdcall GetEQParamEx(int fExternal, int *pnPreAmpGain, int *pnBandGain);

		int __stdcall SetMasterVolume(unsigned int nLeftVolume,unsigned int nRightVolume);
		int __stdcall SetPlayerVolume(unsigned int nLeftVolume,unsigned int nRightVolume);
		int __stdcall GetMasterVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume);
		int __stdcall GetPlayerVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume);

		int __stdcall GetFFTValues(FFT_STRUCT *fftStruct);



// ----------------------------------------------------------------------------------------	


		
    private:


		struct mad_stream c_stream; // main mad stream structure ( initialized and free in thread ) 
		struct mad_frame c_frame;	// main mad frame structure ( initialized and free in thread ) 
		struct mad_synth c_synth;	// main mad synth structure ( initialized and free in thread ) 

		unsigned char* c_stream_start; // first byte of input stream ( including ID3 tag )
		unsigned int c_stream_size; // size of input mp3 stream ( whole stream with ID3 tags )
		unsigned char* c_stream_end;  // last byte of input stream ( including ID3 tag )

		unsigned char* c_start;				// first valid frame
		unsigned char* c_end;				// last byte of stream without ID3 tag
		unsigned char* c_position;			// starting playing position in stream
		unsigned int c_current_position;	// current position in playing stream
		unsigned int c_size;			// size of stream (without ID3 tags )


		HANDLE c_hStreamEvent;	// wait for new buffer in dynamic stream
		int c_buffered_stream;	// specify buffered stream
		int c_endstream;		// specify if playing needs to stop if stream runs out of data
		int c_stream_managed;	// specifies managed stream ( can receive new data )
		WNumberQueue c_queue;		// queue for stream playing
		WQueue c_buffered_queue;	// queue for buffered stream playing

		BOOL c_ID3v1;	// ID3v1 tag present
		BOOL c_ID3v2;	// ID3v2 tag present

		unsigned int c_old_sampling_rate;	// old sampling rate, need this for equ_makeTables
		unsigned int c_sampling_rate;		// sampling rate frequency  ( Hz )
		int c_layer;						// layer  1, 2, 3
		int c_mode;							// MAD_MODE_SINGLE_CHANNEL,MAD_MODE_DUAL_CHANNEL,MAD_MODE_JOINT_STEREO,MAD_MODE_STEREO
		int c_channel;						// number of channels
		int c_emphasis;						// emphasis
		int c_mode_extension;				// mode extension
		unsigned int c_bitrate;				// bitrate of first frame
		float c_avg_bitrate;				// average bitrate
		unsigned int c_current_bitrate;		// bitrate of decoding frame
		unsigned int c_frame_counter;		// number of decoded frames
		unsigned int c_bitrate_sum;			// sum of all decoded bitrates
		unsigned int c_nPosition;			// seek position in milliseconds

		float c_avg_frame_size;	// average frame size
		unsigned int c_sample_per_frame;	// number of wave samples in one frame
		int c_flags;

		unsigned int c_mpeg_version;		// MPEG_1, MPEG_2, MPEG_25
		unsigned int c_mpeg_layer_version;	// LAYER_1, LAYER_2 , LAYER_3
		unsigned int c_channel_mode;		// MODE_SINGLE_CHANNEL, MODE_DUAL_CHANNEL, MODE_JOINT_STEREO, MODE_STEREO
		unsigned int c_emphasis_mode;

		unsigned int c_xing_frame_size;
		struct tag_xl xing;
		BOOL c_valid_xing;

		unsigned int c_song_length_samples; // length of song in samples
		unsigned int c_song_length_ms;		// length of song in milliseconds
		unsigned int c_song_length_bytes;	// length of song in bytes
		unsigned int c_song_length_frames;	// length of song in mp3 frames
		unsigned int c_input_buffer_size;	// size of input buffer ( frame number )
		
		unsigned char* c_tmp;		// fix MAD_BUFFER_GUARD ( allocated and free in thread ) 
		unsigned char* c_tmp_ds;	// buffered stream runtime buffer ( allocated and free in thread ) 			

		unsigned int c_play; // playing flag
		unsigned int c_vbr;  // file is vbr or cbr
		unsigned int c_pause;	// pause flag, control Pause() and Reset() 


			// VU meter
		DWORD c_dwPos; // position in current playing wave buffer, need to get corect VU data
		DWORD c_dwIndex; // index of current playing wave buffer
		BOOL c_vudata_ready; // get VU data only when playing



		
		// loop playing
		BOOL c_bLoop;		// indicate loop playing
		unsigned int c_nLoopCount;	// repeat loop n times
		unsigned int c_nLoop;			// number of played loops
		unsigned int c_nLoopFrameNum; // number of mp3 frames needed for one loop
		unsigned int c_nLoopFramePlayed; // number of played mp3 frames in one loop
		BOOL c_bLoopIgnoreFrame;	// ignore all frames in this input buffer
		unsigned int c_dLoopPositionCorrector; // korektor pozicije u pjesmi kad se ponavlja loop
		

		// fade out, fade in
		BOOL c_bFadeOut;
		BOOL c_bFadeIn;
		unsigned int c_nFadeOutFrameNum;
		unsigned int c_nFadeInFrameNum;
		double c_fadeOutStep;
		double c_fadeInStep;
		unsigned int c_fade_frame_count;
		// vocal cut
		BOOL c_bVocalCut; // enable/disable vocal cut processing

		
		// echo and reverb
		BOOL c_echo; // enable echo processing 
		real c_rIgOg;
		real c_rEgOg;
		unsigned int c_delay; // echo delay
		int c_old_echo_buffer_size; // use to check if we need to allocate new echo buffers
		int c_echo_alloc_buffer_size; // size of allocated echo buffer
		SFX_MODE c_sfx_mode_l; // SFX_MODE data, use this data to add echo
		SFX_MODE c_sfx_mode_r; // SFX_MODE data, use this data to add echo
		int c_echo_mode; // current echo_mode
		int c_sfx_offset; // offset ( number of echo buffers to skip into past )
		short* sfx_buffer[ECHO_BUFFER_NUMBER]; // circular echo wave buffers, each buffer contains small wave chunk ( about 50 ms )
		short* sfx_buffer1[ECHO_BUFFER_NUMBER]; // there are 40 buffers ( 40 * 50 ms = 2 sec )
		int c_nInputGain;
		int c_nEchoGain;
		int c_nOutputGain;
		int c_nDelayMs;


		// equalizer
		paramlist c_paramroot;// eqalizer param


		int c_bands_num;	// number of  bands
		int *c_eq;			// equalizer data 
		int c_eq_preamp;	// preamp value

		int c_eq_internal_preamp;

		REAL *c_lbands;		// eqalizer param, internal values,  goes to equ_MakeTable
		REAL *c_rbands;		// eqalizer param, internal values, goes to equ_MakeTable

		
		int c_eq_band_points_num;	// number of eq band points
		REAL *c_eq_band_points;		// custom eq bans frequency points


		

		int c_EqFilter1[10];
		mad_fixed_t	c_EqFilter[32];
		BOOL c_bUseInternalEQ;  // enable internal equalizer
		BOOL c_bUseExternalEQ;	// enable external equalizer
		BOOL c_bEQIn;	
		//BOOL c_bChangeEQ; // trigger for EQ parameter change
		char* c_gapless_buffer;
		unsigned int c_byte_to_skip;
		HANDLE c_hThreadEqSetParam;	// thread for calculating EQ parameters, active only when external eq enabled
		DWORD c_hThreadEqSetParamId;
		HANDLE c_hEventEqSetParam;	// inidicates start of c_hThreadEqSetParam thread
		static int WINAPI EqParamThreadFunc(void* lpdwParam);
		
		
		// internal volume
		BOOL c_internalVolume; // adjust internal volume ( process wave samples )

        unsigned int c_lvolume; // left channel volume for internal volume control
        unsigned int c_rvolume; // right channel volume for internal volume control
		
		
		mad_fixed_t gain[2]; // internal gain	


		// channel mixer
		BOOL c_mix_channels;	// enable channel mixing
		mad_fixed_t c_l_mix;
		mad_fixed_t c_r_mix;

		// dithering
		AUTODITHER_STRUCT left_dither;
		AUTODITHER_STRUCT right_dither;
		

		// disk file mapping
		HANDLE w_hFile;		// handle of opened mp3 file
		HANDLE w_hFileMap;
		unsigned char* w_mapping;	

	
		WWaveOut* w_wave; // wave out class, initialize wave out, play wave data to soundcard
		HANDLE w_hThread; // playing thread, decode mp3, run sound processing, send wave data to soundcard
		DWORD w_hThreadID; // playing thread ID
		HANDLE w_hEvent;	// event, trigger on wave header DONE, decode new mp3 data and send buffer to soundcard
		//CRITICAL_SECTION c_wcsThreadDone; // assurance for Stop() function, when Stop() function exits, playing thread must be dead
		CRITICAL_SECTION c_wcsSfx; //  protect sound processing functions ( allocation and deallocation of buffers ) from SetSfxParam
		
		CRITICAL_SECTION c_wcs_fade;
		HANDLE c_hThreadStartEvent;
		int c_thread_no_error;
       
        ID3STRUCT w_myID3; // ID3 structure, LoadID3(...) fill this structure with ID3 data
	

		unsigned int c_nFreeBitrate;

	
		MP3_STATUS c_status;
		
		
		void ClearSfxBuffers(); // clear all echo buffers
		void error(char* err_message);
		static void  DoEcho(WMp3* mp3, short* buf, unsigned int len); // process adion data and add echo
		static BOOL _LoadID3(WMp3* mp3,int id3Version); // load ID3 data
		static char* _RetrField(WMp3* mp3, char *identifier, DWORD* Size);
		static int WINAPI _ThreadFunc(void* lpdwParam);
		static void CALLBACK _WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
		static int reallocate_sfx_buffers(WMp3* mp3);
		static unsigned long _Get32bits(unsigned char *buf);
        static DWORD _RemoveLeadingNULL( char *str , DWORD size);
        static BOOL _DecodeFrameHeader(WMp3* mp3, char *header, char *identifier);
        static DWORD _DecodeUnsync(unsigned char* header, DWORD size);
		static void init(WMp3* mp3);	// initialize all parameters
		static int _OpenMp3(WMp3* mp3, int WaveBufferLength);


		static void cf_clear(WMp3* mp3); // clear and initialize all playing memory

		static void _GetInternalVolume(WMp3 *mp3, unsigned int *lvolume, unsigned int *rvolume);
		static void internal_volume(WMp3 *mp3, unsigned char *buf, unsigned int size, int bit, int ch);
		static void s_vocal_cut(unsigned char *buf, unsigned int size, int bit, int ch);

		static int DecodeID3v2Header(WMp3 *mp3);
		static int decode_id3v2_2(WMp3 *mp3);
		static int decode_id3v2_3(WMp3 *mp3);
		static int decode_id3v2_4(WMp3 *mp3);
		

		
	
		char _lpErrorMessage[ERRORMESSAGE_SIZE];
		double _fade_volume;
		int flying_loop;	// loop playing stream, dont stop after loop is done, continue
		int c_in_loop;

		EQ_INTER_PARAM eq_inter_param;

		unsigned int c_woc_num;	// number of wave output devices
		unsigned int c_current_woc;	// current wave out device
		WAVEOUTCAPS *c_pwoc; // wave output devices

		int c_skip_xing;	// don't use xing frame to determine song length

		int c_fReverseMode;
		unsigned int c_nFrameNumber;
		unsigned int c_nFrameOverlay;

		MP3_FRAME_PTR *c_pFramePtr;


		double c_dLeftGainVolume;		// left channel gain ( for internal volume  )
		double c_dRightGainVolume;	// right channel gain ( for internal volume  )
		double c_dLeftGainFade;		// left channel gain ( for fade effect  )
		double c_dRightGainFade;	// right channel gain ( for fade effect )
		double c_dLeftGain;		// left channel gain ( internal volume + fade  )
		double c_dRightGain;	// right channel gain ( internal volume + fade  )


		t_mp3_callback callback_func; // pointer to callback function
		unsigned int callback_messages;	// callback mode
		HANDLE c_hThreadCallback;
		DWORD c_hThreadCallbackId;
		HANDLE c_hEventCallback;
		static int WINAPI CallbackThreadFunc(void* lpdwParam);

		int c_enter_loop_sent;	// indicates that enter loop message was send
		int c_enter_fade_out_sent;
		int c_exit_fade_out_sent;
		int c_exit_loop_sent;
		int c_enter_fade_in_sent;
		int c_exit_fade_in_sent;
		unsigned int c_nInterPosition; // internal calculated position for callback function

		int c_no_audio_output;
		
		ID3v2Header c_id3header;

		static int send_stop(WMp3 *mp3);
		static int send_fadeout_exit(WMp3 *mp3);
		static int send_fadein_exit(WMp3 *mp3);

		static int send_fadeout_enter(WMp3 *mp3);
		static int send_fadein_enter(WMp3 *mp3);

		static int send_loop_enter(WMp3 *mp3);
		static int send_loop_exit(WMp3 *mp3);

		static int send_buffer_done(WMp3 *mp3, unsigned int count, unsigned int sumsize);
		static int send_stream_out_of_data(WMp3 *mp3);


		static int send_callback_message(WMp3 *mp3, unsigned int message, unsigned int param1, unsigned int param2);
		int c_callback_ret;


		// fft processing

		REAL *c_pflSamples;	// samples buffer
		REAL *c_pflWindow;	// samples buffer
		int *c_pnIp;		// work area for bit reversal
		REAL *c_pflw;		// cos/sin table
		int c_nFFTPoints;
		int c_nFFTWindow;
		

		static void free_fft(WMp3 *mp3);
		static int allocate_fft(WMp3 *mp3);
		static void set_fft_window(REAL *pflWindow, int nFFTPoints, int nWindow);
		static void load_fft_buffer(WMp3 *mp3, int right);




};



#endif

