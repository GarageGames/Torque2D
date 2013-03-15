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

#ifndef _TEXTURE_HANDLE_H_
#define _TEXTURE_HANDLE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMGL_H_
#include "platform/platformAssert.h"
#include "platform/platformGL.h"
#endif

//-----------------------------------------------------------------------------

class GBitmap;
class TextureObject;

//------------------------------------------------------------------------------

/// This is the main texture manager interface.  Texturing can be
/// a bit complicated, but if you follow these easy steps, it is
/// really quite simple!
///
/// In order to use a texture on disk, first you must create a
/// TextureHandle data structure for it.
/// @code
/// TextureHandle handle = TextureHandle("pathToTexture", textureType);
/// @endcode
/// See the documentation on the different enumerated types for more info
/// on texture types.
///
/// Ok, now you have your texture loaded into video memory or ram,
/// whichever is chooses.  In order to tell OpenGL to use your texture,
/// you have to bind it.  GL_TEXTURE_2D is the type of texture you're
/// binding - a 2 dimisional texture.  Also note that you only have
/// to do this if you are using direct OpenGL commands to draw rather
/// than dgl.  Dgl manages the below on it's own so you don't have to worry about it.
/// @code
/// glBindTexture(GL_TEXTURE_2D, handle.getGLName());
/// @endcode
/// Now you can begin to draw you texture.  If you havn't already,
/// make sure you make a call to glEnable(GL_TEXTURE_2D);  before
/// you start drawing and a call to glDisable(GL_TEXTURE_2D); when
/// you're done.  Failure to call glEnable will cause the texture not
/// to draw, and failure to call glDisable will probably case
/// an assert in debug mode and ugly artifacts in release.
///
/// If you are going through dgl, all you need is the TextureHandle and
/// some points.  See the dgl documentation for more info on each
/// individual function in the dgl library.  However, most dgl functions
/// will take a TextureObject data type.  And, it just so happens that
/// a TextureHandle has a TextureObject!  It also has an
/// operator TextureObject*(), which lets you cast a TextureHandle to
/// a TextureObject.  That means that all you have to do is ignore
/// the TextureObject parameter and just give it a TextureHandle.
///
/// Some tips on texture performance:
///
/// Instead of using hard-coded paths, use a hook to a console variable.
/// You will probably change the directory structure for your game,
/// and that means that you will have to go back to all of the hardcoded
/// paths and change them by hand, then rebuild the engine.  It is much
/// better to use script variables since they are all in one place and
/// easier to change.
///
/// Add the path string for your texture to the StringTable.  Doing so
/// helps in string lookups and faster string performance.
///
/// Don't create the texture every frame if at all possible.  Make it
/// a global variable if you have to - just don't load every frame.
/// Loading data off of the disk every frame WILL cause massive
/// performance loss and should be avoided at all costs.  This is
/// not to mention the overhead of generating mip map levels
/// and uploading the texture into memory when the texture is created.
///
/// @note
/// Texture handles can be allocated in 2 ways - by name to be loaded
/// from disk, or by name to a dynamically generated texture
///
/// If you create a GBitmap and register it, the Texture manager
/// owns the pointer - so if you re-register a texture with the same
/// name, the texture manager will delete the second copy.
///
/// Also note the operator TextureObject*, as you can actually cast
/// a TextureHandle to a TextureObject* if necessary.
class TextureHandle
{    
public:
    enum TextureHandleType
    {
        // Invalid texture.
        InvalidTexture = 0,

        /// Bitmap that will be unloaded after texture has been uploaded.
        /// The bitmap will need reloading if the textures need to be restored.
        /// This is the preferred type.
        BitmapTexture = 100,

        /// Same as BitmapTexture except that the bitmap is kept which occupies main memory however
        /// it does not require loading if textures need to be restored.
        BitmapKeepTexture = 200,
    };

public:
    TextureHandle() : object( NULL ) {}

    TextureHandle( TextureObject *to )
    {
        object = to;
        lock();
    }

    TextureHandle( const TextureHandle &th ) 
    {
        object = th.object;
        lock();
    }

    TextureHandle(const char* textureKey, TextureHandleType type, bool clampToEdge = false, bool force16Bit = false );

    TextureHandle(const char* textureKey, GBitmap* bmp, TextureHandleType type, bool clampToEdge = false);

    ~TextureHandle() { unlock(); }

    TextureHandle& operator=(const TextureHandle &t) 
    {
        unlock();
        object = t.object;
        lock();
        return *this;
    }

    bool set(const char* pTextureKey, TextureHandleType type = BitmapTexture, bool clampToEdge = false, bool force16Bit = false );

    bool set(const char* pTextureKey, GBitmap *bmp, TextureHandleType type, bool clampToEdge = false);

    bool operator==( const TextureHandle& handle ) const { return handle.object == object; }

    bool operator!=( const TextureHandle& handle ) const { return handle.object != object; }

    void setClamp( const bool clamp );

    void setFilter( const GLuint filter );

    void clear( void ) { unlock(); }

    void refresh( void );

    operator TextureObject*() { return object; }
    inline bool NotNull( void ) const { return object != NULL; }
    inline bool IsNull( void ) const { return object == NULL; }
    const char* getTextureKey( void ) const;
    U32 getWidth( void ) const;
    U32 getHeight( void ) const;
    GBitmap* getBitmap( void );
    const GBitmap* getBitmap( void ) const;
    U32 getGLName( void ) const;

private:
    void lock( void );
    void unlock( void );

private:
    TextureObject *object;

};

//-----------------------------------------------------------------------------

extern TextureHandle BadTextureHandle;

#endif // _TEXTURE_HANDLE_H_