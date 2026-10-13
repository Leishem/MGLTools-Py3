# Microsoft Developer Studio Generated NMAKE File, Based on VolumeLibrary.dsp
!IF "$(CFG)" == ""
CFG=VolumeLibrary - Win32 Debug
!MESSAGE No configuration specified. Defaulting to VolumeLibrary - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "VolumeLibrary - Win32 Release" && "$(CFG)" != "VolumeLibrary - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VolumeLibrary.mak" CFG="VolumeLibrary - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VolumeLibrary - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "VolumeLibrary - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "VolumeLibrary - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\VolumeLibrary.lib"


CLEAN :
	-@erase "$(INTDIR)\OpenGLVolumeClipCube.obj"
	-@erase "$(INTDIR)\OpenGLVolumePlane.obj"
	-@erase "$(INTDIR)\OpenGLVolumePolygon.obj"
	-@erase "$(INTDIR)\OpenGLVolumeRenderer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VolumeRenderer.obj"
	-@erase "$(OUTDIR)\VolumeLibrary.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\VolumeLibrary.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VolumeLibrary.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\VolumeLibrary.lib" 
LIB32_OBJS= \
	"$(INTDIR)\OpenGLVolumeClipCube.obj" \
	"$(INTDIR)\OpenGLVolumePlane.obj" \
	"$(INTDIR)\OpenGLVolumePolygon.obj" \
	"$(INTDIR)\OpenGLVolumeRenderer.obj" \
	"$(INTDIR)\VolumeRenderer.obj"

"$(OUTDIR)\VolumeLibrary.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "VolumeLibrary - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\VolumeLibrary.lib"


CLEAN :
	-@erase "$(INTDIR)\OpenGLVolumeClipCube.obj"
	-@erase "$(INTDIR)\OpenGLVolumePlane.obj"
	-@erase "$(INTDIR)\OpenGLVolumePolygon.obj"
	-@erase "$(INTDIR)\OpenGLVolumeRenderer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\VolumeRenderer.obj"
	-@erase "$(OUTDIR)\VolumeLibrary.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\VolumeLibrary.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VolumeLibrary.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\VolumeLibrary.lib" 
LIB32_OBJS= \
	"$(INTDIR)\OpenGLVolumeClipCube.obj" \
	"$(INTDIR)\OpenGLVolumePlane.obj" \
	"$(INTDIR)\OpenGLVolumePolygon.obj" \
	"$(INTDIR)\OpenGLVolumeRenderer.obj" \
	"$(INTDIR)\VolumeRenderer.obj"

"$(OUTDIR)\VolumeLibrary.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("VolumeLibrary.dep")
!INCLUDE "VolumeLibrary.dep"
!ELSE 
!MESSAGE Warning: cannot find "VolumeLibrary.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "VolumeLibrary - Win32 Release" || "$(CFG)" == "VolumeLibrary - Win32 Debug"
SOURCE=.\OpenGLVolumeClipCube.cpp

"$(INTDIR)\OpenGLVolumeClipCube.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OpenGLVolumePlane.cpp

"$(INTDIR)\OpenGLVolumePlane.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OpenGLVolumePolygon.cpp

"$(INTDIR)\OpenGLVolumePolygon.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OpenGLVolumeRenderer.cpp

"$(INTDIR)\OpenGLVolumeRenderer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\VolumeRenderer.cpp

"$(INTDIR)\VolumeRenderer.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

