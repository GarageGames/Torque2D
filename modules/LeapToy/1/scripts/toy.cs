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

function LeapToy::createBackground( %this )
{    
    // Create the sprite.
    %background = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %background.setBodyType( static );
       
    // Set the position.
    %background.Position = "0 0";

    // Set the size.        
    %background.Size = "40 30";
    
    // Set to the furthest background layer.
    %background.SceneLayer = 31;
    
    // Set an image.
    %background.Image = "LeapToy:menuBackground";

    %background.createEdgeCollisionShape( -20, -15, -20, 15 );
    %background.createEdgeCollisionShape( 20, -15, 20, 15 );
    %background.createEdgeCollisionShape( -20, 15, 20, 15 );

    // Add the sprite to the scene.
    SandboxScene.add( %background );
    
    %this.createCircles();
}

//-----------------------------------------------------------------------------

function LeapToy::createCircles( %this )
{
    // Create the sprite.
    %circleOne = new Sprite();
    %circleTwo = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %circleOne.BodyType = "Kinematic";
    %circleTwo.BodyType = "Kinematic";
       
    // Set the position.
    %circleOne.Position = "15 10";
    %circleTwo.Position = "15 10";
    
    // Set the size.        
    %circleOne.Size = "15 15";
    %circleTwo.Size = "15 15";
    
    // Set to the furthest background layer.
    %circleOne.SceneLayer = 30;
    %circleTwo.SceneLayer = 30;
    
    // Set an image.
    %circleOne.Image = "LeapToy:complexCircle";
    %circleTwo.Image = "LeapToy:simpleCircle";
    
    %circleOne.AngularVelocity = 15;
    %circleTwo.AngularVelocity = -15;
    
    // Add the sprite to the scene.
    SandboxScene.add( %circleOne );
    SandboxScene.add( %circleTwo );
}

//-----------------------------------------------------------------------------

function LeapToy::createGround( %this )
{
    // Create the ground
    %ground = new Scroller();
    %ground.setBodyType("static");
    %ground.Image = "LeapToy:window";
    %ground.setPosition(0, -12);
    %ground.SceneLayer = 11;
    %ground.setSize(LeapToy.GroundWidth, 6);
    %ground.setRepeatX(LeapToy.GroundWidth / 40);   
    %ground.createEdgeCollisionShape(LeapToy.GroundWidth/-2, 3, LeapToy.GroundWidth/2, 3);
    SandboxScene.add(%ground);    
}

//-----------------------------------------------------------------------------
// This will be called when the user presses the spacebar
//
// %val - Will be true if the event is down, false if it was released
function LeapToy::pickSprite( %this, %val )
{
    // Find out where the cursor is located, then convert to world coordinates
    %cursorPosition = Canvas.getCursorPos();
    %worldPosition = SandboxWindow.getWorldPoint(%cursorPosition);
    
    // If true, force an onTouchDown for the Sandbox input listener
    // If false, force an onTouchUp for the Sandbox input listener    
    if (%val)
    {
        Sandbox.InputController.onTouchDown(0, %worldPosition);
    }
    else
    {
        Sandbox.InputController.onTouchUp(0, %worldPosition);
    }
}