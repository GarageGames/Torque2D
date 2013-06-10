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

    %this.asset = "SpineToy:goblins";
    %this.skin = "goblin";
    %this.animation = "walk";
    
    addSelectionOption( "goblin,goblingirl", "Select Skin", 4, "setSkin", false, "Sets the skin for the skeleton object." );
    
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
    
    if (%value $= "SpineToy:goblins")
        %this.setSkin("goblin");
    else
        %this.setSkin("default");
}

function SpineToy::setSkin(%this, %value)
{
    %this.skin = %value;
    %this.walker.Skin = %this.skin;
}

// This can be used to reset the state of a module,
// without reloading it entirely
function SpineToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();

    if (%this.resetSchedule !$= "")
        cancel(%this.resetSchedule);
        
    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );

/*
// Uncomment this block to test mixing (WIP)
    // Create the skeleton object
    %object = new SkeletonObject()
    {
        class = "SpineBoy";
    };

    %object.Asset = "SpineToy:TestSkeleton";
    %object.setAnimation("walk", true);
    %object.SkeletonScale = 0.025;

    %object.SceneLayer = 29;
    %object.Position = 0;
    %object.setMix("walk", "jump", 0.2);
    %object.setMix("jump", "walk", 0.4);

    SandboxScene.add(%object);

    %object.schedule(4000, "doJump");

return;
*/

    %this.createBackground();
    //%this.createGround();
    
    // Create the skeleton object
    %spineSkeletonObject = new SkeletonObject();
    
    // Assign it an asset
    %spineSkeletonObject.Asset = %this.asset;
    %spineSkeletonObject.Skin = %this.skin;
        
    // Set the animation name
    %spineSkeletonObject.setAnimation(%this.animation, true);
    
    %spineSkeletonObject.SkeletonScale = 0.025;
    %spineSkeletonObject.setSkeletonOffset(0, -5);
    
    %spineSkeletonObject.position = "-25 -8";
    %spineSkeletonObject.SceneLayer = 29;
    %spineSkeletonObject.setLinearVelocity(7.5, 0);
        
    %this.walker = %spineSkeletonObject;
    
    %this.resetSchedule = %this.schedule(8000, resetWalker);

    // Add it to the scene
    SandboxScene.add(%spineSkeletonObject);
}

function SpineBoy::onAnimationFinished(%this, %animationName)
{
    if (%animationName $= "jump")
    {
        %this.setAnimation("walk", true);
        %this.schedule(4000, "doJump");
    }
}

function SpineBoy::doJump(%this)
{
    %this.setAnimation("jump", false);
}

function SpineToy::resetWalker(%this)
{
    %this.walker.setPosition("-25 -8");
    %this.resetSchedule = %this.schedule(8000, resetWalker);
}

// Create a background.
function SpineToy::createBackground(%this)
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
    %object.Image = "SpineToy:background";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );
    
    %this.createPowerup(-14, 6);
    %this.createPowerup(14, 6);
    
    // Create the skeleton object
    %animatedMenu = new SkeletonObject();
    
    // Assign it an asset
    %animatedMenu.Asset = "SpineToy:spinosaurus";

    // Set properties    
    %animatedMenu.setAnimation("Animation", false);
    %duration = %animatedMenu.getAnimationDuration();
    %animatedMenu.schedule(%duration*1000, "setAnimation", "loop", true);
    %animatedMenu.position = "0 0";
    %animatedMenu.SceneLayer = 30;
    %animatedMenu.SkeletonScale = 0.025;
    %animatedMenu.setSkeletonOffset(0, -10);
    
    // Add it to the scene
    SandboxScene.add(%animatedMenu);
}

function SpineToy::createPowerup(%this, %xPos, %yPos)
{
    // Create the skeleton object
    %powerup = new SkeletonObject();
    
    // Assign it an asset
    %powerup.Asset = "SpineToy:powerup";

    // Set properties    
    %powerup.setAnimation("Animation", true);    
    %powerup.position = %xPos SPC %yPos;
    %powerup.SceneLayer = 30;
    
    %powerup.SkeletonScale = 0.025;
    %powerup.setSkeletonOffset(0, -5);
    
    // Add it to the scene
    SandboxScene.add(%powerup);
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
    %ground.SceneLayer = 30;
    SandboxScene.add(%ground);  
    
    // Create the grass.
    %grass = new Sprite();
    %grass.setBodyType("static");
    %grass.Image = "ToyAssets:grassForeground";
    %grass.setPosition(0, -8.5);
    %grass.setSize(40, 2); 
    %grass.SceneLayer = 28;
    SandboxScene.add(%grass);       
}