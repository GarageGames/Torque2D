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

function LeapToy::create( %this )
{
    // Execute toy scripts
    exec("./scripts/input.cs");
    exec("./scripts/toy.cs");
    exec("./scripts/gestureDemo.cs");
    exec("./scripts/breakoutDemo.cs");
    exec("./scripts/fingerDemo.cs");
    exec("./scripts/builderDemo.cs");
    exec("./scripts/helpText.cs");
    
    exec("./scripts/DealsDamageBehavior.cs");
    exec("./scripts/TakesDamageBehavior.cs");
    exec("./scripts/SwapImageOnCollision.cs");

    %this.handPosDeadzone = "-1.0 1.0";
    %this.handRotDeadzone = "-1.0 1.0";
    %this.fingerPosDeadzone = "-1.0 1.0";
    %this.enableSwipeGesture = false;
    %this.enableCircleGesture = false;
    %this.enableScreenTapGesture = false;
    %this.enableKeyTapGesture = false;
    %this.enableHandRotation = false;
    %this.enableFingerTracking = true;
    %this.currentLevel = "Builder Demo";
    
    addFlagOption( "Enable Swipe Gesture", "setEnableSwipeGesture", LeapToy.enableSwipeGesture, false, "Turns on swipe gesture recognition" );
    addFlagOption( "Enable Circle Gesture", "setEnableCircleGesture", LeapToy.enableCircleGesture, false, "Turns on circle gesture recognition" );
    addFlagOption( "Enable Screen Tap Gesture", "setEnableScreenTapGesture", LeapToy.enableScreenTapGesture, false, "Turns on screen tap gesture recognition" );
    addFlagOption( "Enable Key Tap Gesture", "setEnableKeyTapGesture", LeapToy.enableKeyTapGesture, false, "Turns on key tap gesture recognition" );
    addFlagOption( "Enable Hand Rotation", "setEnableHandRotation", LeapToy.enableHandRotation, false, "Turns on tracking of hand rotation" );
    addFlagOption( "Enable Finger Tracking", "setenableFingerTracking", LeapToy.enableFingerTracking, false, "Turns on tracking of finger position" );
    
    %options = "Gestures,Breakout,Finger Tracking,Builder Demo";
    addSelectionOption(%options, "Level", 4, setLevel, true, "Choose which Leap Motion Demo to play");

    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pull );
    
    // Configure the toy.
    LeapToy.GroundWidth = 40;
    LeapToy.BlockSize = 2;
    LeapToy.BlockCount = 15;
    LeapToy.BrickRows = 4;
    LeapToy.BrickColumns = 9;
    LeapToy.BrickSize = "3 1";
    LeapToy.PaddleSpeed = 30;
    LeapToy.maxBallSpeed = 20;
    LeapToy.fingerCount = 0;
    
    // Initialize all bindings
    %this.initializeInput();
    
    // Build the behaviors
    %this.createDealsDamageBehavior();
    %this.createTakesDamageBehavior();
    %this.createSwapImageBehavior();
    
    %this.selectedObjects = new SimSet();
    %this.add(%this.selectedObjects);
    
    // Reset the toy.
    LeapToy.reset();
}

//-----------------------------------------------------------------------------

function LeapToy::destroy( %this )
{
    // Turn off Leap driven cursor control, if it was activated
    if (isLeapCursorControlled())
        enableLeapMotionManager(false);
    
    %this.selectedObjects.clear();
    %this.selectedObjects.delete();
    // Clean up the help text
    if (isObject(HelpTextScene))
      HelpTextScene.delete();
    // Clean up the ActionMaps
    %this.destroyInput();
}

//-----------------------------------------------------------------------------

function LeapToy::reset( %this )
{
    // Reset the input
    FingerMap.pop();
    GestureMap.pop();
    BreakoutMap.pop();
    BuilderMap.pop();
    // Clean up the help text if there is one
    %this.clearHelpTextScene();    
      
     // Clear the scene.
    SandboxScene.clear();
    
    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );

    if (%this.currentLevel $= "Gestures")
        %this.createGesturesLevel();
    else if (%this.currentLevel $= "Breakout")
        %this.createBreakoutLevel();
    else if (%this.currentLevel $= "Finger Tracking")
        %this.createFingerLevel();
    else if (%this.currentLevel $= "Builder Demo")
        %this.createBuilderLevel();
}

//-----------------------------------------------------------------------------

function LeapToy::setLevel( %this, %value)
{
    %this.currentLevel = %value;
}

//-----------------------------------------------------------------------------

function LeapToy::setEnableSwipeGesture( %this, %value )
{
    %this.enableSwipeGesture = %value;
}

//-----------------------------------------------------------------------------

function LeapToy::setEnableCircleGesture( %this, %value )
{
    %this.enableCircleGesture = %value;
}

//-----------------------------------------------------------------------------

function LeapToy::setEnableScreenTapGesture( %this, %value )
{
    %this.enableScreenTapGesture = %value;
}

//-----------------------------------------------------------------------------

function LeapToy::setEnableKeyTapGesture( %this, %value )
{
    %this.enableKeyTapGesture = %value;
}

//-----------------------------------------------------------------------------

function LeapToy::setEnableHandRotation( %this, %value )
{
    %this.enableHandRotation = %value;
}

//-----------------------------------------------------------------------------

function LeapToy::setEnableFingerTracking( %this, %value )
{
    %this.enableFingerTracking = %value;
}