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

//-----------------------------------------------------------------------------------------
// Undo adding an object
function UndoActionAddObject::create(%set, %trash, %treeView)
{
    %act =  UndoActionAddDelete::create(UndoActionAddObject, %set, %trash, %treeView);
    %act.actionName = "Add Objects";
    
    return %act;
}

function UndoActionAddObject::undo(%this)
{
    %this.trashObjects();
}

function UndoActionAddObject::redo(%this)
{
    %this.restoreObjects();
}


//-----------------------------------------------------------------------------------------
// Undo Deleting an object
function UndoActionDeleteObject::create(%set, %trash, %treeView)
{
    %act = UndoActionAddDelete::create(UndoActionDeleteObject, %set, %trash, %treeView);
    %act.designatedDeleter = true;
    %act.actionName = "Delete Objects";
    
    return %act;
}

function UndoActionDeleteObject::undo(%this)
{
    %this.restoreObjects();
}

function UndoActionDeleteObject::redo(%this)
{
    %this.trashObjects();
}

//-----------------------------------------------------------------------------------------
// Behavior common to Add and Delete UndoActions
function UndoActionAddDelete::create(%class, %set, %trash, %treeView)
{
    // record objects
    // record parents
    // record trash
    // return new subclass %class of UndoActionAddDelete
    %act = new UndoScriptAction() { class = %class; superclass = UndoActionAddDelete; };
    for(%i = 0; %i < %set.getCount(); %i++)
    {
        %obj = %set.getObject(%i);

        %act.object[%i] = %obj.getId();
        %act.parent[%i] = %obj.getParent();
    }
    
    %act.objCount = %set.getCount();
    %act.trash = %trash;
    %act.tree = %treeView;
    
    return %act;
}

function UndoActionAddDelete::trashObjects(%this)
{
    //move objects to trash
    for(%i = 0; %i < %this.objCount; %i++)
    {
        %this.trash.add(%this.object[%i]);
    }
    
    // note that we're responsible for deleting those objects we've moved to the trash.
    %this.designatedDeleter = true;

    // update the tree view
    if (isObject(%this.tree))
        %this.tree.update();
}

function UndoActionAddDelete::restoreObjects(%this)
{
    //move objects to saved parent
    for(%i = 0; %i < %this.objCount; %i++)
    {
        %this.parent[%i].add(%this.object[%i]);
    }
    
    // note that we no longer own the objects, and should not delete them when we're deleted.
    %this.designatedDeleter = false;

    // update the tree view
    if (isObject(%this.tree))
        %this.tree.update();
}

function UndoActionAddObject::onRemove(%this)
{
    // if this undoAction owns objects in the trash, delete them.
    if (! %this.designatedDeleter)
        return;

    for(%i = 0; %i < %this.objCount; %i++)
        %this.object[%i].delete();
}

//------------------------------------------------------------------------------
// Undo Any State Change.
function GenericUndoAction::create()
{
    %act = new UndoScriptAction() { class = GenericUndoAction; };
    %act.actionName = "Edit Objects";

    return %act;
}

function GenericUndoAction::watch(%this, %object)
{
    // make sure we're working with the object id, because it cannot change.
    %object = %object.getId();

    %fieldCount = %object.getFieldCount();
    %dynFieldCount = %object.getDynamicFieldCount();

    // inspect all the fields on the object, including dyanamic ones.
    // record field names and values.
    for(%i = 1; %i <= %fieldCount; %i++)
    {
        %field = %object.getField(%i);
        %this.fieldNames[%object] = %this.fieldNames[%object] SPC %field;
        %this.fieldValues[%object, %field] = %object.getFieldValue(%field);
    }
    for(%i = 1; %i <= %dynFieldCount; %i++)
    {
        %field = %object.getDynamicField(%i);
        %this.fieldNames[%object] = %this.fieldNames[%object] SPC %field;
        %this.fieldValues[%object, %field] = %object.getFieldValue(%field);
    }
    // clean spurious spaces from the field name list
    %this.fieldNames[%object] = trim(%this.fieldNames[%object]);
    
    // record that we know this object
    %this.objectIds[%object] = 1;
    %this.objectIdList = %this.objectIdList SPC %object;
}

function GenericUndoAction::learn(%this, %object)
{
    // make sure we're working with the object id, because it cannot change.
    %object = %object.getId();

    %fieldCount = %object.getFieldCount();
    %dynFieldCount = %object.getDynamicFieldCount();

    // inspect all the fields on the object, including dyanamic ones.
    // record field names and values.
    for(%i = 0; %i < %fieldCount; %i++)
    {
        %field = %object.getField(%i);
        %this.newFieldNames[%object] = %this.newFieldNames[%object] SPC %field;
        %this.newFieldValues[%object, %field] = %object.getFieldValue(%field);
    }
    for(%i = 0; %i < %dynFieldCount; %i++)
    {
        %field = %object.getDynamicField(%i);
        %this.newFieldNames[%object] = %this.newFieldNames[%object] SPC %field;
        %this.newFieldValues[%object, %field] = %object.getFieldValue(%field);
    }
    // trim
    %this.newFieldNames[%object] = trim(%this.newFieldNames[%object]);
   
    // look for differences
    //----------------------------------------------------------------------
    %diffs = false;
    %newFieldNames = %this.newFieldNames[%object];
    %oldFieldNames = %this.fieldNames[%object];
    %numNewFields = getWordCount(%newFieldNames);
    %numOldFields = getWordCount(%numOldFields);
    // compare the old field list to the new field list.
    // if a field is on the old list that isn't on the new list, 
    // add it to the newNullFields list.
    for(%i = 0; %i < %numOldFields; %i++)
    {
        %field = getWord(%oldFieldNames, %i);
        %newVal = %this.newFieldValues[%object, %field];
        %oldVal = %this.fieldValues[%object, %field];
        if (%newVal !$= %oldVal)
        {
            %diffs = true;
            
            if (%newVal $= "")
                %newNullFields = %newNullFields SPC %field;
        }
    }
    
    // scan the new field list
    // add missing fields to the oldNullFields list
    for(%i = 0; %i < %numNewFields; %i++)
    {
        %field = getWord(%newFieldNames, %i);
        %newVal = %this.newFieldValues[%object, %field];
        %oldVal = %this.fieldValues[%object, %field];
        if (%newVal !$= %oldVal)
        {
            %diffs = true;
            
            if (%oldVal $= "")
                %oldNullFields = %oldNullFields SPC %field;
        }
    }
    %this.newNullFields[%object] = trim(%newNullFields);
    %this.oldNullFields[%object] = trim(%oldNullFields);

    return %diffs;
}

function GenericUndoAction::watchSet(%this, %set)
{
    // scan the set
    // this.watch each object.
    %setcount = %set.getCount();
    %i = 0;
    for(; %i < %setcount; %i++)
    {
        %object = %set.getObject(%i);
        %this.watch(%object);
    }
}

function GenericUndoAction::learnSet(%this, %set)
{
    // scan the set
    // this.learn any objects that we have a this.objectIds[] entry for.
    %diffs = false;
    for(%i = 0; %i < %set.getCount(); %i++)
    {
        %object = %set.getObject(%i).getId();
        
        if (%this.objectIds[%object] != 1)
            continue;

        if (%this.learn(%object))
            %diffs = true;
    }
}

function GenericUndoAction::undo(%this)
{
    // set the objects to the old values
    // scan through our objects
    %objectList = %this.objectIdList;
    for(%i = 0; %i < getWordCount(%objectList); %i++)
    {
        %object = getWord(%objectList, %i);
        
        // scan through the old extant fields
        %fieldNames = %this.fieldNames[%object];
        for(%j = 0; %j < getWordCount(%fieldNames); %j++)
        {
            %field = getWord(%fieldNames, %j);
            %object.setFieldValue(%field, %this.fieldValues[%object, %field]);
        }
        
        // null out the fields in the null list
        %fieldNames = %this.oldNullFields[%object];
        for(%j = 0; %j < getWordCount(%fieldNames); %j++)
        {
            %field = getWord(%fieldNames, %j);
            %object.setFieldValue(%field, "");
        }
    }

    // update the tree view
    if (isObject(%this.tree))
        %this.tree.update();
}

function GenericUndoAction::redo(%this)
{
    // set the objects to the new values
    // set the objects to the new values
    // scan through our objects
    %objectList = %this.objectIdList;
    for(%i = 0; %i < getWordCount(%objectList); %i++)
    {
        %object = getWord(%objectList, %i);
        // scan through the new extant fields
        %fieldNames = %this.newFieldNames[%object];
        for(%j = 0; %j < getWordCount(%fieldNames); %j++)
        {
            %field = getWord(%fieldNames, %j);
            %object.setFieldValue(%field, %this.newFieldValues[%object, %field]);
        }
        
        // null out the fields in the null list
        %fieldNames = %this.newNullFields[%object];
        for(%j = 0; %j < getWordCount(%fieldNames); %j++)
        {
            %field = getWord(%fieldNames, %j);
            %object.setFieldValue(%field, "");
        }
    }

    // update the tree view
    if (isObject(%this.tree))
        %this.tree.update();
}

//-----------------------------------------------------------------------------------------
// Gui Editor Undo hooks from code
function GuiEditor::onPreEdit(%this, %selection)
{
    if (isObject(%this.pendingGenericUndoAction))
        error("Error: attempting to create two generic undo actions at once in the same editor!");

    //echo("pre edit");
    %act = GenericUndoAction::create();
    %act.watchSet(%selection);
    %act.tree = GuiEditorTreeView;

    %this.pendingGenericUndoAction = %act;
}

function GuiEditor::onPostEdit(%this, %selection)
{
    if (!isObject(%this.pendingGenericUndoAction))
        error("Error: attempting to complete a GenericUndoAction that hasn't been started!");

    %act = %this.pendingGenericUndoAction;
    %this.pendingGenericUndoAction = "";

    %diffs = %act.learnSet(%selection);
    if (%diffs)
    {
        %act.addToManager(%this.getUndoManager());
        $GuiDirty = true;
    }
    else
    {
        %act.delete();
    }

    %this.updateUndoMenu();
}

function GuiEditor::onPreSelectionNudged(%this, %selection)
{
    %this.onPreEdit(%selection);
    %this.pendingGenericUndoAction.actionName = "Nudge";
}

function GuiEditor::onPostSelectionNudged(%this, %selection)
{
    %this.onPostEdit(%selection);
}

function GuiEditor::onAddNewCtrl(%this, %ctrl)
{
    %set = new SimSet();
    %set.add(%ctrl);
    %act = UndoActionAddObject::create(%set, %this.getTrash(), GuiEditorTreeView);
    %set.delete();
    %act.addToManager(%this.getUndoManager());
    %this.updateUndoMenu();
    $GuiDirty = true;
}

function GuiEditor::onAddNewCtrlSet(%this, %selection)
{
    %act = UndoActionAddObject::create(%selection, %this.getTrash(), GuiEditorTreeView);
    %act.addToManager(%this.getUndoManager());
    %this.updateUndoMenu();
    $GuiDirty = true;
}

function GuiEditor::onTrashSelection(%this, %selection)
{
    %act = UndoActionDeleteObject::create(%selection, %this.getTrash(), GuiEditorTreeView);
    %act.addToManager(%this.getUndoManager());
    %this.updateUndoMenu();
    $GuiDirty = true;
}

function GuiEditor::onControlInspectPreApply(%this, %object)
{
    %set = new SimSet();
    %set.add(%object);
    %this.onPreEdit(%set);
    %this.pendingGenericUndoAction.actionName = "Change Properties";
    %set.delete();
}

function GuiEditor::onControlInspectPostApply(%this, %object)
{
    %set = new SimSet();
    %set.add(%object);
    %this.onPostEdit(%set);
    %set.delete();
    GuiEditorTreeView.update();
}
