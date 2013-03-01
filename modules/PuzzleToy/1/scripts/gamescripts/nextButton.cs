///-----------------------------------------------------------------------------
/// next Button class for proceeding to the next level
///-----------------------------------------------------------------------------

/// onTouchDown callback.  This also fires on left mouse down.
function nextButton::onTouchDown(%this, %modifier, %worldPosition, %clicks)
{   
   /// Set the image for the button so it looks pressed
   %this.setImage(%this.downImage);
   
   // If sound is enabled, play the select option sound
   if (PuzzleToy.soundEnabled)
      alxPlay("PuzzleToy:SelectOptionSound");
   
   // Since our scenes only store the simple name of the level, call setSelectedLevel
   // so it can create the full filename.
   PuzzleToy.setSelectedLevel(SandboxScene.NextLevel);   
   
   
}