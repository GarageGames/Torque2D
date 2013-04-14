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

#ifndef _UNDO_H_
#define _UNDO_H_

#include "sim/simBase.h"
#include "collection/vector.h"

class UndoManager;

//-----------------------------------------------------------------------------
class UndoAction : public SimObject
{
private:
    // Other actions necessary to support this action of which the user need not be aware	[KNM | 08/10/11 | ITGB-152]
    Vector<UndoAction*> mQuietSubActions;
protected:
   // The manager this was added to.
   UndoManager* mUndoManager;

public:
   /// A brief description of the action, for display in menus and the like.
   // not private because we're exposing it to the console.
   StringTableEntry mActionName;

   // Required in all ConsoleObject subclasses.
   typedef SimObject Parent;
   DECLARE_CONOBJECT(UndoAction);
   static void initPersistFields();
   
   /// Create a new action, asigning it a name for display in menus et cetera.
   UndoAction( const UTF8* actionName = " ");

   // Modified to clean up quiet sub actions [KNM | 08/10/11 | ITGB-152]
   virtual ~UndoAction();

   /// Implement these methods to perform your specific undo & redo tasks. 
   // Implemented to trickle down into quiet sub actions [KNM | 08/10/11 | ITGB-152]
   virtual void undo();
   virtual void redo();

   // Adds a "quiet (hidden from user)" sub action [KNM | 08/10/11 | ITGB-152]
   void addQuietSubAction(UndoAction * quietSubAction);
   
   /// Adds the action to the undo stack of the default UndoManager, or the provided manager.
   void addToManager(UndoManager* theMan = NULL);
};

//-----------------------------------------------------------------------------
class UndoManager : public SimObject
{
private:
   /// Default number of undo & redo levels.
   const static U32 kDefaultNumLevels = 100;

   /// The stacks of undo & redo actions. They will be capped at size mNumLevels.
   Vector<UndoAction*> mUndoStack;
   Vector<UndoAction*> mRedoStack;
   
   /// Deletes all the UndoActions in a stack, then clears it.
   void clearStack(Vector<UndoAction*> &stack);
   /// Clamps a Vector to mNumLevels entries.
   void clampStack(Vector<UndoAction*> &stack);
   
public:
   /// Number of undo & redo levels.
   // not private because we're exposing it to the console.
   U32 mNumLevels;

   // Required in all ConsoleObject subclasses.
   typedef SimObject Parent;
   DECLARE_CONOBJECT(UndoManager);
   static void initPersistFields();

   /// Constructor. If levels = 0, we use the default number of undo levels.
   UndoManager(U32 levels = kDefaultNumLevels);
   /// Destructor. deletes and clears the undo & redo stacks.
   ~UndoManager();
   /// Accessor to the default undo manager singleton. Creates one if needed.
   static UndoManager& getDefaultManager();
   
   /// Undo last action, and put it on the redo stack.
   void undo();
   /// Redo the last action, and put it on the undo stack.
   void redo();
   
   /// Clears the undo and redo stacks.
   void clearAll();

   /// Returns the printable name of the top actions on the undo & redo stacks.
   StringTableEntry getNextUndoName();
   StringTableEntry getNextRedoName();
   
   S32 getUndoCount();
   S32 getRedoCount();
   StringTableEntry getUndoName(S32 index);
   StringTableEntry getRedoName(S32 index);

   /// Add an action to the top of the undo stack, and clear the redo stack.
   void addAction(UndoAction* action);
   void removeAction(UndoAction* action);
};

// Script Undo Action Creation
// 
// Undo actions can be created in script like this:
// 
// ...
// %undo = new UndoScriptAction() { class = SampleUndo; actionName = "Sample Undo"; };
// %undo.addToManager(UndoManager);
// ...
// 
// function SampleUndo::undo()
// {
//    ...
// }
// 
// function SampleUndo::redo()
// {
//    ...
// }
// 
class UndoScriptAction : public UndoAction
{
public:
   typedef UndoAction Parent;

   UndoScriptAction() : UndoAction()
   {
   };

   virtual void undo() { Con::executef(this, 1, "undo"); };
   virtual void redo() { Con::executef(this, 1, "redo"); }

   virtual bool onAdd()
   {
      // Let Parent Do Work.
      if(!Parent::onAdd())
         return false;

      // Return Success.
      return true;
   };

   virtual void onRemove()
   {
      if (mUndoManager)
         mUndoManager->removeAction((UndoAction*)this);

      Parent::onRemove();
   }

   DECLARE_CONOBJECT(UndoScriptAction);
};

#endif
