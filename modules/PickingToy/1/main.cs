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

function PickingToy::create( %this )
{
    // Configure the toy.
    PickingToy.PickMode = "Any";
    PickingToy.NotPickedAlpha = 0.25;

    // Add the configuration options.
    addSelectionOption( "Any,Size,Collision", "Pick Mode", 3, "setPickMode", false, "Selects the picking mode." );

    // Reset the toy.
    PickingToy.reset();
}


//-----------------------------------------------------------------------------

function PickingToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function PickingToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
       
    // Create some sprites.
    %this.createSprites();   
}

//-----------------------------------------------------------------------------

function PickingToy::createBackground( %this )
{    
    // Create the sprite.
    %object = SandboxScene.create( Sprite );
    %object.BodyType = static;
    %object.Position = "0 0";
    %object.Size = "100 75";
    %object.SceneLayer = 31;
    %object.Image = "ToyAssets:highlightBackground";
    %object.BlendColor = SlateGray;
}

//-----------------------------------------------------------------------------

function PickingToy::createSprites( %this )
{    
    // Create the sprite.
    %object = SandboxScene.create( Sprite );
    %object.Size = 40;
    %object.Angle = -30;
    %object.Image = "ToyAssets:Tiles";
    %object.Frame = 0;
    %object.setBlendAlpha( PickingToy.NotPickedAlpha );
    
    // Set the target object.
    PickingToy.TargetObject = %object;

    // Create some collision shapes.    
    %object.createCircleCollisionShape( 10, "-20 -20" );
    %object.createPolygonBoxCollisionShape( "20 20", "20 20" );
}

//-----------------------------------------------------------------------------

function PickingToy::onTouchMoved(%this, %touchID, %worldPosition)
{
    // Pick at position.
    %picked = SandboxScene.pickPoint( %worldPosition, "", "", PickingToy.PickMode );
    
    // Fetch pick count.
    %pickCount = %picked.Count;
    
    // See if the target object is amongst those picked.
    for( %i = 0; %i < %pickCount; %i++ )
    {
        // If this is the target object then make it opaque.
        if ( getWord( %picked, %i ) == PickingToy.TargetObject )
        {
            PickingToy.TargetObject.setBlendAlpha( 1.0 );
            return;
        }
    }
    
    // Target not picked so make it transparent.
    PickingToy.TargetObject.setBlendAlpha( 0.25 );
}

//-----------------------------------------------------------------------------

function PickingToy::setPickMode( %this, %value )
{
    PickingToy.PickMode = %value;
}