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

#ifndef _WORLD_QUERY_FILTER_H_
#define _WORLD_QUERY_FILTER_H_

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

///-----------------------------------------------------------------------------

struct WorldQueryFilter
{
    WorldQueryFilter()
        {
            resetQuery();
        }

    WorldQueryFilter(
        const U32 sceneLayerMask,
        const U32 sceneGroupMask,
        const bool enabledFilter,
        const bool visibleFilter,
        const bool pickingAllowedFilter,
        const bool alwaysInScopeFilter ) :        
        mSceneLayerMask( sceneLayerMask ),
        mSceneGroupMask( sceneGroupMask ),
        mEnabledFilter( enabledFilter ),
        mVisibleFilter( visibleFilter ),
        mPickingAllowedFilter( pickingAllowedFilter ),
        mAlwaysInScopeFilter( alwaysInScopeFilter )
        {
        }

    void resetQuery( void )
    {
        mSceneLayerMask       = MASK_ALL;
        mSceneGroupMask       = MASK_ALL;
        mEnabledFilter        = true;
        mVisibleFilter        = false;
        mPickingAllowedFilter = true;
        mAlwaysInScopeFilter  = false;
    }

    inline void     setEnabledFilter( const bool filter )           { mEnabledFilter = filter; }
    inline bool     getEnabledFilter( void ) const                  { return mEnabledFilter; }
    inline void     setVisibleFilter( const bool filter )           { mVisibleFilter = filter; }
    inline bool     getVisibleFilter( void ) const                  { return mVisibleFilter; }
    inline void     setPickingAllowedFilter( const bool filter )    { mPickingAllowedFilter = filter; }
    inline bool     getPickingAllowedFilter( void ) const           { return mPickingAllowedFilter; }
    inline void     setAlwaysInScopeFilter( const bool filter )     { mAlwaysInScopeFilter = filter; }
    inline bool     getAlwaysInScopeFilter( void ) const            { return mAlwaysInScopeFilter; }
    
    U32     mSceneLayerMask;
    U32     mSceneGroupMask;
    bool    mEnabledFilter;
    bool    mVisibleFilter;
    bool    mPickingAllowedFilter;
    bool    mAlwaysInScopeFilter;
};

#endif // _WORLD_QUERY_FILTER_H_