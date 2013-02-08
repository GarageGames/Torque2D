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

#ifndef _SCENE_RENDER_QUEUE_H_
#include "2d/scene/SceneRenderQueue.h"
#endif

#ifndef _SCENE_RENDER_OBJECT_H_
#include "2d/scene/SceneRenderObject.h"
#endif

//-----------------------------------------------------------------------------

static EnumTable::Enums renderSortLookup[] =
                {
                { SceneRenderQueue::RENDER_SORT_OFF,            "Off" },
                { SceneRenderQueue::RENDER_SORT_NEWEST,         "New" },
                { SceneRenderQueue::RENDER_SORT_OLDEST,         "Old" },
                { SceneRenderQueue::RENDER_SORT_BATCH,          "Batch" },
                { SceneRenderQueue::RENDER_SORT_GROUP,          "Group" },
                { SceneRenderQueue::RENDER_SORT_XAXIS,          "X" },
                { SceneRenderQueue::RENDER_SORT_YAXIS,          "Y" },
                { SceneRenderQueue::RENDER_SORT_ZAXIS,          "Z" },
                { SceneRenderQueue::RENDER_SORT_INVERSE_XAXIS,  "-X" },
                { SceneRenderQueue::RENDER_SORT_INVERSE_YAXIS,  "-Y" },
                { SceneRenderQueue::RENDER_SORT_INVERSE_ZAXIS,  "-Z" },
                };

EnumTable SceneRenderQueue::renderSortTable(sizeof(renderSortLookup) /  sizeof(EnumTable::Enums), &renderSortLookup[0]);

//-----------------------------------------------------------------------------

SceneRenderQueue::RenderSort SceneRenderQueue::getRenderSortEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(renderSortLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(renderSortLookup[i].label, label) == 0)
            return((RenderSort)renderSortLookup[i].index);

    // Warn.
    Con::warnf( "SceneRenderQueue::getRenderSortEnum() - Invalid sort enum of '%s'", label );

    return SceneRenderQueue::RENDER_SORT_INVALID;
}

//-----------------------------------------------------------------------------

const char* SceneRenderQueue::getRenderSortDescription( const RenderSort& sortMode )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(renderSortLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( renderSortLookup[i].index == sortMode )
            return renderSortLookup[i].label;
    }

    // Warn.
    Con::warnf( "SceneRenderQueue::getRenderSortDescription() - Invalid sort enum." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredNewFrontSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    // Use serial Id,
    return pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}
    
//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredOldFrontSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    // Use reverse serial Id,
    return pSceneRenderRequestB->mSerialId - pSceneRenderRequestA->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredDepthSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    // Fetch depths.
    const F32 depthA = pSceneRenderRequestA->mDepth;
    const F32 depthB = pSceneRenderRequestB->mDepth;

    return depthA < depthB ? 1 : depthA > depthB ? -1 : pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredInverseDepthSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    // Fetch depths.
    const F32 depthA = pSceneRenderRequestA->mDepth;
    const F32 depthB = pSceneRenderRequestB->mDepth;

    return depthA < depthB ? -1 : depthA > depthB ? 1 : pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layerBatchOrderSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    // Fetch scene render objects.
    SceneRenderObject* pSceneRenderObjectA = pSceneRenderRequestA->mpSceneRenderObject;
    SceneRenderObject* pSceneRenderObjectB = pSceneRenderRequestB->mpSceneRenderObject;

    // Fetch batch render isolation.
    const bool renderIsolatedA = pSceneRenderObjectA->getBatchIsolated();
    const bool renderIsolatedB = pSceneRenderObjectB->getBatchIsolated();

    // A not render isolated?
    if ( !renderIsolatedA )
    {
        // Use the serial Id if neither are render isolated.
        if ( !renderIsolatedB )
            return pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;

        // A not render isolated but B is.
        return 1;
    }

    // B not render isolated?
    if ( !renderIsolatedB )
    {
        // A is render isolated.
        return -1;
    }

    // A and B are render isolated so use serial Id,
    return pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layerGroupOrderSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    // Fetch the groups.
    StringTableEntry renderGroupA = pSceneRenderRequestA->mRenderGroup;
    StringTableEntry renderGroupB = pSceneRenderRequestB->mRenderGroup;

    // Sort by render group (address, arbitrary but static) and use age if render groups are identical.
    return renderGroupA == renderGroupB ? pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId : renderGroupA < renderGroupB ? -1 : 1;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredXSortPointSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    const F32 x1 = pSceneRenderRequestA->mWorldPosition.x + pSceneRenderRequestA->mSortPoint.x;
    const F32 x2 = pSceneRenderRequestB->mWorldPosition.x + pSceneRenderRequestB->mSortPoint.x;

    // We sort lower x values before higher values.
    return x1 < x2 ? -1 : x1 > x2 ? 1 : pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredYSortPointSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    const F32 y1 = pSceneRenderRequestA->mWorldPosition.y + pSceneRenderRequestA->mSortPoint.y;
    const F32 y2 = pSceneRenderRequestB->mWorldPosition.y + pSceneRenderRequestB->mSortPoint.y;

    // We sort lower y values before higher values.
    return y1 < y2 ? -1 : y1 > y2 ? 1 : pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredInverseXSortPointSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    const F32 x1 = pSceneRenderRequestA->mWorldPosition.x + pSceneRenderRequestA->mSortPoint.x;
    const F32 x2 = pSceneRenderRequestB->mWorldPosition.x + pSceneRenderRequestB->mSortPoint.x;

    // We sort higher x values before lower values.
    return x1 < x2 ? 1 : x1 > x2 ? -1 : pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneRenderQueue::layeredInverseYSortPointSort(const void* a, const void* b)
{
    // Fetch scene render requests.
    SceneRenderRequest* pSceneRenderRequestA  = *((SceneRenderRequest**)a);
    SceneRenderRequest* pSceneRenderRequestB  = *((SceneRenderRequest**)b);

    const F32 y1 = pSceneRenderRequestA->mWorldPosition.y + pSceneRenderRequestA->mSortPoint.y;
    const F32 y2 = pSceneRenderRequestB->mWorldPosition.y + pSceneRenderRequestB->mSortPoint.y;

    // We sort higher y values before lower values.
    return y1 < y2 ? 1 : y1 > y2 ? -1 : pSceneRenderRequestA->mSerialId - pSceneRenderRequestB->mSerialId;
}
