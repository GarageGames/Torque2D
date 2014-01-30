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

#ifndef _SPRITE_BASE_H_
#include "2d/core/SpriteBase.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

// Script bindings.
#include "2d/core/SpriteBase_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SpriteBase);

//------------------------------------------------------------------------------

SpriteBase::SpriteBase()
{
}

//------------------------------------------------------------------------------

SpriteBase::~SpriteBase()
{
}

//------------------------------------------------------------------------------

void SpriteBase::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("Image", TypeImageAssetPtr, Offset(mImageAsset, SpriteBase), &setImage, &getImage, &writeImage, "");
    addProtectedField("Frame", TypeS32, Offset(mImageFrame, SpriteBase), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "");
    addProtectedField("FrameName", TypeString, Offset(mImageFrame, SpriteBase), &setImageNameFrame, &defaultProtectedGetFn, &writeImageNameFrame, "");
    addProtectedField("Animation", TypeAnimationAssetPtr, Offset(mAnimationAsset, SpriteBase), &setAnimation, &getAnimation, &writeAnimation, "");
}

//-----------------------------------------------------------------------------

void SpriteBase::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( totalTime, elapsedTime, pDebugStats );

    // Update image frame provider.
    ImageFrameProvider::update( elapsedTime );
}

//------------------------------------------------------------------------------

bool SpriteBase::validRender( void ) const
{
    return ImageFrameProvider::validRender();
}

//------------------------------------------------------------------------------

void SpriteBase::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to sprite.
    SpriteBase* pSpriteBase = static_cast<SpriteBase*>(object);

    // Sanity!
    AssertFatal(pSpriteBase != NULL, "SpriteBase::copyTo() - Object is not the correct type.");

    // Call image frame provider.
    ImageFrameProvider::copyTo( pSpriteBase );
}

//------------------------------------------------------------------------------

void SpriteBase::onAnimationEnd( void )
{
    // Do script callback.
    Con::executef( this, 1, "onAnimationEnd" );
}
