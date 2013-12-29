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

if($platform $= "macos")
{
    new GuiCursor(DefaultCursor)
    {
        hotSpot = "4 4";
        renderOffset = "0 0";
        bitmapName = "^EditorAssets/data/images/macCursor";
    };
} 
else 
{
    new GuiCursor(DefaultCursor)
    {
        hotSpot = "1 1";
        renderOffset = "0 0";
        bitmapName = "^EditorAssets/data/images/macCursor";
    };
}

new GuiCursor(LeftRightCursor)
{
    hotSpot = "0.5 0";
    renderOffset = "0.5 0";
    bitmapName = "^EditorAssets/data/images/leftRight";
};

new GuiCursor(UpDownCursor)
{
    hotSpot = "1 1";
    renderOffset = "0 1";
    bitmapName = "^EditorAssets/data/images/upDown";
};

new GuiCursor(NWSECursor)
{
    hotSpot = "1 1";
    renderOffset = "0.5 0.5";
    bitmapName = "^EditorAssets/data/images/NWSE";
};

new GuiCursor(NESWCursor)
{
    hotSpot = "1 1";
    renderOffset = "0.5 0.5";
    bitmapName = "^EditorAssets/data/images/NESW";
};

new GuiCursor(MoveCursor)
{
    hotSpot = "1 1";
    renderOffset = "0.5 0.5";
    bitmapName = "^EditorAssets/data/images/move";
};

new GuiCursor(TextEditCursor)
{
    hotSpot = "1 1";
    renderOffset = "0.5 0.5";
    bitmapName = "^EditorAssets/data/images/textEdit";
};

//---------------------------------------------------------------------------------------------
// Cursor toggle functions.
//---------------------------------------------------------------------------------------------
$cursorControlled = true;
function showCursor()
{
    Canvas.cursorOn();
}

function hideCursor()
{
    Canvas.cursorOff();
}

//---------------------------------------------------------------------------------------------
// In the CanvasCursor package we add some additional functionality to the built-in GuiCanvas
// class, of which the global Canvas object is an instance. In this case, the behavior we want
// is for the cursor to automatically display, except when the only guis visible want no
// cursor - usually the in game interface.
//---------------------------------------------------------------------------------------------
package CanvasCursorPackage
{

//---------------------------------------------------------------------------------------------
// checkCursor
// The checkCursor method iterates through all the root controls on the canvas checking each
// ones noCursor property. If the noCursor property exists as anything other than false or an
// empty string on every control, the cursor will be hidden.
//---------------------------------------------------------------------------------------------
function GuiCanvas::checkCursor(%this)
{
    %count = %this.getCount();
    for (%i = 0; %i < %count; %i++)
    {
        %control = %this.getObject(%i);
        
        if ((%control.noCursor $= "") || !%control.noCursor)
        {
            showCursor();
            return;
        }
    }
    
    // If we get here, every control requested a hidden cursor, so we oblige.
    hideCursor();
}

//---------------------------------------------------------------------------------------------
// The following functions override the GuiCanvas defaults that involve changing the content
// of the Canvas. Basically, all we are doing is adding a call to checkCursor to each one.
//---------------------------------------------------------------------------------------------
function GuiCanvas::setContent(%this, %ctrl)
{
    Parent::setContent(%this, %ctrl);
    %this.checkCursor();
}

function GuiCanvas::pushDialog(%this, %ctrl, %layer)
{
    Parent::pushDialog(%this, %ctrl, %layer);
    %this.checkCursor();
}

function GuiCanvas::popDialog(%this, %ctrl)
{
    Parent::popDialog(%this, %ctrl);
    %this.checkCursor();
}

function GuiCanvas::popLayer(%this, %layer)
{
    Parent::popLayer(%this, %layer);
    %this.checkCursor();
}

};

activatePackage(CanvasCursorPackage);
