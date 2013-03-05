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

#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMGL_H_
#include "platform/platformAssert.h"
#include "platform/platformGL.h"
#endif

#ifndef _TEXTURE_OBJECT_H_
#include "graphics/TextureObject.h"
#endif

#ifndef _TEXTURE_DICTIONARY_H_
#include "graphics/TextureDictionary.h"
#endif

//-----------------------------------------------------------------------------

#define MaximumProductSupportedTextureWidth 2048
#define MaximumProductSupportedTextureHeight MaximumProductSupportedTextureWidth

class TextureManager
{
   friend class TextureHandle;
   friend class TextureDictionary;

public:
    /// Texture manager event codes.
    enum TextureEventCode
    {
        BeginZombification,
        BeginResurrection,
        EndResurrection,
    };

    typedef void (*TextureEventCallback)(const TextureEventCode eventCode, void *userData);

    /// Textrue manager state.
    enum ManagerState
    {
        NotInitialized = 0,
        Alive,
        Dead,
        Resurrecting,
    };

private:
    static S32 mMasterTextureKeyIndex;
    static ManagerState mManagerState;
    static S32 mTextureResidentWasteSize;
    static S32 mTextureResidentSize;
    static S32 mTextureResidentCount;
    static S32 mBitmapResidentSize;
    static bool mForce16BitTexture;
    static bool mAllowTextureCompression;
    static bool mDisableTextureSubImageUpdates;

public:
    static bool mDGLRender;
    static GLenum mTextureCompressionHint;

public:
    static void create();
    static void destroy();
    static ManagerState getManagerState( void ) { return mManagerState; }

    static void killManager();
    static void resurrectManager();
    static void flush();
    static void refresh( const char *textureName );
    static S32 getBitmapResidentSize( void ) { return mBitmapResidentSize; }
    static S32 getTextureResidentSize( void ) { return mTextureResidentSize; }
    static S32 getTextureResidentWasteSize( void ) { return mTextureResidentWasteSize; }
    static S32 getTextureResidentCount( void ) { return mTextureResidentCount; }

    static U32  registerEventCallback(TextureEventCallback, void *userData);
    static void unregisterEventCallback(const U32 callbackKey);

    static StringTableEntry getUniqueTextureKey( void );

    static void dumpMetrics( void );

private:
    static void postTextureEvent(const TextureEventCode eventCode);

    static void createGLName( TextureObject* pTextureObject );
    static TextureObject* registerTexture(const char *textureName, GBitmap* pNewBitmap, TextureHandle::TextureHandleType type, bool clampToEdge);
    static TextureObject* loadTexture(const char *textureName, TextureHandle::TextureHandleType type, bool clampToEdge, bool checkOnly = false, bool force16Bit = false );
    static void freeTexture( TextureObject* pTextureObject );
    static void refresh(TextureObject* pTextureObject);

    static GBitmap* loadBitmap(const char *textureName, bool recurse = true, bool nocompression = false);
    static GBitmap* createPowerOfTwoBitmap( GBitmap* pBitmap );
    static U16* create16BitBitmap( GBitmap *pDL, U8 *in_source8, GBitmap::BitmapFormat alpha_info, GLint *GLformat, GLint *GLdata_type, U32 width, U32 height );
    static void getSourceDestByteFormat(GBitmap *pBitmap, U32 *sourceFormat, U32 *destFormat, U32 *byteFormat, U32* texelSize);
    static F32 getResidentFraction( void );
};

#endif // _TEXTURE_MANAGER_H_