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

#include "undo_ScriptBinding.h"

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

S32 UndoManager::getRedoCount()
{
   return mRedoStack.size();
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

S32 UndoManager::getUndoCount()
{
   return mUndoStack.size();
}

StringTableEntry UndoManager::getUndoName(S32 index)
{
   if ((index < getUndoCount()) && (index >= 0))
      return mUndoStack[index]->mActionName;

   return NULL;
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
