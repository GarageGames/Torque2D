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

#ifndef _VECTOR2D_H
#define _VECTOR2D_H

#ifndef _VECTOR_H_
#include "vector.h"
#endif

//-------------------------------------------------------------------------------------
// Adds accessors for using vector as a 2d structure
// Vector2d class inheriting from vector
//-------------------------------------------------------------------------------------

template <class T>
class Vector2d : public Vector<T>
{
protected:
    U32	mWidth;
    U32 mHeight;
   using Vector<T>::mArray;
   using Vector<T>::mElementCount;
   using Vector<T>::mArraySize;
   using Vector<T>::reserve;

public:
    Vector2d( const U32 initialWidth = 0, const U32 initialHeight = 0 )
    {
        mArray        = 0;
        mElementCount = 0;
        mArraySize    = 0;
        mWidth = initialWidth;
        mHeight = initialHeight;

        if(initialWidth && initialHeight)
            reserve( initialWidth * initialHeight );
    }

    //---------------------------------------------------------

    U32 width()
    {
        return mWidth;
    }

    //---------------------------------------------------------

    U32 height()
    {
        return mHeight;
    }

    //---------------------------------------------------------

    bool resize( const U32 width, const U32 height )
    {
        reserve( width * height );
        mWidth = width;
        mHeight = height;
        return true;
    }

    //---------------------------------------------------------

    T& get( const U32 indexX, const U32 indexY )
    {
        U32 index = ( indexY * mWidth ) + indexX;
        return mArray[index];
    }

    //---------------------------------------------------------

    T& get( const Vector2 v )
    {
        U32 index = ( v.mY * mWidth ) + v.mX;
        return mArray[index];
    }
};

#endif