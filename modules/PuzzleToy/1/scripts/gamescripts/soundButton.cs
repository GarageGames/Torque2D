/// soundButton class. 
/// This class keeps track of 3 objects, the real button and two sprites that 
/// show the current status of the button using separate on and off sprites with
/// only one visible

/// callback when this button is added to a scene
function soundButton::onAdd(%this)
{
   // set up or on and off image visibility based on the current soundEnabled setting.
   if (PuzzleToy.soundEnabled)
   { 
      // Sound is enabled, so set the alpha for the Off image to 0.0.
      // bSoundOff.setBlendAlpha(0.0);
      // Then set the alpha for the On image to 1.0.
      // bSoundOn.setBlendAlpha(1.0);
      %this.setImage(%this.onImage);
   }
   else
   {
      // Sound is disabled, so set the alpha for the Off image to 1.0.
      // bSoundOff.setBlendAlpha(1.0);
      // Then set the alpha for the On image to 0.0.
      // bSoundOn.setBlendAlpha(0.0);
      %this.setImage(%this.offImage);
   }
   // This is a hack right now because my sounds are a bit loud so I set the volume
   // here.   
   alxSetChannelVolume(2, 0.25);
}

/// callback for onTouchDown
/// This will toggle our soundEnabled and update our sprites
function soundButton::onTouchDown(%this, %modifier, %worldPosition, %clicks)
{
   // Check if sound is enabled and toggle appropriately
   if (PuzzleToy.soundEnabled)
   {      
      // Sound was enabled, so set it to disabled
      PuzzleToy.soundEnabled = false;
      // Change our image visibility to match the new setting.
      %this.setImage(%this.offImage);
      // bSoundOff.setBlendAlpha(1.0);
      // bSoundOn.setBlendAlpha(0.0);
      // Stop all sound since our sound should be off now.
      alxStopAll();
   }
   else
   {
      // Sound was disabled, so enable it
      PuzzleToy.soundEnabled = true;
      // Change our image visibility to match the new setting.
      // bSoundOn.setBlendAlpha(1.0);
      // bSoundOff.setBlendAlpha(0.0);
      %this.setImage(%this.onImage);
      // Play the current scenes music
      alxPlay(SandboxScene.MusicAsset);      
   }  
   
}



