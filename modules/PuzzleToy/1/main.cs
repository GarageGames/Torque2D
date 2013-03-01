//-----------------------------------------------------------------------------
// Puzzle Toy Demo
//-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/// Function used to create the toy.
///-----------------------------------------------------------------------------
function PuzzleToy::create( %this )
{
   // This is a tab separated list of color strings so that we can use a color index
   // to store a game pieces color and then use this list to find out it's color string
   // We do this using getWord(PuzzleToy::colors, index)
   PuzzleToy.colors =  "Black" TAB "White" TAB "Red" TAB "Blue" TAB "Yellow" TAB 
                     "Green" TAB "Orange" TAB "Purple" TAB "Grey" TAB "Bucket" 
                     TAB "Bomb" TAB "Eraser"; 
   // default the current level to the main menu scene.
   PuzzleToy.currentLevel = "./levels/mainMenu.scene.taml";
   
   // load up my game specific game scripts    
   exec("./scripts/gamescripts/gameBoard.cs");   
   exec("./scripts/gamescripts/gamePiece.cs");
   exec("./scripts/gamescripts/soundButton.cs");   
   exec("./scripts/gamescripts/startButton.cs");
   exec("./scripts/gamescripts/nextButton.cs");
   exec("./scripts/gamescripts/mainMenuButton.cs");
   exec("./scripts/gamescripts/timeBar.cs");
   // Set some global variables
   // this will be false until the options, level time etc are set.
   PuzzleToy.bToyInit = false;	
	PuzzleToy.soundEnabled = true;	
	PuzzleToy.LevelTime = 360000;	
	
	// add some options in the tool ui.
	addNumericOption("LevelTime in Minutes", 1, 10, 1, "setLevelTime", 6,  true);
   addSelectionOption(getLevelList(), "Select Level", 5, "setSelectedLevel", false);  
   // now my toy options are initialized, so set bToyInit to true.
   PuzzleToy.bToyInit = true;   
   
   // I moved all the script loading up the main menu and such in the reset function 
   // so I don't have to write it twice.
   // I call it here to start everything up
   PuzzleToy.reset();
      
}

///-----------------------------------------------------------------------------
/// set the selected level
/// Param %value The base level name without the path or ".scene.taml" added
/// This is specifically for our toolbox control don't call this elsewhere.
///-----------------------------------------------------------------------------
function PuzzleToy::setSelectedLevel( %this, %value )
{
   // Only set if we have already initialized the toy.
   // this is to keep from resetting when the toolbox options are added.
   if (PuzzleToy.bToyInit)
   {
      // Create the filename string.
      %levelToLoad = "./levels/" @ %value @ ".scene.taml";
      // Load the level and show a loading screen.
      PuzzleToy.loadLevel( %levelToLoad, true );
   }
}

///-----------------------------------------------------------------------------
/// set the time before you fail a level
/// param %value - time in seconds
/// This will be converted to milliseconds
///-----------------------------------------------------------------------------
function PuzzleToy::setLevelTime( %this, %value )
{
   // Only set if we have already initialized the toy.
   // this is to keep from resetting when the toolbox options are added.
   if (PuzzleToy.bToyInit)
   {
      // Set the time allowed to complete a level
      PuzzleToy.LevelTime = %value * 60000;
   }
}
///-----------------------------------------------------------------------------
/// Get the level list
/// returns a comma separated list of level names
///-----------------------------------------------------------------------------
function getLevelList()
{
   // create the comma separated list   
   %list = "level1_1,level1_2,level1_3,level1_4,mainMenu";
   // return the created list
   return %list;
}

///-----------------------------------------------------------------------------
/// destroy function for this toy
///-----------------------------------------------------------------------------
function PuzzleToy::destroy( %this )
{
   // Before I do anything, I want to cancel any level load events so we don't 
   // try to load a level after destroying the toy.
   %this.cancelLoadEvents();
   // Stop any sounds that are playing.
   alxStopAll();
   // Since I changed these, I am setting them back to their defaults
   SandboxWindow.setUseWindowInputEvents( true );
   SandboxWindow.setUseObjectInputEvents( false ); 
   
}

///-----------------------------------------------------------------------------
/// Cancel any pending load events
/// This function is to keep from having unwanted load events
///-----------------------------------------------------------------------------
function PuzzleToy::cancelLoadEvents( %this )
{
   // If we don't have a pending load event, just return
   if (!isEventPending( %this.loadEventId ))
      return;
   // Otherwise, we should cancel the load event
   cancel(%this.loadEventId);
   // Now lets clear out the load event Id so we won't get a false positive next time
   %this.loadEventId = "";
   
}

///-----------------------------------------------------------------------------
/// Reset the toy
///-----------------------------------------------------------------------------
function PuzzleToy::reset( %this )
{
   // Since we use this to start and reset our toy, we should make sure there
   // aren't any pending load events scheduled.
   %this.cancelLoadEvents();
   // Stop any sounds that may be playing
   alxStopAll();
   // Use our custom loadLevel function so we can schedule main menu to be loaded.
   // Loading is true to show the loading screen first.
   PuzzleToy.loadLevel( "./levels/mainMenu.scene.taml", true );

   
}

///-----------------------------------------------------------------------------
/// Load the currently set level
/// This should not be called directly.  Call PuzzleToy:loadLevel instead so
/// it can cycle to the loading screen, then the level you want.
///-----------------------------------------------------------------------------
function loadLevel(%this)
{
   // Any time we load a level we should stop any playing sounds.
   alxStopAll();   
   
   // Set our scene by reading in the level file value stored in currentLevel
   setCustomScene(TamlRead(PuzzleToy.currentLevel));
   
   // If we are loading the loading scene, schedule the next level to load with
   // loading as false.
   if (PuzzleToy.loading)
   {
      // set loading to false since we already loaded the loading scene.
      PuzzleToy.loading = false;
      // Schedule the level we intend to load next
      PuzzleToy.loadLevel(PuzzleToy.nextLevel, false);
   }
   else
   {
      // play this levels background music if the sound is enabled.
      if (PuzzleToy.soundEnabled)
         alxPlay(SandboxScene.MusicAsset);
      
   }
   // Set up our window input so we can play the game correctly.
   SandboxWindow.setUseWindowInputEvents( false);
   // We want our objects to be able to get input events so set this to true.
   SandboxWindow.setUseObjectInputEvents( true );
   // We don't want gravity, so I want to make sure our gravity is set to (0,0).
   SandboxScene.setGravity(0, 0);
   // Since we may be coming from other toys, and my scene size might be different,
   // I set the window position, size, and zoom here.
   SandboxWindow.setCameraPosition( 0, 0);
   SandboxWindow.setCameraSize( 160, 90 );
   SandboxWindow.setCameraZoom( 1 );
   
   
}

///-----------------------------------------------------------------------------
/// Load the level specified.
/// Param %levelName - the name of the level, not the file name.
/// Param %loading - bool for if we should transition to the loading screen
/// be displayed first.
///-----------------------------------------------------------------------------
function PuzzleToy::loadLevel( %this, %levelName, %loading )
{
   // There shoudln't be any load events at this point, so clear out the load event id.
   %this.loadEventId = "";
   // Set our global bool to know whether or not we should load the loading scene.
   PuzzleToy.loading = %loading;
   
   // Should we show the loading scene, or load the specified level?
   if (PuzzleToy.loading)
   {
      // set the current level to the loading scene so we can schedule the load.
      PuzzleToy.currentLevel = "./levels/LoadLevel.scene.taml";
      // Since we aren't loading the supplied %levelName, we need to store it in 
      // $nextLevel so we can load it after the loading scene.
      PuzzleToy.nextLevel = %levelName;
      // Schedule the load and store the schedule event Id so we can cancel it if needed.
      %this.loadEventId = schedule(500, 0, "loadLevel" );
   }
   else
   {
      // Set the current level to the supplied level so we can schedule the load.
      PuzzleToy.currentLevel = %levelName;
      // Schedule the load and store the schedule event Id so we can cancel it if needed.
      %this.loadEventId = schedule(1000, 0, "loadLevel" );
   }

}

