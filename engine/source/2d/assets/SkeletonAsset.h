//-----------------------------------------------------------------------------
// Copyright Here

#ifndef _SKELETON_ASSET_H_
#define _SKELETON_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/imageAsset.h"
#endif

#ifndef SPINE_SPINE_H_
#include "spine/spine.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeSkeletonAssetPtr )

//-----------------------------------------------------------------------------

class SkeletonAsset : public AssetBase
{
private:
    typedef AssetBase Parent;
    bool                       mAtlasDirty;

public:
    StringTableEntry            mSkeletonFile;
    StringTableEntry            mAtlasFile;
	float                       mScale;
    AssetPtr<ImageAsset>        mImageAsset;
	spine::Atlas*               mAtlas;
	spine::SkeletonData*        mSkeletonData;
    spine::AnimationStateData*  mStateData;

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

    void                    setAtlasFile( const char* pAtlasFile );
    inline StringTableEntry getAtlasFile( void ) const                      { return mAtlasFile; };

    void                    setScale( const F32 fScale );
    inline F32              getScale( void ) const                          { return mScale; } ;
    
    virtual bool            isAssetValid( void ) const;

    /// Declare Console Object.
    DECLARE_CONOBJECT(SkeletonAsset);

private:
    void buildAtlasData( void );
    void buildSkeletonData( void );

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
    static bool writeSkeletonFile( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonAsset*>(obj)->getSkeletonFile() != StringTable->EmptyString; }
    static bool setAtlasFile( void* obj, const char* data )              { static_cast<SkeletonAsset*>(obj)->setAtlasFile(data); return false; }
    static const char* getAtlasFile(void* obj, const char* data)         { return static_cast<SkeletonAsset*>(obj)->getAtlasFile(); }
    static bool writeAtlasFile( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonAsset*>(obj)->getAtlasFile() != StringTable->EmptyString; }
    static bool setScale( void* obj, const char* data )              { static_cast<SkeletonAsset*>(obj)->setScale(dAtof(data)); return false; }
    static F32 getScale(void* obj, const char* data)         { return static_cast<SkeletonAsset*>(obj)->getScale(); }
    static bool writeScale( void* obj, StringTableEntry pFieldName )   { return static_cast<SkeletonAsset*>(obj)->getScale() != 1.0f; }
};

#endif // _SKELETON_ASSET_H_
