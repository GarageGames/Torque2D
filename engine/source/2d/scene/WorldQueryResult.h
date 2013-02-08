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

#ifndef _WORLD_QUERY_RESULT_H_
#define _WORLD_QUERY_RESULT_H_

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

///-----------------------------------------------------------------------------

class SceneObject;

///-----------------------------------------------------------------------------

struct WorldQueryResult
{
    WorldQueryResult() :
        mpSceneObject( NULL ),
        mShapeIndex( 0 ),
        mPoint( 0.0f, 0.0f ),
        mNormal( 0.0f, 0.0f ),
        mFraction( 0.0f )
    {
    }

    /// Initialize a non-ray-cast result.
    WorldQueryResult( SceneObject* pSceneObject ) :
        mpSceneObject( pSceneObject ),
        mShapeIndex( 0 ),
        mPoint( 0.0f, 0.0f ),
        mNormal( 0.0f, 0.0f ),
        mFraction( 0.0f )
    {
    }

    /// Initialize a ray-cast result.    
    WorldQueryResult( SceneObject* pSceneObject, const b2Vec2& point, const b2Vec2& normal, const F32 fraction, const U32 shapeIndex ) :
        mpSceneObject( pSceneObject ),
        mShapeIndex( shapeIndex ),
        mPoint( point ),
        mNormal( normal ),
        mFraction( fraction )
    {
    }

    b2Vec2          mPoint;
    b2Vec2          mNormal;
    F32             mFraction;
    SceneObject*    mpSceneObject;
    U32             mShapeIndex;
};

///-----------------------------------------------------------------------------

typedef Vector<WorldQueryResult> typeWorldQueryResultVector;

#endif // _WORLD_QUERY_RESULT_H_