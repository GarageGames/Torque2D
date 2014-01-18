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

ConsoleMethod(TextObject, setText, void, 3, 3,   "(text) - Set the text to render.\n")
{
  object->setText(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethod( TextObject, addFontSize, bool, 3, 3, "(S32 fontSize)\n"
  "Adds a font size to the text object. Additional font sizes can make the font "
  "look better at more display sizes and resolutions."
  "@param fontSize The font size to add to the text object.\n"
  "@return Whether or not the creation of the font was successful." )
{
  return object->addFontSize( dAtoi( argv[2] ) );
}

//------------------------------------------------------------------------------

ConsoleMethod( TextObject, removeFontSize, void, 3, 3, "(S32 fontSize)\n"
  "Removes a font from the text object.\n"
  "@param fontSize The font size to remove from the text object.\n"
  "@return No return value." )
{
  object->removeFontSize( dAtoi( argv[2] ) );
}

//------------------------------------------------------------------------------

ConsoleMethod( TextObject, removeAllFontSizes, void, 2, 2, "()\n"
  "Removes all of the font sizes from the text object. Effectively, "
  "all fonts will be removed, so, nothing will be rendered until a font size "
  "is added.\n"
  "@return No return value." )
{
  object->removeAllFontSizes();
}

//------------------------------------------------------------------------------

ConsoleMethod( TextObject, addAutoFontSize, bool, 5, 5, "(S32 screenHeight, F32 cameraHeight, F32 characterHeight)\n"
  "Adds a font size based on a screen height, camera height, and character height. The resulting font "
  "size is automatically calculated.\n"
  "@param screenHeight The height of the screen to generate the font for.\n"
  "@param cameraHeight The height of the camera to generate the font for.\n"
  "@param characterHeight The height of the text in world units to generate the size for.\n"
  "@return Whether or not the font was created successfully." )
{
  S32 screenHeight = dAtoi( argv[2] );
  F32 cameraHeight = dAtof( argv[3] );
  F32 characterHeight = dAtof( argv[4] );

  // [neo, 5/7/2007 - #3001]
  // Make sure camera size, etc are valid or addFontSize() will do nasty things and crash
  // as well as the obv div by zero.
  if( cameraHeight > 0 && screenHeight > 0 && characterHeight > 0 )
    return object->addFontSize( S32( characterHeight / cameraHeight * screenHeight ) );

  Con::errorf( "TextObject::addAutoFontSize() - invalid screenHeight, cameraHeight or characterHeight" );

  return false;
}

