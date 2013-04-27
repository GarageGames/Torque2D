/*******************************************************************************
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

// This is called when the module is loaded
function SpineToy::create(%this)
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );

    // Set the manipulation mode.
    Sandbox.useManipulation( pan );

    %this.asset = "SpineToy:TestSkeleton";
    %this.skin = "default";
    
    addSelectionOption( "SpineToy:TestSkeleton,SpineToy:goblins", "Select Skeleton", 4, "setSkeleton", true, "Picks the skeleton asset for the object." );
    addSelectionOption( "default,goblin,goblingirl", "Select Skin", 4, "setSkin", true, "Sets the skin for the skeleton object." );
    
    // Reset the toy.
    SpineToy.reset();
}

// This is called when the module is destroyed
function SpineToy::destroy(%this)
{
}

function SpineToy::setSkeleton(%this, %value)
{
    %this.asset = %value;
    if (%value $= "SpineToy:TestSkeleton")
        %this.setSkin("default");
    else
        %this.setSkin("goblin");
}

function SpineToy::setSkin(%this, %value)
{
    %this.skin = %value;
}

// This can be used to reset the state of a module,
// without reloading it entirely
function SpineToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();

    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );
    
    %this.createBackground();
    %this.createGround();
    
    // Create the skeleton object
    %spineSkeletonObject = new SkeletonObject();
    
    // Assign it an asset
    %spineSkeletonObject.Asset = %this.asset;
    %spineSkeletonObject.Skin = %this.skin;
    
    // Set the animation name
    %spineSkeletonObject.Animation = "walk";
    
    %spineSkeletonObject.position = "0 -9";
        
    // Add it to the scene
    SandboxScene.add(%spineSkeletonObject);
}

// Create a background.
function SpineToy::createBackground()
{
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "40 30";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:jungleSky";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );
}

function SpineToy::createGround( %this )
{
    // Create the ground
    %ground = new Scroller();
    %ground.setBodyType("static");
    %ground.Image = "ToyAssets:dirtGround";
    %ground.setPosition(0, -12);
    %ground.setSize(40, 6);
    %ground.setRepeatX(40 / 60);   
    %ground.createEdgeCollisionShape(40/-2, 3, 40/2, 3);
    SandboxScene.add(%ground);  
    
    // Create the grass.
    %grass = new Sprite();
    %grass.setBodyType("static");
    %grass.Image = "ToyAssets:grassForeground";
    %grass.setPosition(0, -8.5);
    %grass.setSize(40, 2); 
    SandboxScene.add(%grass);       
}