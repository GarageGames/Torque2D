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

function EditorMenuBar::initialize( %this )
{
   %this.clearMenus();
   %this.menuCount = 0;
}

function EditorMenuBar::addMenu( %this, %name )
{
   Parent::addMenu( %this, %name, %this.menuCount );
   %this.menuCount++;
   %this.itemCount[%name] = 0;
}

function EditorMenuBar::addMenuItem( %this, %menu, %text, %command, %hotkey )
{
   %item = %this.itemCount[%menu];
   
   Parent::addMenuItem( %this, %menu, %text, %item, %hotkey );
   %this.scriptCommand[%menu, %item] = %command;
   %this.itemCount[%menu]++;
   
   return %item;
}


function detachMenuBars()
{
   %content = Canvas.getContent();
   if( !isObject( %content ) || !isObject( %content.menuGroup ) )
      return;
      
   if( %content.isMethod( "detachMenuGroup" ) )
      %content.detachMenuGroup();
}

function attachMenuBars()
{
   %content = Canvas.getContent();
   if( !isObject( %content ) || !isObject( %content.menuGroup ) )
      return;
      
   if( %content.isMethod( "attachMenuGroup" ) )
      %content.attachMenuGroup();   
}