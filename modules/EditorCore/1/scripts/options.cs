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

//---------------------------------------------------------------------------------------------
// onWake
// Called by the engine when the options gui is first set.
//---------------------------------------------------------------------------------------------
function OptionsDlg::onWake(%this)
{
   // Fill the graphics drop down menu.
   %buffer = getDisplayDeviceList();
   %count = getFieldCount(%buffer);
   GraphicsDriverMenu.clear();
   
   for(%i = 0; %i < %count; %i++)
      GraphicsDriverMenu.add(getField(%buffer, %i), %i);
      
   // Grab the current graphics driver selection.
   %selId = GraphicsDriverMenu.findText($pref::Video::displayDevice);
   if (%selId == -1)
      %selId = 0;
      
   GraphicsDriverMenu.setSelected(%selId);
   
   // Set audio.
   MusicVolume.setValue($pref::Audio::channelVolume[$musicAudioType]);
   EffectsVolume.setValue($pref::Audio::channelVolume[$effectsAudioType]);
   MusicVolume.oldValue = $pref::Audio::channelVolume[$musicAudioType];
   EffectsVolume.oldValue = $pref::Audio::channelVolume[$effectsAudioType];
   
   // set sleep times
   ForegroundSleep.setValue($Pref::timeManagerProcessInterval);
   BackgroundSleep.setValue($Pref::backgroundSleepTime);
   ForegroundSleep.oldValue = $Pref::timeManagerProcessInterval;
   BackgroundSleep.oldValue = $Pref::backgroundSleepTime;
   
   // Fill the screenshot drop down menu.
   ScreenshotMenu.clear();
   ScreenshotMenu.add("PNG", 0);
   ScreenshotMenu.add("JPEG", 1);
   ScreenshotMenu.setValue($pref::Video::screenShotFormat);
   
   // Set up the keybind options.
   initializeKeybindOptions();
   
}

// [neo, 6/12/2007 - #3222]
function closeOptionsDialog( %save )
{
   // we either save
   if( %save )
   {
      // Save keybindings
      saveCustomEditorBindings();      
   
      // Save audio / video settings 
      applyAVOptions(); 
      
      // Save Scene editor settings
      updateLevelEdOptions();
      
   }
   else // or revert
   {
      // rdbnote: I think keybindings will get reset so don't do it here
      
      // Reset audio / video settings
      revertAVOptionChanges();
      
      // Reset Scene editor settings
      revertLevelEdOptionChanges();
   }
   
   Canvas.popDialog(OptionsDlg);
}


//---------------------------------------------------------------------------------------------
// updateChannelVolume
// Update an audio channels volume.
//---------------------------------------------------------------------------------------------
$AudioTestHandle = 0;
function updateChannelVolume(%channel, %volume)
{
   // Only valid channels are 1-8
   if (%channel < 1 || %channel > 8)
      return;
      
   alxSetChannelVolume(%channel, %volume);
   $pref::Audio::channelVolume[%channel] = %volume;
   
   // Play a test sound for volume feedback.
   if (!alxIsPlaying($AudioTestHandle))
   {
      $AudioTestHandle = alxCreateSource("AudioChannel" @ %channel,
                                         expandPath("~/data/audio/volumeTest.wav"));
      alxPlay($AudioTestHandle);
   }
}

//---------------------------------------------------------------------------------------------
// applyAVOptions
// Apply the AV changes.
//---------------------------------------------------------------------------------------------
function applyAVOptions()
{
   %newDriver = GraphicsDriverMenu.getText();
   $pref::Video::screenShotFormat = ScreenshotMenu.getText();
   
   %res = getRes();
   if (%newDriver !$= $pref::Video::displayDevice)
   {
      // Sort out the menu disappearing by detaching then reattaching popups so
      // that they link/recreate platform dependencies correctly. 
      detachMenuBars();
      
      setDisplayDevice(%newDriver, firstWord(%res), getWord(%res, 1), getWord(%res, 2), isFullScreen());
      
      // Reattach popupmenus to menu bar
      attachMenuBars();
   }

   $Pref::timeManagerProcessInterval = ForegroundSleep.getValue();
   $Pref::backgroundSleepTime = BackgroundSleep.getValue();

   OptionsDlg.onWake();
}

//---------------------------------------------------------------------------------------------
// revertAVOptions
// Revert the AV options to the defaults. Does not apply the changes. Only resets the
// selections.
//---------------------------------------------------------------------------------------------
function revertAVOptions()
{
   // Default graphics driver: OpenGL;
   %selId = GraphicsDriverMenu.findText("OpenGL");
   if (%selId == -1)
      %selId = 0;
      
   GraphicsDriverMenu.setSelected(%selId);
   
   // Default volume: 0.8;
   EffectsVolume.setValue(0.8);
   MusicVolume.setValue(0.8);

   ScreenshotMenu.clear();
   
   // Default screenshot format: PNG;
   ScreenshotMenu.setValue("PNG");
   
   ForegroundSleep.setValue(0);
   BackgroundSleep.setValue(200);
}

//---------------------------------------------------------------------------------------------
// revertAVOptionChanges
// Revert the AV changes made since the options menu was opened - which happen to be the
// values of the related $prefs.
//---------------------------------------------------------------------------------------------
function revertAVOptionChanges()
{
   %selId = GraphicsDriverMenu.findText($pref::Video::displayDevice);
   if (%selId == -1)
      %selId = 0;
      
   GraphicsDriverMenu.setSelected(%selId);
   
   ScreenshotMenu.setValue($pref::video::screenshotFormat);

   EffectsVolume.setValue(EffectsVolume.oldValue);
   MusicVolume.setValue(MusicVolume.oldValue);
   
   ForegroundSleep.setValue(ForegroundSleep.oldValue);
   BackgroundSleep.setValue(BackgrondSleep.oldValue);
}


function MusicVolume::onAction(%this)
{
   updateChannelVolume($musicAudioType, 0.8);
}

function EffectsVolume::onAction(%this)
{
   updateChannelVolume($effectsAudioType, 0.8);
}


function updateiPhoneOptions()
{
   $pref::iPhone::StatusBarHidden = HideStatusBarToggle.getValue();
   $pref::iPhone::StatusBarType = StatusBarTypeMenu.getText();
   $pref::iPhone::ScreenUpsideDown = ScreenUpsideDownToggle.getValue();
   $pref::iPhone::ScreenAutoRotate = ScreenAutoRotateToggle.getValue();
   $pref::iPhone::ScreenOrientation = ScreenOrientationMenu.getText();
   $pref::iPhone::ForcePalletedBMPsTo16Bit = ForcePalletedTo16BitToggle.getValue();

   // Torque 2D does not have true, GameKit networking support. 
   // The old socket network code is untested, undocumented and likely broken. 
   // This will eventually be replaced with GameKit. 
   // For now, it is confusing to even have a checkbox in the editor that no one uses or understands. 
   // If you are one of the few that uses this, uncomment the next line. -MP 1.5   
   // $pref::iPhone::UsesNetwork = UseNetworkToggle.getValue();
   //$Game::UsesNetwork = $pref::iPhone::UsesNetwork;
}
