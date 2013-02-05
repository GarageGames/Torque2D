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

#ifndef _SIM_OBJECT_LIST_H_
#define _SIM_OBJECT_LIST_H_

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

//-----------------------------------------------------------------------------

class SimObject;

//-----------------------------------------------------------------------------

class SimObjectList : public VectorPtr<SimObject*>
{
   static S32 QSORT_CALLBACK compareId(const void* a,const void* b);

public:
   void pushBack(SimObject*);       ///< Add the SimObject* to the end of the list, unless it's already in the list.
   void pushBackForce(SimObject*);  ///< Add the SimObject* to the end of the list, moving it there if it's already present in the list.
   void pushFront(SimObject*);      ///< Add the SimObject* to the start of the list.
   void remove(SimObject*);         ///< Remove the SimObject* from the list; may disrupt order of the list.

   inline SimObject* at(S32 index) const {  if(index >= 0 && index < size()) return (*this)[index]; return NULL; }

   /// Remove the SimObject* from the list; guaranteed to preserve list order.
   void removeStable(SimObject* pObject);

   void sortId();                   ///< Sort the list by object ID.
};

#endif // _SIM_OBJECT_LIST_H_