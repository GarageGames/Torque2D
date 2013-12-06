// games.cs
// (c) Pedro Vicente
// Notes: folders are 
// /assets/images -- where we store images
// /assets/fonts  -- where we store fonts
// /scripts       -- game script code



if(!isObject(GuiDefaultProfile)) new GuiControlProfile (GuiDefaultProfile)
{
  Modal = true;
}; 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//games::create
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function games::create( %this )
{
  exec("./scripts/canvas.cs");
  exec("./scripts/constants.cs");
  exec("./scripts/default_preferences.cs");
  exec("./scripts/openal.cs");
 
  // Initialize the canvas
  initializeCanvas("window_name");

  %this.createScene(TextScene);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//games::destroy
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function games::destroy( %this )
{
  %scene_obj = mainWindow.getScene();
  %scene_obj.delete();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//games::createScene
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function games::createScene(%this, %scene_obj)
{
  %this.clearScene();
  new Scene(%scene_obj);
  mainWindow.setScene(%scene_obj);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//games::clearScene
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function games::clearScene(%this)
{  
  %scene_obj = mainWindow.getScene();
  if (isObject(%scene_obj))
  {
    %scene_obj.clear(false); //false: do not delete objects, otherwise engine asserts when called on a callback
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//games::createRectangle
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function games::createRectangle( %this, %scene_id, %x_size, %y_size)
{	
  %obj = new ShapeVector() 
  { 
    Scene = %scene_id; 
  };	
  %obj.setPolyCustom( 4, "-1 -1 1 -1 1 1 -1 1" );
  %obj.setSize( %x_size, %y_size );
  %obj.setLineColor( "0 0 0 1" );
  %obj.setFillMode( true );
  %obj.setFillColor( "1 0 0" );
  %obj.setFillAlpha( 0.6 );
  %obj.setSceneLayer( 0 );
  return %obj;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//games::MakeTextObject (t2dTextObject)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function games::MakeTextObject( %this, %scene_ID, %size, %text )
{
  %obj = new TextObject()
  { 
    Scene                   = %scene_ID; 
    textAlign               = "Center";
    font                    = "Times New Roman Bold";
    hideOverflow            = false;
    autoSize                = true;
    Visible                 = true;
    blendIgnoreTextureAlpha = "0";
    wordWrap                = "0";
    hideOverflow            = "0";
    aspectRatio             = "1";
    lineSpacing             = "0";
    characterSpacing        = "0";
    autoSize                = "1";
    filter                  = "1";
    integerPrecision        = "1";
    noUnicode               = "0";
    hideOverlap             = "0";        
  };

  %obj.removeAllFontSizes();
  %fontsize = 48;    
  %obj.addFontSize( %fontsize );
  %obj.LineHeight = %fontsize + 10; 
  //call  setSize after LineHeight
  %obj.setSize( %size );
  %obj.setBlendColor( 0.0, 0.0, 0.0, 1.0 );    
  %obj.setSceneLayer( 5 );
  %obj.setText(%text);

  //add to scene
  %obj.addToScene(%scene_ID);
  return %obj;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TextScene::OnAdd 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function TextScene::OnAdd(%this)
{
  %obj = games.MakeTextObject(%this,40,"Torque rules");
  %obj.SetPosition( 0, 100 );
  
  %obj = games.createRectangle(%this,30,30);
  %obj.SetPosition( 0, 100 );
  
}



