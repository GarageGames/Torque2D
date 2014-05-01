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

ConsoleMethodGroupBeginWithDocs(UndoManager, SimObject)

/*!  @return Returns the number of redo Actions stored as an integer
*/
ConsoleMethodWithDocs(UndoManager, getRedoCount, ConsoleInt, 2, 2, ())
{
   return object->getRedoCount();
}


/*! Clears the undo manager.
    @return No Return Value
*/
ConsoleMethodWithDocs(UndoManager, clearAll, ConsoleVoid, 2, 2, ())
{
   object->clearAll();
}

/*!  @return Returns the number of UndoActions stored as an integer
*/
ConsoleMethodWithDocs(UndoManager, getUndoCount, ConsoleInt, 2, 2, ())
{
   return object->getUndoCount();
}

/*! Gets the name of the UndoAction at given index. 
    @param index An integer index value for the desired undo
    @return The name as a string
*/
ConsoleMethodWithDocs(UndoManager, getUndoName, ConsoleString, 3, 3, ( S32 index ))
{
   return object->getUndoName(dAtoi(argv[2]));
}

/*! Gets the name of the Action at given index. 
    @param index An integer index value for the desired redo
    @return The name as a string
*/
ConsoleMethodWithDocs(UndoManager, getRedoName, ConsoleString, 3, 3, ( S32 index ))
{
   return object->getRedoName(dAtoi(argv[2]));
}

/*! Adds an UndoAction to the manager
    @param undoManager The manager to add the object to (default NULL)
    @return No Return Value
*/
ConsoleMethodWithDocs(UndoAction, addToManager, ConsoleVoid, 2, 3, ([undoManager]?))
{
   UndoManager *theMan = NULL;
   if(argc == 3)
   {
      SimObject *obj = Sim::findObject(argv[2]);
      if(obj)
         theMan = dynamic_cast<UndoManager*> (obj);
   }
   object->addToManager(theMan);
}

/*! Pops the top undo action off the stack, resolves it, 
    and then pushes it onto the redo stack
*/
ConsoleMethodWithDocs(UndoManager, undo, ConsoleVoid, 2, 2, ())
{
   object->undo();
}

/*! Pops the top redo action off the stack, resolves it, 
    and then pushes it onto the undo stack
*/
ConsoleMethodWithDocs(UndoManager, redo, ConsoleVoid, 2, 2, ())
{
   object->redo();
}

/*! Gets the name of the action at the top of the undo stack
    @return The name of the top action on the undo stack
*/
ConsoleMethodWithDocs(UndoManager, getNextUndoName, ConsoleString, 2, 2, ())
{
   StringTableEntry name = object->getNextUndoName();
   if(!name)
      return NULL;
   char *ret = Con::getReturnBuffer(dStrlen(name) + 1);
   dStrcpy(ret, name);
   return ret;
}

/*! Gets the name of the action at the top of the undo stack
    @return The name of the top action on the redo stack
*/
ConsoleMethodWithDocs(UndoManager, getNextRedoName, ConsoleString, 2, 2, ())
{
   StringTableEntry name = object->getNextRedoName();
   if(!name)
      return NULL;
   char *ret = Con::getReturnBuffer(dStrlen(name) + 1);
   dStrcpy(ret, name);
   return ret;
}

ConsoleMethodGroupEndWithDocs(UndoManager)
