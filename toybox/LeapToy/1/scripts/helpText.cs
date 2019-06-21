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

function LeapToy::createHelpTextScene(%this, %helptext)
{
    %this.clearHelpTextScene();
      
    %this.helpTextScene = new Scene(HelpTextScene);

    // Create the background
    %background = new Sprite();
    %background.setSize( 40, 30 );
    %background.setImage("ToyAssets:skyBackground");
    %background.setSceneLayer(20);
    HelpTextScene.add(%background);
    
    // Check if we have text
    if (%helptext.getCount() > 0)
    {
        %startingy = 0 + ((%helptext.getCount()/2) *0.6);
        %spacing = 0.75;
        %textsize = 0.6;
        
        // Iterate through the text lines
        for (%x = 0; %x < %helptext.getCount(); %x++)
        {                  
            %object = new ImageFont();         
            %object.Image = "ToyAssets:Font";
            %object.FontSize = %textsize SPC %textsize;
            %object.TextAlignment = "Left";
            %positiony = %startingy - (%x * %spacing);
            %object.Position = "-17" SPC %positiony;
            %object.Text = %helptext.getObject(%x).Text;
            HelpTextScene.add(%object);
        }
        
        // Since we had text let's put up the help prompt.
        %helpPrompt = new ImageFont();       
        %helpPrompt.Image = "ToyAssets:Font";
        %helpPrompt.FontSize = "0.7 0.7";
        %helpPrompt.TextAlignment = "Center";      
        %helpPrompt.Position = "0 -12";
        %helpPrompt.Text = "Press H for help";
        SandboxScene.add(%helpPrompt);      
    }   
}

function LeapToy::toggleHelpTextScene(%this, %val)
{
    if (!isObject(HelpTextScene) || !%val)
        return;

    if (SandboxWindow.getScene() $= %this.helpTextScene)
        SandboxWindow.setScene(SandboxScene);    
    else
        SandboxWindow.setScene(HelpTextScene);    
}

function LeapToy::clearHelpTextScene(%this)
{
    if (!isObject(HelpTextScene))
        return;

    if (SandboxWindow.getScene() $= %this.helpTextScene)
        SandboxWindow.setScene(SandboxScene);    

    HelpTextScene.clear();
    HelpTextScene.delete();
}