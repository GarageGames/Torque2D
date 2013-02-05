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

#ifndef _VECTOREXT_H
#define _VECTOREXT_H

#ifndef _VECTOR_H_
#include "vector.h"
#endif

//-------------------------------------------------------------------------------------
// Simple wrapper for vector so it works like the Heap class
// adds:
// enqueue( T )	- adds item to back
// dequeue()	- removes item from front
// item()		- gets item from front
//-------------------------------------------------------------------------------------

template <class T>
class Queue : public Vector<T>
{
   using Vector<T>::mElementCount;
   using Vector<T>::pop_front;
   using Vector<T>::front;
   
public:

    //---------------------------------------------------------
    
    Queue()
    {
    }

    //---------------------------------------------------------
    
    void enqueue( T element )
    {
        push_back( element );
    }

    //---------------------------------------------------------
    
    void dequeue()
    {
        if( mElementCount >= 0 )
        {
            pop_front();
        }
    }

    //---------------------------------------------------------
    T& item()
    {
        return front();
    }
};

#endif