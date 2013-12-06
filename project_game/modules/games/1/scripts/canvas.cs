
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// initializeCanvas
// Constructs and initializes the default canvas window.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

$canvasCreated = false;
function initializeCanvas(%windowName)
{
  // Don't duplicate the canvas.
  if($canvasCreated)
  {
    error("Cannot instantiate more than one canvas!");
    return;
  }

  videoSetGammaCorrection($pref::OpenGL::gammaCorrection);

  if ( !createCanvas(%windowName) )
  {
    error("Canvas creation failed. Shutting down.");
    quit();
  }

  $pref::iOS::ScreenDepth = 32;

  if ( $pref::iOS::DeviceType !$= "" )
  {
    %resolution = iOSResolutionFromSetting($pref::iOS::DeviceType, $pref::iOS::ScreenOrientation);
  }
  else if ($platform $= "Android")
  {
    %resolution = GetAndroidResolution();
  }
  else  //$platform $= "windows" || $platform $= "macos"
  {
    %resolution = $pref::Video::windowedRes;  
    %resolution._0 = %resolution._0 / 2;   
    %resolution._1 = %resolution._1 / 2;  
  }

  setScreenMode( %resolution._0, %resolution._1, %resolution._2, false );
  $canvasCreated = true;

  // Now that we have a Canvas, we need a viewport into the scene.
  // Give it a global name "mainWindow" since we may want to access it directly in our scripts.
  new SceneWindow(mainWindow);
  mainWindow.profile = new GuiControlProfile();
  Canvas.setContent(mainWindow);

  // Set the canvas color
  Canvas.BackgroundColor = "CornflowerBlue";
  Canvas.UseBackgroundColor = true;

  // Finally, connect our scene into the viewport (or sceneWindow).
  // Note that a viewport comes with a camera built-in.
  mainWindow.setCameraPosition( 0, 0 );
  mainWindow.setCameraSize( %resolution._0, %resolution._1 );
  mainWindow.setUseObjectInputEvents(true);

  if ($platform $= "Android")
    hideSplashScreen();

  echo ( "platform: ", $platform);
  echo ( "resolution: ", %resolution._0, "x", %resolution._1, "x", %resolution._2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// resetCanvas
// Forces the canvas to redraw itself.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function resetCanvas()
{
  if (isObject(Canvas))
    Canvas.repaint();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iOSResolutionFromSetting
// Helper function that grabs resolution strings based on device type
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function iOSResolutionFromSetting( %deviceType, %deviceScreenOrientation )
{
  // A helper function to get a string based resolution from the settings given.
  %x = 0;
  %y = 0;

  %scaleFactor = $pref::iOS::RetinaEnabled ? 2 : 1;

  switch(%deviceType)
  {
  case $iOS::constant::iPhone:
    if(%deviceScreenOrientation == $iOS::constant::Landscape)
    {
      %x =  $iOS::constant::iPhoneWidth * %scaleFactor;
      %y =  $iOS::constant::iPhoneHeight * %scaleFactor;
    }
    else
    {
      %x =  $iOS::constant::iPhoneHeight * %scaleFactor;
      %y =  $iOS::constant::iPhoneWidth * %scaleFactor;
    }

  case $iOS::constant::iPad:
    if(%deviceScreenOrientation == $iOS::constant::Landscape)
    {
      %x =  $iOS::constant::iPadWidth * %scaleFactor;
      %y =  $iOS::constant::iPadHeight * %scaleFactor;
    }
    else
    {
      %x =  $iOS::constant::iPadHeight * %scaleFactor;
      %y =  $iOS::constant::iPadWidth * %scaleFactor;
    }

  case $iOS::constant::iPhone5:
    if(%deviceScreenOrientation == $iOS::constant::Landscape)
    {
      %x =  $iOS::constant::iPhone5Width;
      %y =  $iOS::constant::iPhone5Height;
    }
    else
    {
      %x =  $iOS::constant::iPhone5Height;
      %y =  $iOS::constant::iPhone5Width;
    }
  }

  return %x @ " " @ %y;
}
