///-------------------------------------------------------
/// Time Bar class to track the time remaining and update
/// The appropriate sprites
///-------------------------------------------------------

/// Callback when the TimeBar is added to a scene
function TimeBar::onAdd( %this )
{
   %this.TimeBack = bTimeBack;
   %this.TimeLeft = 1.0;
}

/// Updates the time bar based on the time left supplied
/// param %timeleft - float from 0.0 to 1.0
function TimeBar::UpdateTime( %this, %timeleft)
{
   // Set our time left
   %this.TimeLeft = %timeleft;
   // Get the width of the timebar
   %baseWidth = %this.getWidth();
   // Calculate the width of the back bar for time remaining.
   %newWidth = %baseWidth * %this.TimeLeft;
   // Set the width of the back bar.
   %this.TimeBack.setWidth(%newWidth);
   // Calculate the new position with an offset for the back bar so it is left justified.
   %newXPos = %this.getPositionX() - ((%baseWidth - %newWidth)/2);
   // Set the back bar's new position
   %this.TimeBack.setPositionX(%newXPos);
   // Update the back bars color
   %this.UpdateColor();
}

/// Update the color of the time bar based on the time left
function TimeBar::UpdateColor( %this )
{
   // check the percentage of time left ot set the color
   if (%this.TimeLeft >= 0.5)
   {
      // bar should be green
      %this.TimeBack.setBlendColor( "Green" );
   }
   else if (%this.TimeLeft >= 0.25 )
   {
      // bar should be yellow
      %this.TimeBack.SetBlendColor( "Yellow" );
   }
   else
   {
      // Bar should be red
      %this.TimeBack.setBlendColor( "Red" );
   }
}