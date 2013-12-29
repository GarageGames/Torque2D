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

$useNewScene = false;

/// Point2F(SceneWindow this, Point2F size)
/// Converts a size from world coordinates to window coordinates.
/// @param this The SceneWindow.
/// @param size The size to convert.
/// @return The converted value.
function SceneWindow::getWindowSize(%this, %size)
{
    %origin = %this.getWindowPoint("0 0");
    %size = %this.getWindowPoint(%size);
    %size = Vector2Sub(%size, %origin);
    return %size;
}

/// Point2F(SceneWindow this, Point2F size)
/// Converts a size from window coordinates to world coordinates.
/// @param this The SceneWindow.
/// @param size The size to convert.
/// @return The converted value.
function SceneWindow::getWorldSize(%this, %size)
{
    %origin = %this.getWorldPoint("0 0");
    %size = %this.getWorldPoint(%size);
    %size = Vector2Sub(%size, %origin);
    return %size;
}

//------------------------------------------------------------------------------
// SceneWindow.loadLevel
// Loads a level file into a scene window.
//------------------------------------------------------------------------------
function SceneWindow::loadLevel(%sceneWindow, %levelFile)
{
    // Clean up any previously loaded stuff.
    %sceneWindow.endLevel();

    // Load the level.
    $useNewScene = true;

    //load level resources        -Mat
    %resPath = expandPath("^project/game/template/data/levels/");    

    //now get level specific resources 
    //Get just filename, then strip out extension         -Mat
    %levelFileName = fileName(%levelFile);
    %levelFileName = strreplace(%levelFileName, ".scene.taml", "");

    $CurrentLevel = %levelFileName;
   
    %newLevelFile = %levelFile;
    echo("--------Adding to Level" SPC %newlevelfile);//-Mat iPhone debug output	   

    //load level         -Mat
    %scene = %sceneWindow.addToLevel(%newlevelfile, %dbFileName);

    //Load any level specific GUI
    %LevelGUIfunction = "load" @ %levelFileName @ "GUI";
    
    //call the function with no parameters
    if (isFunction(%LevelGUIfunction) )
        eval(%LevelGUIfunction @ "();");
   
    if (!isObject(%scene))
        return 0;
   
    %sceneWindow.setScene(%scene);

    // Set the window properties from the scene if they are available.
    %cameraPosition = %sceneWindow.getCurrentCameraPosition();
    %cameraSize = Vector2Sub(getWords(%sceneWindow.getCurrentCameraArea(), 2, 3), getWords(%sceneWindow.getCurrentCameraArea(), 0, 1));

    if (%scene.cameraPosition !$= "")
        %cameraPosition = %scene.cameraPosition;
    if (%scene.cameraSize !$= "")
        %cameraSize = %scene.cameraSize;
      
    %sceneWindow.setCurrentCameraPosition(%cameraPosition, %cameraSize);

    // Only perform "onLevelLoaded" callbacks when we're NOT editing a level
    // This is so that objects that may have script associated with them that
    // the level builder cannot undo will not be executed while using the tool.
          
    if (!$LevelEditorActive)
    {
        // Notify the scene that it was loaded.
        if (%scene.isMethod("onLevelLoaded") )
            %scene.onLevelLoaded(%levelFile);

        // And finally, notify all the objects that they were loaded.
        %sceneObjectList = %scene.getSceneObjectList();
        %sceneObjectCount = getWordCount(%sceneObjectList);
        for (%i = 0; %i < %sceneObjectCount; %i++)
        {
            %sceneObject = getWord(%sceneObjectList, %i);
            
            %sceneObject.onLevelLoaded(%scene);
        }
        
        GameEventManager.postEvent("_LevelLoaded", %scene);
    }

    if (!isObject(%sceneWindow.lockedObjectSet))
        %sceneWindow.lockedObjectSet = new SimSet();   

    $lastLoadedScene = %scene;

    return %scene;
}

//------------------------------------------------------------------------------
// SceneWindow.addToLevel
// Adds a level file to a scene window's scene.
//------------------------------------------------------------------------------
function SceneWindow::addToLevel(%sceneWindow, %levelFile, %dbFileName)
{
    %scene = %sceneWindow.getScene();
    if (!isObject(%scene))
    {
        %scene = new Scene();
        %sceneWindow.setScene(%scene);
    }

    %newScene = %scene.addToLevel(%levelFile, %dbFileName);

    $lastLoadedScene = %newScene;
    return %newScene;
}



//------------------------------------------------------------------------------
// SceneWindow.endLevel
// Clears a scene window.
//------------------------------------------------------------------------------
function SceneWindow::endLevel(%sceneWindow)
{
    %scene = %sceneWindow.getScene();

    if (!isObject(%scene))
        return;

    %scene.endLevel();

    if (isObject(%this.lockedObjectSet))
        %this.lockedObjectSet.clear();

    if (isObject(%scene))
    {
        if (isObject(%scene.getGlobalTileMap()) )
            %scene.getGlobalTileMap().delete();

        %scene.delete();
    }

    $lastLoadedScene = "";
}


function SceneWindow::addLockedObject(%this, %objectID)
{
    %this.lockedObjectSet.add(%objectID);
}

function SceneWindow::removeLockedObject(%this, %objectID)
{
    if (%this.lockedObjectSet.isMember(%objectID))
        %this.lockedObjectSet.remove(%objectID);
}

function SceneWindow::onTouchDown(%this, %touchID, %worldPos)
{
    if (!isObject(%this.lockedObjectSet))
        return;  
        
    for(%i = 0; %i < %this.lockedObjectSet.getCount(); %i++)
    {
      %object = %this.lockedObjectSet.getObject(%i);
      
      if (%object.isMethod(onTouchDown))
        %object.onTouchDown(%touchID, %worldPos);
    }
}

function SceneWindow::onTouchUp(%this, %touchID, %worldPos)
{
    if (!isObject(%this.lockedObjectSet))
        return;  
        
    for(%i = 0; %i < %this.lockedObjectSet.getCount(); %i++)
    {
      %object = %this.lockedObjectSet.getObject(%i);
      
      if (%object.isMethod(onTouchUp))
        %object.onTouchUp(%touchID, %worldPos);
    }
}

function SceneWindow::onTouchMoved(%this, %touchID, %worldPos)
{
    if (!isObject(%this.lockedObjectSet))
        return;    
    
    for(%i = 0; %i < %this.lockedObjectSet.getCount(); %i++)
    {
        %object = %this.lockedObjectSet.getObject(%i);

        if (%object.isMethod(onTouchMoved))
            %object.onTouchMoved(%touchID, %worldPos);
    }
}

function SceneWindow::onTouchDragged(%this, %touchID, %worldPos)
{
    if (!isObject(%this.lockedObjectSet))
        return;     
    
    for(%i = 0; %i < %this.lockedObjectSet.getCount(); %i++)
    {
        %object = %this.lockedObjectSet.getObject(%i);

        if (%object.isMethod(onTouchDragged))
            %object.onTouchDragged(%touchID, %worldPos);
    }
}