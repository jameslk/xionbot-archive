# Microsoft Developer Studio Generated NMAKE File, Based on xionbot.dsp
!IF "$(CFG)" == ""
CFG=xionbot - Win32 Debug
!MESSAGE No configuration specified. Defaulting to xionbot - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xionbot - Win32 Release" && "$(CFG)" != "xionbot - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xionbot - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\xionbot.exe"


CLEAN :
	-@erase "$(INTDIR)\conf.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\irc_admin.obj"
	-@erase "$(INTDIR)\irc_botcmd.obj"
	-@erase "$(INTDIR)\irc_channel.obj"
	-@erase "$(INTDIR)\irc_commands.obj"
	-@erase "$(INTDIR)\irc_mode.obj"
	-@erase "$(INTDIR)\irc_parse.obj"
	-@erase "$(INTDIR)\irc_socket.obj"
	-@erase "$(INTDIR)\irc_tools.obj"
	-@erase "$(INTDIR)\irc_user.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mod-irc_relay.obj"
	-@erase "$(INTDIR)\mod-weburlcache.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\xionbot.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\xionbot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xionbot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\xionbot.pdb" /machine:I386 /out:"$(OUTDIR)\xionbot.exe" 
LINK32_OBJS= \
	"$(INTDIR)\conf.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\irc_admin.obj" \
	"$(INTDIR)\irc_botcmd.obj" \
	"$(INTDIR)\irc_channel.obj" \
	"$(INTDIR)\irc_commands.obj" \
	"$(INTDIR)\irc_mode.obj" \
	"$(INTDIR)\irc_parse.obj" \
	"$(INTDIR)\irc_socket.obj" \
	"$(INTDIR)\irc_tools.obj" \
	"$(INTDIR)\irc_user.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mod-irc_relay.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\mod-weburlcache.obj"

"$(OUTDIR)\xionbot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xionbot - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\xionbot.exe"


CLEAN :
	-@erase "$(INTDIR)\conf.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\irc_admin.obj"
	-@erase "$(INTDIR)\irc_botcmd.obj"
	-@erase "$(INTDIR)\irc_channel.obj"
	-@erase "$(INTDIR)\irc_commands.obj"
	-@erase "$(INTDIR)\irc_mode.obj"
	-@erase "$(INTDIR)\irc_parse.obj"
	-@erase "$(INTDIR)\irc_socket.obj"
	-@erase "$(INTDIR)\irc_tools.obj"
	-@erase "$(INTDIR)\irc_user.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mod-irc_relay.obj"
	-@erase "$(INTDIR)\mod-weburlcache.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\xionbot.exe"
	-@erase "$(OUTDIR)\xionbot.ilk"
	-@erase "$(OUTDIR)\xionbot.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\xionbot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xionbot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib WS2_32.LIB WSOCK32.LIB /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\xionbot.pdb" /debug /machine:I386 /out:"$(OUTDIR)\xionbot.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\conf.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\irc_admin.obj" \
	"$(INTDIR)\irc_botcmd.obj" \
	"$(INTDIR)\irc_channel.obj" \
	"$(INTDIR)\irc_commands.obj" \
	"$(INTDIR)\irc_mode.obj" \
	"$(INTDIR)\irc_parse.obj" \
	"$(INTDIR)\irc_socket.obj" \
	"$(INTDIR)\irc_tools.obj" \
	"$(INTDIR)\irc_user.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mod-irc_relay.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\mod-weburlcache.obj"

"$(OUTDIR)\xionbot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("xionbot.dep")
!INCLUDE "xionbot.dep"
!ELSE 
!MESSAGE Warning: cannot find "xionbot.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xionbot - Win32 Release" || "$(CFG)" == "xionbot - Win32 Debug"
SOURCE=.\conf.c

"$(INTDIR)\conf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\event.c

"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_admin.c

"$(INTDIR)\irc_admin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_botcmd.c

"$(INTDIR)\irc_botcmd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_channel.c

"$(INTDIR)\irc_channel.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_commands.c

"$(INTDIR)\irc_commands.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_mode.c

"$(INTDIR)\irc_mode.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_parse.c

"$(INTDIR)\irc_parse.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_socket.c

"$(INTDIR)\irc_socket.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_tools.c

"$(INTDIR)\irc_tools.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\irc_user.c

"$(INTDIR)\irc_user.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\log.c

"$(INTDIR)\log.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.c

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\mod-irc_relay.c"

"$(INTDIR)\mod-irc_relay.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\mod-weburlcache.c"

"$(INTDIR)\mod-weburlcache.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tools.c

"$(INTDIR)\tools.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

