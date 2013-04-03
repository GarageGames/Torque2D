//-----------------------------------------------------------------------------
// Copyright Here

#ifndef _SKELETON_ASSET_H_
#define _SKELETON_ASSET_H_

#ifndef _ASSET_BASE_H_
#include "assets/assetBase.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeSkeletonAssetPtr )

//-----------------------------------------------------------------------------

class SkeletonAsset : public AssetBase
{
private:
    typedef AssetBase Parent;

public:

    //
    // mSourceFile can be replaced with amore appropriate
    // name, based on what it's loading. For example,
    // if it's a png, this could be mImageFile
    StringTableEntry            mSourceFile;

    // Leaving this here, just in case we need to create textures
    TextureHandle               mImageTextureHandle;

public:
    SkeletonAsset();
    virtual ~SkeletonAsset();

    /// Core.
    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void copyTo(SimObject* object);

    void                    setSourceFile( const char* pSourceFile );
    inline StringTableEntry getSourceFile( void ) const                      { return mSourceFile; };

    inline TextureHandle&   getImageTexture( void )                         { return mImageTextureHandle; }
    inline S32              getImageWidth( void ) const                     { return mImageTextureHandle.getWidth(); }
    inline S32              getImageHeight( void ) const                    { return mImageTextureHandle.getHeight(); }

    inline const void       bindImageTexture( void)                         { glBindTexture( GL_TEXTURE_2D, getImageTexture().getGLName() ); };

    virtual bool            isAssetValid( void ) const                      { return false; }

    /// Declare Console Object.
    DECLARE_CONOBJECT(SkeletonAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

    /// Taml callbacks.
    virtual void onTamlPreWrite( void );
    virtual void onTamlPostWrite( void );
    virtual void onTamlCustomWrite( TamlCustomNodes& customNodes );
    virtual void onTamlCustomRead( const TamlCustomNodes& customNodes );


protected:
    static bool setSourceFile( void* obj, const char* data )                 { static_cast<SkeletonAsset*>(obj)->setSourceFile(data); return false; }
    static const char* getSourceFile(void* obj, const char* data)            { return static_cast<SkeletonAsset*>(obj)->getSourceFile(); }
    static bool writeImageFile( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonAsset*>(obj)->getSourceFile() != StringTable->EmptyString; }
};

#endif // _SKELETON_ASSET_H_
