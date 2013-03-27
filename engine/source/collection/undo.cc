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

#include "undo.h"

#include "console/console.h"
#include "console/consoleTypes.h"

//-----------------------------------------------------------------------------
// UndoAction
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(UndoAction);
IMPLEMENT_CONOBJECT(UndoScriptAction);

UndoAction::UndoAction( const UTF8* actionName)
{
   mActionName = StringTable->insert(actionName);
   mUndoManager = NULL;
}

// Modified to clean up quiet sub actions [KNM | 08/10/11 | ITGB-152]
UndoAction::~UndoAction()
{
   clearAllNotifications();

    for (U32 i = 0; i < (U32)mQuietSubActions.size(); i++)
      delete mQuietSubActions[i];

    mQuietSubActions.clear();
}

//-----------------------------------------------------------------------------
void UndoAction::initPersistFields()
{
   Parent::initPersistFields();
   addField("actionName", TypeString, Offset(mActionName, UndoAction), 
      "A brief description of the action, for UI representation of this undo/redo action.");
}

//-----------------------------------------------------------------------------
// Implemented to trickle down into quiet sub actions [KNM | 08/10/11 | ITGB-152]
void UndoAction::undo(void)
{
   for (U32 i = 0; i < (U32)mQuietSubActions.size(); i++)
      mQuietSubActions[i]->undo();
}

//-----------------------------------------------------------------------------
// Implemented to trickle down into quiet sub actions [KNM | 08/10/11 | ITGB-152]
void UndoAction::redo(void)
{
   for (U32 i = 0; i < (U32)mQuietSubActions.size(); i++)
      mQuietSubActions[i]->redo();
}

//-----------------------------------------------------------------------------
// Adds a "quiet (hidden from user)" sub action [KNM | 08/10/11 | ITGB-152]
void UndoAction::addQuietSubAction(UndoAction * quietSubAction)
{
   if (!quietSubAction)
      return;

   mQuietSubActions.push_back(quietSubAction);
}

//-----------------------------------------------------------------------------
void UndoAction::addToManager(UndoManager* theMan)
{
   if(theMan)
   {
      mUndoManager = theMan;
      (*theMan).addAction(this);
   }
   else
   {
      mUndoManager = &UndoManager::getDefaultManager();
      mUndoManager->addAction(this);
   }
}

//-----------------------------------------------------------------------------
// UndoManager
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(UndoManager);

UndoManager::UndoManager(U32 levels)
{
   mNumLevels = levels;
   // levels can be arbitrarily high, so we don't really want to reserve(levels).
   mUndoStack.reserve(10);
   mRedoStack.reserve(10);
}

//-----------------------------------------------------------------------------
UndoManager::~UndoManager()
{
   clearStack(mUndoStack);
   clearStack(mRedoStack);
}

//-----------------------------------------------------------------------------
void UndoManager::initPersistFields()
{
   addField("numLevels", TypeS32, Offset(mNumLevels, UndoManager), "Number of undo & redo levels.");
   // arrange for the default undo manager to exist.
//   UndoManager &def = getDefaultManager();
//   Con::printf("def = %s undo manager created", def.getName());
   
}

//-----------------------------------------------------------------------------
UndoManager& UndoManager::getDefaultManager()
{
   // the default manager is created the first time it is asked for.
   static UndoManager *defaultMan = NULL;
   if(!defaultMan)
   {
      defaultMan = new UndoManager();
      defaultMan->assignName("DefaultUndoManager");
      defaultMan->registerObject();
   }
   return *defaultMan;
}

ConsoleMethod(UndoManager, clearAll, void, 2, 2, "Clears the undo manager."
              "@return No Return Value")
{
   object->clearAll();
}

void UndoManager::clearAll()
{
   clearStack(mUndoStack); clearStack(mRedoStack);
   Con::executef(this, 1, "onClear");
}

//-----------------------------------------------------------------------------
void UndoManager::clearStack(Vector<UndoAction*> &stack)
{
   Vector<UndoAction*>::iterator itr = stack.begin();
   while (itr != stack.end())
   {
      UndoAction* undo = stack.first();
      stack.pop_front();
      // Don't delete script created undos.
      if (dynamic_cast<UndoScriptAction*>(undo))
         undo->deleteObject();
      else
         delete undo;
   }
   stack.clear();
}

//-----------------------------------------------------------------------------
void UndoManager::clampStack(Vector<UndoAction*> &stack)
{
   while((U32)stack.size() > mNumLevels)
   {
      UndoAction *act = stack.front();
      stack.pop_front();

      UndoScriptAction* scriptAction = dynamic_cast<UndoScriptAction*>(act);
      if (scriptAction)
         scriptAction->deleteObject();
      else
         delete act;
   }
}

void UndoManager::removeAction(UndoAction *action)
{
   Vector<UndoAction*>::iterator itr = mUndoStack.begin();
   while (itr != mUndoStack.end())
   {
      if ((*itr) == action)
      {
         UndoAction* deleteAction = *itr;
         mUndoStack.erase(itr);
         if (!dynamic_cast<UndoScriptAction*>(deleteAction))
            delete deleteAction;
         Con::executef(this, 1, "onRemoveUndo");
         return;
      }
      itr++;
   }

   itr = mRedoStack.begin();
   while (itr != mRedoStack.end())
   {
      if ((*itr) == action)
      {
         UndoAction* deleteAction = *itr;
         mRedoStack.erase(itr);
         if (!dynamic_cast<UndoScriptAction*>(deleteAction))
            delete deleteAction;
         Con::executef(this, 1, "onRemoveUndo");
         return;
      }
      itr++;
   }
}

//-----------------------------------------------------------------------------
void UndoManager::undo()
{
   // make sure we have an action available
   if(mUndoStack.size() < 1)
      return;

   // pop the action off the undo stack
   UndoAction *act = mUndoStack.last();
   mUndoStack.pop_back();
   
   // add it to the redo stack
   mRedoStack.push_back(act);
   if((U32)mRedoStack.size() > mNumLevels)
      mRedoStack.pop_front();
   
   Con::executef(this, 1, "onUndo");

   // perform the undo, whatever it may be.
   (*act).undo();
}

//-----------------------------------------------------------------------------
void UndoManager::redo()
{
   // make sure we have an action available
   if(mRedoStack.size() < 1)
      return;

   // pop the action off the redo stack
   UndoAction *react = mRedoStack.last();
   mRedoStack.pop_back();
   
   // add it to the undo stack
   mUndoStack.push_back(react);
   if((U32)mUndoStack.size() > mNumLevels)
      mUndoStack.pop_front();
   
   Con::executef(this, 1, "onRedo");
   
   // perform the redo, whatever it may be.
   (*react).redo();
}

ConsoleMethod(UndoManager, getUndoCount, S32, 2, 2, "() \n @return Returns the number of UndoActions stored as an integer")
{
   return object->getUndoCount();
}

S32 UndoManager::getUndoCount()
{
   return mUndoStack.size();
}

ConsoleMethod(UndoManager, getUndoName, const char*, 3, 3, "( S32 index ) Gets the name of the UndoAction at given index.\n "
              "@param index An integer index value for the desired undo\n"
              "@return The name as a string")
{
   return object->getUndoName(dAtoi(argv[2]));
}

StringTableEntry UndoManager::getUndoName(S32 index)
{
   if ((index < getUndoCount()) && (index >= 0))
      return mUndoStack[index]->mActionName;

   return NULL;
}

ConsoleMethod(UndoManager, getRedoCount, S32, 2, 2, "() \n @return Returns the number of redo Actions stored as an integer")
{
   return object->getRedoCount();
}

S32 UndoManager::getRedoCount()
{
   return mRedoStack.size();
}

ConsoleMethod(UndoManager, getRedoName, const char*, 3, 3, "( S32 index ) Gets the name of the Action at given index.\n "
              "@param index An integer index value for the desired redo\n"
              "@return The name as a string")
{
   return object->getRedoName(dAtoi(argv[2]));
}

StringTableEntry UndoManager::getRedoName(S32 index)
{
   if ((index < getRedoCount()) && (index >= 0))
      return mRedoStack[getRedoCount() - index - 1]->mActionName;

   return NULL;
}

//-----------------------------------------------------------------------------
StringTableEntry UndoManager::getNextUndoName()
{
   if(mUndoStack.size() < 1)
      return NULL;
      
   UndoAction *act = mUndoStack.last();
   return (*act).mActionName;
}

//-----------------------------------------------------------------------------
StringTableEntry UndoManager::getNextRedoName()
{
   if(mRedoStack.size() < 1)
      return NULL;

   UndoAction *act = mRedoStack.last();
   return (*act).mActionName;
}

//-----------------------------------------------------------------------------
void UndoManager::addAction(UndoAction* action)
{
   // push the incoming action onto the stack, move old data off the end if necessary.
   mUndoStack.push_back(action);
   if((U32)mUndoStack.size() > mNumLevels)
      mUndoStack.pop_front();
   
   Con::executef(this, 1, "onAddUndo");

   // clear the redo stack
   clearStack(mRedoStack);
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoAction, addToManager, void, 2, 3, "action.addToManager([undoManager]) Adds an UndoAction to the manager"
              "@param undoManager The manager to add the object to (default NULL)\n"
              "@return No Return Value")
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

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, undo, void, 2, 2, "UndoManager.undo(); Pops the top undo action off the stack, resolves it, "
                                            "and then pushes it onto the redo stack")
{
   object->undo();
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, redo, void, 2, 2, "UndoManager.redo(); Pops the top redo action off the stack, resolves it, "
                                            "and then pushes it onto the undo stack")
{
   object->redo();
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, getNextUndoName, const char *, 2, 2, "UndoManager.getNextUndoName(); Gets the name of the action at the top of the undo stack\n"
              "@return The name of the top action on the undo stack")
{
   StringTableEntry name = object->getNextUndoName();
   if(!name)
      return NULL;
   char *ret = Con::getReturnBuffer(dStrlen(name) + 1);
   dStrcpy(ret, name);
   return ret;
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, getNextRedoName, const char *, 2, 2, "UndoManager.getNextRedoName(); Gets the name of the action at the top of the undo stack\n"
              "@return The name of the top action on the redo stack")
{
   StringTableEntry name = object->getNextRedoName();
   if(!name)
      return NULL;
   char *ret = Con::getReturnBuffer(dStrlen(name) + 1);
   dStrcpy(ret, name);
   return ret;
}