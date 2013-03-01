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

function AudioBasicsToy::create( %this )
{
    // Activate mouse input.    
    $enableDirectInput = true;
    activateDirectInput();    

    // Turn on input events for scene objects.
    SandboxWindow.setUseObjectInputEvents(true);
    
    // Setup default
    AudioBasicsToy.Option = "ToyAssets:titleMusic";
    
    // Sandbox options
    addSelectionOption( "ToyAssets:titleMusic,ToyAssets:winMusic", "Background Music", 2, "setupMusic", true, "Choose a music file" );
    
    // Reset the toy initially.
    AudioBasicsToy.reset();        
}

//-----------------------------------------------------------------------------

function AudioBasicsToy::destroy( %this )
{
    alxStopAll();
}

//-----------------------------------------------------------------------------

function AudioBasicsToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Stop all music
    alxStopAll();
    
    // Start game scripts here.
    %this.createBackground();
    %this.createButtons();
    %this.createText();
    %this.createTrap();
}

//-----------------------------------------------------------------------------

function AudioBasicsToy::setupMusic(%this, %value)
{
   %this.Option = %value;
}

//-----------------------------------------------------------------------------

function AudioBasicsToy::createBackground(%this)
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType(static);
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set the scroller to use an animation!
    %object.Image = "ToyAssets:highlightBackground";
            
    // Add the sprite to the scene.
    SandboxScene.add(%object);    
}

function AudioBasicsToy::createButtons(%this)
{
   // Create the buttons and configure.
    %object1 = new Sprite() {class = "ButtonClass";};
    %object1.type = "play";
    %object1.setBodyType(static);
    %object1.Position = "-20 20";
    %object1.Size = "15";
    %object1.SceneLayer = 5;
    %object1.Image = "AudioBasicsToy:PlayButton";
    %object1.setUseInputEvents(true);
    
    %object2 = new Sprite() {class = "ButtonClass";};
    %object2.type = "pause";
    %object2.setBodyType(static);
    %object2.Position = "0 20";       
    %object2.Size = "15";
    %object2.SceneLayer = 5;
    %object2.Image = "AudioBasicsToy:PauseButton";
    %object2.setUseInputEvents(true);
    
    %object3 = new Sprite() {class = "ButtonClass";};
    %object3.type = "stop";
    %object3.setBodyType(static);
    %object3.Position = "20 20";       
    %object3.Size = "15";
    %object3.SceneLayer = 5;
    %object3.Image = "AudioBasicsToy:StopButton";
    %object3.setUseInputEvents(true);
    
    %object4 = new Sprite() {class = "ButtonClass";};
    %object4.type = "sfxPlay";
    %object4.setBodyType(static);
    %object4.Position = "20 -2";       
    %object4.Size = "5";
    %object4.SceneLayer = 5;
    %object4.Image = "AudioBasicsToy:PlayButton";
    %object4.setUseInputEvents(true);
    
            
    // Add the sprite to the scene.
    SandboxScene.add(%object1);
    SandboxScene.add(%object2);
    SandboxScene.add(%object3);
    SandboxScene.add(%object4);
}

function AudioBasicsToy::createText(%this)
{
   // Create the image fonts.
    %text1 = new ImageFont();
    %text2 = new ImageFont();
    
    // Set the image font to use the font image asset.
    %text1.Image = "ToyAssets:Font";
    %text2.Image = "ToyAssets:Font";
    
    // Position it above the buttons.
    %text1.Position = "0 32";
    %text2.Position = "-3 -2";
   
    // Set the font size in both axis.  This is in world-units and not typicaly font "points".
    %text1.FontSize = "2 2";
    %text2.FontSize = "2 2";

    // Set the text alignment.
    %text1.TextAlignment = "Center";
    %text2.TextAlignment = "Center";

    // Set the text to display.
    %text1.Text = "Background Music Controls";
    %text2.Text = "Sound Effect Demo";
    
    // Add the fonts to the scene.
    SandboxScene.add(%text1);
    SandboxScene.add(%text2);
}

function AudioBasicsToy::createTrap(%this)
{
   // Trap trigger
    %trap = new SceneObject() {class = "TrapClass";};
    
    %trap.setSize(10, 10);
    %trap.setPosition(-10, -15);
    %trap.createPolygonBoxCollisionShape(10, 10);        
    %trap.setAwake(true);
    %trap.setActive(true);
    %trap.setCollisionCallback(true);
    %trap.setBodyType("static");
    %trap.setCollisionShapeIsSensor(0, true);
    SandboxScene.add(%trap);
}

function AudioBasicsToy::createAnimatedSprite(%this)
{
   // Create the sprite.
    %object = new Sprite() {class = "KnightClass";};

    // Set the position.
    %object.Position = "50 -15";
        
    // If the size is to be square then we can simply pass a single value.
    // This applies to any 'Vector2' engine type.
    %object.Size = 8;
    
    // Set the sprite to use an animation.  This is known as "animated" image mode.
    %object.Animation = "ToyAssets:TD_Knight_MoveWest";
    
    // Setup collision properties.
    %object.createPolygonBoxCollisionShape(7, 8);
    %object.setCollisionCallback(true);
       
    // Add the sprite to the scene.
    SandboxScene.add(%object);
    
    // Give the sprite a velocity.
    %object.setLinearVelocityX(-12);
}

function ButtonClass::onTouchDown(%this, %touchID, %worldPos)
{
   switch$(%this.type)
   {
      case "play":
      if (!alxIsPlaying(AudioBasicsToy.Music))
      {
         AudioBasicsToy.Music = alxPlay(AudioBasicsToy.Option);
         echo("Assigned ID is" SPC AudioBasicsToy.Music);
      }
      
      case "pause":
      if (alxIsPlaying(AudioBasicsToy.Music))
      {
         echo("pause" SPC AudioBasicsToy.Music);
         alxPause(AudioBasicsToy.Music);
      }else
      {
         echo("unpause" SPC AudioBasicsToy.Music);
         alxUnpause(AudioBasicsToy.Music);
      }
      
      case "stop":
      alxStop(AudioBasicsToy.Music);
      
      case "sfxPlay":
      AudioBasicsToy.createAnimatedSprite();
   }
}

function TrapClass::handleCollision(%this, %object, %collisionDetails)
{
   // Create explosion
   %anim = new Sprite() {class = "KnightClass";};
   %anim.Position = "0 -16";
   %anim.Size = 13;
   %anim.Animation = "ToyAssets:Impact_ExplosionAnimation";
   SandboxScene.add(%anim);
   %anim.checkAnimation();
   
   // Play sound effect
   %sound = alxPlay("ToyAssets:KnightDeathSound");
   
   // Stop movement
   %object.setLinearVelocityX(0);
   %object.setActive(false);
   
   // Change object animation
   %object.Animation = "ToyAssets:TD_Knight_Death";
   %object.checkAnimation();
}

function KnightClass::checkAnimation(%this)
{
   if (%this.getIsAnimationFinished())
   {
      %this.setEnabled(false);
      %this.safeDelete();
   }else
   {
      %this.schedule(50, "checkAnimation");
   }
}