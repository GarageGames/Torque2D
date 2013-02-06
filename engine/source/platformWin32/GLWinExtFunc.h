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


#ifndef WGL_GROUP_BEGIN
#define WGL_GROUP_BEGIN( flag )
#define UNDEF_BEGIN
#endif

#ifndef WGL_GROUP_END
#define WGL_GROUP_END()
#define UNDEF_END
#endif

//WGL_ARB_extensions_string
WGL_GROUP_BEGIN(WGL_ARB_extensions_string)
WGLEXT_FUNCTION( const char*, wglGetExtensionsStringARB, (HDC), return NULL; )
WGL_GROUP_END()

//WGL_EXT_swap_control
WGL_GROUP_BEGIN(WGL_EXT_swap_control)
WGLEXT_FUNCTION( BOOL, wglSwapIntervalEXT,(int), return 0; )
WGLEXT_FUNCTION( int, wglGetSwapIntervalEXT,(void), return 0; )
WGL_GROUP_END()

WGL_GROUP_BEGIN(WGL_3DFX_gamma_control)
WGLEXT_FUNCTION( BOOL, wglGetDeviceGammaRamp3DFX, (HDC, LPVOID), return false; )
WGLEXT_FUNCTION( BOOL, wglSetDeviceGammaRamp3DFX, (HDC, LPVOID), return false; )
WGL_GROUP_END()

#ifdef UNDEF_BEGIN
#undef WGL_GROUP_BEGIN
#undef UNDEF_BEGIN
#endif

#ifdef UNDEF_END
#undef WGL_GROUP_END
#undef UNDEF_END
#endif
