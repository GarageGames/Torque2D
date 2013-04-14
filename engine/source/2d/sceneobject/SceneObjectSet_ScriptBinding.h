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

ConsoleMethod(SceneObjectSet, dumpObjects, void, 2, 2, "() Dumps the object data within the set\n"
              "@return No return value")
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

ConsoleMethod(SceneObjectSet, add, void, 3, 0, "(obj1,...) Adds given list of objects to the SceneObjectSet.\n"
              "@param obj_i (i is unilimited) Variable list of objects to add\n"
              "@return No return value")
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

ConsoleMethod(SceneObjectSet, remove, void, 3, 0, "(obj1,...) Removes given listy of objects from the SceneObjectSet.\n"
              "@param obj_i (i is unilimited) Variable list of objects to remove\n"
              "@return No return value")
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

ConsoleMethod(SceneObjectSet, deleteObjects, void, 2, 2,    "() Deletes all the objects in the SceneObjectSet.\n"
                                                    "@return No return value")
{
    object->deleteObjects();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObjectSet, clear, void, 2, 2, "() Clears the SceneObjectSet\n"
              "@return No return value")
{
   object->clear();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObjectSet, callOnChildren, void, 3, 0,
   "( string method, string args... ) Call a method on all objects contained in the set.\n\n"
   "@param method The name of the method to call.\n"
   "@param args The arguments to the method.\n\n"
   "@note This method recurses into all SimSets that are children to the set.\n\n"
   "@see callOnChildrenNoRecurse" )
{
   object->callOnChildren( argv[2], argc - 3, argv + 3 );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObjectSet, reorderChild, void, 4,4," (child1, child2) Uses SceneObjectSet reorder to push child 1 before child 2 - both must already be child controls of this control\n"
              "@param child1 The child you wish to set first\n"
              "@param child2 The child you wish to set after child1\n"
              "@return No return value.")
{
   SceneObject* pObject = Sim::findObject<SceneObject>(argv[2]);
   SceneObject* pTarget	 = Sim::findObject<SceneObject>(argv[3]);

   if(pObject && pTarget)
   {
      object->reOrder(pObject,pTarget);
   }
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObjectSet, getCount, S32, 2, 2, "() @return Returns the number of objects in the SceneObjectSet")
{
   return object->size();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObjectSet, getObject, S32, 3, 3, "(objIndex) @return Returns the ID of the desired object or -1 on failure")
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

ConsoleMethod(SceneObjectSet, isMember, bool, 3, 3, "(object) @return Returns true if specified object is a member of the set, and false otherwise")
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

ConsoleMethod( SceneObjectSet, findObjectByInternalName, S32, 3, 4, "(string name, [bool searchChildren]) Returns the object with given internal name\n"
              "@param name The internal name of the object you wish to find\n"
              "@param searchChildren Set this true if you wish to search all children as well.\n"
              "@return Returns the ID of the object.")
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

ConsoleMethod(SceneObjectSet, bringToFront, void, 3, 3, "(object) Brings object to front of set.\n"
              "@return No return value.")
{
   SceneObject *obj = Sim::findObject<SceneObject>(argv[2]);
   if(!obj)
      return;
   object->bringObjectToFront(obj);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObjectSet, pushToBack, void, 3, 3, "(object) Sends item to back of set.\n"
              "@return No return value.")
{
   SceneObject *obj = Sim::findObject<SceneObject>(argv[2]);
   if(!obj)
      return;
   object->pushObjectToBack(obj);
}
