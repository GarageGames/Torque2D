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

#ifndef _ANIMATION_ASSET_H_
#define _ANIMATION_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeAnimationAssetPtr )

//-----------------------------------------------------------------------------

class ImageAsset;

//-----------------------------------------------------------------------------

class AnimationAsset : public AssetBase
{
private:
    typedef AssetBase  Parent;

    AssetPtr<ImageAsset>     mImageAsset;
    Vector<S32>              mAnimationFrames;
    Vector<StringTableEntry> mNamedAnimationFrames;
    Vector<S32>              mValidatedFrames;
    Vector<StringTableEntry> mValidatedNameFrames;
    F32                      mAnimationTime;
    bool                     mAnimationCycle;
    bool                     mRandomStart;

    F32                      mAnimationIntegration;

    bool                     mNamedCellsMode;

public:
    AnimationAsset();
    virtual ~AnimationAsset();

    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void copyTo(SimObject* object);

    void            setImage( const char* pAssetId );
    inline const AssetPtr<ImageAsset>& getImage( void ) const           { return mImageAsset; }

    void            setAnimationFrames( const char* pAnimationFrames );
    inline const Vector<S32>& getSpecifiedAnimationFrames( void ) const { return mAnimationFrames; }
    inline const Vector<S32>& getValidatedAnimationFrames( void ) const { return mValidatedFrames; }

    void            setNamedAnimationFrames( const char* pAnimationFrames );
    inline const Vector<StringTableEntry>& getSpecifiedNamedAnimationFrames( void ) const { return mNamedAnimationFrames; }
    inline const Vector<StringTableEntry>& getValidatedNamedAnimationFrames( void ) const { return mValidatedNameFrames; }

    void            setAnimationTime( const F32 animationTime );
    inline F32      getAnimationTime( void ) const                      { return mAnimationTime; }
    void            setAnimationCycle( const bool animationCycle );
    inline bool     getAnimationCycle( void ) const                     { return mAnimationCycle; }
    void            setRandomStart( const bool randomStart );
    inline bool     getRandomStart( void ) const                        { return mRandomStart; }
    void            setNamedCellsMode( const bool namedCellsMode );
    inline bool     getNamedCellsMode( void ) const                     { return mNamedCellsMode; }

    // Frame validation.
    void            validateFrames( void );
    void            validateNumericalFrames( void );
    void            validateNamedFrames( void );

    // Asset validation.
    virtual bool    isAssetValid( void ) const;

    /// Declare Console Object.
    DECLARE_CONOBJECT(AnimationAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

protected:
    static bool setImage( void* obj, const char* data )                             { static_cast<AnimationAsset*>(obj)->setImage( data ); return false; }
    static bool writeImage( void* obj, StringTableEntry pFieldName )                { return static_cast<AnimationAsset*>(obj)->mImageAsset.notNull(); }
    static bool setAnimationFrames( void* obj, const char* data )                   { static_cast<AnimationAsset*>(obj)->setAnimationFrames( data ); return false; }
    static bool writeAnimationFrames( void* obj, StringTableEntry pFieldName )      { return static_cast<AnimationAsset*>(obj)->mAnimationFrames.size() > 0; }
    static bool setNamedAnimationFrames( void* obj, const char* data )              { static_cast<AnimationAsset*>(obj)->setNamedAnimationFrames( data ); return false; }    
    static bool writeNamedAnimationFrames( void* obj, StringTableEntry pFieldName ) { return static_cast<AnimationAsset*>(obj)->mNamedAnimationFrames.size() > 0; }
    static bool setAnimationTime( void* obj, const char* data )                     { static_cast<AnimationAsset*>(obj)->setAnimationTime( dAtof(data) ); return false; }
    static bool setAnimationCycle( void* obj, const char* data )                    { static_cast<AnimationAsset*>(obj)->setAnimationCycle( dAtob(data) ); return false; }
    static bool writeAnimationCycle( void* obj, StringTableEntry pFieldName )       { return static_cast<AnimationAsset*>(obj)->getAnimationCycle() == false; }
    static bool setRandomStart( void* obj, const char* data )                       { static_cast<AnimationAsset*>(obj)->setRandomStart( dAtob(data) ); return false; }
    static bool writeRandomStart( void* obj, StringTableEntry pFieldName )          { return static_cast<AnimationAsset*>(obj)->getRandomStart() == true; }
    static bool setNamedCellsMode( void* obj, const char* data )                    { static_cast<AnimationAsset*>(obj)->setNamedCellsMode( dAtob(data) ); return false; }
    static bool writeNamedCellsMode( void* obj, StringTableEntry pFieldName )       { return static_cast<AnimationAsset*>(obj)->getNamedCellsMode() == true; }
};

#endif // _ANIMATION_ASSET_H_