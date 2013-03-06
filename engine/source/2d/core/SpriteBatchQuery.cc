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

#include "2d/core/SpriteBatchQuery.h"

#ifndef _SPRITE_BATCH_H_
#include "2d/core/spriteBatch.h"
#endif

#ifndef _SPRITE_BATCH_ITEM_H_
#include "2d/core/spriteBatchItem.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

SpriteBatchQuery::SpriteBatchQuery( SpriteBatch* pSpriteBatch ) :
        mpSpriteBatch(pSpriteBatch),
        mIsRaycastQueryResult(false),
        mMasterQueryKey(0),
        mCheckFixturePoint(false),
        mFixturePoint(0.0f, 0.0f)
{
    // Set debug associations.
    VECTOR_SET_ASSOCIATION( mQueryResults );

    // Clear the query.
    clearQuery();
}

//-----------------------------------------------------------------------------

S32 SpriteBatchQuery::add( SpriteBatchItem* pSpriteBatchItem )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_Add);

    return CreateProxy( pSpriteBatchItem->getLocalAABB(), pSpriteBatchItem );
}

//-----------------------------------------------------------------------------

void SpriteBatchQuery::remove( SpriteBatchItem* pSpriteBatchItem )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_Remove);

    DestroyProxy( pSpriteBatchItem->getProxyId() );
}

//-----------------------------------------------------------------------------

bool SpriteBatchQuery::update( SpriteBatchItem* pSpriteBatchItem, const b2AABB& aabb, const b2Vec2& displacement )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_Update);

    return MoveProxy( pSpriteBatchItem->getProxyId(), aabb, displacement );
}

//-----------------------------------------------------------------------------

U32 SpriteBatchQuery::renderQueryArea( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_RenderQueryArea);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    Query( this, aabb );

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 SpriteBatchQuery::renderQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_RenderQueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    b2RayCastInput rayInput;
    rayInput.p1 = point1;
    rayInput.p2 = point2;

    rayInput.maxFraction = 1.0f;

    RayCast( this, rayInput );

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 SpriteBatchQuery::renderQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_RenderQueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2RayCastInput rayInput;
    rayInput.p1 = point;
    rayInput.p2 = b2Vec2( point.x + b2_linearSlop, point.y + b2_linearSlop );
    rayInput.maxFraction = 1.0f;

    RayCast( this, rayInput );

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

void SpriteBatchQuery::clearQuery( void )
{
    mQueryResults.clear();
}

//-----------------------------------------------------------------------------

void SpriteBatchQuery::sortRaycastQueryResult( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_SortRayCastQueryResult);

    // Ignore if not a ray-cast query result or there are not results to sort.
    if ( !getIsRaycastQueryResult() || getQueryResultsCount() == 0 )
        return;

    // Sort query results.
    dQsort( mQueryResults.address(), mQueryResults.size(), sizeof(SpriteBatchQueryResult), rayCastFractionSort );
}

//-----------------------------------------------------------------------------

bool SpriteBatchQuery::QueryCallback( S32 proxyId )
{    
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_QueryCallback);

    // Fetch sprite batch item.
    SpriteBatchItem* pSpriteBatchItem = static_cast<SpriteBatchItem*>(GetUserData( proxyId ));

    // Ignore if already tagged with the sprite batch query key.
    if ( pSpriteBatchItem->getSpriteBatchQueryKey() == mMasterQueryKey )
        return true;

    // Tag with world query key.
    pSpriteBatchItem->setSpriteBatchQueryKey( mMasterQueryKey );

    SpriteBatchQueryResult queryResult( pSpriteBatchItem );
    mQueryResults.push_back( queryResult );

    return true;
}

//-----------------------------------------------------------------------------

F32 SpriteBatchQuery::RayCastCallback( const b2RayCastInput& input, S32 proxyId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_RayCastCallback);

    // Fetch sprite batch item.
    SpriteBatchItem* pSpriteBatchItem = static_cast<SpriteBatchItem*>(GetUserData( proxyId ));

    // Ignore if already tagged with the sprite batch query key.
    if ( pSpriteBatchItem->getSpriteBatchQueryKey() == mMasterQueryKey )
        return 1.0f;

    // Tag with world query key.
    pSpriteBatchItem->setSpriteBatchQueryKey( mMasterQueryKey );

    SpriteBatchQueryResult queryResult( pSpriteBatchItem );
    mQueryResults.push_back( queryResult );

    return 1.0f;
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK SpriteBatchQuery::rayCastFractionSort(const void* a, const void* b)
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_RayCastFractionSort);

    // Fetch scene objects.
    SpriteBatchQueryResult* pQueryResultA  = (SpriteBatchQueryResult*)a;
    SpriteBatchQueryResult* pQueryResultB  = (SpriteBatchQueryResult*)b;

    // Fetch fractions.
    const F32 queryFractionA = pQueryResultA->mFraction;
    const F32 queryFractionB = pQueryResultB->mFraction;

    if ( queryFractionA < queryFractionB )
        return -1;

    if ( queryFractionA > queryFractionB )
        return 1;

    return 0;
}

