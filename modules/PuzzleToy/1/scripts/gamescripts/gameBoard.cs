//---------------------------------------------------------------------------------------------
// The Game Board
//---------------------------------------------------------------------------------------------
// Global constants can be placed in any scripts that are executed.  
// I'm placing it here because it is specifically used for the gameBoard class.

// the default amount of time to complete a board in case this is not set.
$gameBoard::DefaultTime = 360000;
// The minimum time allowed.
$gameBoard::MinTime = 60000;
// The color count. How many colors should be used
$gameBoard::ColorCount = 5;
// What size should our pieces be
$gameBoard::PieceSize = 7;

///-----------------------------------------------------------------------------
/// onAdd called when the object is added to a scene.
///-----------------------------------------------------------------------------
function gameBoard::onAddToScene(%this, %scenegraph)
{
   // When a gameBoard is added to the scene, I want to make sure we know it hasn't yet been completed.   
	$BoardComplete = false;	
}

///-----------------------------------------------------------------------------
/// Initialize the game board with the supplied values
/// Here I will initialize all the starting values and build the needed objects
/// to have a working game board.
/// param - %xcellcount: How many pieces wide the game board is.
/// param - %ycellcount: How many pieces tall the game board is.
/// param - %colorcount: How many colors the game pieces can be for difficulty.
///-----------------------------------------------------------------------------
function gameBoard::initialize(%this, %xcellcount, %ycellcount, %colorcount)
{
   // set the color count global variable
   $gameBoard::ColorCount = %colorcount;
   // If the level time is not set to a valid value, set it to the default time
   if ($LevelTime < $MinTime)
   {
      $LevelTime = $gameBoard::DefaultTime;
   }
   // Start out our timeElapsed at 0.
   %this.timeElapsed = 0;
   %this.previousTime = getRealTime();   
   // We should not start with a piece selected so setting this to false.
   %this.bPieceSelected = false;
   // Clearing out the selected piece.  
   // I only really need this here if I plan to re-initialize a board that's already in use.
	%this.selectedPiece = "";
   // store our values for the board when initialize is called   
   %this.cellCountX = %xcellcount;
   %this.cellCountY = %ycellcount;
   // This value is to track whether or not we are spawning new pieces.   
   %this.isFilling = false;   
   // figure out how big the game pieces should be for sizing the break anim and sparkle particle effect objects when needed..   
   $gameBoard::PieceSize = %this.getSizeX()/%this.cellCountX;
   // calculate the bottom left position for a piece and store it
   // so we don't have to do it every time we make a new piece.
   // get this boards position so we don't rely on it being in a specific place.
   %position = %this.getPosition();
   // Store the X and Y value of this position by getting the first and second word from the full vector2d
   %positionX = getWord(%position, 0);
   %positionY = getWord(%position, 1);
   // What is half the size of this gameBoard
   %halfX = %this.getSizeX()/2;
   %halfY = %this.getSizeY()/2;
   // The bottom left is the current position minus half the size of the board then offset by half the size 
   // of a piece since the position is at the center of the object.
   %this.startLocationX = %positionX - %halfX + ($gameBoard::PieceSize/2);	
	%this.startLocationY = %positionY - %halfY + ($gameBoard::PieceSize/2);
   // some simsets we need to be persistent ish.
   // Since I don't want to have to create and destroy these simsets all the time, I will create them here.
   %this.tempBreakList = new SimSet() {};   
   %this.breaklist = new SimSet() {};
   %this.CheckList = new SimSet() {};
   // Here we will get the levels board data.  We can do that by loading stored data or just make random
   // data.  I don't have any data to load, so I will just create some random data
   // I made a function for this that I use below.
   %this.BoardData = gameBoard::createBoardData(%xcellcount, %ycellcount, %colorcount);
   // Here I need to populate the canvas and game board CompositeSprite objects based on my board data
   // In order to do this, I iterate through all the possible positions on my gameboard.
   // I'm using the cell count variables I stored earlier.   
   for (%ix = 0; %ix < %this.cellCountX; %ix++)
   {
      for (%iy = 0; %iy < %this.cellCountY; %iy++)
      {
         // For simplicity, I added the bCanvas object to my level taml so I don't need to create one, I just populate
         // it with sprites.
         // Add a sprite at the logical location.
         bCanvas.addSprite(%ix SPC %iy);
         // After adding a sprite, that sprite is automatically selected, otherwise you would need to select it before 
         // you can access anything on it.  This would be done by calling selectSprite - bCanvas.selectSprite(%ix, %iy);
         // Now I set the sprite image
         bCanvas.setSpriteImage("PuzzleToy:CanvasTexture");
         // And since I want the piece to be colored based on my board data, I set the blend color of the sprite
         // here.  The board data is essentially a long space separated string list.  I use getWord(%var, int) to get the data
         // from the desired word in this list.
         bCanvas.setSpriteBlendColor(%this.getBlendFromColor(getWord(%this.BoardData, %ix + (%iy * %this.cellCountX))));         
         // I set up a special function (getBlendFromColor()) to get the blend color from the color enum in board data.
         // Sometimes it makes sense if you know you will need to perform the same actions in multiple locations,
         // to create a function for it.  Then you can use the function in place of rewriting all that script.
         
         // Behind the canvas will be an image that mirrors the colors of the canvas, so I will also add sprites to bBoardImage
         // in the same way.
         bBoardImage.addSprite(%ix SPC %iy);
         bBoardImage.setSpriteImage("PuzzleToy:WhiteBackdrop");
         bBoardImage.setSpriteBlendColor(%this.getBlendFromColor(getWord(%this.BoardData, %ix + (%iy * %this.cellCountX))));
         
         // I want to keep track of the canvas states, so I'm storing an array of booleans to let me know which canvas pieces 
         // still need to be "Painted". This is how I will check to see if the level has been completed later.
         %this.canvasPieces[%ix, %iy] = true;
                  
         // When I fill the board with pieces I check to see that there isn't a piece in each location.  So I default each
         // locatin to a false value so I know it will register false when checked.
         %this.gamePieces[%ix, %iy] = false;       
         
      }
   }   
   // We have finished initializing so set bInit to true. In other functions if actions require the board to be initialized,
   // I can check this variable before proceeding.
   %this.bInit = true;   
  
}

// Using this function I can set the color of any object based on it's color enum
// I want this to be a function so anywhere I want to set an object's blend color based on it's color enum
// I can do it by calling this function and just passing it the object.
///-----------------------------------------------------------------------------
/// Choose the blend color based on the color of the object.
/// param %object - The object I want to set the blend color for.
/// param %alpha - desired alpha
///-----------------------------------------------------------------------------
function gameBoard::setObjectBlendColor(%this, %object, %alpha)
{   
   // I am using a switch here, but I could go with a cleaner approach and set the
   // blend color by name if I didn't have special color values I wanted to use.
   // The commented funciton on the next line would set the color by name. 
   // %object.setBlendColor(getWord($gelPiece::colors, %object.color));
   
   // I use getWord here to get the string from my enum which is essentially a tab separated list.
   // This makes it easier to understand what is happening here.
   switch$(getWord($gelPiece::colors, %object.color))
   {
      case "White":
         %object.setBlendColor(1.0, 1.0, 1.0, %alpha);      
      case "Black":
         %object.setBlendColor(0.2, 0.2, 0.2, %alpha);      
      case "Red":
         %object.setBlendColor(1.0, 0.0, 0.0, %alpha);
      case "Blue":
         %object.setBlendColor(0.0, 0.0, 1.0, %alpha);      
      case "Yellow":
         %object.setBlendColor(1.0, 1.0, 0.0, %alpha);      
      case "Green":
         %object.setBlendColor(0.0, 1.0, 0.0, %alpha);      
      case "Orange":
         %object.setBlendColor(1.0, 0.5, 0.0, %alpha);      
      case "Purple":
         %object.setBlendColor(1.0, 0.0, 1.0, %alpha);
      case "grey":
         %object.setBlendColor(0.5, 0.5, 0.5, %alpha);
      default:
         %object.setBlendColor(1.0, 1.0, 1.0, %alpha);      
   }
}

// I found that for composite sprites, I wanted to be able to get a blend color that I want from the 
// color enum.  I created this function for that.
// Instead of setting an objects blend color based on it's color enum, I just pass the color enum value
// and this function returns the blend color that I want.
///-----------------------------------------------------------------------------
/// Get a color from a color index
/// param %newcolor - The color index.
///-----------------------------------------------------------------------------
function gameBoard::getBlendFromColor(%this, %newcolor)
{
   // make sure we have a valid color stored in case something goes wrong.
   %blendColor = "1.0 1.0 1.0 1.0";
   // Choose the blend color based on the color of the object.
   switch$(getWord($gelPiece::colors, %newcolor))
   {
      case "White":
         %blendColor = "1.0 1.0 1.0 1.0";      
      case "Black":
         %blendColor = "0.2 0.2 0.2 1.0";      
      case "Red":
         %blendColor = "1.0 0.0 0.0 1.0";
      case "Blue":
         %blendColor = "0.0 0.0 1.0 1.0";      
      case "Yellow":
         %blendColor = "1.0 1.0 0.0 1.0";      
      case "Green":
         %blendColor = "0.0 1.0 0.0 1.0";      
      case "Orange":
         %blendColor = "1.0 0.5 0.0 1.0";      
      case "Purple":
         %blendColor = "1.0 0.0 1.0 1.0";
      case "grey":
         %blendColor = "0.5 0.5 0.5 1.0";
      default:
         %blendColor = "1.0 1.0 1.0 1.0";      
   }
   // Return the color
   return %blendColor;
}

///-----------------------------------------------------------------------------
/// onAdd callback when the object is added to the scene
///-----------------------------------------------------------------------------
function gameBoard::onAdd(%this)
{
   // default values for some fields.
   %this.bInit = false;   
      
   // this is where we will be storing pieces that need to be checked.
   %this.checkSet = new SimSet() {};   
}
///-----------------------------------------------------------------------------
/// onUpdate callback.  Called every 32ms.
///-----------------------------------------------------------------------------
function gameBoard::onUpdate(%this)
{         
   // if the board is complete, we don't need any updating so just return.
   if ($BoardComplete)
      return;
   // if the board is initialized.  we should top fill, otherwise we should initialize
   if (%this.bInit)
   {      
      %this.topFill();
   }
   else
   {
      // Initialize the board passing the number of pieces wide tall and color count.
      %this.initialize(8, 10, 5);	
   }      
   // Iterate through all our pieces to check if they need updating.
   for(%ix = 0; %ix < %this.cellCountX; %ix++)
   {
      for (%iy = 0; %iy < %this.cellCountY; %iy++)
      {
         // If there is a game piece in this location, check it.
         if (%this.gamePieces[%ix, %iy])
         {
            // Ensure this piece is in the right location.
            if (%this.gamePieces[%ix, %iy].locationX != %ix || %this.gamePieces[%ix, %iy].locationY != %iy)
            {
               // If it's not, force it.
               %this.gamePieces[%ix, %iy].locationX = %ix;
               %this.gamePieces[%ix, %iy].locationY = %iy;
            }
            // If the piece needs updating, update it.
            if (%this.gamePieces[%ix, %iy].bUpdate)
               %this.gamePieces[%ix, %iy].myUpdate();            
         }
      }
   }
   // If we are not currently filling, and the board is ready to be checked,
   // Lets check the necessary pieces for match 3 status.
   if (!%this.isFilling && %this.getCheckReady())
   {
      // Clear out the break list before we start.
      %this.breakList.clear();
      // Get the list of pieces we need to check.      
      %this.checkSet = %this.getCheckList();
      // Iterate through our check list till we don't have any more.
      while (%this.checkSet.getCount() > 0)
      {
         // To make sure we don't check this piece more than once, set bCheckBreaks to false.
         %this.checkSet.getObject(0).bCheckBreaks = false;
         // See if we get a mach and need to break pieces
         %this.getBreaks(%this.checkSet.getObject(0));      
      }
      // Once we've checked the pieces we need to, check if we have enough pieces in the list
      if (%this.breaklist.getCount() >=3)
      {
         // Break all the pieces in the break list.
         for (%i = 0; %i < %this.breaklist.getCount(); %i++)
         {
            %this.BreakPiece(%this.breaklist.getObject(%i));         
         }
         // clear out the break list.
         %this.breaklist.clear();
      }
   }   
   // Check if we are out of time, if we aren't, increment our time counter.
   if (%this.TimeElapsed < $LevelTime)
   {
      // store the current time value as the current time.
      %this.currentTime = getRealTime();
      // Increment our elapsed time by the time since our last update.
      %this.TimeElapsed += %this.currentTime - %this.previousTime;
      // If we have run out of time, fail the board
      if (%this.TimeElapsed >= $LevelTime)
      {
         // time is over, we should give a failure screen. 
         // set the board as complete so no more updating can occur
         $BoardComplete = true;
         // clear the board pieces
         %this.clearBoard();
         // set the complete image to the failure one.
         bCompleteImage.setImage("PuzzleToy:Failed");         
         // set the complete image as visible
         bCompleteImage.setVisible(true);
         // Set the elapsed time to the total time in case we went over.
         %this.TimeElapsed = $LevelTime;
      }            
      // Update the time bar with the new time left.
      bTimeFront.UpdateTime( ($LevelTime - %this.TimeElapsed)/$LevelTime );      
      // store our current time as previous time so we can calculate elapsed time
      // in the next update
      %this.previousTime = %this.currentTime;
   }   
   // If the board is initialized and not yet complete, check if the board is completed
   if (%this.bInit && !$BoardComplete)
      %this.checkDone();
}

///-----------------------------------------------------------------------------
/// Set the selected piece to the supplied piece
/// param - %gamepiece - game piece to set as selected.
///-----------------------------------------------------------------------------
function gameBoard::setSelectedPiece(%this, %gamepiece)
{
   // Do we have a piece selected already otherwise make sure this piece isn't moving
   if (%this.bPieceSelected)
   {
      // Is the selected piece the same as this piece otherwise, is either this 
      // or the selected piece moving?
      if (%this.selectedPiece == %gamepiece )
      {
         // If so, set it to not selected
         %gamepiece.setSelected(false);
         // update bPieceSelected so we know we don't have a piece selected
         %this.bPieceSelected = false;
         // Return so we don't do any of the logic below.
         return;
      }
      else if (%gamepiece.bMoving || %this.selectedPiece.bMoving)
      {
         // If either piece is moving, unselect both pieces.
         %this.selectedPiece.setSelected(false);
         %gamepiece.setSelected(false);
         // update bPieceSelected so we know we don't have a piece selected
         %this.bPieceSelected = false;
         // return so we don't do any of the logic below
         return;
      }
      // Calculate how many spaces the two pieces are away from each other.
      %valueX = mAbs(%this.selectedPiece.locationX - %gamepiece.locationX);
      %valueY = mAbs(%this.selectedPiece.locationY - %gamepiece.locationY);
      
      // make sure we are only 1 space away in only one direction.      
      if ((%valueX <= 1 && %valueY <= 1) && (%valueX != %valueY))
      {
         // swap the pieces
         %this.swapPieces(%this.selectedPiece, %gamepiece);
         // Check each piece in it's new location to see if it will break
         %bSelectedMove = %this.checkForMoves(%this.selectedPiece);
         %bGelPieceMove = %this.checkForMoves(%gamepiece);
         // Check that at least one will break
         if (!%bSelectedMove && !%bGelPieceMove)
         {
            // If neither break, this is an invalid move, move the pieces back.
            %this.swapPieces(%this.selectedPiece, %gamepiece);
            //If sound is enabled, play the bad move sound
            if ($soundEnabled)
               alxPlay("PuzzleToy:BadMoveSound");
         }
         else
         {
            // Otherwise we can move so we should update our locations and set them
            // to check for breaks on their next update.
            // Update the selected pieces location            
            %this.selectedPiece.updateTargetLocation();
            // Flag the selected piece for checkbreaks.
            %this.selectedPiece.bCheckBreaks = true;         
            // Update the %gamepiece
            %gamepiece.updateTargetLocation();
            // Flag the %gamepiece for checkbreaks.
            %gamepiece.bCheckBreaks = true; 
            // if sound is enabled, play the move sound.
            if ($soundEnabled)
               alxPlay("PuzzleToy:MoveSound");        
         }
         
      }
      // set selected to false since we are done either way
      %this.selectedPiece.setSelected(false);
      // No matter what happened, there is not a piece selected.      
      %this.bPieceSelected = false;
   }
   else if(!%gamepiece.bMoving)
   {
      // No piece was selected, so %gamepiece is now the selected piece
      %this.selectedPiece = %gamepiece;
      // let the piece know it's selected.
      %gamepiece.setSelected(true);
      // Change the flag so we know we have a piece selected.
      %this.bPieceSelected = true;
      // If sound is enabled play the select sound.
      if ($soundEnabled)
         alxPlay("PuzzleToy:SelectSound");
   }
}
///-----------------------------------------------------------------------------
/// Swap board positions between the tow pieces provided.
/// param %pieceone - gamepiece
/// param %piecetwo - gamepiece
///-----------------------------------------------------------------------------
function gameBoard::swapPieces(%this, %pieceone, %piecetwo)
{
   // if the color of both pieces are the same, then there is no point in swapping
   // so just return.
   if (%pieceone.color == %piecetwo.color)
      return;
      
   // Store our locationX and Y values in preparation for the swap
   %locationOneX = %pieceone.locationX;
   %locationOneY = %pieceone.locationY;
   %locationTwoX = %piecetwo.locationX;
   %locationTwoY = %piecetwo.locationY;      
   // Store the pieces in their new location in the array   
   %this.gamePieces[%locationOneX, %locationOneY] = %piecetwo;
   %this.gamePieces[%locationTwoX, %locationTwoY] = %pieceone;
   // Update the locationX and Y on each piece to reflect their new positions
   %piecetwo.locationX = %locationOneX;
   %piecetwo.locationY = %locationOneY;   
   %pieceone.locationX = %locationTwoX;
   %pieceone.locationY = %locationTwoY;
   // Temporarily change the layers so one piece will move under the other.
   // This is to prevent popping
   %pieceone.setSceneLayer(1);
   %piecetwo.setSceneLayer(2);
}

///-----------------------------------------------------------------------------
/// Check the piece for potential matches
/// param %gamepiece - gamePiece
/// This is used when moving pieces to make sure the move is valid.
///-----------------------------------------------------------------------------
function gameBoard::checkForMoves(%this, %gamepiece)
{     
   
   // find out how many pieces we should check in each direction.
   // How many pieces should be above this one.
   %upDirection = %this.cellCountY - 1 - %gamepiece.locationY;
   if (%upDirection > 2)
      %upDirection = 2;
   //How many piece should be below this one.
   %downDirection = %gamepiece.locationY;
   if (%downDirection > 2)
      %downDirection = 2;
   // How many pieces should be to the left.
   %leftDirection = %gamepiece.locationX;
   if (%leftDirection > 2)
      %leftDirection = 2;
   // How many pieces should be to the right up to 2.
   %rightDirection = %this.cellCountX - 1 - %gamepiece.locationX;
   if (%rightDirection > 2)
      %rightDirection = 2;
   // init the match count so we can start counting
   %matchCountY = 0;  
   // start %bMatch as true so we can count our first piece
   %bMatch = true; 
   // Perform the check on pieces above
   for (%i = 0; %i < %upDirection; %i++)
   {
      // Check if the colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX, %gamepiece.locationY + %i + 1].color)
      {
         // If all the pieces in this direction have matched so far count it.
         if (%bMatch)
            %matchCountY += 1;  
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatch = false;
      }
   }
   // Start match at true again so we can check down.
   %bMatch = true;
   // Perform the check on pieces below
   for (%i = 0; %i < %downDirection; %i++)
   {
      // Check if the colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX, %gamepiece.locationY - (%i + 1)].color)
      {
         // If all the pieces in this direction have matched so far count it
         if (%bMatch)
            %matchCountY += 1;
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatch = false;
      }
   }
   // If our match count is 2 or more then counting the piece we are checking, 
   // that's 3, the match is good.  return true.
   if (%matchCountY >= 2)
      return true;
      
   // If we didn't get a match yet, we need to check left and right
   // Init our matchCountX.
   %matchCountX = 0;
   // Start match at true again so we can check left.
   %bMatch = true;   
   // Perform the check on pieces left
   for (%i = 0; %i < %leftDirection; %i++)
   {
      // Check if the colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX - (%i + 1), %gamepiece.locationY].color)
      {
         // If all the pieces in this direction have matched so far count it
         if (%bMatch)
            %matchCountX += 1;
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatch = false;
      }
   }
   // Start match at true again so we can check right.
   %bMatch = true;
   // Perform the check on pieces right
   for (%i = 0; %i < %rightDirection; %i++)
   {
      // Check if the colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX + %i + 1, %gamepiece.locationY].color)
      {
         // If all the pieces in this direction have matched so far count it
         if (%bMatch)
            %matchCountX += 1;
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatch = false;
      }
   }
   // If our match count is 2 or more then counting the piece we are checking, 
   // that's 3, the match is good.  return true.
   if (%matchCountX >= 2)
      return true;
   // If we make it this far, then there's no match.
   return false;
   
}

///-----------------------------------------------------------------------------
/// Check for matches with this piece and any matches to the breaklist
/// param - %gamepiece - piece to check
/// This one is to actually get the match and break the pieces.
///-----------------------------------------------------------------------------
function gameBoard::getBreaks(%this, %gamepiece)
{
   
   // temporary break list to keep piece until we know if we have enough for a match.
   %this.tempBreakList.clear();   
   // remove the piece from the check list since we are checking it now.
   %this.checkSet.remove(%gamepiece);      
   // find out how many pieces we should check in each direction.
   // How many pieces should be above this one.
   %upDirection = %this.cellCountY - 1 - %gamepiece.locationY;   
   // How many pieces should be below this one.
   %downDirection = %gamepiece.locationY;      
   // How many pieces should be left of this one.
   %leftDirection = %gamepiece.locationX;   
   // How many pieces should be right of this one.
   %rightDirection = %this.cellCountX - 1 - %gamepiece.locationX;
   // Start match true so we count if our first piece is a match
   %bMatchV = true;
   // Perform the check on pieces above
   for (%i = 1; %i <= %upDirection; %i++)
   {
      // Check if our colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX, %gamepiece.locationY + %i].color)
      {
         // If all the pieces in this direction have matched so add to tempBreakList
         if (%bMatchV)
            %this.tempBreakList.add(%this.gamePieces[%gamepiece.locationX, %gamepiece.locationY + %i]);         
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatchV = false;         
      }
   }
   // Start match true so we count if our first piece is a match
   %bMatchV = true;
   // Perform the check on pieces down
   for (%i = 1; %i <= %downDirection; %i++)
   {
      // Check if our colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX, %gamepiece.locationY - %i].color)
      {
         // If all the pieces in this direction have matched so add to tempBreakList
         if (%bMatchV)
            %this.tempBreakList.add(%this.gamePieces[%gamepiece.locationX, %gamepiece.locationY - %i]);
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatchV = false;
      }
   }
   // Check if we have enough pieces to break
   if (%this.tempBreakList.getCount() >= 2)
   {
      // Iterate through our tempBreakList and add pieces to full break list
      for (%i = 0; %i < %this.tempBreakList.getCount(); %i++)
      {
         %this.breakList.add(%this.tempBreakList.getObject(%i));         
      }
      // We got a match, add the checked piece to the break list.
      %this.breaklist.add(%gamepiece);      
   }
   // Clear out our temporary break list so we can do our checks left and right.
   %this.tempBreakList.clear();
   // Start match true so we count if our first piece is a match
   %bMatchH = true;
   // Perform the check on pieces left
   for (%i = 1; %i <= %leftDirection; %i++)
   {
      // Check if our colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX - %i, %gamepiece.locationY].color)
      {
         // If all the pieces in this direction have matched so add to tempBreakList
         if (%bMatchH)
            %this.tempBreakList.add(%this.gamePieces[%gamepiece.locationX - %i, %gamepiece.locationY]);
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatchH = false;
      }
   }
   // Start match true so we count if our first piece is a match
   %bMatchH = true;
   // Perform the check on pieces right
   for (%i = 1; %i <= %rightDirection; %i++)
   {
      // Check if our colors match
      if (%gamepiece.color == %this.gamePieces[%gamepiece.locationX + %i, %gamepiece.locationY].color)
      {
         // If all the pieces in this direction have matched so add to tempBreakList
         if (%bMatchH)
            %this.tempBreakList.add(%this.gamePieces[%gamepiece.locationX + %i, %gamepiece.locationY]);
      }
      else
      {
         // If no match, set this false so it stops counting in this direction.
         %bMatchH = false;
      }
   }
   // Check if we have enough pieces to break
   if (%this.tempBreakList.getCount() >= 2)
   {
      // Iterate through our tempBreakList and add pieces to full break list
      for (%i = 0; %i < %this.tempBreakList.getCount(); %i++)
      {
         %this.breakList.add(%this.tempBreakList.getObject(%i));         
      }      
      // We got a match, add the checked piece to the break list.
      %this.breaklist.add(%gamepiece);      
   }
   // Clear our tempBreakList since we don't need anything stored in it anymore
   %this.tempBreakList.clear();
}

///-----------------------------------------------------------------------------
/// Break the game piece.
/// param - %gamepiece - Piece to break.
/// This is how we get rid of our pieces that have matched.
/// This will do the check with the level board and create the particle effect
/// and break animation objects.
///-----------------------------------------------------------------------------
function gameBoard::BreakPiece(%this, %gamepiece)
{
   // Check if this piece is the selected piece
   if (%this.selectedPiece == %gamepiece)
   {
      // If it is, clear the selection
      %this.clearSelected();            
   }   
   // Get the state of the canvas at the same location. true means it needs to be painted
   %canvasPieceState = %this.canvasPieces[%gamepiece.locationX, %gamepiece.locationY];
   // Check if the canvas needs to be painted and the canvas below is the same color as this piece
   if (%canvasPieceState && %gamepiece.color == getWord(%this.BoardData, %gamepiece.locationX + (%gamepiece.locationY * %this.cellCountX)))
   {         
      // If this checks out, set the canvas piece state to false in our array.
      %this.canvasPieces[%gamepiece.locationX, %gamepiece.locationY] = false;
      // Now we need to update the canvas composite sprite to match
      // Select the sprite at this location.
      bCanvas.selectSprite(%gamepiece.locationX SPC %gamepiece.locationY);
      // Now change the sprites alpha so it isn't visible
      bCanvas.setSpriteBlendAlpha(0.0);           
   }
   // Here we clone the break anim sprite object called pBreak
   // Store the clone  so we can change initialize it.
   %breakanim = pBreak.clone(true);
   // We only want the break anim to live for the time of the animation with is 0.25
   // So we set it's lifetime
   %breakanim.Lifetime = 0.25;   
   // The piece we clone from has an alpha of 0.0, so we need to set this to make it
   // visible.  Set it's alpha to 0.5.
   %alpha = 0.5;
   // We want it to be the same color as our piece.  We have a cutom function to set
   // an objects blend color but it needs a color, so set the color to 
   // the same as our game piece.
   %breakanim.color = %gamepiece.color;
   // Use our custom function to set the blend color
   %this.setObjectBlendColor(%breakanim, %alpha);
   // Our template piece may not be sized correctly, set it to the correct size.
   %breakanim.setSize($gameBoard::PieceSize, $gameBoard::PieceSize);
   // When you clone an object, it clones it's position, so we need to set it's position.
   // Calculate it's position based on location   
   %destX = (%this.startLocationX + (%gamepiece.locationX * $gameBoard::PieceSize));
   %destY = (%this.startLocationY + (%gamepiece.locationY * $gameBoard::PieceSize));
   // Set the position
   %breakanim.setPosition(%destX, %destY);
   // We also want a particle effect to play when a piece breaks.
   // I had a template particle, but I would rather just use the asset system to
   // make a new one.
   // Create a particle player
   %breakParticle = new ParticlePlayer();
   // Set the players particle to the one we want
   %breakParticle.Particle = "PuzzleToy:Sparkles";
   // We want the particle to be able to move, so we can set it's position.
   %breakParticle.ParticleInterpolation = true;
   // Set the particles players position to the same as the break clone
   %breakParticle.setPosition(%destX, %destY);
   // Now we have our break anim and our particle player, but we need to add them
   // to the scene.   
   SandboxScene.add(%breakParticle);
   SandboxScene.add(%breakanim);
   // Set bCheckBreaks to false so we don't check it again before it is removed.
   %gamepiece.bCheckBreaks = false;   
   // Clear out the location in the array of gamepieces.
   %this.gamePieces[%gamepiece.locationX, %gamepiece.locationY] = false;
   // Iterate through the pieces above to let them know they need to update. 
   // Otherwise they won't know to move down.  
   if ( %gamepiece.locationY < %this.cellCountY - 1 )
   {
      // get a handle on the piece above this one if there is one.
      %pieceAbove = %this.gamePieces[%gamepiece.locationX, %gamepiece.locationY + 1];
      // If there is a piece
      if (%pieceAbove)
      {
         // Let it know to update
         %pieceAbove.bUpdate = true;         
      }
   } 
   // In order to clean up the piece, we need to remove it from the scene.   
   %gamepiece.removeFromScene();   
   // If sound is enabled play the break sound.
   if ($soundEnabled)
      alxPlay("PuzzleToy:BreakSound"); 
}
///-----------------------------------------------------------------------------
/// Check the top row for empty slots and fill them
/// This is how we keep the board full.
///-----------------------------------------------------------------------------
function gameBoard::topFill(%this)
{
   // Init isFilling since we don't know if we are filling yet.
   %this.isFilling = false;
   // Iterate through the top pieces horizontally
   for (%i = 0; %i < %this.cellCountX; %i++)
   {
      // Check if there is a piece at the top in this row.
      if(%this.gamePieces[%i, %this.cellCountY - 1] == false)
      {                        
         // There isn't, so we need to create a new piece                  
         // clone  a new piece from our template pFront         
         %newpiece = pFront.clone(true);
         // Start it as invisible so we don't see it moving to it's starting
         // location.
         %newpiece.isVisible = false;
         // Piece will need to move to it's starting location, so set as moving.
         %newpiece.bMoving = true;         
         // Associate this gameboard witht he game piece.
         %newpiece.setGameBoard(%this);         
         // Assign this piece to this location in the game piece array.
         %this.gamePieces[%i, %this.cellCountY - 1] = %newpiece;
         // Set the pieces location so it knows where it is.
         %newpiece.locationX = %i;
         %newpiece.locationY = %this.cellCountY - 1;
         // We are using moveTo to move the piece around, so we don't want physics
         // to slow it down.
         // Set it's linear damping to 0.
         %newpiece.setLinearDamping(0.0);
         // And make it have no density so it will have no mass.
         %newpiece.setDefaultDensity(0.0, false);
         // Add the new piece to the scene.
         SandboxScene.add(%newpiece);
         // Now that we have a scene, we can set our starting location
         // We have a function in the game piece to do this.
         %newpiece.setStartLocation();
         // Looks like we are filling the top row, so set isFilling to true.          
         %this.isFilling = true;
      }
   }
}

///-----------------------------------------------------------------------------
/// Check if we are ready to check pieces
/// This returns true if no pieces are moving and the board has no empty 
/// locations in the game piece array
///-----------------------------------------------------------------------------
function gameBoard::getCheckReady(%this)
{
   // Iterate through all available locations
   for(%ix = 0; %ix < %this.cellCountX; %ix++)
   {      
      for (%iy = 0; %iy < %this.cellCountY; %iy++)
      {
         // Check if a piece exists. This will be false if not.
         if (%this.gamePieces[%ix, %iy])
         {
            // Check if the piece is moving and if so, we aren't ready, return false.
            if (%this.gamePieces[%ix, %iy].bMoving)
               return false;
         }
         else
         {
            // If the piece didn't exist, we need to fill, so return false.
            return false;
         }
      }
   }
   // If we make it here, then all is good and we are ready to check for breaks
   // so return true.   
   return true;
}

///-----------------------------------------------------------------------------
/// Get the list of pieces that needs to be checked.
/// Returns a simset of game pieces.
///-----------------------------------------------------------------------------
function gameBoard::getCheckList(%this)
{
   // clear our check list so we don't have any pieces left over from previous checks.
   %this.CheckList.clear();
   // Iterate through all the pieces
   for(%ix = 0; %ix < %this.cellCountX; %ix++)
   {
      for (%iy = 0; %iy < %this.cellCountY; %iy++)
      {
         // Check that there is a piece here.
         if (%this.gamePieces[%ix, %iy])
         {
            // If this piece needs to be checked, add it to the check list.
            if (%this.gamePieces[%ix, %iy].bCheckBreaks)
               %this.CheckList.add(%this.gamePieces[%ix, %iy]);               
         }
      }
   }
   // Return the check list
   return %this.CheckList;
}

///-----------------------------------------------------------------------------
/// Check if the game board has been completed
///-----------------------------------------------------------------------------
function gameBoard::checkDone(%this)
{
   // Iterate through all the canvas states
   for(%ix = 0; %ix < %this.cellCountX; %ix++)
   {
      for (%iy = 0; %iy < %this.cellCountY; %iy++)
      {
         // If any of these are true meaning visible, then we arent' done, so return.
         if (%this.canvasPieces[%ix, %iy])
            return;
      }
   }
   // If we get past this, we are done.
   // Set the board to complete so it won't update.
   $BoardComplete = true;   
   // Clear the game pieces
   %this.clearBoard();
   // Show the complete image by setting it's visibility
   bCompleteImage.setVisible(true);
   // Enable the next button so the player can progress to the next level.
   nButton.setEnabled(true);   
   // If sound is enabled
   if ($soundEnabled)
   {
      // Stop currently playing sound
      alxStopAll();
      // Play the win music.
      alxPlay("ToyAssets:winMusic");
   }   
}

///-----------------------------------------------------------------------------
/// Clear the current game piece selection
///-----------------------------------------------------------------------------
function gameBoard::clearSelected(%this)
{
   // We no longer have a selected piece
   %this.bPieceSelected = false;   
   // Let the game piece know it is no longer selected
   %this.selectedPiece.setSelected(false);
   // Clear out the selected piece
   %this.selectedPiece = "";
}

///-----------------------------------------------------------------------------
/// Clear the visibility of all the game pieces.
///-----------------------------------------------------------------------------
function gameBoard::clearBoard(%this)
{
   // Iterate through all the piece locations
   for(%ix = 0; %ix < %this.cellCountX; %ix++)
   {
      for (%iy = 0; %iy < %this.cellCountY; %iy++)
      {
         // If a piece exists, set it's visibility
         if (%this.gamePieces[%ix, %iy])
            %this.gamePieces[%ix, %iy].setVisible(false);                  
      }
   }
   // If the board is comlete, then all the canvas sprites should be invisible, 
   // but we need to set the canvas visibility to false in case this is a failure.
   bCanvas.setVisible(false);
}

///---------------------------------------------------------------------------------------------
/// Load the boards data
/// param %level - level string. world-level ie 1-1, 1-2, etc
/// This is hard coded data right now.
/// I intend to change this to data files that I can load but I just wanted to
/// get it working for now. 
///---------------------------------------------------------------------------------------------
function gameBoard::LoadBoard(%level)
{
   %boarddata = "";   
   switch$(%level)
   {
      case "1-1":
      // do stuff
      %boarddata = "0 0 4 4 4 0 4 4 0 0 4 4 0 0 4 4 0 0 4 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 0 4 4 4 4 0 0 0 0 0 4 4 4 0 0 0 0 0 0 0 0";      
      case "1-2":
      // do stuff
      %boarddata = "4 4 4 4 4 0 0 0 0 0 4 4 4 0 0 0 0 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 0 0 0 0 4 4 4 4 0 0 0 0 4 4 4 0 0 0 0 0 4 4 4 0 0 0 0 0 4 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0";      
      case "1-3":
      // do stuff
      %boarddata = "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0 0 4 0 0 0 3 0 0 0 4 4 3 4 0 0 0 4 4 4 4 4 4";      
      case "1-4":
      // do stuff
      %boarddata = "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 4 0 0 0 0 0 0 0 4 4 0 0 0 0 0 0 4 4 4 0 0 0 0 0 4 4 4 3 0 0 0 0 4 4 4 0 0 0 0 0 4 4 4 4 0 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0";      
      default:
      %boarddata = "0 0 4 4 4 0 4 4 0 0 4 4 0 0 4 4 0 0 4 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 4 4 4 4 4 0 0 0 0 4 4 4 4 0 0 0 0 0 4 4 4 0 0 0 0 0 0 0 0";      
            
   }
   
   return %boarddata;   
}

///-----------------------------------------------------------------------------
/// Create random board data
/// param %columns - number of columns
/// param %rows - number of rows
/// param %colors - number of colors to use
///-----------------------------------------------------------------------------
function gameBoard::createBoardData( %columns, %rows, %colors )
{
   %numberofpieces = %columns * %rows;
   %boarddata = "";
   for (%i=0; %i<%numberofpieces; %i++)
   {
      %pcolor = getRandom(0, %colors - 1);
      %boarddata = %boarddata @ %pcolor @ " ";
   }
   
   return %boarddata;
}