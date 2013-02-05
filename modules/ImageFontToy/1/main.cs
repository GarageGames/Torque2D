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

function ImageFontToy::create( %this )
{
    // Reset the toy.    
    ImageFontToy.reset();
}

//-----------------------------------------------------------------------------

function ImageFontToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function ImageFontToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
            
    // Create the image font.
    %object = new ImageFont();
    
    // Always try to configure a scene-object prior to adding it to a scene for best performance.
    
    // Set the image font to use the font image asset.
    %object.Image = "ToyAssets:Font";
    
    // We don't really need to do this as the position is set to zero by default.
    %object.Position = "0 0";
    
    // We don't need to size this object as it sizes automatically according to the alignment, font-size and text.
   
    // Set the font size in both axis.  This is in world-units and not typicaly font "points".
    %object.FontSize = "2 2";
    
    // We don't really need to do this as the padding is set to zero by default.
    // Padding is specified in world-units and relates to the space added between each character.
    %object.FontPadding = 0;

    // Set the text alignment.
    %object.TextAlignment = "Center";

    // Set the text to display.
    %object.Text = " !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`~abcdefghijklmnopqrstuvwxyz";

    // Make the text spin just to make it more interesting!
    %object.AngularVelocity = 30;
    
    // The ImageFont is a type that defaults to a "static" body-type (typically so it's not affected by gravity)
    // but we want this to spin so we need a "dynamic" body-type/
    %object.BodyType = "dynamic";
    
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}
