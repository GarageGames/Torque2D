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

#ifndef _SPINE_TORQUE2D_H_
#include "spine/spine-torque2d.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeSkeletonAssetPtr )

//-----------------------------------------------------------------------------

class SkeletonAsset : public AssetBase
{
private:
    typedef AssetBase Parent;

public:
    StringTableEntry            mSkeletonFile;
	 float                       mScale; // BOZO - How to expose scale to scripts?
	 spine::Atlas*               mAtlas;
	 spine::SkeletonData*        mSkeletonData;
	 spine::AnimationStateData*  mStateData;

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

    void                    setSkeletonFile( const char* pSkeletonFile );
    inline StringTableEntry getSkeletonFile( void ) const                   { return mSkeletonFile; };
    void                    setScale( const F32 fScale );
    inline F32              getScale( void ) const                          { return mScale; } ;

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
    static bool setSkeletonFile( void* obj, const char* data )              { static_cast<SkeletonAsset*>(obj)->setSkeletonFile(data); return false; }
    static const char* getSkeletonFile(void* obj, const char* data)         { return static_cast<SkeletonAsset*>(obj)->getSkeletonFile(); }
    static bool writeImageFile( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonAsset*>(obj)->getSkeletonFile() != StringTable->EmptyString; }
    static bool setScale( void* obj, const char* data )              { static_cast<SkeletonAsset*>(obj)->setScale(dAtof(data)); return false; }
    static F32 getScale(void* obj, const char* data)         { return static_cast<SkeletonAsset*>(obj)->getScale(); }
    static bool writeScale( void* obj, StringTableEntry pFieldName )   { return static_cast<SkeletonAsset*>(obj)->getScale() != 1.0f; }
};

#endif // _SKELETON_ASSET_H_
