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

function BlankGame::create( %this )
{
    // We need a main "Scene" we can use as our game world.  The place where sceneObjects play.
    // Give it a global name "mainScene" since we may want to access it directly in our scripts.
    new Scene(mainScene);

    // Without a system window or "Canvas", we can't see or interact with our scene.
    // AppCore initialized the Canvas already

    // Now that we have a Canvas, we need a viewport into the scene.
    // Give it a global name "mainWindow" since we may want to access it directly in our scripts.
    new SceneWindow(mainWindow);
    mainWindow.profile = new GuiControlProfile();
    Canvas.setContent(mainWindow);

    // Finally, connect our scene into the viewport (or sceneWindow).
    // Note that a viewport comes with a camera built-in.
    mainWindow.setScene(mainScene);
    mainWindow.setCameraPosition( 0, 0 );
    mainWindow.setCameraSize( 100, 75 );

    // load some scripts and variables
    // exec("./scripts/someScript.cs");

    // let's do a little something to make sure we are up and running.
    // write "hello world!"  :)
    %this.sayHello();
}

//-----------------------------------------------------------------------------

function BlankGame::destroy( %this )
{
}

//-----------------------------------------------------------------------------


function BlankGame::sayHello( %this )
{
    %phrase = new ImageFont();
    %phrase.Image = "BlankGame:Font";
       
    // Set the font size in both axis.  This is in world-units and not typicaly font "points".
    %phrase.FontSize = "2 2";
    
    %phrase.TextAlignment = "Center";
    %phrase.Text = "Hello, World!";
    mainScene.add( %phrase );    
}
