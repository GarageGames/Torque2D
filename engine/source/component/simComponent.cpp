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

#include "platform/platform.h"
#include "sim/simBase.h"
#include "console/consoleTypes.h"
#include "component/simComponent.h"
#include "io/stream.h"
#include "behaviors/behaviorTemplate.h"

#include "simComponent_ScriptBinding.h"

SimComponent::SimComponent() : mOwner( NULL )
{
   mComponentList.clear();
   mMutex = Mutex::createMutex();
   
   mEnabled = true;
}

SimComponent::~SimComponent()
{
   Mutex::destroyMutex( mMutex );
   mMutex = NULL;
}

IMPLEMENT_CONOBJECT(SimComponent);

bool SimComponent::onAdd()
{
    if( !Parent::onAdd() )
        return false;
   
    if( !_registerComponents( this ) )
        return false;

    return true;
}

bool SimComponent::_registerComponents( SimComponent *owner )
{
   // This method will return true if the object contains no components. See the
   // documentation for SimComponent::onComponentRegister for more information
   // on this behavior.
   bool ret =  true;

   // If this doesn't contain components, don't even lock the list.
   if( hasComponents() )
   {
      VectorPtr<SimComponent *> &components = lockComponentList();
      for( SimComponentIterator i = components.begin(); i != components.end(); i++ )
      {
         if( !(*i)->onComponentRegister( owner ) )
         {
            ret = false;
            break;
         }

         AssertFatal( (*i)->mOwner == owner, "Component failed to call parent onComponentRegister!" );

         // Recurse
         if( !(*i)->_registerComponents( owner ) )
         {
            ret = false;
            break;
         }
      }

      unlockComponentList();
   }

   return ret;
}

void SimComponent::_unregisterComponents()
{
   if( !hasComponents() )
      return;

   VectorPtr<SimComponent *> &components = lockComponentList();
   for( SimComponentIterator i = components.begin(); i != components.end(); i++ )
   {
      (*i)->onComponentUnRegister();

      AssertFatal( (*i)->mOwner == NULL, "Component failed to call parent onUnRegister" );

      // Recurse
      (*i)->_unregisterComponents();
   }

   unlockComponentList();
}

void SimComponent::onRemove()
{
   _unregisterComponents();

   // Delete all components
   VectorPtr<SimComponent *>&componentList = lockComponentList();
   while(componentList.size() > 0)
   {
      SimComponent *c = componentList[0];
      componentList.erase( componentList.begin() );

      if( c->isProperlyAdded() )
         c->deleteObject();
      else if( !c->isRemoved() && !c->isDeleted() )
         delete c;
      // else, something else is deleting this, don't mess with it
   }
   unlockComponentList();

   Parent::onRemove();
}

//////////////////////////////////////////////////////////////////////////

bool SimComponent::processArguments(S32 argc, const char **argv)
{
   for(S32 i = 0; i < argc; i++)
   {
      SimComponent *obj = dynamic_cast<SimComponent*> (Sim::findObject(argv[i]) );
      if(obj)
         addComponent(obj);
      else
         Con::printf("SimComponent::processArguments - Invalid Component Object \"%s\"", argv[i]);
   }
   return true;
}

//////////////////////////////////////////////////////////////////////////

void SimComponent::initPersistFields()
{
   addProtectedField( "Enabled", TypeBool, Offset(mEnabled, SimComponent), &setEnabled, &defaultProtectedGetFn, &writeEnabled, "" );

   Parent::initPersistFields();
} 

// Add Component to this one
bool SimComponent::addComponent( SimComponent *component )
{
   AssertFatal( dynamic_cast<SimObject*>(component), "SimComponent - Cannot add non SimObject derived components!" );

   MutexHandle mh;
   if( mh.lock( mMutex, true ) )
   {
      for( SimComponentIterator nItr = mComponentList.begin(); nItr != mComponentList.end(); nItr++ )
      {
         SimComponent *pComponent = dynamic_cast<SimComponent*>(*nItr);
         AssertFatal( pComponent, "SimComponent::addComponent - NULL component in list!" );
         if( pComponent == component )
            return true;
      }

      if(component->onComponentAdd(this))
      {
         component->mOwner = this;
         mComponentList.push_back( component );
         return true;
      }
   }

   return false;
}

// Remove Component from this one
bool SimComponent::removeComponent( SimComponent *component )
{
   MutexHandle mh;
   if( mh.lock( mMutex, true ) )
   {
      for( SimComponentIterator nItr = mComponentList.begin(); nItr != mComponentList.end(); nItr++ )
      {
         SimComponent *pComponent = dynamic_cast<SimComponent*>(*nItr);
         AssertFatal( pComponent, "SimComponent::removeComponent - NULL component in list!" );
         if( pComponent == component )
         {
            AssertFatal( component->mOwner == this, "Somehow we contain a component who doesn't think we are it's owner." );
            (*nItr)->onComponentRemove(this);
            component->mOwner = NULL;
            mComponentList.erase( nItr );
            return true;
         }
      }
   }
   return false;
}

//////////////////////////////////////////////////////////////////////////

bool SimComponent::onComponentAdd(SimComponent *target)
{
   Con::executef(this, 2, "onComponentAdd", Con::getIntArg(target->getId()));
   return true;
}

void SimComponent::onComponentRemove(SimComponent *target)
{
   Con::executef(this, 2, "onComponentRemove", Con::getIntArg(target->getId()));
}

//////////////////////////////////////////////////////////////////////////

bool SimComponent::writeField(StringTableEntry fieldname, const char* value)
{
   if (!Parent::writeField(fieldname, value))
      return false;

   if( fieldname == StringTable->insert("owner") )
      return false;

   return true;
}

void SimComponent::write(Stream &stream, U32 tabStop, U32 flags /* = 0 */)
{
#if 1
    Parent::write( stream, tabStop, flags );
#else
   MutexHandle handle;
   handle.lock(mMutex); // When this goes out of scope, it will unlock it

   // export selected only?
   if((flags & SelectedOnly) && !isSelected())
   {
      for(U32 i = 0; i < mComponentList.size(); i++)
         mComponentList[i]->write(stream, tabStop, flags);

      return;
   }

   stream.writeTabs(tabStop);
   char buffer[1024];
   dSprintf(buffer, sizeof(buffer), "new %s(%s) {\r\n", getClassName(), getName() ? getName() : "");
   stream.write(dStrlen(buffer), buffer);
   writeFields(stream, tabStop + 1);

   if(mComponentList.size())
   {
      stream.write(2, "\r\n");

      stream.writeTabs(tabStop+1);
      stream.writeLine((U8 *)"// Note: This is a list of behaviors, not arbitrary SimObjects as in a SimGroup or SimSet.\r\n");

      for(U32 i = 0; i < mComponentList.size(); i++)
         mComponentList[i]->write(stream, tabStop + 1, flags);
   }

   stream.writeTabs(tabStop);
   stream.write(4, "};\r\n");
#endif
}

//////////////////////////////////////////////////////////////////////////

bool SimComponent::callMethodOnComponents( U32 argc, const char* argv[], const char** result )
{
   const char *cbName = StringTable->insert(argv[0]);

   if (isEnabled())
   {
      if(isMethod(cbName))
      {
         // This component can handle the given method
         *result = Con::execute( this, argc, argv, true );
         return true;
      }
      else if( getComponentCount() > 0 )
      {
         // Need to try the component's children
         bool handled = false;
         VectorPtr<SimComponent *>&componentList = lockComponentList();
         for( SimComponentIterator nItr = (componentList.end()-1);  nItr >= componentList.begin(); nItr-- )
         {
            argv[0] = cbName;

            SimComponent *pComponent = (*nItr);
            AssertFatal( pComponent, "SimComponent::callMethodOnComponents - NULL component in list!" );

            // Call on children
            handled = pComponent->callMethodOnComponents( argc, argv, result );
            if (handled)
               break;
         }

         unlockComponentList();

         if (handled)
            return true;
      }
   }

   return false;
}
