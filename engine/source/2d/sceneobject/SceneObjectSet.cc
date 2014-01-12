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
#include "string/stringTable.h"
#include "console/console.h"
#include "io/fileStream.h"
#include "input/actionMap.h"
#include "io/resource/resourceManager.h"
#include "io/fileObject.h"
#include "console/consoleInternal.h"
#include "debug/profiler.h"
#include "console/ConsoleTypeValidators.h"
#include "memory/frameAllocator.h"

#include "2d/sceneobject/SceneObject.h"
#include "2d/sceneobject/SceneObjectSet.h"

// Script bindings.
#include "2d/sceneobject/SceneObjectSet_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SceneObjectSet);

//-----------------------------------------------------------------------------

void SceneObjectSet::addObject(SceneObject* obj)
{
	if ( obj == NULL )
		return;

   mObjectList.pushBack(obj);
   deleteNotify((SimObject*)obj);
}

void SceneObjectSet::removeObject(SceneObject* obj)
{
	if ( obj == NULL )
		return;

   mObjectList.remove(obj);
   clearNotify((SimObject*)obj);
}

void SceneObjectSet::pushObject(SceneObject* pObj)
{
   mObjectList.pushBackForce(pObj);
   deleteNotify((SimObject*)pObj);
}

void SceneObjectSet::popObject()
{
   MutexHandle handle;

   if (mObjectList.size() == 0) 
   {
      AssertWarn(false, "Stack underflow in SceneObjectSet::popObject");
      return;
   }

   SceneObject* pObject = mObjectList[mObjectList.size() - 1];

   mObjectList.removeStable(pObject);
   clearNotify((SimObject*)pObject);
}

//-----------------------------------------------------------------------------

void SceneObjectSet::addTamlChild( SimObject* pSimObject )
{
    // Sanity!
    AssertFatal( pSimObject != NULL, "SceneObjectSet::addTamlChild() - Cannot add a NULL child object." );

    addObject( dynamic_cast<SceneObject*>(pSimObject) );
}

//-----------------------------------------------------------------------------

void SceneObjectSet::callOnChildren( const char * method, S32 argc, const char *argv[], bool executeOnChildGroups )
{
   // Prep the arguments for the console exec...
   // Make sure and leave args[1] empty.
   const char* args[21];
   args[0] = method;
   for (S32 i = 0; i < argc; i++)
      args[i + 2] = argv[i];

   for( iterator i = begin(); i != end(); i++ )
   {
      SceneObject *childObj = static_cast<SceneObject*>(*i);
	  if ( childObj == NULL )
		  continue;

      if( childObj->isMethod( method ) )
         Con::execute(childObj, argc + 2, args);

      if( executeOnChildGroups )
      {
         SceneObjectSet* childSet = dynamic_cast<SceneObjectSet*>(*i);
         if ( childSet )
            childSet->callOnChildren( method, argc, argv, executeOnChildGroups );
      }
   }
}

//-----------------------------------------------------------------------------

bool SceneObjectSet::reOrder( SceneObject *obj, SceneObject *target )
{
   iterator itrS, itrD;
   if ( (itrS = find(begin(),end(),obj)) == end() )
   {
      return false;  // object must be in list
   }

   if ( obj == target )
   {
      return true;   // don't reorder same object but don't indicate error
   }

   if ( !target )    // if no target, then put to back of list
   {
      if ( itrS != (end()-1) )      // don't move if already last object
      {
         mObjectList.erase(itrS);    // remove object from its current location
         mObjectList.push_back(obj); // push it to the back of the list
      }
   }
   else              // if target, insert object in front of target
   {
      if ( (itrD = find(begin(),end(),target)) == end() )
         return false;              // target must be in list

      mObjectList.erase(itrS);

      //Tinman - once itrS has been erased, itrD won't be pointing at the same place anymore - re-find...
      itrD = find(begin(),end(),target);
      mObjectList.insert(itrD,obj);
   }

   return true;
}   

//-----------------------------------------------------------------------------

void SceneObjectSet::onDeleteNotify(SimObject *object)
{
   removeObject(dynamic_cast<SceneObject*>(object));
   Parent::onDeleteNotify(object);
}

void SceneObjectSet::onRemove()
{
   mObjectList.sortId();
   if (mObjectList.size())
   {
      // This backwards iterator loop doesn't work if the
      // list is empty, check the size first.
      for (SceneObjectList::iterator ptr = mObjectList.end() - 1;
         ptr >= mObjectList.begin(); ptr--)
      {
         clearNotify(*ptr);
      }
   }

   Parent::onRemove();
}

//-----------------------------------------------------------------------------

void SceneObjectSet::deleteObjects( void )
{
        while(size() > 0 )
        {
            mObjectList[0]->deleteObject();
        }
}

//-----------------------------------------------------------------------------

void SceneObjectSet::clear()
{
   while (size() > 0)
      removeObject(mObjectList.last());
}
//-----------------------------------------------------------------------------

SimObject *SceneObjectSet::findObject(const char *namePath)
{
   // find the end of the object name
   S32 len;
   for(len = 0; namePath[len] != 0 && namePath[len] != '/'; len++)
      ;

   StringTableEntry stName = StringTable->lookupn(namePath, len);
   if(!stName)
      return NULL;

   for(SceneObjectSet::iterator i = begin(); i != end(); i++)
   {
      if((*i)->getName() == stName)
      {
         if(namePath[len] == 0)
            return *i;
         return (*i)->findObject(namePath + len + 1);
      }
   }
   return NULL;
}

//-----------------------------------------------------------------------------

SceneObject* SceneObjectSet::findObjectByInternalName(const char* internalName, bool searchChildren)
{
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      SceneObject *childObj = static_cast<SceneObject*>(*i);
      if(childObj->getInternalName() == internalName)
         return childObj;
      else if (searchChildren)
      {
         SceneObjectSet* childSet = dynamic_cast<SceneObjectSet*>(*i);
         if (childSet)
         {
            SceneObject* found = childSet->findObjectByInternalName(internalName, searchChildren);
            if (found) return found;
         }
      }
   }

   return NULL;
}

//-----------------------------------------------------------------------------

inline void SceneObjectSetIterator::Stack::push_back(SceneObjectSet* set)
{
   increment();
   last().set = set;
   last().itr = set->begin();
}

//-----------------------------------------------------------------------------

SceneObjectSetIterator::SceneObjectSetIterator(SceneObjectSet* set)
{
   VECTOR_SET_ASSOCIATION(stack);

   if (!set->empty())
      stack.push_back(set);
}

//-----------------------------------------------------------------------------

SceneObject* SceneObjectSetIterator::operator++()
{
   SceneObjectSet* set;
   if ((set = dynamic_cast<SceneObjectSet*>(*stack.last().itr)) != 0) 
   {
      if (!set->empty()) 
      {
         stack.push_back(set);
         return *stack.last().itr;
      }
   }

   while (++stack.last().itr == stack.last().set->end()) 
   {
      stack.pop_back();
      if (stack.empty())
         return 0;
   }
   return *stack.last().itr;
}	
