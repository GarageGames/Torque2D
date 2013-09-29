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

#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "platform/event.h"
#include "console/console.h"

//------------------------------------------------------------------------------
//*** DAW: Cursor Manager Methods
CursorManager* Input::getCursorManager()
{
   return smCursorManager;
}

void CursorManager::pushCursor(S32 cursorID)
{
   //*** Place the new cursor shape onto the stack
   mCursors.increment();
   mCursors.last().mCursorID = cursorID;

   //*** Now actually change the shape
   changeCursorShape(cursorID);
}

void CursorManager::popCursor()
{
   //*** Before poping the stack, make sure we're not trying to remove the last cursor shape
   if(mCursors.size() <= 1)
      return;

   //*** Pop the stack
   mCursors.pop_back();

   //*** Now set the cursor shape
   changeCursorShape(mCursors.last().mCursorID);
}

void CursorManager::refreshCursor()
{
   //*** Refresh the cursor's shape
   changeCursorShape(mCursors.last().mCursorID);
}

void CursorManager::changeCursorShape(S32 cursorID)
{
   if(cursorID >= 0)
      Input::setCursorShape((U32)cursorID);
}

static EnumTable::Enums curManagerShapesEnums[] = 
{
   { CursorManager::curArrow, "Arrow" },
   { CursorManager::curWait, "Wait" },
   { CursorManager::curPlus, "Plus" },
   { CursorManager::curResizeVert, "ResizeVert" },
   { CursorManager::curResizeHorz, "ResizeHorz" },
   { CursorManager::curResizeAll, "ResizeAll" },
   { CursorManager::curIBeam, "ibeam" },
   { CursorManager::curResizeNESW, "ResizeNESW" },
   { CursorManager::curResizeNWSE, "ResizeNWSE" },
};
      
static EnumTable gCurManagerShapesTable(8, &curManagerShapesEnums[0]); 

//------------------------------------------------------------------------------

#include "CursorManager_ScriptBinding.h"
