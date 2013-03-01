///-----------------------------------------------------------------------------
/// startButton class for the title screen.
///-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// onTouchDown callback for our main menu start button.
function startButton::onTouchDown(%this, %modifier, %worldPosition, %clicks)
{
   // Change the image to the down image so it looks pressed visually
   %this.setImage("PuzzleToy:PlayBtnDown", 0);
   // If the sound is enabled, play the select option sound
   if (PuzzleToy.soundEnabled)
      alxPlay("PuzzleToy:SelectOptionSound");
      
   // Call our custom loadLevel function to schedule the load.
   // This is so we can load the loading scene then the intended scene
   PuzzleToy.loadLevel( "./levels/level1_1.scene.taml", true );
}




