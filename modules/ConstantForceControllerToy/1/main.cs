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

function ConstantForceControllerToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Configure settings.
    ConstantForceControllerToy.ForceAngle = 45;
    ConstantForceControllerToy.ForceMagnitude = 50;
    ConstantForceControllerToy.DebrisCount = 100;

    // Add options.    
    addNumericOption("Force Angle", -359, 359, 1, "setForceAngle", ConstantForceControllerToy.ForceAngle, false, "The angle of the constant force.");   
    addNumericOption("Force Magnitude", 0, 1000, 10, "setForceMagnitude", ConstantForceControllerToy.ForceMagnitude, false, "The magnitude of the constant force.");   
    addNumericOption("Debris Count", 10, 1000, 10, "setDebrisCount", ConstantForceControllerToy.DebrisCount, true, "The amount of debris affected by the constant force controller.");
    
    // Reset the toy.
    ConstantForceControllerToy.reset();
}


//-----------------------------------------------------------------------------

function ConstantForceControllerToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
    
    // Create constant force controller.
    %this.createConstantForceController();
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set the scroller to use an animation!
    %object.Image = "ToyAssets:highlightBackground";
    
    // Set the blend color.
    %object.BlendColor = LightBlue;
    
    // Create border collisions.
    %object.createEdgeCollisionShape( -50, -37.5, -50, 37.5 );
    %object.createEdgeCollisionShape( 50, -37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, 37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, -34.5, 50, -34.5 );
    
           
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::createSprite( %this, %asset, %position, %size, %angle, %blendColor )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the position.
    %object.Position = %position;

    // Set the size.        
    %object.Size = %size;
    
    // Set the angle.
    %object.Angle = %angle;
       
    // Set the scroller to use an animation!
    %object.Image = %asset;
    
    // Set the blend color.
    %object.BlendColor = %blendColor $= "" ? White : %blendColor;
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
    
    return %object;
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::createConstantForceController( %this )
{
    // Create a new controller.
    %controller = new ConstantForceController();
    
    // Set scene controller.
    ConstantForceControllerToy.SceneController = %controller;

    // Update the constant force controller.
    %this.updateConstantForceController();
    
    // Add the controller.
    SandboxScene.Controllers.add( %controller );
   
    // Create some sprites used by the controller.
    for( %n = 0; %n < ConstantForceControllerToy.DebrisCount; %n++ )
    {    
        %sizeX = getRandom(1,4);
        %sizeY = getRandom(1,4);
        %size = %sizeX SPC %sizeY;
        
        // Create some sprites.
        %sprite = %this.createSprite( "ToyAssets:tiles", getRandom(-40,40) SPC getRandom(-20,20), %size, getRandom(0,360), White );
        %sprite.Frame = getRandom(0,15);
        %sprite.createPolygonBoxCollisionShape( %sizeX, %sizeY );
        %sprite.setAngularVelocity(getRandom(-180,180));
        
        // Add to the controller.
        %controller.add( %sprite );
    }   
    
    // Create some sprites NOT used by the controller.
    for( %n = 0; %n < 10; %n++ )
    {    
        %size = getRandom(3,4);
        
        // Create some sprites.
        %sprite = %this.createSprite( "ToyAssets:football", getRandom(-40,40) SPC getRandom(-20,20), %size, getRandom(0,360), White );
        %sprite.createCircleCollisionShape( %size * 0.5 );
        %sprite.setAngularVelocity(getRandom(-180,180));
    }
    
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::updateConstantForceController( %this )
{
    // Calculate the force.
    ConstantForceControllerToy.SceneController.Force = Vector2Direction( %this.ForceAngle, %this.ForceMagnitude );
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::setForceAngle(%this, %value)
{
    %this.ForceAngle = %value;
    
    // Update the constant force.
    %this.updateConstantForceController();   
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::setForceMagnitude(%this, %value)
{
    %this.ForceMagnitude = %value;
    
    // Update the constant force.
    %this.updateConstantForceController();    
}

//-----------------------------------------------------------------------------

function ConstantForceControllerToy::setDebrisCount(%this, %value)
{
    %this.DebrisCount = %value;
}
