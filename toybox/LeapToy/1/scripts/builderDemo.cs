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

function LeapToy::createBuilderLevel( %this )
{
    // Create background.
    %this.createBackground();

    // Create the ground.
    %this.createGround();

    // Create the pyramid.
    %this.createPyramid();

    // Create circle gesture visual.
    %this.createCircleSprite();

    // Create circle gesture visual.
    %this.createBuilderFingers();    

    // Set the gravity.
    SandboxScene.setGravity( 0, -9.8 );

    // Set the manipulation mode.
    Sandbox.useManipulation( off );

    // Create the help text scene
    %helpText = new SimSet();
    %helpText.add(new ScriptObject() { Text = "Press TAB to toggle between Cursor and Finger control."; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "Finger Control: "; });
    %helpText.add(new ScriptObject() { Text = "   Reach in to enable finger collision."; });
    %helpText.add(new ScriptObject() { Text = "   Fingers will turn yellow when collision is enabled."; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "Cursor Control: "; });
    %helpText.add(new ScriptObject() { Text = "   Key tap creates a new block."; });
    %helpText.add(new ScriptObject() { Text = "   Circle Gesture selects blocks within the circle."; });
    %helpText.add(new ScriptObject() { Text = "   Screen Tap deletes the selected blocks."; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "Press H to return to the demo."; });
    %this.createHelpTextScene(%helpText);
    %helpText.delete();

    %this.CenterZ = 125;

    // Swap action maps
    FingerMap.pop();
    BreakoutMap.pop();
    GestureMap.pop();

    // Enable builder map
    BuilderMap.push();
}

function LeapToy::createBuilderBlock(%this, %position)
{
    // Set the block size.
    %blockSize = LeapToy.BlockSize;

    %blockFrames = "0 2 4 6";
    %randomNumber = getRandom(0, 4);

    // Create the sprite.
    %obj = new Sprite()
    {
        class = "Block";
        flipped = false;
    };

    %obj.setSceneLayer(3);
    %obj.setPosition( %position.x, %position.y );
    %obj.setSize( %blockSize );
    %obj.setImage( "LeapToy:objectsBlocks" );
    %obj.setImageFrame( getWord(%blockFrames, %randomNumber) );
    %obj.setDefaultFriction( 1.0 );
    %obj.createPolygonBoxCollisionShape( %blockSize, %blockSize );

    // Add to the scene.
    SandboxScene.add( %obj );
}



//-----------------------------------------------------------------------------
// Called when the user makes a circle with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %progress - How much of the circle has been completed
// %radius - Radius of the circle created by the user's motion
// %isClockwise - Toggle based on the direction the user made the circle
// %state - State of the gesture progress: 1: Started, 2: Updated, 3: Stopped
function LeapToy::reactToCircleBuilder(%this, %id, %progress, %radius, %isClockwise, %state)
{
    if (!%this.enableCircleGesture)
        return;

    if (isLeapCursorControlled())
    {
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
}

//-----------------------------------------------------------------------------
// Called when the user makes a swipe with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %state - State of the gesture progress: 1: Started, 2: Updated, 3: Stopped
// %direction - 3 point vector based on the direction the finger swiped
// %speed - How fast the user's finger moved. Values will be quite large
function LeapToy::reactToSwipeBuilder(%this, %id, %state, %direction, %speed)
{
    if (!%this.enableSwipeGesture)
        return;
}

//-----------------------------------------------------------------------------
// Called when the user makes a screen tap gesture with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %position - 3 point vector based on where the finger was located in "Leap Space"
// %direction - 3 point vector based on the direction the finger motion
function LeapToy::reactToScreenTapBuilder(%this, %id, %position, %direction)
{
    if (!%this.enableScreenTapGesture)
        return;        
    
    %this.deleteSelectedObjects();
}

//-----------------------------------------------------------------------------
// Called when the user makes a key tap gesture with their finger(s)
//
// %id - Finger ID, based on the order the finger was added to the tracking
// %position - 3 point vector based on where the finger was located in "Leap Space"
// %direction - 3 point vector based on the direction the finger tap
function LeapToy::reactToKeyTapBuilder(%this, %id, %position, %direction)
{
    if (!%this.enableKeyTapGesture)
        return;

    if (isLeapCursorControlled())
    {       
       %worldPosition = SandboxWindow.getWorldPoint(Canvas.getCursorPos());
       %this.createBuilderBlock(%worldPosition);
    }
}

//-----------------------------------------------------------------------------
// Constantly polling callback based on the finger position on a hand
// %id - Ordered hand ID based on when it was added to the tracking device
// %position - 3 point vector based on where the finger is located in "Leap Space"
function LeapToy::trackFingerPosBuilder(%this, %ids, %fingersX, %fingersY, %fingersZ)
{
    if (!%this.enableFingerTracking)
        return;

    for(%i = 0; %i < getWordCount(%ids); %i++)
    {
        %id = getWord(%ids, %i);
        
        // The next two lines of code use projection. To use intersection
        // comment out both lines, then uncomment the getPointFromIntersection call.
        %position = getWord(%fingersX, %i) SPC getWord(%fingersY, %i) SPC getWord(%fingersZ, %i);
        %screenPosition = getPointFromProjection(%position);
        
        // This uses intersection
        //%screenPosition = getPointFromIntersection(%id);        

        %worldPosition = SandboxWindow.getWorldPoint(%screenPosition);
        %this.showFingerBuilder(%id, %worldPosition, %position.z);
    }
}

//-----------------------------------------------------------------------------

function LeapToy::showFingerBuilder(%this, %id, %worldPosition, %zpos)
{
    echo("Finger " SPC %id SPC ":" SPC %worldPosition);
    
    %finger = %this.fingers[%id];
    
    if (!isObject(%finger))
        return;
        
    if (!%finger.visible)
    {
        %finger.visible = true;
        %finger.setPosition(%worldPosition);
        %this.movePosition[%id] = %worldPosition;
    }
    else
    {
       %distance = VectorSub(%finger.getPosition(), %worldPosition);
       %finger.moveTo(%worldPosition, VectorLen(%distance) * 10, true, false);
       %this.movePosition[%id] = %worldPosition;
    }
    
    %size = 2;
    if( %zpos > LeapToy.CenterZ )
    {
       %finger.setCollisionSuppress(true);
       // set the size to be bigger to give the illusion it is in front of the play area
       %size = (%zpos/LeapToy.CenterZ) * 2;
       %finger.setBlendColor(1.0, 1.0, 1.0, 0.5);
    }
    else
    {
       %finger.setCollisionSuppress(false);
       %finger.setBlendColor("Yellow");
    }
    
    %finger.setSize(%size, %size);
    %this.schedule(200, "checkFingerBuilder", %id, %worldPosition);
}

//-----------------------------------------------------------------------------

function LeapToy::checkFingerBuilder(%this, %id, %worldPosition)
{
    %finger = %this.fingers[%id];
    
    // Likely no activity, since the device is sensitive
    if (%this.movePosition[%id] $= %worldPosition)
    {
        %finger.visible = false;
        %finger.setCollisionSuppress(true);
    }
}

//-----------------------------------------------------------------------------

function LeapToy::createBuilderFingers(%this)
{
    for (%i = 0; %i < 10; %i++)
    {
        %name = "Finger" @ %i;
        
        // Create the circle.
        %finger = new Sprite();        
        %finger.setName(%name);
        %finger.Position = "-10 5";
        
        %finger.Size = 2;
        %finger.Image = "LeapToy:circleThree";
        %finger.Visible = false; 
        %finger.createCircleCollisionShape(1.0, "0 0");  
        %finger.FixedAngle = true;
        %finger.setDefaultFriction(1.0, true);    
        %finger.CollisionSuppress = true;
        %finger.setSceneLayer(0);
        %finger.setCollisionLayers("1 2 3 4 5");       
        
        %this.fingers[%i] = %finger;
        
        // Add to the scene.
        SandboxScene.add( %finger );
    }
}

