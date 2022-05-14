/*

Shibatch Super Equalizer ver 0.03 for winamp ( modified version )
written by Naoki Shibata  shibatch@users.sourceforge.net

This is modified version of Shibatch Super Equalizer
Some parts are excluded from original version and code is modified
to compile on free Borland commandline compiler 5.5

Date of modification: 01 April, 2009.
 

Shibatch Super Equalizer is a graphic and parametric equalizer plugin
for winamp. This plugin uses 16383th order FIR filter with FFT algorithm.
It's equalization is very precise. Equalization setting can be done
for each channel separately.

Processes of internal equalizer in winamp are actually done by each
input plugin, so the results may differ for each input plugin.
With this plugin, this problem can be avoided.

This plugin is optimized for processors which have cache equal to or
greater than 128k bytes(16383*2*sizeof(float) = 128k). This plugin
won't work efficiently with K6 series processors(buy Athlon!!!).

Do not forget pressing "preview" button after changing setting.

http://shibatch.sourceforge.net/

***

  This program(except FFT part) is distributed under LGPL. See LGPL.txt for
details.

  FFT part is a routine made by Mr.Ooura. This routine is a freeware. Contact
Mr.Ooura for details of distributing licenses.

http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html

*/


#ifndef _WEQU_Z_
#define _WEQU_Z_


#define DEF_NBANDS_POINTS 9


class  paramlistelm {
public:
	class paramlistelm *next;

	char left;
	char right;
	float lower;
	float upper;
	float gain;
	float gain2;
	int sortindex;

	paramlistelm();
	~paramlistelm();


};

class  paramlist {
public:
	class paramlistelm *elm;
	paramlist();
	~paramlist();

};

typedef float REAL;



typedef struct {

	REAL *lires;
	REAL *lires1;
	REAL *lires2;
	REAL *rires;
	REAL *rires1;
	REAL *rires2;
	REAL *irest;
	REAL *fsamples;
	volatile int chg_ires;
	volatile int cur_ires;
	int winlen;
	int winlenbit;
	int tabsize;
	int nbufsamples;
	short *inbuf;
	REAL *outbuf;
	int weq_enable;
	int dither;
	REAL *ditherbuf;
	int ditherptr;
	float hm1;
	float hm2;
	int ipsize;
	int wsize;
	int *ip;
	REAL *w;
	REAL *internal_band_points; // internal bands pointer
	int internal_band_points_num;	// internal bands num

	REAL *user_band_points; // user bands
	int user_band_points_num;	// user bands num

	int ready;

} EQ_INTER_PARAM;

void rdft(int n, int isgn, REAL *a, int *ip, REAL *w);
void ddct(int n, int isgn, REAL *a, int *ip, REAL *w);
void dfct(int n, REAL *a, REAL *t, int *ip, REAL *w);

void equ_init(EQ_INTER_PARAM *inter_param, int wb);
void equ_makeTable(EQ_INTER_PARAM *inter_param, REAL *lbc,REAL *rbc, paramlist *param, REAL fs);
void equ_quit(EQ_INTER_PARAM *inter_param);
void equ_clearbuf(EQ_INTER_PARAM *inter_param, int srate);
int equ_modifySamples(EQ_INTER_PARAM *inter_param, char *buf,int nsamples,int nch,int bps);
unsigned int equ_latency(EQ_INTER_PARAM *inter_param);
void process_param(EQ_INTER_PARAM *inter_param, REAL *bc, paramlist *param, paramlist &param2, REAL fs, int ch);





#endif
