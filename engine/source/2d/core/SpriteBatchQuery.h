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

#ifndef _SPRITE_BATCH_QUERY_H_
#define _SPRITE_BATCH_QUERY_H_

#ifndef _SPRITE_BATCH_QUERY_RESULT_H_
#include "2d/core/SpriteBatchQueryResult.h"
#endif

///-----------------------------------------------------------------------------

class SpriteBatch;
class SpriteBatchItem;

///-----------------------------------------------------------------------------

class SpriteBatchQuery :
    protected b2DynamicTree,
    public b2QueryCallback,
    public b2RayCastCallback
{
public:
    SpriteBatchQuery( SpriteBatch* pSpriteBatch );
    virtual         ~SpriteBatchQuery() {}

    /// Standard scope.
    S32             add( SpriteBatchItem* pSpriteBatchItem );
    void            remove( SpriteBatchItem* pSpriteBatchItem );
    bool            update( SpriteBatchItem* pSpriteBatchItem, const b2AABB& aabb, const b2Vec2& displacement );

    //// Spatial queries.
    U32             queryArea( const b2AABB& aabb, const bool targetOOBB );
    U32             queryOOBB( const b2AABB& aabb, b2PolygonShape& oobb, const bool targetOOBB );
    U32             queryRay( const Vector2& point1, const Vector2& point2, const bool targetOOBB );
    U32             queryPoint( const Vector2& point, const bool targetOOBB );
 
    /// Results.
    void            clearQuery( void );
    typeSpriteBatchQueryResultVector& getQueryResults( void ) { return mQueryResults; }
    inline U32      getQueryResultsCount( void ) const { return mQueryResults.size(); }
    inline bool     getIsRaycastQueryResult( void ) const { return mIsRaycastQueryResult; }
    void            sortRaycastQueryResult( void );

    /// Unused result callbacks.
    virtual bool    ReportFixture( b2Fixture* fixture ) { return true; }
    virtual F32     ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, F32 fraction ) { return 1.0; }


    /// Callbacks.
    bool            QueryCallback( S32 proxyId );
    F32             RayCastCallback( const b2RayCastInput& input, S32 proxyId );

private:
    static S32      QSORT_CALLBACK rayCastFractionSort(const void* a, const void* b);

private:
    SpriteBatch*                mpSpriteBatch;
    b2PolygonShape              mComparePolygonShape;
    b2RayCastInput              mCompareRay;
    b2Vec2                      mComparePoint;
    b2Transform                 mCompareTransform;
    bool                        mCheckOOBB;
    bool                        mCheckPoint;
    typeSpriteBatchQueryResultVector mQueryResults;
    bool                        mIsRaycastQueryResult;
    typeSceneObjectVector       mAlwaysInScopeSet;
    U32                         mMasterQueryKey;
};

#endif // _SPRITE_BATCH_QUERY_H_
