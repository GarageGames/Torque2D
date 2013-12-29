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

function GuiEditorPaletteGui::onWake(%this)
{
    GuiEditorStealPalette(%this);
}

function GuiEditorStealPalette(%thief)
{
    %pal = %thief-->palette;
    
    if (!isObject(%pal))
        %thief.add(GuiEditorPalette);
}

//----------------------------------------
function GuiEditorStartCreate()
{
    NewGuiDialogClass.setText("GuiControl");
    NewGuiDialogClass.sort();
    NewGuiDialogName.setValue("NewGui");
    
    Canvas.pushDialog(NewGuiDialog);
}

//----------------------------------------
function GuiEditorCreate()
{
    %name = NewGuiDialogName.getValue();
    %class = NewGuiDialogClass.getText();
    
    Canvas.popDialog(NewGuiDialog);
    
    %obj = eval("return new " @ %class @ "(" @ %name @ ");");
    
    GuiEditorOpen(%obj);
}

//----------------------------------------
function GuiEditorOpenGui()
{
    %openFileName = GuiBuilder::getOpenName();
    
    if (%openFileName $= "")
        return;   

    // Make sure the file is valid.
    if ((!isFile(%openFileName)) && (!isFile(%openFileName @ ".dso")))
        return;

    // first loop through all the game gui's and see if this already exist
    // we don't want to re-exec a file that was already exec'ed otherwise
    // we will end up with two copies of the gui, and this would be bad
    %foundMatch = false;
    %guiCount = GuiGroup.getCount();
   
    // for every file, look it up in the GuiGroup
    for (%i = 0; %i < %guiCount; %i++)
    {
        %guiObj = GuiGroup.getObject(%i);
        
        if (isObject(%guiObj))
        {
            %guiPath = %guiObj.getScriptFile();
            
            if (strcmp(%openFileName, %guiPath) == 0)
            {
                %foundMatch = true;

                // we found a match, so set %guiContent to the gui object that we
                // found so that GuiEditorOpen will function properly
                %guiContent = %guiObj;
                break;
            }
        }
    }
   
    // If we didn't find a match, then we can exec this file
    if (!%foundMatch)
        %guiContent = TamlRead(%openFileName);
   
    // The level file should have contained a scene, which should now be in the instant
    // group. And, it should be the only thing in the group.
    if (!isObject(%guiContent))
    {
        MessageBox("Torque Game Builder", "You have loaded a Gui file that was created before this version.  It has been loaded but you must open it manually from the content list dropdown", "Ok", "Information");   
        return 0;
    }
  
    GuiEditorOpen(%guiContent);
}
//----------------------------------------
function GuiEditorSaveGui()
{
    %currentObject = GuiEditorContent.getObject(0);
   
    if (%currentObject == -1)
        return;

    if (%currentObject.getName() !$= "")
        %name =  %currentObject.getName() @ ".gui.taml";
    else
        %name = "Untitled.gui.taml";

    %currentFile = %currentObject.getScriptFile();
    
    if (%currentFile $= GuiEditor.blankgui.getScriptFile())
        %currentFile = "";

    // get the filename
    %filename = GuiBuilder::getSaveName(%currentFile);

    if (%filename $= "")
        return;
      
    // Save the Gui
    if (isWriteableFileName(%filename) )
    {
        TamlWrite(%currentObject, %filename);
        
        // set the script file name
        %currentObject.setScriptFile(%filename);

        // Clear the blank gui if we save it
        if (GuiEditor.blankGui == %currentObject)
            GuiEditor.blankGui = new GuiControl();

        $GuiDirty = false;
    }
    else
        MessageBox("Torque Game Builder", "There was an error writing to file '" @ %currentFile @ "'. The file may be read-only.", "Ok", "Error");
   
}

//----------------------------------------
function GuiEdit(%val)
{
    if (%val != 0)
        return;
    
    // [neo, 8/7/2006]  
    // If we're already showing we want to toggle
    if (Canvas.getContent() == GuiEditorGui.getId())
    {
        %undoMgr =  GuiEditor.getUndoManager();
        %undoCount = %undoMgr.getUndoCount();
        %redoCount = %undoMgr.getRedoCount();

        // Prompt to save changes
        if ((%undoCount > 0 || %redoCount > 0) && $GuiDirty)
        {
            Canvas.setContent(GuiEditorGui);
            Canvas.repaint();

            %file = %editObject.getScriptFile();
            %file = fileName(%file);
            %mbResult = checkSaveChanges(%file , false);
            if (%mbResult $= $MROk)
            GuiEditorSaveGui();
        }
        
        %undoMgr.clearAll();

        //showMainEditor();
        Canvas.popDialog(GuiEditorGui);
        Canvas.setContent($holdContent);

        return;
    }

    if (!isobject(GuiEditor.blankGui))
        GuiEditor.blankGui = new GuiControl();

    GuiEditorOpen(GuiEditor.blankGui);
}

package GuiEditor_BlockDialogs
{

function GuiCanvas::pushDialog()
{

}

function GuiCanvas::popDialog()
{

}

};

function GuiEditorGui::initMenus(%this)
{
   if (isObject(%this.menuGroup) )
      %this.menuGroup.delete();
      
   %this.menuGroup = new SimGroup();
   
   //set up %cmdctrl variable so that it matches OS standards
   %cmdCtrl = $platform $= "macos" ? "Cmd" : "Ctrl";

   %filemenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 0;
      barName = "File";
      
      item[0] = "New Gui..." TAB %cmdCtrl SPC "N" TAB "GuiEditorStartCreate();";
      item[1] = "Open Gui..." TAB %cmdCtrl SPC "O" TAB "GuiEditorOpenGui();";
      item[2] = "-";
      item[3] = "Close Gui" TAB %cmdCtrl SPC "W" TAB  "GuiEditorOpen(GuiEditor.blankgui);";
      item[4] = "Save Gui..." TAB %cmdCtrl SPC "S" TAB "GuiEditorSaveGui();";
      item[5] = "-";
      item[6] = "Close Gui Editor..." TAB "" TAB "GuiEdit(0);";
   };

   %editmenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 1;
      barName = "Edit";
      
      item[0] = "Undo" TAB %cmdCtrl SPC "Z" TAB "GuiEditor.undo();";
      item[1] = "Redo" TAB %cmdCtrl @ "-shift Z" TAB "GuiEditor.redo();";
      item[2] = "-";
      item[3] = "Cut" TAB %cmdCtrl SPC "X" TAB "GuiEditor.saveSelection($Gui::clipboardFile); GuiEditor.deleteSelection();";
      item[4] = "Copy" TAB %cmdCtrl SPC "C" TAB "GuiEditor.saveSelection($Gui::clipboardFile);";
      item[5] = "Paste" TAB %cmdCtrl SPC "V" TAB "GuiEditor.loadSelection($Gui::clipboardFile);";
      item[6] = "Select All" TAB %cmdCtrl SPC "A" TAB "GuiEditor.selectAll();";
      item[7] = "-";
      item[8] = "Preferences" TAB %cmdCtrl SPC "," TAB "GuiEditor.showPrefsDialog();";
   };

   %layoutmenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 2;
      barName = "Layout";
      
      item[0] = "Align Left" TAB %cmdCtrl SPC "L" TAB "GuiEditor.Justify(0);";
      item[1] = "Align Right" TAB %cmdCtrl SPC "R" TAB "GuiEditor.Justify(2);";
      item[2] = "Align Top" TAB %cmdCtrl SPC "T" TAB "GuiEditor.Justify(3);";
      item[3] = "Align Bottom" TAB %cmdCtrl SPC "B" TAB "GuiEditor.Justify(4);";
      item[4] = "-";
      item[5] = "Center Horizontally" TAB "" TAB "GuiEditor.Justify(1);";
      item[6] = "Space Vertically" TAB "" TAB "GuiEditor.Justify(5);";
      item[7] = "Space Horizontally" TAB "" TAB "GuiEditor.Justify(6);";
      item[8] = "-";
      item[9] = "Bring to Front" TAB "" TAB "GuiEditor.BringToFront();";
      item[10] = "Send to Back" TAB "" TAB "GuiEditor.PushToBack();";
      item[11] = "Lock Selection" TAB "" TAB "GuiEditorTreeView.lockSelection(true);";
      item[12] = "Unlock Selection" TAB "" TAB "GuiEditorTreeView.lockSelection(false);";
   };
   
   %movemenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 3;
      barName = "Move";
         
      item[0] = "Nudge Left" TAB "Left" TAB "GuiEditor.moveSelection(-1,0);";
      item[1] = "Nudge Right" TAB "Right" TAB "GuiEditor.moveSelection(1,0);";
      item[2] = "Nudge Up" TAB "Up" TAB "GuiEditor.moveSelection(0,-1);";
      item[3] = "Nudge Down" TAB "Down" TAB "GuiEditor.moveSelection(0,1);";
      item[4] = "-";
      item[5] = "Big Nudge Left" TAB "Shift Left" TAB "GuiEditor.moveSelection(-$pref::guiEditor::snap2gridsize * 2,0);";
      item[6] = "Big Nudge Right" TAB "Shift Right" TAB "GuiEditor.moveSelection($pref::guiEditor::snap2gridsize * 2,0);";
      item[7] = "Big Nudge Up" TAB "Shift Up" TAB "GuiEditor.moveSelection(0,-$pref::guiEditor::snap2gridsize * 2);";
      item[8] = "Big Nudge Down" TAB "Shift Down" TAB "GuiEditor.moveSelection(0,$pref::guiEditor::snap2gridsize * 2);";
   };
   
   // add menus to a group
   %this.menuGroup.add(%fileMenu);
   %this.menuGroup.add(%editmenu);
   %this.menuGroup.add(%layoutmenu);
   %this.menuGroup.add(%movemenu);
   // we will be editing the edit menu's undo & redo items. save off a ref to that menu.
   %this.menuGroup.editMenu = %editMenu;
   
}

// Have we already created our menus?
function GuiEditorGui::hasMenus(%this)
{
    return (isObject(%this.menuGroup) && %this.menuGroup.getCount() > 0);
}


// Refactored menu attach stuff so we can call it as needed
// e.g. before and after display changes, etc.
function GuiEditorGui::attachMenuGroup(%this)
{
    if (!isObject(%this.menuGroup) ) 
        return;

    for(%i = 0; %i < %this.menuGroup.getCount(); %i++)
        %this.menuGroup.getObject(%i).attachToMenuBar();
}

// Refactored menu detach stuff so we can call it as needed
// e.g. before and after display changes, etc.
function GuiEditorGui::detachMenuGroup(%this)
{
    if (!isObject(%this.menuGroup) ) 
        return;

    for(%i = 0; %i < %this.menuGroup.getCount(); %i++)
        %this.menuGroup.getObject(%i).removeFromMenuBar();
}

//----------------------------------------
function GuiEditorOpen(%content)
{
	$holdContent = Canvas.getContent();
    if (!GuiEditorGui.hasMenus()) 
        GuiEditorGui.initMenus();
    else if (!Canvas.getContent() == GuiEditorGui.getId())
        attachMenuBars();

    // create the blank gui.
    if (!isObject(GuiEditor.blankgui)) 
        GuiEditor.blankgui = new GuiControl();

    Canvas.setContent(GuiEditorGui);
    
    while((%obj = GuiEditorContent.getObject(0)) != -1)
        GuiGroup.add(%obj); // get rid of anything being edited

    // used to determine which gui files are part of the editor
    //%toolPath = expandPath("^tool/");

    // clear the popup list
    GuiEditorContentList.clear();
   
    // first loop through all the game gui's and see if they already exist
    // we don't want to re-exec a file that was already exec'ed otherwise
    // we will end up with two copies of the gui in memory, and this would be
    // bad
    %search = expandPath("^project/gui/*");
    
    for (%file = findFirstFile(%search); %file !$= ""; %file = findNextFile(%search))
    {
        // only gui files should be here
        if (fileExt(%file) !$= ".gui.taml")
            continue;

        %foundMatch = false;
        %guiCount = GuiGroup.getCount();

        // for every file, look it up in the GuiGroup
        for (%i = 0; %i < %guiCount; %i++)
        {
            %guiObj = GuiGroup.getObject(%i);
            
            if (isObject(%guiObj))
            {
                %guiPath = %guiObj.getScriptFile();
                if (strcmp(%file, %guiPath) == 0)
                {
                    %foundMatch = true;
                    break;
                }
            }
        }

        if (!%foundMatch)
            TamlRead(%file); // load it
    }
   
   // now add all necessary gui's to the drop down list
   %guiCount = GuiGroup.getCount();
   
   for (%i = 0; %i < %guiCount; %i++)
   {
      %guiObj = GuiGroup.getObject(%i);
      if (isObject(%guiObj))
      {
         if (%guiObj.getName() !$= "Canvas" && %guiObj.getId() != GuiEditor.blankGui.getId())
         {
            //%guiPath = %guiObj.getScriptFile();
           if (%guiObj.getName() $= "")
              %name = "(unnamed) -" SPC %guiObj;
           else
              %name = %guiObj.getName() SPC "-" SPC %guiObj;
               
           GuiEditorContentList.add(%name, %guiObj);
           
         }
      }
   }   
   
	activatePackage(GuiEditor_BlockDialogs);
	if(!isObject(%content))
		%content = GuiEditor.blankGui;
	GuiEditorContent.add(%content);
	deactivatePackage(GuiEditor_BlockDialogs);
	GuiEditorContentList.sort();

	GuiEditorResList.clear();
	GuiEditorResList.add("320 x 480", 320);
	GuiEditorResList.add("480 x 320", 480);
	GuiEditorResList.add("640 x 480", 640);
	GuiEditorResList.add("800 x 600", 800);
	GuiEditorResList.add("1024 x 768", 1024);
   
   %ext = $Pref::GuiEditor::PreviewResolution;
   
    if (%ext $= "")
    {
        %ext = GuiEditorRegion.getExtent();
        echo("extent is " @ %ext);
       
        switch(getWord(%ext, 0))
        {
            case 320:
                GuiEditorResList.setText("320 x 480");
            case 480:
                GuiEditorResList.setText("480 x 320");
            case 640:
                GuiEditorResList.setText("640 x 480");
            case 800:
                GuiEditorResList.setText("800 x 600");
            case 1024:
                GuiEditorResList.setText("1024 x 768");
        }
    }
    else
    {
        GuiEditorResList.setText(getWord(%ext,0) @ " x " @ getWord(%ext, 1));
    }
    
    if (!isObject(%content) || %content.getName() $= "")
        %name = "(unnamed) - " @ %content;
    else
        %name = %content.getName() @ " - " @ %content;
   
    GuiEditorContentList.setText(%name);

    GuiEditor.setRoot(%content);
    GuiEditorRegion.resize(0,0,getWord(%ext,0), getWord(%ext, 1));
    //GuiEditorContent.getObject(0).resize(0,0,getWord(%ext,0), getWord(%ext, 1));

    GuiEditorTreeView.open(%content);
   
    // clear the undo manager if we're switching controls.
    if (GuiEditor.lastContent != %content)
        GuiEditor.getUndoManager().clearAll();
      
   GuiEditor.setFirstResponder();
   
    GuiEditor.updateUndoMenu();
    GuiEditor.lastContent = %content;
    $pref::GuiEditor::lastContent = %content;
}

//----------------------------------------
function GuiEditorContentList::onSelect(%this, %id)
{
    GuiEditorOpen(%id);
}

//----------------------------------------
function GuiEditorResList::onSelect(%this, %id)
{
    //Instead of recreating the item's resolution by switch(%id), just
    //grab the text by the id, and use the actual values, this way, we
    //can add in new values, enter the info once, then just grab it when
    //we need it.
    %text = GuiEditorResList.getTextById(%id);
    
    %width = getWord(%text, 0);
    %height = getWord(%text, 2);//text 1 is the x between the values
    
    %preview = %width SPC %height;
    
    GuiEditorRegion.resize(0,0,%width,%height);
    //GuiEditorContent.getObject(0).resize(0,0,%width,%height);
    
    $Pref::GuiEditor::PreviewResolution = %preview;
}

function GuiEditorTreeView::update(%this)
{
    %obj = GuiEditorContent.getObject(0);

    if (!isObject(%obj))
        GuiEditorTreeView.clear();
    else
        GuiEditorTreeView.open(GuiEditorContent.getObject(0));
}

function GuiEditorTreeView::onRightMouseDown(%this, %item, %pts, %obj)
{
    if (%obj)
        GuiEditor.setCurrentAddSet(%obj);
}
function GuiEditorTreeView::onAddSelection(%this,%ctrl)
{
    // rdbnote: commented this out, because it causes the control to get added
    // to the selected controls vector, in the gui editor, multiple times. This
    // is because addSelection does NOT clear the currently selected, like using
    // the .select would. Ironically enough, .select will get called from the
    // code, thus already selecting the object for us.
    //GuiEditor.addSelection(%ctrl);

    GuiEditor.setFirstResponder();
}
function GuiEditorTreeView::onRemoveSelection(%this,%ctrl)
{
    GuiEditor.removeSelection(%ctrl);
}

function GuiEditor::onClearSelected(%this)
{ 
    GuiEditorTreeView.clearSelection();
}

function GuiEditor::onAddSelected(%this,%ctrl)
{
    GuiEditorTreeView.addSelection(%ctrl);
    GuiEditorTreeView.scrollVisibleByObjectId(%ctrl);
}

function GuiEditor::onRemoveSelected(%this,%ctrl)
{
    GuiEditorTreeView.removeSelection(%ctrl); 
}
function GuiEditor::onDelete(%this)
{
    GuiEditorTreeView.update();

    // clear out the gui inspector.
    GuiEditorInspectFields.update(0);
}

// rdbnote: this is a hack to fix a problem with the tree view
function GuiEditor::onSelectionParentChange(%this)
{
    GuiEditorTreeView.update();
}

function GuiEditorTreeView::onDeleteSelection(%this)
{ 
    GuiEditor.clearSelection();
}

function GuiEditorTreeView::onSelect(%this, %obj)
{
    if (isObject(%obj))
    {
        GuiEditorInspectFields.update(%obj);
        GuiEditor.select(%obj);
    }
}

//----------------------------------------
function GuiEditorInspectFields::update(%this, %inspectTarget)
{
    GuiEditorInspectFields.inspect(%inspectTarget);
    
    if (isObject(%inspectTarget))
        GuiEditorInspectName.setValue(%inspectTarget.getName());
}

//----------------------------------------
function GuiEditorInspectApply()
{
    GuiEditorInspectFields.setName(GuiEditorInspectName.getValue());
}

//----------------------------------------
function GuiEditor::onSelect(%this, %ctrl)
{
    GuiEditorInspectFields.update(%ctrl);
    GuiEditor.clearSelection();
    GuiEditor.select(%ctrl);
    GuiEditorTreeView.addSelection(%ctrl); 
}


function GuiEditorSnapCheckBox::onWake(%this)
{
    %snap = $pref::guiEditor::snap2grid * $pref::guiEditor::snap2gridsize;
    %this.setValue(%snap);
    GuiEditor.setSnapToGrid(%snap);
}
function GuiEditorSnapCheckBox::onAction(%this)
{
    %snap = $pref::guiEditor::snap2gridsize * %this.getValue();
    $pref::guiEditor::snap2grid = %this.getValue();
    
    GuiEditor.setSnapToGrid(%snap);
}

function GuiEditor::showPrefsDialog(%this)
{
    Canvas.pushDialog(GuiEditorPrefsDlg);
}

function GuiEditor::togglePalette(%this, %show)
{
    %vis = GuiEditorGui-->togglePaletteBtn.getValue();
    
    if (%show !$= "")
        %vis = %show;

    if (%vis)
        GuiEditorGui.add(GuiEditorPalette);
    else  
        GuiEditorPaletteGui.add(GuiEditorPalette);

    GuiEditorGui-->togglePaletteBtn.setValue(%vis);
}

function GuiEditorPalette::onWake(%this)
{
    %controls = enumerateConsoleClasses("GuiControl");
    %this-->listboxAll.clearItems();
    
    for (%i = 0; %i < getFieldCount(%controls); %i++)
    {
        %field = getField(%controls, %i);
        
        if ((%field $= "GuiCanvas") || (%field $= "GuiEffectCanvas") || (%field $= "GuiDirectoryTreeCtrl"))
            continue;

        %this-->listboxAll.addItem(%field);
    }
   
    // add some common controls to the common page.
    %list = %this-->listboxCommon;
    %list.clearItems();
    %list.addItem("GuiControl");
    %list.addItem("GuiTextCtrl");
    %list.addItem("GuiTextEditCtrl");
    %list.addItem("GuiButtonCtrl");
    %list.addItem("GuiImageButtonCtrl");
    %list.addItem("GuiSpriteCtrl");    
    %list.addItem("GuiPopUpMenuCtrl");
    %list.addItem("GuiCheckBoxCtrl");
    %list.addItem("GuiScrollCtrl");
    %list.addItem("GuiRadioCtrl");
    %list.addItem("GuiSeparatorCtrl");
    %list.addItem("GuiFrameSetCtrl");
    %list.addItem("GuiListBoxCtrl");        
    %list.addItem("GuiRolloutCtrl");    
    %list.addItem("GuiSliderCtrl");
    %list.addItem("GuiTabBookCtrl");
    %list.addItem("GuiTabPageCtrl");
    %list.addItem("GuiWindowCtrl");

    %this-->paletteBook.selectPage(0);
}

function GuiEditorPaletteDragList::onMouseDragged(%this)
{
    %position = %this.getGlobalPosition();
    %cursorpos = Canvas.getCursorPos();

    %class = %this.getItemText(%this.getSelectedItem());
    %payload = eval("return new " @ %class @ "();");
    if (!isObject(%payload))
        return;
      
    // this offset puts the cursor in the middle of the dragged object.
    %xOffset = getWord(%payload.extent, 0) / 2;
    %yOffset = getWord(%payload.extent, 1) / 2;  

    // position where the drag will start, to prevent visible jumping.
    %xPos = getWord(%cursorpos, 0) - %xOffset;
    %yPos = getWord(%cursorpos, 1) - %yOffset;

    %dragCtrl = new GuiDragAndDropControl()
    {
        canSaveDynamicFields = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = %xPos SPC %yPos;
        extent = %payload.extent;
        MinExtent = "32 32";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        deleteOnMouseUp = true;
    };

    %dragCtrl.add(%payload);
    Canvas.getContent().add(%dragCtrl);

    %dragCtrl.startDragging(%xOffset, %yOffset);
}

function GuiEditor::onControlDragged(%this, %payload, %position)
{
    // use the position under the mouse cursor, not the payload position.
    %position = VectorSub(%position, GuiEditorContent.getGlobalPosition());
    %x = getWord(%position, 0);
    %y = getWord(%position, 1);
    %target = GuiEditorContent.findHitControl(%x, %y);

    while(! %target.isContainer)
        %target = %target.getParent();
    
    if (%target != %this.getCurrentAddSet())
        %this.setCurrentAddSet(%target);
}

function GuiEditor::onControlDropped(%this, %payload, %position)
{  
    %pos = %payload.getGlobalPosition();
    %x = getWord(%pos, 0);
    %y = getWord(%pos, 1);

    %this.addNewCtrl(%payload);

    %payload.setPositionGlobal(%x, %y);
    %this.setFirstResponder();
}

function GuiEditor::undo(%this)
{
    %this.getUndoManager().undo();
    %this.updateUndoMenu();
    %this.clearSelection();
}

function GuiEditor::redo(%this)
{
    %this.getUndoManager().redo();
    %this.updateUndoMenu();
    %this.clearSelection();
}

function GuiEditor::updateUndoMenu(%this)
{
    %man = %this.getUndoManager();
    %nextUndo = %man.getNextUndoName();
    %nextRedo = %man.getNextRedoName();

    %cmdCtrl = $platform $= "macos" ? "Cmd" : "Ctrl";
    %undoitem = "Undo" SPC %nextUndo TAB %cmdCtrl SPC "Z" TAB "GuiEditor.undo();";
    %redoitem = "Redo" SPC %nextRedo TAB %cmdCtrl @ "-shift Z" TAB "GuiEditor.redo();";

    GuiEditorGui.menuGroup.editMenu.removeItem(0);
    GuiEditorGui.menuGroup.editMenu.removeItem(0);
    GuiEditorGui.menuGroup.editMenu.addItem(0, %undoitem);
    GuiEditorGui.menuGroup.editMenu.addItem(1, %redoitem);

    GuiEditorGui.menuGroup.editMenu.enableItem(0, %nextUndo !$= "");
    GuiEditorGui.menuGroup.editMenu.enableItem(1, %nextRedo !$= "");
}

//------------------------------------------------------------------------------
// Gui Editor Menu activation
function GuiEditorGui::onWake(%this)
{
    if (!isObject(%this.menuGroup))
        return;

    for(%i = 0; %i < %this.menuGroup.getCount(); %i++)
        %this.menuGroup.getObject(%i).attachToMenuBar();
}

function GuiEditorGui::onSleep(%this)
{
    if (!isObject(%this.menuGroup))
        return;

    for(%i = 0; %i < %this.menuGroup.getCount(); %i++)
        %this.menuGroup.getObject(%i).removeFromMenuBar();
}
