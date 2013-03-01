//------------------------------------------------------------------------------
// Gel Piece Class
//------------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/// onAdd called when the object is added to a scene.
///-----------------------------------------------------------------------------
function gamePiece::onAdd(%this)
{
   // default the color to 0 or black.	
	%this.color = 0;
	// default the location to 0, 0.
	%this.locationX = 0;
	%this.locationY = 0;
	// clear the gameBoard reference.  this will be set by the game board.
	%this.gameBoard = "";
	// We need to track when we set our color, so start bColorSet as false.
	%this.bColorSet = false;	
	// We also need to track when we set our gameboard so we don't try to reference
	// it until its set. Set bGameBoardSet to false.
	%this.bGameBoardSet = false;
	// This piece should start as selected, so set bSelected to false.
	%this.bSelected = false;	
	// When created, we have to move our piece to the proper location, so start bMoving as true.	
	%this.bMoving = true;
	// Since we should be moving, we shouldn't check for breaks.  Set bCheckBreaks to false.
	%this.bCheckBreaks = false;
	// Since there isn't a previous position, set this to 0,0.
	%this.previousPosition = 0 SPC 0;
	// initialize the current delta values
	%this.currentDeltaX = 0;
	%this.currentDeltaY = 0;	
	
}
///-----------------------------------------------------------------------------
/// get the color string from the color index
/// param %colorNumber - color index
/// returns the color name as a string
///-----------------------------------------------------------------------------
function gamePiece::getColor(%this, %colorNumber)
{
   // get the field at the supplied index
   return getField(PuzzleToy.colors, %colorNumber);
   
}
///-----------------------------------------------------------------------------
/// set the game pieces blend color based on it's color index
///-----------------------------------------------------------------------------
function gamePiece::setPieceBlendColor(%this)
{
   // default the alpha to 1.0 in case this piece isn't selected
   %alpha = 1;
   // if we are selected set the alpha to 0.75.
   if (%this.bSelected)   
      %alpha = 0.75;
   // set the blend color to white with the supplied alpha
   %this.setBlendColor(1.0, 1.0, 1.0, %alpha);   
   // Our game piece uses an image with multiple frames for the different colors
   // set the frame to the color index.
   %this.setImageFrame( %this.color );   
}
///-----------------------------------------------------------------------------
/// Set the object to selected or not so it renders correctly
/// Param %bSelect - Whether or not the piece is selected
///-----------------------------------------------------------------------------
function gamePiece::setSelected(%this, %bSelect)
{
   // Set whether or not we are selected based on the supplied bool
   %this.bSelected = %bSelect;
   // Update the blend color so we can update the apha.
   %this.setPieceBlendColor();
}
///-----------------------------------------------------------------------------
/// update function for the game piece
/// This is called from the gameBoard if it needs to be updated.
///-----------------------------------------------------------------------------
function gamePiece::myUpdate(%this)
{
   // if we aren't moving, check if we should move down.
   if (!%this.bMoving)
   {
      // check if we should move down.
      %this.checkDownMove();
   }
   
   // since we are updating, we shouldn't update again until something changes
   %this.bUpdate = false;
}
///-----------------------------------------------------------------------------
/// Pick a random color index for the object
///-----------------------------------------------------------------------------
function gamePiece::pickColor(%this)
{
   // set tempcolor to 0 so we can use the while loop
   %tempcolor = 0;
   // I want a good amount of randomness, so I'm going to use a much larger range
   // for my random numbers, this means I need to know what to divide the result
   // by to get my actual color index.   
   %divisor = 1000/%this.gameBoard.ColorCount;
   // while the color is 0 or less, keep trying for a good color
   while (%tempcolor <= 0)
   {
      // pick a random number and divide by our divisor.  This should be
      // a number between 1 and our color count.
      %tempcolor = mCeil(getRandom(0, 1000)/%divisor);  
      
   }
   // set the color to the random number minus 1.  I was getting alot of 0's so 
   // I do this to ignore any 0 values.  But we have a color that is zero so I have
   // to offset my results.
   %this.color = %tempcolor - 1;
   // Set the pieces color based on the random color we just got.
   %this.setPieceBlendColor();
   // We have picked a color, so set bColorSet to true.
   %this.bColorSet = true;   
}
///-----------------------------------------------------------------------------
/// Set the gameboard reference and choose the pieces color
/// param %gameboard - the gameBoard this piece belongs to.
///-----------------------------------------------------------------------------
function gamePiece::setGameBoard(%this, %gameboard)
{
   // Set our gameBoard reference to the supplied gameboard.
   // This is so I can access the gameboard that this piece is associated with.
   %this.gameBoard = %gameboard;
   // We have set our gameboard reference, so set this to true.
   %this.bGameBoardSet = true;    
   // Now that the gameboard is set, we need to pick a color for this piece.
	%this.pickColor();
}
///-----------------------------------------------------------------------------
/// update the pieces target location based on it's location on the game board
///-----------------------------------------------------------------------------
function gamePiece::updateTargetLocation(%this)
{
   // If we have already set up our gameboard then it's okay to update our target 
   // location. This is just a safeguard to keep us from accessing the gameBoard 
   // until we know what gameboard we are attached to.
   if(%this.bGameBoardSet)
   {
      // Get our current X and Y positions
      %currentX = %this.getPositionX();
      %currentY = %this.getPositionY();
      // Calculate where we should be based on our locationX and Y indexes.      
      %this.destX = (%this.gameBoard.startLocationX + (%this.locationX * %this.gameBoard.PieceSize));
      %this.destY = (%this.gameBoard.startLocationY + (%this.locationY * %this.gameBoard.PieceSize));
      // Create a vector2 for our target destination.
      %targetDest = %this.destX SPC %this.destY;
      // Move to our target destination
      %this.moveTo( %targetDest, 30 );
      // Since we are moving, set bMoving to true.
      %this.bMoving = true;
      // Are we already at our target destination?
      if (%this.destX $= %currentX && %this.destY $= %currentY)
      {
         // Check if we should move down.
         %this.checkDownMove();         
      }
   }
}
///-----------------------------------------------------------------------------
/// moveToComplete callback
/// Called when the alotted time passes from a moveTo command
///-----------------------------------------------------------------------------
function gamePiece::onMoveToComplete(%this)
{   
   // When we move, if we are swapping, we need to put one piece above and one
   // below, so we set the scene layer on one to 0 and the other to 1.
   // Once the move is complete, we set them back to 1.
   %this.setSceneLayer(1);
   // We finished moving, so set bMoving to false.
   %this.bMoving = false;
   // The rest we only want to do if we aren't paused
   if (SandboxScene.getScenePause())
   {
      %this.bUpdate = true;
      %this.bCheckBreaks = true;
      
      return;
   }
   // Check if we need to move down.
   %this.checkDownMove();
   // If after checking we are still not moving, we need to check this piece
   // for matches, so set bCheckBreaks to true.
   if (!%this.bMoving)
      %this.bCheckBreaks = true; 
}
///-----------------------------------------------------------------------------
/// Check the location below this piece on the gameBoard and move down if
/// there isn't a piece there.
///-----------------------------------------------------------------------------
function gamePiece::checkDownMove(%this)
{
   // If our current location is such that it could move down
   // and there is no piece below this piece then we need to move down.
   if (%this.locationY > 0 && %this.gameBoard.gamePieces[%this.locationX, %this.locationY - 1] == false)
   {
      // If this piece is not at the top of the board, and there is a piece
      // above this one, let it know it needs to update.
      if (%this.locationY + 1 < %this.gameBoard.cellCountY && %this.gameBoard.gamePieces[%this.locationX, %this.locationY + 1])
      {
         // Set bUpdate on the piece above this one to true.
         %this.gameBoard.gamePieces[%this.locationX, %this.locationY + 1].bUpdate = true;
      }      
      // Move to next slot down in the array.
      %this.gameBoard.gamePieces[%this.locationX, %this.locationY - 1] = %this;
      // Clear out where we used to be so the piece above knows that slot is empty.
      %this.gameBoard.gamePieces[%this.locationX, %this.locationY] = false;    
      // Decrease our locationY variable so we know where we are in the array.
      %this.locationY -= 1;
      // Now that we are in a new location, update our target location.
      %this.updateTargetLocation();
      // Since we are moving, we shouldn't be selected, so if we are then let the
      // gameBoard know to clear the selection.
      if (%this.bSelected)
         %this.gameBoard.clearSelected();
   }
}
///-----------------------------------------------------------------------------
/// Set the location we start at based on the board, piece size, and locationX
///-----------------------------------------------------------------------------
function gamePiece::setStartLocation(%this)
{
   // Calculate our starting destX and destY.
   %this.destX = (%this.gameBoard.startLocationX + (%this.locationX * %this.gameBoard.PieceSize));
   %this.destY = (%this.gameBoard.startLocationY + ((%this.locationY + 1) * %this.gameBoard.PieceSize));
   // Set our position
   %this.setPosition(%this.destX, %this.destY); 
   // Clear out our linear velocity.
   %this.setLinearVelocity(0,0);
   // We shouldn't start out moving since we hard set the position, so set
   // bMoving to false.   
   %this.bMoving = false;
   // the game piece that this is cloned from starts out invisible, so now that 
   // we are at our correct position, set this to visible.
   %this.isVisible = true;
   // Our starting location is not actually in the correct location because we
   // want the piece to slide down into it's correct location.
   // Update the target location so it can find out where it needs to be.
   %this.updateTargetLocation();
}
///-----------------------------------------------------------------------------
/// onTouchDown input callback for the game piece.
///-----------------------------------------------------------------------------
function gamePiece::onTouchDown(%this, %modifier, %worldPosition, %clicks)
{
   // if we are paused just return
   if (SandboxScene.getScenePause())
      return;
      
   // Tell the gameboard to select this piece.
   %this.gameBoard.setSelectedPiece(%this);
   // Store previous position to initialize dragging calculations
   %this.previousPosition = %worldPosition;
   // Clear out our current for dragging calculations
   %this.currentDeltaX = 0.0;
   %this.currentDeltaY = 0.0;
}
///-----------------------------------------------------------------------------
/// onTouchDragged input callback for the game piece.
///-----------------------------------------------------------------------------
function gamePiece::onTouchDragged(%this, %modifier, %worldPosition, %clicks)
{
   // if we are paused then return
   if (SandboxScene.getScenePause())
      return;
      
   // If I'm dragging on a piece that's selected check if we dragged far enough
   if (%this.bSelected)
   {
      // get the drag delta by using the previousPosition and the current worldPosition.
      %deltaX = getWord(%this.previousPosition, 0) - getWord(%worldPosition, 0);
      %deltaY = getWord(%this.previousPosition, 1) - getWord(%worldPosition, 1);
      // Add the new delta to the overall currentDelta values.
      %this.currentDeltaX += %deltaX;
      %this.currentDeltaY += %deltaY;
      // If our total delta is high enough in either direction, attempt to swap
      if (mAbs(%this.currentDeltaX) > 1.0 || mAbs(%this.currentDeltaY) > 1.0)
      {
         // If the deltaX is larger, move in the X direction. Otherwise move in the Y direction
         if (mAbs(%deltaX) > mAbs(%deltaY))
         {
            // If the current delta is positive, select left, otherwise select right
            if (%this.currentDeltaX > 0 && %this.locationX > 0)
            {
               %this.gameBoard.setSelectedPiece(%this.gameBoard.gamePieces[%this.locationX - 1, %this.locationY]);
            }
            else if (%this.locationX < %this.gameBoard.cellCountX - 1)
            {
               %this.gameBoard.setSelectedPiece(%this.gameBoard.gamePieces[%this.locationX + 1, %this.locationY]);
            }
         }
         else
         {
            // If the current delta is positive, select down, otherwise select up.
            if (%this.currentDeltaY > 0 && %this.locationY > 0)
            {
               %this.gameBoard.setSelectedPiece(%this.gameBoard.gamePieces[%this.locationX, %this.locationY - 1]);               
            }
            else if (%this.locationY < %this.gameBoard.cellCountY - 1)
            {
               %this.gameBoard.setSelectedPiece(%this.gameBoard.gamePieces[%this.locationX, %this.locationY + 1]);
            }
         }
         // Now that we selected, reset the currentDeltas.
         %this.currentDeltaX = 0;
         %this.currentDeltaY = 0;
         // We should no longer be selected so set bSelected to false.
         %this.bSelected = false;
      }
      // Track the new position as the previous position for later calculations.
      %this.previousPosition = %worldPosition;
   }
   
}

