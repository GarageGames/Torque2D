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
// KeybindPackage
// This package is used to add additional functionality to the bind command. We need to grab
// some info about each bind for the keybinding gui.
//---------------------------------------------------------------------------------------------
$keybindCount = 0;
package KeybindPackage
{
   function ActionMap::bind(%this, %device, %action, %callback, %description)
   {
      // Grab some info about the bind.
      $keybindName[$keybindCount] = %description $= "" ? %callback : %description;
      $keybindCommand[$keybindCount] = %callback;
      $keybindDefaultKey[$keybindCount] = %action;
      $keybindMap[$keybindCount] = %this;
      $keybindCount++;
      
      // Let the parent do its thing.
      Parent::bind(%this, %device, %action, %callback);
   }
};

//---------------------------------------------------------------------------------------------
// initializeKeybindOptions
// Called when the options gui is placed to set up the keybinding options.
//---------------------------------------------------------------------------------------------
function initializeKeybindOptions()
{
   KeysTextList.clear();
   
   // Loop through all the keybinds.
   for (%i = 0; %i < $keybindCount; %i++)
   {
      %currentBinding = $keybindMap[%i].getBinding($keybindCommand[%i]);
      %action = getWord( %currentBinding, 1);
      if (strstr(%action,"ctrl") >= 0 || strstr(%action,"alt") >= 0 || strstr(%action,"shift") >= 0 || strstr(%action,"cmd") >= 0 || strstr(%action,"opt") >= 0)
         %action = %action SPC getWord( %currentBinding, 2);
         
      KeysTextList.addRow(%i, $keybindName[%i] TAB getFriendlyKeyName(%action));
   }
}

// [neo, 6/8/2007 - #3191]
function isModifierKey( %k )
{
   // Make sure we don't find of these below by mistake (strstr() will find e.g. the shift in rshitf, etc)
   if( %k $= "lctrl" || %k $= "rctrl" || %k $= "lshift" || %k $= "rshift" )
      return false;
   
   // Modifiers can be combined e.g. ctrl-alt or shift-ctrl, etc
   if( strstr( %k, "ctrl" ) >= 0 
    || strstr( %k, "alt" ) >= 0 
    || strstr( %k, "shift" ) >= 0 
    || strstr( %k, "cmd" ) >= 0 
    || strstr( %k, "opt" ) >= 0 )
      return true;
      
   return false;
}

//---------------------------------------------------------------------------------------------
// [neo, 6/8/2007 - #3191]
// RemapInput.unbindAllActionsForCommand
// Clears all current actions for a command so a new one can be assigned
//---------------------------------------------------------------------------------------------
function RemapInput::unbindAllActionsForCommand( %this, %map, %device, %cmd )
{
   %oldAction = %map.getBinding( %cmd );
         
   if( %oldAction !$= "" )
   {
      // May be more than one so loop
      %cnt = getWordCount( %oldAction );
      
      for( %i = 1; %i < %cnt; %i++ )
      {
         %a = getWord( %oldAction, %i );
         
         if( isModifierKey( %a ) )
         {
            %i++;
            
            if( %i < %cnt )
               %a = %a SPC getWord( %oldAction, %i );
         }
         
         %map.unbind( %device, %a );
      }            
   }
}

//---------------------------------------------------------------------------------------------
// RemapInput.onInputEvent
// Changes the selected commands key binding to whatever key was pressed.
//---------------------------------------------------------------------------------------------
function RemapInput::onInputEvent(%this, %device, %action)
{
   // This can happen when the window loses focus (i.e the window key or menu key are pressed).
   if ((%device $= "") || (%action $= ""))
      return;
   
   // don't allow mouse bindings
   if (%device $= "mouse0")
      return;
      
   // Close the remap gui.
   Canvas.popDialog(RemapGui);
   
   // Grab the command and name of the bind being changed, and the map that owns it.
   %cmd = $keybindCommand[%this.index];
   %name = $keybindName[%this.index];
   %map = $keybindMap[%this.index];
   
   // Check if this key is already mapped to something.
   %prevMap = %map.getCommand(%device, %action);
   
   // If the previous mapping is the same as the new one, nothing needs to happen.
   if (%prevMap !$= %cmd)
   {
      // If there was no previous mapping for the new key, just update the bind.
      if (%prevMap $= "")
      {
         // [neo, 6/8/2007 - #3191]
         // Make sure to unbind any other keys for this command
         %this.unbindAllActionsForCommand( %map, %device, %cmd );
         
         %map.bind( %device, %action, %cmd );
      }      
      // Otherwise, do something to remedy the situation.
      else
      {
         // Find the index of the command that is bound to the new key.
         %prevMapIndex = findCommandIndex(%prevMap);
         
         if (%prevMapIndex != -1)
         {
            // Ask the user whether they want to proceed with the change.
            MessageBoxOKCancel("", getFriendlyKeyName(%action) @ " is already bound to " @
                                   $keybindName[%prevMapIndex] @ ". Changing the bind " @
                                   "will switch the keys for the two actions. Proceed?",
                                   "forceUpdateBind(" @ %this.index @ ", " @ %prevMapIndex @
                                   ", \"" @ %device SPC %action @ "\", " @ "\"" @
                                   %map.getBinding(%cmd) @ "\");");
         }
         
         // If we get a -1, the keybinding is a reserved key.
         else
            MessageBoxOK("", "Cannot bind key " @ getFriendlyName(%prevMap) @
                             ". It is reserved.");
      }
   }
   initializeKeybindOptions();
}

//---------------------------------------------------------------------------------------------
// revertControlOptions
// Revert the keybindings to their default values.
//---------------------------------------------------------------------------------------------
function revertControlOptions()
{
   // Delete all the maps.
   for (%i = 0; %i < $keybindCount; %i++)
   {
      if (isObject($keybindMap[%i]))
         $keybindMap[%i].delete();
   }
   
   // Reset the key count.
   $keybindCount = 0;
   
   // [neo, 6/12/2007 - #3222]
   %bindFile = getPrefsPath( "customEditorBind.cs" );
   
   if( isFile( %bindFile ) )
      fileDelete( %bindFile );
   
   // Load the defaults.
   activatePackage(KeybindPackage);
   loadKeybindings();
   deactivatePackage(KeybindPackage);
   
   // And update the gui.
   initializeKeybindOptions();
}

//---------------------------------------------------------------------------------------------
// changeBinding
// Pushes the change binding dialog for a specific command.
//---------------------------------------------------------------------------------------------
function changeBinding()
{
   // Grab the index.
   %index = KeysTextList.getSelectedId();
   // kick out if nothing is selected
   if(%index < 0)
      return;
   // Store the index so the appropriate change can be made.
   RemapInput.index = %index;
   // Show the remap gui.
   Canvas.pushDialog(RemapGui);
   RemapText.setText("Enter key for " @ $keybindName[%index]);
}

//---------------------------------------------------------------------------------------------
// forceUpdateBind
// This should only be called when there is a conflict of keybindings and they need to be
// switched.
//---------------------------------------------------------------------------------------------
function forceUpdateBind(%index, %prevIndex, %action, %prevAction)
{
   // [neo, 6/8/2007 - #3191]
   // Make sure [modifier key] aren't cut off to just [modifier]
   %newAction = getWord( %action, 1 );
   %oldAction = getWord( %prevAction, 1 );
   
   if( isModifierKey( %newAction ) )
      %newAction = %newAction SPC getWord( %action, 2 );
      
   if( isModifierKey( %oldAction ) )
      %oldAction = %oldAction SPC getWord( %prevAction, 2 );
   
   $keybindMap[%index].bind(getWord(%action, 0), %newAction, $keybindCommand[%index]);
   $keybindMap[%index].bind(getWord(%prevAction, 0), %oldAction, $keybindCommand[%prevIndex]);
   
   initializeKeybindOptions();
}

//---------------------------------------------------------------------------------------------
// findCommandIndex
// Returns the index of the keybind for a command.
//---------------------------------------------------------------------------------------------
function findCommandIndex(%command)
{
   // Loop through the keybinds.
   for (%i = 0; %i < $keybindCount; %i++)
   {
      if ($keybindCommand[%i] $= %command)
         return %i;
   }
   return -1;
}

//---------------------------------------------------------------------------------------------
// getFriendlyName
// Takes a key binding as input and translates it into something more visually pleasing.
// Incomplete at the moment.
//---------------------------------------------------------------------------------------------
function getFriendlyKeyName(%action)
{
   %name = strlwr(%action);
   %len = strlen(%action);
   
   // Convert single letters to uppercase.
   if (%len == 1)
      %name = strupr(%action);
   
   // Convert single letter but not modifier or names to uppercase.
   %mark = strpos(%action, " ");
   if (%mark >= 0 && %len - %mark == 2)
   {
      %chunk1 = getSubStr(%action, 0, %mark);
      %chunk2 = getSubStr(%action, %mark, %len);
      %name = %chunk1 @ strupr(%chunk2);
   }
   
   // Convert F-keys to uppercase
   %f = getSubStr(%action, 0, 1);
   if (%f $= "F" && (%len == 2 || %len == 3))
      %name = strupr(%action);

   // Mouse buttons.
   else if(%action $= "BUTTON0")
      %name = "Left Mouse";
   else if(%action $= "BUTTON1")
      %name = "Right Mouse";
   else if(%action $= "BUTTON2")
      %name = "Middle Mouse";
      
   // Mouse axis.
   else if (%action $= "XAXIS")
      %name = "Mouse X";
   else if (%action $= "YAXIS")
      %name = "Mouse Y";
   else if (%action $= "ZAXIS")
      %name = "Scroll Wheel";
      
   return %name;
}
