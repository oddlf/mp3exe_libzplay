# Microsoft Developer Studio Project File - Name="mp3exe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mp3exe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mp3exe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp3exe.mak" CFG="mp3exe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp3exe - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mp3exe - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp3exe - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "W_STATIC_LIB" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41a /d "NDEBUG"
# ADD RSC /l 0x41a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "mp3exe - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "W_STATIC_LIB" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41a /d "_DEBUG"
# ADD RSC /l 0x41a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mp3exe - Win32 Release"
# Name "mp3exe - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Mainform.cpp
# End Source File
# Begin Source File

SOURCE=.\src\vumeter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wapp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wbmpbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Wbmpfont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wbmptextbox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wcontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wfont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wform.cpp
# End Source File
# Begin Source File

SOURCE=.\src\winikey.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wmem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wmixer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wopendialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wsavedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wscrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wtooltip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\main.h
# End Source File
# Begin Source File

SOURCE=.\src\mainform.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\vumeter.h
# End Source File
# Begin Source File

SOURCE=.\src\wapp.h
# End Source File
# Begin Source File

SOURCE=.\src\wbmpbutton.h
# End Source File
# Begin Source File

SOURCE=.\src\wbmpfont.h
# End Source File
# Begin Source File

SOURCE=.\src\wbmptextbox.h
# End Source File
# Begin Source File

SOURCE=.\src\wcontrol.h
# End Source File
# Begin Source File

SOURCE=.\src\wfile.h
# End Source File
# Begin Source File

SOURCE=.\src\wfont.h
# End Source File
# Begin Source File

SOURCE=.\src\wform.h
# End Source File
# Begin Source File

SOURCE=.\src\winikey.h
# End Source File
# Begin Source File

SOURCE=.\src\wmem.h
# End Source File
# Begin Source File

SOURCE=.\src\wmixer.h
# End Source File
# Begin Source File

SOURCE=.\src\wopendialog.h
# End Source File
# Begin Source File

SOURCE=.\src\wsavedialog.h
# End Source File
# Begin Source File

SOURCE=.\src\wscrollbar.h
# End Source File
# Begin Source File

SOURCE=.\src\wtooltip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\resource\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\src\resource\large.ico
# End Source File
# Begin Source File

SOURCE=.\src\resource.rc
# End Source File
# Begin Source File

SOURCE=.\src\resource\small.ico
# End Source File
# End Group
# End Target
# End Project
