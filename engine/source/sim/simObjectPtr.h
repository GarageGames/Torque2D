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

#ifndef _SIM_OBJECT_PTR_H_
#define _SIM_OBJECT_PTR_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif


//---------------------------------------------------------------------------
/// Smart SimObject pointer.
///
/// This class keeps track of the book-keeping necessary
/// to keep a registered reference to a SimObject or subclass
/// thereof.
///
/// Normally, if you want the SimObject to be aware that you
/// have a reference to it, you must call SimObject::registerReference()
/// when you create the reference, and SimObject::unregisterReference() when
/// you're done. If you change the reference, you must also register/unregister
/// it. This is a big headache, so this class exists to automatically
/// keep track of things for you.
///
/// @code
///     // Assign an object to the
///     SimObjectPtr<GameBase> mOrbitObject = Sim::findObject("anObject");
///
///     // Use it as a GameBase*.
///     mOrbitObject->getWorldBox().getCenter(&mPosition);
///
///     // And reassign it - it will automatically update the references.
///     mOrbitObject = Sim::findObject("anotherObject");
/// @endcode
template <class T> class SimObjectPtr
{
  private:
   SimObject *mObj;

  public:
   SimObjectPtr() { mObj = 0; }
   SimObjectPtr(T* ptr)
   {
      mObj = ptr;
      if(mObj)
         mObj->registerReference(&mObj);
   }
   SimObjectPtr(const SimObjectPtr<T>& rhs)
   {
      mObj = const_cast<T*>(static_cast<const T*>(rhs));
      if(mObj)
         mObj->registerReference(&mObj);
   }
   SimObjectPtr<T>& operator=(const SimObjectPtr<T>& rhs)
   {
      if(this == &rhs)
         return(*this);
      if(mObj)
         mObj->unregisterReference(&mObj);
      mObj = const_cast<T*>(static_cast<const T*>(rhs));
      if(mObj)
         mObj->registerReference(&mObj);
      return(*this);
   }
   ~SimObjectPtr()
   {
      if(mObj)
         mObj->unregisterReference(&mObj);
   }
   SimObjectPtr<T>& operator= (T *ptr)
   {
      if(mObj != (SimObject *) ptr)
      {
         if(mObj)
            mObj->unregisterReference(&mObj);
         mObj = (SimObject *) ptr;
         if (mObj)
            mObj->registerReference(&mObj);
      }
      return *this;
   }
#if defined(__MWERKS__) && (__MWERKS__ < 0x2400)
   // CW 5.3 seems to get confused comparing SimObjectPtrs...
   bool operator == (const SimObject *ptr) { return mObj == ptr; }
   bool operator != (const SimObject *ptr) { return mObj != ptr; }
#endif
   bool isNull() const   { return mObj == 0; }
   bool notNull() const   { return mObj != 0; }
   T* operator->() const { return static_cast<T*>(mObj); }
   T& operator*() const  { return *static_cast<T*>(mObj); }
   operator T*() const   { return static_cast<T*>(mObj)? static_cast<T*>(mObj) : 0; }
};

#endif // _SIM_OBJECT_PTR_H_