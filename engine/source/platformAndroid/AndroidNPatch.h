//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// AndroidNPatch.h
//
// mac specific implementation(s) of NPatch functionality
// since each platform might use slightly diff methods to control.

// current Mac NPatches is ATI TRUFORM implementation, accessed on OS9 via a
// back door method.  OSX tests for the ATIX extension.

#if !defined(TORQUE_OS_MAC_OSX)
#define AGLSETINT_NPATCH_FLAG          ((unsigned long)500)
#define AGLSETINT_NPATCH_LOD           ((unsigned long)501)
#define AGLSETINT_NPATCH_POINTINTERP   ((unsigned long)502)
#define AGLSETINT_NPATCH_NORMALINTERP  ((unsigned long)503)
#endif

// for the moment, this seems to be the best roundup of
// the npatch extensions on the PC.

#ifndef GL_ATIX_pn_triangles
#define GL_ATIX_pn_triangles 1
#define GL_PN_TRIANGLES_ATIX                            0x6090
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATIX      0x6091
#define GL_PN_TRIANGLES_POINT_MODE_ATIX                 0x6092
#define GL_PN_TRIANGLES_NORMAL_MODE_ATIX                0x6093
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATIX          0x6094
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATIX          0x6095
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATIX           0x6096
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATIX         0x6097
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATIX      0x6098

#if defined(TORQUE_OS_MAC_OSX) // for the moment...
extern void glPNTrianglesiATIX(GLenum pname, GLint param);
extern void glPNTrianglesfATIX(GLenum pname, GLfloat param);
#endif
#endif

typedef void (*PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
//typedef void (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);

#define GL_NPATCH_EXT_STRING        "GL_ATIX_pn_triangles"
#define GL_NPATCH_SETINT_STRING     "glPNTrianglesiATIX"
typedef PFNGLPNTRIANGLESIATIPROC    PFNNPatchSetInt;

#define GETINT_NPATCH_MAX_LEVEL     GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATIX

#define GL_NPATCH_FLAG              GL_PN_TRIANGLES_ATIX

#define SETINT_NPATCH_LOD           GL_PN_TRIANGLES_TESSELATION_LEVEL_ATIX

#define SETINT_NPATCH_POINTINTERP   GL_PN_TRIANGLES_POINT_MODE_ATIX
#define SETINT_NPATCH_NORMALINTERP  GL_PN_TRIANGLES_NORMAL_MODE_ATIX

#define NPATCH_POINTINTERP_MIN      GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATIX
#define NPATCH_POINTINTERP_MAX      GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATIX

#define NPATCH_NORMALINTERP_MIN     GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATIX
#define NPATCH_NORMALINTERP_MAX     GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATIX
