# Microsoft Developer Studio Project File - Name="xionbot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=xionbot - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xionbot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xionbot.mak" CFG="xionbot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xionbot - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "xionbot - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xionbot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "xionbot - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "php5" /I "php5\main" /I "php5\Zend" /I "php5\TSRM" /I "php5\sapi\embed" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=0 /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php5embed.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib WS2_32.LIB WSOCK32.LIB /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"MSVCRT" /pdbtype:sept /libpath:"php5\libs"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "xionbot - Win32 Release"
# Name "xionbot - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "php"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\php_event.c
# End Source File
# Begin Source File

SOURCE=.\php_event.h
# End Source File
# Begin Source File

SOURCE=.\php_funcs.c
# End Source File
# Begin Source File

SOURCE=.\php_funcs.h
# End Source File
# Begin Source File

SOURCE=.\php_global.h
# End Source File
# Begin Source File

SOURCE=.\php_log.c
# End Source File
# Begin Source File

SOURCE=.\php_log.h
# End Source File
# Begin Source File

SOURCE=.\php_main.c
# End Source File
# Begin Source File

SOURCE=.\php_main.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\conf.c
# End Source File
# Begin Source File

SOURCE=.\conf.h
# End Source File
# Begin Source File

SOURCE=.\event.c
# End Source File
# Begin Source File

SOURCE=.\event.h
# End Source File
# Begin Source File

SOURCE=.\irc_admin.c
# End Source File
# Begin Source File

SOURCE=.\irc_admin.h
# End Source File
# Begin Source File

SOURCE=.\irc_botcmd.c
# End Source File
# Begin Source File

SOURCE=.\irc_botcmd.h
# End Source File
# Begin Source File

SOURCE=.\irc_channel.c
# End Source File
# Begin Source File

SOURCE=.\irc_channel.h
# End Source File
# Begin Source File

SOURCE=.\irc_commands.c
# End Source File
# Begin Source File

SOURCE=.\irc_commands.h
# End Source File
# Begin Source File

SOURCE=.\irc_def.h
# End Source File
# Begin Source File

SOURCE=.\irc_mode.c
# End Source File
# Begin Source File

SOURCE=.\irc_mode.h
# End Source File
# Begin Source File

SOURCE=.\irc_parse.c
# End Source File
# Begin Source File

SOURCE=.\irc_parse.h
# End Source File
# Begin Source File

SOURCE=.\irc_socket.c
# End Source File
# Begin Source File

SOURCE=.\irc_socket.h
# End Source File
# Begin Source File

SOURCE=.\irc_tools.c
# End Source File
# Begin Source File

SOURCE=.\irc_tools.h
# End Source File
# Begin Source File

SOURCE=.\irc_user.c
# End Source File
# Begin Source File

SOURCE=.\irc_user.h
# End Source File
# Begin Source File

SOURCE=.\log.c
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=".\mod-irc_relay.c"
# End Source File
# Begin Source File

SOURCE=".\mod-irc_relay.h"
# End Source File
# Begin Source File

SOURCE=".\mod-weburlcache.c"
# End Source File
# Begin Source File

SOURCE=".\mod-weburlcache.h"
# End Source File
# Begin Source File

SOURCE=.\tools.c
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
