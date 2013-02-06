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

// for the moment, this seems to be the best roundup of
// the npatch extensions on the PC.

#ifndef GL_ATI_pn_triangles
#define GL_ATI_pn_triangles 1

#define GL_PN_TRIANGLES_ATI							0x87F0
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI	0x87F1
#define GL_PN_TRIANGLES_POINT_MODE_ATI				0x87F2
#define GL_PN_TRIANGLES_NORMAL_MODE_ATI				0x87F3
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI		0x87F4
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI		0x87F5
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI		0x87F6
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI		0x87F7
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI   0x87F8

typedef void (APIENTRY *PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);

#endif

#define GL_NPATCH_EXT_STRING		"GL_ATI_pn_triangles"
#define GL_NPATCH_SETINT_STRING		"glPNTrianglesiATI"
typedef PFNGLPNTRIANGLESIATIPROC	PFNNPatchSetInt;

#define GETINT_NPATCH_MAX_LEVEL		GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI
#define GL_NPATCH_FLAG				GL_PN_TRIANGLES_ATI
#define GL_NPATCH_LOD				GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI

#define SETINT_NPATCH_POINTINTERP	GL_PN_TRIANGLES_POINT_MODE_ATI
#define SETINT_NPATCH_NORMALINTERP	GL_PN_TRIANGLES_NORMAL_MODE_ATI

#define NPATCH_POINTINTERP_MIN		GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI
#define NPATCH_POINTINTERP_MAX		GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI

#define NPATCH_NORMALINTERP_MIN		GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI
#define NPATCH_NORMALINTERP_MAX		GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI
