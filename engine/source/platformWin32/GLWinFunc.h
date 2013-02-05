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


WGLD3D_FUNCTION( BOOL,  CopyContext, (HGLRC, HGLRC, UINT), return false; )
WGLD3D_FUNCTION( HGLRC, CreateContext, (HDC), return NULL; )
//WGL_FUNCTION( HGLRC, wglCreateLayerContext, (HDC, S32), return NULL; )
WGLD3D_FUNCTION( BOOL,  DeleteContext, (HGLRC), return false; )
WGLD3D_FUNCTION( HGLRC, GetCurrentContext, (VOID), return NULL; )
WGLD3D_FUNCTION( HDC,   GetCurrentDC, (VOID), return NULL; )
WGLD3D_FUNCTION( PROC,  GetProcAddress, (LPCSTR), return NULL; )
WGLD3D_FUNCTION( BOOL,  MakeCurrent, (HDC, HGLRC), return false; )
WGLD3D_FUNCTION( BOOL,  ShareLists, (HGLRC, HGLRC), return false; )
//WGL_FUNCTION( BOOL,  wglUseFontBitmaps, (HDC, DWORD, DWORD, DWORD), return false; )
//WGL_FUNCTION( BOOL,  wglUseFontOutlines, (HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, S32, LPGLYPHMETRICSFLOAT), return false; )
WGLD3D_FUNCTION( BOOL,  DescribeLayerPlane, (HDC, S32, S32, UINT, LPLAYERPLANEDESCRIPTOR), return false; )
//WGL_FUNCTION( S32,   wglSetLayerPaletteEntries, (HDC, S32, S32, S32, CONST COLORREF *), return 0; )
//WGL_FUNCTION( S32,   wglGetLayerPaletteEntries, (HDC, S32, S32, S32, COLORREF *), return 0; )
//WGL_FUNCTION( BOOL,  wglRealizeLayerPalette, (HDC, S32, BOOL), return false; )
//WGL_FUNCTION( BOOL,  wglSwapLayerBuffers, (HDC, UINT), return false; )

WGL_FUNCTION( S32,   wglChoosePixelFormat,(HDC, CONST PIXELFORMATDESCRIPTOR *), return 0; )
WGL_FUNCTION( S32,   wglDescribePixelFormat, (HDC, S32, UINT, LPPIXELFORMATDESCRIPTOR), return 0; )
WGL_FUNCTION( S32,   wglGetPixelFormat, (HDC), return 0; )
WGL_FUNCTION( BOOL,  wglSetPixelFormat, (HDC, S32, CONST PIXELFORMATDESCRIPTOR *), return false; )
WGL_FUNCTION( BOOL,  wglSwapBuffers, (HDC), return false; )
