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

#include "2d/scene/WorldQuery.h"

#ifndef _SCENE_H_
#include "Scene.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

WorldQuery::WorldQuery( Scene* pScene ) :
        mpScene(pScene),
        mIsRaycastQueryResult(false),
        mMasterQueryKey(0),
        mCheckFixturePoint(false),
        mFixturePoint(0.0f, 0.0f)
{
    // Set debug associations.
    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; n++ )
    {
        VECTOR_SET_ASSOCIATION( mLayeredQueryResults[n] );
    }
    VECTOR_SET_ASSOCIATION( mQueryResults );

    // Clear the query.
    clearQuery();
}

//-----------------------------------------------------------------------------

S32 WorldQuery::add( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_Add);

    return CreateProxy( pSceneObject->getAABB(), static_cast<PhysicsProxy*>(pSceneObject) );
}

//-----------------------------------------------------------------------------

void WorldQuery::remove( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_Remove);

    DestroyProxy( pSceneObject->getWorldProxy() );
}

//-----------------------------------------------------------------------------

bool WorldQuery::update( SceneObject* pSceneObject, const b2AABB& aabb, const b2Vec2& displacement )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_Update);

    return MoveProxy( pSceneObject->getWorldProxy(), aabb, displacement );
}

//-----------------------------------------------------------------------------

void WorldQuery::addAlwaysInScope( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AddAlwaysInScope);

    // Sanity!
    for( typeSceneObjectVector::iterator itr = mAlwaysInScopeSet.begin(); itr != mAlwaysInScopeSet.end(); ++itr )
    {
        AssertFatal( (*itr) != pSceneObject, "Object attempted to be in Always-in-Scope more than once." );
    }

    // Add to always-in-scope.
    mAlwaysInScopeSet.push_back( pSceneObject );

    // Set always in scope.
    pSceneObject->mAlwaysInScope = true;
}

//-----------------------------------------------------------------------------

void WorldQuery::removeAlwaysInScope( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RemoveAlwaysInScope);

    // Remove from always-in-scope.
    for( typeSceneObjectVector::iterator itr = mAlwaysInScopeSet.begin(); itr != mAlwaysInScopeSet.end(); ++itr )
    {
        // Skip if not object.
        if ( (*itr) != pSceneObject )
            continue;

        mAlwaysInScopeSet.erase_fast( itr );

        // Reset always in scope.
        pSceneObject->mAlwaysInScope = false;

        return;
    }

    AssertFatal( false, "Object attempted to be removed from Always-in-Scope but wasn't present." );
}

//-----------------------------------------------------------------------------

U32 WorldQuery::fixtureQueryArea( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_FixtureQueryArea);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    mpScene->getWorld()->QueryAABB( this, aabb );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::fixtureQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_FixtureQueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    mpScene->getWorld()->RayCast( this, point1, point2 );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::fixtureQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_FixtureQueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    aabb.lowerBound = point;
    aabb.upperBound = point;
    mCheckFixturePoint = true;
    mFixturePoint = point;
    mpScene->getWorld()->QueryAABB( this, aabb );
    mCheckFixturePoint = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::renderQueryArea( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RenderQueryArea);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    Query( this, aabb );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::renderQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RenderQueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    b2RayCastInput rayInput;
    rayInput.p1 = point1;
    rayInput.p2 = point2;

    rayInput.maxFraction = 1.0f;

    RayCast( this, rayInput );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::renderQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RenderQueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2RayCastInput rayInput;
    rayInput.p1 = point;
    rayInput.p2 = b2Vec2( point.x + b2_linearSlop, point.y + b2_linearSlop );
    rayInput.maxFraction = 1.0f;

    RayCast( this, rayInput );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryArea( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryAreaAABB);

    // Query.
    renderQueryArea( aabb );
    mMasterQueryKey--;
    fixtureQueryArea( aabb );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryArea( const Vector2& lowerBound, const Vector2& upperBound )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryAreaBounds);

    // Calculate AABB.
    b2AABB aabb;
    aabb.lowerBound.Set( getMin( lowerBound.x, upperBound.x ), getMin( lowerBound.x, upperBound.x ) );
    aabb.upperBound.Set( getMax( lowerBound.x, upperBound.x ), getMax( lowerBound.x, upperBound.x ) );

    // Query.
    return anyQueryArea( aabb );
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryRay);

    // Query.
    renderQueryRay( point1, point2 );
    mMasterQueryKey--;
    fixtureQueryRay( point1, point2 );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryPoint);

    // Query.
    renderQueryPoint( point );
    mMasterQueryKey--;
    fixtureQueryPoint( point );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

void WorldQuery::clearQuery( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_ClearQuery);

    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; n++ )
    {
        mLayeredQueryResults[n].clear();
    }

    mQueryResults.clear();
}

//-----------------------------------------------------------------------------

typeWorldQueryResultVector& WorldQuery::getLayeredQueryResults( const U32 layer ) 
{
    // Sanity!
    AssertFatal( layer < MAX_LAYERS_SUPPORTED, "WorldQuery::getResults() - Layer out of range." );

    return mLayeredQueryResults[ layer ];
}

//-----------------------------------------------------------------------------

void WorldQuery::sortRaycastQueryResult( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_SortRayCastQueryResult);

    // Ignore if not a ray-cast query result or there are not results to sort.
    if ( !getIsRaycastQueryResult() || getQueryResultsCount() == 0 )
        return;

    // Sort query results.
    dQsort( mQueryResults.address(), mQueryResults.size(), sizeof(WorldQueryResult), rayCastFractionSort );

    for ( U32 layer = 0; layer < MAX_LAYERS_SUPPORTED; ++layer )
    {
        // Fetch layer query results.
        typeWorldQueryResultVector& layerQueryResults = mLayeredQueryResults[layer];

        // Skip if nothing in the layer query results.
        if ( layerQueryResults.size() == 0 )
            continue;

        // Sort query results.
        dQsort( layerQueryResults.address(), layerQueryResults.size(), sizeof(WorldQueryResult), rayCastFractionSort );
    }
}

//-----------------------------------------------------------------------------

bool WorldQuery::ReportFixture( b2Fixture* fixture )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_ReportFixture);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(fixture->GetBody()->GetUserData());
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return true;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return true;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return true;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return true;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getIsPickingAllowed() )
        return true;

    // Check fixture point.
    if ( mCheckFixturePoint && !fixture->TestPoint( mFixturePoint ) )
        return true;

    // Tag with world query key.
    pSceneObject->setWorldQueryKey( mMasterQueryKey );

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );
    }

    return true;
}

//-----------------------------------------------------------------------------

F32 WorldQuery::ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, F32 fraction )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_ReportFixtureRay);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(fixture->GetBody()->GetUserData());
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return 1.0f;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return 1.0f;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return 1.0f;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return 1.0f;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getIsPickingAllowed() )
        return 1.0f;

    // Tag with world query key.
    pSceneObject->setWorldQueryKey( mMasterQueryKey );

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Fetch collision shape index.
    const S32 shapeIndex = pSceneObject->getCollisionShapeIndex( fixture );

    // Sanity!
    AssertFatal( shapeIndex >= 0, "2dWorldQuery::ReportFixture() - Cannot find shape index reported on physics proxy of a fixture." );

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject, point, normal, fraction, (U32)shapeIndex );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );
    }

    return 1.0f;
}

//-----------------------------------------------------------------------------

bool WorldQuery::QueryCallback( S32 proxyId )
{    
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_QueryCallback);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(GetUserData( proxyId ));
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return true;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return true;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return true;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return true;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getIsPickingAllowed() )
        return true;

    // Tag with world query key.
    pSceneObject->setWorldQueryKey( mMasterQueryKey );

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( pSceneObject );
    }

    return true;
}

//-----------------------------------------------------------------------------

F32 WorldQuery::RayCastCallback( const b2RayCastInput& input, S32 proxyId )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RayCastCallback);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(GetUserData( proxyId ));
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return 1.0f;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return 1.0f;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return 1.0f;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return 1.0f;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getIsPickingAllowed() )
        return 1.0f;

    // Tag with world query key.
    pSceneObject->setWorldQueryKey( mMasterQueryKey );

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );
    }

    return 1.0f;
}

//-----------------------------------------------------------------------------

void WorldQuery::injectAlwaysInScope( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_InjectAlwaysInScope);

    // Finish if filtering always-in-scope.
    if ( mQueryFilter.mAlwaysInScopeFilter )
        return;

    // Iterate always-in-scope.
    for( typeSceneObjectVector::iterator itr = mAlwaysInScopeSet.begin(); itr != mAlwaysInScopeSet.end(); ++itr )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = (*itr);

        // Ignore if already tagged with the world query key.
        if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
            continue;

        // Tag with world query key.
        pSceneObject->setWorldQueryKey( mMasterQueryKey );

        // Enabled filter.
        if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
            continue;

        // Visible filter.
        if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
            continue;

        // Picking allowed filter.
        if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getIsPickingAllowed() )
            continue;

        // Fetch layer and group masks.
        const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
        const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

        // Compare masks and report.
        if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
        {
            mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( pSceneObject );
            mQueryResults.push_back( pSceneObject );
        }
    }
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK WorldQuery::rayCastFractionSort(const void* a, const void* b)
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RayCastFractionSort);

    // Fetch scene objects.
    WorldQueryResult* pQueryResultA  = (WorldQueryResult*)a;
    WorldQueryResult* pQueryResultB  = (WorldQueryResult*)b;

    // Fetch fractions.
    const F32 queryFractionA = pQueryResultA->mFraction;
    const F32 queryFractionB = pQueryResultB->mFraction;

    if ( queryFractionA < queryFractionB )
        return -1;

    if ( queryFractionA > queryFractionB )
        return 1;

    return 0;
}

