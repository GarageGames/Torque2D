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

// Many OSX frameworks include OpenTransport, and OT's new operator conflicts 
// with our redefinition of 'new', so we have to pre-include platformAndroid.h,
// which contains the workaround.
#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/platformGL.h"
#include "platformAndroid/AndroidUtil.h"

#include "console/console.h"
#include "graphics/dgl.h"

GLState gGLState;

bool gOpenGLDisablePT          = false;
bool gOpenGLDisableCVA         = false;
bool gOpenGLDisableTEC         = false;
bool gOpenGLDisableARBMT       = false;
bool gOpenGLDisableFC          = true; //false;
bool gOpenGLDisableTCompress   = false;
bool gOpenGLNoEnvColor         = false;
float gOpenGLGammaCorrection   = 0.5;
bool gOpenGLNoDrawArraysAlpha  = false;

//-----------------------------------------------------------------------------
/// These stubs are legacy stuff for the d3d wrapper layer.
// The code that requires these stubs should probably be ifdef'd out of any non w32 build
//-----------------------------------------------------------------------------
GLboolean glAvailableVertexBufferEXT() {   return(false); }
GLint glAllocateVertexBufferEXT(GLsizei size, GLint format, GLboolean preserve) {   return(0); }
void* glLockVertexBufferEXT(GLint handle, GLsizei size) {   return(NULL); }
void glUnlockVertexBufferEXT(GLint handle) {}
void glSetVertexBufferEXT(GLint handle) {}
void glOffsetVertexBufferEXT(GLint handle, GLuint offset) {}
void glFillVertexBufferEXT(GLint handle, GLint first, GLsizei count) {}
void glFreeVertexBufferEXT(GLint handle) {}


// Find out which extensions are available for this renderer. 
void getGLCapabilities( )
{
   AssertFatal(platState.engine, "getGLCapabilities() was called before a monitor was chosen!");

   // silently create an opengl context on the current display,
   // so that we can get valid renderer and capability info.
   // we save off the current context so that we can silently restore it.
   // the user should not be aware of this little shuffle.
   //CGLContextObj curr_ctx = CGLGetCurrentContext ();
   //CGLContextObj temp_ctx =  getContextForCapsCheck();
   /*
   if(!temp_ctx)
   {
      Con::errorf("OpenGL may not be set up correctly!");
      return;
   }
   */
   //CGLSetCurrentContext(temp_ctx);

   // Get the OpenGL info strings we'll need
   const char* pVendString    = (const char*) glGetString( GL_VENDOR );
   const char* pRendString    = (const char*) glGetString( GL_RENDERER );
   const char* pVersString    = (const char*) glGetString( GL_VERSION );
   const char* pExtString     = (const char*) glGetString( GL_EXTENSIONS );

   // Output some driver info to the console:
   Con::printf( "OpenGL driver information:" );
   if ( pVendString )
      Con::printf( "  Vendor: %s", pVendString );
   if ( pRendString )
      Con::printf( "  Renderer: %s", pRendString );
   if ( pVersString )
      Con::printf( "  Version: %s", pVersString );

   // pre-clear the structure
   dMemset(&gGLState, 0, sizeof(gGLState));

   if(pExtString)
   {
      // EXT_paletted_texture ========================================
      if (dStrstr(pExtString, (const char*)"GL_EXT_paletted_texture") != NULL)
         gGLState.suppPalettedTexture = true;
      
      // EXT_compiled_vertex_array ========================================
      gGLState.suppLockedArrays = false;
      if (dStrstr(pExtString, (const char*)"GL_EXT_compiled_vertex_array") != NULL)
         gGLState.suppLockedArrays = true;

      // ARB_multitexture ========================================
      if (dStrstr(pExtString, (const char*)"GL_ARB_multitexture") != NULL)
		  gGLState.suppARBMultitexture = true;
      
      // EXT_blend_color
      if(dStrstr(pExtString, (const char*)"GL_EXT_blend_color") != NULL)
         gGLState.suppEXTblendcolor = true;

      // EXT_blend_minmax
      if(dStrstr(pExtString, (const char*)"GL_EXT_blend_minmax") != NULL)
         gGLState.suppEXTblendminmax = true;

      // NV_vertex_array_range ========================================
      // does not appear to be supported by apple, at all. ( as of 10.4.3 )
      // GL_APPLE_vertex_array_range is similar, and may be nearly identical.
      if (dStrstr(pExtString, (const char*)"GL_NV_vertex_array_range") != NULL)
         gGLState.suppVertexArrayRange = true;
      

      // EXT_fog_coord ========================================
      if (dStrstr(pExtString, (const char*)"GL_EXT_fog_coord") != NULL)
         gGLState.suppFogCoord = true;
      
      // ARB_texture_compression ========================================
      if (dStrstr(pExtString, (const char*)"GL_ARB_texture_compression") != NULL)
         gGLState.suppTextureCompression = true;

      // 3DFX_texture_compression_FXT1 ========================================
      if (dStrstr(pExtString, (const char*)"GL_3DFX_texture_compression_FXT1") != NULL)
         gGLState.suppFXT1 = true;

      // EXT_texture_compression_S3TC ========================================
      if (dStrstr(pExtString, (const char*)"GL_EXT_texture_compression_s3tc") != NULL)
         gGLState.suppS3TC = true;

      // EXT_vertex_buffer ========================================
      // This extension is deprecated, and not supported by Apple. ( 10.4.3 )
      // Instead, the ARB Vertex Buffer extension is supported.
      // The new extension has a different API, so TGE should be updated to use it.
      if (dStrstr(pExtString, (const char*)"GL_EXT_vertex_buffer") != NULL)
         gGLState.suppVertexBuffer = true;

      // Anisotropic filtering ========================================
      gGLState.suppTexAnisotropic    = (dStrstr(pExtString, (const char*)"GL_EXT_texture_filter_anisotropic") != NULL);

      if (gGLState.suppTexAnisotropic)
         glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gGLState.maxAnisotropy);

      // Binary states, i.e., no supporting functions  ========================================
      // NOTE:
      // Some of these have multiple representations, via EXT and|or ARB and|or NV and|or SGIS ... etc.
      // Check all relative versions.
      
      gGLState.suppPackedPixels      = (dStrstr(pExtString, (const char*)"GL_EXT_packed_pixels") != NULL);
      gGLState.suppPackedPixels     |= (dStrstr(pExtString, (const char*)"GL_APPLE_packed_pixel") != NULL);

      gGLState.suppTextureEnvCombine = (dStrstr(pExtString, (const char*)"GL_EXT_texture_env_combine") != NULL);
      gGLState.suppTextureEnvCombine|= (dStrstr(pExtString, (const char*)"GL_ARB_texture_env_combine") != NULL);

      gGLState.suppEdgeClamp         = (dStrstr(pExtString, (const char*)"GL_EXT_texture_edge_clamp") != NULL);
      gGLState.suppEdgeClamp        |= (dStrstr(pExtString, (const char*)"GL_SGIS_texture_edge_clamp") != NULL);
      gGLState.suppEdgeClamp        |= (dStrstr(pExtString, (const char*)"GL_ARB_texture_border_clamp") != NULL);
	   gGLState.suppEdgeClamp		= true;

      gGLState.suppTexEnvAdd         = (dStrstr(pExtString, (const char*)"GL_ARB_texture_env_add") != NULL);
      gGLState.suppTexEnvAdd        |= (dStrstr(pExtString, (const char*)"GL_EXT_texture_env_add") != NULL);

   }
   
   // Texture combine units  ========================================
//   if (gGLState.suppARBMultitexture)
//      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &gGLState.maxTextureUnits);
	  gGLState.suppARBMultitexture = false;
      gGLState.maxTextureUnits = 1;

   // Swap interval ========================================
   // Mac inherently supports a swap interval via AGL-set-integer.
   gGLState.suppSwapInterval = true;

   // FSAA support, TODO: check for ARB multisample support
   // multisample support should be checked via CGL
   gGLState.maxFSAASamples = 4;

   // dump found extensions to the console... 
   Con::printf("OpenGL Init: Enabled Extensions");
   if (gGLState.suppARBMultitexture)    Con::printf("  ARB_multitexture (Max Texture Units: %d)", gGLState.maxTextureUnits);
   if (gGLState.suppEXTblendcolor)      Con::printf("  EXT_blend_color");
   if (gGLState.suppEXTblendminmax)     Con::printf("  EXT_blend_minmax");
   if (gGLState.suppPalettedTexture)    Con::printf("  EXT_paletted_texture");
   if (gGLState.suppLockedArrays)       Con::printf("  EXT_compiled_vertex_array");
   if (gGLState.suppVertexArrayRange)   Con::printf("  NV_vertex_array_range");
   if (gGLState.suppTextureEnvCombine)  Con::printf("  EXT_texture_env_combine");
   if (gGLState.suppPackedPixels)       Con::printf("  EXT_packed_pixels");
   if (gGLState.suppFogCoord)           Con::printf("  EXT_fog_coord");
   if (gGLState.suppTextureCompression) Con::printf("  ARB_texture_compression");
   if (gGLState.suppS3TC)               Con::printf("  EXT_texture_compression_s3tc");
   if (gGLState.suppFXT1)               Con::printf("  3DFX_texture_compression_FXT1");
   if (gGLState.suppTexEnvAdd)          Con::printf("  (ARB|EXT)_texture_env_add");
   if (gGLState.suppTexAnisotropic)     Con::printf("  EXT_texture_filter_anisotropic (Max anisotropy: %f)", gGLState.maxAnisotropy);
   if (gGLState.suppSwapInterval)       Con::printf("  Vertical Sync");
   if (gGLState.maxFSAASamples)         Con::printf("  ATI_FSAA");

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
   if (!gGLState.suppSwapInterval)       Con::warnf("  Vertical Sync");
   if (!gGLState.maxFSAASamples)         Con::warnf("  ATI_FSAA");
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

   // get fsaa samples. default to normal, no antialiasing
   // TODO: clamp this against ARB_multisample capabilities.
   gFSAASamples = Con::getIntVariable("$pref::OpenGL::numFSAASamples", 1);

}
