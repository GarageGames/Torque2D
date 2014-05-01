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
#include "2d/core/SpriteBatch.h"
#endif

#ifndef _SPRITE_BATCH_ITEM_H_
#include "2d/core/SpriteBatchItem.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

SpriteBatchQuery::SpriteBatchQuery( SpriteBatch* pSpriteBatch ) :
        mpSpriteBatch(pSpriteBatch),
        mIsRaycastQueryResult(false),
        mMasterQueryKey(0),
        mCheckPoint(false),
        mComparePoint(0.0f, 0.0f)
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

U32 SpriteBatchQuery::queryOOBB( const b2AABB& aabb, b2PolygonShape& oobb, const bool targetOOBB )
{
  // This function is used exclusively when picking rectangular areas using CompositeSprite's pickArea ConsoleMethod
  // For rendering, SpriteBatchQuery::queryArea is used instead

  // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_QueryArea);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

  mComparePolygonShape.Set(oobb.m_vertices,4);
  mComparePolygonShape.m_centroid = oobb.m_centroid;

    mCompareTransform.SetIdentity();
    mCheckOOBB = targetOOBB;
    Query( this, aabb );
    mCheckOOBB = false;

    return getQueryResultsCount();
}

U32 SpriteBatchQuery::queryArea( const b2AABB& aabb, const bool targetOOBB )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_QueryArea);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2Vec2 verts[4];
    verts[0].Set( aabb.lowerBound.x, aabb.lowerBound.y );
    verts[1].Set( aabb.upperBound.x, aabb.lowerBound.y );
    verts[2].Set( aabb.upperBound.x, aabb.upperBound.y );
    verts[3].Set( aabb.lowerBound.x, aabb.upperBound.y );
    mComparePolygonShape.Set( verts, 4 );
    mCompareTransform.SetIdentity();
    mCheckOOBB = targetOOBB;
    Query( this, aabb );
    mCheckOOBB = false;

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 SpriteBatchQuery::queryRay( const Vector2& point1, const Vector2& point2, const bool targetOOBB )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_QueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    mCompareRay.p1 = point1;
    mCompareRay.p2 = point2;
    mCompareRay.maxFraction = 1.0f;
    mCompareTransform.SetIdentity();
    mCheckOOBB = targetOOBB;
    RayCast( this, mCompareRay );
    mCheckOOBB = false;

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 SpriteBatchQuery::queryPoint( const Vector2& point, const bool targetOOBB )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchQuery_QueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    aabb.lowerBound = point;
    aabb.upperBound = point;
    mCompareTransform.SetIdentity();
    mCheckOOBB = targetOOBB;
    Query( this, aabb );
    mCheckOOBB = false;

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

    // Check OOBB.
    if ( mCheckOOBB )
    {
            // Fetch the shapes render OOBB.
        b2PolygonShape oobb;
        oobb.Set( pSpriteBatchItem->getLocalOOBB(), 4);

        if ( mCheckPoint )
        {
            if ( !oobb.TestPoint( mCompareTransform, mComparePoint ) )
                return true;
        }
        else
        {
            if ( !b2TestOverlap( &mComparePolygonShape, 0, &oobb, 0, mCompareTransform, mCompareTransform ) )
                return true;
        }
    }

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

    // Check OOBB.
    if ( mCheckOOBB )
    {
        // Fetch the shapes render OOBB.
        b2PolygonShape oobb;
        oobb.Set( pSpriteBatchItem->getLocalOOBB(), 4);
        b2RayCastOutput rayOutput;
        if ( !oobb.RayCast( &rayOutput, mCompareRay, mCompareTransform, 0 ) )
            return true;
    }

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

