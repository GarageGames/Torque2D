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

#ifndef _SIMCOMPONENT_H_
#define _SIMCOMPONENT_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _STREAM_H_
#include "io/stream.h"
#endif

//-----------------------------------------------------------------------------

class SimComponent : public SimObject
{
   typedef SimObject Parent;

private:
   VectorPtr<SimComponent *> mComponentList; ///< The Component List
   void *mMutex;                             ///< Component List Mutex

   SimObjectPtr<SimComponent> mOwner;        ///< The component which owns this one.

   bool _registerComponents( SimComponent *owner );
   void _unregisterComponents();

protected: 
   bool mEnabled;

   // Non-const getOwner for derived classes
   SimComponent *_getOwner() { return mOwner; }

   /// Returns a const reference to private mComponentList
   typedef VectorPtr<SimComponent *>::iterator SimComponentIterator;
   VectorPtr<SimComponent *> &lockComponentList()
   {
      Mutex::lockMutex( mMutex );
      return mComponentList;
   };

   void unlockComponentList()
   {
      Mutex::unlockMutex( mMutex );
   }

   /// onComponentRegister is called on each component by it's owner. If a component
   /// has no owner, onComponentRegister will not be called on it. The purpose
   /// of onComponentRegister is to allow a component to check for any external
   /// interfaces, or other dependencies which it needs to function. If any component
   /// in a component hierarchy returns false from it's onComponentRegister call
   /// the entire hierarchy is invalid, and SimObject::onAdd will fail on the
   /// top-level component. To put it another way, if a component contains other
   /// components, it will be registered successfully with Sim if each subcomponent
   /// returns true from onComponentRegister. If a component does not contain
   /// other components, it will not receive an onComponentRegister call.
   ///
   /// Overloads of this method must pass the call along to their parent, as is
   /// shown in the example below. 
   ///
   /// @code
   /// bool FooComponent::onComponentRegister( SimComponent *owner )
   /// {
   ///    if( !Parent::onComponentRegister( owner ) )
   ///       return false;
   ///    ...
   /// }
   /// @endcode
   virtual bool onComponentRegister( SimComponent *owner )
   {
      mOwner = owner;
      return true;
   }

   /// onUnregister is called when the owner is unregistering. Your object should
   /// do cleanup here, as well as pass a call up the chain to the parent.
   virtual void onComponentUnRegister()
   {
      mOwner = NULL;
   }


public:
   DECLARE_CONOBJECT(SimComponent);

   /// Constructor
   /// Add this component
   SimComponent();

   /// Destructor
   /// Remove this component and destroy child references
   virtual ~SimComponent();

public:

   virtual bool onAdd();
   virtual void onRemove();

   static void initPersistFields();

   virtual bool processArguments(S32 argc, const char **argv);

   /// Will return true if this object contains components.
   bool hasComponents() const { return ( mComponentList.size() > 0 ); };

   /// The component which owns this object
   const SimComponent *getOwner() const { return mOwner; };

   // Component Information
   inline virtual StringTableEntry  getComponentName() { return StringTable->insert( getClassName() ); };

   /// Add Component to this one
   virtual bool addComponent( SimComponent *component );

   /// Remove Component from this one
   virtual bool removeComponent( SimComponent *component );

   /// Clear Child components of this one
   virtual bool clearComponents() { mComponentList.clear(); return true; };

   virtual bool onComponentAdd(SimComponent *target);
   virtual void onComponentRemove(SimComponent *target);

   inline U32 getComponentCount() { return mComponentList.size(); }
   inline SimComponent *getComponent( const U32 index ) { return mComponentList[index]; }

   static bool setEnabled( void* obj, const char* data ) { static_cast<SimComponent*>(obj)->setEnabled( dAtob( data ) ); return false; };
   virtual void setEnabled( const bool enabled ) { mEnabled = enabled; }
   bool isEnabled() const { return mEnabled; }
   static bool writeEnabled( void* obj, StringTableEntry pFieldName ) { return static_cast<SimComponent*>(obj)->mEnabled == false; }

   virtual void write(Stream &stream, U32 tabStop, U32 flags = 0);
   virtual bool writeField(StringTableEntry fieldname, const char* value);

   virtual void onUpdate( void ) {}
   virtual void onAddToScene( void ) {}
   virtual void onRemoveFromScene( void ) {}

   /// Call the given method and arguments on this component or, if not handled by
   /// this component, then on its children.
   /// @return True if the given method and arguments was handled by the component or 
   /// one of its children.
   bool callMethodOnComponents( U32 argc, const char* argv[], const char** result );
};

#endif // _SIMCOMPONENT_H_
