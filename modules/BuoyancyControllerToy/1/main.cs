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

function BuoyancyControllerToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Set gravity.
    SandboxScene.setGravity( 0, -29.8 );
    
    // Configure settings.
    BuoyancyControllerToy.SurfaceNormal = "0 1";
    BuoyancyControllerToy.SurfaceOffset = 0;
    BuoyancyControllerToy.FluidDensity = 2.5;
    BuoyancyControllerToy.LinearDrag = 3;
    BuoyancyControllerToy.AngularDrag = 3;
    BuoyancyControllerToy.FluidGravity = "0 -29.8";    
    BuoyancyControllerToy.UseShapeDensity = false;

    BuoyancyControllerToy.FlowAngle = 90;
    BuoyancyControllerToy.FlowMagnitude = 0;
    
    BuoyancyControllerToy.MaxDebrisCount = 50;
    BuoyancyControllerToy.MinDebrisDensity = 1;
    BuoyancyControllerToy.MaxDebrisDensity = 1;

    // Add options.    
    addNumericOption("Surface Offset", -35, 35, 1, "setSurfaceOffset", BuoyancyControllerToy.SurfaceOffset, false, "The height of the fluid surface along the normal.");
    addNumericOption("Fluid Density", 0, 10, 0.1, "setFluidDensity", BuoyancyControllerToy.FluidDensity, false, "The fluid density.");   
    addNumericOption("Fluid Flow Angle", -359, 359, 1, "setFluidFlowAngle", BuoyancyControllerToy.FlowAngle, true, "The angle of the constant force.");   
    addNumericOption("Fluid Flow Magnitude", 0, 1000, 10, "setFluidFlowMagnitude", BuoyancyControllerToy.FlowMagnitude, true, "The magnitude of the constant force.");      
    addNumericOption("Linear Drag", 0, 10, 0.1, "setLinearDrag", BuoyancyControllerToy.LinearDrag, false, "The linear drag co-efficient for the fluid.");
    addNumericOption("Angular Drag", 0, 10, 0.1, "setAngularDrag", BuoyancyControllerToy.AngularDrag, false, "The angular drag co-efficient for the fluid.");
    addNumericOption("Maximum Debris Count", 1, 500, 1, "setMaxDebrisCount", BuoyancyControllerToy.MaxDebrisCount, true, "The maximum number of debris to create.");
    addNumericOption("Minimum Debris Density", 0, 10, 0.1, "setMinDebrisDensity", BuoyancyControllerToy.MinDebrisDensity, true, "The minimum density to randomly choose for the debris.");
    addNumericOption("Maximum Debris Density", 0, 10, 0.1, "setMaxDebrisDensity", BuoyancyControllerToy.MaxDebrisDensity, true, "The maximum density to randomly choose for the debris.");   
    addFlagOption("Use Collision Shape Densities", "setUseShapeDensity", BuoyancyControllerToy.UseShapeDensity, false, "Whether to use the collision shape densities or assume a uniform density." );
    
    // Reset the toy.
    BuoyancyControllerToy.reset();
}


//-----------------------------------------------------------------------------

function BuoyancyControllerToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();

    // Build the aquarium.       
    %this.buildAquarium();
    
    // Create the aquarium effects.
    %this.createAquariumEffects();
           
    // Create background.
    %this.createBackground();
    
    // Create buoyancy controller.
    %this.createBuoyancyController();
    
    // Create the water overlay.
    %this.createWaterOverlay();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::buildAquarium(%this)
{
    // Background
    %background = new Sprite();
    %background.setBodyType( "static" );
    %background.setImage( "BuoyancyControllerToy:background" );
    %background.setPosition( 0, -18.75 );
    %background.setSize( 100, 37.5 );
    %background.setCollisionSuppress();
    %background.setAwake( false );
    %background.setActive( false );
    %background.setSceneLayer(5);
    SandboxScene.add( %background );
    
    // Far rocks
    %farRocks = new Sprite();
    %farRocks.setBodyType( "static" );
    %farRocks.setImage( "BuoyancyControllerToy:rocksfar" );
    %farRocks.setPosition( 0, -18.75 );
    %farRocks.setSize( 100, 37.5 );
    %farRocks.setCollisionSuppress();
    %farRocks.setAwake( false );
    %farRocks.setActive( false );
    %farRocks.setSceneLayer(4);
    SandboxScene.add( %farRocks );
    
    // Near rocks
    %nearRocks = new Sprite();
    %nearRocks.setBodyType( "static" );
    %nearRocks.setPosition( 0, -18.75 );
    %nearRocks.setImage( "BuoyancyControllerToy:rocksnear" );
    %nearRocks.setSize( 100, 37.5 );
    %nearRocks.setCollisionSuppress();
    %nearRocks.setAwake( false );
    %nearRocks.setActive( false );
    %nearRocks.setSceneLayer(3);
    SandboxScene.add( %nearRocks ); 
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createAquariumEffects(%this)
{
    %obj = new Scroller();
    %obj.setBodyType( "static" );
    %obj.setImage( "BuoyancyControllerToy:wave" );
    %obj.setPosition( 0, 0 );
    %obj.setScrollX(2);
    %obj.setPosition( 0, -18.75 );
    %obj.setSize( 100, 37.5 );
    %obj.setRepeatX( 0.2 );   
    %obj.setSceneLayer( 0 );
    %obj.setSceneGroup( 0 );
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createWaterOverlay( %this )
{
    // Add the water.
    %water = new Sprite();
    %water.BodyType = static;
    %water.Position = "0 -18.75";
    %water.Size = "100 37.5";
    %water.Image = "ToyAssets:SkyBackground";
    %water.SetBlendAlpha( 0.6 );
    
    SandboxScene.add( %water );      
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createBackground( %this )
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
    %object.Image = "ToyAssets:skyBackground";

    // Flip the sky.
    %object.FlipY = true;
    
    // Set the blend color.
    %object.BlendColor = LightBlue;
    
    // Create border collisions.
    %object.createEdgeCollisionShape( -50, -37.5, -50, 37.5 );
    %object.createEdgeCollisionShape( 50, -37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, -34.5, 50, -34.5 );
        
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createSprite( %this, %asset, %position, %size, %angle, %blendColor )
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

function BuoyancyControllerToy::createBuoyancyController( %this )
{
    // Create a new controller.
    %controller = new BuoyancyController();
    
    // Set scene controller.
    BuoyancyControllerToy.SceneController = %controller;  
    
    // Update the controller.
    %this.updateBuoyancyController();  
    
    // Add the controller.
    SandboxScene.Controllers.add( %controller );
    
    // Fetch the debris count.
    %debrisCount = BuoyancyControllerToy.MaxDebrisCount;

    for( %debris = 0; %debris < %debrisCount; %debris++ )
    {
        // Choose a random debris (polygon or circle shapes).
        if ( getRandom(0,100) < 10 )
        {    
            %size = 3;
            
            // Create some sprites.
            %sprite = %this.createSprite( "ToyAssets:football", getRandom(-40,40) SPC getRandom(50,70), %size, getRandom(0,360), White );
            %sprite.setDefaultFriction( 0.5 );
            %sprite.setDefaultDensity( getRandom(%this.MinDebrisDensity, %this.MaxDebrisDensity) );
            %sprite.createCircleCollisionShape( %size * 0.5 );
            %sprite.setAngularVelocity(getRandom(-180,180));
            
            // Add to the controller.
            %controller.add( %sprite );
        }
        else
        {    
            %sizeX = getRandom(1,10);
            %sizeY = getRandom(1,2);
            %size = %sizeX SPC %sizeY;
            
            // Create some sprites.
            %sprite = %this.createSprite( "ToyAssets:Blocks", getRandom(-40,40) SPC getRandom(50,70), %size, getRandom(0,360), White );
            %sprite.Frame = getRandom( 0, 55 );
            %sprite.setDefaultFriction( 0.5 );
            %sprite.setDefaultDensity( getRandom(%this.MinDebrisDensity, %this.MaxDebrisDensity) );
            %sprite.createPolygonBoxCollisionShape( %sizeX, %sizeY );
            %sprite.setAngularVelocity(getRandom(-180,180));
            
            // Add to the controller.
            %controller.add( %sprite );
        }
    }
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setSurfaceOffset(%this, %value)
{
    %this.SurfaceOffset = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidFlowAngle(%this, %value)
{
    %this.FlowAngle = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidFlowMagnitude(%this, %value)
{
    %this.FlowMagnitude = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidDensity(%this, %value)
{
    %this.FluidDensity = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setLinearDrag(%this, %value)
{
    %this.LinearDrag = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setAngularDrag(%this, %value)
{
    %this.AngularDrag = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidGravity(%this, %value)
{
    %this.FluidGravity = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setUseShapeDensity(%this, %value)
{
    %this.UseShapeDensity = %value;
    
    // Update the controller.
    %this.updateBuoyancyController();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setMaxDebrisCount(%this, %value)
{
    %this.MaxDebrisCount = %value;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setMinDebrisDensity(%this, %value)
{
    %this.MinDebrisDensity = %value;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setMaxDebrisDensity(%this, %value)
{
    %this.MaxDebrisDensity = %value;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::updateBuoyancyController( %this )
{
    // Fetch the buoyancy controller.
    %controller = BuoyancyControllerToy.SceneController;
    
    // Update the controller.
    %controller.SurfaceNormal = BuoyancyControllerToy.SurfaceNormal;
    %controller.SurfaceOffset = BuoyancyControllerToy.SurfaceOffset;
    %controller.FluidDensity = BuoyancyControllerToy.FluidDensity;
    %controller.FlowVelocity = Vector2Direction( %this.FlowAngle, %this.FlowMagnitude );
    %controller.LinearDrag = BuoyancyControllerToy.LinearDrag;
    %controller.AngularDrag = BuoyancyControllerToy.AngularDrag;
    %controller.FluidGravity = BuoyancyControllerToy.FluidGravity;
    %controller.UseShapeDensity = BuoyancyControllerToy.UseShapeDensity;
}
