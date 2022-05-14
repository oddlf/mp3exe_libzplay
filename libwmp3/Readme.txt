LIBWMP3 (Win32) 

  Version 2.4, April 2009
  Author: Zoran Cindori
  Email: zcindori@inet.hr
  Web: http://www.inet.hr/~zcindori/libwmp3/

This is multimedia library for playing mp3 and mp2 files.
It has integrated LIBMAD 0.15.1b mp3 decoder http://www.underbit.com/products/mad/
and you don't need additional library or dll etc. The simplest way to play mp3 files in your
C++ projects. Just 3 lines of C++ code and you are playing mp3 file.

LIBWMP3 has CWMp3 class interface for using in C++ projects. All member functions in this interface
are using __stdcall calling convention.
It has also C interface with standard data types and __stdcall calling convention for Visual Basic, Delphi and C#

All this interfaces are in libwmp3.dll.

Features:
- REVERSE PLAYING ( you can play mp3 backward in real time, "real" backward playing without predecoding in PCM)
- support Layer1, Layer2 and Layer3.
- support free format bitstream ( e.g. bitrate over 320 kbps )
- read ID3v1 and ID3v2 (2.2, 2.3, 2.4) tags
- built in internal equalizer ( 10 band based on mp3 decoding engine )
- built in external Shibatch Super Equalizer ver 0.03 ( 10 band precise FFT engine )
- user can specify own equalizer bands in external Shibatch Super Equalizer ( add more bands, set bands frequencies ... )
- built in echo and reverb sound processor
- loop playing ( you can play just one portion of mp3 file, repeat this portion ... )
- channel mixer ( you can mix left and right chanel, convert stereo to mono ... )
- fade in processor ( fade in volume in specified interval)
- fade out processor ( fade out volume in specified interval)
- internal volume control ( without affecting wave out device volume )
- function for pulling VU meter data ( you can simply create  VU meter )
- function for pulling FFT data ( you can simply build FFT spectrum analyzer )
- callback messages ( this class will inform you about events like  play, stop, fade, ...)
- access to decoded PCM data throug callback message
- play data from disk files
- play data from exe resource
- play data from allocated memory
- play managed streams ( push small memory chunks into stream while song is playing, for download streams )
- selecting wave output ( for multiple waveout devices )
- easy play, pause, seek ...
- C interface functions  for Visual Basic,  Borland Delphi and C#.

  With this library you can easy play mp3 with VB.NET, C#, Delphi, C/C++ and any other language capable
  to call exported functions from Dynamic Link Libraries ( Dll ) with __stdcall calling convention.


In C++ you can link LIBWMP3 as static library with your project and you don't need libwmp3.dll.
Or you can link with libwmp3.dll and then you need to distribute libwmp3.dll with your application.

In VB.NET, Delphi and C# you can only use libwmp3.dll as dynamic library and you need to distribute
libwmp3.dll with your application. 


NOTE:
Documentation for VB.NET, Delphi and C# was generated from C++ documentation. Function declarations
are tested and there is all OK. But comments, examples and parametes explanations can contain C++ syntax code
or some other strange expressions because I didn't read all this. I just adapted function declarations.

============================================================================================
============================================================================================
How to play mp3 in C++ ?
============================================================================================


 /* Example 1: ( using CWMp3  interface) */


#include "libwmp3.h"

/* create class instance */
CWMp3* mp3 = CreateCWMp3();

/* open file from disk */
mp3->OpenFile("mysong.mp3",800, 0, 0);

mp3->Play();

/* ... wait here in some loop */

MP3_STATUS status;
mp3->GetStatus(&status);
while(status.fPlay)
{
    Sleep(200);
    mp3->GetStatus(&status);
}



/* destroy class instance */
mp3->Release();


============================================================================================


/* Example 2: (using C interface) */


#include "libwmp3.h"

/* create class instance */
char *myclass = Mp3_Initialize(); 


/* open file from disk */
Mp3_OpenFile(myclass, "mysong.mp3", 800, 0,0);		
	

Mp3_Play(myclass);

/* ... wait here in some loop */

MP3_STATUS status;
Mp3_GetStatus(myclass, &status);
while(status.fPlay)
{
    Sleep(200);
    Mp3_GetStatus(myclass, &status);

}



/* destroy class instance */
Mp3_Destroy(myclass); 



============================================================================================
How to play mp3 in C# ?
============================================================================================

Add into your file declarations for  interface from .\c_sharp\libwmp3.cs

For now, you can use just C interface functions. Class interface isn't available for C#.
I don't know how to convert C++ abstract class (interface) to C# interface.
If somebody knowns, please contact me.


This interface is using unsafe code.
If you use Microsoft Visual Studio you should set to true 'Allow unsafe code block' option in project property window.


/* Example: (using C interface) */

============================================================================================

#include "libwmp3.h"

/* create class instance */
uint myclass = Mp3_Initialize(); 


/* open file from disk */
Mp3_OpenFile(myclass, "mysong.mp3", 800, 0,0);	
		

Mp3_Play(myclass);

/* ... wait here in some loop */

unsafe
{
	MP3_STATUS status;
	Mp3_GetStatus(myclass, &status);
	while(status.fPlay)
	{
	    Sleep(200);
	    Mp3_GetStatus(myclass, &status);
	
	}
}



/* destroy class instance */
Mp3_Destroy(myclass); 


// You need to distribute libwmp3.dll with your application.

============================================================================================
How to play mp3 in Visual Basic
============================================================================================


Add into your file declarations for  interface from .\vb_net\libwmp3.vb

For now, you can use just C interface functions. Class interface isn't available for VB.NET.
I don't know how to convert C++ abstract class (interface) to VB.NET interface.
If somebody knowns, please contact me.

============================================================================================

'EXAMPLE:

Dim myclass As Integer

' get class instance
myclass = Mp3_Initialize()

' open file from disk
Mp3_OpenFile(myclass, "mysong.mp3", 1000, 0, 0)

Mp3_Play(myclass)


' ... wait here in some loop 

Dim status As MP3_STATUS
Mp3_GetStatus(myclass, status)
while status.fPlay > 0

    ' wait here, a'm not expert with VB.NET, is there some Sleep function ???
   Mp3_GetStatus(myclass, status)

End While

'destroy class instance 
Mp3_Destroy(myclass); 


' You need to distribute libwmp3.dll with your application.


============================================================================================
How to play mp3 in Borland Delphi
============================================================================================


Add into your PROJECT  CWMp3 interface from .\delphi\libwmp3.pas
This file contains CWMp3 class interface, structures and constants.

NOTE: DON'T ADD, DELETE OR CHANGE ORDER OF MEMBER FUNCTIONS IN THIS CLASS BECAUSE THIS IS INTERFACE TO
      LIBWMP3.DLL. IF YOU CHANGE ORDER OF FUNCTIONS IN CWMP3 CLASS INTERFACE YOU WILL CALL
      WRONG FUNCTIONS IN DLL WITH WRONG PARAMETERS AND THIS WILL BE CATASTROPHE :-)

============================================================================================

// EXAMPLE

var
  mp3:CWMp3;
  status : MP3_STATUS;

begin
  mp3 := CreateCWMp3();
  mp3.OpenFile('mysong.mp3', 700, 0, 0);
  mp3.Play;

// ... wait here in some loop 

  mp3.GetStatus(Addr(status));

  while status.fPlay <> 0 do
  begin
     Sleep(200);
     mp3.OpenFile('mysong.mp3', 700, 0, 0);
  end;


  // destroy class instance
  mp3.Release();
end


============================================================================================
============================================================================================


*********************************************************************************************


This class is tested on:

Visual C++ 6.0 Standard Edition 
C++BUILDER COMMAND LINE TOOLS 5.5 
MinGW compiler 
Dev-C++ 
Microsoft Visual Basic 2008 Express Edition 
Microsoft Visual C# 2008 Express Edition 
Microsoft Visual C++ 2008 Express Edition 
Delphi 7.0 


HISTORY of the LIBWMP3 - WMP3 C++ class
-----------------------------

  Version 2.4                     April 2009
  ----------------------------------------------
  - fixed crash bug in ID3v2.4 functions ( crash if you try to load unsynchronized frame)
  - fixed handle leak when disabling external equalizer while song is playing 
  + added function for pulling FFT data to build FFT spectrum and frequency graphs

  Version 2.3                     March 2009
  ----------------------------------------------
  - fixed crash bug in ID3v2 functions ( crash if you try to load ID3v2.2.0 or ID3v2.3.0
  - fixed memory leak on Release() without Close() 
  - fixed PushDataToStream(0,0); endless repeating bug for managed streams
  + added blocking and non blocking callback messages
  


  Version 2.2                     March 2009
  ----------------------------------------------
  - fixed fStop bug ( this value was always 1 )
  - fixed "internal volume" bug
  - fixed "fade in" right channel bug
  - fixed invalid reported stream load of managed buffered stream
  + added callback function
  + added new functions for volume settings
  

  Version 2.1                     February 2009
  ----------------------------------------------
  - WMp3 class is renamed into LIBWMP3 library
  - WMp3 class now has CWMp3 interface
  - add wrapper with C functions
  - add support for Visual Basic ( C wrapper functions )
  - add support for Borland Delphi ( CWMp3 interface)
  - added functions for specifying equalizer bands
  - added functions for modifying echo modes
  - added support for reverse playing ( now you can play mp3 backward )
  - added support for managed memory streams ( you can push new data into stream while song is playing,
    you can now play song starting with small chunk of data and later add new data chunks
  - fixed serious bug in decoding thread.
    Thanks to Nicholas Chapman: nick_gwen_chapman@yahoo.fr
  - fixed thread handle leak
  - fixed loop playing
  - add bit reservoir recovery to LIBMAD engine ( need this for "real" reverse playing )


  Version 2.0                     July 2008
  ---------------------------------------------
  - WMp3 C++ clas is now based on LIBMAD decoding engine.
  - added Shibatch Super Equalizer

 
  Version 1.1                   April 2008
  -------------------------------------------   
  - updated decoding engine to mpg123 v4.1


  Version 1.0                   June 2005
  ------------------------------------------
  - mpg123 decoding engine is now integrated into class
  - there is no need for mpglib.dll

  Version 0.9                   June 2004
  ------------------------------------------
  - wrapper for mpglib.dll mp3 decoding engine


License
=======

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.


For license info read:

lgpl.txt
gpl.txt
libmad_copyright.txt
dsp_superequ.txt
libmad_readme.txt


All other infos on email: zcindori@inet.hr







