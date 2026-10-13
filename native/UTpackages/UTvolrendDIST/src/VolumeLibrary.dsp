# Microsoft Developer Studio Project File - Name="VolumeLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=VolumeLibrary - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VolumeLibrary.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VolumeLibrary - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "VolumeLibrary - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "VolumeLibrary - Win32 Release"
# Name "VolumeLibrary - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ClipCube.cpp
# End Source File
# Begin Source File

SOURCE=.\Extent.cpp
# End Source File
# Begin Source File

SOURCE=.\FragmentProgramARBImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\FragmentProgramImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\MyExtensions.cpp
# End Source File
# Begin Source File

SOURCE=.\Paletted2DImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\PalettedImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\Polygon.cpp
# End Source File
# Begin Source File

SOURCE=.\PolygonArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\RendererBase.cpp
# End Source File
# Begin Source File

SOURCE=.\RGBABase.cpp
# End Source File
# Begin Source File

SOURCE=.\SGIColorTableImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleRGBA2DImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleRGBAImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\UnshadedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeRendererFactory.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ClipCube.h
# End Source File
# Begin Source File

SOURCE=.\ExtensionPointers.h
# End Source File
# Begin Source File

SOURCE=.\Extent.h
# End Source File
# Begin Source File

SOURCE=.\FragmentProgramARBImpl.h
# End Source File
# Begin Source File

SOURCE=.\FragmentProgramImpl.h
# End Source File
# Begin Source File

SOURCE=.\LookupTables.h
# End Source File
# Begin Source File

SOURCE=.\MyExtensions.h
# End Source File
# Begin Source File

SOURCE=.\Paletted2DImpl.h
# End Source File
# Begin Source File

SOURCE=.\PalettedImpl.h
# End Source File
# Begin Source File

SOURCE=.\Plane.h
# End Source File
# Begin Source File

SOURCE=.\Polygon.h
# End Source File
# Begin Source File

SOURCE=.\PolygonArray.h
# End Source File
# Begin Source File

SOURCE=.\Renderer.h
# End Source File
# Begin Source File

SOURCE=.\RendererBase.h
# End Source File
# Begin Source File

SOURCE=.\RGBABase.h
# End Source File
# Begin Source File

SOURCE=.\SGIColorTableImpl.h
# End Source File
# Begin Source File

SOURCE=.\SimpleRGBA2DImpl.h
# End Source File
# Begin Source File

SOURCE=.\SimpleRGBAImpl.h
# End Source File
# Begin Source File

SOURCE=.\StaticExtensionPointers.h
# End Source File
# Begin Source File

SOURCE=.\UnshadedBase.h
# End Source File
# Begin Source File

SOURCE=.\VolumeRenderer.h
# End Source File
# Begin Source File

SOURCE=.\VolumeRendererFactory.h
# End Source File
# End Group
# End Target
# End Project
