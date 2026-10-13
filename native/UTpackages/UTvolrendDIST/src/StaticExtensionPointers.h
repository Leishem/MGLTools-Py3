/*
  Copyright 2002-2003 The University of Texas at Austin
  
	Authors: Anthony Thane <thanea@ices.utexas.edu>
	Advisor: Chandrajit Bajaj <bajaj@cs.utexas.edu>

  This file is part of Volume Rover.

  Volume Rover is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Volume Rover is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Volume Rover; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef STATICEXTENSIONPOINTERS_H
#define STATICEXTENSIONPOINTERS_H

#if !defined(_WIN32) && !defined(GLX_ARB_get_proc_address)
	static bool staticPointersInitialized = false;

// GL_VERSION_1_2
	static MYPFNGLBLENDCOLORPROC staticPointerglBlendColor = 0;
	static MYPFNGLBLENDEQUATIONPROC staticPointerglBlendEquation = 0;
	static MYPFNGLDRAWRANGEELEMENTSPROC staticPointerglDrawRangeElements = 0;
	static MYPFNGLCOLORTABLEPROC staticPointerglColorTable = 0;
	static MYPFNGLCOLORTABLEPARAMETERFVPROC staticPointerglColorTableParameterfv = 0;
	static MYPFNGLCOLORTABLEPARAMETERIVPROC staticPointerglColorTableParameteriv = 0;
	static MYPFNGLCOPYCOLORTABLEPROC staticPointerglCopyColorTable = 0;
	static MYPFNGLGETCOLORTABLEPROC staticPointerglGetColorTable = 0;
	static MYPFNGLGETCOLORTABLEPARAMETERFVPROC staticPointerglGetColorTableParameterfv = 0;
	static MYPFNGLGETCOLORTABLEPARAMETERIVPROC staticPointerglGetColorTableParameteriv = 0;
	static MYPFNGLCOLORSUBTABLEPROC staticPointerglColorSubTable = 0;
	static MYPFNGLCOPYCOLORSUBTABLEPROC staticPointerglCopyColorSubTable = 0;
	static MYPFNGLCONVOLUTIONFILTER1DPROC staticPointerglConvolutionFilter1D = 0;
	static MYPFNGLCONVOLUTIONFILTER2DPROC staticPointerglConvolutionFilter2D = 0;
	static MYPFNGLCONVOLUTIONPARAMETERFPROC staticPointerglConvolutionParameterf = 0;
	static MYPFNGLCONVOLUTIONPARAMETERFVPROC staticPointerglConvolutionParameterfv = 0;
	static MYPFNGLCONVOLUTIONPARAMETERIPROC staticPointerglConvolutionParameteri = 0;
	static MYPFNGLCONVOLUTIONPARAMETERIVPROC staticPointerglConvolutionParameteriv = 0;
	static MYPFNGLCOPYCONVOLUTIONFILTER1DPROC staticPointerglCopyConvolutionFilter1D = 0;
	static MYPFNGLCOPYCONVOLUTIONFILTER2DPROC staticPointerglCopyConvolutionFilter2D = 0;
	static MYPFNGLGETCONVOLUTIONFILTERPROC staticPointerglGetConvolutionFilter = 0;
	static MYPFNGLGETCONVOLUTIONPARAMETERFVPROC staticPointerglGetConvolutionParameterfv = 0;
	static MYPFNGLGETCONVOLUTIONPARAMETERIVPROC staticPointerglGetConvolutionParameteriv = 0;
	static MYPFNGLGETSEPARABLEFILTERPROC staticPointerglGetSeparableFilter = 0;
	static MYPFNGLSEPARABLEFILTER2DPROC staticPointerglSeparableFilter2D = 0;
	static MYPFNGLGETHISTOGRAMPROC staticPointerglGetHistogram = 0;
	static MYPFNGLGETHISTOGRAMPARAMETERFVPROC staticPointerglGetHistogramParameterfv = 0;
	static MYPFNGLGETHISTOGRAMPARAMETERIVPROC staticPointerglGetHistogramParameteriv = 0;
	static MYPFNGLGETMINMAXPROC staticPointerglGetMinmax = 0;
	static MYPFNGLGETMINMAXPARAMETERFVPROC staticPointerglGetMinmaxParameterfv = 0;
	static MYPFNGLGETMINMAXPARAMETERIVPROC staticPointerglGetMinmaxParameteriv = 0;
	static MYPFNGLHISTOGRAMPROC staticPointerglHistogram = 0;
	static MYPFNGLMINMAXPROC staticPointerglMinmax = 0;
	static MYPFNGLRESETHISTOGRAMPROC staticPointerglResetHistogram = 0;
	static MYPFNGLRESETMINMAXPROC staticPointerglResetMinmax = 0;
	static MYPFNGLTEXIMAGE3DPROC staticPointerglTexImage3D = 0;
	static MYPFNGLTEXSUBIMAGE3DPROC staticPointerglTexSubImage3D = 0;
	static MYPFNGLCOPYTEXSUBIMAGE3DPROC staticPointerglCopyTexSubImage3D = 0;
// End OpenGL version 1.2

// OpenGL version 1.3
	static MYPFNGLACTIVETEXTUREPROC staticPointerglActiveTexture = 0;
	static MYPFNGLCLIENTACTIVETEXTUREPROC staticPointerglClientActiveTexture = 0;
	static MYPFNGLMULTITEXCOORD1DPROC staticPointerglMultiTexCoord1d = 0;
	static MYPFNGLMULTITEXCOORD1DVPROC staticPointerglMultiTexCoord1dv = 0;
	static MYPFNGLMULTITEXCOORD1FPROC staticPointerglMultiTexCoord1f = 0;
	static MYPFNGLMULTITEXCOORD1FVPROC staticPointerglMultiTexCoord1fv = 0;
	static MYPFNGLMULTITEXCOORD1IPROC staticPointerglMultiTexCoord1i = 0;
	static MYPFNGLMULTITEXCOORD1IVPROC staticPointerglMultiTexCoord1iv = 0;
	static MYPFNGLMULTITEXCOORD1SPROC staticPointerglMultiTexCoord1s = 0;
	static MYPFNGLMULTITEXCOORD1SVPROC staticPointerglMultiTexCoord1sv = 0;
	static MYPFNGLMULTITEXCOORD2DPROC staticPointerglMultiTexCoord2d = 0;
	static MYPFNGLMULTITEXCOORD2DVPROC staticPointerglMultiTexCoord2dv = 0;
	static MYPFNGLMULTITEXCOORD2FPROC staticPointerglMultiTexCoord2f = 0;
	static MYPFNGLMULTITEXCOORD2FVPROC staticPointerglMultiTexCoord2fv = 0;
	static MYPFNGLMULTITEXCOORD2IPROC staticPointerglMultiTexCoord2i = 0;
	static MYPFNGLMULTITEXCOORD2IVPROC staticPointerglMultiTexCoord2iv = 0;
	static MYPFNGLMULTITEXCOORD2SPROC staticPointerglMultiTexCoord2s = 0;
	static MYPFNGLMULTITEXCOORD2SVPROC staticPointerglMultiTexCoord2sv = 0;
	static MYPFNGLMULTITEXCOORD3DPROC staticPointerglMultiTexCoord3d = 0;
	static MYPFNGLMULTITEXCOORD3DVPROC staticPointerglMultiTexCoord3dv = 0;
	static MYPFNGLMULTITEXCOORD3FPROC staticPointerglMultiTexCoord3f = 0;
	static MYPFNGLMULTITEXCOORD3FVPROC staticPointerglMultiTexCoord3fv = 0;
	static MYPFNGLMULTITEXCOORD3IPROC staticPointerglMultiTexCoord3i = 0;
	static MYPFNGLMULTITEXCOORD3IVPROC staticPointerglMultiTexCoord3iv = 0;
	static MYPFNGLMULTITEXCOORD3SPROC staticPointerglMultiTexCoord3s = 0;
	static MYPFNGLMULTITEXCOORD3SVPROC staticPointerglMultiTexCoord3sv = 0;
	static MYPFNGLMULTITEXCOORD4DPROC staticPointerglMultiTexCoord4d = 0;
	static MYPFNGLMULTITEXCOORD4DVPROC staticPointerglMultiTexCoord4dv = 0;
	static MYPFNGLMULTITEXCOORD4FPROC staticPointerglMultiTexCoord4f = 0;
	static MYPFNGLMULTITEXCOORD4FVPROC staticPointerglMultiTexCoord4fv = 0;
	static MYPFNGLMULTITEXCOORD4IPROC staticPointerglMultiTexCoord4i = 0;
	static MYPFNGLMULTITEXCOORD4IVPROC staticPointerglMultiTexCoord4iv = 0;
	static MYPFNGLMULTITEXCOORD4SPROC staticPointerglMultiTexCoord4s = 0;
	static MYPFNGLMULTITEXCOORD4SVPROC staticPointerglMultiTexCoord4sv = 0;
	static MYPFNGLLOADTRANSPOSEMATRIXFPROC staticPointerglLoadTransposeMatrixf = 0;
	static MYPFNGLLOADTRANSPOSEMATRIXDPROC staticPointerglLoadTransposeMatrixd = 0;
	static MYPFNGLMULTTRANSPOSEMATRIXFPROC staticPointerglMultTransposeMatrixf = 0;
	static MYPFNGLMULTTRANSPOSEMATRIXDPROC staticPointerglMultTransposeMatrixd = 0;
	static MYPFNGLSAMPLECOVERAGEPROC staticPointerglSampleCoverage = 0;
	static MYPFNGLCOMPRESSEDTEXIMAGE3DPROC staticPointerglCompressedTexImage3D = 0;
	static MYPFNGLCOMPRESSEDTEXIMAGE2DPROC staticPointerglCompressedTexImage2D = 0;
	static MYPFNGLCOMPRESSEDTEXIMAGE1DPROC staticPointerglCompressedTexImage1D = 0;
	static MYPFNGLCOMPRESSEDTEXSUBIMAGE3DPROC staticPointerglCompressedTexSubImage3D = 0;
	static MYPFNGLCOMPRESSEDTEXSUBIMAGE2DPROC staticPointerglCompressedTexSubImage2D = 0;
	static MYPFNGLCOMPRESSEDTEXSUBIMAGE1DPROC staticPointerglCompressedTexSubImage1D = 0;
	static MYPFNGLGETCOMPRESSEDTEXIMAGEPROC staticPointerglGetCompressedTexImage = 0;
	// End OpenGL version 1.3

// OpenGL version 1.4
	static MYPFNGLBLENDFUNCSEPARATEPROC staticPointerglBlendFuncSeparate = 0;
	static MYPFNGLFOGCOORDFPROC staticPointerglFogCoordf = 0;
	static MYPFNGLFOGCOORDFVPROC staticPointerglFogCoordfv = 0;
	static MYPFNGLFOGCOORDDPROC staticPointerglFogCoordd = 0;
	static MYPFNGLFOGCOORDDVPROC staticPointerglFogCoorddv = 0;
	static MYPFNGLFOGCOORDPOINTERPROC staticPointerglFogCoordPointer = 0;
	static MYPFNGLMULTIDRAWARRAYSPROC staticPointerglMultiDrawArrays = 0;
	static MYPFNGLMULTIDRAWELEMENTSPROC staticPointerglMultiDrawElements = 0;
	static MYPFNGLPOINTPARAMETERFPROC staticPointerglPointParameterf = 0;
	static MYPFNGLPOINTPARAMETERFVPROC staticPointerglPointParameterfv = 0;
	static MYPFNGLPOINTPARAMETERIPROC staticPointerglPointParameteri = 0;
	static MYPFNGLPOINTPARAMETERIVPROC staticPointerglPointParameteriv = 0;
	static MYPFNGLSECONDARYCOLOR3BPROC staticPointerglSecondaryColor3b = 0;
	static MYPFNGLSECONDARYCOLOR3BVPROC staticPointerglSecondaryColor3bv = 0;
	static MYPFNGLSECONDARYCOLOR3DPROC staticPointerglSecondaryColor3d = 0;
	static MYPFNGLSECONDARYCOLOR3DVPROC staticPointerglSecondaryColor3dv = 0;
	static MYPFNGLSECONDARYCOLOR3FPROC staticPointerglSecondaryColor3f = 0;
	static MYPFNGLSECONDARYCOLOR3FVPROC staticPointerglSecondaryColor3fv = 0;
	static MYPFNGLSECONDARYCOLOR3IPROC staticPointerglSecondaryColor3i = 0;
	static MYPFNGLSECONDARYCOLOR3IVPROC staticPointerglSecondaryColor3iv = 0;
	static MYPFNGLSECONDARYCOLOR3SPROC staticPointerglSecondaryColor3s = 0;
	static MYPFNGLSECONDARYCOLOR3SVPROC staticPointerglSecondaryColor3sv = 0;
	static MYPFNGLSECONDARYCOLOR3UBPROC staticPointerglSecondaryColor3ub = 0;
	static MYPFNGLSECONDARYCOLOR3UBVPROC staticPointerglSecondaryColor3ubv = 0;
	static MYPFNGLSECONDARYCOLOR3UIPROC staticPointerglSecondaryColor3ui = 0;
	static MYPFNGLSECONDARYCOLOR3UIVPROC staticPointerglSecondaryColor3uiv = 0;
	static MYPFNGLSECONDARYCOLOR3USPROC staticPointerglSecondaryColor3us = 0;
	static MYPFNGLSECONDARYCOLOR3USVPROC staticPointerglSecondaryColor3usv = 0;
	static MYPFNGLSECONDARYCOLORPOINTERPROC staticPointerglSecondaryColorPointer = 0;
	static MYPFNGLWINDOWPOS2DPROC staticPointerglWindowPos2d = 0;
	static MYPFNGLWINDOWPOS2DVPROC staticPointerglWindowPos2dv = 0;
	static MYPFNGLWINDOWPOS2FPROC staticPointerglWindowPos2f = 0;
	static MYPFNGLWINDOWPOS2FVPROC staticPointerglWindowPos2fv = 0;
	static MYPFNGLWINDOWPOS2IPROC staticPointerglWindowPos2i = 0;
	static MYPFNGLWINDOWPOS2IVPROC staticPointerglWindowPos2iv = 0;
	static MYPFNGLWINDOWPOS2SPROC staticPointerglWindowPos2s = 0;
	static MYPFNGLWINDOWPOS2SVPROC staticPointerglWindowPos2sv = 0;
	static MYPFNGLWINDOWPOS3DPROC staticPointerglWindowPos3d = 0;
	static MYPFNGLWINDOWPOS3DVPROC staticPointerglWindowPos3dv = 0;
	static MYPFNGLWINDOWPOS3FPROC staticPointerglWindowPos3f = 0;
	static MYPFNGLWINDOWPOS3FVPROC staticPointerglWindowPos3fv = 0;
	static MYPFNGLWINDOWPOS3IPROC staticPointerglWindowPos3i = 0;
	static MYPFNGLWINDOWPOS3IVPROC staticPointerglWindowPos3iv = 0;
	static MYPFNGLWINDOWPOS3SPROC staticPointerglWindowPos3s = 0;
	static MYPFNGLWINDOWPOS3SVPROC staticPointerglWindowPos3sv = 0;
// End OpenGL version 1.4

// GL_EXT_paletted_texture
	static MYPFNGLCOLORTABLEEXTPROC staticPointerglColorTableEXT = (MYPFNGLCOLORTABLEEXTPROC) 0;
	static MYPFNGLGETCOLORTABLEEXTPROC staticPointerglGetColorTableEXT = (MYPFNGLGETCOLORTABLEEXTPROC) 0;
	static MYPFNGLGETCOLORTABLEPARAMETERIVEXTPROC staticPointerglGetColorTableParameterivEXT = (MYPFNGLGETCOLORTABLEPARAMETERIVEXTPROC) 0;
	static MYPFNGLGETCOLORTABLEPARAMETERFVEXTPROC staticPointerglGetColorTableParameterfvEXT = (MYPFNGLGETCOLORTABLEPARAMETERFVEXTPROC) 0;
// End GL_EXT_paletted_texture

// GL_ARB_multitexture
	static MYPFNGLACTIVETEXTUREARBPROC staticPointerglActiveTextureARB = (MYPFNGLACTIVETEXTUREARBPROC) 0;
	static MYPFNGLCLIENTACTIVETEXTUREARBPROC staticPointerglClientActiveTextureARB = (MYPFNGLCLIENTACTIVETEXTUREARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1DARBPROC staticPointerglMultiTexCoord1dARB = (MYPFNGLMULTITEXCOORD1DARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1DVARBPROC staticPointerglMultiTexCoord1dvARB = (MYPFNGLMULTITEXCOORD1DVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1FARBPROC staticPointerglMultiTexCoord1fARB = (MYPFNGLMULTITEXCOORD1FARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1FVARBPROC staticPointerglMultiTexCoord1fvARB = (MYPFNGLMULTITEXCOORD1FVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1IARBPROC staticPointerglMultiTexCoord1iARB = (MYPFNGLMULTITEXCOORD1IARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1IVARBPROC staticPointerglMultiTexCoord1ivARB = (MYPFNGLMULTITEXCOORD1IVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1SARBPROC staticPointerglMultiTexCoord1sARB = (MYPFNGLMULTITEXCOORD1SARBPROC) 0;
	static MYPFNGLMULTITEXCOORD1SVARBPROC staticPointerglMultiTexCoord1svARB = (MYPFNGLMULTITEXCOORD1SVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2DARBPROC staticPointerglMultiTexCoord2dARB = (MYPFNGLMULTITEXCOORD2DARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2DVARBPROC staticPointerglMultiTexCoord2dvARB = (MYPFNGLMULTITEXCOORD2DVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2FARBPROC staticPointerglMultiTexCoord2fARB = (MYPFNGLMULTITEXCOORD2FARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2FVARBPROC staticPointerglMultiTexCoord2fvARB = (MYPFNGLMULTITEXCOORD2FVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2IARBPROC staticPointerglMultiTexCoord2iARB = (MYPFNGLMULTITEXCOORD2IARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2IVARBPROC staticPointerglMultiTexCoord2ivARB = (MYPFNGLMULTITEXCOORD2IVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2SARBPROC staticPointerglMultiTexCoord2sARB = (MYPFNGLMULTITEXCOORD2SARBPROC) 0;
	static MYPFNGLMULTITEXCOORD2SVARBPROC staticPointerglMultiTexCoord2svARB = (MYPFNGLMULTITEXCOORD2SVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3DARBPROC staticPointerglMultiTexCoord3dARB = (MYPFNGLMULTITEXCOORD3DARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3DVARBPROC staticPointerglMultiTexCoord3dvARB = (MYPFNGLMULTITEXCOORD3DVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3FARBPROC staticPointerglMultiTexCoord3fARB = (MYPFNGLMULTITEXCOORD3FARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3FVARBPROC staticPointerglMultiTexCoord3fvARB = (MYPFNGLMULTITEXCOORD3FVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3IARBPROC staticPointerglMultiTexCoord3iARB = (MYPFNGLMULTITEXCOORD3IARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3IVARBPROC staticPointerglMultiTexCoord3ivARB = (MYPFNGLMULTITEXCOORD3IVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3SARBPROC staticPointerglMultiTexCoord3sARB = (MYPFNGLMULTITEXCOORD3SARBPROC) 0;
	static MYPFNGLMULTITEXCOORD3SVARBPROC staticPointerglMultiTexCoord3svARB = (MYPFNGLMULTITEXCOORD3SVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4DARBPROC staticPointerglMultiTexCoord4dARB = (MYPFNGLMULTITEXCOORD4DARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4DVARBPROC staticPointerglMultiTexCoord4dvARB = (MYPFNGLMULTITEXCOORD4DVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4FARBPROC staticPointerglMultiTexCoord4fARB = (MYPFNGLMULTITEXCOORD4FARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4FVARBPROC staticPointerglMultiTexCoord4fvARB = (MYPFNGLMULTITEXCOORD4FVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4IARBPROC staticPointerglMultiTexCoord4iARB = (MYPFNGLMULTITEXCOORD4IARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4IVARBPROC staticPointerglMultiTexCoord4ivARB = (MYPFNGLMULTITEXCOORD4IVARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4SARBPROC staticPointerglMultiTexCoord4sARB = (MYPFNGLMULTITEXCOORD4SARBPROC) 0;
	static MYPFNGLMULTITEXCOORD4SVARBPROC staticPointerglMultiTexCoord4svARB = (MYPFNGLMULTITEXCOORD4SVARBPROC) 0;
// End GL_ARB_multitexture

// GL_SGI_texture_color_table
	// No new functions.
// End GL_SGI_texture_color_table

// GL_SGI_color_table
	static MYPFNGLCOLORTABLESGIPROC staticPointerglColorTableSGI = (MYPFNGLCOLORTABLESGIPROC) 0;
	static MYPFNGLCOLORTABLEPARAMETERFVSGIPROC staticPointerglColorTableParameterfvSGI = (MYPFNGLCOLORTABLEPARAMETERFVSGIPROC) 0;
	static MYPFNGLCOLORTABLEPARAMETERIVSGIPROC staticPointerglColorTableParameterivSGI = (MYPFNGLCOLORTABLEPARAMETERIVSGIPROC) 0;
	static MYPFNGLCOPYCOLORTABLESGIPROC staticPointerglCopyColorTableSGI = (MYPFNGLCOPYCOLORTABLESGIPROC) 0;
	static MYPFNGLGETCOLORTABLESGIPROC staticPointerglGetColorTableSGI = (MYPFNGLGETCOLORTABLESGIPROC) 0;
	static MYPFNGLGETCOLORTABLEPARAMETERFVSGIPROC staticPointerglGetColorTableParameterfvSGI = (MYPFNGLGETCOLORTABLEPARAMETERFVSGIPROC) 0;
	static MYPFNGLGETCOLORTABLEPARAMETERIVSGIPROC staticPointerglGetColorTableParameterivSGI = (MYPFNGLGETCOLORTABLEPARAMETERIVSGIPROC) 0;
// End GL_SGI_color_table

// GL_SGIS_texture_edge_clamp
	// No new functions.
// End GL_SGIS_texture_edge_clamp

// GL_EXT_texture3D
	static MYPFNGLTEXIMAGE3DEXTPROC staticPointerglTexImage3DEXT = (MYPFNGLTEXIMAGE3DEXTPROC) 0;
	static MYPFNGLTEXSUBIMAGE3DEXTPROC staticPointerglTexSubImage3DEXT = (MYPFNGLTEXSUBIMAGE3DEXTPROC) 0;
// End GL_EXT_texture3D

// GL_NV_fragment_program
	static MYPFNGLPROGRAMNAMEDPARAMETER4FNVPROC staticPointerglProgramNamedParameter4fNV = (MYPFNGLPROGRAMNAMEDPARAMETER4FNVPROC) 0;
	static MYPFNGLPROGRAMNAMEDPARAMETER4DNVPROC staticPointerglProgramNamedParameter4dNV = (MYPFNGLPROGRAMNAMEDPARAMETER4DNVPROC) 0;
	static MYPFNGLPROGRAMNAMEDPARAMETER4FVNVPROC staticPointerglProgramNamedParameter4fvNV = (MYPFNGLPROGRAMNAMEDPARAMETER4FVNVPROC) 0;
	static MYPFNGLPROGRAMNAMEDPARAMETER4DVNVPROC staticPointerglProgramNamedParameter4dvNV = (MYPFNGLPROGRAMNAMEDPARAMETER4DVNVPROC) 0;
	static MYPFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC staticPointerglGetProgramNamedParameterfvNV = (MYPFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC) 0;
	static MYPFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC staticPointerglGetProgramNamedParameterdvNV = (MYPFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC) 0;
// GL_NV_fragment_program

// GL_NV_vertex_program
	static MYPFNGLAREPROGRAMSRESIDENTNVPROC staticPointerglAreProgramsResidentNV = (MYPFNGLAREPROGRAMSRESIDENTNVPROC) 0;
	static MYPFNGLBINDPROGRAMNVPROC staticPointerglBindProgramNV = (MYPFNGLBINDPROGRAMNVPROC) 0;
	static MYPFNGLDELETEPROGRAMSNVPROC staticPointerglDeleteProgramsNV = (MYPFNGLDELETEPROGRAMSNVPROC) 0;
	static MYPFNGLEXECUTEPROGRAMNVPROC staticPointerglExecuteProgramNV = (MYPFNGLEXECUTEPROGRAMNVPROC) 0;
	static MYPFNGLGENPROGRAMSNVPROC staticPointerglGenProgramsNV = (MYPFNGLGENPROGRAMSNVPROC) 0;
	static MYPFNGLGETPROGRAMPARAMETERDVNVPROC staticPointerglGetProgramParameterdvNV = (MYPFNGLGETPROGRAMPARAMETERDVNVPROC) 0;
	static MYPFNGLGETPROGRAMPARAMETERFVNVPROC staticPointerglGetProgramParameterfvNV = (MYPFNGLGETPROGRAMPARAMETERFVNVPROC) 0;
	static MYPFNGLGETPROGRAMIVNVPROC staticPointerglGetProgramivNV = (MYPFNGLGETPROGRAMIVNVPROC) 0;
	static MYPFNGLGETPROGRAMSTRINGNVPROC staticPointerglGetProgramStringNV = (MYPFNGLGETPROGRAMSTRINGNVPROC) 0;
	static MYPFNGLGETTRACKMATRIXIVNVPROC staticPointerglGetTrackMatrixivNV = (MYPFNGLGETTRACKMATRIXIVNVPROC) 0;
	static MYPFNGLGETVERTEXATTRIBDVNVPROC staticPointerglGetVertexAttribdvNV = (MYPFNGLGETVERTEXATTRIBDVNVPROC) 0;
	static MYPFNGLGETVERTEXATTRIBFVNVPROC staticPointerglGetVertexAttribfvNV = (MYPFNGLGETVERTEXATTRIBFVNVPROC) 0;
	static MYPFNGLGETVERTEXATTRIBIVNVPROC staticPointerglGetVertexAttribivNV = (MYPFNGLGETVERTEXATTRIBIVNVPROC) 0;
	static MYPFNGLGETVERTEXATTRIBPOINTERVNVPROC staticPointerglGetVertexAttribPointervNV = (MYPFNGLGETVERTEXATTRIBPOINTERVNVPROC) 0;
	static MYPFNGLISPROGRAMNVPROC staticPointerglIsProgramNV = (MYPFNGLISPROGRAMNVPROC) 0;
	static MYPFNGLLOADPROGRAMNVPROC staticPointerglLoadProgramNV = (MYPFNGLLOADPROGRAMNVPROC) 0;
	static MYPFNGLPROGRAMPARAMETER4DNVPROC staticPointerglProgramParameter4dNV = (MYPFNGLPROGRAMPARAMETER4DNVPROC) 0;
	static MYPFNGLPROGRAMPARAMETER4DVNVPROC staticPointerglProgramParameter4dvNV = (MYPFNGLPROGRAMPARAMETER4DVNVPROC) 0;
	static MYPFNGLPROGRAMPARAMETER4FNVPROC staticPointerglProgramParameter4fNV = (MYPFNGLPROGRAMPARAMETER4FNVPROC) 0;
	static MYPFNGLPROGRAMPARAMETER4FVNVPROC staticPointerglProgramParameter4fvNV = (MYPFNGLPROGRAMPARAMETER4FVNVPROC) 0;
	static MYPFNGLPROGRAMPARAMETERS4DVNVPROC staticPointerglProgramParameters4dvNV = (MYPFNGLPROGRAMPARAMETERS4DVNVPROC) 0;
	static MYPFNGLPROGRAMPARAMETERS4FVNVPROC staticPointerglProgramParameters4fvNV = (MYPFNGLPROGRAMPARAMETERS4FVNVPROC) 0;
	static MYPFNGLREQUESTRESIDENTPROGRAMSNVPROC staticPointerglRequestResidentProgramsNV = (MYPFNGLREQUESTRESIDENTPROGRAMSNVPROC) 0;
	static MYPFNGLTRACKMATRIXNVPROC staticPointerglTrackMatrixNV = (MYPFNGLTRACKMATRIXNVPROC) 0;
	static MYPFNGLVERTEXATTRIBPOINTERNVPROC staticPointerglVertexAttribPointerNV = (MYPFNGLVERTEXATTRIBPOINTERNVPROC) 0;
	static MYPFNGLVERTEXATTRIB1DNVPROC staticPointerglVertexAttrib1dNV = (MYPFNGLVERTEXATTRIB1DNVPROC) 0;
	static MYPFNGLVERTEXATTRIB1DVNVPROC staticPointerglVertexAttrib1dvNV = (MYPFNGLVERTEXATTRIB1DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB1FNVPROC staticPointerglVertexAttrib1fNV = (MYPFNGLVERTEXATTRIB1FNVPROC) 0;
	static MYPFNGLVERTEXATTRIB1FVNVPROC staticPointerglVertexAttrib1fvNV = (MYPFNGLVERTEXATTRIB1FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB1SNVPROC staticPointerglVertexAttrib1sNV = (MYPFNGLVERTEXATTRIB1SNVPROC) 0;
	static MYPFNGLVERTEXATTRIB1SVNVPROC staticPointerglVertexAttrib1svNV = (MYPFNGLVERTEXATTRIB1SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB2DNVPROC staticPointerglVertexAttrib2dNV = (MYPFNGLVERTEXATTRIB2DNVPROC) 0;
	static MYPFNGLVERTEXATTRIB2DVNVPROC staticPointerglVertexAttrib2dvNV = (MYPFNGLVERTEXATTRIB2DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB2FNVPROC staticPointerglVertexAttrib2fNV = (MYPFNGLVERTEXATTRIB2FNVPROC) 0;
	static MYPFNGLVERTEXATTRIB2FVNVPROC staticPointerglVertexAttrib2fvNV = (MYPFNGLVERTEXATTRIB2FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB2SNVPROC staticPointerglVertexAttrib2sNV = (MYPFNGLVERTEXATTRIB2SNVPROC) 0;
	static MYPFNGLVERTEXATTRIB2SVNVPROC staticPointerglVertexAttrib2svNV = (MYPFNGLVERTEXATTRIB2SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB3DNVPROC staticPointerglVertexAttrib3dNV = (MYPFNGLVERTEXATTRIB3DNVPROC) 0;
	static MYPFNGLVERTEXATTRIB3DVNVPROC staticPointerglVertexAttrib3dvNV = (MYPFNGLVERTEXATTRIB3DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB3FNVPROC staticPointerglVertexAttrib3fNV = (MYPFNGLVERTEXATTRIB3FNVPROC) 0;
	static MYPFNGLVERTEXATTRIB3FVNVPROC staticPointerglVertexAttrib3fvNV = (MYPFNGLVERTEXATTRIB3FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB3SNVPROC staticPointerglVertexAttrib3sNV = (MYPFNGLVERTEXATTRIB3SNVPROC) 0;
	static MYPFNGLVERTEXATTRIB3SVNVPROC staticPointerglVertexAttrib3svNV = (MYPFNGLVERTEXATTRIB3SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4DNVPROC staticPointerglVertexAttrib4dNV = (MYPFNGLVERTEXATTRIB4DNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4DVNVPROC staticPointerglVertexAttrib4dvNV = (MYPFNGLVERTEXATTRIB4DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4FNVPROC staticPointerglVertexAttrib4fNV = (MYPFNGLVERTEXATTRIB4FNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4FVNVPROC staticPointerglVertexAttrib4fvNV = (MYPFNGLVERTEXATTRIB4FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4SNVPROC staticPointerglVertexAttrib4sNV = (MYPFNGLVERTEXATTRIB4SNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4SVNVPROC staticPointerglVertexAttrib4svNV = (MYPFNGLVERTEXATTRIB4SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4UBNVPROC staticPointerglVertexAttrib4ubNV = (MYPFNGLVERTEXATTRIB4UBNVPROC) 0;
	static MYPFNGLVERTEXATTRIB4UBVNVPROC staticPointerglVertexAttrib4ubvNV = (MYPFNGLVERTEXATTRIB4UBVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS1DVNVPROC staticPointerglVertexAttribs1dvNV = (MYPFNGLVERTEXATTRIBS1DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS1FVNVPROC staticPointerglVertexAttribs1fvNV = (MYPFNGLVERTEXATTRIBS1FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS1SVNVPROC staticPointerglVertexAttribs1svNV = (MYPFNGLVERTEXATTRIBS1SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS2DVNVPROC staticPointerglVertexAttribs2dvNV = (MYPFNGLVERTEXATTRIBS2DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS2FVNVPROC staticPointerglVertexAttribs2fvNV = (MYPFNGLVERTEXATTRIBS2FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS2SVNVPROC staticPointerglVertexAttribs2svNV = (MYPFNGLVERTEXATTRIBS2SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS3DVNVPROC staticPointerglVertexAttribs3dvNV = (MYPFNGLVERTEXATTRIBS3DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS3FVNVPROC staticPointerglVertexAttribs3fvNV = (MYPFNGLVERTEXATTRIBS3FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS3SVNVPROC staticPointerglVertexAttribs3svNV = (MYPFNGLVERTEXATTRIBS3SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS4DVNVPROC staticPointerglVertexAttribs4dvNV = (MYPFNGLVERTEXATTRIBS4DVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS4FVNVPROC staticPointerglVertexAttribs4fvNV = (MYPFNGLVERTEXATTRIBS4FVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS4SVNVPROC staticPointerglVertexAttribs4svNV = (MYPFNGLVERTEXATTRIBS4SVNVPROC) 0;
	static MYPFNGLVERTEXATTRIBS4UBVNVPROC staticPointerglVertexAttribs4ubvNV = (MYPFNGLVERTEXATTRIBS4UBVNVPROC) 0;
// GL_NV_vertex_program

	// GL_ARB_vertex_program
	static MYPFNGLVERTEXATTRIB1DARBPROC staticPointerglVertexAttrib1dARB = (MYPFNGLVERTEXATTRIB1DARBPROC) 0;
	static MYPFNGLVERTEXATTRIB1DVARBPROC staticPointerglVertexAttrib1dvARB = (MYPFNGLVERTEXATTRIB1DVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB1FARBPROC staticPointerglVertexAttrib1fARB = (MYPFNGLVERTEXATTRIB1FARBPROC) 0;
	static MYPFNGLVERTEXATTRIB1FVARBPROC staticPointerglVertexAttrib1fvARB = (MYPFNGLVERTEXATTRIB1FVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB1SARBPROC staticPointerglVertexAttrib1sARB = (MYPFNGLVERTEXATTRIB1SARBPROC) 0;
	static MYPFNGLVERTEXATTRIB1SVARBPROC staticPointerglVertexAttrib1svARB = (MYPFNGLVERTEXATTRIB1SVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB2DARBPROC staticPointerglVertexAttrib2dARB = (MYPFNGLVERTEXATTRIB2DARBPROC) 0;
	static MYPFNGLVERTEXATTRIB2DVARBPROC staticPointerglVertexAttrib2dvARB = (MYPFNGLVERTEXATTRIB2DVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB2FARBPROC staticPointerglVertexAttrib2fARB = (MYPFNGLVERTEXATTRIB2FARBPROC) 0;
	static MYPFNGLVERTEXATTRIB2FVARBPROC staticPointerglVertexAttrib2fvARB = (MYPFNGLVERTEXATTRIB2FVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB2SARBPROC staticPointerglVertexAttrib2sARB = (MYPFNGLVERTEXATTRIB2SARBPROC) 0;
	static MYPFNGLVERTEXATTRIB2SVARBPROC staticPointerglVertexAttrib2svARB = (MYPFNGLVERTEXATTRIB2SVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB3DARBPROC staticPointerglVertexAttrib3dARB = (MYPFNGLVERTEXATTRIB3DARBPROC) 0;
	static MYPFNGLVERTEXATTRIB3DVARBPROC staticPointerglVertexAttrib3dvARB = (MYPFNGLVERTEXATTRIB3DVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB3FARBPROC staticPointerglVertexAttrib3fARB = (MYPFNGLVERTEXATTRIB3FARBPROC) 0;
	static MYPFNGLVERTEXATTRIB3FVARBPROC staticPointerglVertexAttrib3fvARB = (MYPFNGLVERTEXATTRIB3FVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB3SARBPROC staticPointerglVertexAttrib3sARB = (MYPFNGLVERTEXATTRIB3SARBPROC) 0;
	static MYPFNGLVERTEXATTRIB3SVARBPROC staticPointerglVertexAttrib3svARB = (MYPFNGLVERTEXATTRIB3SVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NBVARBPROC staticPointerglVertexAttrib4NbvARB = (MYPFNGLVERTEXATTRIB4NBVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NIVARBPROC staticPointerglVertexAttrib4NivARB = (MYPFNGLVERTEXATTRIB4NIVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NSVARBPROC staticPointerglVertexAttrib4NsvARB = (MYPFNGLVERTEXATTRIB4NSVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NUBARBPROC staticPointerglVertexAttrib4NubARB = (MYPFNGLVERTEXATTRIB4NUBARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NUBVARBPROC staticPointerglVertexAttrib4NubvARB = (MYPFNGLVERTEXATTRIB4NUBVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NUIVARBPROC staticPointerglVertexAttrib4NuivARB = (MYPFNGLVERTEXATTRIB4NUIVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4NUSVARBPROC staticPointerglVertexAttrib4NusvARB = (MYPFNGLVERTEXATTRIB4NUSVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4BVARBPROC staticPointerglVertexAttrib4bvARB = (MYPFNGLVERTEXATTRIB4BVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4DARBPROC staticPointerglVertexAttrib4dARB = (MYPFNGLVERTEXATTRIB4DARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4DVARBPROC staticPointerglVertexAttrib4dvARB = (MYPFNGLVERTEXATTRIB4DVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4FARBPROC staticPointerglVertexAttrib4fARB = (MYPFNGLVERTEXATTRIB4FARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4FVARBPROC staticPointerglVertexAttrib4fvARB = (MYPFNGLVERTEXATTRIB4FVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4IVARBPROC staticPointerglVertexAttrib4ivARB = (MYPFNGLVERTEXATTRIB4IVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4SARBPROC staticPointerglVertexAttrib4sARB = (MYPFNGLVERTEXATTRIB4SARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4SVARBPROC staticPointerglVertexAttrib4svARB = (MYPFNGLVERTEXATTRIB4SVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4UBVARBPROC staticPointerglVertexAttrib4ubvARB = (MYPFNGLVERTEXATTRIB4UBVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4UIVARBPROC staticPointerglVertexAttrib4uivARB = (MYPFNGLVERTEXATTRIB4UIVARBPROC) 0;
	static MYPFNGLVERTEXATTRIB4USVARBPROC staticPointerglVertexAttrib4usvARB = (MYPFNGLVERTEXATTRIB4USVARBPROC) 0;
	static MYPFNGLVERTEXATTRIBPOINTERARBPROC staticPointerglVertexAttribPointerARB = (MYPFNGLVERTEXATTRIBPOINTERARBPROC) 0;
	static MYPFNGLENABLEVERTEXATTRIBARRAYARBPROC staticPointerglEnableVertexAttribArrayARB = (MYPFNGLENABLEVERTEXATTRIBARRAYARBPROC) 0;
	static MYPFNGLDISABLEVERTEXATTRIBARRAYARBPROC staticPointerglDisableVertexAttribArrayARB = (MYPFNGLDISABLEVERTEXATTRIBARRAYARBPROC) 0;
	static MYPFNGLPROGRAMSTRINGARBPROC staticPointerglProgramStringARB = (MYPFNGLPROGRAMSTRINGARBPROC) 0;
	static MYPFNGLBINDPROGRAMARBPROC staticPointerglBindProgramARB = (MYPFNGLBINDPROGRAMARBPROC) 0;
	static MYPFNGLDELETEPROGRAMSARBPROC staticPointerglDeleteProgramsARB = (MYPFNGLDELETEPROGRAMSARBPROC) 0;
	static MYPFNGLGENPROGRAMSARBPROC staticPointerglGenProgramsARB = (MYPFNGLGENPROGRAMSARBPROC) 0;
	static MYPFNGLPROGRAMENVPARAMETER4DARBPROC staticPointerglProgramEnvParameter4dARB = (MYPFNGLPROGRAMENVPARAMETER4DARBPROC) 0;
	static MYPFNGLPROGRAMENVPARAMETER4DVARBPROC staticPointerglProgramEnvParameter4dvARB = (MYPFNGLPROGRAMENVPARAMETER4DVARBPROC) 0;
	static MYPFNGLPROGRAMENVPARAMETER4FARBPROC staticPointerglProgramEnvParameter4fARB = (MYPFNGLPROGRAMENVPARAMETER4FARBPROC) 0;
	static MYPFNGLPROGRAMENVPARAMETER4FVARBPROC staticPointerglProgramEnvParameter4fvARB = (MYPFNGLPROGRAMENVPARAMETER4FVARBPROC) 0;
	static MYPFNGLPROGRAMLOCALPARAMETER4DARBPROC staticPointerglProgramLocalParameter4dARB = (MYPFNGLPROGRAMLOCALPARAMETER4DARBPROC) 0;
	static MYPFNGLPROGRAMLOCALPARAMETER4DVARBPROC staticPointerglProgramLocalParameter4dvARB = (MYPFNGLPROGRAMLOCALPARAMETER4DVARBPROC) 0;
	static MYPFNGLPROGRAMLOCALPARAMETER4FARBPROC staticPointerglProgramLocalParameter4fARB = (MYPFNGLPROGRAMLOCALPARAMETER4FARBPROC) 0;
	static MYPFNGLPROGRAMLOCALPARAMETER4FVARBPROC staticPointerglProgramLocalParameter4fvARB = (MYPFNGLPROGRAMLOCALPARAMETER4FVARBPROC) 0;
	static MYPFNGLGETPROGRAMENVPARAMETERDVARBPROC staticPointerglGetProgramEnvParameterdvARB = (MYPFNGLGETPROGRAMENVPARAMETERDVARBPROC) 0;
	static MYPFNGLGETPROGRAMENVPARAMETERFVARBPROC staticPointerglGetProgramEnvParameterfvARB = (MYPFNGLGETPROGRAMENVPARAMETERFVARBPROC) 0;
	static MYPFNGLGETPROGRAMLOCALPARAMETERDVARBPROC staticPointerglGetProgramLocalParameterdvARB = (MYPFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) 0;
	static MYPFNGLGETPROGRAMLOCALPARAMETERFVARBPROC staticPointerglGetProgramLocalParameterfvARB = (MYPFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) 0;
	static MYPFNGLGETPROGRAMIVARBPROC staticPointerglGetProgramivARB = (MYPFNGLGETPROGRAMIVARBPROC) 0;
	static MYPFNGLGETPROGRAMSTRINGARBPROC staticPointerglGetProgramStringARB = (MYPFNGLGETPROGRAMSTRINGARBPROC) 0;
	static MYPFNGLGETVERTEXATTRIBDVARBPROC staticPointerglGetVertexAttribdvARB = (MYPFNGLGETVERTEXATTRIBDVARBPROC) 0;
	static MYPFNGLGETVERTEXATTRIBFVARBPROC staticPointerglGetVertexAttribfvARB = (MYPFNGLGETVERTEXATTRIBFVARBPROC) 0;
	static MYPFNGLGETVERTEXATTRIBIVARBPROC staticPointerglGetVertexAttribivARB = (MYPFNGLGETVERTEXATTRIBIVARBPROC) 0;
	static MYPFNGLGETVERTEXATTRIBPOINTERVARBPROC staticPointerglGetVertexAttribPointervARB = (MYPFNGLGETVERTEXATTRIBPOINTERVARBPROC) 0;
	static MYPFNGLISPROGRAMARBPROC staticPointerglIsProgramARB = (MYPFNGLISPROGRAMARBPROC) 0;
	// End GL_ARB_vertex_program

	// GL_ARB_fragment_program
	// No new functions.
	// End GL_ARB_fragment_program




















	void initStaticPointers() {
		if (staticPointersInitialized==true) 
			return;

		staticPointersInitialized = true;

#if defined(GL_VERSION_1_2) && GL_VERSION_1_2
	staticPointerglBlendColor = (MYPFNGLBLENDCOLORPROC)glBlendColor;
	staticPointerglBlendEquation = (MYPFNGLBLENDEQUATIONPROC)glBlendEquation;
	staticPointerglDrawRangeElements = (MYPFNGLDRAWRANGEELEMENTSPROC)glDrawRangeElements;
	staticPointerglColorTable = (MYPFNGLCOLORTABLEPROC)glColorTable;
	staticPointerglColorTableParameterfv = (MYPFNGLCOLORTABLEPARAMETERFVPROC)glColorTableParameterfv;
	staticPointerglColorTableParameteriv = (MYPFNGLCOLORTABLEPARAMETERIVPROC)glColorTableParameteriv;
	staticPointerglCopyColorTable = (MYPFNGLCOPYCOLORTABLEPROC)glCopyColorTable;
	staticPointerglGetColorTable = (MYPFNGLGETCOLORTABLEPROC)glGetColorTable;
	staticPointerglGetColorTableParameterfv = (MYPFNGLGETCOLORTABLEPARAMETERFVPROC)glGetColorTableParameterfv;
	staticPointerglGetColorTableParameteriv = (MYPFNGLGETCOLORTABLEPARAMETERIVPROC)glGetColorTableParameteriv;
	staticPointerglColorSubTable = (MYPFNGLCOLORSUBTABLEPROC)glColorSubTable;
	staticPointerglCopyColorSubTable = (MYPFNGLCOPYCOLORSUBTABLEPROC)glCopyColorSubTable;
	staticPointerglConvolutionFilter1D = (MYPFNGLCONVOLUTIONFILTER1DPROC)glConvolutionFilter1D;
	staticPointerglConvolutionFilter2D = (MYPFNGLCONVOLUTIONFILTER2DPROC)glConvolutionFilter2D;
	staticPointerglConvolutionParameterf = (MYPFNGLCONVOLUTIONPARAMETERFPROC)glConvolutionParameterf;
	staticPointerglConvolutionParameterfv = (MYPFNGLCONVOLUTIONPARAMETERFVPROC)glConvolutionParameterfv;
	staticPointerglConvolutionParameteri = (MYPFNGLCONVOLUTIONPARAMETERIPROC)glConvolutionParameteri;
	staticPointerglConvolutionParameteriv = (MYPFNGLCONVOLUTIONPARAMETERIVPROC)glConvolutionParameteriv;
	staticPointerglCopyConvolutionFilter1D = (MYPFNGLCOPYCONVOLUTIONFILTER1DPROC)glCopyConvolutionFilter1D;
	staticPointerglCopyConvolutionFilter2D = (MYPFNGLCOPYCONVOLUTIONFILTER2DPROC)glCopyConvolutionFilter2D;
	staticPointerglGetConvolutionFilter = (MYPFNGLGETCONVOLUTIONFILTERPROC)glGetConvolutionFilter;
	staticPointerglGetConvolutionParameterfv = (MYPFNGLGETCONVOLUTIONPARAMETERFVPROC)glGetConvolutionParameterfv;
	staticPointerglGetConvolutionParameteriv = (MYPFNGLGETCONVOLUTIONPARAMETERIVPROC)glGetConvolutionParameteriv;
	staticPointerglGetSeparableFilter = (MYPFNGLGETSEPARABLEFILTERPROC)glGetSeparableFilter;
	staticPointerglSeparableFilter2D = (MYPFNGLSEPARABLEFILTER2DPROC)glSeparableFilter2D;
	staticPointerglGetHistogram = (MYPFNGLGETHISTOGRAMPROC)glGetHistogram;
	staticPointerglGetHistogramParameterfv = (MYPFNGLGETHISTOGRAMPARAMETERFVPROC)glGetHistogramParameterfv;
	staticPointerglGetHistogramParameteriv = (MYPFNGLGETHISTOGRAMPARAMETERIVPROC)glGetHistogramParameteriv;
	staticPointerglGetMinmax = (MYPFNGLGETMINMAXPROC)glGetMinmax;
	staticPointerglGetMinmaxParameterfv = (MYPFNGLGETMINMAXPARAMETERFVPROC)glGetMinmaxParameterfv;
	staticPointerglGetMinmaxParameteriv = (MYPFNGLGETMINMAXPARAMETERIVPROC)glGetMinmaxParameteriv;
	staticPointerglHistogram = (MYPFNGLHISTOGRAMPROC)glHistogram;
	staticPointerglMinmax = (MYPFNGLMINMAXPROC)glMinmax;
	staticPointerglResetHistogram = (MYPFNGLRESETHISTOGRAMPROC)glResetHistogram;
	staticPointerglResetMinmax = (MYPFNGLRESETMINMAXPROC)glResetMinmax;
	staticPointerglTexImage3D = (MYPFNGLTEXIMAGE3DPROC)glTexImage3D;
	staticPointerglTexSubImage3D = (MYPFNGLTEXSUBIMAGE3DPROC)glTexSubImage3D;
	staticPointerglCopyTexSubImage3D = (MYPFNGLCOPYTEXSUBIMAGE3DPROC)glCopyTexSubImage3D;
#else
	staticPointerglBlendColor = 0;
	staticPointerglBlendEquation = 0;
	staticPointerglDrawRangeElements = 0;
	staticPointerglColorTable = 0;
	staticPointerglColorTableParameterfv = 0;
	staticPointerglColorTableParameteriv = 0;
	staticPointerglCopyColorTable = 0;
	staticPointerglGetColorTable = 0;
	staticPointerglGetColorTableParameterfv = 0;
	staticPointerglGetColorTableParameteriv = 0;
	staticPointerglColorSubTable = 0;
	staticPointerglCopyColorSubTable = 0;
	staticPointerglConvolutionFilter1D = 0;
	staticPointerglConvolutionFilter2D = 0;
	staticPointerglConvolutionParameterf = 0;
	staticPointerglConvolutionParameterfv = 0;
	staticPointerglConvolutionParameteri = 0;
	staticPointerglConvolutionParameteriv = 0;
	staticPointerglCopyConvolutionFilter1D = 0;
	staticPointerglCopyConvolutionFilter2D = 0;
	staticPointerglGetConvolutionFilter = 0;
	staticPointerglGetConvolutionParameterfv = 0;
	staticPointerglGetConvolutionParameteriv = 0;
	staticPointerglGetSeparableFilter = 0;
	staticPointerglSeparableFilter2D = 0;
	staticPointerglGetHistogram = 0;
	staticPointerglGetHistogramParameterfv = 0;
	staticPointerglGetHistogramParameteriv = 0;
	staticPointerglGetMinmax = 0;
	staticPointerglGetMinmaxParameterfv = 0;
	staticPointerglGetMinmaxParameteriv = 0;
	staticPointerglHistogram = 0;
	staticPointerglMinmax = 0;
	staticPointerglResetHistogram = 0;
	staticPointerglResetMinmax = 0;
	staticPointerglTexImage3D = 0;
	staticPointerglTexSubImage3D = 0;
	staticPointerglCopyTexSubImage3D = 0;
#endif
	// End OpenGL version 1.2

#if defined(GL_VERSION_1_3) && GL_VERSION_1_3
	staticPointerglActiveTexture = (MYPFNGLACTIVETEXTUREPROC) glActiveTexture;
	staticPointerglClientActiveTexture = (MYPFNGLCLIENTACTIVETEXTUREPROC) glClientActiveTexture;
	staticPointerglMultiTexCoord1d = (MYPFNGLMULTITEXCOORD1DPROC) glMultiTexCoord1d;
	staticPointerglMultiTexCoord1dv = (MYPFNGLMULTITEXCOORD1DVPROC) glMultiTexCoord1dv;
	staticPointerglMultiTexCoord1f = (MYPFNGLMULTITEXCOORD1FPROC) glMultiTexCoord1f;
	staticPointerglMultiTexCoord1fv = (MYPFNGLMULTITEXCOORD1FVPROC) glMultiTexCoord1fv;
	staticPointerglMultiTexCoord1i = (MYPFNGLMULTITEXCOORD1IPROC) glMultiTexCoord1i;
	staticPointerglMultiTexCoord1iv = (MYPFNGLMULTITEXCOORD1IVPROC) glMultiTexCoord1iv;
	staticPointerglMultiTexCoord1s = (MYPFNGLMULTITEXCOORD1SPROC) glMultiTexCoord1s;
	staticPointerglMultiTexCoord1sv = (MYPFNGLMULTITEXCOORD1SVPROC) glMultiTexCoord1sv;
	staticPointerglMultiTexCoord2d = (MYPFNGLMULTITEXCOORD2DPROC) glMultiTexCoord2d;
	staticPointerglMultiTexCoord2dv = (MYPFNGLMULTITEXCOORD2DVPROC) glMultiTexCoord2dv;
	staticPointerglMultiTexCoord2f = (MYPFNGLMULTITEXCOORD2FPROC) glMultiTexCoord2f;
	staticPointerglMultiTexCoord2fv = (MYPFNGLMULTITEXCOORD2FVPROC) glMultiTexCoord2fv;
	staticPointerglMultiTexCoord2i = (MYPFNGLMULTITEXCOORD2IPROC) glMultiTexCoord2i;
	staticPointerglMultiTexCoord2iv = (MYPFNGLMULTITEXCOORD2IVPROC) glMultiTexCoord2iv;
	staticPointerglMultiTexCoord2s = (MYPFNGLMULTITEXCOORD2SPROC) glMultiTexCoord2s;
	staticPointerglMultiTexCoord2sv = (MYPFNGLMULTITEXCOORD2SVPROC) glMultiTexCoord2sv;
	staticPointerglMultiTexCoord3d = (MYPFNGLMULTITEXCOORD3DPROC) glMultiTexCoord3d;
	staticPointerglMultiTexCoord3dv = (MYPFNGLMULTITEXCOORD3DVPROC) glMultiTexCoord3dv;
	staticPointerglMultiTexCoord3f = (MYPFNGLMULTITEXCOORD3FPROC) glMultiTexCoord3f;
	staticPointerglMultiTexCoord3fv = (MYPFNGLMULTITEXCOORD3FVPROC) glMultiTexCoord3fv;
	staticPointerglMultiTexCoord3i = (MYPFNGLMULTITEXCOORD3IPROC) glMultiTexCoord3i;
	staticPointerglMultiTexCoord3iv = (MYPFNGLMULTITEXCOORD3IVPROC) glMultiTexCoord3iv;
	staticPointerglMultiTexCoord3s = (MYPFNGLMULTITEXCOORD3SPROC) glMultiTexCoord3s;
	staticPointerglMultiTexCoord3sv = (MYPFNGLMULTITEXCOORD3SVPROC) glMultiTexCoord3sv;
	staticPointerglMultiTexCoord4d = (MYPFNGLMULTITEXCOORD4DPROC) glMultiTexCoord4d;
	staticPointerglMultiTexCoord4dv = (MYPFNGLMULTITEXCOORD4DVPROC) glMultiTexCoord4dv;
	staticPointerglMultiTexCoord4f = (MYPFNGLMULTITEXCOORD4FPROC) glMultiTexCoord4f;
	staticPointerglMultiTexCoord4fv = (MYPFNGLMULTITEXCOORD4FVPROC) glMultiTexCoord4fv;
	staticPointerglMultiTexCoord4i = (MYPFNGLMULTITEXCOORD4IPROC) glMultiTexCoord4i;
	staticPointerglMultiTexCoord4iv = (MYPFNGLMULTITEXCOORD4IVPROC) glMultiTexCoord4iv;
	staticPointerglMultiTexCoord4s = (MYPFNGLMULTITEXCOORD4SPROC) glMultiTexCoord4s;
	staticPointerglMultiTexCoord4sv = (MYPFNGLMULTITEXCOORD4SVPROC) glMultiTexCoord4sv;
	staticPointerglLoadTransposeMatrixf = (MYPFNGLLOADTRANSPOSEMATRIXFPROC) glLoadTransposeMatrixf;
	staticPointerglLoadTransposeMatrixd = (MYPFNGLLOADTRANSPOSEMATRIXDPROC) glLoadTransposeMatrixd;
	staticPointerglMultTransposeMatrixf = (MYPFNGLMULTTRANSPOSEMATRIXFPROC) glMultTransposeMatrixf;
	staticPointerglMultTransposeMatrixd = (MYPFNGLMULTTRANSPOSEMATRIXDPROC) glMultTransposeMatrixd;
	staticPointerglSampleCoverage = (MYPFNGLSAMPLECOVERAGEPROC) glSampleCoverage;
	staticPointerglCompressedTexImage3D = (MYPFNGLCOMPRESSEDTEXIMAGE3DPROC) glCompressedTexImage3D;
	staticPointerglCompressedTexImage2D = (MYPFNGLCOMPRESSEDTEXIMAGE2DPROC) glCompressedTexImage2D;
	staticPointerglCompressedTexImage1D = (MYPFNGLCOMPRESSEDTEXIMAGE1DPROC) glCompressedTexImage1D;
	staticPointerglCompressedTexSubImage3D = (MYPFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) glCompressedTexSubImage3D;
	staticPointerglCompressedTexSubImage2D = (MYPFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) glCompressedTexSubImage2D;
	staticPointerglCompressedTexSubImage1D = (MYPFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) glCompressedTexSubImage1D;
	staticPointerglGetCompressedTexImage = (MYPFNGLGETCOMPRESSEDTEXIMAGEPROC) glGetCompressedTexImage;
#else
	staticPointerglActiveTexture = 0;
	staticPointerglClientActiveTexture = 0;
	staticPointerglMultiTexCoord1d = 0;
	staticPointerglMultiTexCoord1dv = 0;
	staticPointerglMultiTexCoord1f = 0;
	staticPointerglMultiTexCoord1fv = 0;
	staticPointerglMultiTexCoord1i = 0;
	staticPointerglMultiTexCoord1iv = 0;
	staticPointerglMultiTexCoord1s = 0;
	staticPointerglMultiTexCoord1sv = 0;
	staticPointerglMultiTexCoord2d = 0;
	staticPointerglMultiTexCoord2dv = 0;
	staticPointerglMultiTexCoord2f = 0;
	staticPointerglMultiTexCoord2fv = 0;
	staticPointerglMultiTexCoord2i = 0;
	staticPointerglMultiTexCoord2iv = 0;
	staticPointerglMultiTexCoord2s = 0;
	staticPointerglMultiTexCoord2sv = 0;
	staticPointerglMultiTexCoord3d = 0;
	staticPointerglMultiTexCoord3dv = 0;
	staticPointerglMultiTexCoord3f = 0;
	staticPointerglMultiTexCoord3fv = 0;
	staticPointerglMultiTexCoord3i = 0;
	staticPointerglMultiTexCoord3iv = 0;
	staticPointerglMultiTexCoord3s = 0;
	staticPointerglMultiTexCoord3sv = 0;
	staticPointerglMultiTexCoord4d = 0;
	staticPointerglMultiTexCoord4dv = 0;
	staticPointerglMultiTexCoord4f = 0;
	staticPointerglMultiTexCoord4fv = 0;
	staticPointerglMultiTexCoord4i = 0;
	staticPointerglMultiTexCoord4iv = 0;
	staticPointerglMultiTexCoord4s = 0;
	staticPointerglMultiTexCoord4sv = 0;
	staticPointerglLoadTransposeMatrixf = 0;
	staticPointerglLoadTransposeMatrixd = 0;
	staticPointerglMultTransposeMatrixf = 0;
	staticPointerglMultTransposeMatrixd = 0;
	staticPointerglSampleCoverage = 0;
	staticPointerglCompressedTexImage3D = 0;
	staticPointerglCompressedTexImage2D = 0;
	staticPointerglCompressedTexImage1D = 0;
	staticPointerglCompressedTexSubImage3D = 0;
	staticPointerglCompressedTexSubImage2D = 0;
	staticPointerglCompressedTexSubImage1D = 0;
	staticPointerglGetCompressedTexImage = 0;
#endif
	// End OpenGL version 1.3

#if defined(GL_VERSION_1_4) && GL_VERSION_1_4
	//staticPointerglBlendFuncSeparate = (MYPFNGLBLENDFUNCSEPARATEPROC) glBlendFuncSeparate;
	//staticPointerglFogCoordf = (MYPFNGLFOGCOORDFPROC) glFogCoordf;
	//staticPointerglFogCoordfv = (MYPFNGLFOGCOORDFVPROC) glFogCoordfv;
	//staticPointerglFogCoordd = (MYPFNGLFOGCOORDDPROC) glFogCoordd;
	//staticPointerglFogCoorddv = (MYPFNGLFOGCOORDDVPROC) glFogCoorddv;
	//staticPointerglFogCoordPointer = (MYPFNGLFOGCOORDPOINTERPROC) glFogCoordPointer;
	//staticPointerglMultiDrawArrays = (MYPFNGLMULTIDRAWARRAYSPROC) glMultiDrawArrays;
	//staticPointerglMultiDrawElements = (MYPFNGLMULTIDRAWELEMENTSPROC) glMultiDrawElements;
        // Stupid apple does not like to be compatible so I had to comment these
        // out temporarily
	//staticPointerglPointParameterf = (MYPFNGLPOINTPARAMETERFPROC) glPointParameterf;
	//staticPointerglPointParameterfv = (MYPFNGLPOINTPARAMETERFVPROC) glPointParameterfv;
	//staticPointerglPointParameteri = (MYPFNGLPOINTPARAMETERIPROC) glPointParameteri;
	//staticPointerglPointParameteriv = (MYPFNGLPOINTPARAMETERIVPROC) glPointParameteriv;
	//staticPointerglSecondaryColor3b = (MYPFNGLSECONDARYCOLOR3BPROC) glSecondaryColor3b;
	//staticPointerglSecondaryColor3bv = (MYPFNGLSECONDARYCOLOR3BVPROC) glSecondaryColor3bv;
	//staticPointerglSecondaryColor3d = (MYPFNGLSECONDARYCOLOR3DPROC) glSecondaryColor3d;
	//staticPointerglSecondaryColor3dv = (MYPFNGLSECONDARYCOLOR3DVPROC) glSecondaryColor3dv;
	//staticPointerglSecondaryColor3f = (MYPFNGLSECONDARYCOLOR3FPROC) glSecondaryColor3f;
	//staticPointerglSecondaryColor3fv = (MYPFNGLSECONDARYCOLOR3FVPROC) glSecondaryColor3fv;
	//staticPointerglSecondaryColor3i = (MYPFNGLSECONDARYCOLOR3IPROC) glSecondaryColor3i;
	//staticPointerglSecondaryColor3iv = (MYPFNGLSECONDARYCOLOR3IVPROC) glSecondaryColor3iv;
	//staticPointerglSecondaryColor3s = (MYPFNGLSECONDARYCOLOR3SPROC) glSecondaryColor3s;
	//staticPointerglSecondaryColor3sv = (MYPFNGLSECONDARYCOLOR3SVPROC) glSecondaryColor3sv;
	//staticPointerglSecondaryColor3ub = (MYPFNGLSECONDARYCOLOR3UBPROC) glSecondaryColor3ub;
	//staticPointerglSecondaryColor3ubv = (MYPFNGLSECONDARYCOLOR3UBVPROC) glSecondaryColor3ubv;
	//staticPointerglSecondaryColor3ui = (MYPFNGLSECONDARYCOLOR3UIPROC) glSecondaryColor3ui;
	//staticPointerglSecondaryColor3uiv = (MYPFNGLSECONDARYCOLOR3UIVPROC) glSecondaryColor3uiv;
	//staticPointerglSecondaryColor3us = (MYPFNGLSECONDARYCOLOR3USPROC) glSecondaryColor3us;
	//staticPointerglSecondaryColor3usv = (MYPFNGLSECONDARYCOLOR3USVPROC) glSecondaryColor3usv;
	//staticPointerglSecondaryColorPointer = (MYPFNGLSECONDARYCOLORPOINTERPROC) glSecondaryColorPointer;
	//staticPointerglWindowPos2d = (MYPFNGLWINDOWPOS2DPROC) glWindowPos2d;
	//staticPointerglWindowPos2dv = (MYPFNGLWINDOWPOS2DVPROC) glWindowPos2dv;
	//staticPointerglWindowPos2f = (MYPFNGLWINDOWPOS2FPROC) glWindowPos2f;
	//staticPointerglWindowPos2fv = (MYPFNGLWINDOWPOS2FVPROC) glWindowPos2fv;
	//staticPointerglWindowPos2i = (MYPFNGLWINDOWPOS2IPROC) glWindowPos2i;
	//staticPointerglWindowPos2iv = (MYPFNGLWINDOWPOS2IVPROC) glWindowPos2iv;
	//staticPointerglWindowPos2s = (MYPFNGLWINDOWPOS2SPROC) glWindowPos2s;
	//staticPointerglWindowPos2sv = (MYPFNGLWINDOWPOS2SVPROC) glWindowPos2sv;
	//staticPointerglWindowPos3d = (MYPFNGLWINDOWPOS3DPROC) glWindowPos3d;
	//staticPointerglWindowPos3dv = (MYPFNGLWINDOWPOS3DVPROC) glWindowPos3dv;
	//staticPointerglWindowPos3f = (MYPFNGLWINDOWPOS3FPROC) glWindowPos3f;
	//staticPointerglWindowPos3fv = (MYPFNGLWINDOWPOS3FVPROC) glWindowPos3fv;
	//staticPointerglWindowPos3i = (MYPFNGLWINDOWPOS3IPROC) glWindowPos3i;
	//staticPointerglWindowPos3iv = (MYPFNGLWINDOWPOS3IVPROC) glWindowPos3iv;
	//staticPointerglWindowPos3s = (MYPFNGLWINDOWPOS3SPROC) glWindowPos3s;
	//staticPointerglWindowPos3sv = (MYPFNGLWINDOWPOS3SVPROC) glWindowPos3sv;
#else
	staticPointerglBlendFuncSeparate = 0;
	staticPointerglFogCoordf = 0;
	staticPointerglFogCoordfv = 0;
	staticPointerglFogCoordd = 0;
	staticPointerglFogCoorddv = 0;
	staticPointerglFogCoordPointer = 0;
	staticPointerglMultiDrawArrays = 0;
	staticPointerglMultiDrawElements = 0;
	staticPointerglPointParameterf = 0;
	staticPointerglPointParameterfv = 0;
	staticPointerglPointParameteri = 0;
	staticPointerglPointParameteriv = 0;
	staticPointerglSecondaryColor3b = 0;
	staticPointerglSecondaryColor3bv = 0;
	staticPointerglSecondaryColor3d = 0;
	staticPointerglSecondaryColor3dv = 0;
	staticPointerglSecondaryColor3f = 0;
	staticPointerglSecondaryColor3fv = 0;
	staticPointerglSecondaryColor3i = 0;
	staticPointerglSecondaryColor3iv = 0;
	staticPointerglSecondaryColor3s = 0;
	staticPointerglSecondaryColor3sv = 0;
	staticPointerglSecondaryColor3ub = 0;
	staticPointerglSecondaryColor3ubv = 0;
	staticPointerglSecondaryColor3ui = 0;
	staticPointerglSecondaryColor3uiv = 0;
	staticPointerglSecondaryColor3us = 0;
	staticPointerglSecondaryColor3usv = 0;
	staticPointerglSecondaryColorPointer = 0;
	staticPointerglWindowPos2d = 0;
	staticPointerglWindowPos2dv = 0;
	staticPointerglWindowPos2f = 0;
	staticPointerglWindowPos2fv = 0;
	staticPointerglWindowPos2i = 0;
	staticPointerglWindowPos2iv = 0;
	staticPointerglWindowPos2s = 0;
	staticPointerglWindowPos2sv = 0;
	staticPointerglWindowPos3d = 0;
	staticPointerglWindowPos3dv = 0;
	staticPointerglWindowPos3f = 0;
	staticPointerglWindowPos3fv = 0;
	staticPointerglWindowPos3i = 0;
	staticPointerglWindowPos3iv = 0;
	staticPointerglWindowPos3s = 0;
	staticPointerglWindowPos3sv = 0;
#endif
	// End OpenGL version 1.4

#if defined(GL_EXT_paletted_texture) && GL_EXT_paletted_texture
	staticPointerglColorTableEXT = (MYPFNGLCOLORTABLEEXTPROC)glColorTableEXT;
	staticPointerglGetColorTableEXT = (MYPFNGLGETCOLORTABLEEXTPROC)glGetColorTableEXT;
	staticPointerglGetColorTableParameterivEXT = (MYPFNGLGETCOLORTABLEPARAMETERIVEXTPROC)glGetColorTableParameterivEXT;
	staticPointerglGetColorTableParameterfvEXT = (MYPFNGLGETCOLORTABLEPARAMETERFVEXTPROC)glGetColorTableParameterfvEXT;
#else
	staticPointerglColorTableEXT = (MYPFNGLCOLORTABLEEXTPROC) 0;
	staticPointerglGetColorTableEXT = (MYPFNGLGETCOLORTABLEEXTPROC) 0;
	staticPointerglGetColorTableParameterivEXT = (MYPFNGLGETCOLORTABLEPARAMETERIVEXTPROC) 0;
	staticPointerglGetColorTableParameterfvEXT = (MYPFNGLGETCOLORTABLEPARAMETERFVEXTPROC) 0;
#endif
	// End GL_EXT_paletted_texture

#if defined(GL_ARB_multitexture) && GL_ARB_multitexture
	staticPointerglActiveTextureARB = (MYPFNGLACTIVETEXTUREARBPROC)glActiveTextureARB;
	staticPointerglClientActiveTextureARB = (MYPFNGLCLIENTACTIVETEXTUREARBPROC)glClientActiveTextureARB;
	staticPointerglMultiTexCoord1dARB = (MYPFNGLMULTITEXCOORD1DARBPROC)glMultiTexCoord1dARB;
	staticPointerglMultiTexCoord1dvARB = (MYPFNGLMULTITEXCOORD1DVARBPROC)glMultiTexCoord1dvARB;
	staticPointerglMultiTexCoord1fARB = (MYPFNGLMULTITEXCOORD1FARBPROC)glMultiTexCoord1fARB;
	staticPointerglMultiTexCoord1fvARB = (MYPFNGLMULTITEXCOORD1FVARBPROC)glMultiTexCoord1fvARB;
	staticPointerglMultiTexCoord1iARB = (MYPFNGLMULTITEXCOORD1IARBPROC)glMultiTexCoord1iARB;
	staticPointerglMultiTexCoord1ivARB = (MYPFNGLMULTITEXCOORD1IVARBPROC)glMultiTexCoord1ivARB;
	staticPointerglMultiTexCoord1sARB = (MYPFNGLMULTITEXCOORD1SARBPROC)glMultiTexCoord1sARB;
	staticPointerglMultiTexCoord1svARB = (MYPFNGLMULTITEXCOORD1SVARBPROC)glMultiTexCoord1svARB;
	staticPointerglMultiTexCoord2dARB = (MYPFNGLMULTITEXCOORD2DARBPROC)glMultiTexCoord2dARB;
	staticPointerglMultiTexCoord2dvARB = (MYPFNGLMULTITEXCOORD2DVARBPROC)glMultiTexCoord2dvARB;
	staticPointerglMultiTexCoord2fARB = (MYPFNGLMULTITEXCOORD2FARBPROC)glMultiTexCoord2fARB;
	staticPointerglMultiTexCoord2fvARB = (MYPFNGLMULTITEXCOORD2FVARBPROC)glMultiTexCoord2fvARB;
	staticPointerglMultiTexCoord2iARB = (MYPFNGLMULTITEXCOORD2IARBPROC)glMultiTexCoord2iARB;
	staticPointerglMultiTexCoord2ivARB = (MYPFNGLMULTITEXCOORD2IVARBPROC)glMultiTexCoord2ivARB;
	staticPointerglMultiTexCoord2sARB = (MYPFNGLMULTITEXCOORD2SARBPROC)glMultiTexCoord2sARB;
	staticPointerglMultiTexCoord2svARB = (MYPFNGLMULTITEXCOORD2SVARBPROC)glMultiTexCoord2svARB;
	staticPointerglMultiTexCoord3dARB = (MYPFNGLMULTITEXCOORD3DARBPROC)glMultiTexCoord3dARB;
	staticPointerglMultiTexCoord3dvARB = (MYPFNGLMULTITEXCOORD3DVARBPROC)glMultiTexCoord3dvARB;
	staticPointerglMultiTexCoord3fARB = (MYPFNGLMULTITEXCOORD3FARBPROC)glMultiTexCoord3fARB;
	staticPointerglMultiTexCoord3fvARB = (MYPFNGLMULTITEXCOORD3FVARBPROC)glMultiTexCoord3fvARB;
	staticPointerglMultiTexCoord3iARB = (MYPFNGLMULTITEXCOORD3IARBPROC)glMultiTexCoord3iARB;
	staticPointerglMultiTexCoord3ivARB = (MYPFNGLMULTITEXCOORD3IVARBPROC)glMultiTexCoord3ivARB;
	staticPointerglMultiTexCoord3sARB = (MYPFNGLMULTITEXCOORD3SARBPROC)glMultiTexCoord3sARB;
	staticPointerglMultiTexCoord3svARB = (MYPFNGLMULTITEXCOORD3SVARBPROC)glMultiTexCoord3svARB;
	staticPointerglMultiTexCoord4dARB = (MYPFNGLMULTITEXCOORD4DARBPROC)glMultiTexCoord4dARB;
	staticPointerglMultiTexCoord4dvARB = (MYPFNGLMULTITEXCOORD4DVARBPROC)glMultiTexCoord4dvARB;
	staticPointerglMultiTexCoord4fARB = (MYPFNGLMULTITEXCOORD4FARBPROC)glMultiTexCoord4fARB;
	staticPointerglMultiTexCoord4fvARB = (MYPFNGLMULTITEXCOORD4FVARBPROC)glMultiTexCoord4fvARB;
	staticPointerglMultiTexCoord4iARB = (MYPFNGLMULTITEXCOORD4IARBPROC)glMultiTexCoord4iARB;
	staticPointerglMultiTexCoord4ivARB = (MYPFNGLMULTITEXCOORD4IVARBPROC)glMultiTexCoord4ivARB;
	staticPointerglMultiTexCoord4sARB = (MYPFNGLMULTITEXCOORD4SARBPROC)glMultiTexCoord4sARB;
	staticPointerglMultiTexCoord4svARB = (MYPFNGLMULTITEXCOORD4SVARBPROC)glMultiTexCoord4svARB;
#else
	staticPointerglActiveTextureARB = (MYPFNGLACTIVETEXTUREARBPROC) 0;
	staticPointerglClientActiveTextureARB = (MYPFNGLCLIENTACTIVETEXTUREARBPROC) 0;
	staticPointerglMultiTexCoord1dARB = (MYPFNGLMULTITEXCOORD1DARBPROC) 0;
	staticPointerglMultiTexCoord1dvARB = (MYPFNGLMULTITEXCOORD1DVARBPROC) 0;
	staticPointerglMultiTexCoord1fARB = (MYPFNGLMULTITEXCOORD1FARBPROC) 0;
	staticPointerglMultiTexCoord1fvARB = (MYPFNGLMULTITEXCOORD1FVARBPROC) 0;
	staticPointerglMultiTexCoord1iARB = (MYPFNGLMULTITEXCOORD1IARBPROC) 0;
	staticPointerglMultiTexCoord1ivARB = (MYPFNGLMULTITEXCOORD1IVARBPROC) 0;
	staticPointerglMultiTexCoord1sARB = (MYPFNGLMULTITEXCOORD1SARBPROC) 0;
	staticPointerglMultiTexCoord1svARB = (MYPFNGLMULTITEXCOORD1SVARBPROC) 0;
	staticPointerglMultiTexCoord2dARB = (MYPFNGLMULTITEXCOORD2DARBPROC) 0;
	staticPointerglMultiTexCoord2dvARB = (MYPFNGLMULTITEXCOORD2DVARBPROC) 0;
	staticPointerglMultiTexCoord2fARB = (MYPFNGLMULTITEXCOORD2FARBPROC) 0;
	staticPointerglMultiTexCoord2fvARB = (MYPFNGLMULTITEXCOORD2FVARBPROC) 0;
	staticPointerglMultiTexCoord2iARB = (MYPFNGLMULTITEXCOORD2IARBPROC) 0;
	staticPointerglMultiTexCoord2ivARB = (MYPFNGLMULTITEXCOORD2IVARBPROC) 0;
	staticPointerglMultiTexCoord2sARB = (MYPFNGLMULTITEXCOORD2SARBPROC) 0;
	staticPointerglMultiTexCoord2svARB = (MYPFNGLMULTITEXCOORD2SVARBPROC) 0;
	staticPointerglMultiTexCoord3dARB = (MYPFNGLMULTITEXCOORD3DARBPROC) 0;
	staticPointerglMultiTexCoord3dvARB = (MYPFNGLMULTITEXCOORD3DVARBPROC) 0;
	staticPointerglMultiTexCoord3fARB = (MYPFNGLMULTITEXCOORD3FARBPROC) 0;
	staticPointerglMultiTexCoord3fvARB = (MYPFNGLMULTITEXCOORD3FVARBPROC) 0;
	staticPointerglMultiTexCoord3iARB = (MYPFNGLMULTITEXCOORD3IARBPROC) 0;
	staticPointerglMultiTexCoord3ivARB = (MYPFNGLMULTITEXCOORD3IVARBPROC) 0;
	staticPointerglMultiTexCoord3sARB = (MYPFNGLMULTITEXCOORD3SARBPROC) 0;
	staticPointerglMultiTexCoord3svARB = (MYPFNGLMULTITEXCOORD3SVARBPROC) 0;
	staticPointerglMultiTexCoord4dARB = (MYPFNGLMULTITEXCOORD4DARBPROC) 0;
	staticPointerglMultiTexCoord4dvARB = (MYPFNGLMULTITEXCOORD4DVARBPROC) 0;
	staticPointerglMultiTexCoord4fARB = (MYPFNGLMULTITEXCOORD4FARBPROC) 0;
	staticPointerglMultiTexCoord4fvARB = (MYPFNGLMULTITEXCOORD4FVARBPROC) 0;
	staticPointerglMultiTexCoord4iARB = (MYPFNGLMULTITEXCOORD4IARBPROC) 0;
	staticPointerglMultiTexCoord4ivARB = (MYPFNGLMULTITEXCOORD4IVARBPROC) 0;
	staticPointerglMultiTexCoord4sARB = (MYPFNGLMULTITEXCOORD4SARBPROC) 0;
	staticPointerglMultiTexCoord4svARB = (MYPFNGLMULTITEXCOORD4SVARBPROC) 0;
#endif
	// End GL_ARB_multitexture

#if defined(GL_SGI_texture_color_table) && GL_SGI_texture_color_table
	// No new functions.
#endif
	// End GL_SGI_texture_color_table

#if defined(GL_SGI_color_table) && GL_SGI_color_table
	staticPointerglColorTableSGI = (MYPFNGLCOLORTABLESGIPROC)glColorTableSGI;
	staticPointerglColorTableParameterfvSGI = (MYPFNGLCOLORTABLEPARAMETERFVSGIPROC)glColorTableParameterfvSGI;
	staticPointerglColorTableParameterivSGI = (MYPFNGLCOLORTABLEPARAMETERIVSGIPROC)glColorTableParameterivSGI;
	staticPointerglCopyColorTableSGI = (MYPFNGLCOPYCOLORTABLESGIPROC)glCopyColorTableSGI;
	staticPointerglGetColorTableSGI = (MYPFNGLGETCOLORTABLESGIPROC)glGetColorTableSGI;
	staticPointerglGetColorTableParameterfvSGI = (MYPFNGLGETCOLORTABLEPARAMETERFVSGIPROC)glGetColorTableParameterfvSGI;
	staticPointerglGetColorTableParameterivSGI = (MYPFNGLGETCOLORTABLEPARAMETERIVSGIPROC)glGetColorTableParameterivSGI;
#else
	staticPointerglColorTableSGI = (MYPFNGLCOLORTABLESGIPROC) 0;
	staticPointerglColorTableParameterfvSGI = (MYPFNGLCOLORTABLEPARAMETERFVSGIPROC) 0;
	staticPointerglColorTableParameterivSGI = (MYPFNGLCOLORTABLEPARAMETERIVSGIPROC) 0;
	staticPointerglCopyColorTableSGI = (MYPFNGLCOPYCOLORTABLESGIPROC) 0;
	staticPointerglGetColorTableSGI = (MYPFNGLGETCOLORTABLESGIPROC) 0;
	staticPointerglGetColorTableParameterfvSGI = (MYPFNGLGETCOLORTABLEPARAMETERFVSGIPROC) 0;
	staticPointerglGetColorTableParameterivSGI = (MYPFNGLGETCOLORTABLEPARAMETERIVSGIPROC) 0;
#endif
	// End GL_SGI_color_table

#if defined(GL_SGIS_texture_edge_clamp) && GL_SGIS_texture_edge_clamp
	// No new functions.
#endif
	// End GL_SGIS_texture_edge_clamp

#if defined(GL_EXT_texture3D) && GL_EXT_texture3D
	staticPointerglTexImage3DEXT = (MYPFNGLTEXIMAGE3DEXTPROC)glTexImage3DEXT;
	staticPointerglTexSubImage3DEXT = (MYPFNGLTEXSUBIMAGE3DEXTPROC)glTexSubImage3DEXT;
#else
	staticPointerglTexImage3DEXT = (MYPFNGLTEXIMAGE3DEXTPROC) 0;
	staticPointerglTexSubImage3DEXT = (MYPFNGLTEXSUBIMAGE3DEXTPROC) 0;
#endif
	// End GL_EXT_texture3D

#if defined(GL_NV_fragment_program) && GL_NV_fragment_program
	staticPointerglProgramNamedParameter4fNV = (MYPFNGLPROGRAMNAMEDPARAMETER4FNVPROC)glProgramNamedParameter4fNV;
	staticPointerglProgramNamedParameter4dNV = (MYPFNGLPROGRAMNAMEDPARAMETER4DNVPROC)glProgramNamedParameter4dNV;
	staticPointerglProgramNamedParameter4fvNV = (MYPFNGLPROGRAMNAMEDPARAMETER4FVNVPROC)glProgramNamedParameter4fvNV;
	staticPointerglProgramNamedParameter4dvNV = (MYPFNGLPROGRAMNAMEDPARAMETER4DVNVPROC)glProgramNamedParameter4dvNV;
	staticPointerglGetProgramNamedParameterfvNV = (MYPFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC)glGetProgramNamedParameterfvNV;
	staticPointerglGetProgramNamedParameterdvNV = (MYPFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC)glGetProgramNamedParameterdvNV;
#else
	staticPointerglProgramNamedParameter4fNV = (MYPFNGLPROGRAMNAMEDPARAMETER4FNVPROC) 0;
	staticPointerglProgramNamedParameter4dNV = (MYPFNGLPROGRAMNAMEDPARAMETER4DNVPROC) 0;
	staticPointerglProgramNamedParameter4fvNV = (MYPFNGLPROGRAMNAMEDPARAMETER4FVNVPROC) 0;
	staticPointerglProgramNamedParameter4dvNV = (MYPFNGLPROGRAMNAMEDPARAMETER4DVNVPROC) 0;
	staticPointerglGetProgramNamedParameterfvNV = (MYPFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC) 0;
	staticPointerglGetProgramNamedParameterdvNV = (MYPFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC) 0;
#endif // GL_NV_fragment_program

#if defined(GL_NV_vertex_program) && GL_NV_vertex_program
	staticPointerglAreProgramsResidentNV = (MYPFNGLAREPROGRAMSRESIDENTNVPROC)glAreProgramsResidentNV;
	staticPointerglBindProgramNV = (MYPFNGLBINDPROGRAMNVPROC)glBindProgramNV;
	staticPointerglDeleteProgramsNV = (MYPFNGLDELETEPROGRAMSNVPROC)glDeleteProgramsNV;
	staticPointerglExecuteProgramNV = (MYPFNGLEXECUTEPROGRAMNVPROC)glExecuteProgramNV;
	staticPointerglGenProgramsNV = (MYPFNGLGENPROGRAMSNVPROC)glGenProgramsNV;
	staticPointerglGetProgramParameterdvNV = (MYPFNGLGETPROGRAMPARAMETERDVNVPROC)glGetProgramParameterdvNV;
	staticPointerglGetProgramParameterfvNV = (MYPFNGLGETPROGRAMPARAMETERFVNVPROC)glGetProgramParameterfvNV;
	staticPointerglGetProgramivNV = (MYPFNGLGETPROGRAMIVNVPROC)glGetProgramivNV;
	staticPointerglGetProgramStringNV = (MYPFNGLGETPROGRAMSTRINGNVPROC)glGetProgramStringNV;
	staticPointerglGetTrackMatrixivNV = (MYPFNGLGETTRACKMATRIXIVNVPROC)glGetTrackMatrixivNV;
	staticPointerglGetVertexAttribdvNV = (MYPFNGLGETVERTEXATTRIBDVNVPROC)glGetVertexAttribdvNV;
	staticPointerglGetVertexAttribfvNV = (MYPFNGLGETVERTEXATTRIBFVNVPROC)glGetVertexAttribfvNV;
	staticPointerglGetVertexAttribivNV = (MYPFNGLGETVERTEXATTRIBIVNVPROC)glGetVertexAttribivNV;
	staticPointerglGetVertexAttribPointervNV = (MYPFNGLGETVERTEXATTRIBPOINTERVNVPROC)glGetVertexAttribPointervNV;
	staticPointerglIsProgramNV = (MYPFNGLISPROGRAMNVPROC)glIsProgramNV;
	staticPointerglLoadProgramNV = (MYPFNGLLOADPROGRAMNVPROC)glLoadProgramNV;
	staticPointerglProgramParameter4dNV = (MYPFNGLPROGRAMPARAMETER4DNVPROC)glProgramParameter4dNV;
	staticPointerglProgramParameter4dvNV = (MYPFNGLPROGRAMPARAMETER4DVNVPROC)glProgramParameter4dvNV;
	staticPointerglProgramParameter4fNV = (MYPFNGLPROGRAMPARAMETER4FNVPROC)glProgramParameter4fNV;
	staticPointerglProgramParameter4fvNV = (MYPFNGLPROGRAMPARAMETER4FVNVPROC)glProgramParameter4fvNV;
	staticPointerglProgramParameters4dvNV = (MYPFNGLPROGRAMPARAMETERS4DVNVPROC)glProgramParameters4dvNV;
	staticPointerglProgramParameters4fvNV = (MYPFNGLPROGRAMPARAMETERS4FVNVPROC)glProgramParameters4fvNV;
	staticPointerglRequestResidentProgramsNV = (MYPFNGLREQUESTRESIDENTPROGRAMSNVPROC)glRequestResidentProgramsNV;
	staticPointerglTrackMatrixNV = (MYPFNGLTRACKMATRIXNVPROC)glTrackMatrixNV;
	staticPointerglVertexAttribPointerNV = (MYPFNGLVERTEXATTRIBPOINTERNVPROC)glVertexAttribPointerNV;
	staticPointerglVertexAttrib1dNV = (MYPFNGLVERTEXATTRIB1DNVPROC)glVertexAttrib1dNV;
	staticPointerglVertexAttrib1dvNV = (MYPFNGLVERTEXATTRIB1DVNVPROC)glVertexAttrib1dvNV;
	staticPointerglVertexAttrib1fNV = (MYPFNGLVERTEXATTRIB1FNVPROC)glVertexAttrib1fNV;
	staticPointerglVertexAttrib1fvNV = (MYPFNGLVERTEXATTRIB1FVNVPROC)glVertexAttrib1fvNV;
	staticPointerglVertexAttrib1sNV = (MYPFNGLVERTEXATTRIB1SNVPROC)glVertexAttrib1sNV;
	staticPointerglVertexAttrib1svNV = (MYPFNGLVERTEXATTRIB1SVNVPROC)glVertexAttrib1svNV;
	staticPointerglVertexAttrib2dNV = (MYPFNGLVERTEXATTRIB2DNVPROC)glVertexAttrib2dNV;
	staticPointerglVertexAttrib2dvNV = (MYPFNGLVERTEXATTRIB2DVNVPROC)glVertexAttrib2dvNV;
	staticPointerglVertexAttrib2fNV = (MYPFNGLVERTEXATTRIB2FNVPROC)glVertexAttrib2fNV;
	staticPointerglVertexAttrib2fvNV = (MYPFNGLVERTEXATTRIB2FVNVPROC)glVertexAttrib2fvNV;
	staticPointerglVertexAttrib2sNV = (MYPFNGLVERTEXATTRIB2SNVPROC)glVertexAttrib2sNV;
	staticPointerglVertexAttrib2svNV = (MYPFNGLVERTEXATTRIB2SVNVPROC)glVertexAttrib2svNV;
	staticPointerglVertexAttrib3dNV = (MYPFNGLVERTEXATTRIB3DNVPROC)glVertexAttrib3dNV;
	staticPointerglVertexAttrib3dvNV = (MYPFNGLVERTEXATTRIB3DVNVPROC)glVertexAttrib3dvNV;
	staticPointerglVertexAttrib3fNV = (MYPFNGLVERTEXATTRIB3FNVPROC)glVertexAttrib3fNV;
	staticPointerglVertexAttrib3fvNV = (MYPFNGLVERTEXATTRIB3FVNVPROC)glVertexAttrib3fvNV;
	staticPointerglVertexAttrib3sNV = (MYPFNGLVERTEXATTRIB3SNVPROC)glVertexAttrib3sNV;
	staticPointerglVertexAttrib3svNV = (MYPFNGLVERTEXATTRIB3SVNVPROC)glVertexAttrib3svNV;
	staticPointerglVertexAttrib4dNV = (MYPFNGLVERTEXATTRIB4DNVPROC)glVertexAttrib4dNV;
	staticPointerglVertexAttrib4dvNV = (MYPFNGLVERTEXATTRIB4DVNVPROC)glVertexAttrib4dvNV;
	staticPointerglVertexAttrib4fNV = (MYPFNGLVERTEXATTRIB4FNVPROC)glVertexAttrib4fNV;
	staticPointerglVertexAttrib4fvNV = (MYPFNGLVERTEXATTRIB4FVNVPROC)glVertexAttrib4fvNV;
	staticPointerglVertexAttrib4sNV = (MYPFNGLVERTEXATTRIB4SNVPROC)glVertexAttrib4sNV;
	staticPointerglVertexAttrib4svNV = (MYPFNGLVERTEXATTRIB4SVNVPROC)glVertexAttrib4svNV;
	staticPointerglVertexAttrib4ubNV = (MYPFNGLVERTEXATTRIB4UBNVPROC)glVertexAttrib4ubNV;
	staticPointerglVertexAttrib4ubvNV = (MYPFNGLVERTEXATTRIB4UBVNVPROC)glVertexAttrib4ubvNV;
	staticPointerglVertexAttribs1dvNV = (MYPFNGLVERTEXATTRIBS1DVNVPROC)glVertexAttribs1dvNV;
	staticPointerglVertexAttribs1fvNV = (MYPFNGLVERTEXATTRIBS1FVNVPROC)glVertexAttribs1fvNV;
	staticPointerglVertexAttribs1svNV = (MYPFNGLVERTEXATTRIBS1SVNVPROC)glVertexAttribs1svNV;
	staticPointerglVertexAttribs2dvNV = (MYPFNGLVERTEXATTRIBS2DVNVPROC)glVertexAttribs2dvNV;
	staticPointerglVertexAttribs2fvNV = (MYPFNGLVERTEXATTRIBS2FVNVPROC)glVertexAttribs2fvNV;
	staticPointerglVertexAttribs2svNV = (MYPFNGLVERTEXATTRIBS2SVNVPROC)glVertexAttribs2svNV;
	staticPointerglVertexAttribs3dvNV = (MYPFNGLVERTEXATTRIBS3DVNVPROC)glVertexAttribs3dvNV;
	staticPointerglVertexAttribs3fvNV = (MYPFNGLVERTEXATTRIBS3FVNVPROC)glVertexAttribs3fvNV;
	staticPointerglVertexAttribs3svNV = (MYPFNGLVERTEXATTRIBS3SVNVPROC)glVertexAttribs3svNV;
	staticPointerglVertexAttribs4dvNV = (MYPFNGLVERTEXATTRIBS4DVNVPROC)glVertexAttribs4dvNV;
	staticPointerglVertexAttribs4fvNV = (MYPFNGLVERTEXATTRIBS4FVNVPROC)glVertexAttribs4fvNV;
	staticPointerglVertexAttribs4svNV = (MYPFNGLVERTEXATTRIBS4SVNVPROC)glVertexAttribs4svNV;
	staticPointerglVertexAttribs4ubvNV = (MYPFNGLVERTEXATTRIBS4UBVNVPROC)glVertexAttribs4ubvNV;
#else
	staticPointerglAreProgramsResidentNV = (MYPFNGLAREPROGRAMSRESIDENTNVPROC) 0;
	staticPointerglBindProgramNV = (MYPFNGLBINDPROGRAMNVPROC) 0;
	staticPointerglDeleteProgramsNV = (MYPFNGLDELETEPROGRAMSNVPROC) 0;
	staticPointerglExecuteProgramNV = (MYPFNGLEXECUTEPROGRAMNVPROC) 0;
	staticPointerglGenProgramsNV = (MYPFNGLGENPROGRAMSNVPROC) 0;
	staticPointerglGetProgramParameterdvNV = (MYPFNGLGETPROGRAMPARAMETERDVNVPROC) 0;
	staticPointerglGetProgramParameterfvNV = (MYPFNGLGETPROGRAMPARAMETERFVNVPROC) 0;
	staticPointerglGetProgramivNV = (MYPFNGLGETPROGRAMIVNVPROC) 0;
	staticPointerglGetProgramStringNV = (MYPFNGLGETPROGRAMSTRINGNVPROC) 0;
	staticPointerglGetTrackMatrixivNV = (MYPFNGLGETTRACKMATRIXIVNVPROC) 0;
	staticPointerglGetVertexAttribdvNV = (MYPFNGLGETVERTEXATTRIBDVNVPROC) 0;
	staticPointerglGetVertexAttribfvNV = (MYPFNGLGETVERTEXATTRIBFVNVPROC) 0;
	staticPointerglGetVertexAttribivNV = (MYPFNGLGETVERTEXATTRIBIVNVPROC) 0;
	staticPointerglGetVertexAttribPointervNV = (MYPFNGLGETVERTEXATTRIBPOINTERVNVPROC) 0;
	staticPointerglIsProgramNV = (MYPFNGLISPROGRAMNVPROC) 0;
	staticPointerglLoadProgramNV = (MYPFNGLLOADPROGRAMNVPROC) 0;
	staticPointerglProgramParameter4dNV = (MYPFNGLPROGRAMPARAMETER4DNVPROC) 0;
	staticPointerglProgramParameter4dvNV = (MYPFNGLPROGRAMPARAMETER4DVNVPROC) 0;
	staticPointerglProgramParameter4fNV = (MYPFNGLPROGRAMPARAMETER4FNVPROC) 0;
	staticPointerglProgramParameter4fvNV = (MYPFNGLPROGRAMPARAMETER4FVNVPROC) 0;
	staticPointerglProgramParameters4dvNV = (MYPFNGLPROGRAMPARAMETERS4DVNVPROC) 0;
	staticPointerglProgramParameters4fvNV = (MYPFNGLPROGRAMPARAMETERS4FVNVPROC) 0;
	staticPointerglRequestResidentProgramsNV = (MYPFNGLREQUESTRESIDENTPROGRAMSNVPROC) 0;
	staticPointerglTrackMatrixNV = (MYPFNGLTRACKMATRIXNVPROC) 0;
	staticPointerglVertexAttribPointerNV = (MYPFNGLVERTEXATTRIBPOINTERNVPROC) 0;
	staticPointerglVertexAttrib1dNV = (MYPFNGLVERTEXATTRIB1DNVPROC) 0;
	staticPointerglVertexAttrib1dvNV = (MYPFNGLVERTEXATTRIB1DVNVPROC) 0;
	staticPointerglVertexAttrib1fNV = (MYPFNGLVERTEXATTRIB1FNVPROC) 0;
	staticPointerglVertexAttrib1fvNV = (MYPFNGLVERTEXATTRIB1FVNVPROC) 0;
	staticPointerglVertexAttrib1sNV = (MYPFNGLVERTEXATTRIB1SNVPROC) 0;
	staticPointerglVertexAttrib1svNV = (MYPFNGLVERTEXATTRIB1SVNVPROC) 0;
	staticPointerglVertexAttrib2dNV = (MYPFNGLVERTEXATTRIB2DNVPROC) 0;
	staticPointerglVertexAttrib2dvNV = (MYPFNGLVERTEXATTRIB2DVNVPROC) 0;
	staticPointerglVertexAttrib2fNV = (MYPFNGLVERTEXATTRIB2FNVPROC) 0;
	staticPointerglVertexAttrib2fvNV = (MYPFNGLVERTEXATTRIB2FVNVPROC) 0;
	staticPointerglVertexAttrib2sNV = (MYPFNGLVERTEXATTRIB2SNVPROC) 0;
	staticPointerglVertexAttrib2svNV = (MYPFNGLVERTEXATTRIB2SVNVPROC) 0;
	staticPointerglVertexAttrib3dNV = (MYPFNGLVERTEXATTRIB3DNVPROC) 0;
	staticPointerglVertexAttrib3dvNV = (MYPFNGLVERTEXATTRIB3DVNVPROC) 0;
	staticPointerglVertexAttrib3fNV = (MYPFNGLVERTEXATTRIB3FNVPROC) 0;
	staticPointerglVertexAttrib3fvNV = (MYPFNGLVERTEXATTRIB3FVNVPROC) 0;
	staticPointerglVertexAttrib3sNV = (MYPFNGLVERTEXATTRIB3SNVPROC) 0;
	staticPointerglVertexAttrib3svNV = (MYPFNGLVERTEXATTRIB3SVNVPROC) 0;
	staticPointerglVertexAttrib4dNV = (MYPFNGLVERTEXATTRIB4DNVPROC) 0;
	staticPointerglVertexAttrib4dvNV = (MYPFNGLVERTEXATTRIB4DVNVPROC) 0;
	staticPointerglVertexAttrib4fNV = (MYPFNGLVERTEXATTRIB4FNVPROC) 0;
	staticPointerglVertexAttrib4fvNV = (MYPFNGLVERTEXATTRIB4FVNVPROC) 0;
	staticPointerglVertexAttrib4sNV = (MYPFNGLVERTEXATTRIB4SNVPROC) 0;
	staticPointerglVertexAttrib4svNV = (MYPFNGLVERTEXATTRIB4SVNVPROC) 0;
	staticPointerglVertexAttrib4ubNV = (MYPFNGLVERTEXATTRIB4UBNVPROC) 0;
	staticPointerglVertexAttrib4ubvNV = (MYPFNGLVERTEXATTRIB4UBVNVPROC) 0;
	staticPointerglVertexAttribs1dvNV = (MYPFNGLVERTEXATTRIBS1DVNVPROC) 0;
	staticPointerglVertexAttribs1fvNV = (MYPFNGLVERTEXATTRIBS1FVNVPROC) 0;
	staticPointerglVertexAttribs1svNV = (MYPFNGLVERTEXATTRIBS1SVNVPROC) 0;
	staticPointerglVertexAttribs2dvNV = (MYPFNGLVERTEXATTRIBS2DVNVPROC) 0;
	staticPointerglVertexAttribs2fvNV = (MYPFNGLVERTEXATTRIBS2FVNVPROC) 0;
	staticPointerglVertexAttribs2svNV = (MYPFNGLVERTEXATTRIBS2SVNVPROC) 0;
	staticPointerglVertexAttribs3dvNV = (MYPFNGLVERTEXATTRIBS3DVNVPROC) 0;
	staticPointerglVertexAttribs3fvNV = (MYPFNGLVERTEXATTRIBS3FVNVPROC) 0;
	staticPointerglVertexAttribs3svNV = (MYPFNGLVERTEXATTRIBS3SVNVPROC) 0;
	staticPointerglVertexAttribs4dvNV = (MYPFNGLVERTEXATTRIBS4DVNVPROC) 0;
	staticPointerglVertexAttribs4fvNV = (MYPFNGLVERTEXATTRIBS4FVNVPROC) 0;
	staticPointerglVertexAttribs4svNV = (MYPFNGLVERTEXATTRIBS4SVNVPROC) 0;
	staticPointerglVertexAttribs4ubvNV = (MYPFNGLVERTEXATTRIBS4UBVNVPROC) 0;
#endif // GL_NV_vertex_program

#if defined(GL_ARB_vertex_program) && GL_ARB_vertex_program
	staticPointerglVertexAttrib1dARB = (MYPFNGLVERTEXATTRIB1DARBPROC) glVertexAttrib1dARB;
	staticPointerglVertexAttrib1dvARB = (MYPFNGLVERTEXATTRIB1DVARBPROC) glVertexAttrib1dvARB;
	staticPointerglVertexAttrib1fARB = (MYPFNGLVERTEXATTRIB1FARBPROC) glVertexAttrib1fARB;
	staticPointerglVertexAttrib1fvARB = (MYPFNGLVERTEXATTRIB1FVARBPROC) glVertexAttrib1fvARB;
	staticPointerglVertexAttrib1sARB = (MYPFNGLVERTEXATTRIB1SARBPROC) glVertexAttrib1sARB;
	staticPointerglVertexAttrib1svARB = (MYPFNGLVERTEXATTRIB1SVARBPROC) glVertexAttrib1svARB;
	staticPointerglVertexAttrib2dARB = (MYPFNGLVERTEXATTRIB2DARBPROC) glVertexAttrib2dARB;
	staticPointerglVertexAttrib2dvARB = (MYPFNGLVERTEXATTRIB2DVARBPROC) glVertexAttrib2dvARB;
	staticPointerglVertexAttrib2fARB = (MYPFNGLVERTEXATTRIB2FARBPROC) glVertexAttrib2fARB;
	staticPointerglVertexAttrib2fvARB = (MYPFNGLVERTEXATTRIB2FVARBPROC) glVertexAttrib2fvARB;
	staticPointerglVertexAttrib2sARB = (MYPFNGLVERTEXATTRIB2SARBPROC) glVertexAttrib2sARB;
	staticPointerglVertexAttrib2svARB = (MYPFNGLVERTEXATTRIB2SVARBPROC) glVertexAttrib2svARB;
	staticPointerglVertexAttrib3dARB = (MYPFNGLVERTEXATTRIB3DARBPROC) glVertexAttrib3dARB;
	staticPointerglVertexAttrib3dvARB = (MYPFNGLVERTEXATTRIB3DVARBPROC) glVertexAttrib3dvARB;
	staticPointerglVertexAttrib3fARB = (MYPFNGLVERTEXATTRIB3FARBPROC) glVertexAttrib3fARB;
	staticPointerglVertexAttrib3fvARB = (MYPFNGLVERTEXATTRIB3FVARBPROC) glVertexAttrib3fvARB;
	staticPointerglVertexAttrib3sARB = (MYPFNGLVERTEXATTRIB3SARBPROC) glVertexAttrib3sARB;
	staticPointerglVertexAttrib3svARB = (MYPFNGLVERTEXATTRIB3SVARBPROC) glVertexAttrib3svARB;
	staticPointerglVertexAttrib4NbvARB = (MYPFNGLVERTEXATTRIB4NBVARBPROC) glVertexAttrib4NbvARB;
	staticPointerglVertexAttrib4NivARB = (MYPFNGLVERTEXATTRIB4NIVARBPROC) glVertexAttrib4NivARB;
	staticPointerglVertexAttrib4NsvARB = (MYPFNGLVERTEXATTRIB4NSVARBPROC) glVertexAttrib4NsvARB;
	staticPointerglVertexAttrib4NubARB = (MYPFNGLVERTEXATTRIB4NUBARBPROC) glVertexAttrib4NubARB;
	staticPointerglVertexAttrib4NubvARB = (MYPFNGLVERTEXATTRIB4NUBVARBPROC) glVertexAttrib4NubvARB;
	staticPointerglVertexAttrib4NuivARB = (MYPFNGLVERTEXATTRIB4NUIVARBPROC) glVertexAttrib4NuivARB;
	staticPointerglVertexAttrib4NusvARB = (MYPFNGLVERTEXATTRIB4NUSVARBPROC) glVertexAttrib4NusvARB;
	staticPointerglVertexAttrib4bvARB = (MYPFNGLVERTEXATTRIB4BVARBPROC) glVertexAttrib4bvARB;
	staticPointerglVertexAttrib4dARB = (MYPFNGLVERTEXATTRIB4DARBPROC) glVertexAttrib4dARB;
	staticPointerglVertexAttrib4dvARB = (MYPFNGLVERTEXATTRIB4DVARBPROC) glVertexAttrib4dvARB;
	staticPointerglVertexAttrib4fARB = (MYPFNGLVERTEXATTRIB4FARBPROC) glVertexAttrib4fARB;
	staticPointerglVertexAttrib4fvARB = (MYPFNGLVERTEXATTRIB4FVARBPROC) glVertexAttrib4fvARB;
	staticPointerglVertexAttrib4ivARB = (MYPFNGLVERTEXATTRIB4IVARBPROC) glVertexAttrib4ivARB;
	staticPointerglVertexAttrib4sARB = (MYPFNGLVERTEXATTRIB4SARBPROC) glVertexAttrib4sARB;
	staticPointerglVertexAttrib4svARB = (MYPFNGLVERTEXATTRIB4SVARBPROC) glVertexAttrib4svARB;
	staticPointerglVertexAttrib4ubvARB = (MYPFNGLVERTEXATTRIB4UBVARBPROC) glVertexAttrib4ubvARB;
	staticPointerglVertexAttrib4uivARB = (MYPFNGLVERTEXATTRIB4UIVARBPROC) glVertexAttrib4uivARB;
	staticPointerglVertexAttrib4usvARB = (MYPFNGLVERTEXATTRIB4USVARBPROC) glVertexAttrib4usvARB;
	staticPointerglVertexAttribPointerARB = (MYPFNGLVERTEXATTRIBPOINTERARBPROC) glVertexAttribPointerARB;
	staticPointerglEnableVertexAttribArrayARB = (MYPFNGLENABLEVERTEXATTRIBARRAYARBPROC) glEnableVertexAttribArrayARB;
	staticPointerglDisableVertexAttribArrayARB = (MYPFNGLDISABLEVERTEXATTRIBARRAYARBPROC) glDisableVertexAttribArrayARB;
	staticPointerglProgramStringARB = (MYPFNGLPROGRAMSTRINGARBPROC) glProgramStringARB;
	staticPointerglBindProgramARB = (MYPFNGLBINDPROGRAMARBPROC) glBindProgramARB;
	staticPointerglDeleteProgramsARB = (MYPFNGLDELETEPROGRAMSARBPROC) glDeleteProgramsARB;
	staticPointerglGenProgramsARB = (MYPFNGLGENPROGRAMSARBPROC) glGenProgramsARB;
	staticPointerglProgramEnvParameter4dARB = (MYPFNGLPROGRAMENVPARAMETER4DARBPROC) glProgramEnvParameter4dARB;
	staticPointerglProgramEnvParameter4dvARB = (MYPFNGLPROGRAMENVPARAMETER4DVARBPROC) glProgramEnvParameter4dvARB;
	staticPointerglProgramEnvParameter4fARB = (MYPFNGLPROGRAMENVPARAMETER4FARBPROC) glProgramEnvParameter4fARB;
	staticPointerglProgramEnvParameter4fvARB = (MYPFNGLPROGRAMENVPARAMETER4FVARBPROC) glProgramEnvParameter4fvARB;
	staticPointerglProgramLocalParameter4dARB = (MYPFNGLPROGRAMLOCALPARAMETER4DARBPROC) glProgramLocalParameter4dARB;
	staticPointerglProgramLocalParameter4dvARB = (MYPFNGLPROGRAMLOCALPARAMETER4DVARBPROC) glProgramLocalParameter4dvARB;
	staticPointerglProgramLocalParameter4fARB = (MYPFNGLPROGRAMLOCALPARAMETER4FARBPROC) glProgramLocalParameter4fARB;
	staticPointerglProgramLocalParameter4fvARB = (MYPFNGLPROGRAMLOCALPARAMETER4FVARBPROC) glProgramLocalParameter4fvARB;
	staticPointerglGetProgramEnvParameterdvARB = (MYPFNGLGETPROGRAMENVPARAMETERDVARBPROC) glGetProgramEnvParameterdvARB;
	staticPointerglGetProgramEnvParameterfvARB = (MYPFNGLGETPROGRAMENVPARAMETERFVARBPROC) glGetProgramEnvParameterfvARB;
	staticPointerglGetProgramLocalParameterdvARB = (MYPFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) glGetProgramLocalParameterdvARB;
	staticPointerglGetProgramLocalParameterfvARB = (MYPFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) glGetProgramLocalParameterfvARB;
	staticPointerglGetProgramivARB = (MYPFNGLGETPROGRAMIVARBPROC) glGetProgramivARB;
	staticPointerglGetProgramStringARB = (MYPFNGLGETPROGRAMSTRINGARBPROC) glGetProgramStringARB;
	staticPointerglGetVertexAttribdvARB = (MYPFNGLGETVERTEXATTRIBDVARBPROC) glGetVertexAttribdvARB;
	staticPointerglGetVertexAttribfvARB = (MYPFNGLGETVERTEXATTRIBFVARBPROC) glGetVertexAttribfvARB;
	staticPointerglGetVertexAttribivARB = (MYPFNGLGETVERTEXATTRIBIVARBPROC) glGetVertexAttribivARB;
	staticPointerglGetVertexAttribPointervARB = (MYPFNGLGETVERTEXATTRIBPOINTERVARBPROC) glGetVertexAttribPointervARB;
	staticPointerglIsProgramARB = (MYPFNGLISPROGRAMARBPROC) glIsProgramARB;
#else
	staticPointerglVertexAttrib1dARB = 0;
	staticPointerglVertexAttrib1dvARB = 0;
	staticPointerglVertexAttrib1fARB = 0;
	staticPointerglVertexAttrib1fvARB = 0;
	staticPointerglVertexAttrib1sARB = 0;
	staticPointerglVertexAttrib1svARB = 0;
	staticPointerglVertexAttrib2dARB = 0;
	staticPointerglVertexAttrib2dvARB = 0;
	staticPointerglVertexAttrib2fARB = 0;
	staticPointerglVertexAttrib2fvARB = 0;
	staticPointerglVertexAttrib2sARB = 0;
	staticPointerglVertexAttrib2svARB = 0;
	staticPointerglVertexAttrib3dARB = 0;
	staticPointerglVertexAttrib3dvARB = 0;
	staticPointerglVertexAttrib3fARB = 0;
	staticPointerglVertexAttrib3fvARB = 0;
	staticPointerglVertexAttrib3sARB = 0;
	staticPointerglVertexAttrib3svARB = 0;
	staticPointerglVertexAttrib4NbvARB = 0;
	staticPointerglVertexAttrib4NivARB = 0;
	staticPointerglVertexAttrib4NsvARB = 0;
	staticPointerglVertexAttrib4NubARB = 0;
	staticPointerglVertexAttrib4NubvARB = 0;
	staticPointerglVertexAttrib4NuivARB = 0;
	staticPointerglVertexAttrib4NusvARB = 0;
	staticPointerglVertexAttrib4bvARB = 0;
	staticPointerglVertexAttrib4dARB = 0;
	staticPointerglVertexAttrib4dvARB = 0;
	staticPointerglVertexAttrib4fARB = 0;
	staticPointerglVertexAttrib4fvARB = 0;
	staticPointerglVertexAttrib4ivARB = 0;
	staticPointerglVertexAttrib4sARB = 0;
	staticPointerglVertexAttrib4svARB = 0;
	staticPointerglVertexAttrib4ubvARB = 0;
	staticPointerglVertexAttrib4uivARB = 0;
	staticPointerglVertexAttrib4usvARB = 0;
	staticPointerglVertexAttribPointerARB = 0;
	staticPointerglEnableVertexAttribArrayARB = 0;
	staticPointerglDisableVertexAttribArrayARB = 0;
	staticPointerglProgramStringARB = 0;
	staticPointerglBindProgramARB = 0;
	staticPointerglDeleteProgramsARB = 0;
	staticPointerglGenProgramsARB = 0;
	staticPointerglProgramEnvParameter4dARB = 0;
	staticPointerglProgramEnvParameter4dvARB = 0;
	staticPointerglProgramEnvParameter4fARB = 0;
	staticPointerglProgramEnvParameter4fvARB = 0;
	staticPointerglProgramLocalParameter4dARB = 0;
	staticPointerglProgramLocalParameter4dvARB = 0;
	staticPointerglProgramLocalParameter4fARB = 0;
	staticPointerglProgramLocalParameter4fvARB = 0;
	staticPointerglGetProgramEnvParameterdvARB = 0;
	staticPointerglGetProgramEnvParameterfvARB = 0;
	staticPointerglGetProgramLocalParameterdvARB = 0;
	staticPointerglGetProgramLocalParameterfvARB = 0;
	staticPointerglGetProgramivARB = 0;
	staticPointerglGetProgramStringARB = 0;
	staticPointerglGetVertexAttribdvARB = 0;
	staticPointerglGetVertexAttribfvARB = 0;
	staticPointerglGetVertexAttribivARB = 0;
	staticPointerglGetVertexAttribPointervARB = 0;
	staticPointerglIsProgramARB = 0;
#endif
	// End GL_ARB_vertex_program

	// GL_ARB_fragment_program
	// No new functions.
	// End GL_ARB_fragment_program

	}

#else
	void initStaticPointers() {
		// do nothing
	}
#endif


#endif
