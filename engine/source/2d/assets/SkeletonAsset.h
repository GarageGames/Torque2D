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

#ifndef _SKELETON_ASSET_H_
#define _SKELETON_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
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
    bool                            mAtlasDirty;

public:
    StringTableEntry                mSkeletonFile;
    StringTableEntry                mAtlasFile;
    AssetPtr<ImageAsset>            mImageAsset;
    spAtlas*                        mAtlas;
    spSkeletonData*                 mSkeletonData;
    spAnimationStateData*           mStateData;

public:
    SkeletonAsset();
    virtual ~SkeletonAsset();

    /// Core.
    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void copyTo(SimObject* object);

    void                    setSkeletonFile( const char* pSkeletonFile );
    inline StringTableEntry getSkeletonFile( void ) const                   { return mSkeletonFile; }

    void                    setAtlasFile( const char* pAtlasFile );
    inline StringTableEntry getAtlasFile( void ) const                      { return mAtlasFile; }
    
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
    static bool writeSkeletonFile( void* obj, StringTableEntry pFieldName ) { return static_cast<SkeletonAsset*>(obj)->getSkeletonFile() != StringTable->EmptyString; }
    static bool setAtlasFile( void* obj, const char* data )                 { static_cast<SkeletonAsset*>(obj)->setAtlasFile(data); return false; }
    static bool writeAtlasFile( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonAsset*>(obj)->getAtlasFile() != StringTable->EmptyString; }
};

#endif // _SKELETON_ASSET_H_
