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
#include <crtdbg.h>
#include <stdio.h>
#include <math.h>

#if _DEBUG
	#define ASSERT_W(f)	_ASSERT(f)
#else
	#define ASSERT_W(f) NULL
#endif

#define PI2 6.283185307179586476925286766559

#define PI	3.1415926535897932384626433832795



#include "wmp3decoder.h"
#include "wmp3.h"



#define GetBit(value, pos) ( (value >> pos) & 0x01)
#define DecodeSyncSafeInteger4(value1, value2, value3, value4) (((value1 & 0x7F) << 21) | ((value2 & 0x7F) << 14) | ((value3 & 0x7F) << 7) | (value4 & 0x7F))
#define DecodeSyncSafeInteger3(value1, value2, value3) (((value1 & 0x7F) << 14) | ((value2 & 0x7F) << 7) | (value3 & 0x7F))



unsigned int id3_deunsynchronise(unsigned char *data, unsigned int length);
unsigned char *id3_get_str(unsigned char *buf, unsigned int *len);
unsigned char *id3_get_comment_str(unsigned char *buf, unsigned int *len);
char *get_genre_from_index(char *ptr, unsigned int size);
unsigned int id3_decode_str(unsigned char *str, unsigned int length, unsigned char *dest, unsigned int encoding);


#define LIBRARY_VERSION 240


#define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
						 (((y) + (1L << 15)) >> 16))


 
unsigned int Mp3TimeToMs(int fFormat, MP3_TIME *pTime);
void reverse_buffer(unsigned char *buf, unsigned int size, int bit, int ch);

# define RGAIN_REFERENCE  83		/* reference level (dB SPL) */

// skip ID3v2 tag at the beginning of file
#define SKIP_ID3TAG_AT_BEGINNING 1


# define RGAIN_SET(rgain)	((rgain)->name != RGAIN_NAME_NOT_SET)

# define RGAIN_VALID(rgain)  \
  (((rgain)->name == RGAIN_NAME_RADIO ||  \
    (rgain)->name == RGAIN_NAME_AUDIOPHILE) &&  \
   (rgain)->originator != RGAIN_ORIGINATOR_UNSPECIFIED)

# define RGAIN_DB(rgain)  ((rgain)->adjustment / 10.0)

void rgain_parse(struct rgain *, struct mad_bitptr *);
char const *rgain_originator(struct rgain const *);

unsigned short crc_compute(char const *data, unsigned int length, unsigned short init);

enum {
	TAG_XING = 0x0001,
	TAG_LAME = 0x0002,
	TAG_VBRI = 0x0004,
	TAG_VBR  = 0x0100
};

enum {
	TAG_XING_FRAMES = 0x00000001L,
	TAG_XING_BYTES  = 0x00000002L,
	TAG_XING_TOC    = 0x00000004L,
	TAG_XING_SCALE  = 0x00000008L
};


enum {
	TAG_LAME_NSPSYTUNE    = 0x01,
	TAG_LAME_NSSAFEJOINT  = 0x02,
	TAG_LAME_NOGAP_NEXT   = 0x04,
	TAG_LAME_NOGAP_PREV   = 0x08,

	TAG_LAME_UNWISE       = 0x10
};




void tag_init(struct tag_xl *);

# define tag_finish(tag)	/* nothing */

int tag_parse(struct tag_xl *, struct mad_stream const *, struct mad_frame const *);


void mono_filter(struct mad_frame *frame);
void gain_filter(struct mad_frame *frame, mad_fixed_t gain[2]);
void experimental_filter(struct mad_frame *frame);

void mix_filter(struct mad_frame *frame, mad_fixed_t left_p, mad_fixed_t right_p);
void vocal_cut_filter(struct mad_frame *frame);







void alert(char* str) {
	MessageBox(0,str,"char",MB_OK);


}
void alert(unsigned char* str) {
	MessageBox(0,(char*) str,"unsigned char",MB_OK);

}

void alert(int num) {
	char tmp[100];
	sprintf(tmp,"%i",num);
	MessageBox(0,tmp,"int",MB_OK);


}

void alert(unsigned int num) {
	char tmp[100];
	sprintf(tmp,"%u",num);
	MessageBox(0,tmp,"unsigned int",MB_OK);


}

void alert(float num) {
	char tmp[100];
	sprintf(tmp,"%f",num);
	MessageBox(0,tmp,"float",MB_OK);
}


void alert(double num) {
	char tmp[100];
	sprintf(tmp,"%f",num);
	MessageBox(0,tmp,"double",MB_OK);
}


void alerti(int num) {
	char tmp[100];
	sprintf(tmp,"%i",num);
	MessageBox(0,tmp,"Message",MB_OK);


}

void alertn(unsigned int num) {
	char tmp[100];
	sprintf(tmp,"%u",num);
	MessageBox(0,tmp,"Message",MB_OK);


}

void alertf(float num) {
	char tmp[100];
	sprintf(tmp,"%f",num);
	MessageBox(0,tmp,"Message",MB_OK);


}

void beep() {
	MessageBeep(0);
}



// eq param thread messages
#define MSG_EQ_PARAM_SET WM_USER + 1



#define AVG_SINE_16 23169.0 // average sine size in 16 bit audio data, need to calculate VU data
#define SINGLE_AUDIO_BUFFER_SIZE 100  // single auio buffer size in ms ( 100 ms each buffer )
#define SEARCH_DEEP_VBR 200 // check next 200 mp3 frames to determine constant or variable bitrate if no XING header found 
#define MIN_FRAME_SIZE 24 // minimal mp3 frame size
#define MAX_FRAME_SIZE 5761 // max frame size



const char empty_string[2] = "\0";


void RemoveEndingSpaces(char* buff);	
unsigned long Get32bits(unsigned char *buf);



// echo processing

#define NUM_SFX_MODE 10 // number of ECHO modes
#define MAX_DELAY 1000 // max echo delay in ms ( 1000 ms = 1 sec )

// SFX_MODE
// 10 echo modes, separate left and right channel.
// there are 20 echo buffers, each buffer has about 50 milliseconds audio data.
// 20 bufers * 50 ms = 1 second past audio data.
// first value represents current playing buffer, second is 50 ms old, third is 100 ms old ....
// buffer contains half volume audio data.
// value in sfx_mode multiply each audio sample volume
// example
// for 50 ms delay set { 0, 0.7, 0, 0, 0, 0, ......
// for e.g. 150 ms delay set {0, 0, 0, 0.6, 0, 0, ....
// for multiple echo set e.g. {0.5, 0.2, 0, 0, 0, 0.3, 0.2, .....
//
// delay value in SetSfxParam determines delay for first value in sfx_mode
//
// changing values in sfx_mode you can create various reverb and multiple echo effects 
 
SFX_MODE sfx_mode[NUM_SFX_MODE * 2] = {
// mode 0
{0.5,0.2 ,0.1 ,0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0, 0, 0, 0, 0, 0}, // left channel
{0.5,0.2 ,0.1 ,0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0, 0, 0, 0, 0, 0}, // right channel
// mode 1
{0.2,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0.2,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
// mode 2
{0.5,0.3,0.1,0.3,0.2,0.1,0.2,0.1,0.3,0.2,0.1,0.1,0.05,0.03,0.2,0.1,0.05,0.1,0.02,0.01},
{0.5,0.3,0.1,0.3,0.2,0.1,0.2,0.1,0.3,0.2,0.1,0.1,0.05,0.03,0.2,0.1,0.05,0.1,0.02,0.01},
// mode 3
{0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.1,0.07,0.05},
{0, 0, 0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.02},
// mode 4
{0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.1,0.07,0.05},
{0.5,0.1,0,0,0,0,0,0,0,0.5,0.2,0.1,0,0,0,0,0,0.3,0.2,0.04},
// mode 5
{0.5,0.2,0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1},
{0.5,0.2,0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1},
// mode 6
{0.3,0.1,0.2,0.05,0.1,0.05,0.05,0.025,0.1,0.05,0.1,0.05,0.1,0.01,0.05,0.01,0.1,0.01,0.1,0.01},
{0.3,0.1,0.2,0.05,0.1,0.05,0.05,0.025,0.1,0.05,0.1,0.05,0.1,0.01,0.05,0.01,0.1,0.01,0.1,0.01},
// mode 7
{0.5,0.4,0.2,0.1,0.025,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1,0.05,0.01},
{0,0,0,0.5,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0.3,0.2,0.05,0.01},
// mode 8
{0.5,0.3,0.2,0.1,0.05,0.025,0.017,0.005,0,0,0,0,0,0,0,0,0,0.1,0.05,0.025},
{0.5,0.3,0.2,0.1,0.05,0.025,0.017,0.005,0,0,0,0,0,0,0,0,0,0.1,0.05,0.025},
// mode 9
{0.5,0.2,0.1,0.3,0.2,0.1,0.2,0.1,0.05,0.1,0.05,0.025,0.1,0.05,0.05,0.025,0.1,0.05,0.025,0.01},
{0.5,0.2,0.1,0.3,0.2,0.1,0.2,0.1,0.05,0.1,0.05,0.025,0.1,0.05,0.05,0.025,0.1,0.05,0.025,0.01}

};




const char *sGenres [] =
	{
		"Blues",
		"Classic Rock",
		"Country",
		"Dance",
		"Disco",
		"Funk",
		"Grunge",
		"Hip-Hop",
		"Jazz",
		"Metal",
		"New Age",
		"Oldies",
		"Other",
		"Pop",
		"R&B",
		"Rap",
		"Reggae",
		"Rock",
		"Techno",
		"Industrial",
		"Alternative",
		"Ska",
		"Death Metal",
		"Pranks",
		"Soundtrack",
		"Euro-Techno",
		"Ambient",
		"Trip Hop",
		"Vocal",
		"Jazz+Funk",
		"Fusion",
		"Trance",
		"Classical",
		"Instrumental",
		"Acid",
		"House",
		"Game",
		"Sound Clip",
		"Gospel",
		"Noise",
		"Alternative Rock",
		"Bass",
		"Soul",
		"Punk",
		"Space",
		"Meditative",
		"Instrumental Pop",
		"Instrumental Rock",
		"Ethnic",
		"Gothic",
		"Darkwave",
		"Techno-Industrial",
		"Electronic",
		"Pop-Folk",
		"Eurodance",
		"Dream",
		"Southern Rock",
		"Comedy",
		"Cult",
		"Gangsta Rap",
		"Top 40",
		"Christian Rap",
		"Pop/Punk",
		"Jungle",
		"Native American",
		"Cabaret",
		"New Wave",
		"Phychedelic",
		"Rave",
		"Showtunes",
		"Trailer",
		"Lo-Fi",
		"Tribal",
		"Acid Punk",
		"Acid Jazz",
		"Polka",
		"Retro",
		"Musical",
		"Rock & Roll",
		"Hard Rock",
		"Folk",
		"Folk/Rock",
		"National Folk",
		"Swing",
		"Fast-Fusion",
		"Bebob",
		"Latin",
		"Revival",
		"Celtic",
		"Blue Grass",
		"Avantegarde",
		"Gothic Rock",
		"Progressive Rock",
		"Psychedelic Rock",
		"Symphonic Rock",
		"Slow Rock",
		"Big Band",
		"Chorus",
		"Easy Listening",
		"Acoustic",
		"Humour",
		"Speech",
		"Chanson",
		"Opera",
		"Chamber Music",
		"Sonata",
		"Symphony",
		"Booty Bass",
		"Primus",
		"Porn Groove",
		"Satire",
		"Slow Jam",
		"Club",
		"Tango",
		"Samba",
		"Folklore",
		"Ballad",
		"power Ballad",
		"Rhythmic Soul",
		"Freestyle",
		"Duet",
		"Punk Rock",
		"Drum Solo",
		"A Capella",
		"Euro-House",
		"Dance Hall",
		"Goa",
		"Drum & Bass",
		"Club-House",
		"Hardcore",
		"Terror",
		"indie",
		"Brit Pop",
		"Negerpunk",
		"Polsk Punk",
		"Beat",
		"Christian Gangsta Rap",
		"Heavy Metal",
		"Black Metal",
		"Crossover",
		"Comteporary Christian",
		"Christian Rock",
		"Merengue",
		"Salsa",
		"Trash Metal",
		"Anime",
		"JPop",
		"Synth Pop"
	};




#define GetFourByteSyncSafe(value1, value2, value3, value4) (((value1 & 255) << 21) | ((value2 & 255) << 14) | ((value3 & 255) << 7) | (value4 & 255))
#define CreateFourByteField(value1, value2, value3, value4) (((value1 & 255) << 24) | ((value2 & 255) << 16) | ((value3 & 255) << 8) | (value4 & 255))
#define ExtractBits(value, start, length) (( value >> start ) & (( 1 << length ) -1 ))




#define FFT_POINTS_NUM 9
int FFT_Points_Table[FFT_POINTS_NUM] = {32, 64, 128, 256, 512, 1024, 2048, 8192, 16384};



WMp3::WMp3()
{
	c_hThreadEqSetParam = 0;	// thread for calculating EQ parameters, active only when external eq enabled
	c_hThreadEqSetParamId = 0;
	c_hEventEqSetParam = 0;


	// fft
	c_pflSamples = 0;	// samples buffer
	c_pflWindow = 0;
	c_pnIp = 0;
	c_pflw = 0;
	c_nFFTPoints = 0;
	c_nFFTWindow = 0;



	c_hThreadCallback = 0;
	c_hThreadCallbackId = 0;
	c_hEventCallback = 0;


	memset(&c_id3header, 0, sizeof(ID3v2Header));

	callback_func = 0;
	callback_messages = 0;





	c_nFreeBitrate = 0;

	c_dLeftGain = 1.0;
	c_dRightGain = 1.0;
	
	c_dLeftGainVolume = 1.0;		// left channel gain ( for internal volume  )
	c_dRightGainVolume = 1.0;	// right channel gain ( for internal volume  )
	c_dLeftGainFade = 1.0;		// left channel gain ( for fade effect  )
	c_dRightGainFade = 1.0;	// right channel gain ( for fade effect )


	c_nFrameOverlay = 2; // overlay frames when playing reverse

	c_fReverseMode = 0;
	c_pFramePtr = 0;
	c_nFrameNumber = 0;


	c_current_position = 0;

	c_frame_counter = 0;



	c_woc_num = 0;	// number of wave output devices
	c_current_woc = WAVE_OUT_WAVE_MAPPER;	// current wave out device
	c_pwoc = 0; // wave output devices

	c_thread_no_error = 1;
	c_buffered_stream = 0;
	c_endstream = 0;


	memset(&eq_inter_param, 0, sizeof(EQ_INTER_PARAM));
	eq_inter_param.weq_enable = 1;
	eq_inter_param.dither = 0;

	// initialize eq with default eq bands
	equ_init(&eq_inter_param, 14);
	





	c_eq_band_points_num = eq_inter_param.internal_band_points_num + 2;
	c_bands_num = eq_inter_param.internal_band_points_num + 1;

	c_eq = (int*) malloc(c_bands_num * sizeof(int)); // equalizer data
	c_lbands = (REAL*) malloc(c_bands_num * sizeof(REAL));	// eqalizer param
	c_rbands = (REAL*) malloc(c_bands_num * sizeof(REAL));	// eqalizer param

	c_eq_band_points = (REAL*) malloc(c_eq_band_points_num * sizeof(REAL));	// eqalizer param
	
	c_eq_internal_preamp = 0;

	// initialize eq
	int i;
	c_eq_preamp = 0;
	for (i = 0; i < c_bands_num; i++)
	{
		c_lbands[i] = 1.0;
		c_rbands[i]= 1.0;
		c_eq[i] = 0;
	}

	// get internal bands
	c_eq_band_points[0] = 0.0;


	for(i = 0; i < 10; i++)
	{
		c_EqFilter1[i] = 0;
	}



	for (i = 1; i < c_eq_band_points_num - 1; i++)
		c_eq_band_points[i] = eq_inter_param.internal_band_points[i - 1];


	c_eq_band_points[c_eq_band_points_num - 1] = EQ_MAX_FREQ;


	c_in_loop = 0;
	c_mix_channels = FALSE;
	c_current_bitrate = 0;

	gain[0] = MAD_F_ONE;
	gain[1] = MAD_F_ONE;

	// sfx 
	c_nInputGain = 0;
	c_nEchoGain = 0;
	c_nOutputGain = 0;
	c_nDelayMs = 0;


	w_hFile = INVALID_HANDLE_VALUE;	// handle of opened mp3 file
	w_hFileMap = NULL;

	c_bUseInternalEQ = FALSE;
	c_internalVolume = FALSE;


	c_bEQIn = FALSE;



	

	c_bUseExternalEQ = 0; // use external equalizer



	for(i = 0; i < 32; i++) 
		c_EqFilter[i] = MAD_F_ONE;






	c_bVocalCut = 0; // start with disabled echo cut


// sfx
	c_echo = 0; // start with disabled echo processing
	c_echo_mode = 0; // starting echo mode

	c_delay = 0;	// delay 0 ms, but some echo modes produces delay with this value set to 0

	c_rIgOg = 1.0;
	c_rEgOg = 1.0;


	// set all pointers to echo buffers to 0, there is no alocated buffer
	for(i = 0; i < ECHO_BUFFER_NUMBER; i++)
		sfx_buffer[i] = 0;
	
	
	c_echo_alloc_buffer_size = 0; // size of single circular echo buffer
	c_old_echo_buffer_size = 0; // need this to check if we need to reallocate memory for echo buffers
				

// sfx end

	




// VU data variables
	c_dwPos = 0;
	c_dwIndex = 0;
	c_vudata_ready = FALSE;



  // internal volume control
    c_lvolume = 100;
    c_rvolume = 100;





	InitializeCriticalSection(&c_wcsSfx);
	InitializeCriticalSection(&c_wcs_fade);

	


    w_wave = new WWaveOut(); // class for playing wave buffers to soundcard


    w_hThread = 0; // playing thread handle
	w_hEvent = 0; // event handle 


    // initialize ID3 fielda
    w_myID3.album = (char*) malloc(2);
	*w_myID3.album = 0;
    w_myID3.artist = (char*) malloc(2);
	*w_myID3.artist = 0;
    w_myID3.comment = (char*) malloc(2);
	*w_myID3.comment = 0;
    w_myID3.genre = (char*) malloc(2);
	*w_myID3.genre = 0;
    w_myID3.title = (char*) malloc(2);
	*w_myID3.title = 0;
    w_myID3.year = (char*) malloc(2);
	*w_myID3.year = 0;
    w_myID3.tracknum = (char*) malloc(2);
	*w_myID3.tracknum = 0;


// initialize equalizer

	c_old_sampling_rate = 0;

	c_gapless_buffer = 0;
	


// stream event

	c_hStreamEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	c_stream_managed = 0;
	c_tmp_ds = 0;

	c_hThreadStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);


	c_hEventCallback = CreateEvent(NULL, TRUE, FALSE, NULL);

	init(this);


}


WMp3::~WMp3()
{


 	Stop(); // stop playing



	if(w_myID3.album)
    	free(w_myID3.album);

    if(w_myID3.artist)
    	free(w_myID3.artist);
 
    if(w_myID3.comment)
    	free(w_myID3.comment);

    if(w_myID3.title)
    	free(w_myID3.title);

    if(w_myID3.year)
    	free(w_myID3.year);

    if(w_myID3.tracknum)
    	free(w_myID3.tracknum);

	if(w_myID3.genre)
    	free(w_myID3.genre);


	Close();





	for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
		if(sfx_buffer[i])
			free(sfx_buffer[i]);


// free critical sections		


	DeleteCriticalSection(&c_wcsSfx);
	DeleteCriticalSection(&c_wcs_fade);


	if(w_wave) // delete wwaveoutclass
		delete w_wave;

// free equalizer

	equ_quit(&eq_inter_param);


	if(c_gapless_buffer)
		free(c_gapless_buffer);



	if(c_eq)
		free(c_eq);

	if(c_lbands)
		free(c_lbands);

	if(c_rbands)
		free(c_rbands);

	if(c_eq_band_points)
		free(c_eq_band_points);


	

	CloseHandle(c_hThreadStartEvent);

	c_woc_num = 0;	// number of wave output devices
	c_current_woc = 0;	// current wave out device
	if(c_pwoc)
		free(c_pwoc);
	c_pwoc = 0;




	SetCallbackFunc(0,0);	// destroy cllback thread

	CloseHandle(c_hEventCallback);





// destructor

	CloseHandle(c_hStreamEvent);

	if(c_hThreadEqSetParam)
	{
		PostThreadMessage(c_hThreadEqSetParamId, WM_QUIT, 0,0);
		if(WaitForSingleObject(c_hThreadEqSetParam, 500) == WAIT_TIMEOUT)
			TerminateThread(c_hThreadEqSetParam, 0);
	}

	CloseHandle(c_hThreadEqSetParam);

	if(c_hEventEqSetParam)
		CloseHandle(c_hEventEqSetParam);

	free_fft(this);


}





int __stdcall WMp3::OpenFile(const char *sFileName, int nWaveBufferLengthMs, unsigned int nSeekFromStart, unsigned int nFileSize)
{
	Close();

	c_skip_xing = 0;

	c_stream_managed = 0;
	if(c_stream_start)
	{
		error("OpenMp3File->You need to close mp3 file before open new one !");
		return 0;
	
	}

// create file mapping
	
	w_hFile = CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);	
	if(w_hFile ==  INVALID_HANDLE_VALUE)
	{
		sprintf(_lpErrorMessage,"OpenMp3FileEx->Can't open file:  %s  !",sFileName);
        return 0;	
	}

	c_stream_size = GetFileSize(w_hFile,NULL);
	if(c_stream_size == 0xFFFFFFFF)
	{
		sprintf(_lpErrorMessage,"OpenMp3FileEx->Can't open file:  %s  !",sFileName);
		CloseHandle(w_hFile);
		w_hFile = INVALID_HANDLE_VALUE;
		c_stream_size = 0;
        return 0;
	}

/*
	if(nFileSize != c_stream_size)
		c_skip_xing = 1;
*/


	if(nSeekFromStart >= c_stream_size)
	{
		error("OpenMp3FileEx->Seek value out of range !");
		CloseHandle(w_hFile);
		w_hFile = INVALID_HANDLE_VALUE;
		c_stream_size = 0;
        return 0;
	}

	if(nFileSize == 0)
		nFileSize = c_stream_size - nSeekFromStart;



	if(nFileSize > (c_stream_size - nSeekFromStart))
	{
		error("OpenMp3FileEx->Size value out of range !");
		CloseHandle(w_hFile);
		w_hFile = INVALID_HANDLE_VALUE;
		c_stream_size = 0;
        return 0;
	}

	w_hFileMap = CreateFileMapping(w_hFile,NULL,PAGE_READONLY,0,0,NULL);
	if(w_hFileMap == NULL)
	{
		error("OpenMp3FileEx->Can't create file mapping !");
		CloseHandle(w_hFile);
		w_hFile = INVALID_HANDLE_VALUE;
		c_stream_size = 0;
        return 0;

	}

	w_mapping = (unsigned char*) MapViewOfFile(w_hFileMap,FILE_MAP_READ,0,0,0);
	if(w_mapping == NULL)
	{
		error("WMp3::OpenMp3File->Can't create file view !");
		CloseHandle(w_hFileMap);
		CloseHandle(w_hFile);
		w_hFile = INVALID_HANDLE_VALUE;
		c_stream_size = 0;
        return 0;

	}

	c_stream_start = w_mapping + nSeekFromStart;
	if(nFileSize)
		c_stream_size = nFileSize;

	c_stream_end = c_stream_start + c_stream_size - 1;

	return _OpenMp3(this, nWaveBufferLengthMs);


}









int __stdcall WMp3::Stop()
{
// stop playing


// prevent program crash if we call this function on empty ( not opened ) mp3
	if(!c_start)
	{
		error("Stop->Stream isn't open !");
		return 0;
	}
	
	if(!c_play)
		return 0;

	// disable VU data, don't waste processor (CPU) time
	c_vudata_ready = FALSE;

	

	c_bLoop  = 0;
	c_in_loop = 0;

// pause playing, 
	w_wave->Pause();

// stop decoder loop and decoding thread		
	c_play = 0;
    c_pause = 0;

	SetEvent(c_hStreamEvent);
	SetEvent(c_hStreamEvent);
	SetEvent(c_hStreamEvent);
	
	w_wave->Stop(); // stop wave playing
	
// ensure that thread will end
	SetEvent(w_hEvent);
	SetEvent(w_hEvent);
	SetEvent(w_hEvent);
	SetEvent(w_hEvent);

	SetEvent(c_hStreamEvent);
	SetEvent(c_hStreamEvent);
	SetEvent(c_hStreamEvent);


// be sure that playing thread is ended

	WaitForSingleObject(w_hThread, INFINITE);


	_fade_volume = 100.0;
	flying_loop = 0;

	if(c_stream_managed)
	{
		c_queue.Clear();
		c_buffered_queue.Clear();
	}

	c_current_position = 0;

    return 1;

}



int __stdcall WMp3::Play()
{
// start playing mp3 stream
// this function starts decoding thread but
// waits to end of previous decoding thread


// prevent program crash if we call this function on empty stream	
	if(!c_start)
	{
		error("Play->Stream isn't open !");
		return 0;
	}

	if(c_pause)
		return Resume();

	if(c_play)
    	return FALSE;


	// previous playing thread must be terminated
	WaitForSingleObject(w_hThread, INFINITE);
	
    
	c_bLoop = 0; // dont play loop

	c_pause = 0; // reset pause indicator


// initialize VU meter	
	c_vudata_ready = FALSE;
	c_dwPos = 0;
	c_dwIndex = 0;

// main decoding thread switch
    c_play = 1; // enable decoding thread


	if(w_hThread)
		CloseHandle(w_hThread);

	w_hThread = 0;

// create playing thread
	c_thread_no_error = 1;
	w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFunc, (void*) this,0,&w_hThreadID);
	 if( w_hThread == NULL)
	{
    	error("Play->Can't create decoding thread !");
        return 0;
	}
	
	// wait for thread to create and get possible error 
	WaitForSingleObject(c_hThreadStartEvent, INFINITE);
	if(c_thread_no_error == 0)	// we have some error in thread
	{
		Stop();
		return 0;

	}

   
	
	
    return 1;
}





int WINAPI WMp3::_ThreadFunc(void* lpdwParam)
{
// main decoding thread
// when this thread ends wave output is closed, position is set to 0 

	WMp3* mp3 = (WMp3*) lpdwParam;

	mp3->c_enter_fade_out_sent = 0;
	mp3->c_exit_fade_out_sent = 0;

	mp3->c_enter_fade_in_sent = 0;
	mp3->c_exit_fade_in_sent = 0;

	
	mp3->c_enter_loop_sent = 0;
	mp3->c_exit_loop_sent = 0;

	mp3->c_nInterPosition = 0;
	
	int call_ret = 0;

	if(mp3->callback_messages & MP3_MSG_PLAY)
		call_ret = send_callback_message(mp3, MP3_MSG_PLAY, 0, 0);
		

	if(mp3->callback_messages & MP3_MSG_PLAY_ASYNC)
		send_callback_message(mp3, MP3_MSG_PLAY_ASYNC, 0, 0);
		
	if(call_ret == 1)
	{
			mp3->c_thread_no_error = 1;
			mp3->c_bFadeOut = 0;
			mp3->c_bFadeIn = 0;
			mp3->c_play = FALSE;
			mp3->c_pause = FALSE;
			mp3->c_vudata_ready = FALSE;
			mp3->c_dwPos = 0; // VU data
			mp3->c_dwIndex = 0; // VU data
			mp3->c_current_bitrate = 0;
			mp3->c_frame_counter = 0;
			mp3->c_bitrate_sum = 0;
			mp3->c_position = mp3->c_start;
			mp3->c_nPosition = 0;
			mp3->c_bLoop = 0;
			mp3->c_current_position = 0;
			mp3->c_in_loop = 0;
			
			send_stop(mp3);

			SetEvent(mp3->c_hThreadStartEvent);
    		return 1;
	}	


	// Play, Seek, Stop function can execute only if this thread is finished


	// initalize values

	mp3->c_current_bitrate = 0;	// bitrate of decoding frame
	mp3->c_frame_counter = 0;	// number of decoded frames
	mp3->c_bitrate_sum = 0;		// sum of all decoded bitrates


	mp3->c_bEQIn = FALSE;



	if(mp3->c_position + MAD_BUFFER_GUARD >= mp3->c_end)
	{

		if(mp3->c_fReverseMode)
		{
			mp3->c_position = mp3->c_end;	

		}
		else
		
		{
			//mp3->c_play = 0;
			//mp3->error("WMp3::_ThreadFunc->Position is out of stream 2!");
			mp3->c_thread_no_error = 1;
			mp3->c_bFadeOut = 0;
			mp3->c_bFadeIn = 0;
			mp3->c_play = FALSE;
			mp3->c_pause = FALSE;
			mp3->c_vudata_ready = FALSE;
			mp3->c_dwPos = 0; // VU data
			mp3->c_dwIndex = 0; // VU data
			mp3->c_current_bitrate = 0;
			mp3->c_frame_counter = 0;
			mp3->c_bitrate_sum = 0;
			mp3->c_position = mp3->c_start;
			mp3->c_nPosition = 0;
			mp3->c_bLoop = 0;
			mp3->c_current_position = 0;
			mp3->c_in_loop = 0;
			send_stop(mp3);
			SetEvent(mp3->c_hThreadStartEvent);
    		return 1;
		}
	}

	unsigned int wave_output_id;
	if(mp3->c_current_woc == WAVE_OUT_WAVE_MAPPER)
		wave_output_id = WAVE_MAPPER;
	else
		wave_output_id = mp3->c_current_woc;
		
	//wave_output_id = 1;		

	// open wave output

	if(mp3->c_no_audio_output == 0)
	{
		 if ( !mp3->w_wave->OpenAudioOutput(wave_output_id, (DWORD)  mp3->_WaveOutProc, (DWORD)  mp3, CALLBACK_FUNCTION))
		 {

			if(wave_output_id == WAVE_MAPPER)
				mp3->error("_ThreadFunc->Can't open wave output: WAVE_MAPPER");
			else
			{
				if(mp3->c_pwoc && ( wave_output_id < mp3->c_woc_num))
					sprintf(mp3->_lpErrorMessage, "_ThreadFunc->Can't open wave output: %s", mp3->c_pwoc[wave_output_id].szPname);  
				else
					mp3->error("_ThreadFunc->Can't open wave output");
			}
			mp3->c_thread_no_error = 0;
			mp3->c_in_loop = 0;

			send_stop(mp3);

			SetEvent(mp3->c_hThreadStartEvent);
    		return 0;
		}
	}

	

	mp3->c_thread_no_error = 1;

	SetEvent(mp3->c_hThreadStartEvent);


	
	unsigned int i;

	if(mp3->c_tmp)
		 LocalFree(mp3->c_tmp);

	mp3->c_tmp = 0;




	struct mad_stream *stream = &mp3->c_stream;
	struct mad_frame *frame = &mp3->c_frame; 
	struct mad_synth *synth = &mp3->c_synth; 




	int skip_byte = mp3->c_byte_to_skip;


	
	unsigned int frame_count = 0;
	unsigned int nSamplesNum = 0;
	int output_size = 0;
	unsigned int output_index = 0;
	unsigned int frame_count_sum = 0;

	char* output_buf = mp3->w_wave->GetBufferData(output_index);

	unsigned int start_index = 0;
	unsigned int stop_index = 0;
	unsigned int skip = 0;

	int ending_managed_stream = 0;



	AUTODITHER_STRUCT *left_dither = &mp3->left_dither;
	AUTODITHER_STRUCT *right_dither = &mp3->right_dither;
	ZeroMemory(left_dither, sizeof(AUTODITHER_STRUCT));
	ZeroMemory(right_dither, sizeof(AUTODITHER_STRUCT));

	register mad_fixed_t *left_ch;
	register mad_fixed_t *right_ch;
	register mad_fixed_t sample;
	register mad_fixed_t output;

	mad_fixed_t random;

	unsigned int nchannels, nsamples1;


	// unprepare wave buffers, all buffers must be free
	for( i = 0; i < mp3->w_wave->GetNumOfBuffers(); i++)
	{
   		mp3->w_wave->SetBufferDone(i);
		mp3->w_wave->UnprepareHeader(i);
	}
				


	// clear sfx buffers
	EnterCriticalSection(&mp3->c_wcsSfx);
				
	if(mp3->c_echo)
		mp3->ClearSfxBuffers();
	
	LeaveCriticalSection(&mp3->c_wcsSfx);
	
	if(mp3->c_bUseExternalEQ)
		equ_clearbuf(&mp3->eq_inter_param, mp3->c_sampling_rate);
	

	// set stream to starting position


	int stream_buffer_size = MAX_FRAME_SIZE * mp3->c_input_buffer_size + MAD_BUFFER_GUARD;;
	if(mp3->c_stream_managed)
	{
		unsigned int load_size;
		
		mp3->c_tmp_ds = (unsigned char*) LocalAlloc(LMEM_FIXED, stream_buffer_size); 
		if(!mp3->c_tmp_ds)
		{
			mp3->c_play = 0;
			send_stop(mp3);
			mp3->w_wave->Close();
			mp3->error("ThreadFunc->Memory allocation error !");
			mp3->c_thread_no_error = 0;
			SetEvent(mp3->c_hThreadStartEvent);
			return 0;
		}
		
		if(mp3->c_buffered_stream)
		{
			int nBufNum;
			load_size = mp3->c_buffered_queue.PullDataFifo(mp3->c_tmp_ds, stream_buffer_size, &nBufNum);
			if(nBufNum)
				send_buffer_done(mp3, mp3->c_buffered_queue.GetCount(), mp3->c_buffered_queue.GetSizeSum());
	


			if(load_size == 0)
			{
				
				ResetEvent(mp3->c_hStreamEvent);
				int callback_ret = send_stream_out_of_data(mp3);

				if(callback_ret == 1 || mp3->c_endstream)
				{
					free(mp3->c_tmp_ds);
					mp3->c_play = 0;
					mp3->c_in_loop = 0;	

					send_stop(mp3);
					mp3->w_wave->Close();
					mp3->c_thread_no_error = 1;
					SetEvent(mp3->c_hThreadStartEvent);
					return 0;
				}

				if(callback_ret != 2)	
					WaitForSingleObject(mp3->c_hStreamEvent, INFINITE);
				
			}
		}
		else
		{	
			int nBufNum;
			int ret  = mp3->c_queue.PullDataFifo(mp3->c_tmp_ds, stream_buffer_size, &load_size, &nBufNum);
			if(nBufNum)
				send_buffer_done(mp3, mp3->c_queue.GetNum(), mp3->c_queue.GetDataSize());


			if( ret == 0)
			{
				ResetEvent(mp3->c_hStreamEvent);
				int callback_ret = send_stream_out_of_data(mp3);
				if(callback_ret || mp3->c_endstream)
				{
					free(mp3->c_tmp_ds);
					mp3->c_play = 0;
					mp3->c_in_loop = 0;	

			
					send_stop(mp3);
					mp3->w_wave->Close();
					mp3->c_thread_no_error = 1;
					SetEvent(mp3->c_hThreadStartEvent);
					return 0;
				}

				if(callback_ret != 2)
					WaitForSingleObject(mp3->c_hStreamEvent, INFINITE);
				
			}
		}

		mad_stream_init(stream);
		mad_frame_init(frame);
		mad_synth_init(synth);
					
		mp3->c_start = mp3->c_tmp_ds;
		mp3->c_position = mp3->c_tmp_ds;
		mp3->c_end = mp3->c_tmp_ds + load_size - 1;
		mp3->c_size =  load_size;

		stream->freerate = mp3->c_nFreeBitrate;
		mad_stream_buffer(stream, mp3->c_tmp_ds, load_size);	
				 

	}
	else
	{
		mad_stream_init(stream);
		mad_frame_init(frame);
		mad_synth_init(synth);

		stream->freerate = mp3->c_nFreeBitrate;

		if(mp3->c_fReverseMode)
		{
			if(mp3->c_pFramePtr == 0 || mp3->c_nFrameNumber == 0)
			{
				mp3->error("_ThreadFunc->pFramePtr is 0 !");
				mp3->c_thread_no_error = 0;
				SetEvent(mp3->c_hThreadStartEvent);
				mp3->c_in_loop = 0;
				mp3->w_wave->Close();
				send_stop(mp3);
    			return 0;
			}


		//	stop_index = (unsigned int) (  (double) mp3->c_nPosition / (double) mp3->c_song_length_ms * (double) mp3->c_nFrameNumber);
			stop_index = MulDiv(mp3->c_nPosition, mp3->c_nFrameNumber, mp3->c_song_length_ms);


			if(stop_index >= mp3->c_nFrameNumber)
			{
				stop_index = mp3->c_nFrameNumber - 1;
			}

		

			if(stop_index < mp3->c_input_buffer_size)
				start_index = 0;
			else
				start_index = stop_index - mp3->c_input_buffer_size;
						
			// overlay
			int back = mp3->c_nFrameOverlay;
			int start = start_index;
			if(start < back)
			{
				start = 0;
				skip = start;
			}
			else
			{
				start -= back;
				skip = back;
			}	


			mad_stream_buffer(stream, mp3->c_start, mp3->c_pFramePtr[stop_index].ptr - mp3->c_start + MAD_BUFFER_GUARD);
			mad_stream_skip(stream,  mp3->c_pFramePtr[start].ptr - mp3->c_start);
			

	
			
		}
		else
		{

			mad_stream_buffer(stream, mp3->c_start, mp3->c_size);
			mad_stream_skip(stream,  mp3->c_position - mp3->c_start);
		}

	}



// main playing thread
	while(mp3->c_play)
	{
		if(mad_frame_decode(frame, stream))
		{ 
			// decoding error
			if(MAD_RECOVERABLE(stream->error)) // if recoverable error try to decode next frame
					continue;

			// stream error, we are at the end of stream, or there is some other problem with strean
			// stream can't continue
				
			if(mp3->c_stream_managed) // if we are using managed stream, wait for more data from user
			{
				
				unsigned int len = mp3->c_end + 1 - stream->this_frame; // calculate rest data in old buffer
				MoveMemory (mp3->c_tmp_ds, stream->this_frame, len); // copy unused data from previous buffer
				unsigned int load_size;
				// now fill buffer to the end
				if(mp3->c_buffered_stream) // if we using buffered stream use buffered queue
				{
					
					while(mp3->c_play) // fill buffer from queue
					{
						if(mp3->c_buffered_queue.GetCount() == 0) // if queue empty, wait or exit
						{
							
							if(mp3->c_play == 0 || mp3->c_endstream != 0)
							{
								ending_managed_stream = 1;
								break;
							}
							
							ResetEvent(mp3->c_hStreamEvent);
							int callback_ret = send_stream_out_of_data(mp3);
							
							if(callback_ret == 1)
							{
								mp3->c_endstream = 1;
								ending_managed_stream = 1;
								break;
							}

			
							if(callback_ret != 2)
								WaitForSingleObject(mp3->c_hStreamEvent, INFINITE);
							

						
							

							// try again
							continue;		
						}

						// fill buffer  to the end
						int nBufNum;
						load_size= mp3->c_buffered_queue.PullDataFifo(mp3->c_tmp_ds + len, stream_buffer_size - len, &nBufNum);
						if(nBufNum)
							send_buffer_done(mp3, mp3->c_buffered_queue.GetCount(), mp3->c_buffered_queue.GetSizeSum());

						
						if(load_size == 0) // if buffer is empty wait
						{
							if(mp3->c_play == 0 || mp3->c_endstream != 0)
							{
								ending_managed_stream = 1;
								break;
							}
							
							ResetEvent(mp3->c_hStreamEvent);

							int callback_ret = send_stream_out_of_data(mp3);
							if(callback_ret == 1)
							{
								mp3->c_endstream = 1;
								ending_managed_stream = 1;
								break;
							}

							if(callback_ret != 2)
								WaitForSingleObject(mp3->c_hStreamEvent, INFINITE);

							continue;
						}

						break;
					} // END: while(mp3->c_play)

		
				} // END: if(mp3->c_buffered_stream)
				else	// we using unbuffered stream
				{
					while(mp3->c_play) // fill buffer from queue
					{
						// wait if stream empty
						if(mp3->c_queue.GetNum() == 0)
						{
							if(mp3->c_play == 0 || mp3->c_endstream != 0)
							{
								ending_managed_stream = 1;
								break;
							}
							
							ResetEvent(mp3->c_hStreamEvent);

							int callback_ret = send_stream_out_of_data(mp3);
							if(callback_ret == 1)
							{
								mp3->c_endstream = 1;
								ending_managed_stream = 1;
								break;
							}

							if(callback_ret != 2)
								WaitForSingleObject(mp3->c_hStreamEvent, INFINITE);

							continue;
							
						}


						int nBufNum;
						int ret = mp3->c_queue.PullDataFifo(mp3->c_tmp_ds + len, stream_buffer_size - len, &load_size, &nBufNum);
						if(nBufNum)
							send_buffer_done(mp3, mp3->c_queue.GetNum(), mp3->c_queue.GetDataSize());

				
						
						if( ret == 0)
						{
							if(mp3->c_play == 0 || mp3->c_endstream != 0)
							{
								ending_managed_stream = 1;
								break;
							}
							
							ResetEvent(mp3->c_hStreamEvent);

							int callback_ret = send_stream_out_of_data(mp3);
							if(callback_ret == 1)
							{
								mp3->c_endstream = 1;
								ending_managed_stream = 1;
								break;
							}

							if(callback_ret != 2)
								WaitForSingleObject(mp3->c_hStreamEvent, INFINITE);

							continue;
						}

						break;
					} // END: while(mp3->c_play)

				} // END if(mp3->c_buffered_stream) else

					
					if(ending_managed_stream == 1 || mp3->c_play == 0)
						break;
					

					mp3->c_start = mp3->c_tmp_ds;
					mp3->c_position = mp3->c_tmp_ds;
					mp3->c_end = mp3->c_tmp_ds + len + load_size - 1;
					mp3->c_size = len + load_size;
					mad_stream_buffer(stream, mp3->c_tmp_ds, len + load_size);

					continue;

			} // END: if(mp3->c_stream_managed)

	

			if(mp3->c_tmp) // MAD_BUFFER_GUARD buffer played, end
			{
				LocalFree(mp3->c_tmp);
				mp3->c_tmp = 0;
					
				if(mp3->c_bLoop) // check if we playing loop
				{
					if(mp3->c_nLoop < mp3->c_nLoopCount)
					{ 
						mp3->c_in_loop = 1;
						mp3->c_nLoop++;
						mp3->c_nLoopFramePlayed = 0;
							

						mad_frame_finish(frame);
						mad_synth_finish(synth);
						mad_stream_finish(stream);

						mad_frame_init(frame);
						mad_synth_init(synth);
						mad_stream_init(stream);

						stream->freerate = mp3->c_nFreeBitrate;

						mad_stream_buffer(stream, mp3->c_start, mp3->c_size);
						mad_stream_skip(stream,  mp3->c_position - mp3->c_start);

						continue;
					}
					else if(mp3->flying_loop == 0) // not flying loop
					{
						mp3->c_in_loop = 0;
						send_loop_exit(mp3);
						break;
					}
					else // flying loop
					{
						mp3->c_in_loop = 0;
						send_loop_exit(mp3);
						
					}
				}
				else // if(mp3->c_bLoop)
				{
						break; // stop decoding, last frame decoded
				}
			}
			else // if(mp3->c_tmp)
			{ 
				if(mp3->c_fReverseMode) // end of buffer in reverse mode
				{
					// load prevoius buffer

					if(start_index == 0 || stop_index == 0 || mp3->c_play == 0) // end of playing
						break;

					if(mp3->c_pFramePtr == 0)
						break;
				
					stop_index = start_index;

					if(stop_index < mp3->c_input_buffer_size)
						start_index = 0;
					else
						start_index = stop_index - mp3->c_input_buffer_size;


					// overlay
					int back = mp3->c_nFrameOverlay;
					int start = start_index;
					if(start < back)
					{
						start = 0;
						skip = start;
					}
					else
					{
						start -= back;
						skip = back;
					}	


					mad_frame_finish(frame);
					mad_synth_finish(synth);
					mad_stream_finish(stream);

					mad_frame_init(frame);
					mad_synth_init(synth);
					mad_stream_init(stream);
						
					stream->freerate = mp3->c_nFreeBitrate;	

					mad_stream_buffer(stream, mp3->c_start, mp3->c_pFramePtr[stop_index].ptr - mp3->c_start  + MAD_BUFFER_GUARD);
					mad_stream_skip(stream,  mp3->c_pFramePtr[start].ptr - mp3->c_start);
						
					continue; // go to decoding of frame

				}

					

				// fix MAD_BUFFER_GUARD problem
				unsigned int len = mp3->c_end + 1 - stream->this_frame;	
				mp3->c_tmp = (unsigned char*) LocalAlloc(0, len + MAD_BUFFER_GUARD + 1);	
					
				if(!mp3->c_tmp)
						break;

				// copy last frame into buffer and add MAD_BUFFER_GUARD zero bytes to end
				CopyMemory (mp3->c_tmp, stream->this_frame, len);
				FillMemory (mp3->c_tmp + len, MAD_BUFFER_GUARD, 0);
				mad_stream_buffer(stream, mp3->c_tmp, len + MAD_BUFFER_GUARD);
				continue;
			}
			
			// unresolved error

			break;

		} // END: if(mad_frame_decode(frame, stream))

		if(mp3->c_fReverseMode)
		{
			if(skip != 0) // frame overlay
			{
				mad_synth_frame(synth, frame);
				skip--;
				continue;
			}
		}



		if(mp3->c_tmp)
			mp3->c_current_position = mp3->c_size + 1 - ( stream->this_frame - stream->buffer);
		else
			mp3->c_current_position =  stream->this_frame - mp3->c_start;
		


			// loop processing
		if(mp3->c_bLoop && mp3->c_in_loop == 1)
		{ 

			int callback_ret = send_loop_enter(mp3);
			if(callback_ret == 1)
			{
				mp3->c_in_loop = 0;
				mp3->c_nLoop = mp3->c_nLoopCount;
				mp3->c_nLoopFramePlayed = mp3->c_nLoopFrameNum + 1;
				mp3->flying_loop = 1;

			}



			if(mp3->c_nLoop < mp3->c_nLoopCount)
			{ // repeat loop ?
				if(mp3->c_nLoopFramePlayed >= mp3->c_nLoopFrameNum)
				{ 
					mp3->c_nLoop++;
					
					mp3->c_nLoopFramePlayed = 0;
					

					mad_frame_finish(frame);
					mad_synth_finish(synth);
					mad_stream_finish(stream);

					mad_frame_init(frame);
					mad_synth_init(synth);
					mad_stream_init(stream);

					stream->freerate = mp3->c_nFreeBitrate;

					mad_stream_buffer(stream, mp3->c_start, mp3->c_size);
					mad_stream_skip(stream,  mp3->c_position - mp3->c_start);
					
					//mp3->c_in_loop = 0;
					
					
				}	
				else
				{
					mp3->c_in_loop = 1;
				}	
			}
			else if(mp3->flying_loop == 0)
			{
				mp3->c_in_loop = 0;
				send_loop_exit(mp3);	
				break;
			}
			else
			{
				mp3->c_in_loop = 0;
				send_loop_exit(mp3);
					
			}
				
				
			mp3->c_nLoopFramePlayed++;	
		}
	// end of loop processing


		// mix channel filter	
		if(mp3->c_mix_channels)
			mix_filter(frame, mp3->c_l_mix, mp3->c_r_mix);
		
		
		// internal subband equalizer

		if(mp3->c_bUseInternalEQ) {

			int	Channel;
			int	Sample;
			int	Samples;
			int	SubBand;


	
			Samples = MAD_NSBSAMPLES(&frame->header);
	

			// strereo

			if(frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
			{
				for(Channel = 0; Channel < 2; Channel++)
					for(Sample = 0; Sample < Samples; Sample++)
						for(SubBand = 0;SubBand < 32; SubBand++) 
							frame->sbsample[Channel][Sample][SubBand]=
								mad_f_mul(frame->sbsample[Channel][Sample][SubBand],
									mp3->c_EqFilter[SubBand]);
	
			}
			else {
								  
			// mono

				for(Sample = 0;Sample < Samples; Sample++)
					for(SubBand = 0;SubBand < 32; SubBand++)
						frame->sbsample[0][Sample][SubBand]=
								mad_f_mul(frame->sbsample[0][Sample][SubBand],
								mp3->c_EqFilter[SubBand]);
			}
		}

		// end of internal subband equalizer


		// fadeout processing
		EnterCriticalSection(&mp3->c_wcs_fade);
		if(mp3->c_bFadeOut)
		{	
		
			if(mp3->c_enter_fade_in_sent == 1 && mp3->c_exit_fade_in_sent == 0)
			{
				send_fadein_exit(mp3);
				mp3->c_exit_fade_in_sent = 0;
				mp3->c_enter_fade_in_sent = 0;
				mp3->c_bFadeIn = 0;

			
			}

			if(mp3->c_fade_frame_count == 0 && mp3->c_enter_fade_out_sent == 1 && mp3->c_exit_fade_out_sent == 0)
			{
				send_fadeout_exit(mp3);
				mp3->c_exit_fade_out_sent = 0;
				mp3->c_enter_fade_out_sent = 0;
			
			}
			  


			int callback_ret = send_fadeout_enter(mp3);
			if(callback_ret == 1)
			{
				send_fadeout_exit(mp3);
				mp3->c_bFadeOut = FALSE;
				mp3->c_exit_fade_out_sent = 0;
				mp3->c_enter_fade_out_sent = 0;

			}

		

			if(callback_ret == 0 && mp3->c_fade_frame_count < mp3->c_nFadeOutFrameNum)
			{ 
				
	
				mp3->_fade_volume -= mp3->c_fadeOutStep;

				if(mp3->_fade_volume <= 0)
				{
					mp3->_fade_volume = 0;
					mp3->c_dLeftGainFade = 0;
					mp3->c_dRightGainFade = 0;
					mp3->c_dLeftGain = mp3->c_dLeftGainVolume * mp3->c_dLeftGainFade;
					mp3->c_dRightGain = mp3->c_dRightGainVolume * mp3->c_dRightGainFade;

					mp3->c_bFadeOut = FALSE;
					mp3->c_fade_frame_count = mp3->c_nFadeOutFrameNum;

	
					send_fadeout_exit(mp3);
					mp3->c_exit_fade_out_sent = 0;
					mp3->c_enter_fade_out_sent = 0;


				}
				else
				{
					mp3->c_dLeftGainFade = mp3->_fade_volume < 100 ? (pow(10, (double) mp3->_fade_volume / 100.0) / 10.0 - 0.1) : 1.0;
					mp3->c_dRightGainFade = mp3->c_dLeftGainFade;
					mp3->c_dLeftGain = mp3->c_dLeftGainVolume * mp3->c_dLeftGainFade;
					mp3->c_dRightGain = mp3->c_dRightGainVolume * mp3->c_dRightGainFade;
					mp3->c_fade_frame_count++;
				}
			}
			else if(callback_ret == 0)
			{
				mp3->_fade_volume = 0;

				mp3->c_dLeftGainFade = 0;
				mp3->c_dRightGainFade = 0;

				mp3->c_dLeftGain = mp3->c_dLeftGainVolume * mp3->c_dLeftGainFade;
				mp3->c_dRightGain = mp3->c_dRightGainVolume * mp3->c_dRightGainFade;

				mp3->c_bFadeOut = FALSE;

				send_fadeout_exit(mp3);
				mp3->c_exit_fade_out_sent = 0;
				mp3->c_enter_fade_out_sent = 0;
			}

					
		}

		LeaveCriticalSection(&mp3->c_wcs_fade);

		EnterCriticalSection(&mp3->c_wcs_fade);


		// fadein processing
		if(mp3->c_bFadeIn)
		{

			
			if(mp3->c_enter_fade_out_sent == 1 && mp3->c_exit_fade_out_sent == 0)
			{
				send_fadeout_exit(mp3);
				mp3->c_exit_fade_out_sent = 0;
				mp3->c_enter_fade_out_sent = 0;
				mp3->c_bFadeOut = 0;

			
			}

			if(mp3->c_fade_frame_count == 0 && mp3->c_enter_fade_in_sent == 1 && mp3->c_exit_fade_in_sent == 0)
			{
				send_fadein_exit(mp3);
				mp3->c_exit_fade_in_sent = 0;
				mp3->c_enter_fade_in_sent = 0;
			
			}

			int callback_ret = send_fadein_enter(mp3);
			if(callback_ret == 1)
			{
				send_fadein_exit(mp3);
				mp3->c_bFadeIn = FALSE;
				mp3->c_exit_fade_in_sent = 0;
				mp3->c_enter_fade_in_sent = 0;
			}



			if(callback_ret == 0 &&  mp3->c_fade_frame_count < mp3->c_nFadeInFrameNum) 
			{ 

				mp3->_fade_volume += mp3->c_fadeInStep;
				if(mp3->_fade_volume >= 100.0)
				{
					mp3->_fade_volume = 100.0;


					mp3->c_dLeftGainFade = 1.0;
					mp3->c_dRightGainFade = 1.0;
					mp3->c_dLeftGain = mp3->c_dLeftGainVolume * mp3->c_dLeftGainFade;
					mp3->c_dRightGain = mp3->c_dRightGainVolume * mp3->c_dRightGainFade;


					mp3->c_bFadeIn = 0;	
					mp3->c_fade_frame_count = mp3->c_nFadeInFrameNum;

					send_fadein_exit(mp3);
					mp3->c_exit_fade_in_sent = 0;
					mp3->c_enter_fade_in_sent = 0;
				}
				else
				{
					if(mp3->_fade_volume < 0)
					{
						mp3->_fade_volume = 0;
					}
							
					mp3->c_dLeftGainFade = mp3->_fade_volume < 100 ? (pow(10, (double) mp3->_fade_volume / 100.0) / 10.0 - 0.1) : 1.0;
					mp3->c_dRightGainFade = mp3->c_dLeftGainFade;

					mp3->c_dLeftGain = mp3->c_dLeftGainVolume * mp3->c_dLeftGainFade;
					mp3->c_dRightGain = mp3->c_dRightGainVolume * mp3->c_dRightGainFade;


					mp3->c_fade_frame_count++;
				}
			}
			else if(callback_ret == 0)
			{
				mp3->_fade_volume = 100.0;
				
				
				mp3->c_dLeftGainFade = 1.0;
				mp3->c_dRightGainFade = 1.0;
				mp3->c_dLeftGain = mp3->c_dLeftGainVolume * mp3->c_dLeftGainFade;
				mp3->c_dRightGain = mp3->c_dRightGainVolume * mp3->c_dRightGainFade;

				mp3->c_bFadeIn = 0;	

				send_fadein_exit(mp3);
				mp3->c_exit_fade_in_sent = 0;
				mp3->c_enter_fade_in_sent = 0;

			}

			
		}

		LeaveCriticalSection(&mp3->c_wcs_fade);


		mp3->c_current_bitrate = frame->header.bitrate;
		mp3->c_frame_counter++;
		mp3->c_bitrate_sum += mp3->c_current_bitrate / 1000;


		// synth
		mad_synth_frame(synth, frame);

		if(!mp3->c_play)
			break;



		// dither, resample ...
		nchannels = synth->pcm.channels;
		nsamples1  = synth->pcm.length;
		left_ch   = synth->pcm.samples[0];
		right_ch  = synth->pcm.samples[1];


 

		

		if(nchannels == 2)
		{ // stereo

			while (nsamples1--)
			{
			// left channel

				sample = *left_ch++;

				// noise shape 
				
				sample += left_dither->error[0] - left_dither->error[1] + left_dither->error[2];

				left_dither->error[2] = left_dither->error[1];
				left_dither->error[1] = left_dither->error[0] / 2;

				// bias 
				output = sample + 4096;
				
				// dither 
				random  = ((unsigned long) left_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;

				output += (random & 8191) - (left_dither->random & 8191);

				// clip
				if (output >= MAD_F_ONE)
					output = MAD_F_ONE - 1;
				else if (output < -MAD_F_ONE)
					output = -MAD_F_ONE;

				// quantize 
				output &= ~8191;

				// error feedback 
				left_dither->error[0] = sample - output;

				
				output_buf[0] = (char) (output >> 13);
				output_buf[1] = (char) (output >> 21);

				// right channel
				sample = *right_ch++;
				
	
				// noise shape 
				sample += right_dither->error[0] - right_dither->error[1] + right_dither->error[2];

				right_dither->error[2] = right_dither->error[1];
				right_dither->error[1] = right_dither->error[0] / 2;

				 // bias 
				output = sample + 4096;
				

				// dither 
				random  = ((unsigned long) right_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;

				output += (random & 8191) - (right_dither->random & 8191);

				if (output >= MAD_F_ONE)
					output = MAD_F_ONE - 1;
				else if (output < -MAD_F_ONE)
					output = -MAD_F_ONE;

				// quantize 
				output &= ~8191;

				// error feedback 
				right_dither->error[0] = sample - output;

				output_buf[2] = (char) (output >> 13);
				output_buf[3] = (char) (output >> 21);

				output_buf += 4;
				
				
			} // while (nsamples--)

			output_size += synth->pcm.length * 4;

			nSamplesNum += (synth->pcm.length * 2);
	
		}
		
		else
		{ // mono
			while (nsamples1--)
			{
		
			// left channel

				sample = *left_ch++;

				// noise shape 
				
				sample += left_dither->error[0] - left_dither->error[1] + left_dither->error[2];

				left_dither->error[2] = left_dither->error[1];
				left_dither->error[1] = left_dither->error[0] / 2;

				// bias 
				output = sample + 4096;
				

				// dither 
				random  = ((unsigned long) left_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;

				output += (random & 8191) - (left_dither->random & 8191);

				// clip
				if (output >= MAD_F_ONE)
					output = MAD_F_ONE - 1;
				else if (output < -MAD_F_ONE)
					output = -MAD_F_ONE;

				
				
			
				// quantize 
				output &= ~8191;

				// error feedback 
				left_dither->error[0] = sample - output;

				output_buf[0] = (char) (output >> 13);
				output_buf[1] = (char) (output >> 21);
				output_buf += 2;
		
			}

			output_size += synth->pcm.length * 2;

			nSamplesNum += synth->pcm.length;

		}



		frame_count++;
		frame_count_sum++;
	
		if(frame_count == mp3->c_input_buffer_size)
		{
		// output buffer is full			
			if(output_size)
			{
				if(mp3->c_fReverseMode)
				{
					reverse_buffer((unsigned char*) mp3->w_wave->GetBufferData(output_index), output_size, 16, mp3->c_channel);
				}

				if(mp3->c_dLeftGain != 1.0 || mp3->c_dRightGain != 1.0)
					internal_volume(mp3, (unsigned char*) mp3->w_wave->GetBufferData(output_index), output_size,16, mp3->c_channel);

				if(mp3->c_bVocalCut)
					s_vocal_cut((unsigned char*) mp3->w_wave->GetBufferData(output_index), output_size, 16, mp3->c_channel);

				// echo processing
				if(mp3->c_echo)
				{
					if(!mp3->c_play)
						break;

					// protect SetSfxParam(...) function from deallocating working buffers

					EnterCriticalSection(&mp3->c_wcsSfx);
					
					int size = output_size;
					int buf_size = size / 2;
					char* buf = (char *) mp3->w_wave->GetBufferData(output_index);
					
					
					// split each ( 100 ms ) wave buffer int 2 ( 50 ms ) echo buffers
					DoEcho(mp3, (short*) buf, buf_size);
					DoEcho(mp3, (short*) ( buf + buf_size), buf_size);
					
				
					LeaveCriticalSection(&mp3->c_wcsSfx);						
				}

				// end of echo processing


			// external equalizer processing
				
				if(mp3->c_bUseExternalEQ)
				{
					if(!mp3->c_play)
						break;

					mp3->c_bEQIn = TRUE;

					int samples_num;
					if(mp3->c_channel == 2) 
						samples_num = output_size / 4;
					else
						samples_num = output_size / 2;
				

					
					if(skip_byte)
					{ // need to skip bytes for gapless playing
						if(skip_byte > output_size)
						{ // not in these buffer
						
							
							equ_modifySamples(&mp3->eq_inter_param, (char *)mp3->w_wave->GetBufferData(output_index),
									samples_num, mp3->c_channel, 16);
								
							skip_byte -= output_size;
							// get next buffer
							output_buf = mp3->w_wave->GetBufferData(output_index);
							output_size = 0;
							frame_count = 0;
								
							continue;

						}
						else
						{ // skip bytes
						
							equ_modifySamples(&mp3->eq_inter_param, (char *)mp3->w_wave->GetBufferData(output_index),
								samples_num, mp3->c_channel, 16);

							char *data = mp3->w_wave->GetBufferData(output_index);
							output_size -= skip_byte;
							MoveMemory(data, data + skip_byte, output_size);
							skip_byte = 0; // no more skip

							// play this bufer
						}
					}
					else
					{ // not skiping, play normal
					
			
						equ_modifySamples(&mp3->eq_inter_param, (char *)mp3->w_wave->GetBufferData(output_index),
								samples_num, mp3->c_channel, 16);

					}
				}
				else
				{ // disable equalizer, flush equalizer buffers
			
					if(mp3->c_bEQIn)
					{
						mp3->c_bEQIn = FALSE;

						int samples_num;
						if(mp3->c_channel == 2) 
							samples_num = output_size / 4;
						else
							samples_num = output_size / 2;
					
						// we need tu flush equ buffer
					
					
						
						CopyMemory (mp3->c_gapless_buffer,
								mp3->w_wave->GetBufferData(output_index),
								output_size);
						
				
						int o_size = output_size;
						skip_byte = mp3->c_byte_to_skip;
					
						//
						while(skip_byte)
						{	
							equ_modifySamples(&mp3->eq_inter_param, mp3->w_wave->GetBufferData(output_index),
								samples_num, mp3->c_channel, 16);

							
							if(skip_byte > o_size)
							{
								skip_byte -= o_size;
							
							}
							else
							{
								o_size = skip_byte;
								skip_byte = 0;
							}

							int ret = 0;
							if(mp3->callback_messages & MP3_MSG_WAVE_BUFFER)
							{
								ret = send_callback_message(mp3, MP3_MSG_WAVE_BUFFER,
										(unsigned int) mp3->w_wave->GetBufferData(output_index),
										o_size);

								if(mp3->c_play == 0 || ret == 2)
								{
									mp3->c_play = 0;
									break;
								}


							}


							if(ret == 0)
							{

								mp3->w_wave->SetBufferSize(output_index, o_size);
								mp3->w_wave->PlayBuffer(output_index);
						

							
								// get new buffer
								output_index++;
								if(output_index == mp3->w_wave->GetNumOfBuffers())
									output_index = 0;

							
								output_buf = mp3->w_wave->GetBufferData(output_index);
						


								while(mp3->w_wave->IsBufferDone(output_index) == 0)
								{
									if(!mp3->c_play)
										break;
									WaitForSingleObject(mp3->w_hEvent, INFINITE);
									if(!mp3->c_play)
										break;
				
								}

								mp3->w_wave->UnprepareHeader(output_index);

							}
							else
							{
									// get new buffer
								output_index++;
								if(output_index == mp3->w_wave->GetNumOfBuffers())
									output_index = 0;

							
								output_buf = mp3->w_wave->GetBufferData(output_index);
						
							}
						}

						CopyMemory(output_buf, mp3->c_gapless_buffer, output_size);
						skip_byte = mp3->c_byte_to_skip;	
					}
					
				}
			

				// end of external equalizer


	
				if(!mp3->c_play)
					break;

				int ret = 0;
				if(mp3->callback_messages & MP3_MSG_WAVE_BUFFER)
				{
					int samples;
					if(mp3->c_channel)
						samples = output_size / 4;
					else
						samples = output_size / 2;
							
					mp3->c_nInterPosition += (MulDiv(samples, 1000, mp3->c_sampling_rate));

					// calculate position

					ret = send_callback_message(mp3, MP3_MSG_WAVE_BUFFER,
										(unsigned int) mp3->w_wave->GetBufferData(output_index),
										output_size);

			

					if(mp3->c_play == 0 || ret == 2)
					{
						mp3->c_play = 0;
						break;
					}

				}

				if(ret == 0)
				{

					mp3->w_wave->SetBufferSize(output_index, output_size);
					mp3->w_wave->PlayBuffer(output_index);
					
					output_index++;
					if(output_index == mp3->w_wave->GetNumOfBuffers())
						output_index = 0;

					
					output_buf = mp3->w_wave->GetBufferData(output_index);
					output_size = 0;
					frame_count = 0;
					nSamplesNum = 0;


					while(mp3->w_wave->IsBufferDone(output_index) == 0)
					{
						if(!mp3->c_play)
							break;

						WaitForSingleObject(mp3->w_hEvent, INFINITE);
						if(!mp3->c_play)
							break;
					
					}
				
					mp3->w_wave->UnprepareHeader(output_index);
				}
				else
				{
					output_index++;
					if(output_index == mp3->w_wave->GetNumOfBuffers())
						output_index = 0;

					output_buf = mp3->w_wave->GetBufferData(output_index);
					output_size = 0;
					frame_count = 0;
					nSamplesNum = 0;
					mp3->w_wave->UnprepareHeader(output_index);
				}
			
			
			}
			else // if(output_size)
			{
				output_buf = mp3->w_wave->GetBufferData(output_index);
				frame_count = 0;
				output_size = 0;
				nSamplesNum = 0;
				
			}
				
		} // if(frame_count == mp3->c_input_buffer_size)


	} // while(mp3->c_play) 


// flush eq buffer to soundcard
	if(mp3->c_bEQIn && mp3->c_play)
	{
		
		// size of one wave buffer
		int out_size = mp3->c_input_buffer_size * mp3->c_sample_per_frame * 2 * mp3->c_channel;

		// sum of all played samples
		unsigned int sum_samples = frame_count_sum * mp3->c_sample_per_frame;
			
		// number of samples in one buffer
		int samples_num = mp3->c_input_buffer_size * mp3->c_sample_per_frame;
	
		int have_byte = sum_samples * 2 * mp3->c_channel;

	
		skip_byte = mp3->c_byte_to_skip;

		if(have_byte < skip_byte)
			skip_byte = have_byte;


		while(mp3->w_wave->IsBufferDone(output_index) == 0)
		{
			if(!mp3->c_play)
					break;		
			WaitForSingleObject(mp3->w_hEvent, INFINITE);

			if(!mp3->c_play)
					break;
			
		}


		while(skip_byte)
		{ // flush eq bufers
			if(skip_byte > out_size)
			{
				skip_byte -= out_size;
							
			}
			else
			{
				out_size = skip_byte;
				skip_byte = 0;
			}


			equ_modifySamples(&mp3->eq_inter_param, mp3->w_wave->GetBufferData(output_index),
				samples_num, mp3->c_channel, 16);

			int ret = 0;
			if(mp3->callback_messages & MP3_MSG_WAVE_BUFFER)
			{

				int samples;
				if(mp3->c_channel)
					samples = output_size / 4;
				else
					samples = output_size / 2;

				mp3->c_nInterPosition += (MulDiv(samples, 1000, mp3->c_sampling_rate));

				ret = send_callback_message(mp3, MP3_MSG_WAVE_BUFFER,
										(unsigned int) mp3->w_wave->GetBufferData(output_index),
										output_size);

				if(mp3->c_play == 0 || ret == 2)
				{
					mp3->c_play = 0;
					break;
				}

			

			}

			if(ret == 0)
			{

				mp3->w_wave->SetBufferSize(output_index, out_size);
				mp3->w_wave->PlayBuffer(output_index);
							
				// get new buffer
				output_index++;
				if(output_index == mp3->w_wave->GetNumOfBuffers())
					output_index = 0;


				while(!mp3->w_wave->IsBufferDone(output_index))
				{
					if(!mp3->c_play)
						break;
					WaitForSingleObject(mp3->w_hEvent, INFINITE);
					if(!mp3->c_play)
						break;
					
				}

				mp3->w_wave->UnprepareHeader(output_index);

			}
		}

	}
					


	if(mp3->c_tmp)
		 LocalFree(mp3->c_tmp);

	mp3->c_tmp = 0;

	if(mp3->c_tmp_ds)
		LocalFree(mp3->c_tmp_ds);
		
	mp3->c_tmp_ds = 0;	

	
	

// wait for all wave buffers to end

	if(mp3->c_play == 0)
	{
		mp3->w_wave->Stop();
		for(int j = 0; j < (int) mp3->w_wave->GetNumOfBuffers(); j++ )
   			mp3->w_wave->UnprepareHeader(j);
		
	}
	else
	{

		for(int j = 0; j < (int) mp3->w_wave->GetNumOfBuffers(); j++ )
		{
			if(mp3->w_wave->IsBufferDone(j))
			{
   				mp3->w_wave->UnprepareHeader(j);
			}
			else
			{
				if(!mp3->c_play)
						break;
				j--;
				WaitForSingleObject(mp3->w_hEvent, INFINITE);
			}
		}
	}
	

   mp3->c_bFadeOut = 0;
   mp3->c_bFadeIn = 0;

	
	mp3->c_play = 0;
    mp3->c_pause = 0;


// initialize VU data
	mp3->c_vudata_ready = FALSE;
	mp3->c_dwPos = 0; // VU data
	mp3->c_dwIndex = 0; // VU data
	
	

	mad_stream_finish(stream);
	mad_frame_finish(frame);
	mad_synth_finish(synth);


	mp3->c_current_bitrate = 0;
	mp3->c_frame_counter = 0;
	mp3->c_bitrate_sum = 0;

// seek stream to begining of mp3 data ( to first frame )

	mp3->c_position = mp3->c_start;
	mp3->c_current_position = 0;

// set seek position to 0

	mp3->c_nPosition = 0;

// stop loop playing, need this to get correct position
// without loop position corrector

	mp3->c_bLoop = 0;


	// stop playing, reset wave position indicator to 0	
	mp3->w_wave->Stop();

// close wave output
   mp3->w_wave->Close();

   mp3->c_in_loop = 0;



	if(mp3->c_stream_managed)
	{
		if(mp3->c_buffered_stream)
		{
			if(mp3->c_buffered_queue.GetCount())
			{
				mp3->c_buffered_queue.Clear();
				send_buffer_done(mp3, 0, 0);
			}

		}
		else
		{
			if(mp3->c_queue.GetNum())
			{
				mp3->c_queue.Clear();
				send_buffer_done(mp3, 0, 0);
			}


		}
	}



	if(mp3->c_enter_loop_sent == 1 && mp3->c_exit_loop_sent == 0)
		send_loop_exit(mp3);
		

	if(mp3->c_enter_fade_out_sent == 1 && mp3->c_exit_fade_out_sent == 0)
		send_fadeout_exit(mp3);

	if(mp3->c_enter_fade_in_sent == 1 && mp3->c_exit_fade_in_sent == 0)
		send_fadein_exit(mp3);
		

	send_stop(mp3);




	return 1;	
}






int __stdcall WMp3::LoadID3(int nId3Version, ID3_INFO *pId3Info)
{

	if(!c_start)
	{
		error("LoadID3->Stream isn't open !");
		return 0;
	}

	if( _LoadID3(this, nId3Version))
	{
		 pId3Info->Artist = w_myID3.artist;
		 pId3Info->Title = w_myID3.title;
		 pId3Info->TrackNum = w_myID3.tracknum;
		 pId3Info->Album = w_myID3.album;
		 pId3Info->Year = w_myID3.year;
		 pId3Info->Comment = w_myID3.comment;
		 pId3Info->Genre = w_myID3.genre;
		 /*
		 if(nId3Version == ID3_VERSION2)
			alert(pId3Info->Genre);
			*/
		 return 1;

	}

	memset(pId3Info, 0, sizeof(ID3_INFO));

	return 0;
	
}


BOOL WMp3::_LoadID3(WMp3* mp3,int id3Version)
{
// load ID3 data into id3 structure

	switch(id3Version) {

    	case ID3_VERSION1:
        {
            if(mp3->w_myID3.album) {
            	free(mp3->w_myID3.album);
                mp3->w_myID3.album = 0;
            }

        	mp3->w_myID3.album = (char*) malloc(31);
			*mp3->w_myID3.album = 0;

             if(mp3->w_myID3.artist) {
             	free(mp3->w_myID3.artist);
                mp3->w_myID3.artist = 0;
             }

			mp3->w_myID3.artist = (char*) malloc(31);
			*mp3->w_myID3.artist = 0;

            if(mp3->w_myID3.comment) {
            	free(mp3->w_myID3.comment);
                mp3->w_myID3.comment = 0;
            }

			mp3->w_myID3.comment = (char*) malloc(31);
			*mp3->w_myID3.comment = 0;

             if(mp3->w_myID3.title) {
             	free(mp3->w_myID3.title);
                mp3->w_myID3.title = 0;
             }

			mp3->w_myID3.title = (char*) malloc(31);
			*mp3->w_myID3.title = 0;

            if(mp3->w_myID3.year) {
            	free(mp3->w_myID3.year);
                mp3->w_myID3.year = 0;
            }

			mp3->w_myID3.year = (char*) malloc(5);
			*mp3->w_myID3.year = 0;

            if(mp3->w_myID3.tracknum) {
            	free(mp3->w_myID3.tracknum);
                mp3->w_myID3.tracknum = 0;
            }

            mp3->w_myID3.tracknum = (char*) malloc(4);
			*mp3->w_myID3.tracknum = 0;

			if(mp3->w_myID3.genre) {
            	free(mp3->w_myID3.genre);
                mp3->w_myID3.genre = 0;
            }

            mp3->w_myID3.genre = (char*) malloc(1);
			*mp3->w_myID3.genre = 0;

	
            if(!mp3->c_ID3v1) {  	
                mp3->error("WMp3::LoadID3->There is no ID3v1 tag"); 
				return FALSE;
            }
 
			
			memcpy(mp3->w_myID3.title,mp3->c_stream_end - 124 , 30);
			mp3->w_myID3.title[30] = 0;
			RemoveEndingSpaces(mp3->w_myID3.title);

			memcpy(mp3->w_myID3.artist,mp3->c_stream_end - 94, 30);
			mp3->w_myID3.artist[30] = 0;
			RemoveEndingSpaces(mp3->w_myID3.artist);

			memcpy(mp3->w_myID3.album,mp3->c_stream_end - 64, 30);
			mp3->w_myID3.album[30] = 0;
			RemoveEndingSpaces(mp3->w_myID3.album);

			memcpy(mp3->w_myID3.year,mp3->c_stream_end - 34, 4);
			mp3->w_myID3.year[4] = 0;
			RemoveEndingSpaces(mp3->w_myID3.year);

			memcpy(mp3->w_myID3.comment,mp3->c_stream_end - 30, 30);
			mp3->w_myID3.comment[30] = 0;

			memcpy(mp3->w_myID3.genre, mp3->c_stream_end,1);

            if( (unsigned char) mp3->w_myID3.genre[0] > 146) {
            	*mp3->w_myID3.genre = 0;
			}
            else {
				unsigned char tmp = mp3->w_myID3.genre[0]; 
				free(mp3->w_myID3.genre);
				if(tmp > 147) tmp = 0;
				mp3->w_myID3.genre = (char*) malloc(strlen(sGenres[(BYTE) tmp]) + 1);
				strcpy(mp3->w_myID3.genre,sGenres[(BYTE) tmp]); 
			}

 

            BYTE tracknum = 0;
            if( mp3->w_myID3.comment[28] == 0 )
                tracknum = mp3->w_myID3.comment[29];
          	sprintf( mp3->w_myID3.tracknum, "%u", tracknum);
	
	
			
        }
        return TRUE;


        case ID3_VERSION2:
        {
			if(!DecodeID3v2Header(mp3))
				return 0;
		
							
        }
        return TRUE;


    }

    return FALSE;
	
}








unsigned int RemoveLeadingNULL( unsigned char *str , unsigned int size)
{
// remove leading NULL from array - shift not NULL substr to left
// INPUT: 	char *str		- input stream
//          DWORD size		- size of input stream
// RETURN: new size of input stream;
	if(!str)
    	return 0;

	unsigned int pos = 0;
	unsigned int len = size;
	for( unsigned int i = 0; i < size; i++ )
	{
    	if( str[i] > 0 )
		{
			// found first non0
			
			memmove(str, str + i, size - i);
			return size - i;

				
        }
    }

    return size;
}







int __stdcall WMp3::GetMp3Info(MP3_STREAM_INFO *pInfo)
{
	//return c_mpeg_version_str;

	if(pInfo == 0)
	{
		error("GetMp3Info->Invalid parameter !\nMP3_STREAM_INFO *pInfo can't be NULL !");
		return 0;
	}

	pInfo->nMPEGVersion = c_mpeg_version;
	pInfo->nLayerVersion = c_mpeg_layer_version;
	pInfo->nChannelMode = c_channel_mode;
	pInfo->nEmphasis = c_emphasis_mode;


	pInfo->nSamplingRate = c_sampling_rate;
	pInfo->nHeaderStart = c_start - c_stream_start;
	pInfo->nChannelNumber = c_channel;
	pInfo->fVBR = c_vbr;
	if(c_vbr)
		pInfo->nBitrate = (int) ( c_avg_bitrate / 1000) ;
	else
		pInfo->nBitrate = c_bitrate / 1000; 

	return 1;
}






unsigned long Get32bits(unsigned char *buf) {

	unsigned long ret;
    ret = (((unsigned long) buf[0]) << 24) |
	(((unsigned long) buf[1]) << 16) |
	(((unsigned long) buf[2]) << 8) |
	((unsigned long) buf[3]) ;

    return ret;
}



int __stdcall WMp3::Pause()
{
	if(!c_start)
	{
		error("Pause->Stream isn't open !");
		return 0;
	}

	w_wave->Pause();
    c_pause = TRUE;
    return 1;
}

int __stdcall WMp3::Resume()
{
	if(!c_start)
	{
		error("Resume->Stream isn't open !");
		return 0;
	}

	w_wave->Resume();
    c_pause = FALSE;
    return 1;
}




int __stdcall WMp3::Seek(int fFormat, MP3_TIME *pTime, int nMoveMethod)
{

	if(pTime == 0)
	{
		error("Seek->MP3_TIME *pTime can't be 0 !");
		return 0;
	}

	if(fFormat == 0)
	{
		error("Seek->Invalid time format !");
		return 0;
	}


	if(c_stream_managed)
	{
		error("Seek->This is managed stream. Can't seek !");
		return 0;
	}

	if(!c_start)
	{
		error("Seek->Stream isn't open !");
		return 0;
	}



	unsigned int seek_time = 0;
	switch(fFormat)
	{
		case TIME_FORMAT_MS:
			seek_time = pTime->ms;
		break;

		case TIME_FORMAT_SEC:
			seek_time = pTime->sec * 1000;
		break;

		case TIME_FORMAT_HMS:
			seek_time = pTime->hms_millisecond;
			seek_time += pTime->hms_second * 1000;
			seek_time += pTime->hms_minute * 60000;
			seek_time += pTime->hms_hour * 3600000;
		break;
		case TIME_FORMAT_BYTES:
		{
			if(c_play)
				Stop();

			unsigned int pos = 0;
			switch(nMoveMethod)
			{
				case SONG_BEGIN:
				{
					if(pTime->bytes > c_size)
						pos = c_size;
					else
						pos = pTime->bytes;
				}	
				break;
				
				case SONG_END:
				{
					if(pTime->bytes > c_size)
						pos = 0;
					else
						pos = c_size - pTime->bytes;			
				}
				break;
				
				case SONG_CURRENT_FORWARD:
				{
					if(pTime->bytes > ( c_size - c_current_position))
						pos = c_size;
					else
						pos = c_current_position + pTime->bytes;
				
			
				}
				break;
				
				case SONG_CURRENT_BACKWARD:
				{
					if(pTime->bytes > c_current_position)
						pos = c_size;
					else
						pos = c_current_position - pTime->bytes;			
				}
				break;
					
			}
			
			c_position = c_start + pos;

				if(c_valid_xing)
				{
					if ((xing.flags & TAG_XING) && (xing.xing.flags & TAG_XING_TOC))
					{
						unsigned int i;
						int l = 0;
						int h = 99;
						int x_perc = pos * 256 / c_size ;
						for(i = 0; i < 100; i++)
						{
							if(xing.xing.toc[i] > x_perc)
							{
								h = i;
								if(i == 0)
									l = 0;
								else
									l = i - 1;
								break;	
							}
						}

						double low_ms = (double) c_song_length_ms * 100.0 / (double) l;
						double high_ms = (double) c_song_length_ms * 100.0 / (double) h;	

						double low_bytes = (double) c_size / 256.0 * xing.xing.toc[l];
						double high_bytes = (double) c_size / 256.0 * xing.xing.toc[h];

						double bytes_perc = 0;
						if(low_bytes < high_bytes)
						{
							bytes_perc = ((double) pos - low_bytes ) / (high_bytes - low_bytes);
						}
						
						c_nPosition = (unsigned int) (low_ms + (high_ms -  low_ms) * bytes_perc);

				 }
				 else 
					c_nPosition = MulDiv(pos ,c_song_length_ms , c_size);
			
				}
				else
					c_nPosition = MulDiv(pos , c_song_length_ms , c_size);


				return 1;		

		}
		break;

	}


	unsigned int nMilliSeconds = 0;

	switch(nMoveMethod)
	{
		case SONG_BEGIN:
			nMilliSeconds = seek_time;
		break;
		
		case SONG_END:
		{
			if( seek_time > c_song_length_ms)
				nMilliSeconds = 0;
			else
				nMilliSeconds = c_song_length_ms - seek_time;	
		}
		break;
		
		case SONG_CURRENT_FORWARD:
		{
			
			MP3_TIME cTime;
			GetPosition(&cTime);
			nMilliSeconds = seek_time + cTime.ms;
	
		}
		break;
		
		case SONG_CURRENT_BACKWARD:
		{
			MP3_TIME cTime;
			GetPosition(&cTime);
			nMilliSeconds = seek_time + cTime.ms;
			if(seek_time > cTime.ms)
				nMilliSeconds = 0;
			else
				nMilliSeconds = cTime.ms - seek_time;		
		}
		break;	
	}

	if(c_play)
		Stop();


	
    if(nMilliSeconds  > c_song_length_ms)
	{
		c_position = c_end;
		c_nPosition = c_song_length_ms;	
		return 1;
	}



	if(c_valid_xing)
	{
		if ((xing.flags & TAG_XING) && (xing.xing.flags & TAG_XING_TOC)) {
			
			double pa, pb, px;
            double percentage = 100.0 * (double) nMilliSeconds / (double) c_song_length_ms;
			
			int perc = (int) percentage;
			if (perc > 99) perc = 99;
			pa = xing.xing.toc[perc];
			if (perc < 99)
				pb = xing.xing.toc[perc+1];
            else 
				pb = 256;
         

            px = pa + (pb - pa) * (percentage - perc);
            c_position = c_start  + (unsigned int) (((double) ( c_size + c_xing_frame_size )   / 256.0) * px);
			

			
         }
		 else 
			c_position = c_start + (unsigned int) ((double) nMilliSeconds / (double) c_song_length_ms * (double) c_size); 
	
	}
	else 
		c_position = c_start + (unsigned int) ((double) nMilliSeconds / (double) c_song_length_ms * (double) c_size); 
	

	if(c_position >= c_end)
	{
		c_position = c_end;
		c_nPosition = c_song_length_ms;
	}
	else
		c_nPosition = nMilliSeconds;



	return 1;

}





int __stdcall WMp3::GetStatus(MP3_STATUS *pStatus)
{
	if(pStatus == 0)
	{
		error("GetStatus->MP3_STATUS *pStatus - this parameter can't be NULL");
		return 0;
	}


	pStatus->fStop = 1;
	pStatus->fPlay = 0;
	pStatus->fPause = 0;


	if(c_play)
	{
		pStatus->fPlay = 1;
		pStatus->fStop = 0;
	}

	if(c_pause)
	{
    	pStatus->fPause = 1;
		pStatus->fPlay = 0;
		pStatus->fStop = 0;
	}

	
	pStatus->fEcho = c_echo;
	pStatus->nSfxMode = c_echo_mode;
	pStatus->fExternalEQ = c_bUseExternalEQ; 
	pStatus->fInternalEQ = c_bUseInternalEQ;
	pStatus->fVocalCut = c_bVocalCut;
	pStatus->fChannelMix = c_mix_channels;
	pStatus->fFadeIn = c_bFadeIn;
	pStatus->fFadeOut = c_bFadeOut;
	pStatus->fInternalVolume = c_internalVolume;
	pStatus->fLoop = c_in_loop;
	pStatus->fReverse = c_fReverseMode;


	return 1;

}







int __stdcall WMp3::GetBitrate(int fAverage)
{ 
	if(!c_start)
		return 0;

	if(fAverage)
	{
		if(c_frame_counter == 0)
			return 0;

		return c_bitrate_sum / c_frame_counter;
	}

	return c_current_bitrate / 1000;
	
}








int __stdcall WMp3::Close()
{
	if(!c_start) return 1;
	
	Stop();

	w_wave->Uninitialize();

    CloseHandle(w_hEvent);
    w_hEvent = 0;


	if(w_hFile != INVALID_HANDLE_VALUE)
	{
		UnmapViewOfFile(w_mapping);
		w_mapping = 0;
		CloseHandle(w_hFileMap);
		w_hFileMap = NULL;
		CloseHandle(w_hFile);
		w_hFile = INVALID_HANDLE_VALUE;

	}

	if(c_gapless_buffer)
		free(c_gapless_buffer);

	c_gapless_buffer = 0;

	if(w_hThread)
		CloseHandle(w_hThread);

	w_hThread = 0;

	c_stream_managed = 0;
	c_buffered_stream = 0;

	if(c_pFramePtr)
		free(c_pFramePtr);

	c_pFramePtr = 0;
	c_nFrameNumber = 0;
	c_fReverseMode = 0;


	c_nFreeBitrate = 0;

	c_queue.Clear();
	c_buffered_queue.Clear();


	if(c_id3header.unsync_buf)
		free(c_id3header.unsync_buf);

	c_id3header.unsync_buf = 0;

	memset(&c_id3header, 0, sizeof(ID3v2Header));

	

	init(this);

	return 1;

	

}




int __stdcall WMp3::OpenResource(HMODULE hModule, const char *sResName, const char *sResType,  int nWaveBufferLengthMs, unsigned int nSeekFromStart, unsigned int nResourceSize)
{
	Close();
	c_stream_managed = 0;
	c_skip_xing = 0;
	if(c_stream_start)
	{
		error("OpenMp3Resource->You need to close mp3 stream before open new one !");
		return 0;
	}

	HRSRC hrSrc = FindResource(hModule, sResName, sResType);
	if(!hrSrc)
	{
		error("OpenMp3Resource->Can't find resource !");
		return 0;

	}

	HGLOBAL hRes = LoadResource(hModule, hrSrc);

	if(!hRes)
	{
		error("OpenMp3Resource->Can't load resource !");
		return 0;

	}

	char* res = (char*) LockResource(hRes);

	if(!res)
	{
		error("OpenMp3Resource->Can't lock resource !");
		return 0;

	}

	c_stream_size = SizeofResource(hModule, hrSrc);
	if(c_stream_size == 0)
	{
		error("OpenMp3Resource->Can't get resource size !");
		return 0;

	}

/*
	if(nResourceSize != c_stream_size)
		c_skip_xing = 1;
	*/

	if(nSeekFromStart >= c_stream_size)
	{
		error("OpenMp3Resource->Seek value out of range !");
		c_stream_size = 0;
        return 0;

	}

	if(nResourceSize == 0)
		nResourceSize = c_stream_size - nSeekFromStart;

	if(nResourceSize > (c_stream_size - nSeekFromStart))
	{
		error("OpenMp3Resource->Size value out of range !");
		c_stream_size = 0;
        return 0;
	}


	c_stream_start = (unsigned char*)  res;
	if(nResourceSize)
		c_stream_size = nResourceSize;

	c_stream_end = c_stream_start + c_stream_size - 1;

	return _OpenMp3(this, nWaveBufferLengthMs);
	

}




int __stdcall WMp3::SetVolume(unsigned int nLeftVolume, unsigned int nRightVolume)
{

    if( nLeftVolume > 100)
    	nLeftVolume = 100;

	if( nRightVolume > 100)
    	nRightVolume = 100;

	if(c_internalVolume)
	{	
		_fade_volume = 100.0;
		c_lvolume =  nLeftVolume;
		c_rvolume = nRightVolume;


		c_dLeftGainVolume =  ( nLeftVolume < 100 ? (pow(10, (double) nLeftVolume / 100.0) / 10.0 - 0.1) : 1.0);		
		c_dRightGainVolume =  ( nRightVolume < 100 ? (pow(10, (double) nRightVolume / 100.0) / 10.0 - 0.1) : 1.0);

		c_dLeftGain = c_dLeftGainVolume * c_dLeftGainFade;
		c_dRightGain = c_dRightGainVolume * c_dRightGainFade;

    }
    else
	{
		unsigned int l = (unsigned int) (nLeftVolume * 0xFFFF / 100);
		unsigned int r = (unsigned int) (nRightVolume * 0xFFFF / 100);
		w_wave->SetVolume((unsigned short) l, (unsigned short) r);

    }
	
	return 1;
}



int __stdcall WMp3::GetVolume(unsigned int *pnLeftVolume, unsigned int *pnRightVolume)
{
	if(pnLeftVolume == 0 || pnRightVolume == 0)
	{
		error("GetVolume->Invalid parameters !");
		return 0; 
	}


	if(c_internalVolume)
	{
		*pnLeftVolume = c_lvolume;
		*pnRightVolume = c_rvolume;

    }
    else {
		WORD lvolume;
		WORD rvolume;
    	w_wave->GetVolume(&lvolume,  &rvolume);
		*pnLeftVolume = MulDiv(lvolume , 100 , 65535);
		*pnRightVolume = MulDiv(rvolume , 100 , 65535);
    }

	return 1;

}





void CALLBACK WMp3::_WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
				DWORD dwParam1, DWORD dwParam2)
{



    WMp3* mp3 = (WMp3*) dwInstance;
	WAVEHDR* header = (WAVEHDR *) dwParam1;
 	switch(uMsg) {
    	case WOM_OPEN:
			//::SetEvent(mp3->w_hEvent);
        break;

        case WOM_CLOSE:
			::SetEvent(mp3->w_hEvent);
        break;

        case WOM_DONE:
		{
			mp3->c_dwPos += header->dwBufferLength;

			if(header->dwUser == mp3->w_wave->GetNumOfBuffers() - 1)
				mp3->c_dwIndex = 0;
			else
				mp3->c_dwIndex = header->dwUser + 1;

		

			::SetEvent(mp3->w_hEvent);
			mp3->c_vudata_ready = TRUE;

		

		}
        break;
    }
}




void WMp3::error(char* err_message)
{
	size_t size = 0;
	if(err_message != 0)
	{
		size = strlen(err_message );
		size = (size > 	ERRORMESSAGE_SIZE) ? ( ERRORMESSAGE_SIZE - 1) : size;
		strncpy(_lpErrorMessage, err_message, size);
		_lpErrorMessage[size] = 0;
	}
	_lpErrorMessage[size]= 0;
	
}






unsigned long WMp3::_Get32bits(unsigned char *buf) {

	unsigned long ret;
    ret = (((unsigned long) buf[0]) << 24) |
	(((unsigned long) buf[1]) << 16) |
	(((unsigned long) buf[2]) << 8) |
	((unsigned long) buf[3]) ;

    return ret;
}



int __stdcall WMp3::GetVUData(unsigned int *pnLeftChannel, unsigned int *pnRightChannel)
{
	if(pnLeftChannel == 0 || pnRightChannel == 0)
	{
		error("GetVUData->Invalid parameters !");
		return 0; 
	}

	*pnLeftChannel = 0;
	*pnRightChannel = 0;	

	if(!c_start || !c_play || !c_vudata_ready)
		return 1;



	short* left1;
	short* right1;
	real sum_left = 0;
	real avg_left;
	real sumww_right = 0;
	real avgww_right;
	real num = 0;
	real la;
	real ra;

	

	// current playing position ( bytes )
	unsigned int pos = w_wave->GetPosition(BYTES);
	// wave output buffer size ( bytes )
	unsigned int buff_size = w_wave->GetBufferSize(c_dwIndex);

	// current playing wave buffer
	char* data = w_wave->GetBufferData(c_dwIndex);

	unsigned int num_chunk = 2;
	
	unsigned int size = buff_size / num_chunk ;

	
	

	int offset;
	offset = pos - c_dwPos;
	if(offset < 0)
		offset = 0;
	

	if(c_channel == 2)
	{
		if(num_chunk >= 2)
		{
		
			
			if(offset > buff_size)
			{
				if(c_dwIndex == w_wave->GetNumOfBuffers() - 1)
					c_dwIndex = 0;
				else
					c_dwIndex++;

				offset = 0;
				data = w_wave->GetBufferData(c_dwIndex);
			}

			if(offset + size + 4 > buff_size) 
				offset = buff_size - size - 4;
				
			
		}
		else 
			offset = 0;

		
		for(DWORD i = 0; i < size; i += 4)
		{
			left1 = (short*) ( data + i + offset);
			sum_left += (real) abs(*left1);
			right1 = (short*) ( data + i + offset + 2);
			sumww_right += (real) abs(*right1);
			num++;

		}

		avg_left = sum_left / num;
		avgww_right = sumww_right / num;

		

		if(avg_left < 21.61)
			*pnLeftChannel = 0;
		else
		{
			la = avg_left / AVG_SINE_16;
			*pnLeftChannel =  (unsigned int) (100 + (33.0 * log10(la)));
			if(*pnLeftChannel > 100)
				*pnLeftChannel = 100;
				
		}


	
		if(avgww_right < 21.61)
			*pnRightChannel = 0;
		else
		{
			ra = avgww_right / AVG_SINE_16 ;
			*pnRightChannel = (unsigned int) (100 + (33.0 * log10(ra)));
			if(*pnRightChannel > 100)
				*pnRightChannel = 100;
		}		
	}
	else if(c_channel == 1)
	{
		if(num_chunk >= 2)
		{
			
			if(offset > buff_size)
			{
				if(c_dwIndex == w_wave->GetNumOfBuffers() - 1)
					c_dwIndex = 0;
				else
					c_dwIndex++;

				offset = 0;
				data = w_wave->GetBufferData(c_dwIndex);
			}

			if(offset + size + 2 > buff_size)
				offset = buff_size - size - 2;
				
			
		}
		else
			offset = 0;


		for(DWORD i = 0; i < size; i += 2)
		{
			left1 = (short*) ( data + i + offset);
			sum_left += (real) abs(*left1);
			num++;
		}

		avg_left = sum_left / num;


		if(avg_left < 21.61)
		{
			*pnLeftChannel = 0;
			*pnRightChannel = 0;
		}
		else
		{
			la = avg_left / AVG_SINE_16 ;
			*pnLeftChannel = (unsigned int) (100 + (33 * log10(la)));
			if(*pnLeftChannel > 100)
				*pnLeftChannel = 100;

			*pnRightChannel = *pnLeftChannel;
		}
	}

	return 1;

}





void RemoveEndingSpaces(char* buff)
{
	int size = strlen(buff);
	for(int i = size - 1; i >= 0; i--) {
		if(buff[i] == ' ')
			buff[i] = 0;
		else
			return;

	}

}



void WMp3::DoEcho(WMp3* mp3, short *buf, unsigned int len)
{


	unsigned int i;
	unsigned int j;
	unsigned int sample_num = len / 2;


	
	for( i = ECHO_BUFFER_NUMBER - 1; i > 0; i--)
		mp3->sfx_buffer1[i] = mp3->sfx_buffer1[i - 1];
		
	mp3->sfx_buffer1[0] = mp3->sfx_buffer1[ECHO_BUFFER_NUMBER - 1];
	

	// set to half volume
	for(i = 0; i < sample_num; i++)
		buf[i] /= 2;



	CopyMemory(mp3->sfx_buffer1[0], buf, len);	



// do echo efect

	real e;
	real f;
	int a;
	

   for(i = 0; i < sample_num; i++)
   {
		e = 0;
		f = 0;
	
		for(j = 0; j < ECHO_MAX_DELAY_NUMBER ; j++)
		{
		
			if(mp3->c_sfx_mode_l[j]) 
				e +=  (real) mp3->sfx_buffer1[j + mp3->c_sfx_offset][i] * mp3->c_sfx_mode_l[j];
				
		
			if(mp3->c_sfx_mode_r[j]) 
				f +=  (real) mp3->sfx_buffer1[j + mp3->c_sfx_offset][i + 1] * mp3->c_sfx_mode_r[j];
			
		}

		
		a = (int) (e * mp3->c_rEgOg + (real) buf[i] * mp3->c_rIgOg);

		if(a > 32767)
			a = 32767;

		if(a < -32768)
			a = -32768;

		buf[i] = (short) a;
		i++;
		a =  (int) (f * mp3->c_rEgOg + (real) buf[i] * mp3->c_rIgOg);

		if(a > 32767)
			a = 32767;

		if(a < -32768)
			a = -32768;

		buf[i] = (short) a;


   }

}


void WMp3::ClearSfxBuffers()
{
		// clear all buffers to 0
	if(sfx_buffer[0] != NULL) {
		for(unsigned int i = 0; i < ECHO_BUFFER_NUMBER; i++)
			ZeroMemory(sfx_buffer[i], c_echo_alloc_buffer_size);
		
	}	
	
}	



int __stdcall WMp3::SetSfxParam(int fEnable, int nMode,  int nDelayMs, int nInputGain, int nEchoGain, int nOutputGain)
{
	
	c_echo = FALSE;

	EnterCriticalSection(&c_wcsSfx);

	if(nDelayMs < 0)
		nDelayMs = 0;
		
	if(nDelayMs > MAX_DELAY)
		nDelayMs = MAX_DELAY;


		
	if(nMode > NUM_SFX_MODE - 1)
		nMode = NUM_SFX_MODE - 1;	

	if(nMode < 0)
		nMode = 0;
	
	int i;

	int mode = nMode * 2;

	for(i = 0; i < ECHO_MAX_DELAY_NUMBER ; i++)
	{
		c_sfx_mode_l[i] =  sfx_mode[mode][i] ;
		if(c_channel > 1)
			c_sfx_mode_r[i] = sfx_mode[mode + 1][i];
		else
			c_sfx_mode_r[i] = c_sfx_mode_l[i];	
	}
	
	c_echo_mode = nMode;

	

	if(fEnable == 0)
	{
		c_echo = 0;
		c_old_echo_buffer_size = 0;
		// free buffers
		if(sfx_buffer[0])
		{
			for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
			{
				free(sfx_buffer[i]);
				sfx_buffer[i] = 0;
				sfx_buffer1[i] = 0;
			}
		}

		LeaveCriticalSection(&c_wcsSfx);
		return 1;
	}



	


	if(fEnable)
	{ // need echo
		// if buffer size changed allocate new buffer
		if(reallocate_sfx_buffers(this) == 0)
		{
			error("SetSfxParam->Can't reallocate sfx buffers !");
			return 0;
		}


		c_nInputGain = nInputGain;
		c_nEchoGain = nEchoGain;
		c_nOutputGain = nOutputGain;
		c_nDelayMs = nDelayMs;


			
		c_rIgOg = pow(10.0, (double) ( (double) ( nInputGain + nOutputGain )  / 20));
		c_rEgOg = pow(10.0, (double) ( (double) ( nEchoGain + nOutputGain )  / 20));
		


		c_delay = nDelayMs;
		
			
		c_sfx_offset = ECHO_MAX_DELAY_NUMBER * c_delay / MAX_DELAY;

		
		ClearSfxBuffers();		
	
	}



	LeaveCriticalSection(&c_wcsSfx);
	if(fEnable)
		c_echo = 1;
	else
		c_echo = 0;	

	return 1;
}



int __stdcall WMp3::EnableEQ(int fEnable, int fExternal)
{
	if(fEnable)
	{
		if(fExternal) // enable external equalizer
		{ 
			// create event, manual reset, nonsignaled state
			c_hEventEqSetParam = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(c_hEventEqSetParam == 0)
			{
				error("EnableEQ->Can't create control event. Please try again !");
				return 0;

			}

			ResetEvent(c_hEventEqSetParam);

			// create thread

			c_hThreadEqSetParam = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) EqParamThreadFunc, (void*) this, 0, &c_hThreadEqSetParamId);
			if(c_hThreadEqSetParam == 0)
			{
				error("EnableEQ->Can't create  thread. Please try again !");
				CloseHandle(c_hEventEqSetParam);
				c_hEventEqSetParam = 0;
				return 0;

			}

			//SetThreadPriority(c_hThreadEqSetParam, THREAD_PRIORITY_BELOW_NORMAL	);

			// wait for thread to create message queue
			WaitForSingleObject(c_hEventEqSetParam, INFINITE);

			equ_clearbuf(&eq_inter_param, c_sampling_rate);

			// send message to generate eq tables
			PostThreadMessage(c_hThreadEqSetParamId, MSG_EQ_PARAM_SET, 0,0);
	

		}
		else  // enable internal equalizer
		{ 
			c_bUseInternalEQ = TRUE; // start internal equalizer
		}
	}
	else
		if(fExternal)  // disable external equalizer
		{
			if(c_hThreadEqSetParam)
			{
				// stop thread
				PostThreadMessage(c_hThreadEqSetParamId, WM_QUIT, 0,0);
				if(WaitForSingleObject(c_hThreadEqSetParam, 500) == WAIT_TIMEOUT)
					TerminateThread(c_hThreadEqSetParam, 0);

				
				CloseHandle(c_hThreadEqSetParam);
				
				c_hThreadEqSetParam = 0;

				// close event
				CloseHandle(c_hEventEqSetParam);
				c_hEventEqSetParam = 0;
				
			}



			c_bUseExternalEQ = FALSE;
		}
		else   // disable internal equalizer
			c_bUseInternalEQ = FALSE;

	return 1;
}


int WINAPI WMp3::EqParamThreadFunc(void* lpdwParam)
{
	WMp3 *mp3 = (WMp3*) lpdwParam;
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(mp3->c_hEventEqSetParam);



	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		switch(msg.message)
		{
			case MSG_EQ_PARAM_SET:
			{
				equ_makeTable(&mp3->eq_inter_param, mp3->c_lbands, mp3->c_rbands, &mp3->c_paramroot, (REAL) mp3->c_sampling_rate);
				mp3->c_old_sampling_rate = mp3->c_sampling_rate;
				mp3->c_bUseExternalEQ = TRUE; // start external equalizer

			
			}
			break;	
		}


	}


	return 1;
}




int __stdcall WMp3::SetEQParam(int fExternal, int nPreAmpGain, int *pnBandGain, int nNumberOfBands)
{
	int i;
	double AmpFactor;
	double max = mad_f_todouble(MAD_F_MAX);
	

	if(!fExternal) { // set parameters for internal equalizer

		for(i = 0; i < 10; i++)
		{
			c_EqFilter1[i] = pnBandGain[i] * 1000;
		}

		c_EqFilter[0] = pnBandGain[0];
		c_EqFilter[1] = pnBandGain[1];
		c_EqFilter[2] = pnBandGain[2];
		c_EqFilter[3] = pnBandGain[3];
		c_EqFilter[4] = pnBandGain[4];
		c_EqFilter[5] = pnBandGain[5];
		c_EqFilter[6] = pnBandGain[5];
		c_EqFilter[7] = pnBandGain[5];
		c_EqFilter[8] = pnBandGain[6];
		c_EqFilter[9] = pnBandGain[6];
		c_EqFilter[10] = pnBandGain[6];
		c_EqFilter[11] = pnBandGain[6];
		c_EqFilter[12] = pnBandGain[6];
		c_EqFilter[13] = pnBandGain[6];
		c_EqFilter[14] = pnBandGain[7];
		c_EqFilter[15] = pnBandGain[7];
		c_EqFilter[16] = pnBandGain[7];
		c_EqFilter[17] = pnBandGain[7];
		c_EqFilter[18] = pnBandGain[7];
		c_EqFilter[19] = pnBandGain[8];
		c_EqFilter[20] = pnBandGain[8];
		c_EqFilter[21] = pnBandGain[8];
		c_EqFilter[22] = pnBandGain[8];
		c_EqFilter[23] = pnBandGain[8];
		c_EqFilter[24] = pnBandGain[9];
		c_EqFilter[25] = pnBandGain[9];
		c_EqFilter[26] = pnBandGain[9];
		c_EqFilter[27] = pnBandGain[9];
		c_EqFilter[28] = pnBandGain[9];
		c_EqFilter[29] = pnBandGain[9];
		c_EqFilter[30] = pnBandGain[9];
		c_EqFilter[31] = pnBandGain[9];

		for(i = 0; i < 32; i++) {
			AmpFactor = pow(10.0, (double) ( (double) ( c_EqFilter[i] + nPreAmpGain )  / 20));
			if(AmpFactor > max)
				AmpFactor = max;

			c_EqFilter[i] = mad_f_tofixed(AmpFactor);
			 
		}

		c_eq_internal_preamp = nPreAmpGain * 1000;

		return 1;
	}

	// set parameters for external equalizer
	
	int size = nNumberOfBands;
	if(size > c_bands_num)
		size = c_bands_num;

	double amp;

	for(i = 0; i < size; i++)
		c_eq[i] = pnBandGain[i] * 1000;



	if(c_eq_band_points[0] == 0)
	{
		for(i = 0; i < size; i++)
		{
			amp = pow(10.0 , (double) ((double) nPreAmpGain + (double) c_eq[i] / 1000.0) / 20.0);
			c_lbands[i] = (REAL) amp;
			c_rbands[i] = (REAL) amp;
		}
	}
	else
	{
		c_lbands[0] = 1.0;
		c_rbands[0] = 1.0;
		for(i = 0; i < size; i++)
		{
			amp = pow(10.0 , (double) ((double) nPreAmpGain + (double) c_eq[i]/ 1000.0) / 20.0);
			c_lbands[i + 1] = (REAL) amp;
			c_rbands[i + 1] = (REAL) amp;
		}
	}

	if(c_eq_band_points[c_eq_band_points_num - 1] != EQ_MAX_FREQ)
	{
		c_lbands[c_eq_band_points_num] = 1.0;
		c_rbands[c_eq_band_points_num] = 1.0;
	}

 


	c_eq_preamp = nPreAmpGain * 1000;
		
	// generate new eq tables
	if(c_bUseExternalEQ)
		PostThreadMessage(c_hThreadEqSetParamId, MSG_EQ_PARAM_SET, 0,0);
			


	return 1;

}




int __stdcall WMp3::SetEQParamEx(int fExternal, int nPreAmpGain, int *pnBandGain, int nNumberOfBands)
{
	int i;
	double AmpFactor;
	double max = mad_f_todouble(MAD_F_MAX);
	

	if(!fExternal) { // set parameters for internal equalizer

		for(i = 0; i < 10; i++)
		{
			c_EqFilter1[i] = pnBandGain[i];
		}

		c_EqFilter[0] = pnBandGain[0] / 1000;
		c_EqFilter[1] = pnBandGain[1] / 1000;
		c_EqFilter[2] = pnBandGain[2] / 1000;
		c_EqFilter[3] = pnBandGain[3] / 1000;
		c_EqFilter[4] = pnBandGain[4] / 1000;
		c_EqFilter[5] = pnBandGain[5] / 1000;
		c_EqFilter[6] = pnBandGain[5] / 1000;
		c_EqFilter[7] = pnBandGain[5] / 1000;
		c_EqFilter[8] = pnBandGain[6] / 1000;
		c_EqFilter[9] = pnBandGain[6] / 1000;
		c_EqFilter[10] = pnBandGain[6] / 1000;
		c_EqFilter[11] = pnBandGain[6] / 1000;
		c_EqFilter[12] = pnBandGain[6] / 1000;
		c_EqFilter[13] = pnBandGain[6] / 1000;
		c_EqFilter[14] = pnBandGain[7] / 1000;
		c_EqFilter[15] = pnBandGain[7] / 1000;
		c_EqFilter[16] = pnBandGain[7] / 1000;
		c_EqFilter[17] = pnBandGain[7] / 1000;
		c_EqFilter[18] = pnBandGain[7] / 1000;
		c_EqFilter[19] = pnBandGain[8] / 1000;
		c_EqFilter[20] = pnBandGain[8] / 1000;
		c_EqFilter[21] = pnBandGain[8] / 1000;
		c_EqFilter[22] = pnBandGain[8] / 1000;
		c_EqFilter[23] = pnBandGain[8] / 1000;
		c_EqFilter[24] = pnBandGain[9] / 1000;
		c_EqFilter[25] = pnBandGain[9] / 1000;
		c_EqFilter[26] = pnBandGain[9] / 1000;
		c_EqFilter[27] = pnBandGain[9] / 1000;
		c_EqFilter[28] = pnBandGain[9] / 1000;
		c_EqFilter[29] = pnBandGain[9] / 1000;
		c_EqFilter[30] = pnBandGain[9] / 1000;
		c_EqFilter[31] = pnBandGain[9] / 1000;

		for(i = 0; i < 32; i++) {
			AmpFactor = pow(10.0, (double) ( (double) ( (double) c_EqFilter[i] + (double) nPreAmpGain / 1000.0 )  / 20));
			if(AmpFactor > max)
				AmpFactor = max;

			c_EqFilter[i] = mad_f_tofixed(AmpFactor);
			 
		}

		c_eq_internal_preamp = nPreAmpGain;

		return 1;
	}

	// set parameters for external equalizer
	
	int size = nNumberOfBands;
	if(size > c_bands_num)
		size = c_bands_num;

	double amp;

	for(i = 0; i < size; i++)
		c_eq[i] =  pnBandGain[i];



	if(c_eq_band_points[0] == 0)
	{
		for(i = 0; i < size; i++)
		{
		
			amp = pow(10.0 , (double) ((double) nPreAmpGain / 1000.0 + (double) c_eq[i] / 1000.0) / 20.0);
			c_lbands[i] = (REAL) amp;
			c_rbands[i] = (REAL) amp;
		}
	}
	else
	{
		c_lbands[0] = 1.0;
		c_rbands[0] = 1.0;
		for(i = 0; i < size; i++)
		{
			amp = pow(10.0 , (double) ((double) nPreAmpGain / 1000.0 + (double) c_eq[i] / 1000.0) / 20.0);
			c_lbands[i + 1] = (REAL) amp;
			c_rbands[i + 1] = (REAL) amp;
		}
	}

	if(c_eq_band_points[c_eq_band_points_num - 1] != EQ_MAX_FREQ)
	{
		c_lbands[c_eq_band_points_num] = 1.0;
		c_rbands[c_eq_band_points_num] = 1.0;
	}

 


	c_eq_preamp = nPreAmpGain;
		
			
	// generate new eq tables
	if(c_bUseExternalEQ)
		PostThreadMessage(c_hThreadEqSetParamId, MSG_EQ_PARAM_SET, 0,0);
	

	return 1;

}



int __stdcall WMp3::GetEQParam(int fExternal, int *pnPreAmpGain, int *pnBandGain)
{
	if(pnPreAmpGain == 0)
	{
		if(fExternal)
			return c_bands_num;

		return 10;
	}


	if(fExternal == 0)
	{ 
		int i;
		for(i = 0; i < 10; i++)
		{
			pnBandGain[i] = c_EqFilter1[i] / 1000;
		}

	

		*pnPreAmpGain = c_eq_internal_preamp / 1000;


		return 10;
	}



	// set parameters for external equalizer
	
	int i;
	for(i = 0; i < c_bands_num; i++)
		pnBandGain[i] = c_eq[i] / 1000;
		
	
	*pnPreAmpGain = c_eq_preamp / 1000;
	return c_bands_num;

}



int __stdcall WMp3::GetEQParamEx(int fExternal, int *pnPreAmpGain, int *pnBandGain)
{
	if(pnPreAmpGain == 0)
	{
		if(fExternal)
			return c_bands_num;

		return 10;
	}





	if(fExternal == 0)
	{ 

		int i;
		for(i = 0; i < 10; i++)
		{
			pnBandGain[i] = c_EqFilter1[i];
		}


	

		*pnPreAmpGain = c_eq_internal_preamp;
		

		return 10;
	}

	// set parameters for external equalizer
	
	int i;
	for(i = 0; i < c_bands_num; i++)
		pnBandGain[i] = c_eq[i];
		
	
	*pnPreAmpGain = c_eq_preamp;
	return c_bands_num;

}




int __stdcall WMp3::VocalCut(int fEnable)
{
	if(fEnable)
		c_bVocalCut = 1;
	else
		c_bVocalCut = 0; 

	return 1;

}



int __stdcall WMp3::InternalVolume(int fEnable)
{
	c_internalVolume = fEnable;
	return 1;

}









int WMp3::_OpenMp3(WMp3* mp3, int WaveBufferLength)
{
// open mp3 file
// INPUT:	WMp3* mp3	- valid mp3 class pointer
//			char* stream	- mp3 memory stream
//			WaveBufferLength	- wave buffer length in milliseconds ( this value will be divided into small 100 ms buffers
//			DWORD seek			- if you need to skip some data in stream, e.g. set real stream beginning to new position
//			DWORD size			- size of stream, if you seek stream, set correct size, if size = 0 stream size is calculated


	

	
	unsigned char* tmp = 0;

	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_header header;


	mp3->c_ID3v1 = FALSE;
	mp3->c_ID3v2 = FALSE;

	mp3->c_start = mp3->c_stream_start;
	mp3->c_end = mp3->c_stream_end;
	mp3->c_position = mp3->c_stream_start;
	mp3->c_size = mp3->c_stream_size;

	mp3->c_current_position = 0;


	

// now we need to find first valid mp3 frame

// 1. check for ID3 tags
//
// 1.1 check for ID3v1 tag

	if(mp3->c_size > 128 && memcmp(mp3->c_end - 127,"TAG",3) == 0)
	{ // we found ID3v1 tag
		mp3->c_end -= 128;
		mp3->c_size -= 128;
		mp3->c_ID3v1 = TRUE;	
	}



// 1.2 check for ID3v2 tag




	if(( mp3->c_size  > 10 ) && (memcmp(mp3->c_start,"ID3",3) == 0) &&
		mp3->c_start[6] < 0x80 && mp3->c_start[7] < 0x80 &&
		 mp3->c_start[8] < 0x80 && mp3->c_start[9] < 0x80)
	{ 
		// we found ID3v1 tag
		// calculate ID3v2 frame size
		unsigned int id3v2_size	= GetFourByteSyncSafe( mp3->c_start[6], mp3->c_start[7], mp3->c_start[8], mp3->c_start[9]);
		
		// add ID3v2 header
		id3v2_size += 10; // size of ID3V2 frame
		if(mp3->c_size >  ( id3v2_size + MIN_FRAME_SIZE) )
		{
			mp3->c_ID3v2 = TRUE;

			
			if(SKIP_ID3TAG_AT_BEGINNING && ( *(mp3->c_start + id3v2_size) == 0xFF) &&  (( *(mp3->c_start + id3v2_size + 1) & 0xE0) == 0xE0))
			{
				mp3->c_start += id3v2_size;
				mp3->c_size -= id3v2_size;
					
			}
		}
	}

	

	
// now we have clear mp3 stream without ID3 tags
//
// 2. seek across the stream and find first valid mp3 frame


	if(mp3->c_size < MAD_BUFFER_GUARD)
	{
		mp3->error("_OpenMp3->Invalid stream!");
		mp3->init(mp3);
		return FALSE;
	}


	
	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_header_init(&header);




	mad_stream_buffer(&stream, mp3->c_start, mp3->c_size - MAD_BUFFER_GUARD);

	unsigned char* first_frame = mp3->c_start;

// searching for first mp3 frame
	while(1)
	{
		while(mad_frame_decode(&frame, &stream))
		{
			if(MAD_RECOVERABLE(stream.error))
				continue;

			mp3->error("_OpenMp3->Can't find first valid mp3 frame");
			mad_stream_finish(&stream);
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			
			mp3->init(mp3);
			return FALSE;

		}

		first_frame =  (unsigned char*) stream.this_frame;

		// we have possible first frame, but we need more checking, now we will check more frames
		
		// remember some infos about stream

		mp3->c_sampling_rate = frame.header.samplerate;
		mp3->c_layer = frame.header.layer;
		mp3->c_mode = frame.header.mode;
		mp3->c_channel = ( frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
		mp3->c_emphasis = frame.header.emphasis;
		mp3->c_mode_extension = frame.header.mode_extension;
		mp3->c_bitrate = frame.header.bitrate;
		mp3->c_flags = frame.header.flags;
		mp3->c_avg_bitrate = 0;
//		mp3->c_duration = frame.header.duration;
		mp3->c_sample_per_frame = (frame.header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152;




		if( (mp3->c_flags & MAD_FLAG_MPEG_2_5_EXT))
			mp3->c_mpeg_version = MPEG_25;
		else if((mp3->c_flags & MAD_FLAG_LSF_EXT))
			mp3->c_mpeg_version = MPEG_2;
		else
			mp3->c_mpeg_version = MPEG_1;


		switch(mp3->c_layer)
		{
			case MAD_LAYER_I:
				mp3->c_mpeg_layer_version = LAYER_1;
			break;

			case MAD_LAYER_II:
				mp3->c_mpeg_layer_version =	LAYER_2;
			break;

			case MAD_LAYER_III:
				mp3->c_mpeg_layer_version =	LAYER_3; 
			break;
			
			default:
				mp3->c_mpeg_layer_version =	0; 
			break;		
		}


		switch(mp3->c_mode)
		{
			case MAD_MODE_SINGLE_CHANNEL:
				mp3->c_channel_mode = MODE_SINGLE_CHANNEL;
			break;

			case MAD_MODE_DUAL_CHANNEL:
				mp3->c_channel_mode = MODE_DUAL_CHANNEL;
			break;

			case MAD_MODE_JOINT_STEREO:
				mp3->c_channel_mode = MODE_JOINT_STEREO;
			break;

			case MAD_MODE_STEREO:
				mp3->c_channel_mode = MODE_STEREO;
			break;
		}


		switch(mp3->c_emphasis)
		{
			case MAD_EMPHASIS_NONE:
				mp3->c_emphasis_mode =  EMPHASIS_NONE;
			break;

			case MAD_EMPHASIS_50_15_US:
				mp3->c_emphasis_mode = EMPHASIS_50_15_US;
			break;

			case MAD_EMPHASIS_CCITT_J_17:
				mp3->c_emphasis_mode = EMPHASIS_CCITT_J_17;
			break;

			case MAD_EMPHASIS_RESERVED:
				mp3->c_emphasis_mode = EMPHASIS_RESERVED; 
			break;
		}
   

		// check next frame
		if(mad_frame_decode(&frame, &stream))
		{
			if(MAD_RECOVERABLE(stream.error))
				continue;

			mp3->error("_OpenMp3->Can't find first valid mp3 frame");
			mad_stream_finish(&stream);
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			mp3->init(mp3);
			return FALSE;
		}
		else
		{
			// more checkinh
			if(mp3->c_sampling_rate != frame.header.samplerate ||
				mp3->c_layer != frame.header.layer)
					continue;
					
			break;	
		}		
	}

	// now we have valid first frame


	mp3->c_nFreeBitrate = stream.freerate;
	
	
	mp3->c_size -= (first_frame - mp3->c_start);

	mp3->c_start = first_frame;



	if(mp3->c_size < MAD_BUFFER_GUARD)
	{
		mp3->error("_OpenMp3->Can't find first valid mp3 frame");
		mad_stream_finish(&stream);
		mad_frame_finish(&frame);
		mad_header_finish(&header);
		mp3->init(mp3);
		return FALSE;
	}





// 3. calculate length of stream
//
// 3.1. check if first frame is XING frame


// set stream to beginning

	mp3->c_valid_xing = FALSE;

	mad_stream_buffer(&stream, mp3->c_start, mp3->c_size - MAD_BUFFER_GUARD);

	if(mad_frame_decode(&frame, &stream) == 0)
	{
		// check if first frame is XING frame
		
		if(mp3->c_skip_xing == 0 && tag_parse(&mp3->xing, &stream, &frame) == 0)
		{
			if(mp3->xing.flags & TAG_XING)
			{ // we have XING frame
				// calculate song length
				if((mp3->xing.xing.flags & TAG_XING_FRAMES) && mp3->xing.xing.flags & TAG_XING_TOC)
				{
					mp3->c_song_length_frames = (unsigned int) mp3->xing.xing.frames;
					mp3->c_song_length_samples = mp3->c_song_length_frames * mp3->c_sample_per_frame;
					mp3->c_song_length_ms = (unsigned int) ( 1000.0 * (double) mp3->c_song_length_frames * (double) mp3->c_sample_per_frame / (double) mp3->c_sampling_rate);
				
			// skip XING frame 
					
					mp3->c_size -= ( stream.next_frame - mp3->c_start);
					mp3->c_start = (unsigned char*) stream.next_frame;

					mp3->c_song_length_bytes = mp3->c_size;
					mp3->c_avg_frame_size =(float) ( (double) mp3->c_song_length_bytes / (double) mp3->c_song_length_frames); 
					mp3->c_avg_bitrate = (float) ((double) mp3->c_song_length_bytes * 8 / (double) mp3->c_song_length_frames *  (double) mp3->c_sampling_rate / (double) mp3->c_sample_per_frame);
				
				
					mp3->c_xing_frame_size = stream.next_frame - stream.this_frame;		
					//mp3->c_vbr = 1;
					mp3->c_valid_xing = TRUE;

					
				}
			}	
		}
	}


	if(mp3->c_size < MAD_BUFFER_GUARD)
	{
		mp3->error("_OpenMp3->Can't find first valid mp3 frame");
		mad_stream_finish(&stream);
		mad_frame_finish(&frame);
		mad_header_finish(&header);
		mp3->init(mp3);
		return FALSE;
	}


// scan mp3 file and check if CBR or VBR


		mad_stream_buffer(&stream, mp3->c_start, mp3->c_size);

		unsigned int frame_num = 0;
		mp3->c_vbr = 0;
		unsigned int size = 0;
		

		tmp = 0;
		while(frame_num < SEARCH_DEEP_VBR)
		{
			if(mad_header_decode(&header,&stream)) { // if some error
				if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
					continue;
		
				

				if(stream.error == MAD_ERROR_BUFLEN)
				{ // if buffer end
					if(tmp) {
						free(tmp);
						tmp = 0;
						break;
					}
				// fix MAD_BUFFER_GUARD problem
					unsigned int len = mp3->c_end + 1 - stream.this_frame;	
					tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
					if(!tmp)
						break;

					// copy last frame into buffer and add MAD_BUFFER_GUARD zero bytes to end
					memcpy(tmp, stream.this_frame, len);
					memset(tmp + len,0,MAD_BUFFER_GUARD);
					mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
					continue;
				}

			}


			if(header.bitrate != mp3->c_bitrate)  // bitrate changed
				mp3->c_vbr = 1;	
	
			size += header.size;
			frame_num++;
			
		}
 
		if(tmp)
			free(tmp);

		tmp = 0;


		if(!mp3->c_valid_xing && mp3->c_vbr)
		{
			mp3->c_song_length_frames = (unsigned int) ((double) frame_num * (double) mp3->c_size / (double) size);
			mp3->c_song_length_samples = mp3->c_song_length_frames * mp3->c_sample_per_frame;
			mp3->c_song_length_ms = (unsigned int) ( 1000.0 * (double) mp3->c_song_length_frames * (double) mp3->c_sample_per_frame / (double) mp3->c_sampling_rate);		
			mp3->c_song_length_bytes = mp3->c_size;
			mp3->c_avg_frame_size = (float) ((double) size / (double) frame_num);
			mp3->c_avg_bitrate = (float) ((double) mp3->c_song_length_bytes * 8 / (double) mp3->c_song_length_frames *  (double) mp3->c_sampling_rate / (double) mp3->c_sample_per_frame);	
		}
		else if(!mp3->c_valid_xing)
		{
			// CBR file
			mp3->c_avg_bitrate = (float) mp3->c_bitrate;
			double length =  (double) mp3->c_size * 8.0 / ( (double) mp3->c_avg_bitrate / 1000.0); 
			mp3->c_song_length_ms = (unsigned int) length;
			mp3->c_song_length_frames = (unsigned int) ceil(length / 1000 * (double) mp3->c_sampling_rate / (double) mp3->c_sample_per_frame);
			mp3->c_song_length_samples = mp3->c_song_length_frames * mp3->c_sample_per_frame;
			mp3->c_song_length_bytes = mp3->c_size;
			mp3->c_avg_frame_size = (float) ( (double) mp3->c_size / (double) mp3->c_song_length_frames);	
		
		}
	


	mad_stream_finish(&stream);
	mad_frame_finish(&frame);
	mad_header_finish(&header);


// initalize wave out


	mp3->c_no_audio_output = 0;

	if(WaveBufferLength == 0)
		mp3->c_no_audio_output = 1;

	

	


// check buffer length, we need at least 200 ms,

    if(mp3->c_no_audio_output == 0 && WaveBufferLength < 200 )
	{
	
    	mp3->error("_Open->Buffer can't be smaller than 200 ms");
		mp3->init(mp3);
		return FALSE;
		
    }

// create event, this event controls playing thread
    if( (mp3->w_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
	{
    	mp3->error("_Open->Can't create event");
		mp3->init(mp3);
		return FALSE;
    }


// calculate number of mp3 frames to fill one internal wave buffer ( ~ 100 ms )


	mp3->c_input_buffer_size = MulDiv(SINGLE_AUDIO_BUFFER_SIZE , mp3->c_sampling_rate , 1000 * mp3->c_sample_per_frame ); 
	

	if(mp3->c_input_buffer_size == 0)
		mp3->c_input_buffer_size = 1;

// calculate size of single wave buffer ( in milliseconds )
	int single_audio_buffer_len = MulDiv(mp3->c_input_buffer_size, mp3->c_sample_per_frame * 1000, mp3->c_sampling_rate) + 10;



// now, divide wave buffer length into number of small wave buffers
	int WaveBufferNum = MulDiv(WaveBufferLength,1 , single_audio_buffer_len - 1);

	if(mp3->c_no_audio_output)
		WaveBufferNum = 1;	


// initialize wave out, sampling rate, channel, bits per sample, ...
     if ( ! mp3->w_wave->Initialize(WAVE_FORMAT_PCM, (WORD)mp3->c_channel, mp3->c_sampling_rate, 16,
    		WaveBufferNum, single_audio_buffer_len))
	{

        mp3->error("_Open->Can't initialize wave out");
		CloseHandle(mp3->w_hEvent);
		mp3->init(mp3);
		return FALSE;
     }

	mp3->_fade_volume = 100.0;
	mp3->c_in_loop = 0;
	mp3->c_current_bitrate = 0;





	mp3->_fade_volume = 100.0;

	mp3->flying_loop = 0;


// calculate size for one echo buffer ( ~ 50 ms ) 
   mp3->c_echo_alloc_buffer_size = mp3->w_wave->GetBufferAllocSize() / 2 + 2;

	if(mp3->c_echo)
	{
		if(reallocate_sfx_buffers(mp3) == 0)
		{
			mp3->error("_Open->Can't allocate echo buffer");
			CloseHandle(mp3->w_hEvent);
			mp3->init(mp3);
			return FALSE;
		}
	}


	mp3->c_gapless_buffer = (char*) malloc (mp3->w_wave->GetBufferAllocSize());
	if(!mp3->c_gapless_buffer)
	{
		mp3->error("_Open->Can't allocate gapless buffer");
		CloseHandle(mp3->w_hEvent);
		mp3->init(mp3);
		return FALSE;

	}


	// change or not change external equalizer tables
	if(mp3->c_bUseExternalEQ && ( mp3->c_old_sampling_rate != mp3->c_sampling_rate))
	{
		equ_makeTable(&mp3->eq_inter_param, mp3->c_lbands, mp3->c_rbands, &mp3->c_paramroot, (REAL) mp3->c_sampling_rate);
		mp3->c_old_sampling_rate = mp3->c_sampling_rate;
		equ_clearbuf(&mp3->eq_inter_param, mp3->c_sampling_rate);	
	}
		
		


	int mode = mp3->c_echo_mode * 2;


	for(int i = 0; i < ECHO_MAX_DELAY_NUMBER ; i++)
	{
		mp3->c_sfx_mode_l[i] = sfx_mode[mode][i];
		if(mp3->c_channel > 1)
			mp3->c_sfx_mode_r[i] = sfx_mode[mode + 1][i];
		else
			mp3->c_sfx_mode_r[i] = mp3->c_sfx_mode_l[i];	
	}


	
	

	

    // latency of external equalizer
	mp3->c_byte_to_skip = 	equ_latency(&mp3->eq_inter_param) * 2 * mp3->c_channel;


	// set position to first frame
	mp3->c_position = mp3->c_start;
	mp3->c_current_position = 0;


    return TRUE;

	
}






int __stdcall WMp3::PlayLoop(int fFormatStartTime, MP3_TIME *pStartTime, int fFormatEndTime, MP3_TIME *pEndTime, unsigned int nNumOfRepeat)
{
	if(c_stream_managed)
	{
		error("PlayLoop->This is dynamic stream. Can't play in loop !");
		return 0;
	}

	if(c_fReverseMode)
	{
		error("PlayLoop->Stream is in reverse mode. Can't play loop !");
		return 0;
	}


	if(fFormatStartTime == TIME_FORMAT_BYTES || fFormatEndTime == TIME_FORMAT_BYTES)
	{
		error("PlayLoop->TIME_FORMAT_BYTES not supported !");
		return 0;
	}


	if(pStartTime == 0 || fFormatStartTime == 0)
	{
		error("PlayLoop->Invalid time format !");
		return 0;
	}

	if(pEndTime == 0 || fFormatEndTime == 0)
	{
		error("PlayLoop->Invalid time format !");
		return 0;
	}
/*
	if(pEndTime == 0)
	{
		error("PlayLoop->MP3_TIME* pEndTime can't be 0 !");
		return 0;

	}
*/

// prevent program crash if we call this function on empty stream	
	if(!c_start)
	{
		error("PlayLoop->Stream isn't open !");
		return 0;
	}

	unsigned int nLoopStartMilliSeconds = Mp3TimeToMs(fFormatStartTime, pStartTime);
	unsigned int nLoopStopMilliSeconds = Mp3TimeToMs(fFormatEndTime, pEndTime);

	if(nLoopStopMilliSeconds < nLoopStartMilliSeconds)
	{
		error("PlayLoop->Loop end time is smaller than loop start time !");
		return 0;
	}


	int fly = 0;
	if(c_pause || c_play) 
		fly = 1;



	unsigned int nLoopLengthMilliSeconds = nLoopStopMilliSeconds - nLoopStartMilliSeconds;		

//	if(pStartTime != 0)
	{
		if(!Seek(fFormatStartTime, pStartTime, SONG_BEGIN))
		{
			error("PlayLoop->Can't seek to start of loop !");
			return 0;
		}

		WaitForSingleObject(w_hThread, INFINITE);
	}


	flying_loop = fly;

	c_bLoop = 1; // play loop

	c_pause = FALSE; // reset pause indicator


// initialize VU meter	
	c_vudata_ready = FALSE;
	c_dwPos = 0;
	c_dwIndex = 0;

// main decoding thread switch
    c_play = TRUE; // enable decoding thread


	// calculate number of mp3 frames to fill one loop 
	c_nLoopFrameNum = MulDiv(nLoopLengthMilliSeconds, c_sampling_rate, c_sample_per_frame * 1000); // number of mp3 frames needed for one loop
// initialize to 0
	c_nLoopFramePlayed = 0;  // number of played mp3 frames in one loop
// enable loop processing
	c_bLoop = 1;
// loop repeating
	c_nLoopCount = nNumOfRepeat;
// loop repeated
	c_nLoop = 0;


// correct position when repeating loop
	if(nLoopLengthMilliSeconds > c_song_length_ms - nLoopStartMilliSeconds)
		c_dLoopPositionCorrector = (unsigned int) (	(double) (c_song_length_ms - nLoopStartMilliSeconds) / 1000.0 * (double) c_sampling_rate);
	else			
		c_dLoopPositionCorrector = (unsigned int) ((double) c_nLoopFrameNum * (double) c_sample_per_frame);




// create playing thread

	if(pStartTime != 0)
	{
		if(w_hThread)
			CloseHandle(w_hThread);

		w_hThread = 0;

		c_thread_no_error = 1;
		c_in_loop = 1;
		w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFunc, (void*) this,0,&w_hThreadID);
		if( w_hThread == NULL) {
    		error("WMp3::PlayLoopM->Can't create decoding thread!");
			return 0;
		}	
	}
	else
	{
		c_position = c_start + c_current_position;
	}

	
    return 1;


}





# define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
# define INFO_MAGIC	(('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')
# define LAME_MAGIC	(('L' << 24) | ('A' << 16) | ('M' << 8) | 'E')
# define VBRI_MAGIC	(('V' << 24) | ('B' << 16) | ('R' << 8) | 'I')

static
unsigned short const crc16_table[256] = {
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,

	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,

	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,

	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

unsigned short crc_compute(char const *data, unsigned int length,
						   unsigned short init)
{
	register unsigned int crc;

	for (crc = init; length >= 8; length -= 8) {
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	}

	switch (length)
	{
	  case 7: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 6: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 5: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 4: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 3: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 2: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 1: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 0: break;
	}

	return (unsigned short) crc;
}

/*
* NAME:	tag->init()
* DESCRIPTION:	initialize tag structure
*/

void tag_init(struct tag_xl *tag)
{
	tag->flags      = 0;
	tag->encoder[0] = 0;
	memset(&tag->xing, 0, sizeof(struct tag_xing));
	memset(&tag->lame, 0, sizeof(struct tag_lame));
}


/*
* NAME:	parse_xing()
* DESCRIPTION:	parse a Xing VBR tag
*/


static
int parse_xing(struct tag_xing *xing,
struct mad_bitptr *ptr, unsigned int *bitlen)
{
	if (*bitlen < 32)
		goto fail;

    xing->flags = mad_bit_read(ptr, 32);
	*bitlen -= 32;

	if (xing->flags & TAG_XING_FRAMES) {
		if (*bitlen < 32)
			goto fail;

		xing->frames = mad_bit_read(ptr, 32);
		*bitlen -= 32;
	}

	if (xing->flags & TAG_XING_BYTES) {
		if (*bitlen < 32)
			goto fail;

		xing->bytes = mad_bit_read(ptr, 32);
		*bitlen -= 32;
	}

	if (xing->flags & TAG_XING_TOC) {
		int i;

		if (*bitlen < 800)
			goto fail;

		for (i = 0; i < 100; ++i)
			xing->toc[i] = (unsigned char) mad_bit_read(ptr, 8);

		*bitlen -= 800;
	}

	if (xing->flags & TAG_XING_SCALE) {
		if (*bitlen < 32)
			goto fail;

		xing->scale = mad_bit_read(ptr, 32);
		*bitlen -= 32;
	}

	return 0;

fail:
	xing->flags = 0;
	return -1;
}



/*
* NAME:	parse_lame()
* DESCRIPTION:	parse a LAME tag
*/


static
int parse_lame(struct tag_lame *lame,
struct mad_bitptr *ptr, unsigned int *bitlen,
	unsigned short crc)
{
	struct mad_bitptr save = *ptr;
	unsigned long magic;
	char const *version;

	if (*bitlen < 36 * 8)
		goto fail;

	/* bytes $9A-$A4: Encoder short VersionString */

	magic   = mad_bit_read(ptr, 4 * 8);
	version = (const char *) mad_bit_nextbyte(ptr);

	mad_bit_skip(ptr, 5 * 8);

	/* byte $A5: Info Tag revision + VBR method */

	lame->revision = (unsigned char) mad_bit_read(ptr, 4);
	if (lame->revision == 15)
		goto fail;

	lame->vbr_method = (enum tag_lame_vbr) mad_bit_read(ptr, 4);

	/* byte $A6: Lowpass filter value (Hz) */

	lame->lowpass_filter = (unsigned short) ( mad_bit_read(ptr, 8) * 100);

	/* bytes $A7-$AA: 32 bit "Peak signal amplitude" */

	lame->peak = mad_bit_read(ptr, 32) << 5;

	/* bytes $AB-$AC: 16 bit "Radio Replay Gain" */

	rgain_parse(&lame->replay_gain[0], ptr);

	/* bytes $AD-$AE: 16 bit "Audiophile Replay Gain" */

	rgain_parse(&lame->replay_gain[1], ptr);

	//LAME 3.95.1 reference level changed from 83dB to 89dB and foobar seems to use 89dB
	if (magic == LAME_MAGIC) {
		char str[6];
		unsigned major = 0, minor = 0, patch = 0;
		int i;

		memcpy(str, version, 5);
		str[5] = 0;

		sscanf(str, "%u.%u.%u", &major, &minor, &patch);

		if (major == 3 && minor < 95) {
			for (i = 0; i < 2; ++i) {
				if (RGAIN_SET(&lame->replay_gain[i]))
					lame->replay_gain[i].adjustment += 6;  // 6.0 dB 
			}
		}
	}

	/* byte $AF: Encoding flags + ATH Type */

	lame->flags    = (unsigned char) mad_bit_read(ptr, 4);
	lame->ath_type = (unsigned char) mad_bit_read(ptr, 4);

	/* byte $B0: if ABR {specified bitrate} else {minimal bitrate} */

	lame->bitrate = (unsigned char) mad_bit_read(ptr, 8);

	/* bytes $B1-$B3: Encoder delays */

	lame->start_delay = mad_bit_read(ptr, 12);
	lame->end_padding = mad_bit_read(ptr, 12);

	/* byte $B4: Misc */

	lame->source_samplerate = (enum tag_lame_source) mad_bit_read(ptr, 2);

	if (mad_bit_read(ptr, 1))
		lame->flags |= TAG_LAME_UNWISE;

	lame->stereo_mode   = (enum tag_lame_mode) mad_bit_read(ptr, 3);
	lame->noise_shaping = (unsigned char) mad_bit_read(ptr, 2);

	/* byte $B5: MP3 Gain */

	lame->gain = (signed char) mad_bit_read(ptr, 8);

	/* bytes $B6-B7: Preset and surround info */

	mad_bit_skip(ptr, 2);

	lame->surround = (enum tag_lame_surround) mad_bit_read(ptr,  3);
	lame->preset   = (enum tag_lame_preset) mad_bit_read(ptr, 11);

	/* bytes $B8-$BB: MusicLength */

	lame->music_length = mad_bit_read(ptr, 32);

	/* bytes $BC-$BD: MusicCRC */

	lame->music_crc = (unsigned short) mad_bit_read(ptr, 16);

	/* bytes $BE-$BF: CRC-16 of Info Tag */

	if (mad_bit_read(ptr, 16) != crc)
		goto fail;

	*bitlen -= 36 * 8;

	return 0;

fail:
	*ptr = save;
	return -1;
}

/*
* NAME:	tag->parse()
* DESCRIPTION:	parse Xing/LAME tag(s)
*/
int tag_parse(struct tag_xl *tag, struct mad_stream const *stream, struct mad_frame const *frame)
{
	struct mad_bitptr ptr = stream->anc_ptr;
	struct mad_bitptr start = ptr;
	unsigned int bitlen = stream->anc_bitlen;
	unsigned long magic;
	unsigned long magic2;
	int i;

	tag_init(tag); //tag->flags = 0;

	if (bitlen < 32)
		return -1;

	magic = mad_bit_read(&ptr, 32);
    
	bitlen -= 32;

	if (magic != XING_MAGIC &&
		magic != INFO_MAGIC &&
		magic != LAME_MAGIC) {
			/*
			* Due to an unfortunate historical accident, a Xing VBR tag may be
			* misplaced in a stream with CRC protection. We check for this by
			* assuming the tag began two octets prior and the high bits of the
			* following flags field are always zero.
			*/

			if (magic != ((XING_MAGIC << 16) & 0xffffffffL) &&
				magic != ((INFO_MAGIC << 16) & 0xffffffffL))
			{
				//check for VBRI tag
				if (bitlen >= 400)
				{
					mad_bit_skip(&ptr, 256);
					magic2 = mad_bit_read(&ptr, 32);
					if (magic2 == VBRI_MAGIC)
					{
						mad_bit_skip(&ptr, 16); //16 bits - version
						tag->lame.start_delay = mad_bit_read(&ptr, 16); //16 bits - delay
						mad_bit_skip(&ptr, 16); //16 bits - quality
						tag->xing.bytes = mad_bit_read(&ptr, 32); //32 bits - bytes
						tag->xing.frames = mad_bit_read(&ptr, 32); //32 bits - frames
						unsigned int table_size = mad_bit_read(&ptr, 16);
						unsigned int table_scale = mad_bit_read(&ptr, 16);
						unsigned int entry_bytes = mad_bit_read(&ptr, 16);
						unsigned int entry_frames = mad_bit_read(&ptr, 16);
						{
						    unsigned int Entry = 0, PrevEntry = 0, Percent, SeekPoint = 0, i = 0;
						    float AccumulatedTime = 0;
                            float TotalDuration = (float) (1000.0 * tag->xing.frames * ((frame->header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152) / frame->header.samplerate);
						    float DurationPerVbriFrames = TotalDuration / ((float)table_size + 1);
						    for (Percent=0;Percent<100;Percent++)
						    {
                                float EntryTimeInMilliSeconds = ((float)Percent/100) * TotalDuration;
                                while (AccumulatedTime <= EntryTimeInMilliSeconds)
                                {
                                    PrevEntry = Entry;
                                    Entry = mad_bit_read(&ptr, entry_bytes * 8) * table_scale;
                                    i++;
                                    SeekPoint += Entry;
                                    AccumulatedTime += DurationPerVbriFrames;
                                    if (i >= table_size) break;
                                }
                                unsigned int fraction = ( (int)(((( AccumulatedTime - EntryTimeInMilliSeconds ) / DurationPerVbriFrames ) 
			                                             + (1.0f/(2.0f*(float)entry_frames))) * (float)entry_frames));
			                    unsigned int SeekPoint2 = SeekPoint - (int)((float)PrevEntry * (float)(fraction) 
				                                         / (float)entry_frames);
				                unsigned int XingPoint = (256 * SeekPoint2) / tag->xing.bytes;
							

				                if (XingPoint > 255) XingPoint = 255;
				                tag->xing.toc[Percent] = (unsigned char)(XingPoint & 0xFF);
                            }
                        }
						tag->flags |= (TAG_XING | TAG_VBRI);
						tag->xing.flags = (TAG_XING_FRAMES | TAG_XING_BYTES | TAG_XING_TOC);
					
						return 0;
					}
				}
				return -1;
			}

			magic >>= 16;

			/* backtrack the bit pointer */

			ptr = start;
			mad_bit_skip(&ptr, 16);
			bitlen += 16;
	}

	if ((magic & 0x0000ffffL) == (XING_MAGIC & 0x0000ffffL))
		tag->flags |= TAG_VBR;

	/* Xing tag */

	if (magic == LAME_MAGIC) {
		ptr = start;
		bitlen += 32;
	}
	else if (parse_xing(&tag->xing, &ptr, &bitlen) == 0)
		tag->flags |= TAG_XING;

	/* encoder string */

	if (bitlen >= 20 * 8) {
		start = ptr;

		for (i = 0; i < 20; ++i) {
			tag->encoder[i] = (char)  mad_bit_read(&ptr, 8);

			if (tag->encoder[i] == 0)
				break;

			/* keep only printable ASCII chars */

			if (tag->encoder[i] < 0x20 || tag->encoder[i] >= 0x7f) {
				tag->encoder[i] = 0;
				break;
			}
		}

		tag->encoder[20] = 0;
		ptr = start;
	}

	/* LAME tag */

	if (memcmp(tag->encoder, "LAME", 4) == 0 && stream->next_frame - stream->this_frame >= 192) {
            unsigned short crc = crc_compute((const char *) stream->this_frame, (frame->header.flags & MAD_FLAG_LSF_EXT) ? 175 : 190, 0x0000);
			if (parse_lame(&tag->lame, &ptr, &bitlen, crc) == 0)
			{
                tag->flags |= TAG_LAME;
			    tag->encoder[9] = 0;
            }
	}
	else {
		for (i = 0; i < 20; ++i) {
			if (tag->encoder[i] == 0)
				break;

			/* stop at padding chars */

			if (tag->encoder[i] == 0x55) {
				tag->encoder[i] = 0;
				break;
			}
		}
	}

	return 0;
}



/*
 * NAME:	rgain->parse()
 * DESCRIPTION:	parse a 16-bit Replay Gain field
 */
void rgain_parse(struct rgain *rgain, struct mad_bitptr *ptr)
{
  int negative;

  rgain->name       = (enum rgain_name) mad_bit_read(ptr, 3);
  rgain->originator = (enum rgain_originator) mad_bit_read(ptr, 3);

  negative          = mad_bit_read(ptr, 1);
  rgain->adjustment = (short) mad_bit_read(ptr, 9);

  if (negative)
    rgain->adjustment = (short) -rgain->adjustment;
}





void WMp3::cf_clear(WMp3* mp3)
{

	mp3->c_stream_start = 0; // first byte of input stream ( including ID3 tag )
	mp3->c_stream_size = 0; // size of input mp3 stream ( whole stream with ID3 tags )
	mp3->c_stream_end = 0;  // last byte of input stream ( including ID3 tag )

	mp3->c_start = 0; // first valid frame
	mp3->c_end = 0;   // last byte of stream without ID3 tag
	mp3->c_position = 0;// current position in stream   
	mp3->c_size = 0;    // size of stream (without ID3 tags )

	mp3->c_queue.Clear();		// queue for stream playing
	mp3->c_buffered_queue.Clear();	// queue for buffered stream playing

	mp3->c_buffered_stream = 0;	// specify buffered stream
	mp3->c_endstream = 0;		// specify if playing needs to stop if stream runs out of data
	mp3->c_stream_managed = 0;	// specifies managed stream ( can receive new data )

	mp3->c_ID3v1 = 0;	// ID3v1 tag present
	mp3->c_ID3v2 = 0;	// ID3v2 tag present


	mp3->c_old_sampling_rate = 0;	// old sampling rate, need this for equ_makeTables
	mp3->c_sampling_rate = 0;		// sampling rate frequency  ( Hz )
	mp3->c_layer = 0;				// layer  1, 2, 3
	mp3->c_mode = 0;				// MAD_MODE_SINGLE_CHANNEL,MAD_MODE_DUAL_CHANNEL,MAD_MODE_JOINT_STEREO,MAD_MODE_STEREO
	mp3->c_channel = 0;				// number of channels
	mp3->c_emphasis = 0;			// emphasis
	mp3->c_mode_extension = 0;		// mode extension
	mp3->c_bitrate = 0;				// bitrate of first frame
	mp3->c_avg_bitrate = 0;			// average bitrate
	mp3->c_current_bitrate = 0;		// bitrate of decoding frame
	mp3-> c_frame_counter = 0;		// number of decoded frames
	mp3->c_bitrate_sum = 0;			// sum of all decoded bitrates
	mp3->c_nPosition = 0;			// seek position in milliseconds

	mp3->c_avg_frame_size = 0;		// average frame size
	mp3->c_sample_per_frame = 0;	// number of wave samples in one frame
	mp3->c_flags = 0;
	mp3->c_mpeg_version = 0;		// MPEG_1, MPEG_2, MPEG_25
	mp3->c_mpeg_layer_version = 0;	// LAYER_1, LAYER_2 , LAYER_3
	mp3->c_channel_mode = 0;		// MODE_SINGLE_CHANNEL, MODE_DUAL_CHANNEL, MODE_JOINT_STEREO, MODE_STEREO
	mp3->c_emphasis_mode = 0;

	mp3->c_xing_frame_size = 0;
	memset(&mp3->xing, 0, sizeof(struct tag_xl));
	mp3->c_valid_xing = 0;

	mp3->c_song_length_samples = 0; // length of song in samples
	mp3->c_song_length_ms = 0;		// length of song in milliseconds
	mp3->c_song_length_bytes = 0;	// length of song in bytes
	mp3->c_song_length_frames = 0;	// length of song in mp3 frames
	mp3->c_input_buffer_size = 0;	// size of input buffer ( frame number )

	mp3->c_tmp = 0;				// fix MAD_BUFFER_GUARD ( allocated and free in thread ) 
	mp3->c_tmp_ds = 0;			// buffered stream runtime buffer ( allocated and free in thread ) 	

	mp3->c_play = 0;			// playing flag
	mp3->c_vbr = 0;				// file is vbr or cbr
	mp3->c_pause = 0;			// pause flag, control Pause() and Reset() 


	mp3->c_dwPos = 0; // position in current playing wave buffer, need to get corect VU data
	mp3->c_dwIndex = 0; // index of current playing wave buffer
	mp3->c_vudata_ready = 0; // get VU data only when playing

		// loop playing
	mp3->c_bLoop = 0;		// indicate loop playing
	mp3->c_nLoopCount = 0;	// repeat loop n times
	mp3->c_nLoop = 0;			// number of played loops
	mp3->c_nLoopFrameNum = 0; // number of mp3 frames needed for one loop
	mp3->c_nLoopFramePlayed = 0; // number of played mp3 frames in one loop
	mp3->c_bLoopIgnoreFrame = 0;	// ignore all frames in this input buffer
	mp3->c_dLoopPositionCorrector = 0; // korektor pozicije u pjesmi kad se ponavlja loop


		// fade out, fade in
	mp3->c_bFadeOut = 0;
	mp3->c_bFadeIn = 0;
	mp3->c_nFadeOutFrameNum = 0;
	mp3->c_nFadeInFrameNum = 0;
	mp3->c_fadeOutStep = 0;
	mp3->c_fadeInStep = 0;
	mp3->c_fade_frame_count = 0;



	// ovo ide direktno u konstruktor jer vrijedi za sve pjesme
	mp3->c_bVocalCut = 0;				// enable/disable vocal cut processing
	mp3->c_echo = 0; 
	mp3->c_rIgOg = 1.0;
	mp3->c_rEgOg = 1.0;
	mp3->c_delay = 0;
	mp3->c_old_echo_buffer_size = 0;	// use to check if we need to allocate new echo buffers
	mp3->c_echo_alloc_buffer_size = 0;	// size of allocated echo buffer
	mp3->c_echo_mode = 0;				// current echo_mode
	mp3->c_sfx_offset = 0;				// offset ( number of echo buffers to skip into past )
	unsigned int i;
	for(i = 0; i < ECHO_BUFFER_NUMBER; i++)
		mp3->sfx_buffer[i] = 0;

	for(i = 0; i < ECHO_BUFFER_NUMBER; i++)
		mp3->sfx_buffer1[i] = 0;

	mp3->c_nInputGain = 0;
	mp3->c_nEchoGain = 0;
	mp3->c_nOutputGain = 0;
	mp3->c_nDelayMs = 0;
	



	mp3->c_stream_managed = 0;
	mp3->c_skip_xing = 0;


	mp3->c_ID3v1 = 0;	// ID3v1 tag present
	mp3->c_ID3v2 = 0;	// ID3v2 tag present


	mp3->c_sampling_rate = 0;     // sampling rate frequency  ( Hz )
	mp3->c_layer = 0;				// layer  1, 2, 3
	mp3->c_mode = 0;				// MAD_MODE_SINGLE_CHANNEL,MAD_MODE_DUAL_CHANNEL,MAD_MODE_JOINT_STEREO,MAD_MODE_STEREO
	mp3->c_channel = 0;			// number of channels
	mp3->c_emphasis = 0;
	mp3->c_mode_extension = 0;
	mp3->c_bitrate = 0;	// bitrate of first frame
	mp3->c_avg_bitrate = 0;	// average bitrate

	mp3->c_avg_frame_size = 0;	// average frame size

//	mp3->c_duration = mad_timer_zero;	// audio playing time of frame
	mp3->c_sample_per_frame = 0;	// number of wave samples in one frame
	mp3->c_flags = 0;
	mp3->c_mpeg_version = 0;	
	mp3->c_mpeg_layer_version =0; // 
	mp3->c_channel_mode = 0;
	mp3->c_emphasis_mode =0;



	memset(&mp3->xing,0,sizeof(mp3->xing));
	mp3->c_valid_xing = 0;
	mp3->c_xing_frame_size = 0;

	mp3->c_song_length_samples = 0; // length of song in samples
	mp3->c_song_length_ms = 0;		// length of song in milliseconds
	mp3->c_song_length_bytes = 0;	// length of song in bytes
	mp3->c_song_length_frames = 0;	// length of song in mp3 frames
		
		
	mp3->c_input_buffer_size = 0;	// size of input buffer ( frame number )
	
	mp3->c_tmp = 0;	
	mp3->c_play = 0;
	mp3->c_vbr = 0;		
	mp3->c_pause = 0;	// pause flag, control Pause() and Reset() 


			// VU meter
	mp3->c_dwPos = 0; // position in current playing wave buffer, need to get corect VU data
	mp3->c_dwIndex = 0; // index of current playing wave buffer
	mp3->c_vudata_ready = 0; // get VU data only when playing
		

	mp3->c_frame_counter = 0;	// number of decoded frames
	mp3->c_bitrate_sum = 0;		// sum of all decoded bitrates

	mp3->c_bLoop = 0;		// indicate loop playing
	mp3->c_nLoopCount = 0;	// repeat loop n times
	mp3->c_nLoop = 0;;			// number of played loops
		
	mp3->c_nPosition = 0;

	mp3->c_bFadeOut = FALSE;
	mp3->c_nFadeOutFrameNum = 0;

	mp3->c_bFadeIn = 0;

	mp3->c_nFreeBitrate = 0;






	if(mp3->c_gapless_buffer)
		free(mp3->c_gapless_buffer);
	

	mp3->c_gapless_buffer = 0;


	memset(&mp3->left_dither,0 ,sizeof(mp3->left_dither));
	memset(&mp3->right_dither,0, sizeof(mp3->right_dither));


}




void WMp3::init(WMp3* mp3)
{

	mp3->c_stream_start = 0; // first byte of input stream ( including ID3 tag )
	mp3->c_stream_size = 0; // size of input mp3 stream ( whole stream with ID3 tags )
	mp3->c_stream_end = 0;  // last byte of input stream ( including ID3 tag )

	mp3->c_start = 0; // first valid frame
	mp3->c_end = 0;   // last byte of stream without ID3 tag
	mp3->c_position = 0;// current position in stream   
	mp3->c_size = 0;    // size of stream (without ID3 tags )



	mp3->c_stream_managed = 0;
	mp3->c_skip_xing = 0;


	mp3->c_ID3v1 = 0;	// ID3v1 tag present
	mp3->c_ID3v2 = 0;	// ID3v2 tag present


	mp3->c_sampling_rate = 0;     // sampling rate frequency  ( Hz )
	mp3->c_layer = 0;				// layer  1, 2, 3
	mp3->c_mode = 0;				// MAD_MODE_SINGLE_CHANNEL,MAD_MODE_DUAL_CHANNEL,MAD_MODE_JOINT_STEREO,MAD_MODE_STEREO
	mp3->c_channel = 0;			// number of channels
	mp3->c_emphasis = 0;
	mp3->c_mode_extension = 0;
	mp3->c_bitrate = 0;	// bitrate of first frame
	mp3->c_avg_bitrate = 0;	// average bitrate

	mp3->c_avg_frame_size = 0;	// average frame size

//	mp3->c_duration = mad_timer_zero;	// audio playing time of frame
	mp3->c_sample_per_frame = 0;	// number of wave samples in one frame
	mp3->c_flags = 0;
	mp3->c_mpeg_version = 0;	
	mp3->c_mpeg_layer_version =0; // 
	mp3->c_channel_mode = 0;
	mp3->c_emphasis_mode =0;



	memset(&mp3->xing,0,sizeof(mp3->xing));
	mp3->c_valid_xing = 0;
	mp3->c_xing_frame_size = 0;

	mp3->c_song_length_samples = 0; // length of song in samples
	mp3->c_song_length_ms = 0;		// length of song in milliseconds
	mp3->c_song_length_bytes = 0;	// length of song in bytes
	mp3->c_song_length_frames = 0;	// length of song in mp3 frames
		
		
	mp3->c_input_buffer_size = 0;	// size of input buffer ( frame number )
	
	mp3->c_tmp = 0;	
	mp3->c_play = 0;
	mp3->c_vbr = 0;		
	mp3->c_pause = 0;	// pause flag, control Pause() and Reset() 


			// VU meter
	mp3->c_dwPos = 0; // position in current playing wave buffer, need to get corect VU data
	mp3->c_dwIndex = 0; // index of current playing wave buffer
	mp3->c_vudata_ready = 0; // get VU data only when playing
		

	mp3->c_frame_counter = 0;	// number of decoded frames
	mp3->c_bitrate_sum = 0;		// sum of all decoded bitrates

	mp3->c_bLoop = 0;		// indicate loop playing
	mp3->c_nLoopCount = 0;	// repeat loop n times
	mp3->c_nLoop = 0;;			// number of played loops
		
	mp3->c_nPosition = 0;

	mp3->c_bFadeOut = FALSE;
	mp3->c_nFadeOutFrameNum = 0;

	mp3->c_bFadeIn = 0;

	mp3->c_nFreeBitrate = 0;






	if(mp3->c_gapless_buffer)
		free(mp3->c_gapless_buffer);
	

	mp3->c_gapless_buffer = 0;


	memset(&mp3->left_dither,0 ,sizeof(mp3->left_dither));
	memset(&mp3->right_dither,0, sizeof(mp3->right_dither));


}



void mono_filter(struct mad_frame *frame)
{
	// equalize left and right channel
	// we can change this filter at playing time
	if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL) {
		unsigned int ns, s, sb;
		mad_fixed_t left, right, mono;

		ns = MAD_NSBSAMPLES(&frame->header);

		for (s = 0; s < ns; ++s) {
			for (sb = 0; sb < 32; ++sb) {
				left  = frame->sbsample[0][s][sb];
				right = frame->sbsample[1][s][sb];
				mono = (left + right) / 2;
				frame->sbsample[0][s][sb] = mono;
				frame->sbsample[1][s][sb] = mono;

			}
		}
	}
}



void gain_filter(struct mad_frame *frame, mad_fixed_t gain[2])
{

	unsigned int nch, ch, ns, s, sb;

	nch = MAD_NCHANNELS(&frame->header);
	ns  = MAD_NSBSAMPLES(&frame->header);

	for (ch = 0; ch < nch; ++ch) {
		for (s = 0; s < ns; ++s) {
			for (sb = 0; sb < 32; ++sb) {
				frame->sbsample[ch][s][sb] =
				mad_f_mul(frame->sbsample[ch][s][sb], gain[ch]);
			}
		}
	}
	
}



void experimental_filter(struct mad_frame *frame)
{
	if (frame->header.mode == MAD_MODE_STEREO ||
		frame->header.mode == MAD_MODE_JOINT_STEREO) {
		
		unsigned int ns, s, sb;

		ns = MAD_NSBSAMPLES(&frame->header);

		/* enhance stereo separation */

		for (s = 0; s < ns; ++s) {
			for (sb = 0; sb < 32; ++sb) {
				mad_fixed_t left, right;

				left  = frame->sbsample[0][s][sb];
				right = frame->sbsample[1][s][sb];

				frame->sbsample[0][s][sb] -= right / 4;
				frame->sbsample[1][s][sb] -= left  / 4;
			}
		}
	}
}





int __stdcall WMp3::MixChannels(int fEnable, unsigned int nLeftPercent, unsigned int nRightPercent)
{
	c_mix_channels = fEnable;

	c_l_mix = mad_f_tofixed( (double) nLeftPercent / 100.0); 
	c_r_mix = mad_f_tofixed( (double) nRightPercent / 100.0);

	return 1;
	

}

void vocal_cut_filter(struct mad_frame *frame)
{
	// equalize left and right channel
	// we can change this filter at playing time
	if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL) {
		unsigned int ns, s, sb;
		mad_fixed_t left, right;

		ns = MAD_NSBSAMPLES(&frame->header);
	
		for (s = 0; s < ns; ++s) {
			for (sb = 0; sb < 32; ++sb) {
				left  = frame->sbsample[0][s][sb] - frame->sbsample[1][s][sb];
				right = frame->sbsample[1][s][sb] - frame->sbsample[0][s][sb];
				
				frame->sbsample[0][s][sb] = left;
				frame->sbsample[1][s][sb] = right;

			}
		}
	}
}


void mix_filter(struct mad_frame *frame, mad_fixed_t left_p, mad_fixed_t right_p)
{
	// equalize left and right channel
	// we can change this filter at playing time
	if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL) {
		unsigned int ns, s, sb;
		mad_fixed_t left, right, mono;

		ns = MAD_NSBSAMPLES(&frame->header);

		for (s = 0; s < ns; ++s) {
			for (sb = 0; sb < 32; ++sb) {
				left  = frame->sbsample[0][s][sb];
				right = frame->sbsample[1][s][sb];
				mono = mad_f_mul(left, left_p);
				mono = mad_f_add(mono, mad_f_mul(right, right_p)); 

				frame->sbsample[0][s][sb] = mono;
				frame->sbsample[1][s][sb] = mono;

			}
		}
	}
}


int __stdcall WMp3::FadeVolume(int fMode, int fFormat, MP3_TIME *pTime)
{
	if(pTime == 0)
	{
		error("FadeVolume->MP3_TIME *pTime can't be 0");
		return 0;
	}

	switch(fMode)
	{
		case FADE_IN:
		{
			EnterCriticalSection(&c_wcs_fade);
			unsigned int nFadeIntervalMilliSeconds = Mp3TimeToMs(fFormat, pTime);
		

			c_bFadeOut = FALSE;
			

			

			
			// calculate number of mp3 frames to fill one loop 
			c_nFadeInFrameNum = MulDiv(nFadeIntervalMilliSeconds, c_sampling_rate, c_sample_per_frame * 1000); 

			if(c_nFadeInFrameNum == 0)
			{
				_fade_volume = 100.0;

				c_dLeftGainFade = 1.0;
				c_dRightGainFade = 1.0;
				c_dLeftGain = c_dLeftGainVolume * c_dLeftGainFade;
				c_dRightGain = c_dRightGainVolume * c_dRightGainFade;

				LeaveCriticalSection(&c_wcs_fade);
				return 1;

			}

			c_bFadeOut = 0;
			c_bFadeIn = 1;

			c_fadeInStep = ( 100.0 - (double) _fade_volume) / (double) c_nFadeInFrameNum;
			if(c_fadeInStep < 0)
				c_fadeInStep = 0;


			c_fade_frame_count = 0;

			LeaveCriticalSection(&c_wcs_fade);

		}
		break;

		case FADE_OUT:
		{
			EnterCriticalSection(&c_wcs_fade);

			unsigned int nFadeIntervalMilliSeconds = Mp3TimeToMs(fFormat, pTime);
			c_bFadeIn = 0;
			
			

		
			
			// calculate number of mp3 frames to fill one loop 
			c_nFadeOutFrameNum = MulDiv(nFadeIntervalMilliSeconds, c_sampling_rate, c_sample_per_frame * 1000); 

			if(c_nFadeOutFrameNum == 0)
			{
				_fade_volume = 0;
				c_dLeftGainFade = 0;
				c_dRightGainFade = 0;
				c_dLeftGain = c_dLeftGainVolume * c_dLeftGainFade;
				c_dRightGain = c_dRightGainVolume * c_dRightGainFade;

				LeaveCriticalSection(&c_wcs_fade);
				return 1;
					
			}



			c_bFadeIn = 0;
			c_bFadeOut = TRUE;


			c_fadeOutStep = (double) _fade_volume / (double) c_nFadeOutFrameNum;
			c_fade_frame_count = 0;

			LeaveCriticalSection(&c_wcs_fade);

		}
		break;

		default:
			error("FadeVolume->Invalid fMode !");
		return 0;




	}

	

	return 1;

}




int __stdcall WMp3::FadeVolumeEx(int fMode, int fFormat, MP3_TIME *pTime)
{
	if(pTime == 0)
	{
		error("FadeVolume->MP3_TIME *pTime can't be 0");
		return 0;
	}

	switch(fMode)
	{
		case FADE_IN:
		{
			unsigned int nFadeIntervalMilliSeconds = Mp3TimeToMs(fFormat, pTime);
			c_bFadeOut = FALSE;

			EnterCriticalSection(&c_wcs_fade);

			
			
			// calculate number of mp3 frames to fill one loop 
			c_nFadeInFrameNum = MulDiv(nFadeIntervalMilliSeconds, c_sampling_rate, c_sample_per_frame * 1000); 

			if(c_nFadeInFrameNum == 0)
			{
				_fade_volume = 100.0;

				c_dLeftGainFade = 1.0;
				c_dRightGainFade = 1.0;
				c_dLeftGain = c_dLeftGainVolume * c_dLeftGainFade;
				c_dRightGain = c_dRightGainVolume * c_dRightGainFade;

				LeaveCriticalSection(&c_wcs_fade);
				return 1;

			}

			c_bFadeOut = 0;
			c_bFadeIn = 1;


			c_fadeInStep = ( 100.0 ) / (double) c_nFadeInFrameNum;
			if(c_fadeInStep < 0)
				c_fadeInStep = 0;

			_fade_volume = 0;
			c_fade_frame_count = 0;


			LeaveCriticalSection(&c_wcs_fade);

		}
		break;

		case FADE_OUT:
		{
			unsigned int nFadeIntervalMilliSeconds = Mp3TimeToMs(fFormat, pTime);
			c_bFadeIn = 0;
			EnterCriticalSection(&c_wcs_fade);
			

			
			
			// calculate number of mp3 frames to fill one loop 
			c_nFadeOutFrameNum = MulDiv(nFadeIntervalMilliSeconds, c_sampling_rate, c_sample_per_frame * 1000); 

			if(c_nFadeOutFrameNum == 0)
			{
				_fade_volume = 0;
				c_dLeftGainFade = 0;
				c_dRightGainFade = 0;
				c_dLeftGain = c_dLeftGainVolume * c_dLeftGainFade;
				c_dRightGain = c_dRightGainVolume * c_dRightGainFade;

				LeaveCriticalSection(&c_wcs_fade);
				return 1;
					
			}

			c_bFadeIn = 0;
			c_bFadeOut = TRUE;

			c_fadeOutStep = 100.0 / (double) c_nFadeOutFrameNum;
			c_fade_frame_count = 0;

			_fade_volume = 100.0;



			LeaveCriticalSection(&c_wcs_fade);

		}
		break;

		default:
			error("FadeVolume->Invalid fMode !");
		return 0;




	}

	

	return 1;

}



int WMp3::reallocate_sfx_buffers(WMp3* mp3)
{

	int i;
// if buffer size changed allocate new buffer
	if(mp3->c_old_echo_buffer_size != mp3->c_echo_alloc_buffer_size)
	{

		// free old buffers
		for(i = 0; i < ECHO_BUFFER_NUMBER; i++) {
			if(mp3->sfx_buffer[i]) {
				free(mp3->sfx_buffer[i]);
				mp3->sfx_buffer[i] = 0;
			}
		}
						
	
				// all buffers free

		// allocate new buffers
		for(i = 0; i < ECHO_BUFFER_NUMBER; i++) {
			mp3->sfx_buffer[i] = (short*) malloc(mp3->c_echo_alloc_buffer_size);
			mp3->sfx_buffer1[i] = mp3->sfx_buffer[i];
			// check allocation
			if(mp3->sfx_buffer[i] == NULL) {
			// allocation error, free allocated buffers
				for(int j = 0; j <= i; j++) {
					free(mp3->sfx_buffer[j]);
					mp3->sfx_buffer[j] = 0;
					mp3->sfx_buffer1[j] = 0;
				}
				mp3->sfx_buffer1[0] = 0;
				mp3->error("Can't allocate buffers for SFX processor!");
				
				return 0;		
			}
		}

		mp3->c_old_echo_buffer_size = mp3->c_echo_alloc_buffer_size;

		// all buffers allocated
	} // no need for new buffers, old buffers are OK
		
	return 1;
}


int __stdcall WMp3::GetPosition(MP3_TIME *pTime)
{	
	if(pTime == 0)
	{
		error("GetPosition->MP3_TIME *pTime - this parameter can't be NULL");
		return 0;
	}

	memset(pTime, 0, sizeof(MP3_TIME));


	unsigned int pos = 0;


// return current playing position i seconds
	if(!c_start)
	{
		return 1;
	}
	else
	{


		// seek position + wave position
		if(c_bLoop && c_nLoop)
		{
			if(!c_play)
				pos = 0;
			else
			{
				if(callback_messages & MP3_MSG_WAVE_BUFFER)
					pos = c_nInterPosition;
				else
					pos = w_wave->GetPosition(MILLISECONDS);
			}

			if(pos >= ( c_dLoopPositionCorrector * c_nLoop))
				pos -= ( c_dLoopPositionCorrector * c_nLoop);
			else
				pos -= ( c_dLoopPositionCorrector * ( c_nLoop - 1));

			pos = ( c_nPosition + (unsigned int) ( (double) pos * 1000.0 / (double) c_sampling_rate) );
		}
		else
		{	
			if(!c_play)
				pos = 0;
			else
			{
				if(callback_messages & MP3_MSG_WAVE_BUFFER)
					pos = c_nInterPosition;
				else
					pos = w_wave->GetPosition(MILLISECONDS);
			}

			if(c_fReverseMode)
			{
				if(pos > c_nPosition)
					pos = 0;
				else
					pos = c_nPosition - pos;
			}
			else
				pos += c_nPosition;
		}
	}


	pTime->frames = ( unsigned int ) ((double)pos / (double) c_song_length_ms * (double) c_song_length_frames);
	pTime->ms = pos;
	pTime->sec = pos / 1000;
	pTime->bytes = c_current_position;
	pTime->hms_hour = pos / 3600000;
	pos = pos % 3600000;
	pTime->hms_minute = pos / 60000;
	pos = pos % 60000;
	pTime->hms_second = pos / 1000;
	pTime->hms_millisecond = pos % 1000;
	
	return 1;
}


int __stdcall WMp3::GetSongLength(MP3_TIME *pLength)
{	
	if(pLength == 0)
	{
		error("GetSongLengthHMS->MP3_TIME *pLength - this parameter can't be NULL");
		return 0;
	}



	pLength->bytes = c_size;	
	pLength->ms = c_song_length_ms;
	pLength->sec = c_song_length_ms / 1000;
	pLength->frames = c_song_length_frames;

	unsigned int length = c_song_length_ms;
	pLength->hms_hour = length / 3600000;
	length = length % 3600000;
	pLength->hms_minute= length / 60000;
	length = length % 60000;
	pLength->hms_second = length / 1000;
	pLength->hms_millisecond = length % 1000;
	
	return 1;
}





void WMp3::_GetInternalVolume(WMp3 *mp3, unsigned int *lvolume, unsigned int *rvolume)
{
	*lvolume = mp3->c_lvolume;
	*rvolume = mp3->c_rvolume;
}


char* __stdcall WMp3::GetError()
{ 
	return _lpErrorMessage;
}




int __stdcall WMp3::CreateEqBands(int *pnFreqPoint, int nNumOfPoints)
{
	Stop(); // stop playing

	if(pnFreqPoint == 0 || nNumOfPoints < 2)
	{
		error("CreateEqBands->Invalid parameters !");
		return 0;
	}

	

	if(pnFreqPoint == 0 || nNumOfPoints == 0)// reset parameters to default values
	{
		if(eq_inter_param.ready)
			equ_quit(&eq_inter_param);

		// must set this, eq_init will load default band points
		eq_inter_param.user_band_points = 0;
		eq_inter_param.user_band_points_num = 0;


		equ_init(&eq_inter_param, 14);

		equ_clearbuf(&eq_inter_param, c_sampling_rate);

		int eq_points_num = eq_inter_param.internal_band_points_num + 2;
		int eq_bands_num = eq_inter_param.internal_band_points_num + 1;


			// allocate memory for new bands data

		int *t_c_eq = (int*) malloc(eq_bands_num * sizeof(int)); // equalizer data
		if(!t_c_eq)
		{
			error("CreateEqBands->Memory allocation error !");
			return 0;
		}

		REAL *t_c_lbands = (REAL*) malloc(eq_bands_num * sizeof(REAL));	// eqalizer param
		if(!t_c_lbands)
		{
			free(t_c_eq);
			error("CreateEqBands->Memory allocation error !");
			return 0;
		}

		REAL *t_c_rbands = (REAL*) malloc(eq_bands_num * sizeof(REAL));	// eqalizer param
		if(!t_c_rbands)
		{	
			free(t_c_eq);
			free(t_c_lbands);
			error("CreateEqBands->Memory allocation error !");
			return 0;
		}

		REAL *t_c_eq_band_points = (REAL*) malloc(eq_points_num * sizeof(REAL));	// eqalizer param
		if(!t_c_eq_band_points)
		{	
			free(t_c_eq);
			free(t_c_lbands);
			free(t_c_rbands);
			error("CreateEqBands->Memory allocation error !");
			return 0;
		}

		free(c_eq);
		free(c_lbands);
		free(c_rbands);
		free(c_eq_band_points);
		c_eq = t_c_eq;
		c_lbands = t_c_lbands;
		c_rbands = t_c_rbands;
		c_eq_band_points = t_c_eq_band_points;


		int i;
		c_eq_preamp = 0;
		for(i = 0; i < eq_bands_num; i++)
		{
			c_lbands[i] = 1.0;
			c_rbands[i] = 1.0;
			c_eq[i] = 0;
		}

				// get internal bands
		c_eq_band_points[i] = 0;
		for (i = 1; i < eq_points_num - 1; i++)
			c_eq_band_points[i] = eq_inter_param.internal_band_points[i];

		c_eq_band_points[eq_points_num - 1] = EQ_MAX_FREQ;

		c_eq_band_points_num = eq_points_num;
		c_bands_num = eq_bands_num;

		
		
		// generate new eq tables
		if(c_bUseExternalEQ)
			PostThreadMessage(c_hThreadEqSetParamId, MSG_EQ_PARAM_SET, 0,0);

		return 1;

	}

		int eq_points_num = nNumOfPoints;

	int eq_bands_num = nNumOfPoints - 1;
	
	// allocate memory for new bands data

	int *t_c_eq = (int*) malloc(eq_bands_num * sizeof(int)); // equalizer data
	if(!t_c_eq)
	{
		error("CreateEqBands->Memory allocation error !");
		return 0;
	}

	REAL *t_c_lbands = (REAL*) malloc((eq_points_num + 1) * sizeof(REAL));	// eqalizer param
	if(!t_c_lbands)
	{
		free(t_c_eq);
		error("CreateEqBands->Memory allocation error !");
		return 0;
	}

	REAL *t_c_rbands = (REAL*) malloc((eq_points_num + 1) * sizeof(REAL));	// eqalizer param
	if(!t_c_rbands)
	{	
		free(t_c_eq);
		free(t_c_lbands);
		error("CreateEqBands->Memory allocation error !");
		return 0;
	}

	REAL *t_c_eq_band_points = (REAL*) malloc(eq_points_num * sizeof(REAL));	// eqalizer param
	if(!t_c_eq_band_points)
	{	
		free(t_c_eq);
		free(t_c_lbands);
		free(t_c_rbands);
		error("CreateEqBands->Memory allocation error !");
		return 0;
	}



	free(c_eq);
	free(c_lbands);
	free(c_rbands);
	free(c_eq_band_points);
	c_eq = t_c_eq;
	c_lbands = t_c_lbands;
	c_rbands = t_c_rbands;
	c_eq_band_points = t_c_eq_band_points;



	equ_quit(&eq_inter_param); // delete old bands

	int i;
	for(i = 0; i < eq_points_num; i++)
		c_eq_band_points[i] = (REAL) pnFreqPoint[i];
	

	int num = eq_points_num;
	if(c_eq_band_points[0] == 0)
	{
		eq_inter_param.user_band_points = &c_eq_band_points[1];
		num--;

	}
	else
	{
		eq_inter_param.user_band_points = c_eq_band_points;

	}

	if(c_eq_band_points[eq_points_num - 1] == EQ_MAX_FREQ)
		num--;

	c_eq_band_points_num = eq_points_num;
	c_bands_num = eq_bands_num;
	
	eq_inter_param.user_band_points_num = num;
	equ_init(&eq_inter_param, 14); // generate new bands

	equ_clearbuf(&eq_inter_param, c_sampling_rate);

	for(i = 0; i < eq_points_num + 1; i++)
	{
		c_lbands[i] = 1.0;
		c_rbands[i] = 1.0;
	}

	c_eq_preamp = 0;
	for(i = 0; i < eq_bands_num; i++)
		c_eq[i] = 0;




	// generate new eq tables
	if(c_bUseExternalEQ)
		PostThreadMessage(c_hThreadEqSetParamId, MSG_EQ_PARAM_SET, 0,0);

	return 1;
}


int __stdcall WMp3::GetEqBands(int *pnFreqPoint)
{
	if(pnFreqPoint == 0)
		return c_eq_band_points_num;

	int i;
	for(i = 0; i < c_eq_band_points_num; i++)
		pnFreqPoint[i] = (int) c_eq_band_points[i];

	return c_eq_band_points_num;

}



int __stdcall WMp3::GetSfxModeData(int nMode,  int *pnModeDataLeft, int *pnModeDataRight)
{
	if(pnModeDataLeft == 0 || pnModeDataRight == 0)
		return ECHO_MAX_DELAY_NUMBER; 

	int i;
	for(i = 0; i < ECHO_MAX_DELAY_NUMBER; i++)
	{
		pnModeDataLeft[i] = (int) (sfx_mode[nMode * 2][i] * 100.0 );
		pnModeDataRight[i] = (int) (sfx_mode[nMode * 2 + 1][i] * 100.0 );

	}


	return ECHO_MAX_DELAY_NUMBER;

}



int __stdcall WMp3::GetSfxParam(int *pnMode, int *pnDelayMs, int *pnInputGain, int *pnEchoGain, int *pnOutputGain)
{
	if(pnMode == 0 || pnDelayMs == 0 || pnInputGain == 0  || pnEchoGain == 0 || pnOutputGain == 0)
	{
		error("GetSfxParam->Invalid parameters !");
		return 0; 
	}

	*pnMode = c_echo_mode;
	*pnDelayMs = c_nDelayMs;
	*pnInputGain = c_nInputGain;
	*pnEchoGain = c_nEchoGain;
	*pnOutputGain = c_nOutputGain;

	return 1;


}

int __stdcall WMp3::SetSfxModeData(int nMode,  int *pnModeDataLeft, int *pnModeDataRight, int nSize)
{
	if(pnModeDataLeft == 0 || pnModeDataRight == 0 || nSize != ECHO_MAX_DELAY_NUMBER)
	{
		error("SetSfxModeData->Invalid parameters !");
		return 0; 
	}

	int i;
	for(i = 0; i < ECHO_MAX_DELAY_NUMBER; i++)
	{
		sfx_mode[nMode * 2][i] = (real) pnModeDataLeft[i] / 100.0;
		sfx_mode[nMode * 2 + 1][i] = (real) pnModeDataRight[i] / 100.0 ;

	}

	if(c_echo && nMode == c_echo_mode)
	{
		SetSfxParam(c_echo, c_echo_mode, c_nDelayMs, c_nInputGain, c_nEchoGain, c_nOutputGain);


	}

	return 1;
}


void __stdcall WMp3::Release()
{
	delete this;
}


int __stdcall WMp3::IsStreamDataFree(const void *sMemNewData)
{

	if(!c_stream_managed || !c_start)
		return 1;

	if(c_buffered_stream)
	{
		unsigned int num = c_buffered_queue.GetCount();
		if(num == 0)	// empty queue
			return 1;

		if(c_buffered_queue.FindFromFirst((void*)  sMemNewData)) // data is in queue
			return 0;

		return 1;
				
				
	}
	else
	{
		unsigned int num;
		num = c_queue.GetNum();
		if(num == 0)	// empty queue
			return 1;

		if(c_queue.FindFromFirst((void*) sMemNewData)) // data is in queue
			return 0;

		return 1;
	}

}

int __stdcall WMp3::PushDataToStream(const void *sMemNewData, unsigned int nNewDataize)
{
	if(!c_start)
	{
		error("PushDataToStream->Stream isn't open !");
		return 0;
	}

	if(!c_stream_managed)
	{
		error("PushDataToStream->Can't push data into unmanaged stream !");
		return 0;
	}

	c_endstream = 0;

	if(sMemNewData == 0 || nNewDataize == 0)
	{
		c_endstream = 1;
		return 1;
	}

	if(c_buffered_stream)
	{
		if(!c_buffered_queue.PushLast((void*) sMemNewData, nNewDataize))
		{
			error("PushDataToStream->Memory allocation error !");
			return 0;

		}
	}
	else
	{	
		if(!c_queue.Push((void*) sMemNewData, nNewDataize))
		{
			error("PushDataToStream->Memory allocation error !");
			return 0;

		}
	}

	SetEvent(c_hStreamEvent);

	return 1;

}


int __stdcall WMp3::GetStreamLoad(STREAM_LOAD_INFO *pStreamLoadInfo)
{

	if(pStreamLoadInfo == 0)
	{
		error("GetStreamLoad->Invalid parameter !  STREAM_LOAD_INFO *pStreamLoadInfo can't be NULL!");
		return 0;
	}

	memset(pStreamLoadInfo, 0, sizeof(STREAM_LOAD_INFO));

	if(!c_start)
	{
		error("GetStreamLoad->Stream isn't open !");
		return 0;
	}

	if(!c_stream_managed)
	{
		error("GetStreamLoad->Stream isn't managed. Can't get this data !");
		return 0;
	}

	if(c_buffered_stream)
	{
		pStreamLoadInfo->NumberOfBuffers =  c_buffered_queue.GetCount();
		pStreamLoadInfo->NumberOfBytes = c_buffered_queue.GetSizeSum();
		if(c_bitrate_sum == 0 || c_frame_counter == 0)
		{
			pStreamLoadInfo->NumberOfMilliseconds = MulDiv(pStreamLoadInfo->NumberOfBytes , 8,	c_bitrate * 1000);

		}
		else
			pStreamLoadInfo->NumberOfMilliseconds = MulDiv(pStreamLoadInfo->NumberOfBytes * 8, c_frame_counter, c_bitrate_sum);

	
	}
	else
	{
		pStreamLoadInfo->NumberOfBuffers =  c_queue.GetNum();
		pStreamLoadInfo->NumberOfBytes = c_queue.GetDataSize();

		if(c_bitrate_sum == 0 || c_frame_counter == 0)
		{
			pStreamLoadInfo->NumberOfMilliseconds = MulDiv(pStreamLoadInfo->NumberOfBytes , 8,	c_bitrate * 1000);

		}
		else
			pStreamLoadInfo->NumberOfMilliseconds = MulDiv(pStreamLoadInfo->NumberOfBytes * 8, c_frame_counter, c_bitrate_sum);


	}


	return 1;
}



int __stdcall WMp3::OpenStream(int fBuffered, int fManaged, const void *sMemStream, unsigned int nStreamSize,  int nWaveBufferLengthMs)
{
	Close();



	c_skip_xing = 0;


	c_buffered_stream = fBuffered;
	c_stream_managed = fManaged;

	c_endstream = 0;

	if(c_tmp_ds)
		LocalFree(c_tmp_ds);

	c_tmp_ds = 0;

	if(c_stream_start)
	{
		error("OpenStream->You need to close mp3 stream before open new one !");
		return 0;
	}

	if(!sMemStream || nStreamSize == 0)
	{
		error("OpenStream->Can't open NULL stream !");
		return 0;
	}



	if(c_buffered_stream)
	{
		if(!c_buffered_queue.PushLast((void*) sMemStream, nStreamSize))
		{

			error("OpenStream->Memory allocation error !");
			return 0;
		}

		unsigned char *ptr;
		unsigned int size; 
		if(c_buffered_queue.QueryFirstPointer((void**) &ptr, &size) == 0)
		{
			c_buffered_queue.Clear();
			error("OpenStream->Memory allocation error !");
			return 0;
		}

		c_stream_start = (unsigned char*) ptr;
		c_stream_size = size;
		c_stream_end = c_stream_start +  c_stream_size - 1;

		

	}
	else
	{
		if(!c_queue.Push((void*) sMemStream, nStreamSize))
		{
			Close();
			error("OpenStream->Memory allocation error !");
			return 0;

		}

		c_stream_start = (unsigned char*) sMemStream;
		c_stream_size = nStreamSize;
		c_stream_end = c_stream_start +  c_stream_size - 1;
	}




	int ret =  _OpenMp3(this, nWaveBufferLengthMs);


	if(ret)
	{	
		return 1;
	}
	else
	{
		if(c_buffered_stream)
			c_buffered_queue.Clear();
		else
			c_queue.Clear();
	}
	

	return 0;

}


int __stdcall WMp3::EnumerateWaveOut()
{
	unsigned int num = waveOutGetNumDevs(); 
	if(num == 0)
	{
		error("EnumerateWaveOut->There is no wave-out devices !");
		return 0;
	}

	unsigned int i;

	WAVEOUTCAPS *pwoc = (WAVEOUTCAPS*) malloc(num * sizeof(WAVEOUTCAPS));
	if(!pwoc)
	{
		error("EnumerateWaveOut->Memory allocation error !");
		return 0;
	}

	
	int num1 = 0;
	for(i = 0; i < num; i++)
	{
		if(waveOutGetDevCaps(i, &pwoc[num1], sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
			num1++;	
	}

	if(c_pwoc)
		free(c_pwoc);

	c_pwoc = pwoc;
	c_woc_num = num1;



	return c_woc_num;

}

int __stdcall WMp3::GetWaveOutInfo(unsigned int nIndex, WAVEOUT_INFO *pWaveOutInfo)
{
	if(pWaveOutInfo == 0)
	{
		error("GetWaveOutInfo->Invalid second parameter. pWaveOutInfo can't be 0 !");	
		return 0;
	}

	memset(pWaveOutInfo, 0, sizeof(WAVEOUT_INFO));


	if(c_woc_num == 0)
	{
		if(EnumerateWaveOut() == 0)
		{
			error("GetWaveOutInfo->There is no wave-out devices !");
			return 0;
		}	
	}

	if(c_woc_num == 0)
	{
		error("GetWaveOutInfo->There is no wave-out devices !");
		return 0;
	}

	if(nIndex >= c_woc_num)
	{
		error("GetWaveOutInfo->Invalid first parameter. nIndex is out of bounds !");	
		return 0;
	}


	pWaveOutInfo->ManufacturerID = c_pwoc[nIndex].wMid;
	pWaveOutInfo->ProductID = c_pwoc[nIndex].wPid;
	pWaveOutInfo->DriverVersion = c_pwoc[nIndex].vDriverVersion;
	pWaveOutInfo->Formats = c_pwoc[nIndex].dwFormats;
	pWaveOutInfo->Channels = c_pwoc[nIndex].wChannels;
	pWaveOutInfo->Support = c_pwoc[nIndex].dwSupport;
	memcpy(pWaveOutInfo->ProductName, c_pwoc[nIndex].szPname, 32);
	pWaveOutInfo->ProductName[31] = 0;

	return 1;
}


int __stdcall WMp3::SetWaveOutDevice(unsigned int nIndex)
{
	if(c_woc_num == 0)
	{
		if(EnumerateWaveOut() == 0)
		{
			error("SetWaveOutDevice->There is no wave-out devices !");
			return 0;
		}	
	}

	if(c_woc_num == 0)
	{
		error("SetWaveOutDevice->There is no wave-out devices !");
		return 0;
	}

	if(nIndex != WAVE_OUT_WAVE_MAPPER && nIndex >= c_woc_num)
	{
		error("SetWaveOutDevice->Invalid first parameter. nIndex is out of bounds !");	
		return 0;
	}

	c_current_woc = nIndex;
	return 1;
}


unsigned int Mp3TimeToMs(int fFormat, MP3_TIME *pTime)
{
	if(pTime == 0)
		return 0;

	switch(fFormat)
	{
		case TIME_FORMAT_MS:
		return  pTime->ms;

		case TIME_FORMAT_SEC:
		return ( pTime->sec * 1000 );


		case TIME_FORMAT_HMS:
		{
			unsigned int seek_time;
			seek_time = pTime->hms_millisecond;
			seek_time += pTime->hms_second * 1000;
			seek_time += pTime->hms_minute * 60000;
			seek_time += pTime->hms_hour * 3600000;
			return seek_time;
		}
	}

	return 0;
}

int __stdcall WMp3::ReverseMode(int fEnable)
{
	if(!c_start)
	{
		error("ReverseMode->Stream isn't open !");
		return 0;
	}

	if(c_stream_managed)
	{
		error("ReverseMode->This is managed stream. Can't use reverse mode !");
		return 0;
	}

	// need to scan whole file
	if(fEnable && c_pFramePtr == 0)
	{
		struct mad_stream stream;
		struct mad_header header;

		mad_stream_init(&stream);
		mad_header_init(&header);

		mad_stream_buffer(&stream, c_start, c_size);

		unsigned char *tmp = 0;
		// count frames
		unsigned int count = 0;
		while(1)
		{
			if(mad_header_decode(&header,&stream))
			{
				if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
					continue;
			
				if(stream.error == MAD_ERROR_BUFLEN)
				{
					if(tmp)
					{
						free(tmp);
						tmp = 0;
						break;
					}

					// fix MAD_BUFFER_GUARD problem
					unsigned int len = 0;
				
					len = c_end + 1 - stream.this_frame;
					if(len == 0)
						break;
					
					tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
					if(!tmp)
						break;
				

					// copy last frame into buffer and add MAD_BUFFER_GUARD zero bytes to end
					memcpy(tmp, stream.this_frame, len);
					memset(tmp + len, 0, MAD_BUFFER_GUARD);
					tmp[len] = 0xFF;
					tmp[len + 1] = 0xE0;
					mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
					continue;
				}
			}

			count++;
		}

		if(tmp)
		{
			free(tmp);
			tmp = 0;
		}

		mad_stream_finish(&stream);
		mad_header_finish(&header);

		if(count == 0)
		{
			error("ReverseMode->There is no mp3 frame in this stream !");
			return 0;
		}

		c_pFramePtr = (MP3_FRAME_PTR*) malloc(count * sizeof(MP3_FRAME_PTR));
		if(!c_pFramePtr)
		{
			error("ReverseMode->Memory allocation error !");
			return 0;
		}


		mad_stream_init(&stream);
		mad_header_init(&header);
		mad_stream_buffer(&stream, c_start, c_size);

		tmp = 0;
		count = 0;

		while(1)
		{
			if(mad_header_decode(&header,&stream))
			{
				if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
					continue;
			
				if(stream.error == MAD_ERROR_BUFLEN)
				{
					if(tmp)
					{
						free(tmp);
						tmp = 0;
						break;
					}

					// fix MAD_BUFFER_GUARD problem
					unsigned int len = 0;
				
					len = c_end + 1 - stream.this_frame;
					if(len == 0)
						break;
					
					tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
					if(!tmp)
						break;
				

					// copy last frame into buffer and add MAD_BUFFER_GUARD zero bytes to end
					memcpy(tmp, stream.this_frame, len);
					memset(tmp + len, 0, MAD_BUFFER_GUARD);
					tmp[len] = 0xFF;
					tmp[len + 1] = 0xE0;
					mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
					continue;
				}
			}

			c_pFramePtr[count].ptr = (unsigned char*) stream.this_frame;
//			c_pFramePtr[count].len = stream.next_frame - stream.this_frame;
			count++;

			
		}

		if(tmp)
		{
			free(tmp);
			tmp = 0;
		}

		mad_stream_finish(&stream);
		mad_header_finish(&header);

		c_nFrameNumber = count;
	}

	if(c_nFrameNumber == 0)
	{
		c_fReverseMode = 0;
		error("ReverseMode->There is no mp3 frame in this stream !");
		return 0;
	}

	MP3_TIME pTime;
	if(fEnable)
	{
		if(c_play)
		{
			GetPosition(&pTime);
			Stop();
			c_fReverseMode = 1;
			Seek(TIME_FORMAT_MS, &pTime,SONG_BEGIN);
			Play();
		}
		else
			c_fReverseMode = 1;	
	}
	else
	{
		if(c_play)
		{
			GetPosition(&pTime);
			Stop();
			c_fReverseMode = 0;
			Seek(TIME_FORMAT_MS, &pTime,SONG_BEGIN);
			Play();
		}
		else
			c_fReverseMode = 0;	
	}


	return 1;
}


void reverse_buffer(unsigned char *buf, unsigned int size, int bit, int ch)
{

	switch(bit)
	{
		case 16:
		{
			if(ch == 1)
			{
				unsigned short *samples = (unsigned short*) buf;
				short s;
				unsigned int num = size / 2;
				unsigned int heigh = num - 1;
				unsigned int low = 0;
		
				while(low < heigh)
				{
			
					s = samples[low];
					samples[low] = samples[heigh];
					samples[heigh] = s;

					
					low++;
					heigh--;
			
				}

				return;
			}

			unsigned int *samples = (unsigned int*) buf;
			unsigned int num = size / 4;
			if(num < 2)
				return;

			unsigned int s;
			unsigned int heigh = num - 1;
			unsigned int low = 0;
			while(low < heigh)
			{
				s = samples[low];
				samples[low] = samples[heigh];
				samples[heigh] = s;	
				low++;
				heigh--;
			
			}
		}
	}
}


void WMp3::internal_volume(WMp3 *mp3, unsigned char *buf, unsigned int size, int bit, int ch)
{
	switch(bit)
	{
		case 16:
		{
			if(ch == 1)
			{
				unsigned int j;
				short* left;
				for(j = 0; j < size; j += 2)
				{               	
					left = (short*) ( buf + j);
					*left = (short) ((double) *left  *  mp3->c_dLeftGain);

				}	

				return;
			}

			unsigned int j;
			short* left;
			short* right;
            for(j = 0; j < size; j += 4)
			{           	
				left = (short*) ( buf + j);
                right = (short*) ( buf + j + 2);
                *left = (short) ((double)*left  *  mp3->c_dLeftGain);
                *right = (short) ((double)*right * mp3->c_dRightGain);
			}
		}
	}
}

void WMp3::s_vocal_cut(unsigned char *buf, unsigned int size, int bit, int ch)
{
	if(ch < 2)
		return;
	unsigned int j;

	switch(bit)
	{
		case 16:
		{
			for(j = 0; j < size; j += 4)
			{
				short* left;
				short* right;
				left = (short*) ( buf + j);
                right = (short*) ( buf + j + 2);
				short l1;
				short r1;
				l1 = *left - *right;
				r1 = *right - *left;
				*left = l1;
				*right = r1;	
			}
		}
		break;
	}
}


int __stdcall WMp3::GetVersion()
{
	return LIBRARY_VERSION;
}

int __stdcall WMp3::SetCallbackFunc(t_mp3_callback callback, unsigned int nMessages)
{

	if(callback == 0)
	{
		callback_func = 0;
		callback_messages = 0;
		if(c_hThreadCallback)
		{
			PostThreadMessage(c_hThreadCallbackId, WM_QUIT, 0,0);
			if(WaitForSingleObject(c_hThreadCallback, 500) == WAIT_TIMEOUT)
				TerminateThread(c_hThreadCallback, 0);

			CloseHandle(c_hThreadCallback);
			c_hThreadCallback = 0;

		}

	}
	else
	{
		if(c_hThreadCallback)
		{
			PostThreadMessage(c_hThreadCallbackId, WM_QUIT, 0,0);
			if(WaitForSingleObject(c_hThreadCallback, 500) == WAIT_TIMEOUT)
				TerminateThread(c_hThreadCallback, 0);

			CloseHandle(c_hThreadCallback);
			c_hThreadCallback = 0;
		}
				
		ResetEvent(c_hEventCallback);
		c_hThreadCallback = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) CallbackThreadFunc, (void*) this, 0, &c_hThreadCallbackId);
		if(c_hThreadCallback == 0)
		{
			error("SetCallbackFunc->Can't create callback thread !");
			return 0;

		}

		WaitForSingleObject(c_hEventCallback, INFINITE);
			 
		


		callback_func = callback;
		callback_messages = nMessages;
	}

	return 1;
}


int WINAPI WMp3::CallbackThreadFunc(void* lpdwParam)
{
	WMp3 *mp3 = (WMp3*) lpdwParam;
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(mp3->c_hEventCallback);



	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		unsigned int message = (unsigned int) msg.message - WM_USER; 

		if(message > 0x3DFF) 
		{
			message = (message - 0x3DFF) << 16;
			mp3->c_callback_ret = mp3->callback_func(message, msg.wParam, msg.lParam);
			SetEvent(mp3->c_hEventCallback);
		}
		else
		{
			mp3->c_callback_ret = mp3->callback_func(message, msg.wParam, msg.lParam);
		}


	}


	return 1;
}


int __stdcall WMp3::SetMasterVolume(unsigned int nLeftVolume,unsigned int nRightVolume)
{

    if( nLeftVolume > 100)
    	nLeftVolume = 100;

	if( nRightVolume > 100)
    	nRightVolume = 100;

	
	unsigned int l = (unsigned int) (nLeftVolume * 0xFFFF / 100);
	unsigned int r = (unsigned int) (nRightVolume * 0xFFFF / 100);
	w_wave->SetVolume((unsigned short) l, (unsigned short) r);


	return 1;

}


int __stdcall WMp3::SetPlayerVolume(unsigned int nLeftVolume,unsigned int nRightVolume)
{

	if( nLeftVolume > 100)
    	nLeftVolume = 100;

	if( nRightVolume > 100)
    	nRightVolume = 100;

		
	_fade_volume = 100.0;
	c_lvolume =  nLeftVolume;
	c_rvolume = nRightVolume;


	c_dLeftGainVolume =  ( nLeftVolume < 100 ? (pow(10, (double) nLeftVolume / 100.0) / 10.0 - 0.1) : 1.0);		
	c_dRightGainVolume =  ( nRightVolume < 100 ? (pow(10, (double) nRightVolume / 100.0) / 10.0 - 0.1) : 1.0);

	c_dLeftGain = c_dLeftGainVolume * c_dLeftGainFade;
	c_dRightGain = c_dRightGainVolume * c_dRightGainFade;

    
   
	
	return 1;

}


int __stdcall WMp3::GetMasterVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume)
{
	if(pnLeftVolume == 0 || pnRightVolume == 0)
	{
		error("GetVolume->Invalid parameters !");
		return 0; 
	}


	WORD lvolume;
	WORD rvolume;
    w_wave->GetVolume(&lvolume,  &rvolume);
	*pnLeftVolume = MulDiv(lvolume , 100 , 65535);
	*pnRightVolume = MulDiv(rvolume , 100 , 65535);
 
	return 1;

}

int __stdcall WMp3::GetPlayerVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume)
{
	if(pnLeftVolume == 0 || pnRightVolume == 0)
	{
		error("GetVolume->Invalid parameters !");
		return 0; 
	}

	*pnLeftVolume = c_lvolume;
	*pnRightVolume = c_rvolume;

	return 1;

}






int WMp3::DecodeID3v2Header(WMp3 *mp3)
{

	// set fields to empty string
	*mp3->w_myID3.artist = 0;
	*mp3->w_myID3.title = 0;
	*mp3->w_myID3.album = 0;
	*mp3->w_myID3.tracknum = 0;
	*mp3->w_myID3.year = 0;
	*mp3->w_myID3.artist = 0;
	*mp3->w_myID3.comment = 0;
	*mp3->w_myID3.genre = 0;


	unsigned char *buf = mp3->c_stream_start;
	unsigned int buf_size = mp3->c_stream_size;

	// if buffer too small
	if(buf == 0 || buf_size < 10)
		return 0;

	// there is no ID3 identification
	if(memcmp(buf, "ID3", 3) != 0)
		return 0;


	// invalid major version and revision number
	if(buf[3] == 0xFF || buf[4] == 0xFF)
		return 0;

	// invalid tag size
	if(buf[6] > 0x80 || buf[7] > 0x80 || buf[8] > 0x80 || buf[9] > 0x80)
		return 0;

	// get tag size
	unsigned int tag_size = 0;
	tag_size = DecodeSyncSafeInteger4(buf[6], buf[7], buf[8], buf[9]);

	// check tag size
	if(tag_size + 10 > buf_size)
		return 0;

	// free unsync buffer
	if(mp3->c_id3header.unsync_buf)
		free(mp3->c_id3header.unsync_buf);

	mp3->c_id3header.unsync_buf = 0;





	// get major version, revision number and flags
	mp3->c_id3header.MajorVersion = buf[3];
	mp3->c_id3header.RevisionNumber = buf[4];
	mp3->c_id3header.nFlags = buf[5];

	mp3->c_id3header.TagSize = tag_size;
	mp3->c_id3header.TotalSize = mp3->c_id3header.TagSize + 10;

	mp3->c_id3header.Flags.Unsync = GetBit(buf[5], 7);
	mp3->c_id3header.Flags.Extended = GetBit(buf[5], 6);
	mp3->c_id3header.Flags.Experimental = GetBit(buf[5], 5);
	mp3->c_id3header.Flags.Footer = GetBit(buf[5], 4);


		// set stream position
	mp3->c_id3header.stream_start = buf;
	mp3->c_id3header.stream_size = tag_size;
	mp3->c_id3header.stream_end = buf + tag_size - 1;
	mp3->c_id3header.pos = buf + 10;



	switch(mp3->c_id3header.MajorVersion)
	{
		case 2:
		return decode_id3v2_2(mp3);
		

		case 3:
		return decode_id3v2_3(mp3);

		case 4:	
		return decode_id3v2_4(mp3);
		
	}

	
	return 0;


}





int WMp3::decode_id3v2_3(WMp3 *mp3)
{
	
	unsigned char *ptr;
	int size;
	if(mp3->c_id3header.Flags.Unsync)
	{
		mp3->c_id3header.unsync_buf = (unsigned char*) malloc(mp3->c_id3header.stream_size);
		if(mp3->c_id3header.unsync_buf == 0)
			return 0;

		memcpy(mp3->c_id3header.unsync_buf, mp3->c_id3header.pos, mp3->c_id3header.stream_size); 
		mp3->c_id3header.stream_size = id3_deunsynchronise(mp3->c_id3header.unsync_buf, mp3->c_id3header.stream_size);
		
		ptr = mp3->c_id3header.unsync_buf;
		size = mp3->c_id3header.stream_size;	
		

	}
	else
	{

		ptr = mp3->c_id3header.pos;
		size = mp3->c_id3header.stream_size;
	}

	


	if(mp3->c_id3header.Flags.Extended)
	{
		unsigned int pading = (ptr[6] << 24) + (ptr[7] << 16) +(ptr[8] << 8) + (ptr[9]);
		size = size - 10 - ptr[3];
		ptr = ptr + 10 + ptr[3];
	}


	int len;
	char **field;
	while(size > 10 && *ptr != 0)
	{
		field = 0;
		if(ptr[4] > 0x80 || ptr[5] > 0x80 || ptr[6] > 0x80 || ptr[7] > 0x80)
			return 0;

		len = DecodeSyncSafeInteger4(ptr[4], ptr[5], ptr[6], ptr[7]);
		if(len == 0 || len + 10  > size)
			return 0;

			// check flags

		if(GetBit(ptr[9], 6) || GetBit(ptr[9], 5))
		{
			ptr += 10;
			size -= 10;
			ptr += len;
			size -= len;
			continue;	// compression and encryption not supported
		}

		unsigned int data_length = len;


		if(memcmp(ptr, "TPE1", 4) == 0)
			field = &mp3->w_myID3.artist;
		else if(memcmp(ptr, "TIT2", 4) == 0)
			field = &mp3->w_myID3.title;
		else if(memcmp(ptr, "TALB", 4) == 0)
			field = &mp3->w_myID3.album;
		else if(memcmp(ptr, "TRCK", 4) == 0)
			field = &mp3->w_myID3.tracknum;
		else if(memcmp(ptr, "TYER", 4) == 0)
			field = &mp3->w_myID3.year;
		else if(memcmp(ptr, "TCON", 4) == 0)
		{
			field = &mp3->w_myID3.genre;
			
			
			ptr += 10;
			size -= 10;
		
			unsigned char *str = id3_get_str(ptr, &data_length);


			if(str)
			{
				if(*field)
					free(*field);

				*field = get_genre_from_index((char*) str, data_length);
				
					
			}
			else
				(*field)[0] = 0;
		

			ptr += len;
			size -= len;

			continue;


		}
		else if(memcmp(ptr, "COMM", 4) == 0)
		{
			field = &mp3->w_myID3.comment;
			ptr += 10;
			size -= 10;


			unsigned char *str = id3_get_comment_str(ptr, &data_length);
			if(str)
			{
				if(*field)
					free(*field);

				*field = (char*) str;

	


			}
			else
				(*field)[0] = 0;	
		

			ptr += len;
			size -= len;

			continue;
		}
	


		ptr += 10;
		size -= 10;

		if(field)
		{

			unsigned char *str = id3_get_str(ptr, &data_length);
			if(str)
			{
				if(*field)
					free(*field);

				*field = (char*) str;


			}
			else
				(*field)[0] = 0;

		}

		ptr += len;
		size -= len;

	
	}


	

	return 1;

}



unsigned int id3_deunsynchronise(unsigned char *data, unsigned int length)
{
	unsigned int bytes = 0;
  unsigned int count;

  unsigned char *end = data + length;
  unsigned char const *ptr;

  if (length == 0)
    return 0;

  for (ptr = data; ptr < end - 1; ++ptr)
  {
    if (ptr[0] == 0xff && (ptr[1] == 0x00 || (ptr[1] & 0xe0) == 0xe0))
      ++bytes;
  }

  if (bytes)
  {
    ptr  = end;
    end += bytes;

    *--end = *--ptr;

    for (count = bytes; count; *--end = *--ptr)
	{
      if (ptr[-1] == 0xff && (ptr[0] == 0x00 || (ptr[0] & 0xe0) == 0xe0))
	  {
			*--end = 0x00;
			--count;
      }
    }
  }

  return length + bytes;
}


unsigned char *id3_get_str(unsigned char *buf, unsigned int *len)
{
	int length = *len;

	if(length < 2)
		return 0;

	unsigned char *tmp = (unsigned char*) malloc(length + 3);

	if(!tmp)
		return 0;

	memcpy(tmp, buf, length);

	tmp[length] = 0;
	tmp[length + 1] = 0;
	tmp[length + 2] = 0;


	unsigned int encoding = tmp[0];


	unsigned int ret = id3_decode_str((unsigned char*) (tmp + 1) , length - 1, (unsigned char*) tmp, encoding);
	if(ret == 0)
	{
		free(tmp);
		return 0;
	}
	
		
	*len = ret;
	return tmp;


}

unsigned char *id3_get_comment_str(unsigned char *buf, unsigned int *len)
{
	int length = *len;

	if(length < 6)
		return 0;

	unsigned char *tmp = (unsigned char*) malloc(length + 3);

	if(!tmp)
		return 0;

	memcpy(tmp, buf, length);

	tmp[length] = 0;
	tmp[length + 1] = 0;
	tmp[length + 2] = 0;

	unsigned int encoding = tmp[0];


	
	unsigned int ret = id3_decode_str((unsigned char*) (tmp + 4) , length - 4, (unsigned char*) tmp, encoding);
	if(ret == 0)
	{
		free(tmp);
		return 0;
	}

	if(ret == 1 && *tmp == 0)
	{
		if(length > 5 && (encoding == 0 || encoding == 3))
		{
			memcpy(tmp, buf, length);
			tmp[length] = 0;
			tmp[length + 1] = 0;
			tmp[length + 2] = 0;
			ret = id3_decode_str((unsigned char*) (tmp + 5) , length - 5, (unsigned char*) tmp, encoding);
			if(ret == 0)
			{
				free(tmp);
				return 0;
			}
		}
		else if(length > 8 && encoding == 1)
		{
			memcpy(tmp, buf, length);
			tmp[length] = 0;
			tmp[length + 1] = 0;
			tmp[length + 2] = 0;

			ret = id3_decode_str((unsigned char*) (tmp + 8) , length - 8, (unsigned char*) tmp, encoding);
			if(ret == 0)
			{
				free(tmp);
				return 0;
			}
		
		}
		else if(length > 6 && encoding == 2)
		{
			memcpy(tmp, buf, length);
			tmp[length] = 0;
			tmp[length + 1] = 0;
			tmp[length + 2] = 0;

			ret = id3_decode_str((unsigned char*) (tmp + 6) , length - 6, (unsigned char*) tmp, encoding);
			if(ret == 0)
			{
				free(tmp);
				return 0;
			}
		
		}
	
		 	
	}

		
	*len = ret;
	return tmp;


}


int WMp3::decode_id3v2_4(WMp3 *mp3)
{
	
	unsigned char *ptr = mp3->c_id3header.pos;
	unsigned char *end = mp3->c_id3header.stream_end;
	int size = mp3->c_id3header.stream_size;

	if(ptr + 10 > end)
		return 0;


	if(mp3->c_id3header.Flags.Extended)
	{
		
		if(ptr[0] > 0x80 || ptr[1] > 0x80 || ptr[2] > 0x80 || ptr[3] > 0x80)
			return 0;

		int ext_size = DecodeSyncSafeInteger4(ptr[0], ptr[1], ptr[2], ptr[3]);
		if(ext_size < 6 || ext_size > size)
			return 0;

		size -= ext_size;
		ptr += ext_size;
	}


	int len;
	char **field;


	while(size > 10 && *ptr != 0) // we need enough data and check for padding 0 bytes
	{
		if(mp3->c_id3header.unsync_buf)
		{
			free(mp3->c_id3header.unsync_buf);
			mp3->c_id3header.unsync_buf = 0;
		}


		field = 0;
		// check size
		if(ptr[4] > 0x80 || ptr[5] > 0x80 || ptr[6] > 0x80 || ptr[7] > 0x80)
			return 0;

		len = DecodeSyncSafeInteger4(ptr[4], ptr[5], ptr[6], ptr[7]);
		if(len == 0 || len + 10  > size)
			return 0;

		// check flags

		if(GetBit(ptr[9], 3) || GetBit(ptr[9], 2))
		{
			ptr += 10;
			size -= 10;
			ptr += len;
			size -= len;
			continue;	// compression and encryption not supported
		}

		unsigned int data_length = len;

		int unsync = GetBit(ptr[9], 1);


		if(unsync)
		{
			mp3->c_id3header.unsync_buf = (unsigned char*) malloc(len * 2);
			if(!mp3->c_id3header.unsync_buf)
			{
				ptr += 10;
				size -= 10;
				ptr += len;
				size -= len;
				continue;
			}

			memcpy(mp3->c_id3header.unsync_buf, ptr + 10, len);
			data_length = id3_deunsynchronise(mp3->c_id3header.unsync_buf, len);
			
		}

		




		if(memcmp(ptr, "TPE1", 4) == 0)
			field = &mp3->w_myID3.artist;
		else if(memcmp(ptr, "TIT2", 4) == 0)
			field = &mp3->w_myID3.title;
		else if(memcmp(ptr, "TALB", 4) == 0)
			field = &mp3->w_myID3.album;
		else if(memcmp(ptr, "TRCK", 4) == 0)
			field = &mp3->w_myID3.tracknum;
		else if(memcmp(ptr, "TDRC", 4) == 0)
			field = &mp3->w_myID3.year;
		else if(memcmp(ptr, "TCON", 4) == 0)
		{
			field = &mp3->w_myID3.genre;
	
			ptr += 10;
			size -= 10;
		
			unsigned char *str = id3_get_str(ptr, &data_length);


			if(str)
			{
				if(*field)
					free(*field);

				*field = get_genre_from_index((char*) str, data_length);


			}
			else
				(*field)[0] = 0;
	

			ptr += len;
			size -= len;

			if(mp3->c_id3header.unsync_buf)
			{
				free(mp3->c_id3header.unsync_buf);
				mp3->c_id3header.unsync_buf = 0;
			}

			continue;


		}
		else if(memcmp(ptr, "COMM", 4) == 0)
		{
			field = &mp3->w_myID3.comment;

			ptr += 10;
			size -= 10;

			unsigned char *str = id3_get_comment_str(ptr, &data_length);
			if(str)
			{
			

				if(*field)
					free(*field);

				*field = (char*) str;

		
			}
			else
				(*field)[0] = 0;

			ptr += len;
			size -= len;

			if(mp3->c_id3header.unsync_buf)
			{
				free(mp3->c_id3header.unsync_buf);
				mp3->c_id3header.unsync_buf = 0;
			}

			continue;


		}
	

		ptr += 10;
		size -= 10;

		if(field)
		{
			unsigned char *str = 0;
			if(unsync && mp3->c_id3header.unsync_buf)
				str = id3_get_str(mp3->c_id3header.unsync_buf, &data_length);
			else
				str = id3_get_str(ptr, &data_length);	

			if(str)
			{
				if(*field)
					free(*field);

				*field = (char*) str;


			}
			else
				(*field)[0] = 0;

			
		}

		ptr += len;
		size -= len;

		if(mp3->c_id3header.unsync_buf)
		{
			free(mp3->c_id3header.unsync_buf);
			mp3->c_id3header.unsync_buf = 0;
		}

	}


	

	return 1;

}





int WMp3::decode_id3v2_2(WMp3 *mp3)
{

	unsigned char *ptr;
	int size;

	if(mp3->c_id3header.Flags.Extended)
		return 0;
	

	if(mp3->c_id3header.Flags.Unsync)
	{
		mp3->c_id3header.unsync_buf = (unsigned char*) malloc(mp3->c_id3header.stream_size);
		if(mp3->c_id3header.unsync_buf == 0)
			return 0;

		memcpy(mp3->c_id3header.unsync_buf, mp3->c_id3header.pos, mp3->c_id3header.stream_size); 
		mp3->c_id3header.stream_size = id3_deunsynchronise(mp3->c_id3header.unsync_buf, mp3->c_id3header.stream_size);
		
		ptr = mp3->c_id3header.unsync_buf;
		size = mp3->c_id3header.stream_size;	
		

	}
	else
	{

		ptr = mp3->c_id3header.pos;
		size = mp3->c_id3header.stream_size;
	}

	



	int len;
	char **field;
	while(size > 6 || *ptr != 0)
	{
		field = 0;
		if(ptr[3] > 0x80 || ptr[4] > 0x80 || ptr[5] > 0x80)
			return 0;

		len = DecodeSyncSafeInteger3(ptr[3], ptr[4], ptr[5]);
		if(len + 6  > size)
			return 0;

		unsigned int data_length = len;

		if(memcmp(ptr, "TP1", 3) == 0)
			field = &mp3->w_myID3.artist;
		else if(memcmp(ptr, "TT2", 3) == 0)
			field = &mp3->w_myID3.title;
		else if(memcmp(ptr, "TAL", 3) == 0)
			field = &mp3->w_myID3.album;
		else if(memcmp(ptr, "TRK", 3) == 0)
			field = &mp3->w_myID3.tracknum;
		else if(memcmp(ptr, "TYE", 3) == 0)
			field = &mp3->w_myID3.year;
		else if(memcmp(ptr, "COM", 3) == 0)
		{

			field = &mp3->w_myID3.comment;

			ptr += 6;
			size -= 6;

			unsigned char *str = id3_get_comment_str(ptr, &data_length);
			if(str)
			{

				if(*field)
					free(*field);

				*field = (char*) str;

			}
			else
				(*field)[0] = 0;

			ptr += len;
			size -= len;





			continue;
		}
		else if(memcmp(ptr, "TCO", 3) == 0)
		{
			field = &mp3->w_myID3.genre;

			ptr += 6;
			size -= 6;

		
			unsigned char *str = id3_get_str(ptr, &data_length);
			if(str)
			{
			
				if(*field)
					free(*field);

				*field = get_genre_from_index((char*) str, data_length);
				
			}
			else
				(*field)[0] = 0;

			ptr += len;
			size -= len;

			continue;
		}
	


		ptr += 6;
		size -= 6;

		if(field)
		{

			unsigned char *str = id3_get_str(ptr, &data_length);
			if(str)
			{

				if(*field)
					free(*field);

				*field = (char*) str;
				

			}
			else
				(*field)[0] = 0;

		}

		ptr += len;
		size -= len;

	
	}


	

	return 1;

}



unsigned int id3_decode_str(unsigned char *str, unsigned int length, unsigned char *dest, unsigned int encoding)
{
	unsigned int i;


	switch(encoding)
	{
		case 0: // Latin1
			memmove(dest, str, length);
			dest[length] = 0;
		break;

		case 1: // UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM
		{
		
			unsigned char *ptr = str;
			int byteorder = (( str[0] << 8 ) | str[1] );

			if (byteorder == 0xFFFE) // LE
			{
				ptr += 2;
			}
			else if (byteorder == 0xFEFF)  // BE
			{
				for(i = 0; i < length - 1; i += 2)
				{
					unsigned char c;
					c = str[i];
					str[i] = str[i + 1];
					str[i + 1] = c;	
				}

				ptr += 2;
			}

			if(length > 5)
			{
				if(ptr[0] == 0 && ptr[1] == 0)
					ptr += 2;

			}



			char *out = (char*) malloc(length);
			if(out == 0)
				return 0;

			length = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, (LPCWSTR) ptr, -1, out, length, NULL, NULL);
			if(length == 0)
			{	
				free(out);
				return 0;
			}
			memcpy(dest, out, length);
			free(out);

		}
		break;

		case 2:
		{
			
			char *out = (char*) malloc(length + 1);
			if(out == 0)
				return 0;

			length = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, (LPCWSTR) str, -1, out, length, NULL, NULL);
			memcpy(dest, out, length);
			free(out);

		}
		break;

		case 3: // UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, (char*) str, -1, NULL, 0) * 2;
			if(size > 0)
			{
				char *wide = (char*) malloc(size + 2);
				if(!wide)
					return 0;
			

				if(MultiByteToWideChar(CP_UTF8, 0, (char*) str, -1, (LPWSTR) wide, size))
				{
					length = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) wide, -1, (char*) dest, length, NULL, NULL);
				
				}
				else
					return 0;	
			}
			else
				return 0;
	
		}
		break;

	}


	return length;


}


char *get_genre_from_index(char *ptr, unsigned int size)
{
	char *tmp = ptr;
	unsigned int data_length = size;
	if(data_length > 3 && tmp[0] == '(' && tmp[1] != '(')
	{			
		char *end = (char*) memchr(tmp, ')', data_length);
		if(end && ((end - ptr) < 5 && (end - ptr) > 1 ) )
		{
						
			tmp++;
			*end = 0;
		
			int index = atoi(tmp);
			if(index < 148)
			{
				int length = strlen(sGenres[index]) + 1;
				if(size < length)
				{
					char *genre = (char*) malloc(length + 1);
					if(genre)
					{
						free(ptr);
						strcpy(genre, sGenres[index]);
						return genre;
	
					}
				}
				else
				{
					strcpy(ptr, sGenres[index]);
					
				}

			}
		}
	}

	return ptr;
	

}



int WMp3::send_fadeout_exit(WMp3 *mp3)
{
	int callback_ret = 0;

	if(mp3->c_enter_fade_out_sent == 1 &&  mp3->c_exit_fade_out_sent == 0)
	{
		mp3->c_exit_fade_out_sent = 1;
		if(mp3->callback_messages & MP3_MSG_EXIT_FADE_OUT)
			callback_ret = send_callback_message(mp3, MP3_MSG_EXIT_FADE_OUT, 0, 0);

		if(mp3->callback_messages & MP3_MSG_EXIT_FADE_OUT_ASYNC)
			send_callback_message(mp3, MP3_MSG_EXIT_FADE_OUT_ASYNC, 0,0);

	}

	return callback_ret;
}


int WMp3::send_fadein_exit(WMp3 *mp3)
{
	int callback_ret = 0;

	if(mp3->c_enter_fade_in_sent == 1 &&  mp3->c_exit_fade_in_sent == 0)
	{
		mp3->c_exit_fade_in_sent = 1;
		if(mp3->callback_messages & MP3_MSG_EXIT_FADE_IN)
			callback_ret = send_callback_message(mp3, MP3_MSG_EXIT_FADE_IN, 0, 0);

		if(mp3->callback_messages & MP3_MSG_EXIT_FADE_IN_ASYNC)
			send_callback_message(mp3,  MP3_MSG_EXIT_FADE_IN_ASYNC,0,0);

	}

	return callback_ret;
}



int WMp3::send_fadeout_enter(WMp3 *mp3)
{
	int callback_ret = 0;

	if(mp3->c_enter_fade_out_sent == 0)
	{
		mp3->c_enter_fade_out_sent = 1;

		if(mp3->callback_messages & MP3_MSG_ENTER_FADE_OUT)			
			callback_ret = send_callback_message(mp3, MP3_MSG_ENTER_FADE_OUT, 0, 0);

		if(mp3->callback_messages & MP3_MSG_ENTER_FADE_OUT_ASYNC)
			send_callback_message(mp3,  MP3_MSG_ENTER_FADE_OUT_ASYNC, 0,0);
	}

	return callback_ret;
			

}

int WMp3::send_fadein_enter(WMp3 *mp3)
{
	int callback_ret = 0;

	if(mp3->c_enter_fade_in_sent == 0)
	{
		mp3->c_enter_fade_in_sent = 1;

		if(mp3->callback_messages & MP3_MSG_ENTER_FADE_IN)			
			callback_ret = mp3->send_callback_message(mp3, MP3_MSG_ENTER_FADE_IN, 0, 0);

		if(mp3->callback_messages & MP3_MSG_ENTER_FADE_IN_ASYNC)
			send_callback_message(mp3, MP3_MSG_ENTER_FADE_IN_ASYNC, 0,0);
	}

	return callback_ret;
}


int WMp3::send_loop_enter(WMp3 *mp3)
{
	int callback_ret = 0;

	if(mp3->c_enter_loop_sent == 0)
	{
		mp3->c_enter_loop_sent = 1;
		if(mp3->callback_messages & MP3_MSG_ENTER_LOOP)
			callback_ret = mp3->send_callback_message(mp3, MP3_MSG_ENTER_LOOP, 0, 0);

		if(mp3->callback_messages & MP3_MSG_ENTER_LOOP_ASYNC)
			send_callback_message(mp3, MP3_MSG_ENTER_LOOP_ASYNC,0,0);


	}

	return callback_ret;
}


int WMp3::send_loop_exit(WMp3 *mp3)
{
	int callback_ret = 0;


	if(mp3->c_exit_loop_sent == 0)
	{
		mp3->c_exit_loop_sent = 1;
		if(mp3->callback_messages & MP3_MSG_EXIT_LOOP)
			callback_ret = mp3->send_callback_message(mp3, MP3_MSG_EXIT_LOOP, 0, 0);

		if(mp3->callback_messages & MP3_MSG_EXIT_LOOP_ASYNC)
			send_callback_message(mp3, MP3_MSG_EXIT_LOOP_ASYNC, 0,0);


	}

	return callback_ret;
}




int WMp3::send_buffer_done(WMp3 *mp3, unsigned int count, unsigned int sumsize)
{
	int callback_ret = 0;

	if(mp3->callback_messages & MP3_MSG_STREAM_BUFFER_DONE)
		callback_ret = send_callback_message(mp3, MP3_MSG_STREAM_BUFFER_DONE, count, sumsize);

	if(mp3->callback_messages & MP3_MSG_STREAM_BUFFER_DONE_ASYNC)
		send_callback_message(mp3, MP3_MSG_STREAM_BUFFER_DONE_ASYNC , count, sumsize);	
								
	return callback_ret;
}



int WMp3::send_stream_out_of_data(WMp3 *mp3)
{
	int callback_ret = 0;

	if(mp3->callback_messages & MP3_MSG_STREAM_OUT_OF_DATA)
		callback_ret = send_callback_message(mp3, MP3_MSG_STREAM_OUT_OF_DATA, 0, 0);

	if(mp3->callback_messages & MP3_MSG_STREAM_OUT_OF_DATA_ASYNC)
		send_callback_message(mp3, MP3_MSG_STREAM_OUT_OF_DATA_ASYNC , 0, 0);	
								
	return callback_ret;
}



int WMp3::send_stop(WMp3 *mp3)
{
	int callback_ret = 0;
	
	if(mp3->callback_messages & MP3_MSG_STOP)
		callback_ret = send_callback_message(mp3, MP3_MSG_STOP, 0, 0);

	if(mp3->callback_messages & MP3_MSG_STOP_ASYNC)
		send_callback_message(mp3, MP3_MSG_STOP_ASYNC, 0, 0);

	return callback_ret;
}



int WMp3::send_callback_message(WMp3 *mp3, unsigned int message, unsigned int param1, unsigned int param2)
{
	mp3->c_callback_ret = 0;
	
	if(message > 0xFFFF)
	{
		ResetEvent(mp3->c_hEventCallback);
		PostThreadMessage(mp3->c_hThreadCallbackId, ( message >> 16 ) + 0x3DFF + WM_USER, param1, param2);
		WaitForSingleObject(mp3->c_hEventCallback, INFINITE);

	}
	else
	{
		PostThreadMessage(mp3->c_hThreadCallbackId, message + WM_USER, param1, param2);
	}


	return mp3->c_callback_ret;
}



void WMp3::free_fft(WMp3 *mp3)
{
	// free FFT memory and initialize all pointers to 0

	if(mp3->c_pflSamples)
		free(mp3->c_pflSamples);

	if(mp3->c_pflWindow)
		free(mp3->c_pflWindow);

	if(mp3->c_pnIp)
		free(mp3->c_pnIp);

	if(mp3->c_pflw)
		free(mp3->c_pflw);



	mp3->c_pflSamples = 0;	// samples buffer
	mp3->c_pflWindow = 0;
	mp3->c_pnIp = 0;
	mp3->c_pflw = 0;
	mp3->c_nFFTPoints = 0;
	mp3->c_nFFTWindow = 0;



}

int WMp3::allocate_fft(WMp3 *mp3)
{
	ASSERT_W(mp3->c_nFFTPoints);

	// allocate FFT memory
	unsigned all_ok = 1;

	int nPoints = mp3->c_nFFTPoints;
	mp3->c_pflSamples = (REAL*) malloc(nPoints * sizeof(REAL));
	if(mp3->c_pflSamples == 0)
		all_ok = 0;	

	mp3->c_pflWindow = (REAL*) malloc(nPoints * sizeof(REAL));
	if(mp3->c_pflWindow == 0)
		all_ok = 0;	


	mp3->c_pnIp = (int*) malloc( (2 + (int) sqrt((REAL) nPoints / 2.0)) * sizeof(int));
	if(mp3->c_pnIp == 0)
		all_ok = 0;	

	mp3->c_pflw = (REAL*) malloc(nPoints / 2* sizeof(REAL));
	if(mp3->c_pflw == 0)
		all_ok = 0;	


	if(all_ok == 0)
	{
		free_fft(mp3);
		return 0;
	}


	mp3->c_pnIp[0] = 0;

	return 1;
}




void WMp3::set_fft_window(REAL *pflWindow, int nFFTPoints, int nWindow)
{
//	set FFT window
//	http://en.wikipedia.org/wiki/Window_function

	ASSERT_W(pflWindow);
	ASSERT_W(nFFTPoints);

	int i;
	double n;
	double N = (double) nFFTPoints;
	double pi = PI;
	for(i = 0; i < nFFTPoints; i++)
	{
		n = (double) i;

		switch(nWindow)
		{
			default:
			case FFT_RECTANGULAR: // Rectangular window
				pflWindow[i] = 1.0;
			break;

			case FFT_HAMMING: // Hamming window
				pflWindow[i] = (REAL) ( 0.53836 - 0.46164 * cos( (pi * 2.0 * n) /  (N - 1.0)));
			break;

			case FFT_HANN: // Hann window 
				pflWindow[i] = (REAL) ( 0.5 * (1 - cos((2.0 * pi * n) /  (N - 1.0))) );
			break;

			case FFT_COSINE: // Cosine window 
				pflWindow[i] = (REAL) ( sin( (pi * n) / (N - 1.0)) ); 
			break;

			case FFT_LANCZOS: // Lanczos window
			{
				double x = (( 2.0 * n / (N - 1.0)) - 1.0) * pi;
				pflWindow[i] = (REAL) ( sin(x) / x ); 
			}
			break;

			case FFT_BARTLETT: // Bartlett window (zero valued end-points)
			{
				double x =  N - 1.0 ;
				pflWindow[i] = (REAL) ( (2.0 / x) * ( x / 2.0 -  fabs(n - (x / 2.0))));
			}
			break;

			case FFT_TRIANGULAR: // Triangular window (non-zero end-points)
				pflWindow[i] = (REAL) ( ( 2.0 / N ) * ( ( N / 2.0) - fabs(n - ((N - 1.0) / 2.0  ) )  ) );
			break;

			case FFT_GAUSS: // Gauss windows
			{
				double q = 0.3;
				double exponent = -0.5 * pow(  (n -  (N - 1.0) / 2.0  ) / ( q *  (N - 1.0) / 2.0)  , 2);
				pflWindow[i] = (REAL) exp(exponent);
			}
			break;

			case FFT_BARTLETT_HANN: // Bartlett–Hann window
			{
				double a0 = 0.62;
				double a1 = 0.48;
				double a2 = 0.38;
				pflWindow[i] = (REAL) ( a0 - a1 * fabs(n / (N - 1.0) - 0.5) - a2 * cos( (2 * pi * n) / ( N - 1)) );
			}
			break;

			case FFT_BLACKMAN: // Blackman windows
			{
				double alpha = 0.16;
				double a0 = (1 - alpha) / 2.0;
				double a1 = 0.5;
				double a2 = alpha / 2.0;
				pflWindow[i] = (REAL) ( a0 - a1 * cos( (2 * pi * n) / (N - 1.0)) + a2 * cos(( 4 * pi * n) / (N - 1.0)));
			}
			break;

			case FFT_NUTTALL: // Nuttall window, continuous first derivative 
			{
				double a0 = 0.355768;
				double a1 = 0.487396;
				double a2 = 0.144232;
				double a3 = 0.012604;
				double x = N - 1.0;
				pflWindow[i] = (REAL) ( a0 - a1 * cos(( 2 * pi * n) / x) + a2 * cos((4 * pi * n) / x) - a3 * cos((6 * pi * n) / x) );
			}
			break;

			case FFT_BLACKMAN_HARRIS: // Blackman–Harris window
			{
				double a0 = 0.35875;
				double a1 = 0.48829;
				double a2 = 0.14128;
				double a3 = 0.01168;
				double x = N - 1.0;
				pflWindow[i] = (REAL) ( a0 - a1 * cos(( 2 * pi * n) / x) + a2 * cos((4 * pi * n) / x) - a3 * cos((6 * pi * n) / x) );
			}
			break;

			case FFT_BLACKMAN_NUTTALL: // Blackman–Nuttall window
			{
				double a0 = 0.3635819;
				double a1 = 0.4891755;
				double a2 = 0.1365995;
				double a3 = 0.0106411;
				double x = N - 1.0;
				pflWindow[i] = (REAL) ( a0 - a1 * cos(( 2 * pi * n) / x) + a2 * cos((4 * pi * n) / x) - a3 * cos((6 * pi * n) / x) );
			}
			break;

			case FFT_FLAT_TOP: // Flat top window
			{
				double a0 = 1.0;
				double a1 = 1.93;
				double a2 = 1.29;
				double a3 = 0.388;
				double a4 = 0.032; 
				double x = N - 1.0;
				pflWindow[i] = (REAL) ( a0 - a1 * cos(( 2 * pi * n) / x) + a2 * cos((4 * pi * n) / x) - a3 * cos((6 * pi * n) / x) +  a4 * cos((8 * pi * n) / x));
			}
			break;
		}
	}
}







void WMp3::load_fft_buffer(WMp3 *mp3, int right)
{
	ASSERT_W(mp3->c_nFFTPoints);

	unsigned int current_index = mp3->c_dwIndex;
	unsigned int pos = mp3->w_wave->GetPosition(BYTES);
	int offset = pos - mp3->c_dwPos;

	if(offset < 0)
		offset = 0;
	
	
	
	int have;
	short *data;
	unsigned int buff_size;

	int i;
	int j = 0;
	
	unsigned int need = mp3->c_nFFTPoints;
	unsigned int num = 0;
	for(i = 0; i < mp3->w_wave->GetNumOfBuffers(); i++)
	{
	
		data = (short*) (mp3->w_wave->GetBufferData(current_index) + offset);
		buff_size = mp3->w_wave->GetBufferSize(current_index);
		have = (int) ( buff_size - offset);
		num = 0;
	

	
		if(mp3->c_channel == 2) // stereo
		{
			for(j; j < need; j++)
			{
				if(have < 4 )
				{
					if(current_index == mp3->w_wave->GetNumOfBuffers() - 1)
						current_index = 0;
					else
						current_index++;

					offset = 0;
				
					break; 
				}

				mp3->c_pflSamples[j] = (REAL) data[num  + right] * mp3->c_pflWindow[j];
				num += 2;
				have -= 4;

			}

		}
		else
		{
			for(j; j < need; j++) // mono
			{
				if(have < 2 )
				{
					if(current_index == mp3->w_wave->GetNumOfBuffers() - 1)
						current_index = 0;
					else
						current_index++;

					offset = 0;
					break; 
				}

				mp3->c_pflSamples[j] = (REAL) data[num] * mp3->c_pflWindow[j];
				num++;
				have -= 2;
			}
		}

	}

	// if we don't have enough data in wave buffers, fill rest with 0
	for(j; j < need; j++) 		
		mp3->c_pflSamples[j] = 0;		
	


}


int __stdcall WMp3::GetFFTValues(FFT_STRUCT *fftStruct)
{
	if(fftStruct == 0) // free memory
	{
		free_fft(this);
		return 1;
	}


	if(c_pflSamples == 0 || c_nFFTPoints != fftStruct->nFFTPoints) // need to reallocte memory
	{
		free_fft(this);
		c_nFFTPoints = 0;

		// check if nFFTPoints is power of 2
		int i;
		for(i = 0; i < FFT_POINTS_NUM; i++)
		{
			if(fftStruct->nFFTPoints == FFT_Points_Table[i])
				break;

		}


		if(i == FFT_POINTS_NUM)
		{
			error("FFT_STRUCT->nFFTPoints parameter can be only one of these values:\n32, 64, 128, 512, 1024, 2048, 8192, 16384");
			return 0;
		}

		c_nFFTPoints = fftStruct->nFFTPoints;
		if(!allocate_fft(this))
		{
			c_nFFTPoints = 0;
			error("GetFFTValues->Memory allocation error !");
			return 0;
		}
	}
	
	

	double flSqrtFFTPoints = sqrt((double) c_nFFTPoints);

	fftStruct->nHarmonicNumber = c_nFFTPoints / 2 + 1;

	
	// calculate harmonics frequency
	if(fftStruct->pnHarmonicFreq)
	{
		int i;
		REAL fFirstHarmonic = (REAL) c_sampling_rate / (REAL) c_nFFTPoints; 
		fftStruct->pnHarmonicFreq[0] = 0;
		for(i = 1; i < c_nFFTPoints / 2 + 1; i++)
			fftStruct->pnHarmonicFreq[i] = (int) ( fFirstHarmonic + fFirstHarmonic * (REAL) (i - 1 ));

	}

	if(c_nFFTWindow != fftStruct->nFFTWindow)
	{
		set_fft_window(c_pflWindow, c_nFFTPoints, fftStruct->nFFTWindow);
		c_nFFTWindow = fftStruct->nFFTWindow;
	}



	

	if(c_play == 0 || c_start == 0)
	{
		

		if(fftStruct->pnLeftAmplitude)
			memset(fftStruct->pnLeftAmplitude, 0, fftStruct->nHarmonicNumber * sizeof(int));

		if(fftStruct->pnRightAmplitude)
			memset(fftStruct->pnRightAmplitude, 0, fftStruct->nHarmonicNumber * sizeof(int));

		if(fftStruct->pnLeftPhase)
			memset(fftStruct->pnLeftPhase, 0, fftStruct->nHarmonicNumber * sizeof(int));

		if(fftStruct->pnRightPhase)
			memset(fftStruct->pnRightPhase, 0, fftStruct->nHarmonicNumber * sizeof(int));

		

	}
	else // calculate what we need
	{

		int i;
		
		if(fftStruct->pnLeftAmplitude)
		{
		
			load_fft_buffer(this, 0);
			
			rdft(c_nFFTPoints, 1, c_pflSamples, c_pnIp, c_pflw);

			REAL re;
			REAL im;
			REAL amp;
		
			re = c_pflSamples[0];
			if(re < 0)
				re = re * (-1);

			amp = re  / flSqrtFFTPoints + 1;
			amp = 20.0 * log10(amp);
			fftStruct->pnLeftAmplitude[0] = (int) amp;

			re = c_pflSamples[1];
			if(re < 0)
				re = re * (-1);

			amp = re  / flSqrtFFTPoints + 1;
			amp = 20.0 * log10(amp);
			fftStruct->pnLeftAmplitude[c_nFFTPoints / 2] = (int) amp;



			for(i = 1; i < c_nFFTPoints / 2; i++)
			{
				re = c_pflSamples[i * 2] ;
				im = c_pflSamples[i * 2 + 1];
				amp = 20.0 * log10(sqrt(re * re + im * im) /  flSqrtFFTPoints + 1.0);
				fftStruct->pnLeftAmplitude[i] = (int) amp;
			}
		}
	

		if(fftStruct->pnLeftPhase)
		{
			if(fftStruct->pnLeftAmplitude == 0)
			{
				load_fft_buffer(this, 0);
				rdft(c_nFFTPoints, 1, c_pflSamples, c_pnIp, c_pflw);
			}
	
			REAL re;
			REAL im;
			fftStruct->pnLeftPhase[0] = 0;
			fftStruct->pnLeftPhase[c_nFFTPoints / 2] = 0;

			for(i = 1; i < c_nFFTPoints / 2; i++)
			{
				re = c_pflSamples[i * 2] ;
				im = c_pflSamples[i * 2 + 1];
				fftStruct->pnLeftPhase[i] = atan(im/re) * 57.295779513082320876798154814105;

			}
		}




		if(fftStruct->pnRightAmplitude)
		{

			load_fft_buffer(this, 1);


			rdft(c_nFFTPoints, 1, c_pflSamples, c_pnIp, c_pflw);
	
			REAL re;
			REAL im;
			REAL amp;
		
			re = c_pflSamples[0];
			if(re < 0)
				re = re * (-1);

			amp = re  / flSqrtFFTPoints + 1;
			amp = 20.0 * log10(amp);
			fftStruct->pnRightAmplitude[0] = (int) amp;

			re = c_pflSamples[1];
			if(re < 0)
				re = re * (-1);

			amp = re  / flSqrtFFTPoints + 1;
			amp = 20.0 * log10(amp);
			fftStruct->pnRightAmplitude[c_nFFTPoints / 2] = (int) amp;



			for(i = 1; i < c_nFFTPoints / 2; i++)
			{
				re = c_pflSamples[i * 2] ;
				im = c_pflSamples[i * 2 + 1];
				amp = 20.0 * log10(sqrt(re * re + im * im) /  flSqrtFFTPoints + 1.0);
				fftStruct->pnRightAmplitude[i] = (int) amp;

			}
		}


		if(fftStruct->pnRightPhase)
		{
			if(fftStruct->pnRightAmplitude == 0)
			{
				load_fft_buffer(this, 1);


				rdft(c_nFFTPoints, 1, c_pflSamples, c_pnIp, c_pflw);
			}
	
			REAL re;
			REAL im;
			fftStruct->pnRightPhase[0] = 0;
			fftStruct->pnRightPhase[c_nFFTPoints / 2] = 0;

			for(i = 1; i < c_nFFTPoints / 2; i++)
			{
				re = c_pflSamples[i * 2] ;
				im = c_pflSamples[i * 2 + 1];
				fftStruct->pnRightPhase[i] = atan(im/re) * 57.295779513082320876798154814105;

			}
		}


	}
	
	

	return 1;
}