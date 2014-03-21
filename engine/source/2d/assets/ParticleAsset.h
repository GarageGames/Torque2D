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

#ifndef _PARTICLE_ASSET_H_
#define _PARTICLE_ASSET_H_

#ifndef _PARTICLE_ASSET_EMITTER_H_
#include "2d/assets/ParticleAssetEmitter.h"
#endif

#ifndef _TAML_CHILDREN_H_
#include "persistence/taml/tamlChildren.h"
#endif

#ifndef _ASSET_BASE_H_
#include "assets/assetBase.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeParticleAssetPtr )

//-----------------------------------------------------------------------------

class ParticleAsset : public AssetBase, public TamlChildren
{
private:
    typedef AssetBase  Parent;
    typedef ParticleAssetEmitter Children;
    typedef Vector<ParticleAssetEmitter*> typeEmitterVector;

public:
    /// Life Mode.
    enum LifeMode
    {
        INVALID_LIFEMODE,

        INFINITE,
        CYCLE,
        STOP,
        KILL
    };

private:
    typeEmitterVector                       mEmitters;

    F32                                     mLifetime;
    LifeMode                                mLifeMode;

    /// Particle fields.
    ParticleAssetFieldCollection            mParticleFields;
    ParticleAssetFieldBase                  mParticleLifeScale;
    ParticleAssetFieldBase                  mQuantityScale;
    ParticleAssetFieldBase                  mSizeXScale;
    ParticleAssetFieldBase                  mSizeYScale;
    ParticleAssetFieldBase                  mSpeedScale;
    ParticleAssetFieldBase                  mSpinScale;
    ParticleAssetFieldBase                  mFixedForceScale;
    ParticleAssetFieldBase                  mRandomMotionScale;
    ParticleAssetFieldBase                  mAlphaChannelScale;

public:
    ParticleAsset();
    virtual ~ParticleAsset();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);
    virtual void onDeleteNotify( SimObject* object );

    // Asset validation.
    virtual bool isAssetValid( void ) const;

    void setLifetime( const F32 lifetime );
    F32 getLifetime( void ) const { return mLifetime; }
    void setLifeMode( const LifeMode lifemode );
    LifeMode getLifeMode( void ) const { return mLifeMode; }

    inline ParticleAssetFieldCollection& getParticleFields( void ) { return mParticleFields; }

    inline ParticleAssetField& getParticleLifeScaleField( void ) { return mParticleLifeScale.getBase(); }
    inline ParticleAssetField& getQuantityScaleField( void ) { return mQuantityScale.getBase(); }
    inline ParticleAssetField& getSizeXScaleField( void ) { return mSizeXScale.getBase(); }
    inline ParticleAssetField& getSizeYScaleField( void ) { return mSizeYScale.getBase(); }
    inline ParticleAssetField& getSpeedScaleField( void ) { return mSpeedScale.getBase(); }
    inline ParticleAssetField& getSpinScaleField( void ) { return mSpinScale.getBase(); }
    inline ParticleAssetField& getFixedForceScaleField( void ) { return mFixedForceScale.getBase(); }
    inline ParticleAssetField& getRandomMotionScaleField( void ) { return mRandomMotionScale.getBase(); }
    inline ParticleAssetField& getAlphaChannelScaleField( void ) { return mAlphaChannelScale.getBase(); }

    ParticleAssetEmitter* createEmitter( void );
    bool addEmitter( ParticleAssetEmitter* pParticleAssetEmitter );
    void removeEmitter( ParticleAssetEmitter* pParticleAssetEmitter, const bool deleteEmitter = true );
    void clearEmitters( void );
    U32 getEmitterCount( void ) const { return (U32)mEmitters.size(); }
    ParticleAssetEmitter* getEmitter( const U32 emitterIndex ) const;
    ParticleAssetEmitter* findEmitter( const char* pEmitterName ) const;
    void moveEmitter( S32 fromIndex, S32 toIndex );

    virtual U32 getTamlChildCount( void ) const
    {
        return (U32)mEmitters.size();
    }

    virtual SimObject* getTamlChild( const U32 childIndex ) const
    {
        // Sanity!
        AssertFatal( childIndex < (U32)mEmitters.size(), "ParticleAsset::getTamlChild() - Child index is out of range." );

        // For when the assert is not used.
        if ( childIndex >= (U32)mEmitters.size() )
            return NULL;

        return mEmitters[ childIndex ];
    }

    virtual void addTamlChild( SimObject* pSimObject )
    {
        // Sanity!
        AssertFatal( pSimObject != NULL, "ParticleAsset::addTamlChild() - Cannot add a NULL child object." );

        // Fetch as particle emitter.
        ParticleAssetEmitter* pParticleAssetEmitter = dynamic_cast<ParticleAssetEmitter*>( pSimObject );

        // Is it a particle emitter?
        if ( pParticleAssetEmitter == NULL )
        {
            // No, so warn.
            Con::warnf( "ParticleAsset::addTamlChild() - Cannot add a child object that isn't a particle emitter." );
            return;
        }

        // Add the emitter.
        addEmitter( pParticleAssetEmitter );
    }

    static LifeMode getParticleAssetLifeModeEnum(const char* label);
    static const char* getParticleAssetLifeModeDescription( const LifeMode lifeMode );

    /// Declare Console Object.
    DECLARE_CONOBJECT(ParticleAsset);

protected:
    virtual void initializeAsset( void );

    void onTamlCustomWrite( TamlCustomNodes& customNodes );
    void onTamlCustomRead( const TamlCustomNodes& customNodes );

protected:
    static bool setLifetime(void* obj, const char* data)                    { static_cast<ParticleAsset*>(obj)->setLifetime(dAtof(data)); return false; }
    static bool writeLifetime( void* obj, StringTableEntry pFieldName )     { return mNotZero( static_cast<ParticleAsset*>(obj)->getLifetime() ); }

    static bool setLifeMode(void* obj, const char* data)                    { static_cast<ParticleAsset*>(obj)->setLifeMode( ParticleAsset::getParticleAssetLifeModeEnum(data) ); return false; }
    static bool writeLifeMode( void* obj, StringTableEntry pFieldName )     { return static_cast<ParticleAsset*>(obj)->getLifeMode() != INFINITE; }
};

#endif // _PARTICLE_ASSET_H_
