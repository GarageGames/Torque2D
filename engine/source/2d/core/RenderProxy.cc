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

#ifndef _RENDER_PROXY_H_
#include "2d/core/RenderProxy.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

// Script bindings.
#include "RenderProxy_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(RenderProxy);

//------------------------------------------------------------------------------

RenderProxy::RenderProxy()
{
    // Require self ticking.
    mSelfTick = true;
}

//------------------------------------------------------------------------------

RenderProxy::~RenderProxy()
{
}

//------------------------------------------------------------------------------

void RenderProxy::initPersistFields()
{
   addProtectedField("Image", TypeImageAssetPtr, Offset(mImageAsset, RenderProxy), &setImage, &getImage, &writeImage, "");
   addProtectedField("Frame", TypeS32, Offset(mImageFrame, RenderProxy), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "");
   addProtectedField("Animation", TypeAnimationAssetPtr, Offset(mAnimationAsset, RenderProxy), &setAnimation, &getAnimation, &writeAnimation, "");

   Parent::initPersistFields();
}

//------------------------------------------------------------------------------

void RenderProxy::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to sprite.
    RenderProxy* pRenderProxy = static_cast<RenderProxy*>(object);

    // Sanity!
    AssertFatal(pRenderProxy != NULL, "RenderProxy::copyTo() - Object is not the correct type.");

    // Call image frame provider.
    ImageFrameProvider::copyTo( pRenderProxy );
}
