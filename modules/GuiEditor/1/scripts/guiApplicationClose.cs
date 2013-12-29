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

// Subscribe to application close event
if (isObject(GuiEditorGui) )
{
    Input::GetEventManager().subscribe(GuiEditor, "ClosePressed");   
    Input::GetEventManager().subscribe(GuiEditor, "BeginShutdown");
}


// Called when the application is beginning to shut down - Before onExit is called
function GuiEditor::onBeginShutdown(%this)
{      
    if (isObject(GuiEditorContent) )
    {
        %editObject = GuiEditorContent.getObject(0);
        %undoMgr =  %this.getUndoManager();
        %undoCount = %undoMgr.getUndoCount();
        %redoCount = %undoMgr.getRedoCount();

        // Prompt to save changes
        if (isObject(%editObject) && (%undoCount > 0 || %redoCount > 0) && $GuiDirty)
        {
            Canvas.setContent(GuiEditorGui);
            Canvas.repaint();

            %file = %editObject.getScriptFile();
            %file = fileName(%file);
            
            %mbResult = checkSaveChanges(%file , false);
            
            if (%mbResult $= $MROk)
                GuiEditorSaveGui();
        }
        
        // Prevent recursion when we shutdown
        %undoMgr.clearAll();      
    }
}

// Called when the application is beginning to shut down - Before onExit is called
function GuiEditor::onClosePressed(%this)
{   
    if (!isObject(GuiEditorContent) )
        return true;

    %editObject = GuiEditorContent.getObject(0);
    %undoMgr =  %this.getUndoManager();

    // Prompt to save changes
    if (isObject(%editObject) && (%undoMgr.getUndoCount() > 0 || %undoMgr.getRedoCount() > 0) && $GuiDirty)
    {
        Canvas.setContent(GuiEditorGui);
        Canvas.repaint();

        %file = %editObject.getScriptFile();
        
        if (%file $= "")
            %file = "Untitled.gui.taml";
            
        %file = fileName(%file);
        %mbResult = checkSaveChanges(%file , true);

        // Don't Quit
        if (%mbResult $= $MRCancel) 
            return false;
        else if (%mbResult $= $MROk)
            GuiEditorSaveGui();
    }
    
    // Prevent recursion when we shutdown
    %undoMgr.clearAll();      

    // Ok to quit
    return true;
}


// 
function checkSaveChanges(%documentName, %cancelOption)
{
    %msg = "Do you want to save changes to document " @ %documentName @ "?";

    if (%cancelOption == true) 
        %buttons = "SaveDontSaveCancel";
    else
        %buttons = "SaveDontSave";

    return MessageBox("Torque Game Builder", %msg, %buttons, "Question");   
}
