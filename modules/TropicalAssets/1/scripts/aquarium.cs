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

function getFishAnimationList()
{
   %list = "TropicalAssets:angelfish1Anim" @ "," @ "TropicalAssets:angelfish2Anim" @ "," @ "TropicalAssets:butterflyfishAnim";
   %list = %list @ "," @ "TropicalAssets:pufferfishAnim" @ "," @ "TropicalAssets:rockfishAnim" @ "," @ "TropicalAssets:seahorseAnim";
   %list = %list @ "," @ "TropicalAssets:triggerfish1Anim" @ "," @ "TropicalAssets:eelAnim";
}

//-----------------------------------------------------------------------------

function getFishSize(%anim)
{
    switch$(%anim)
    {
        case "TropicalAssets:angelfish1Anim":
        %fishInfo = "15 15";

        case "TropicalAssets:angelfish2Anim":
        %fishInfo = "15 15";
        
        case "TropicalAssets:butterflyfishAnim":
        %fishInfo = "15 15";
        
        case "TropicalAssets:pufferfishAnim":
        %fishInfo = "15 15";
        
        case "TropicalAssets:rockfishAnim":
        %fishInfo = "15 7.5";
        
        case "TropicalAssets:seahorseAnim":
        %fishInfo = "7.5 15";
        
        case "TropicalAssets:triggerfish1Anim":
        %fishInfo = "15 15";

        case "TropicalAssets:eelAnim":
        %fishInfo = "7.5 3.75";
    }

    return %fishInfo;
}

//-----------------------------------------------------------------------------

function buildAquarium(%scene)
{
    // A pre-built aquarium of size 100x75, with blue water, some haze, and some nice rocks.
    // Triggers will be provide around the edges, kind of like an electric fence,
    // with a call to addAquariumBoundaries.

    // Background
    %background = new Sprite();
    %background.setBodyType( "static" );
    %background.setImage( "TropicalAssets:background" );
    %background.setSize( 100, 75 );
    %background.setCollisionSuppress();
    %background.setAwake( false );
    %background.setActive( false );
    %background.setSceneLayer(30);
    %scene.add( %background );
    
    // Far rocks
    %farRocks = new Sprite();
    %farRocks.setBodyType( "static" );
    %farRocks.setPosition( 0, -7.5 );
    %farRocks.setImage( "TropicalAssets:rocksfar" );
    %farRocks.setSize( 100, 75 );
    %farRocks.setCollisionSuppress();
    %farRocks.setAwake( false );
    %farRocks.setActive( false );
    %farRocks.setSceneLayer(20);
    %scene.add( %farRocks );
    
    // Near rocks
    %nearRocks = new Sprite();
    %nearRocks.setBodyType( "static" );
    %nearRocks.setPosition( 0, -8.5 );
    %nearRocks.setImage( "TropicalAssets:rocksnear" );
    %nearRocks.setSize( 100, 75 );
    %nearRocks.setCollisionSuppress();
    %nearRocks.setAwake( false );
    %nearRocks.setActive( false );
    %nearRocks.setSceneLayer(10);
    %scene.add( %nearRocks );
    
    addAquariumBoundaries( %scene, 100, 75 );
}

function addAquariumBoundaries(%scene, %width, %height)
{
    // add boundaries on all sides of the aquarium a bit outside of the border of the tank.
    // The triggers allow for onCollision to be sent to any fish or other object that touch the edges.
    // The triggers are far enough outside the tank so that objects will most likely be just out of view
    // before they are sent the onCollision callback.  This will they can adjust "off stage".

    // Calculate a width and height to use for the bounds.
    // They should be bigger than the aquarium itself.
    %wrapWidth = %width * 1.5;
    %wrapHeight = %height * 1.5;

    // Left trigger
    %leftTrigger = new SceneObject() { class = "AquariumBoundary"; };
    
    %leftTrigger.side = "left";
    // make the bound as tall as the tank
    %leftTrigger.setSize( 5, %wrapHeight );
    // put the bounds 50% further out than the edge of the tank
    %leftTrigger.setPosition( -%wrapWidth/2, 0 );
    %leftTrigger.setSceneLayer( 1 );
    %leftTrigger.createPolygonBoxCollisionShape( 5, %wrapHeight );
    %leftTrigger.setDefaultDensity( 1 );
    %leftTrigger.setDefaultFriction( 1.0 );        
    %leftTrigger.setAwake( true );
    %leftTrigger.setActive( true );
    // the objects that collide with us should handle any callbacks.
    // remember to set those scene objects to collide with scene group 15 (which is our group)!
    %leftTrigger.setSceneGroup( 15 );
    %leftTrigger.setCollisionCallback(false);
    %leftTrigger.setBodyType( "static" );
    %leftTrigger.setCollisionShapeIsSensor(0, true);
    %scene.add( %leftTrigger );
    
    // Right trigger
    %rightTrigger = new SceneObject() { class = "AquariumBoundary"; };
    
    // make the bound as tall as the tank
    %rightTrigger.setSize( 5, %wrapHeight );
    %rightTrigger.side = "right";
    // put the bounds 50% further out than the edge of the tank
    %rightTrigger.setPosition( %wrapWidth/2, 0 );
    %rightTrigger.setSceneLayer( 1 );
    %rightTrigger.createPolygonBoxCollisionShape( 5, %wrapHeight );
    %rightTrigger.setDefaultDensity( 1 );
    %rightTrigger.setDefaultFriction( 1.0 );    
    %rightTrigger.setAwake( true );
    %rightTrigger.setActive( true );
    // the objects that collide with us should handle any callbacks.
    // remember to set those scene objects to collide with scene group 15 (which is our group)!
    %rightTrigger.setSceneGroup( 15 );
    %rightTrigger.setCollisionCallback(false);
    %rightTrigger.setBodyType( "static" );
    %rightTrigger.setCollisionShapeIsSensor(0, true);
    %scene.add( %rightTrigger );    

    // Left trigger
    %topTrigger = new SceneObject() { class = "AquariumBoundary"; };
    
    %topTrigger.side = "top";
    // make the bound as wide as the tank
    %topTrigger.setSize( %wrapWidth, 5 );
    // put the bounds 50% further out than the edge of the tank
    %topTrigger.setPosition( 0, -%wrapHeight/2 );
    %topTrigger.setSceneLayer( 1 );
    %topTrigger.createPolygonBoxCollisionShape( %wrapWidth, 5 );
    %topTrigger.setDefaultDensity( 1 );
    %topTrigger.setDefaultFriction( 1.0 );        
    %topTrigger.setAwake( true );
    %topTrigger.setActive( true );
    // the objects that collide with us should handle any callbacks.
    // remember to set those scene objects to collide with scene group 15 (which is our group)!
    %topTrigger.setSceneGroup( 15 );
    %topTrigger.setCollisionCallback(false);
    %topTrigger.setBodyType( "static" );
    %topTrigger.setCollisionShapeIsSensor(0, true);
    %scene.add( %topTrigger );
    
    // Right trigger
    %bottomTrigger = new SceneObject() { class = "AquariumBoundary"; };
    
    // make the bound as wide as the tank
    %bottomTrigger.setSize( %wrapWidth, 5 );
    %bottomTrigger.side = "bottom";
    // put the bounds 50% further out than the edge of the tank
    %bottomTrigger.setPosition( 0, %wrapHeight/2 );
    %bottomTrigger.setSceneLayer( 1 );
    %bottomTrigger.createPolygonBoxCollisionShape( %wrapWidth, 5 );
    %bottomTrigger.setDefaultDensity( 1 );
    %bottomTrigger.setDefaultFriction( 1.0 );    
    %bottomTrigger.setAwake( true );
    %bottomTrigger.setActive( true );
    // the objects that collide with us should handle any callbacks.
    // remember to set those scene objects to collide with scene group 15 (which is our group)!
    %bottomTrigger.setSceneGroup( 15 );
    %bottomTrigger.setCollisionCallback(false);
    %bottomTrigger.setBodyType( "static" );
    %bottomTrigger.setCollisionShapeIsSensor(0, true);
    %scene.add( %bottomTrigger );
}

//-----------------------------------------------------------------------------

function createAquariumEffects(%scene)
{
    %obj = new Scroller();
    %obj.setBodyType( "static" );
    %obj.setImage( "TropicalAssets:wave" );
    %obj.setPosition( 0, 0 );
    %obj.setScrollX(2);
    %obj.setSize( 100, 75 );
    %obj.setRepeatX( 0.2 );   
    %obj.setSceneLayer( 0 );
    %obj.setSceneGroup( 0 );
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    %scene.add( %obj );
    
    // Add the caustics particle.
    %caustics = new ParticlePlayer();
    %caustics.Particle = "TropicalAssets:Caustics";
    %scene.add( %caustics ); 
}

//-----------------------------------------------------------------------------
