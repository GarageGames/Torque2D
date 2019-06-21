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

function LeapToy::initializeInput( %this )
{
    // General toy action map
    new ActionMap(ToyMap);
    
    // Gesture action map for sandbox
    new ActionMap(GestureMap);
    
    // Absolute palm rotation/pos action map for breakout game
    new ActionMap(BreakoutMap);
    
    // Absolute finger positioning for finger demo
    new ActionMap(FingerMap);
    
    // action map for the builder demo game
    new ActionMap(BuilderMap);
    
    // Create keyboard bindings
    ToyMap.bindObj(keyboard, tab, toggleCursorMode, %this);
    ToyMap.bindObj(keyboard, escape, showToolBox, %this);
    // Help text binding
    ToyMap.bindObj(keyboard, h, toggleHelpTextScene, %this);

    // Debugging keybinds
    ToyMap.bindObj(keyboard, space, simulateCircle, %this);
    ToyMap.bindObj(keyboard, x, simulateKeytap, %this);
    ToyMap.bindObj(keyboard, z, showParticle, %this);    
    ToyMap.bindObj(keyboard, a, movePaddleLeft, %this);    
    ToyMap.bindObj(keyboard, d, movePaddleRight, %this);    
    
    // Create Leap Motion gesture bindings
    GestureMap.bindObj(leapdevice, circleGesture, reactToCircleGesture, %this);
    GestureMap.bindObj(leapdevice, screenTapGesture, reactToScreenTapGesture, %this);
    GestureMap.bindObj(leapdevice, swipeGesture, reactToSwipeGesture, %this);
    GestureMap.bindObj(leapdevice, keyTapGesture, reactToKeyTapGesture, %this);
    
    // Create the Leap Motion hand/finger bindings
    BreakoutMap.bindObj(leapdevice, leapHandRot, "D", %this.handRotDeadzone, trackHandRotation, %this);
    FingerMap.bindObj(leapdevice, leapFingerPos, trackFingerPos, %this);
    
   // Create Leap Motion Builder Demo bindings
    BuilderMap.bindObj(leapdevice, circleGesture, reactToCircleBuilder, %this);
    BuilderMap.bindObj(leapdevice, screenTapGesture, reactToScreenTapBuilder, %this);
    BuilderMap.bindObj(leapdevice, swipeGesture, reactToSwipeBuilder, %this);
    BuilderMap.bindObj(leapdevice, keyTapGesture, reactToKeyTapBuilder, %this);
    BuilderMap.bindObj(leapdevice, leapFingerPos, trackFingerPosBuilder, %this);

    // Push the LeapMap to the stack, making it active
    ToyMap.push();
    
    // Initialize the Leap Motion manager
    initLeapMotionManager();
    enableLeapMotionManager(true);
    enableLeapCursorControl(true);
    
    configureLeapGesture("Gesture.Circle.MinProgress", 1);
    configureLeapGesture("Gesture.ScreenTap.MinForwardVelocity", 0.1);
    configureLeapGesture("Gesture.ScreenTap.MinDistance", 0.1);
    configureLeapGesture("Gesture.KeyTap.MinDownVelocity", 20);
    configureLeapGesture("Gesture.KeyTap.MinDistance", 1.0);
}

//-----------------------------------------------------------------------------

function LeapToy::destroyInput(%this)
{
    FingerMap.pop();
    FingerMap.delete();
    
    BreakoutMap.pop();
    BreakoutMap.delete();
    
    GestureMap.pop();
    GestureMap.delete();
    
    ToyMap.pop();
    ToyMap.delete();
    
    BuilderMap.pop();
    BuilderMap.delete();
}

//-----------------------------------------------------------------------------
// Called when the user makes a circle with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %progress - How much of the circle has been completed
// %radius - Radius of the circle created by the user's motion
// %isClockwise - Toggle based on the direction the user made the circle
// %state - State of the gesture progress: 1: Started, 2: Updated, 3: Stopped
function LeapToy::reactToCircleGesture(%this, %id, %progress, %radius, %isClockwise, %state)
{
    if (!%this.enableCircleGesture)
        return;

    if (%progress > 0 && %state == 3)
    {
        %this.grabObjectsInCircle();
	    %this.schedule(300, "hideCircleSprite");
    }
    else if (%progress > 0 && %state != 3)
    {
        %this.showCircleSprite(%radius/5, %isClockwise);
    }
}

//-----------------------------------------------------------------------------
// Called when the user makes a swipe with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %state - State of the gesture progress: 1: Started, 2: Updated, 3: Stopped
// %direction - 3 point vector based on the direction the finger swiped
// %speed - How fast the user's finger moved. Values will be quite large
function LeapToy::reactToSwipeGesture(%this, %id, %state, %direction, %speed)
{
    if (!%this.enableSwipeGesture)
        return;

    %worldPosition = SandboxWindow.getWorldPoint(Canvas.getCursorPos());

    if (isLeapCursorControlled())
        %worldPosition = "0 0";

    %this.createAsteroid(%worldPosition, %direction, %speed);
}

//-----------------------------------------------------------------------------
// Called when the user makes a screen tap gesture with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %position - 3 point vector based on where the finger was located in "Leap Space"
// %direction - 3 point vector based on the direction the finger motion
function LeapToy::reactToScreenTapGesture(%this, %id, %position, %direction)
{
    if (!%this.enableScreenTapGesture)
        return;

    %control = Canvas.getMouseControl();
    %control.performClick();
}

//-----------------------------------------------------------------------------
// Called when the user makes a key tap gesture with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %position - 3 point vector based on where the finger was located in "Leap Space"
// %direction - 3 point vector based on the direction the finger tap
function LeapToy::reactToKeyTapGesture(%this, %id, %position, %direction)
{
    if (!%this.enableKeyTapGesture)
        return;

    %this.deleteSelectedObjects();
}

//-----------------------------------------------------------------------------
// Constantly polling callback based on the palm position of a hand
//
// %id - Ordered hand ID based on when it was added to the tracking device
// %position - 3 point vector based on where the hand is located in "Leap Space"
function LeapToy::trackHandPosition(%this, %id, %position)
{
    echo("Hand " @ %id @ " - x:" SPC %position._0 SPC "y:" SPC %position._1 SPC "z:" SPC %position._2);
}

//-----------------------------------------------------------------------------
// Constantly polling callback based on the palm rotation of a hand
//
// %id - Ordered hand ID based on when it was added to the tracking device
// %rotation - 3 point vector based on the hand's rotation: "yaw pitch roll"
function LeapToy::trackHandRotation(%this, %id, %rotation)
{
    if (isLeapCursorControlled() || !%this.enableHandRotation)
        return;

    // Grab the values. Only going to use roll in this demo
    %roll = %rotation._2;

    %this.movePaddle(%roll*-1);
}

//-----------------------------------------------------------------------------
// Constantly polling callback based on the finger position on a hand
// %id - Ordered hand ID based on when it was added to the tracking device
// %position - 3 point vector based on where the finger is located in "Leap Space"
function LeapToy::trackFingerPos(%this, %ids, %fingersX, %fingersY, %fingersZ)
{
    if (!%this.enableFingerTracking)
        return;

    for(%i = 0; %i < getWordCount(%ids); %i++)
    {
        %id = getWord(%ids, %i);

        %position = getWord(%fingersX, %i) SPC getWord(%fingersY, %i) SPC getWord(%fingersZ, %i);
        %screenPosition = getPointFromProjection(%position);
        //%screenPosition = getPointFromIntersection(%id);

        %worldPosition = SandboxWindow.getWorldPoint(%screenPosition);
        %this.showFingerCircle(%id, %worldPosition);        
    }
}

//-----------------------------------------------------------------------------
// Flips a switch to activate/deactivate cursor control by the Leap Motion Manager
function LeapToy::toggleCursorMode( %this, %val )
{
    if (!%val)
        return;
        
    if (isLeapCursorControlled())
        enableLeapCursorControl(false);
    else
        enableLeapCursorControl(true);
}

//-----------------------------------------------------------------------------
// Shows the Sandbox tool box
function LeapToy::showToolBox( %this, %val )
{
    if (%val)
        toggleToolbox(true);
}

//-----------------------------------------------------------------------------
// DEBUGGING FUNCTIONS
function LeapToy::simulateCircle( %this, %val)
{
    if (%val)
    {
        %this.grabObjectsInCircle(2);
    }
}
function LeapToy::simulateKeyTap( %this, %val )
{
    if (%val)
    {
        %this.deleteSelectedObjects();
    }
}

function LeapToy::showParticle(%this)
{
    %worldPosition = SandboxWindow.getWorldPoint(Canvas.getCursorPos());
    
    %particlePlayer = new ParticlePlayer();
    %particlePlayer.BodyType = static;
    %particlePlayer.SetPosition( %worldPosition );
    %particlePlayer.SceneLayer = 0;
    %particlePlayer.ParticleInterpolation = true;
    %particlePlayer.Particle = "LeapToy:blockFadeParticle";
    %particlePlayer.SizeScale = 1;
    SandboxScene.add( %particlePlayer ); 
    %particlePlayer.setLifeTime(3);
}

function LeapToy::movePaddleLeft(%this, %val)
{
    if (%val)
    {
        %this.paddle.setLinearVelocity(%this.PaddleSpeed*-1, 0);
    }
    else
    {
        %this.paddle.setLinearVelocity(0, 0);
    }
}

function LeapToy::movePaddleRight(%this, %val)
{
    if (%val)
    {
        %this.paddle.setLinearVelocity(%this.PaddleSpeed, 0);
    }
    else
    {
        %this.paddle.setLinearVelocity(0, 0);
    }
}

function LeapToy::simulateFingerPositions(%this)
{
    %numFingers = getRandom(0, 9);
    setRandomSeed(-1);
    %randomPosition = getRandom(-10, 10) SPC getRandom(-10, 10);
    for (%i = 0; %i < %numFingers; %i++)
    {
        %this.showFingerCircle(%i, %randomPosition);
    }
}

// DEBUGGING FUNCTIONS
//-----------------------------------------------------------------------------