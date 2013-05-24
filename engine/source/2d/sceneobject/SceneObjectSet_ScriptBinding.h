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

ConsoleMethodGroupBeginWithDocs(SceneObjectSet, SimObject)

/*! Dumps the object data within the set
    @return No return value
*/
ConsoleMethodWithDocs(SceneObjectSet, dumpObjects, ConsoleVoid, 2, 2, ())
{
   SceneObjectSet::iterator itr;
   for(itr = object->begin(); itr != object->end(); itr++)
   {
      SceneObject *obj = *itr;
      bool isSet = dynamic_cast<SceneObjectSet *>(obj) != 0;
      const char *name = obj->getName();
      if(name)
         Con::printf("   %d,\"%s\": %s %s", obj->getId(), name,
         obj->getClassName(), isSet ? "(g)":"");
      else
         Con::printf("   %d: %s %s", obj->getId(), obj->getClassName(),
         isSet ? "(g)" : "");
   }
}

/*! Adds given list of objects to the SceneObjectSet.
    @param obj_i (i is unilimited) Variable list of objects to add
    @return No return value
*/
ConsoleMethodWithDocs(SceneObjectSet, add, ConsoleVoid, 3, 0, (obj1,...))
{
   for(S32 i = 2; i < argc; i++)
   {
      SceneObject *obj = Sim::findObject<SceneObject>(argv[i]);
      if(obj)
         object->addObject(obj);
      else
         Con::printf("Set::add: Object \"%s\" doesn't exist", argv[i]);
   }
}

/*! Removes given listy of objects from the SceneObjectSet.
    @param obj_i (i is unilimited) Variable list of objects to remove
    @return No return value
*/
ConsoleMethodWithDocs(SceneObjectSet, remove, ConsoleVoid, 3, 0, (obj1,...))
{
   for(S32 i = 2; i < argc; i++)
   {
      SceneObject *obj = Sim::findObject<SceneObject>(argv[i]);
      if(obj && object->find(object->begin(),object->end(),obj) != object->end())
         object->removeObject(obj);
      else
         Con::printf("Set::remove: Object \"%s\" does not exist in set", argv[i]);
   }
}

//-----------------------------------------------------------------------------

/*! Deletes all the objects in the SceneObjectSet.
    @return No return value
*/
ConsoleMethodWithDocs(SceneObjectSet, deleteObjects, ConsoleVoid, 2, 2, ())
{
    object->deleteObjects();
}

//-----------------------------------------------------------------------------

/*! Clears the SceneObjectSet
    @return No return value
*/
ConsoleMethodWithDocs(SceneObjectSet, clear, ConsoleVoid, 2, 2, ())
{
   object->clear();
}

//-----------------------------------------------------------------------------

/*! Call a method on all objects contained in the set.
   @param method The name of the method to call.
   @param args The arguments to the method.
   @note This method recurses into all SimSets that are children to the set.
   @see callOnChildrenNoRecurse
*/
ConsoleMethodWithDocs( SceneObjectSet, callOnChildren, ConsoleVoid, 3, 0, ( string method, string args... ))
{
   object->callOnChildren( argv[2], argc - 3, argv + 3 );
}

//-----------------------------------------------------------------------------

/*! Uses SceneObjectSet reorder to push child 1 before child 2 - both must already be child controls of this control
    @param child1 The child you wish to set first
    @param child2 The child you wish to set after child1
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObjectSet, reorderChild, ConsoleVoid, 4,4,  (child1, child2))
{
   SceneObject* pObject = Sim::findObject<SceneObject>(argv[2]);
   SceneObject* pTarget	 = Sim::findObject<SceneObject>(argv[3]);

   if(pObject && pTarget)
   {
      object->reOrder(pObject,pTarget);
   }
}

//-----------------------------------------------------------------------------

/*! @return Returns the number of objects in the SceneObjectSet
*/
ConsoleMethodWithDocs(SceneObjectSet, getCount, ConsoleInt, 2, 2, ())
{
   return object->size();
}

//-----------------------------------------------------------------------------

/*! @return Returns the ID of the desired object or -1 on failure
*/
ConsoleMethodWithDocs(SceneObjectSet, getObject, ConsoleInt, 3, 3, (objIndex))
{
   S32 objectIndex = dAtoi(argv[2]);
   if(objectIndex < 0 || objectIndex >= S32(object->size()))
   {
      Con::printf("Set::getObject index out of range.");
      return -1;
   }
   return ((*object)[objectIndex])->getId();
}

//-----------------------------------------------------------------------------

/*! @return Returns true if specified object is a member of the set, and false otherwise
*/
ConsoleMethodWithDocs(SceneObjectSet, isMember, ConsoleBool, 3, 3, (object))
{
   SceneObject *testObject = Sim::findObject<SceneObject>(argv[2]);
   if(!testObject)
   {
      Con::printf("SceneObjectSet::isMember: %s is not an object.", argv[2]);
      return false;
   }

   for(SceneObjectSet::iterator i = object->begin(); i != object->end(); i++)
   {
      if(*i == testObject)
      {
         return true;
      }
   }

   return false;
}

//-----------------------------------------------------------------------------

/*! Returns the object with given internal name
    @param name The internal name of the object you wish to find
    @param searchChildren Set this true if you wish to search all children as well.
    @return Returns the ID of the object.
*/
ConsoleMethodWithDocs( SceneObjectSet, findObjectByInternalName, ConsoleInt, 3, 4, (string name, [bool searchChildren]))
{

   StringTableEntry pcName = StringTable->insert(argv[2]);
   bool searchChildren = false;
   if (argc > 3)
      searchChildren = dAtob(argv[3]);

   SceneObject* child = object->findObjectByInternalName(pcName, searchChildren);
   if(child)
      return child->getId();
   return 0;
}

//-----------------------------------------------------------------------------

/*! Brings object to front of set.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObjectSet, bringToFront, ConsoleVoid, 3, 3, (object))
{
   SceneObject *obj = Sim::findObject<SceneObject>(argv[2]);
   if(!obj)
      return;
   object->bringObjectToFront(obj);
}

//-----------------------------------------------------------------------------

/*! Sends item to back of set.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObjectSet, pushToBack, ConsoleVoid, 3, 3, (object))
{
   SceneObject *obj = Sim::findObject<SceneObject>(argv[2]);
   if(!obj)
      return;
   object->pushObjectToBack(obj);
}

ConsoleMethodGroupEndWithDocs(SceneObjectSet)
