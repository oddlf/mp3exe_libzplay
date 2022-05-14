#define STRICT
#include <windows.h>

#pragma hdrstop


#include <commctrl.h>

#include <stdio.h>
#include <math.h>

#include <dbt.h>


#include "..\libzplay.h"



#include "wopendialog.h"
#include "wsavedialog.h"
#include "wfile.h"

#include "main.h"

#include "mainform.h"
#include "resource.h"

#include "wbmpbutton.h"
#include "wbmptextbox.h"
#include "vumeter.h"
#include "wscrollbar.h"

#define REGISTRY_KEY "Software\\ZokSoft\\Mp3Exe"

using namespace libZPlay;


#define BT_REVERSE 99
#define BT_PREV 100
#define BT_PLAY 101
#define BT_PAUSE 102
#define BT_STOP 103
#define BT_NEXT 104
#define BT_OPEN 105
#define BT_MPEXE 106
#define BT_MENU 107
#define BT_MINIMIZE 108
#define BT_CLOSE 109

#define SB_DELAY 111

#define SB_SEEK 112
#define SB_LVOLUME 113
#define SB_RVOLUME  114


#define SB_MODE 118
#define SB_ECHO 119

#define SB_EQ1 120
#define SB_EQ2 121
#define SB_EQ3 122
#define SB_EQ4 123
#define SB_EQ5 124
#define SB_EQ6 125
#define SB_EQ7 126
#define SB_EQ8 127
#define SB_EQ9 128
#define SB_EQ10 129
#define SB_PREAMP 130

#define SB_INPUT 131
#define SB_OUTPUT 132
#define BT_EQ 133
#define BT_RESETEQ 134
#define BT_ECHO 135
#define TB_MODE 136
#define BT_MODENEXT 137
#define BT_MODEPREV 138
#define BT_LOCKVOLUME	139
#define TB_POSITION	140

#define BT_FFT_SPECTRUM_NEXT 141
#define BT_FFT_SPECTRUM_PREV 142
#define TB_SPECTRUM_MODE 143

#define BT_FFT_WINDOW_NEXT 144
#define BT_FFT_WINDOW_PREV 145
#define TB_FFT_WINDOW 146
#define BT_LINEAR 147



//int eq_points[11] = {0, 600, 1300, 1800, 2500, 4000, 6000, 8500, 11000, 14000, EQ_MAX_FREQ};

char *sModes [] = {
	"echo 1",
	"echo 2",
	"echo 3",
	"echo 4",
	"echo 5",
	"echo 6",
	"echo 7",
	"echo 8",
	"echo 9",
	"echo 10"
};



int eq_values[10];


WTooltip* tooltip;

WBmpButton* btReverse;
WBmpButton* btPrev;
WBmpButton* btPlay;
WBmpButton* btPause;
WBmpButton* btNext;
WBmpButton* btOpen;
WBmpButton* btMpexe;
WBmpButton* btMenu;
WBmpButton* btStop;
WBmpButton* btMinimize;
WBmpButton* btClose;
WBmpButton* btLock;

WBmpButton* btEq;
WBmpButton* btResetEQ;
WBmpButton* btEcho;
WBmpButton* btModePrev;
WBmpButton* btModeNext;

WScrollbar* sbSeek;

WVUMeter* vul;
WVUMeter* vur;

WScrollbar* sbLvolume;
WScrollbar* sbRvolume;
WScrollbar* sbDelay;
WScrollbar* sbEcho;
WScrollbar* sbInput;
WScrollbar* sbOutput;

WScrollbar* sbEq1;
WScrollbar* sbEq2;
WScrollbar* sbEq3;
WScrollbar* sbEq4;
WScrollbar* sbEq5;
WScrollbar* sbEq6;
WScrollbar* sbEq7;
WScrollbar* sbEq8;
WScrollbar* sbEq9;
WScrollbar* sbEq10;
WScrollbar* sbPreAmp;


WBmpFont* bmpfont;


WBmpTextBox* tbSongLength;
WBmpTextBox* tbSongPosition;
WBmpTextBox* tbSongName;
WBmpTextBox* tbSongSamplingRate;
WBmpTextBox* tbSongBitrate;
WBmpTextBox* tbSongStereoMode;
WBmpTextBox* tbSongStatus;
WBmpTextBox* tbHz;
WBmpTextBox* tbkbps;
WBmpTextBox* tbMode;

DWORD vu_l = 0;
DWORD vu_r = 0;









BOOL bMinimizeToTray = TRUE;		// minimize window to tray icon
BOOL bElapsedTime = TRUE;		// show elapsed time
BOOL bShowTooltip = TRUE;		// show tooltip
BOOL bInternalVolumeontrol = FALSE;	// internal volume control
BOOL bKeyShortcuts = TRUE;		// juse key shortcuts
BOOL bLockVolume = FALSE;		// lock left and right channel volume scrollbar
BOOL bEscKeyMinimize = FALSE;	// minimize with esc key
BOOL bAlwaysOnTop = FALSE;
BOOL sfx = FALSE;
BOOL eq = FALSE;


char mp3filename[MAX_PATH];


int reverse = 0;

BOOL Start(char* filename);


extern ZPlay *player;


extern WTooltip *tooltip;
extern MainForm *mainForm;
extern WApp *myApp;





BOOL NotifyButton(WBmpButton *button,BOOL down);

BOOL blockStatus = FALSE;



DWORD VolumeID = 0;
DWORD MVolumeID = 0;
DWORD MuteID = 0;
DWORD MWaveID = 0;


HMENU hmenuTrackPopup;

UINT IconID = 1547;



BOOL BlockTimer = FALSE;	// block main timer
BOOL Pause = FALSE;			// pause cd

BOOL Playing = FALSE;

//int scrolltrack = -1;

POINT moveMain;	// need for moving main window

#define SPECTRUM_MODES_NUM 7

char *g_szSpectrumModes[SPECTRUM_MODES_NUM] = { "Lines (Left on top)", "Lines (Right on top)", "Area (Left on top)",
												"Area (Right on top)", "Bars (Left on top)" , "Bars (Right on top)", "Spectral view"};

#define FFT_WINDOW_NUM 14
char *g_szFFTWindow[FFT_WINDOW_NUM] = { "Rectangular", "Hamming", "Hann", "Cosine", "Lanczos", "Bartlett", "Triangular",
										"Gauss", "Bartlett-Hann", "Blackman", "Nuttall", "Blackman-Harris", "Blackman-Nuttall", "Flat top"};

#define FFT_DISPLAY_WIDTH 275
#define FFT_DISPLAY_HEIGHT 141

#define FFT_X 0
#define FFT_Y 247


HBITMAP vubg;
HBITMAP vufg;

int g_FFTDisplayMode = gsLogarithmic;

LRESULT MainForm::OnCreate()
{

	



	// initialize eq
	memset(eq_values, 0, 10 * sizeof(int));

	c_nSpectrumType = gtBarsLeftOnTop;
	c_nSpectrumWindow = fwBlackmanHarris;

// load settings from registry

	HKEY registry;
	DWORD reg_type = REG_DWORD;
	DWORD size = 4;
	DWORD dwDisposition;
	DWORD value = 0;

	int eq_preamp = 12;

	int sfx_input = 12;
	int sfx_echo = 12;
	int sfx_output = 12;
	int sfx_mode = 0;
	int sfx_delay = 17;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY,
						0, "", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0,
						&registry, &dwDisposition) == ERROR_SUCCESS)
	{
						
		if(RegQueryValueEx(registry, "fftWindow", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			c_nSpectrumWindow = value;


		if(RegQueryValueEx(registry, "fftSpectrumType", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			c_nSpectrumType = value;

		if(RegQueryValueEx(registry, "fftDisplayMode", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			g_FFTDisplayMode = value;

		if(RegQueryValueEx(registry, "eqEnabled", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			eq = value;

		size = 10 * sizeof(int);
		if(RegQueryValueEx(registry, "eqData", 0, 0, (unsigned char*) &eq_values, &size) != ERROR_SUCCESS)
			NULL;

		size = 4;
		if(RegQueryValueEx(registry, "eqPreamp", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			eq_preamp = value;



		if(RegQueryValueEx(registry, "sfxInput", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			sfx_input = value;

		if(RegQueryValueEx(registry, "sfxEcho", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			sfx_echo = value;

		if(RegQueryValueEx(registry, "sfxOutput", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			sfx_output = value;

		if(RegQueryValueEx(registry, "sfxDelay", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			sfx_delay = value;

		if(RegQueryValueEx(registry, "sfxEnabled", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			sfx = value;

		if(RegQueryValueEx(registry, "sfxMode", 0, 0, (unsigned char*) &value, &size) == ERROR_SUCCESS)
			sfx_mode = value;


						
		RegCloseKey(registry);
	}

	// create new eq bands

	//player->SetEqualizerPoints(eq_points, 11);


    // create tooltip control
    tooltip = new WTooltip(Handle);
    tooltip->Activate(bShowTooltip);

	hbMain = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_MAIN));
	hbTitlebar = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_TITLEBAR));
	
    HBITMAP hbm;

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_FONT));
	bmpfont = new WBmpFont();
	if(!bmpfont->Open(hbm,"06 AMNWXYmw\r\n06 4 #*+@^_\r\n05 BCDGHJKOP\r\n05 QRSUVZabcè\r\n05 deéèghknoæ\r\n05 pqrsuvxy\r\n05 z02356789ž\r\n05 ""$%&<=>?\r\n04 EFLTft/\\{}ïI\r\n03 j1'(),-.:;[]`\r\n02 il!|\0",11)) {
		MessageBox(0,"Cant open","",0);
	}

	DeleteObject(hbm);


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTPREV));
	btPrev = new WBmpButton();
	btPrev->Create(PUSH_BUTTON,6,88,22,18,Handle,BT_PREV,myApp->Instance,hbm);
	DeleteObject(hbm);
	btPrev->AddTooltip(tooltip,"Jump 5 sec. back");


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTREVERSE));
	btReverse = new WBmpButton();
	btReverse->Create(PUSH_BUTTON,29,88,22,18,Handle,BT_REVERSE,myApp->Instance,hbm);
	DeleteObject(hbm);
	btReverse->AddTooltip(tooltip,"Play backward");
	btReverse->SetCheck(0);


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTPLAY));
	btPlay = new WBmpButton();
	btPlay->Create(PUSH_BUTTON,52,88,22,18,Handle,BT_PLAY,myApp->Instance,hbm);
	DeleteObject(hbm);
	btPlay->AddTooltip(tooltip,"Play forward");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTPAUSE));
	btPause = new WBmpButton();
	btPause->Create(PUSH_BUTTON,75,88,22,18,Handle,BT_PAUSE,myApp->Instance,hbm);
	DeleteObject(hbm);
	btPause->AddTooltip(tooltip,"Pause");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTSTOP));
	btStop = new WBmpButton();
	btStop->Create(PUSH_BUTTON,98,88,22,18,Handle,BT_STOP,myApp->Instance,hbm);
	DeleteObject(hbm);
	btStop->AddTooltip(tooltip,"Stop");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTNEXT));
	btNext = new WBmpButton();
	btNext->Create(PUSH_BUTTON,121,88,22,18,Handle,BT_NEXT,myApp->Instance,hbm);
	DeleteObject(hbm);
	btNext->AddTooltip(tooltip,"Jump 5 sec. forward");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTOPEN));
	btOpen = new WBmpButton();
	btOpen->Create(PUSH_BUTTON,144,88,22,18,Handle,BT_OPEN,myApp->Instance,hbm);
	DeleteObject(hbm);
	btOpen->AddTooltip(tooltip,"Open mp3 file");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMPEXE));
	btMpexe = new WBmpButton();
	btMpexe->Create(PUSH_BUTTON,170,88,22,18,Handle,BT_MPEXE,myApp->Instance,hbm);
	DeleteObject(hbm);
	btMpexe->AddTooltip(tooltip,"Convert Mp3 to Exe");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMENU));
	btMenu = new WBmpButton();
	btMenu->Create(PUSH_BUTTON,195,88,22,18,Handle,BT_MENU,myApp->Instance,hbm);
	DeleteObject(hbm);
	btMenu->AddTooltip(tooltip,"Show menu");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMINIMIZE));
	btMinimize = new WBmpButton();
	btMinimize->Create(PUSH_BUTTON,252,5,8,8,Handle,BT_MINIMIZE,myApp->Instance,hbm);
	DeleteObject(hbm);
	btMinimize->AddTooltip(tooltip,"Minimize player");

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTCLOSE));
	btClose = new WBmpButton();
	btClose->Create(PUSH_BUTTON,262,5,8,8,Handle,BT_CLOSE,myApp->Instance,hbm);
	
	btClose->AddTooltip(tooltip,"Close player");

	btLock = new WBmpButton();
	btLock->Create(CHECK_BUTTON,255,39,8,8,Handle,BT_LOCKVOLUME,myApp->Instance,hbm);
	DeleteObject(hbm);
	btLock->AddTooltip(tooltip,"Lock left and right volume");
	btLock->SetCheck(bLockVolume);

	

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTECHO));
	btEcho = new WBmpButton();
	btEcho->Create(CHECK_BUTTON,198,134,29,11,Handle,BT_ECHO,myApp->Instance,hbm);
	DeleteObject(hbm);
	btEcho->AddTooltip(tooltip,"Enable echo effects");
	DeleteObject(hbm);
	btEcho->SetCheck(sfx);

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTEQ));
	btEq = new WBmpButton();
	btEq->Create(CHECK_BUTTON,9,134,55,11,Handle,BT_EQ,myApp->Instance,hbm);
	DeleteObject(hbm);
	btEq->AddTooltip(tooltip,"Enable equalizer");
	DeleteObject(hbm);
	btEq->SetCheck(eq);



	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTRESETEQ));
	btResetEQ = new WBmpButton();
	btResetEQ->Create(PUSH_BUTTON,178,175,12,12,Handle,BT_RESETEQ,myApp->Instance,hbm);
	DeleteObject(hbm);
	btResetEQ->AddTooltip(tooltip,"Reset equalizer");
	DeleteObject(hbm);


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMODENEXT));
	btModeNext = new WBmpButton();
	btModeNext->Create(PUSH_BUTTON,260,212,9,9,Handle,BT_MODENEXT,myApp->Instance,hbm);
	DeleteObject(hbm);
	btModeNext->AddTooltip(tooltip,"Select next echo mode");
	DeleteObject(hbm);

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMODEPREV));
	btModePrev = new WBmpButton();
	btModePrev->Create(PUSH_BUTTON,196,212,9,9,Handle,BT_MODEPREV,myApp->Instance,hbm);
	DeleteObject(hbm);
	btModePrev->AddTooltip(tooltip,"Select previous echo mode");
	DeleteObject(hbm);


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMODENEXT));
	btSpectrumNext = new WBmpButton();
	btSpectrumNext->Create(PUSH_BUTTON,260,236,9,9,Handle,BT_FFT_SPECTRUM_NEXT,myApp->Instance,hbm);
	DeleteObject(hbm);
	btSpectrumNext->AddTooltip(tooltip,"Select next FFT display mode");
	DeleteObject(hbm);

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMODEPREV));
	btSpectrumPrev = new WBmpButton();
	btSpectrumPrev->Create(PUSH_BUTTON,150, 236,9,9,Handle,BT_FFT_SPECTRUM_PREV,myApp->Instance,hbm);
	DeleteObject(hbm);
	btSpectrumPrev->AddTooltip(tooltip,"Select previous FFT display mode");
	DeleteObject(hbm);


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_TBBG));
	tbSpectrumMode = new WBmpTextBox();
	tbSpectrumMode->Create(g_szSpectrumModes[c_nSpectrumType], 160, 236,99,11,Handle,TB_SPECTRUM_MODE, myApp->Instance, hbm, bmpfont);
	DeleteObject(hbm);
	tbSpectrumMode->AddTooltip(tooltip,"Change spectrum display mode");


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTLINEAR));
	btLinear = new WBmpButton();
	btLinear->Create(CHECK_BUTTON,9,236,36,11,Handle,BT_LINEAR, myApp->Instance,hbm);
	DeleteObject(hbm);
	btLinear->AddTooltip(tooltip,"Linear FFT spectrum");
	DeleteObject(hbm);
	btLinear->SetCheck(g_FFTDisplayMode);



	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMODENEXT));
	btFFTWindowNext = new WBmpButton();
	btFFTWindowNext->Create(PUSH_BUTTON,131,236,9,9,Handle,BT_FFT_WINDOW_NEXT,myApp->Instance,hbm);
	DeleteObject(hbm);
	btFFTWindowNext->AddTooltip(tooltip,"Select next FFT window");
	DeleteObject(hbm);

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMODEPREV));
	btFFTWindowPrev = new WBmpButton();
	btFFTWindowPrev->Create(PUSH_BUTTON,50,236,9,9,Handle,BT_FFT_WINDOW_PREV,myApp->Instance,hbm);
	DeleteObject(hbm);
	btFFTWindowPrev->AddTooltip(tooltip,"Select previous FFT window");
	DeleteObject(hbm);


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_TBBG));
	tbFFTWindow = new WBmpTextBox();
	tbFFTWindow->Create(g_szFFTWindow[c_nSpectrumWindow], 60, 236,70,11,Handle,TB_FFT_WINDOW, myApp->Instance, hbm, bmpfont);
	DeleteObject(hbm);
	tbFFTWindow->AddTooltip(tooltip,"Change FFT window");


	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_SBBG));
	HBITMAP hbm1 = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_SBTHUMB));
	sbSeek = new WScrollbar();
	sbSeek->Create(HORZ,12,75,210,6,Handle,SB_SEEK,myApp->Instance,hbm,hbm1);
	sbSeek->SetRange(0,0);
	DeleteObject(hbm);
	DeleteObject(hbm1);
	sbSeek->AddTooltip(tooltip,"Seek position");

	
	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_SBVERT));
	hbm1 = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_SBVTHUMB));
	
	sbLvolume = new WScrollbar();
	sbLvolume->Create(VERT,250,50,4,60,Handle,SB_LVOLUME,myApp->Instance,hbm,hbm1);
	sbLvolume->AddTooltip(tooltip,"Change left volume");

	sbRvolume = new WScrollbar();
	sbRvolume->Create(VERT,264,50,4,60,Handle,SB_RVOLUME,myApp->Instance,hbm,hbm1);
	
	sbDelay = new WScrollbar();
	sbDelay->Create(VERT,252,150,4,60,Handle,SB_DELAY,myApp->Instance,hbm,hbm1);
	sbDelay->AddTooltip(tooltip,"EFX delay");
	sbDelay->SetRange(20,0);
	//sbDelay->SetPos(sfx_delay,1);


	sbEcho = new WScrollbar();
	sbEcho->Create(VERT,225,150,4,60,Handle,SB_ECHO,myApp->Instance,hbm,hbm1);
	sbEcho->AddTooltip(tooltip,"EFX volume");
	sbEcho->SetRange(24,0);
	//sbEcho->SetPos(sfx_echo,1);

	sbInput = new WScrollbar();
	sbInput->Create(VERT,211,150,4,60,Handle,SB_INPUT,myApp->Instance,hbm,hbm1);
	sbInput->AddTooltip(tooltip,"EFX input volume");
	sbInput->SetRange(24,0);
	//sbInput->SetPos(sfx_input,1);

	sbOutput = new WScrollbar();
	sbOutput->Create(VERT,239,150,4,60,Handle,SB_OUTPUT,myApp->Instance,hbm,hbm1);
	sbOutput->AddTooltip(tooltip,"EFX output volume");
	sbOutput->SetRange(24,0);
	//sbOutput->SetPos(sfx_output,1);

	sbPreAmp = new WScrollbar();
	sbPreAmp->Create(VERT,20,150,4,60,Handle,SB_PREAMP,myApp->Instance,hbm,hbm1);
	sbPreAmp->SetRange(24000,0);
	sbPreAmp->SetPos(12000,1);

	sbEq1 = new WScrollbar();
	sbEq1->Create(VERT,40,150,4,60,Handle,SB_EQ1,myApp->Instance,hbm,hbm1);
	sbEq1->SetRange(24000,0);
	sbEq1->SetPos(12000 - eq_values[0],1);

	sbEq2 = new WScrollbar();
	sbEq2->Create(VERT,54,150,4,60,Handle,SB_EQ2,myApp->Instance,hbm,hbm1);
	sbEq2->SetRange(24000,0);
	sbEq2->SetPos(12000 - eq_values[1] ,1);


	sbEq3 = new WScrollbar();
	sbEq3->Create(VERT,68,150,4,60,Handle,SB_EQ3,myApp->Instance,hbm,hbm1);
	sbEq3->SetRange(24000,0);
	sbEq3->SetPos(12000 - eq_values[2] ,1);


	sbEq4 = new WScrollbar();
	sbEq4->Create(VERT,82,150,4,60,Handle,SB_EQ4,myApp->Instance,hbm,hbm1);
	sbEq4->SetRange(24000,0);
	sbEq4->SetPos(12000 - eq_values[3] ,1);


	sbEq5 = new WScrollbar();
	sbEq5->Create(VERT,96,150,4,60,Handle,SB_EQ5,myApp->Instance,hbm,hbm1);
	sbEq5->SetRange(24000,0);
	sbEq5->SetPos(12000 - eq_values[4] ,1);


	sbEq6 = new WScrollbar();
	sbEq6->Create(VERT,110,150,4,60,Handle,SB_EQ6,myApp->Instance,hbm,hbm1);
	sbEq6->SetRange(24000,0);
	sbEq6->SetPos(12000 - eq_values[5] ,1);


	sbEq7 = new WScrollbar();
	sbEq7->Create(VERT,124,150,4,60,Handle,SB_EQ7,myApp->Instance,hbm,hbm1);
	sbEq7->SetRange(24000,0);
	sbEq7->SetPos(12000 - eq_values[6],1);


	sbEq8 = new WScrollbar();
	sbEq8->Create(VERT,138,150,4,60,Handle,SB_EQ8,myApp->Instance,hbm,hbm1);
	sbEq8->SetRange(24000,0);
	sbEq8->SetPos(12000 - eq_values[7],1);


	sbEq9 = new WScrollbar();
	sbEq9->Create(VERT,152,150,4,60,Handle,SB_EQ9,myApp->Instance,hbm,hbm1);
	sbEq9->SetRange(24000,0);
	sbEq9->SetPos(12000 - eq_values[8],1);


	sbEq10 = new WScrollbar();
	sbEq10->Create(VERT,166,150,4,60,Handle,SB_EQ10,myApp->Instance,hbm,hbm1);
	sbEq10->SetRange(24000,0);
	sbEq10->SetPos(12000 - eq_values[9],1);


	DeleteObject(hbm);
	DeleteObject(hbm1);
	sbRvolume->AddTooltip(tooltip,"Change right volume");




	vubg = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_VUBG));
	vufg = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_VU));

	
	vul = new WVUMeter();
	vur = new WVUMeter();
	vul->Create(Handle,myApp->Instance,225,50,7,61,vubg, vufg,1,1);
	vul->SetRange(100);
	vur->Create(Handle,myApp->Instance,235,50,7,61,vubg, vufg,1,1);
	vur->SetRange(100);

	vul->SetPeekFalloff(1,100);
	vur->SetPeekFalloff(1,100);

	
	
	player->SetFFTGraphParam(gpGraphType, c_nSpectrumType);
	player->SetFFTGraphParam(gpHorizontalScale, g_FFTDisplayMode);
	player->SetFFTGraphParam(gpWindow, c_nSpectrumWindow);
    
	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_TBMODE));
	tbMode = new WBmpTextBox();
	tbMode->Create(sModes[0],207,211,52,11,Handle,TB_MODE,myApp->Instance,hbm, bmpfont);
	DeleteObject(hbm);

 

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_TBBG));

	tbSongLength = new WBmpTextBox();
	tbSongLength->Create("00:00:00",15,26,40,11,Handle,0,myApp->Instance,hbm, bmpfont);
	
	tbSongPosition = new WBmpTextBox();
	tbSongPosition->Create("00:00:00",15,40,38,11,Handle,M_TIMEELAPSED,myApp->Instance,hbm, bmpfont);
	tbSongPosition->AddTooltip(tooltip,"Time Display (click to Toggle - Elapsed/Remaining)");

	tbSongName = new WBmpTextBox();
	tbSongName->Create("",60,26,210,11,Handle,0,myApp->Instance,hbm, bmpfont);
	tbSongName->SetScrollTextDelimiter(" *** ");
	tbSongName->EnableScroll(1,300);
	

	tbSongSamplingRate = new WBmpTextBox();
	tbSongSamplingRate->Create("0",60,40,29,11,Handle,0,myApp->Instance,hbm, bmpfont);

	tbHz = new WBmpTextBox();
	tbHz->Create("Hz",89,40,11,11,Handle,0,myApp->Instance,hbm, bmpfont);

	

	tbSongStereoMode = new WBmpTextBox();
	tbSongStereoMode->Create("",110,40,135,11,Handle,0,myApp->Instance,hbm, bmpfont);


	tbSongStatus = new WBmpTextBox();
	tbSongStatus->Create("Volume: 100",135,54,80,11,Handle,0,myApp->Instance,hbm, bmpfont);


	

	tbSongBitrate = new WBmpTextBox();
	tbSongBitrate->Create("360",15,54,18,11,Handle,0,myApp->Instance,hbm, bmpfont);

	tbkbps = new WBmpTextBox();
	tbkbps->Create("kbps",35,54,51,11,Handle,0,myApp->Instance,hbm, bmpfont);

	DeleteObject(hbm);




    

	mainForm->SetTimer(MAIN_TIMER,200);
	mainForm->SetTimer(VU_TIMER, 50);





	 if(mixer->GetControlID(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,
     						MIXERCONTROL_CONTROLTYPE_VOLUME,&MWaveID)) {

     	DWORD lv, rv;
        if(mixer->GetVolume(MWaveID, 0, 0, &lv, &rv)) {

        	sbLvolume->SetRange(100);
            sbRvolume->SetRange(100);
            sbLvolume->SetPos(100 - (lv*100 /65535));
            sbRvolume->SetPos(100 - (rv*100 /65535));
			

        }
     }



	player->EnableEqualizer(eq);
	player->SetEqualizerParam(0, eq_values, 10);
	//player->SetSfxParam(sfx, sfx_mode, (20 - sfx_delay) * 50, 12 - sfx_input, 12 - sfx_echo, 12 - sfx_output);
   

    return 0L;
}


LRESULT MainForm::OnDestroy()
{
// save settings into registry

	HKEY registry;
	DWORD reg_type = REG_DWORD;
	DWORD size = 4;
	DWORD dwDisposition;
	DWORD value = 0;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY,
						0, "", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0,
						&registry, &dwDisposition) == ERROR_SUCCESS)
	{
						
						
	

		RegSetValueEx(registry, "fftWindow", 0, reg_type, (unsigned char*) &c_nSpectrumWindow, size);
		RegSetValueEx(registry, "fftSpectrumType", 0, reg_type, (unsigned char*) &c_nSpectrumType, size);

		RegSetValueEx(registry, "fftDisplayMode", 0, reg_type, (unsigned char*) &g_FFTDisplayMode, size);
		
		RegSetValueEx(registry, "eqEnabled", 0, reg_type, (unsigned char*) &eq, size);
		RegSetValueEx(registry, "eqData", 0, REG_BINARY	, (unsigned char*) eq_values, 10 * sizeof(int));

		value = sbPreAmp->GetPos();
		RegSetValueEx(registry, "eqPreamp", 0, reg_type, (unsigned char*) &value, 4);


		value = sbInput->GetPos();
		RegSetValueEx(registry, "sfxInput", 0, reg_type, (unsigned char*) &value, 4);

		value = sbEcho->GetPos();
		RegSetValueEx(registry, "sfxEcho", 0, reg_type, (unsigned char*) &value, 4);

		value = sbOutput->GetPos();
		RegSetValueEx(registry, "sfxOutput", 0, reg_type, (unsigned char*) &value, 4);

		value = sbDelay->GetPos();
		RegSetValueEx(registry, "sfxDelay", 0, reg_type, (unsigned char*) &value, 4);

		RegSetValueEx(registry, "sfxEnabled", 0, reg_type, (unsigned char*) &sfx, 4);

		//TStreamStatus status;
		//player->GetStatus(&status);
		int mode = 1;

		RegSetValueEx(registry, "sfxMode", 0, reg_type, (unsigned char*) &mode, 4);

		
		

						
		RegCloseKey(registry);
	}

   
   TaskBarDeleteIcon(IconID);
 
    mainForm->KillTimer(MAIN_TIMER);
	mainForm->KillTimer(VU_TIMER);

	player->Close();

	delete vul;
	delete vur;
	DeleteObject(vubg);
	DeleteObject(vufg);


	
    
   
	delete btPrev;
	delete btPlay;
	delete btPause;
	delete btNext;
	delete btMpexe;
	delete btOpen;
	delete btMenu;
	delete btStop;
	delete btMinimize;
	delete btClose;
	delete sbSeek;
	delete sbLvolume;
	delete sbRvolume;
	delete btLock;
	delete btEq;
	delete btResetEQ;
	delete btEcho;
	delete btModeNext;
	delete btModePrev;
	delete btReverse;

	delete btSpectrumNext;
	delete btSpectrumPrev;
	delete tbSpectrumMode;

	delete btFFTWindowNext;
	delete btFFTWindowPrev;
	delete tbFFTWindow;


	delete sbDelay;
	delete sbEcho;
	delete sbInput;
	delete sbOutput;


	delete  sbEq1;
	delete  sbEq2;
	delete  sbEq3;
	delete  sbEq4;
	delete  sbEq5;
	delete  sbEq6;
	delete  sbEq7;
	delete  sbEq8;
	delete  sbEq9;
	delete  sbEq10;
	delete	sbPreAmp;



	delete  tbSongLength;
	delete  tbSongPosition;
	delete  tbSongName;
	delete  tbSongSamplingRate;
	delete  tbSongBitrate;
	delete  tbSongStereoMode;
	delete  tbSongStatus;
	delete  tbHz;
	delete  tbkbps;
	delete	tbMode;


	delete tooltip;





	delete bmpfont;

	
	DeleteObject(hbMain);
	DeleteObject(hbTitlebar);



	myApp->Terminate();

 	return 0L;
}






LRESULT MainForm::OnPaint()
{
	PAINTSTRUCT ps;
    HDC hdc;
    hdc = BeginPaint(Handle, &ps);
	RECT rcClient;
	GetClientRect(Handle,&rcClient);
    

    // draw background
    HDC hdcmem = CreateCompatibleDC(0);
	HBITMAP old = (HBITMAP )SelectObject(hdcmem, hbMain);


	BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, 
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top, hdcmem,
				ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);

	SelectObject(hdcmem, hbTitlebar);


    // draw titlebar

	if(ps.rcPaint.top < 14) {
		if(GetActiveWindow() != Handle) {
	

			BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, 
				ps.rcPaint.right - ps.rcPaint.left,
				14 - ps.rcPaint.top, hdcmem,
					ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
		}
		else {
	
					
			BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, 
			ps.rcPaint.right - ps.rcPaint.left,
			14 - ps.rcPaint.top , hdcmem,
				ps.rcPaint.left, 14 + ps.rcPaint.top,SRCCOPY);	
		}
	}

	SelectObject(hdcmem,old);
	DeleteDC(hdcmem);

    EndPaint(Handle, &ps);

    return 0L;
  

}


LRESULT MainForm::OnHScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle)
{
	
    switch(ScrollbarID) {
    	case SB_SEEK:	
        	switch(nScrollCode)
			{
    			case SB_THUMBTRACK:
        		{
        			BlockTimer = TRUE; // block main timer ( no repainting )
                   
                    char text[20];
					TStreamInfo pInfo;
					player->GetStreamInfo(&pInfo);
                    if(bElapsedTime)
					{
						unsigned int l = nPos;

						int h = l / 3600;
						l = l % 3600;
						int m = l / 60;
						l = l % 60;
						int s = l;

                    	sprintf(text,"%02i:%02i:%02i", h, m,s);
					}
                    else
					{
						unsigned int l = pInfo.Length.sec - nPos;

						int h = l / 3600;
						l = l % 3600;
						int m = l / 60;
						l = l % 60;
						int s = l;

                    	sprintf(text,"%02i:%02i:%02i",h, m, s);
					}


                    tbSongPosition->SetText(text,1);



        		}
        		break;

                case SB_THUMBPOSITION:
                {
					
					TStreamTime time;
					player->GetPosition(&time);

					if((unsigned int) nPos == time.sec)
					{
						BlockTimer = FALSE;
						break;
					}

					TStreamStatus status;
					player->GetStatus(&status);

                    if(status.fPlay)
                    {
						time.sec = nPos;
                    	player->Seek(tfSecond, &time, smFromBeginning);
						
                        player->Play();
					
                    }
                    else
                    {
						time.sec = nPos;
                    	player->Seek(tfSecond, &time, smFromBeginning);
						player->Pause();
                    }

					BlockTimer = FALSE;

                	

                }
                break;
            }
    }

    return 0L;
}

LRESULT MainForm::OnVScroll(DWORD ScrollbarID, int nScrollCode, int nPos, HWND scHandle)
{
	switch(ScrollbarID)
    {

		case SB_EQ1:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 1: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					
					break;
            }
			else if(nScrollCode == SB_THUMBPOSITION)
			{

				eq_values[0] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;
               
        }
        break;

		case SB_EQ2:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 2: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[1] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE; 
        }
        break;

		case SB_EQ3:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 3: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[2] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;  
        }
        break;

		case SB_EQ4:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
					sprintf(tmp,"Eq band 4: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[3] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;  
        }
        break;

		case SB_EQ5:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 5: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[4] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;   
        }
        break;

		case SB_EQ6:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 6: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[5] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;  
        }
        break;

		case SB_EQ7:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 7: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[6] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;  
        }
        break;

		case SB_EQ8:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 8: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[7] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;   
        }
        break;

		case SB_EQ9:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 9: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[8] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;   
        }
        break;

		case SB_EQ10:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq band 10: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
					break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				eq_values[9] = 12000 - nPos;
				player->SetEqualizerParam(12000 - sbPreAmp->GetPos(), eq_values, 10);

			}

            
            blockStatus = FALSE;   
        }
        break;

		case SB_PREAMP:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Eq preamp: %i",  (12000 - nPos) / 1000);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
				break;
            }

            else if(nScrollCode == SB_THUMBPOSITION) {
				player->SetEqualizerPreampGain(12000 - sbPreAmp->GetPos());

			}

            
            blockStatus = FALSE;   
        }
        break;


		case SB_ECHO:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Echo gain: %i",  12 - sbEcho->GetPos());
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
            }
            else {
				//MP3_STATUS status;
				//mp3->GetStatus(&status);
				//int mode = status.nSfxMode;
				//mp3->SetSfxParam(sfx, mode, (20 - sbDelay->GetPos()) * 50,(12 - sbInput->GetPos()), (12 - sbEcho->GetPos()), (12 - sbOutput->GetPos()));	
				
            	blockStatus = FALSE;
            }  

          
        }
        break;

		case SB_INPUT:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Input gain: %i",  12 - sbInput->GetPos());
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
            }
            else {
				//MP3_STATUS status;
				//mp3->GetStatus(&status);
				//int mode = status.nSfxMode;
				//mp3->SetSfxParam(sfx, mode, (20 - sbDelay->GetPos()) * 50,(12 - sbInput->GetPos()) , (12 - sbEcho->GetPos()) , (12 - sbOutput->GetPos()));	
			
            	blockStatus = FALSE;
            }  

          
        }
        break;

		case SB_OUTPUT:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	char tmp[20];
                	sprintf(tmp,"Output gain: %i",  12 - sbOutput->GetPos());
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);
            }
            else {
				//MP3_STATUS status;
				//mp3->GetStatus(&status);
				//int mode = status.nSfxMode;
				//mp3->SetSfxParam(sfx, mode, (20 - sbDelay->GetPos()) * 50,(12 - sbInput->GetPos()) , (12 - sbEcho->GetPos()) , (12 - sbOutput->GetPos()));	
			
            	blockStatus = FALSE;
            }  

          
        }
        break;



		case SB_DELAY:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                

                	char tmp[20];
                	sprintf(tmp,"Delay: %i ms",  (20 - sbDelay->GetPos()) * 50);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);

                
            }

            else {
				//MP3_STATUS status;
				//mp3->GetStatus(&status);
				//int mode = status.nSfxMode;
				//mp3->SetSfxParam(sfx, mode, (20 - sbDelay->GetPos()) * 50,(12 - sbInput->GetPos()) , (12 - sbEcho->GetPos()), (12 - sbOutput->GetPos()));	
			

            	blockStatus = FALSE;

            }  

          
        }
        break;
     	case SB_LVOLUME:
        {
			if(nScrollCode == SB_THUMBTRACK) {
                	if(bLockVolume)
                		sbRvolume->SetPos(nPos);

                	char tmp[20];
                	sprintf(tmp,"Volume: %i",  100 - nPos);
                	blockStatus = TRUE;
                	tbSongStatus->SetText(tmp,1);

					player->SetPlayerVolume((int) (100 - nPos), (int) (100 - sbRvolume->GetPos()));
            }

            else {
			
            	blockStatus = FALSE;

            }  

          
        }
        break;

        case SB_RVOLUME:
		
        	if(nScrollCode == SB_THUMBTRACK) {
            
				if(bLockVolume)
                	sbLvolume->SetPos(nPos);

                char tmp[20];
                sprintf(tmp,"Volume: %i", 100 - nPos );
                blockStatus = TRUE;
                tbSongStatus->SetText(tmp,1);

                  player->SetPlayerVolume((int) (100 - sbLvolume->GetPos()), (int) (100 - nPos));
            }
            else {
				
            	blockStatus = FALSE;

            }

        break;
/*
        case SC_MVOLUME:
		
        	if(nScrollCode == SB_THUMBTRACK) {
            	scList[3]->SetScrollPos(nPos);
                int pos = (655 - nPos) * 100;
                mixer->SetVolume(MVolumeID, 2, 0, pos, pos);
                char tmp[20];
                sprintf(tmp,"Volume: %i", ( 655 - nPos) * 100 /655);
                blockStatus = TRUE;
                tbList[5]->SetText(tmp);

            }
            else {
            	blockStatus = FALSE;

            }

        break;
*/

    }

    return 0L;
}

LRESULT MainForm::OnMouseMove(DWORD fwKeys, int X, int Y)
{
    if(fwKeys == MK_LBUTTON)
    {
        
    	POINT pt;
        GetCursorPos(&pt);

        int x,y;
        x = pt.x - moveMain.x; // new left
        y = pt.y - moveMain.y; // new top
    	MoveTo(x,y);
    }
	return 0L;
}

LRESULT MainForm::OnMouseDown(DWORD fwKeys,int X, int Y)
{
     if(fwKeys == MK_LBUTTON)
     {
          moveMain.x = X;
         moveMain.y = Y;
         SetCapture(Handle);
     }

	return 0L;
}

LRESULT MainForm::OnMouseUp(DWORD fwKeys,int X, int Y)
{
	ReleaseCapture();
	return 0L;
}

LRESULT MainForm::OnTrayIconNotify(DWORD Message, UINT iconID)
{
	switch (Message) {
     	case WM_LBUTTONDOWN:
        	RestoreFromTray(iconID);


        break;



    }



 	return 0L;
}

LRESULT MainForm::OnTimer(UINT idTimer)
{

    switch(idTimer) {
    	case MAIN_TIMER:
        {
			if(BlockTimer) {
				//MessageBeep(0);
    			return 0;
			}

				

			//BlockTimer = TRUE;

            char text[50];
			
            if(!blockStatus)
            {
            	TStreamStatus status;
				player->GetStatus(&status);
        	
        			if(!status.fPlay)
            			strcpy(text,"Stop   ");
					if(status.fPlay)
                		strcpy(text,"Playing");
					if(status.fPause)
                		strcpy(text,"Pause  ");
            	
        	
                tbSongStatus->SetText(text,1);
            }

  

			TStreamTime time;
			player->GetPosition(&time);
			sbSeek->SetPos(time.sec);

            sprintf(text, "%i", player->GetBitrate(0));
			
            tbSongBitrate->SetText(text,1);
			TStreamInfo info;
			player->GetStreamInfo(&info);

            if(bElapsedTime)
            	sprintf(text,"%02i:%02i:%02i", time.hms.hour, time.hms.minute, time.hms.second);

			else
			{
				unsigned int l = info.Length.sec - time.sec;

				int h = l / 3600;
				l = l % 3600;
				int m = l / 60;
				l = l % 60000;
				int s = l;
	

            	sprintf(text,"%02i:%02i", h, m, s);
			}

	
		
			tbSongPosition->SetText(text,1);
         
        }
        break;



		case VU_TIMER:
        {
			unsigned int left;
			unsigned int right;
			TStreamStatus status;
			player->GetStatus(&status);
        	if(status.fPlay) {
				
				player->GetVUData(&left,&right);

				vul->SetPos(left);
				vur->SetPos(right);

				player->DrawFFTGraphOnHWND(Handle, FFT_X, FFT_Y, FFT_DISPLAY_WIDTH, FFT_DISPLAY_HEIGHT);
			}
			else if(!status.fPlay) {
				left = 0;
				right = 0;
				vul->SetPos(left);
				vur->SetPos(right);

				player->DrawFFTGraphOnHWND(Handle, FFT_X, FFT_Y, FFT_DISPLAY_WIDTH, FFT_DISPLAY_HEIGHT);
			}
        }
        break;

    }

    return 0;
}

LRESULT MainForm::OnCommand(int wNotifyCode, int ControlID, HWND hWndControl)
{


	switch (ControlID) {
        // popup menu handle

		case M_ABOUT:
		{
		
			char message[] = "Mp3Exe Player ver.2.4  based on LibZPlay ver.2.02\n"
							"\n"
							"https://github.com/oddlf/mp3exe_libzplay\n"
							"http://www.inet.hr/~zcindori/mp3exe/\n"
							"http://www.inet.hr/~zcindori/libwmp3/\n"
							"\n"
							"This program is free software; you can redistribute it and/or modify it\n"
							"under the terms of the GNU General Public License as published by\n"
							"the Free Software Foundation; either version 2 of the License, or (at\n"
							"your option) any later version.\n"
							" \n"

							"This program is distributed in the hope that it will be useful, but\n"
							"WITHOUT ANY WARRANTY; without even the implied warranty of\n"
							"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
							"See the GNU General Public License for more details.";


			MessageBox(Handle, message, "About Mp3Exe player", MB_OK|MB_APPLMODAL);
		}
		break;



		case BT_LINEAR:
		{
			if(g_FFTDisplayMode == gsLogarithmic)
				g_FFTDisplayMode = gsLinear;
			else
				g_FFTDisplayMode = gsLogarithmic;

			player->SetFFTGraphParam(gpHorizontalScale, g_FFTDisplayMode);
		}
		break;

		case TB_FFT_WINDOW:
		case  BT_FFT_WINDOW_NEXT:
		{
			if(c_nSpectrumWindow == fwFlatTop)
				c_nSpectrumWindow = fwRectangular;
			else
				++c_nSpectrumWindow;

			player->SetFFTGraphParam(gpWindow, c_nSpectrumWindow);
			tbFFTWindow->SetText(g_szFFTWindow[c_nSpectrumWindow - 1], TRUE);	
		}
		break;

		case  BT_FFT_WINDOW_PREV:
		{
			if(c_nSpectrumWindow == fwRectangular)
				c_nSpectrumWindow = fwFlatTop;
			else
				--c_nSpectrumWindow;

			player->SetFFTGraphParam(gpWindow, c_nSpectrumWindow);
			tbFFTWindow->SetText(g_szFFTWindow[c_nSpectrumWindow - 1], TRUE);
		}
		break;

		case TB_SPECTRUM_MODE:
		case  BT_FFT_SPECTRUM_NEXT:
		{
			if(c_nSpectrumType == gtSpectrum)
				c_nSpectrumType = gtLinesLeftOnTop;
			else
				++c_nSpectrumType;

			player->SetFFTGraphParam(gpGraphType, c_nSpectrumType);
			tbSpectrumMode->SetText(g_szSpectrumModes[c_nSpectrumType], TRUE);
		}
		break;

		case  BT_FFT_SPECTRUM_PREV:
		{
			if(c_nSpectrumType == gtLinesLeftOnTop)
				c_nSpectrumType = gtSpectrum;
			else
				--c_nSpectrumType;

			player->SetFFTGraphParam(gpGraphType, c_nSpectrumType);
			tbSpectrumMode->SetText(g_szSpectrumModes[c_nSpectrumType], TRUE);
		}
		break;

		case TB_MODE:
		case BT_MODENEXT:
		{
			/*TStreamStatus status;
			player->GetStatus(&status);
			int mode = status.nSfxMode;
			mode++;
			if(mode > 9) mode = 0;
			mp3->SetSfxParam(sfx,mode, (20 - sbDelay->GetPos()) * 50,(12 - sbInput->GetPos()) , (12 - sbEcho->GetPos()) , (12 - sbOutput->GetPos()) );	
			tbMode->SetText(sModes[mode],1);
			*/

		}
		break;


		case BT_MODEPREV:
		{
		/*	MP3_STATUS status;
			mp3->GetStatus(&status);
			int mode = status.nSfxMode;
			mode--;
			if(mode < 0) mode = 9;
			mp3->SetSfxParam(sfx, mode, (20 - sbDelay->GetPos()) ,(12 - sbInput->GetPos()) , (12 - sbEcho->GetPos()) , (12 - sbOutput->GetPos()) );	
			tbMode->SetText(sModes[mode],1);
			*/

		}
		break;


		case BT_ECHO:
		{
		/*	if(sfx == 0)
				sfx = 1;
			else
				sfx = 0;
		
			btEcho->SetCheck(sfx);
			MP3_STATUS status;
			mp3->GetStatus(&status);
			int mode = status.nSfxMode;
			mp3->SetSfxParam(sfx, mode, (20 - sbDelay->GetPos()) * 50,(12 - sbInput->GetPos()) , (12 - sbEcho->GetPos()) , (12 - sbOutput->GetPos()) );	
			
			*/
		}
		break;


		case BT_EQ:
		{
			if(eq == 1)
				eq = 0;
			else
				eq = 1;
		
			btEq->SetCheck(eq);
			player->EnableEqualizer(eq);
		}
		break;

		case BT_RESETEQ:
		{
			memset(eq_values, 0, 10 * sizeof(int));

			player->SetEqualizerParam(0,eq_values, 10);

			sbEq1->SetPos(12000,1);
			sbEq2->SetPos(12000,1);
			sbEq3->SetPos(12000,1);
			sbEq4->SetPos(12000,1);
			sbEq5->SetPos(12000,1);
			sbEq6->SetPos(12000,1);
			sbEq7->SetPos(12000,1);
			sbEq8->SetPos(12000,1);
			sbEq9->SetPos(12000,1);
			sbEq10->SetPos(12000,1);
			sbPreAmp->SetPos(12000,1);
		}
		break;


	
		case BT_MPEXE:
		{
			SendMessage(mainForm->Handle, WM_COMMAND, MAKEWPARAM(M_CONVERT,0), 0);
		}
		break;

		case BT_MENU:
		{
			POINT pt;
			GetCursorPos(&pt);
			OnContextMenu(pt.x, pt.y, Handle);
			//UpdateWindow(mainForm->Handle);
			//InvalidateRect(mainForm->Handle,NULL,FALSE);

		}
		break;

		case M_EXTRACT:
		{
			player->Close();
			char filename[MAX_PATH];
			GetModuleFileName(NULL,filename,MAX_PATH);

			WFile* file = new WFile;
			if(!file->Open(filename,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING)) {
				MessageBox(mainForm->Handle,"Can't open file!","Error",MB_APPLMODAL|MB_ICONSTOP);
					return 0;
			}

				DWORD lpLast;
				DWORD lpCheck;
				DWORD lpCheck1;

				DWORD dwRead;
				DWORD dwSeek = 0;
				
				file->Seek(-4,FILE_END);
				file->Read(&lpLast,4,dwRead);
				if(lpLast != 0 && lpLast < file->GetFileSize()) {
					file->Seek(-8,FILE_END);	
					file->Read(&lpCheck,4,dwRead);
					file->Seek(-1 * ( lpLast + 12),FILE_END);
						
					file->Read(&lpCheck1,4,dwRead);
					if(lpCheck1 != 0 && lpCheck1 != 0xFFFFFFFF && lpCheck1 == lpCheck) {
						dwSeek = file->GetFileSize() - lpLast - 8;
						
					}
					else {
						lpLast = 0;
						dwSeek = 0;
					}
				}
				else {
					lpLast = 0;
					dwSeek = 0;
				}

				char outfilename[MAX_PATH];

				if(dwSeek != 0) {

					WSaveDialog* savedlg = new WSaveDialog;
					if(savedlg->Execute(Handle, "Mp3 file\0*.mp3\0\0", 1, 0, "Save mp3...","","mp3" ))
					{
						strcpy(outfilename,savedlg->GetFileName());
						delete savedlg;

						DWORD dwRead;
						char text[MAX_PATH * 2];
						// open out file
						WFile* outfile;   
						outfile = new WFile;
						if(!outfile->Open(outfilename,GENERIC_WRITE	,0,CREATE_ALWAYS)) {		
							sprintf(text,"Can't open file: %s for writing",outfilename);
							MessageBox(Handle,text, "Error",MB_ICONSTOP	);
							delete file;
							delete outfile;
							break;
					
						}

						file->Seek(0,dwSeek);

						char* mp3buf = (char*) malloc (lpLast);
						if(!mp3buf) {
							MessageBox(Handle,"Can't allocate memory for exe file.", "Error",MB_ICONSTOP	);
							delete file;
							delete outfile;
							break;			
						}

			

						file->Read(mp3buf,lpLast, dwRead);


						// write exe in file
						outfile->Write(mp3buf,lpLast, dwRead);
						delete outfile;

					}
					else {
						delete savedlg;
						delete file;
						break;
					}	
				}
				else {
					MessageBox(mainForm->Handle," No embedded mp3.","Error",MB_APPLMODAL|MB_ICONSTOP);	
				}

				delete file;
				


		}
		break;

		case M_EMBEDDED:
			SendMessage(mainForm->Handle, WM_COMMAND, MAKEWPARAM(MESSAGE_PLAY_EMBEDED,0 ), 0);
		break;

		case MESSAGE_PLAY_EMBEDED:
		{
			if(reverse)
				player->ReverseMode(0);

			reverse = 0;

			player->Close();
			char filename[MAX_PATH];
			GetModuleFileName(NULL,filename,MAX_PATH);

			WFile* file = new WFile;
			if(!file->Open(filename,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING)) {
				MessageBox(mainForm->Handle,"Can't open file!","Error",MB_APPLMODAL|MB_ICONSTOP);
					return 0;
			}

				DWORD lpLast;
				DWORD lpCheck;
				DWORD lpCheck1;

				DWORD dwRead;
				DWORD dwSeek = 0;
				
				file->Seek(-4,FILE_END);
				file->Read(&lpLast,4,dwRead);
				if(lpLast != 0 && lpLast < file->GetFileSize()) {
					file->Seek(-8,FILE_END);	
					file->Read(&lpCheck,4,dwRead);
					file->Seek(-1 * ( lpLast + 12),FILE_END);
						
					file->Read(&lpCheck1,4,dwRead);
					if(lpCheck1 != 0 && lpCheck1 != 0xFFFFFFFF && lpCheck1 == lpCheck) {
						dwSeek = file->GetFileSize() - lpLast - 8;
						
					}
					else {
						lpLast = 0;
						dwSeek = 0;
					}
				}
				else {
					lpLast = 0;
					dwSeek = 0;
				}

				delete file;

				if(dwSeek != 0) {

					if(!player->OpenFile(filename,sfAutodetect)) {
						MessageBox(mainForm->Handle,player->GetError(),"Error",MB_APPLMODAL|MB_ICONSTOP);
						return 0;
					}
       
					Start(filename);
					player->Play();
				
					
				}
				else {

					Start(filename);
					tbSongName->SetText("No mp3 in exe file.",1);

				}
		
				

		}
		break;


		case MESSAGE_PLAY_ARGUMENT:
		{
			if(reverse)
				player->ReverseMode(0);

			reverse = 0;

			player->Close();
			
			if(!player->OpenFile(mp3filename,sfAutodetect)) {
						MessageBox(mainForm->Handle,player->GetError(),"Error",MB_APPLMODAL|MB_ICONSTOP);
				return 0;
			}
       
			Start(mp3filename);
			player->Play();
		

		} 
		break;
 
        case M_EXIT:
        	Close();
        break;


        case M_ONTOP:
        	bAlwaysOnTop = !bAlwaysOnTop;
            if(bAlwaysOnTop)
            	mainForm->AlwaysOnTop(TRUE);
            else
            	mainForm->AlwaysOnTop(FALSE);
        break;

      

        case M_MINTOTRAY:
        	bMinimizeToTray = !bMinimizeToTray;
        break;

        case M_OPEN:
            SendMessage(Handle, WM_COMMAND,MAKEWPARAM(BT_OPEN,0), MAKELPARAM(0,0));
        break;

        case M_TOOLTIPS:
        	bShowTooltip = !bShowTooltip;
            tooltip->Activate(bShowTooltip);
        break;

        case M_SHORTCUTS:
         	bKeyShortcuts = !bKeyShortcuts;
        break;



        case M_ESCMIN:
        	bEscKeyMinimize = !bEscKeyMinimize;
        break;





        case M_TIMEELAPSED:
        	bElapsedTime = !bElapsedTime;
        break;




    	case  BT_PREV:
        {
		
        	
			TStreamStatus status;
			player->GetStatus(&status);
			TStreamTime time;
			time.sec = 5;
        	if(status.fPlay)
            {
                    	player->Seek(tfSecond, &time, smFromCurrentBackward);
						player->Play();
					
            }
            else
            {
                    	player->Seek(tfSecond, &time, smFromCurrentBackward);
            }

        }
        break;

        case  BT_PLAY:
        {
			if(reverse)
				player->ReverseMode(0);

			reverse = 0;
			player->Play();
		
			Pause = FALSE;
        }
        break;

		case  BT_REVERSE:
        {
			reverse = 1;
			player->ReverseMode(1);


			TStreamStatus status;
			player->GetStatus(&status);
			if(!status.fPlay)
			{
				TStreamTime time;
				player->GetPosition(&time);
				if(time.ms == 0)
				{
					time.ms = 0;
					player->Seek(tfMillisecond, &time, smFromEnd);
				}

					
				player->Play();
				Pause = FALSE;
			}
        }
        break;


        case  BT_PAUSE:
        {
            if(Pause) {
            	player->Resume();
                Pause = FALSE;
            }
            else {
                Playing = FALSE;
         		player->Pause();
                Pause = TRUE;
            }
        }
        break;
        case  BT_STOP:
        {

            player->Stop();
        }
        break;
        case  BT_NEXT:
        {
            	TStreamStatus status;
			player->GetStatus(&status);
			TStreamTime time;
			time.sec = 5;
        	if(status.fPlay)
            {
                    	player->Seek(tfSecond, &time, smFromCurrentForward);
						player->Play();
					
            }
            else
            {
                    	player->Seek(tfSecond, &time, smFromCurrentForward);
            }
         	
        }
        break;
        case  BT_OPEN:
        {
			reverse = 0;
			
        	WOpenDialog* opendlg = new WOpenDialog;
            if(opendlg->Execute(Handle, "Music File\0*.mp3;*.mp2;*.mp1;*.ogg;*.flac;*.oga;*.ac3;*.;*.aac;*.wav\0All Files\0*.*\0\0", 1, 0, "Open music file",	NULL))
            {
                strcpy(mp3filename,opendlg->GetFileName());
				delete opendlg;
				SendMessage(mainForm->Handle, WM_COMMAND, MAKEWPARAM(MESSAGE_PLAY_ARGUMENT,0 ), 0);
 
			} 
			else {
				delete opendlg;
				break;
			}
   
        }
        break;




		case  M_CONVERT:
        {
			char mp3filename[MAX_PATH];
			char exefilename[MAX_PATH];
			char outfilename[MAX_PATH];

			::GetModuleFileName(GetModuleHandle(0),(char*) &exefilename,MAX_PATH);
            
		
        	WOpenDialog* opendlg = new WOpenDialog;
            if(opendlg->Execute(Handle, "Mp3 File\0*.mp3\0\0", 1, 0, "Open mp3 file",	NULL))
            {
                strcpy(mp3filename,opendlg->GetFileName());
				delete opendlg;
			} 
			else {
				delete opendlg;
				break;
			}

			char tmp[2 * MAX_PATH];
			sprintf(tmp,"%s.exe",mp3filename);

			WSaveDialog* savedlg = new WSaveDialog;
			if(savedlg->Execute(Handle, "Exe file\0*.exe\0\0", 1, 0, "Save mp3 as exe",tmp,"exe" ))
			{
				strcpy(outfilename,savedlg->GetFileName());
				delete savedlg;
			}
			else {
				delete savedlg;
				break;

			}

		
			//GetModuleFileName(NULL,exefilename,MAX_PATH);

	

			DWORD dwRead;
			char text[MAX_PATH * 2];
			// open out file
			WFile* outfile;   
			outfile = new WFile;
			if(!outfile->Open(outfilename,GENERIC_WRITE	,0,CREATE_ALWAYS)) {		
				sprintf(text,"Can't open file: %s for writing",outfilename);
				MessageBox(Handle,text, "Error",MB_ICONSTOP	);
				delete outfile;
				break;
					
			}

		
			WFile* exefile = new WFile;
			if(!exefile->Open(exefilename,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING)) {	
				sprintf(text,"Can't open file: %s",exefilename);
				MessageBox(Handle,text, "Error",MB_ICONSTOP	);
				delete exefile;
				delete outfile;
				break;

			}

			DWORD Last;
			DWORD Check;
			DWORD Check1;

		
			DWORD dwSeek = 0;
			
			exefile->Seek(-4,FILE_END);
			exefile->Read(&Last,4,dwRead);
			if(Last != 0 && Last < exefile->GetFileSize()) {
				exefile->Seek(-8,FILE_END);	
				exefile->Read(&Check,4,dwRead);
				exefile->Seek(-1 * ( Last + 12),FILE_END);
					
				exefile->Read(&Check1,4,dwRead);
				if(Check1 == Check) {
					dwSeek = Last + 12;
				}
			}


			exefile->Seek(0,FILE_BEGIN);



			DWORD exe_size = exefile->GetFileSize() - dwSeek;

			char* exebuf = (char*) malloc (exe_size);
			if(!exebuf) {
				MessageBox(Handle,"Can't allocate memory for exe file.", "Error",MB_ICONSTOP	);
				delete exefile;
				delete outfile;
				break;			
			}

			

			exefile->Read(exebuf,exe_size, dwRead);


			// write exe in file
			outfile->Write(exebuf,exe_size, dwRead);
			// write delimiter, check
			DWORD check = MAKELONG(LOWORD(GetTickCount()),LOWORD(GetTickCount()));
		
			outfile->Write(&check,4, dwRead);
			free(exebuf);
			delete exefile;
						
			
			// open mp3 file
            WFile* mp3file;
			mp3file = new WFile;
			if(!mp3file->Open(mp3filename,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING)) {
				sprintf(text,"Can't open file: %s", mp3filename);
				MessageBox(Handle,text, "Error",MB_ICONSTOP	);
				delete mp3file;
				delete outfile;
				break;
			}

			DWORD mp3_size = mp3file->GetFileSize();

			// read mp3 file into memory
			char* mp3buf = (char*) malloc (mp3_size);
			if(!mp3buf) {
				MessageBox(Handle,"Can't allocate memory for mp3 file.", "Error",MB_ICONSTOP	);
				delete mp3file;
				delete outfile;
				break;
					
			}
			
				
			mp3file->Read(mp3buf, mp3_size, dwRead);
			delete mp3file;		
			outfile->Write(mp3buf, mp3_size, dwRead);
			// write delimiter, check
			outfile->Write(&check,4, dwRead);
			outfile->Write(&mp3_size,4, dwRead);


			delete outfile;
			free(mp3buf);
				
		
                
           
        }
        break;

        


        case  BT_LOCKVOLUME:
        {
		
        	bLockVolume = !bLockVolume;
            if(bLockVolume) {
            	int l = sbLvolume->GetPos();
            	int r = sbRvolume->GetPos();
            	int m = ( l + r ) / 2;
            	sbLvolume->SetPos(m);
            	sbRvolume->SetPos(m);
            	int vol = 100 - m;
                player->SetPlayerVolume(vol, vol);
            }

			btLock->SetCheck(bLockVolume);

        }
        break;

     



        case  BT_MINIMIZE:
        {
            if(bMinimizeToTray) {


            	MinimizeToTray(1,LoadIcon(myApp->Instance, MAKEINTRESOURCE(SMALL_ICON)),
			 		"Mp3-Exe Player");

            }
            else {
        		Minimize();
                

            }
        }
        break;

        case  BT_CLOSE:
        {
            Close();
        }
        break;

        case  TB_POSITION:
        {
        	bElapsedTime = !bElapsedTime;
        }
        break;
       
    }

    return 0;
}

LRESULT MainForm::OnActivate()
{
	RECT rcTitlebar;
	GetClientRect(Handle,&rcTitlebar);
	rcTitlebar.bottom = 14;
	HBITMAP hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTCLOSE));
	btClose->SetBitmap(hbm);
	DeleteObject(hbm);

	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMINIMIZE));
	btMinimize->SetBitmap(hbm);
	DeleteObject(hbm);



	InvalidateRect(Handle,&rcTitlebar,FALSE);


    return 0;

}

LRESULT MainForm::OnDeactivate()
{
	RECT rcTitlebar;
	GetClientRect(Handle,&rcTitlebar);
	rcTitlebar.bottom = 14;
	HBITMAP hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTCLOSE1));
	btClose->SetBitmap(hbm);
	DeleteObject(hbm);
	hbm = LoadBitmap(myApp->Instance, MAKEINTRESOURCE(BMP_BTMINIMIZE1));
	btMinimize->SetBitmap(hbm);
	DeleteObject(hbm);

	InvalidateRect(Handle,&rcTitlebar,FALSE);
    return 0;

}

LRESULT MainForm::OnKeyUp(int VirtKey, int KeyData)
{

    switch (VirtKey) {
    	case 0x58:	// X  play

			NotifyButton(btPlay,FALSE);
        break;

        case 0x43:	// C  pause
        	NotifyButton(btPause,FALSE);
        break;
        case 0x56:	// V stop
        	NotifyButton(btStop,FALSE);
        break;

        case 0x59: 	// Y  previous
        	NotifyButton(btPrev,FALSE);
        break;

        case 0x5A: 	// Z  previous
        	NotifyButton(btPrev,FALSE);
        break;

        case 0x42:	// B next
        	NotifyButton(btNext,FALSE);
        break;





    }
    
 	return 0;
}

LRESULT MainForm::OnKeyDown(int VirtKey, int KeyData)
{

    switch (VirtKey) {
    	case 0x58:	// X  play

			NotifyButton(btPlay,TRUE);
        break;

        case 0x43:	// C  pause
        	NotifyButton(btPause,TRUE);
        break;
        case 0x56:	// V stop
        	NotifyButton(btStop,TRUE);
        break;

        case 0x59: 	// Y  previous
        	NotifyButton(btPrev,TRUE);
        break;

        case 0x5A: 	// Z  previous
        	NotifyButton(btPrev,TRUE);
        break;

        case 0x42:	// B next
        	NotifyButton(btNext,TRUE);
        break;

        case VK_ESCAPE: // escape key
        // simulate click on MINIMIZE BUTTON
        if(bEscKeyMinimize)
        	SendMessage(Handle,WM_COMMAND,MAKEWPARAM(BT_MINIMIZE,BN_CLICKED),(LPARAM)NULL);
        return 0;




    }
  
 	return 0;
}





BOOL NotifyButton(WBmpButton *button,BOOL down)
{

	if(!bKeyShortcuts)
    	return 0;
	

    UINT message;
    if(down) {
    	message = WM_LBUTTONDOWN;
        button->SetCheck(TRUE);
    }
    else {
    	message=WM_LBUTTONUP;
        button->SetCheck(FALSE);
    }

    SendMessage(button->Handle,message,MK_LBUTTON,
    	MAKELPARAM(0,0));

    button->Redraw();

    return TRUE;

}





int StrToRect(char *str, RECT *rect)
{
    LONG *data;
    data = ( LONG *) rect;
	char *start;
    int i = 0;
    start = strtok(str, ",");
    if(!start) {
    	data[i] = atoi(str);
        return -1;
    }
    else {
    	data[i] = atoi(start);
        while(TRUE) {
        	i++;
        	start = strtok(NULL, ",");
            if(start)
            	data[i] = atoi(start);
            else
            	return -1;
        }
    }
}


int StrToRect(char *str, RECT *rc, int num)
{
    char *p;
    LONG *array = (LONG *) rc;
    p = strtok(str, ",");

    if(!p) {
    	array[0] = atoi(str);
        return 1;
    }
    else
    	array[0] = atoi(p);

	for ( int i = 1; i < num; i++) {
    	p = strtok(NULL, ",");
        if(p)
        	array[i] = atoi(p);
        else
        	return i;
    }

    return 0;
}









LRESULT MainForm::OnContextMenu(int X, int Y, HWND Handle)
{
     HMENU hMenu = LoadMenu(myApp->Instance, MAKEINTRESOURCE (MAIN_MENU));
     hmenuTrackPopup = GetSubMenu(hMenu, 0);
     if(bAlwaysOnTop)
     	CheckMenuItem(hmenuTrackPopup, M_ONTOP, MF_CHECKED);
     if(bMinimizeToTray)
     	CheckMenuItem(hmenuTrackPopup, M_MINTOTRAY, MF_CHECKED);
     if(bShowTooltip)
     	CheckMenuItem(hmenuTrackPopup, M_TOOLTIPS, MF_CHECKED);
     if(bKeyShortcuts)
     	CheckMenuItem(hmenuTrackPopup, M_SHORTCUTS, MF_CHECKED);

     if(bEscKeyMinimize)
     	CheckMenuItem(hmenuTrackPopup, M_ESCMIN, MF_CHECKED);


     if(bElapsedTime)
     	CheckMenuItem(hmenuTrackPopup, M_TIMEELAPSED, MF_CHECKED);



	 TrackPopupMenu(hmenuTrackPopup, TPM_LEFTALIGN|TPM_LEFTBUTTON, X, Y, 0, mainForm->Handle, NULL);

     
    DestroyMenu(hMenu);
    return 0;
}


BOOL Start(char* filename)
{
	char listBoxEntry[MAX_PATH];
    char fileTitle[MAX_PATH];

    GetFileTitle(filename, fileTitle, MAX_PATH);

    sprintf(listBoxEntry, "%s",  fileTitle);

	TID3Info id3;
	if(player->LoadID3(id3Version2, &id3))
    {
		sprintf(listBoxEntry, "%s - %s", id3.Artist, id3.Title);

    }
	else if(player->LoadID3(id3Version1, &id3))
    {
        	sprintf(listBoxEntry, "%s - %s", id3.Artist, id3.Title);

    }
	



    tbSongName->SetText(listBoxEntry,1);

	TStreamInfo info;
	player->GetStreamInfo(&info);




    sbSeek->SetRange(info.Length.sec);

 

    
    char text[100];
    
    sprintf(text,"%02i:%02i:%02i", info.Length.hms.hour, info.Length.hms.minute , info.Length.hms.second );
    tbSongLength->SetText(text,1);


    sprintf(text, "%i", info.SamplingRate);
    tbSongSamplingRate->SetText(text,1);

/*	char *channel_ver = "";

	switch(info.nChannelMode)
	{
		case MODE_DUAL_CHANNEL:
			channel_ver = "Dual channel (Stereo)";
		break;

		case MODE_JOINT_STEREO:
			channel_ver = "Joint stereo (Stereo)";
		break;

		case MODE_STEREO:
			channel_ver = "Stereo";
		break;

		case MODE_SINGLE_CHANNEL:
			channel_ver = "Single channel (Mono)";
		break;
	}



	tbSongStereoMode->SetText(channel_ver, 1);*/


	if(info.VBR)
		tbkbps->SetText("kbps VBR",1);
	else
		tbkbps->SetText("kbps CBR",1);


    return TRUE;


}


int MainForm::OnMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD &dwReturn)
{
	if(msg == WM_COPYDATA)
	{
		COPYDATASTRUCT *data = (COPYDATASTRUCT*) lParam;
		char* filename = (char*) data->lpData;
			if(filename)
			{
				strcpy(mp3filename, filename);

				player->Close();
				if(!player->OpenFile(filename,sfAutodetect))
				{
					MessageBox(mainForm->Handle,player->GetError(),"Error",MB_APPLMODAL|MB_ICONSTOP);
					return 0;
				}

				if(reverse)
					player->ReverseMode(0);

					reverse = 0;
       
				Start(mp3filename);
				player->Play();
			}
		return 0;
	}


	return 0;
}



BOOL MakeWinTrans2000(HWND hWnd, BYTE alpha) 
{
#ifndef WS_EX_LAYERED 
	#define WS_EX_LAYERED           0x00080000 
	#define LWA_ALPHA               0x00000002 
#endif // ndef WS_EX_LAYERED 
typedef BOOL (WINAPI *SETLAYERED_FUNC)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags); 

	HMODULE hUser32 = GetModuleHandle("USER32.DLL"); 
	SETLAYERED_FUNC fSetLayeredWindowAttributes; 

	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED); 
	fSetLayeredWindowAttributes = (SETLAYERED_FUNC)GetProcAddress(hUser32, "SetLayeredWindowAttributes"); 

	if (fSetLayeredWindowAttributes != NULL) {
		fSetLayeredWindowAttributes(hWnd, NULL, alpha, LWA_ALPHA); 
		return TRUE;
	} else {
		return FALSE;
	}
}
