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

function LeapToy::createFingerLevel( %this )
{
    // Create background.
    %this.createBackground();
    
    // Create the ground.
    %this.createGround();

    // Create circles that will track the fingers    
    %this.createFingerCircles();
    
    // Set the gravity.
    SandboxScene.setGravity( 0, 0);
    
    // Set the manipulation mode.
    Sandbox.useManipulation( off );

    // Swap action maps
    // Swap action maps
    GestureMap.pop();
    BreakoutMap.pop();
    FingerMap.push();
    
    // Create the help text scene
    %helpText = new SimSet();
    %helpText.add(new ScriptObject() { Text = "This demo will track pointables."; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "Whenever the Leap Motion device detects a finger"; });    
    %helpText.add(new ScriptObject() { Text = "or tool, a circle will appear on screen."; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "Press H to return to the demo."; });
    %this.createHelpTextScene(%helpText);
    %helpText.delete();
}

//-----------------------------------------------------------------------------

function LeapToy::createFingerCircles(%this)
{
    for (%i = 0; %i < 10; %i++)
    {
        %name = "Finger" @ %i;
        
        // Create the circle.
        %circle = new Sprite();        
        %circle.setName(%name);
        %circle.Position = "-10 5";
        %circle.setBodyType("Kinematic");
        %circle.Size = 5;
        %circle.Image = "LeapToy:circleThree";
        %circle.Visible = false;
        %circle.AngularVelocity = 180;
        
        %this.fingers[%i] = %circle;
        
        // Add to the scene.
        SandboxScene.add( %circle );
    }
}

//-----------------------------------------------------------------------------

function LeapToy::showFingerCircle(%this, %id, %worldPosition)
{
    echo("Finger " SPC %id SPC ":" SPC %worldPosition);
    
    %finger = %this.fingers[%id];
    
    if (!isObject(%finger))
        return;
        
    if (!%finger.visible)
    {
        %finger.visible = true;
    }
    
    %finger.setPosition(%worldPosition);
    %this.schedule(100, "checkFingerActivity", %id, %worldPosition);
}

//-----------------------------------------------------------------------------

function LeapToy::checkFingerActivity(%this, %id, %worldPosition)
{
    %finger = %this.fingers[%id];
    
    // Likely no activity, since the device is sensitive
    if (%finger.position $= %worldPosition)
        %finger.visible = false;
}