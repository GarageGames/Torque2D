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

#ifndef _DEBUG_STATS_H_
#define _DEBUG_STATS_H_

#ifndef _PROFILER_H_
#include "debug/profiler.h"
#endif

#ifndef _TORQUE_TYPES_H_
#include "platform\types.h"
#endif

#ifndef _PLATFORM_MEMORY_H_
#include "platform/platformMemory.h"
#endif

#ifndef BOX2D_H
#include "Box2D/Box2D.h"
#endif

//-----------------------------------------------------------------------------

class DebugStats
{
public:

    DebugStats()
    {
        reset();
    }

    inline void updateRanges( void )
    {
        // Debug Profiling.
        PROFILE_SCOPE(DebugStats_UpdateRanges);

        // Frames per-second.
        if ( fps > 0.0f )
        {
            if ( fps < minFPS ) minFPS = fps;
            if ( fps > maxFPS ) maxFPS = fps;
        }

        // Bodies, joints, contacts and proxies.
        if ( bodyCount > maxBodyCount ) maxBodyCount = bodyCount;
        if ( jointCount > maxJointCount ) maxJointCount = jointCount;
        if ( contactCount > maxContactCount ) maxContactCount = contactCount;
        if ( proxyCount > maxProxyCount ) maxProxyCount = proxyCount;

        // Objects.
        if ( objectsCount > maxObjectsCount ) maxObjectsCount = objectsCount;
        if ( objectsEnabled > maxObjectsEnabled ) maxObjectsEnabled = objectsEnabled;
        if ( objectsVisible > maxObjectsVisible ) maxObjectsVisible = objectsVisible;
        if ( objectsAwake > maxObjectsAwake ) maxObjectsAwake = objectsAwake;

        // Render pick/requests.
        if ( renderPicked > maxRenderPicked ) maxRenderPicked = renderPicked;
        if ( renderRequests > maxRenderRequests ) maxRenderRequests = renderRequests;
        if ( renderFallbacks > maxRenderFallbacks ) maxRenderFallbacks = renderFallbacks;

        // Batching.
        if ( batchTrianglesSubmitted > maxBatchTrianglesSubmitted ) maxBatchTrianglesSubmitted = batchTrianglesSubmitted;
        if ( batchDrawCallsStrict > maxBatchDrawCallsStrict ) maxBatchDrawCallsStrict = batchDrawCallsStrict;
        if ( batchDrawCallsSorted > maxBatchDrawCallsSorted ) maxBatchDrawCallsSorted = batchDrawCallsSorted;
        if ( batchFlushes > maxBatchFlushes ) maxBatchFlushes = batchFlushes;
        if ( batchBlendStateFlush > maxBatchBlendStateFlush ) maxBatchBlendStateFlush = batchBlendStateFlush;
        if ( batchColorStateFlush > maxBatchColorStateFlush ) maxBatchColorStateFlush = batchColorStateFlush;
        if ( batchAlphaStateFlush > maxBatchAlphaStateFlush ) maxBatchAlphaStateFlush = batchAlphaStateFlush;
        if ( batchTextureChangeFlush > maxBatchTextureChangeFlushes ) maxBatchTextureChangeFlushes = batchTextureChangeFlush;
        if ( batchBufferFullFlush > maxBatchBufferFullFlush ) maxBatchBufferFullFlush = batchBufferFullFlush;
        if ( batchIsolatedFlush > maxBatchIsolatedFlush ) maxBatchIsolatedFlush = batchIsolatedFlush;
        if ( batchLayerFlush > maxBatchLayerFlush ) maxBatchLayerFlush = batchLayerFlush;
        if ( batchNoBatchFlush > maxBatchNoBatchFlush ) maxBatchNoBatchFlush = batchNoBatchFlush;
        if ( batchAnonymousFlush > maxBatchAnonymousFlush ) maxBatchAnonymousFlush = batchAnonymousFlush;

        // Particles.
        if ( particlesUsed > maxParticlesUsed ) maxParticlesUsed = particlesUsed;

        // World profile.
        if ( worldProfile.step > maxWorldProfile.step ) maxWorldProfile.step = worldProfile.step;
        if ( worldProfile.collide > maxWorldProfile.collide ) maxWorldProfile.collide = worldProfile.collide;
        if ( worldProfile.solve > maxWorldProfile.solve ) maxWorldProfile.solve = worldProfile.solve;
        if ( worldProfile.solveInit > maxWorldProfile.solveInit ) maxWorldProfile.solveInit = worldProfile.solveInit;
        if ( worldProfile.solveVelocity > maxWorldProfile.solveVelocity ) maxWorldProfile.solveVelocity = worldProfile.solveVelocity;
        if ( worldProfile.solvePosition > maxWorldProfile.solvePosition ) maxWorldProfile.solvePosition = worldProfile.solvePosition;
        if ( worldProfile.broadphase > maxWorldProfile.broadphase ) maxWorldProfile.broadphase = worldProfile.broadphase;
        if ( worldProfile.solveTOI > maxWorldProfile.solveTOI ) maxWorldProfile.solveTOI = worldProfile.solveTOI;
    }

    /// Reset debug stats.
    void reset( void )
    {
        objectsCount = 0;
        maxObjectsCount = 0;

        objectsEnabled = 0;
        maxObjectsEnabled = 0;

        objectsVisible = 0;
        maxObjectsVisible = 0;

        objectsAwake = 0;
        maxObjectsAwake = 0;

        renderPicked = 0;
        maxRenderPicked = 0;

        renderRequests = 0;
        maxRenderRequests = 0;

        renderFallbacks = 0;
        maxRenderFallbacks = 0;

        bodyCount = 0;
        maxBodyCount = 0;

        jointCount = 0;
        maxJointCount = 0;

        contactCount = 0;
        maxContactCount = 0;

        proxyCount = 0;
        maxProxyCount = 0;

        batchTrianglesSubmitted = 0;
        maxBatchTrianglesSubmitted = 0;

        batchMaxVertexBuffer = 0;
        batchMaxTriangleDrawn = 0;

        batchDrawCallsStrict = 0;
        maxBatchDrawCallsStrict = 0;

        batchDrawCallsSorted = 0;
        maxBatchDrawCallsSorted = 0;

        batchFlushes = 0;
        maxBatchFlushes = 0;

        batchBlendStateFlush = 0;
        maxBatchBlendStateFlush = 0;

        batchColorStateFlush = 0;
        maxBatchColorStateFlush = 0;

        batchAlphaStateFlush = 0;
        maxBatchAlphaStateFlush = 0;

        batchTextureChangeFlush = 0;
        maxBatchTextureChangeFlushes = 0;

        batchBufferFullFlush = 0;
        maxBatchBufferFullFlush = 0;

        batchIsolatedFlush = 0;
        maxBatchIsolatedFlush = 0;

        batchLayerFlush = 0;
        maxBatchLayerFlush = 0;

        batchNoBatchFlush = 0;
        maxBatchNoBatchFlush = 0;

        batchAnonymousFlush = 0;
        maxBatchAnonymousFlush = 0;

        particlesAlloc = 0;
        particlesFree = 0;
        particlesUsed = 0;
        maxParticlesUsed = 0;

        fps = 0.0f;
        minFPS = 10000.0f;
        maxFPS = 0.0f;

        frameCount = 0;

        dMemset( &worldProfile, 0, sizeof(worldProfile) );
        dMemset( &maxWorldProfile, 0, sizeof(maxWorldProfile) );
    }

    U32     objectsCount;
    U32     maxObjectsCount;

    U32     objectsEnabled;
    U32     maxObjectsEnabled;

    U32     objectsVisible;
    U32     maxObjectsVisible;

    U32     objectsAwake;
    U32     maxObjectsAwake;

    U32     renderPicked;
    U32     maxRenderPicked;

    U32     renderRequests;
    U32     maxRenderRequests;

    U32     renderFallbacks;
    U32     maxRenderFallbacks;

    U32     bodyCount;
    U32     maxBodyCount;

    U32     jointCount;
    U32     maxJointCount;

    U32     contactCount;
    U32     maxContactCount;

    U32     proxyCount;
    U32     maxProxyCount;

    U32     batchTrianglesSubmitted;
    U32     maxBatchTrianglesSubmitted;

    U32     batchMaxVertexBuffer;
    U32     batchMaxTriangleDrawn;

    U32     batchDrawCallsStrict;
    U32     maxBatchDrawCallsStrict;

    U32     batchDrawCallsSorted;
    U32     maxBatchDrawCallsSorted;

    U32     batchFlushes;
    U32     maxBatchFlushes;

    U32     batchBlendStateFlush;
    U32     maxBatchBlendStateFlush;

    U32     batchColorStateFlush;
    U32     maxBatchColorStateFlush;

    U32     batchAlphaStateFlush;
    U32     maxBatchAlphaStateFlush;

    U32     batchTextureChangeFlush;
    U32     maxBatchTextureChangeFlushes;

    U32     batchBufferFullFlush;
    U32     maxBatchBufferFullFlush;

    U32     batchIsolatedFlush;
    U32     maxBatchIsolatedFlush;

    U32     batchLayerFlush;
    U32     maxBatchLayerFlush;

    U32     batchNoBatchFlush;
    U32     maxBatchNoBatchFlush;

    U32     batchAnonymousFlush;
    U32     maxBatchAnonymousFlush;

    U32     particlesAlloc;
    U32     particlesFree;
    U32     particlesUsed;
    U32     maxParticlesUsed;

    F32     fps;
    F32     minFPS;
    F32     maxFPS;

    U32     frameCount;

    b2Profile worldProfile;
    b2Profile maxWorldProfile;
};

#endif // _DEBUG_STATS_H_
