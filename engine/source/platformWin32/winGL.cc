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

#include "platformWin32/platformWin32.h"
#include "platformWin32/platformGL.h"
#include "graphics/dgl.h"
#include "platform/platformVideo.h"
#include "string/unicode.h"

#include "console/console.h"
#include "console/consoleTypes.h"

GLState gGLState;

bool gOpenGLDisablePT         = false;
bool gOpenGLDisableCVA        = false;
bool gOpenGLDisableTEC        = false;
bool gOpenGLDisableARBMT      = false;
bool gOpenGLDisableFC         = false;
bool gOpenGLDisableTCompress  = false;
bool gOpenGLNoEnvColor        = false;
float gOpenGLGammaCorrection  = 0.5;
bool gOpenGLNoDrawArraysAlpha = false;

//------------------------------------------------------------------
//bind functions for each function prototype
//------------------------------------------------------------------

//GL_EXT/ARB
enum {
   ARB_multitexture              = BIT(0),
   ARB_texture_compression       = BIT(1),
   EXT_compiled_vertex_array     = BIT(2),
   EXT_fog_coord                 = BIT(3),
   EXT_paletted_texture          = BIT(4),
   NV_vertex_array_range         = BIT(5),
   EXT_blend_color               = BIT(6),
   EXT_blend_minmax              = BIT(7)
};

//WGL_ARB
enum {
   WGL_ARB_extensions_string  = BIT(0),
   WGL_EXT_swap_control       = BIT(1),
   WGL_3DFX_gamma_control     = BIT(2)
};


//------------------------------------------------------------------
//create dummy functions and set real functions to dummies for:
// -core GL
// -core WGL
// -WGL extensions
//------------------------------------------------------------------

//defines...
//-------------
// we want to declare/define all GL functions here and set them all to a "stub"
// function so that if they're called before they're initialized, they'll spew
// some console spam to make it easier to debug.  We also need to declare/define
// a "dll" version, which will ALWAYS point to the function defined in the dll.
// This for special functionality like wireframe, logging, and performance metrics
// that will override the normal function and do some additional work.  We'll make
// the dll versions extern so that we can seperate out all the performance/logging
// functionality to a seperate file to keep this one a little bit cleaner.  So,
// our macros look like this for the most part (although a lot uglier in practice):

// type name##_t(params) { console_warning; return ret; }
// type name(params) = name##_t;
// extern type dll##name(params = name##_t

#ifndef TORQUE_LIB

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY fn_name##_t)fn_args \
   { Con::printf("Could not load this GL function: %s", #fn_name); fn_body } \
   fn_type (APIENTRY * fn_name)fn_args = fn_name##_t; \
   extern fn_type (APIENTRY * dll##fn_name)fn_args = fn_name##_t;
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { Con::printf("Could not load this WGL function: %s", #fn_name); fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t; \
   extern fn_type (APIENTRY * dlld##fn_name)fn_args = d##fn_name##_t;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY dwgl##fn_name##_t)fn_args \
   { Con::printf("Could not load this WGLD3D function: wgl%s", #fn_name); fn_body } \
   fn_type (APIENTRY * dwgl##fn_name)fn_args = dwgl##fn_name##_t; \
   extern fn_type (APIENTRY * dlldwgl##fn_name)fn_args = dwgl##fn_name##_t;
#define WGLEXT_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { Con::printf("Could not load this WGLEXT function: %s", #fn_name); fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t;

#else

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * fn_name)fn_args = fn_name##_t; \
   extern fn_type (APIENTRY * dll##fn_name)fn_args = fn_name##_t;
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t; \
   extern fn_type (APIENTRY * dlld##fn_name)fn_args = d##fn_name##_t;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY dwgl##fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * dwgl##fn_name)fn_args = dwgl##fn_name##_t; \
   extern fn_type (APIENTRY * dlldwgl##fn_name)fn_args = dwgl##fn_name##_t;
#define WGLEXT_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t;

#endif

//includes...
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#include "platformWin32/GLWinFunc.h"
#include "platformWin32/GLWinExtFunc.h"

//undefs...
#undef GL_FUNCTION
#undef WGL_FUNCTION
#undef WGLD3D_FUNCTION
#undef WGLEXT_FUNCTION

// These functions won't be in the normal OGL dll, so don't give
// errors about them because we know we'll be ok without them
static bool isFnOk(const char* name)
{
   if (dStrcmp(name, "glAvailableVertexBufferEXT") == 0 ||
      dStrcmp(name, "glAllocateVertexBufferEXT") == 0  ||
      dStrcmp(name, "glLockVertexBufferEXT") == 0 ||
      dStrcmp(name, "glUnlockVertexBufferEXT") == 0 ||
      dStrcmp(name, "glSetVertexBufferEXT") == 0 ||
      dStrcmp(name, "glOffsetVertexBufferEXT") == 0 ||
      dStrcmp(name, "glFillVertexBufferEXT") == 0 ||
      dStrcmp(name, "glFreeVertexBufferEXT") == 0)
      return true;
   return false;
}

//------------------------------------------------------------------
//bind functions for each function prototype
//------------------------------------------------------------------
static bool bindGLFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(GetProcAddress( winState.hinstOpenGL, name ));
   bool ok = (bool)addr;
   if( !ok )
   {
      if (!isFnOk(name))
         Con::errorf(ConsoleLogEntry::General, " Missing OpenGL function '%s'", name);
      else
         ok = true;
   }
   else
      fnAddress = addr;
   return ok;
}

static bool bindGLUFunction( void *&fnAddress, const char *name )
{
   void* addr =  (void*)(GetProcAddress( winState.hinstGLU, name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing GLU function '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

static bool bindEXTFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(dwglGetProcAddress( name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing OpenGL extension '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

static bool bindWGLFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(GetProcAddress( winState.hinstOpenGL, name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing WGL function '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

static bool bindWGLEXTFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(dwglGetProcAddress( name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing WGLEXT function '%s'", name);
   else
      fnAddress = addr;

   return (addr != NULL);
}

//------------------------------------------------------------------
//binds for each function group
//------------------------------------------------------------------
static bool bindGLFunctions()
{
   bool result = true;
   #define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   result &= bindGLFunction( *(void**)&dll##fn_name, #fn_name); \
   fn_name = dll##fn_name;
   #include "platform/GLCoreFunc.h"
   #undef GL_FUNCTION
   return result;
}

static bool bindGLUFunctions()
{
   bool result = true;
   #define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   result &= bindGLUFunction( *(void**)&dll##fn_name, #fn_name); \
   fn_name = dll##fn_name;
   #include "platform/GLUFunc.h"
   #undef GL_FUNCTION
   return result;
}

static bool bindEXTFunctions(U32 extMask)
{
   bool result = true;

   #define GL_GROUP_BEGIN( flag ) \
      if( extMask & flag ) {
   #define GL_GROUP_END() }

   #define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   result &= bindEXTFunction( *(void**)&dll##fn_name, #fn_name); \
   fn_name = dll##fn_name;
   #include "platform/GLExtFunc.h"
   #undef GL_FUNCTION

   #undef GL_GROUP_BEGIN
   #undef GL_GROUP_END

   return result;
}

static bool bindWGLFunctions(const char* prefix)
{
   //ugh... the stupid D3D wrapper prefixes some functions
   //with either wd3d or wgl, so we have to account for that
   static char buff[200];
   bool result = true;
   #define WGLD3D_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   dSprintf(buff, 200, "%s%s", prefix, #fn_name); \
   result &= bindWGLFunction( *(void**)&dlldwgl##fn_name, buff); \
   dwgl##fn_name = dlldwgl##fn_name;
   #define WGL_FUNCTION(fn_return, fn_name, fn_args, fn_valud) \
   result &= bindWGLFunction( *(void**)&dlld##fn_name, #fn_name); \
   d##fn_name = dlld##fn_name;
   #include "platformWin32/GLWinFunc.h"
   #undef WGLD3D_FUNCTION
   #undef WGL_FUNCTION

   return result;
}

static bool bindWGLEXTFunctions(U32 extMask)
{
   bool result = true;

   #define WGL_GROUP_BEGIN( flag ) \
      if( extMask & flag ) {
   #define WGL_GROUP_END() }

   #define WGLEXT_FUNCTION(fn_return, fn_name, fn_args, fn_value) result &= bindWGLEXTFunction( *(void**)&d##fn_name, #fn_name);
   #include "platformWin32/GLWinExtFunc.h"
   #undef WGLEXT_FUNCTION

   #undef WGL_GROUP_BEGIN
   #undef WGL_GROUP_END

   return result;
}

//------------------------------------------------------------------
//unbind functions
//------------------------------------------------------------------

//GL core, GL_EXT/ARB, and GLU can all be done in one shot
static void unbindGLFunctions()
{
#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_name = dll##fn_name = fn_name##_t;
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#undef GL_FUNCTION
}

static void unbindWGLFunctions()
{
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) d##fn_name = dlld##fn_name = d##fn_name##_t;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) dwgl##fn_name = dlldwgl##fn_name = dwgl##fn_name##_t;
#include "platformWin32/GLWinFunc.h"
#undef WGLD3D_FUNCTION
#undef WGL_FUNCTION
}

static void unbindWGLEXTFunctions()
{
#define WGLEXT_FUNCTION(fn_type, fn_name, fn_args, fn_body) d##fn_name = d##fn_name##_t;
#include "platformWin32/GLWinExtFunc.h"
#undef WGLEXT_FUNCTION
}

//------------------------------------------------------------------
// GL_Shutdown - unbind all functions and unload libraries
//------------------------------------------------------------------
void GL_Shutdown( void )
{
   if ( winState.hinstOpenGL )
      FreeLibrary( winState.hinstOpenGL );
   winState.hinstOpenGL = NULL;

   if ( winState.hinstGLU )
      FreeLibrary( winState.hinstGLU );
   winState.hinstGLU = NULL;

   unbindGLFunctions(); //we can do GL, GLU, and EXTs in the same function
   unbindWGLFunctions();//these have to be seperate
   unbindWGLEXTFunctions();

   gGLState.suppSwapInterval = false;
}

//---------------------------------------------------------
// GL_Init - load OpenGL library and bind core GL/GLU/WGL functions
//---------------------------------------------------------
bool GL_Init( const char *dllname_gl, const char *dllname_glu )
{
   if ( winState.hinstOpenGL && winState.hinstGLU)
      return true;

#ifdef UNICODE
   UTF16 dn_gl[1024], dn_glu[1024];
   convertUTF8toUTF16((UTF8 *)dllname_gl, dn_gl, sizeof(dn_gl));
   convertUTF8toUTF16((UTF8 *)dllname_glu, dn_glu, sizeof(dn_glu));
#endif
   // Load OpenGL DLL
   if (!winState.hinstOpenGL)
   {
#ifdef UNICODE
       if ( ( winState.hinstOpenGL = LoadLibrary( dn_gl ) ) == 0 )
#else
      if ( ( winState.hinstOpenGL = LoadLibrary( dllname_gl ) ) == 0 )
#endif
         return false;
   }

   // Load OpenGL GLU DLL
   if ( !winState.hinstGLU )
   {
#ifdef UNICODE
       if ( ( winState.hinstGLU = LoadLibrary( dn_glu ) ) == 0 )
#else
      if ( ( winState.hinstGLU = LoadLibrary( dllname_glu ) ) == 0 )
#endif
         return false;
   }

   if (!bindGLFunctions())
      Con::errorf("You are missing some OpenGL functions.  That's bad.");
   if (!bindGLUFunctions())
      Con::errorf("You are missing some GLU functions.  That's bad.");
   // these will have already been bound thru the OGL version
   if (dStrstr(dllname_gl, "d3d") == NULL)
   {
      if (!bindWGLFunctions("wgl"))
         Con::errorf("You are missing some WGL Functions.  That's REALLY bad.");
   }
   else
      if (!bindWGLFunctions("wd3d"))
         Con::errorf("You are missing some WGL Functions.  That's REALLY bad.");


   return true;
}


//---------------------------------------------------------
// GL_EXT_Init - Initialize all GL/WGL extensions
//---------------------------------------------------------
bool GL_EXT_Init( )
{
   // Load extensions...
   //
   const char* pExtString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
   gGLState.primMode = 0;
   U32 extBitMask = 0;

   // GL_EXT_paletted_texture
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_paletted_texture") != NULL)
   {
      extBitMask |= EXT_paletted_texture;
      gGLState.suppPalettedTexture = true;
   }
   else
      gGLState.suppPalettedTexture = false;

   // EXT_compiled_vertex_array
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_compiled_vertex_array") != NULL)
   {
      extBitMask |= EXT_compiled_vertex_array;
      gGLState.suppLockedArrays = true;
   }
   else
   {
      gGLState.suppLockedArrays = false;
   }

   // ARB_multitexture
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_multitexture") != NULL)
   {
      extBitMask |= ARB_multitexture;
      gGLState.suppARBMultitexture = true;
   } else {
      gGLState.suppARBMultitexture = false;
   }

   // EXT_blend_color
   if(pExtString && dStrstr(pExtString, (const char*)"GL_EXT_blend_color") != NULL)
   {
      extBitMask |= EXT_blend_color;
      gGLState.suppEXTblendcolor = true;
   } else {
      gGLState.suppEXTblendcolor = false;
   }

   // EXT_blend_minmax
   if(pExtString && dStrstr(pExtString, (const char*)"GL_EXT_blend_minmax") != NULL)
   {
      extBitMask |= EXT_blend_color;
      gGLState.suppEXTblendminmax = true;
   } else {
      gGLState.suppEXTblendminmax = false;
   }

   // EXT_fog_coord
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_fog_coord") != NULL)
   {
      extBitMask |= EXT_fog_coord;
      gGLState.suppFogCoord = true;
   } else {
      gGLState.suppFogCoord = false;
   }

   // EXT_texture_compression_s3tc
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_texture_compression_s3tc") != NULL)
      gGLState.suppS3TC = true;
   else
      gGLState.suppS3TC = false;

   // ARB_texture_compression
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_texture_compression") != NULL)
   {
      extBitMask |= ARB_texture_compression;
      gGLState.suppTextureCompression = true;
   } else {
      gGLState.suppTextureCompression = false;
   }

   // NV_vertex_array_range
   if (pExtString && dStrstr(pExtString, (const char*)"NV_vertex_array_range") != NULL)
   {
      extBitMask |= NV_vertex_array_range;
      gGLState.suppVertexArrayRange = true;
   }
   else
      gGLState.suppVertexArrayRange = false;

   // 3DFX_texture_compression_FXT1
   if (pExtString && dStrstr(pExtString, (const char*)"3DFX_texture_compression_FXT1") != NULL)
      gGLState.suppFXT1 = true;
   else
      gGLState.suppFXT1 = false;

   if (!bindEXTFunctions(extBitMask))
      Con::warnf("You are missing some OpenGL Extensions.  This is bad.");

   // Binary states, i.e., no supporting functions
   // EXT_packed_pixels
   // EXT_texture_env_combine
   //
   // dhc note: a number of these can have multiple matching 'versions', private, ext, and arb.
   gGLState.suppPackedPixels      = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_packed_pixels") != NULL) : false;
   gGLState.suppTextureEnvCombine = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_env_combine") != NULL) : false;
   gGLState.suppEdgeClamp         = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_edge_clamp") != NULL) : false;
   gGLState.suppEdgeClamp        |= pExtString? (dStrstr(pExtString, (const char*)"GL_SGIS_texture_edge_clamp") != NULL) : false;
   gGLState.suppTexEnvAdd         = pExtString? (dStrstr(pExtString, (const char*)"GL_ARB_texture_env_add") != NULL) : false;
   gGLState.suppTexEnvAdd        |= pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_env_add") != NULL) : false;

   // Anisotropic filtering
   gGLState.suppTexAnisotropic    = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_filter_anisotropic") != NULL) : false;
   if (gGLState.suppTexAnisotropic)
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gGLState.maxAnisotropy);
   if (gGLState.suppARBMultitexture)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &gGLState.maxTextureUnits);
   else
      gGLState.maxTextureUnits = 1;

   //----------------------

   //WGL extensions....
   U32 wglExtMask = 0;

   // Swap interval
   if (pExtString && dStrstr(pExtString, (const char*)"WGL_EXT_swap_control") != NULL)
   {
      wglExtMask |= WGL_EXT_swap_control;
      gGLState.suppSwapInterval = true;
   }
   else
   {
      gGLState.suppSwapInterval = false;
   }

   if (!bindWGLEXTFunctions(wglExtMask))
   {
      Con::warnf("You are missing some WGLEXT Functions.  That's possibly VERY bad.");
   }

   Con::printf("OpenGL Init: Enabled Extensions");
   if (gGLState.suppARBMultitexture)      Con::printf("  ARB_multitexture (Max Texture Units: %d)", gGLState.maxTextureUnits);
   if (gGLState.suppEXTblendcolor)        Con::printf("  EXT_blend_color");
   if (gGLState.suppEXTblendminmax)       Con::printf("  EXT_blend_minmax");
   if (gGLState.suppPalettedTexture)      Con::printf("  EXT_paletted_texture");
   if (gGLState.suppLockedArrays)         Con::printf("  EXT_compiled_vertex_array");
   if (gGLState.suppVertexArrayRange)     Con::printf("  NV_vertex_array_range");
   if (gGLState.suppTextureEnvCombine)    Con::printf("  EXT_texture_env_combine");
   if (gGLState.suppPackedPixels)         Con::printf("  EXT_packed_pixels");
   if (gGLState.suppFogCoord)             Con::printf("  EXT_fog_coord");
   if (gGLState.suppTextureCompression)   Con::printf("  ARB_texture_compression");
   if (gGLState.suppS3TC)                 Con::printf("  EXT_texture_compression_s3tc");
   if (gGLState.suppFXT1)                 Con::printf("  3DFX_texture_compression_FXT1");
   if (gGLState.suppTexEnvAdd)            Con::printf("  (ARB|EXT)_texture_env_add");
   if (gGLState.suppTexAnisotropic)       Con::printf("  EXT_texture_filter_anisotropic (Max anisotropy: %g)", gGLState.maxAnisotropy);
   if (gGLState.suppSwapInterval)         Con::printf("  WGL_EXT_swap_control");

   Con::warnf("OpenGL Init: Disabled Extensions");
   if (!gGLState.suppARBMultitexture)    Con::warnf("  ARB_multitexture");
   if (!gGLState.suppEXTblendcolor)      Con::warnf("  EXT_blend_color");
   if (!gGLState.suppEXTblendminmax)     Con::warnf("  EXT_blend_minmax");
   if (!gGLState.suppPalettedTexture)    Con::warnf("  EXT_paletted_texture");
   if (!gGLState.suppLockedArrays)       Con::warnf("  EXT_compiled_vertex_array");
   if (!gGLState.suppVertexArrayRange)   Con::warnf("  NV_vertex_array_range");
   if (!gGLState.suppTextureEnvCombine)  Con::warnf("  EXT_texture_env_combine");
   if (!gGLState.suppPackedPixels)       Con::warnf("  EXT_packed_pixels");
   if (!gGLState.suppFogCoord)           Con::warnf("  EXT_fog_coord");
   if (!gGLState.suppTextureCompression) Con::warnf("  ARB_texture_compression");
   if (!gGLState.suppS3TC)               Con::warnf("  EXT_texture_compression_s3tc");
   if (!gGLState.suppFXT1)               Con::warnf("  3DFX_texture_compression_FXT1");
   if (!gGLState.suppTexEnvAdd)          Con::warnf("  (ARB|EXT)_texture_env_add");
   if (!gGLState.suppTexAnisotropic)     Con::warnf("  EXT_texture_filter_anisotropic");
   if (!gGLState.suppSwapInterval)       Con::warnf("  WGL_EXT_swap_control");
   Con::printf("");

   // Set some console variables:
   Con::setBoolVariable( "$FogCoordSupported", gGLState.suppFogCoord );
   Con::setBoolVariable( "$TextureCompressionSupported", gGLState.suppTextureCompression );
   Con::setBoolVariable( "$AnisotropySupported", gGLState.suppTexAnisotropic );
   Con::setBoolVariable( "$PalettedTextureSupported", gGLState.suppPalettedTexture );
   Con::setBoolVariable( "$SwapIntervalSupported", gGLState.suppSwapInterval );

   if (!gGLState.suppPalettedTexture && Con::getBoolVariable("$pref::OpenGL::forcePalettedTexture",false))
   {
      Con::setBoolVariable("$pref::OpenGL::forcePalettedTexture", false);
      Con::setBoolVariable("$pref::OpenGL::force16BitTexture", true);
   }

   return true;
}

