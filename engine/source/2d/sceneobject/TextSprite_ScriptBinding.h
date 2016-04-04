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

ConsoleMethodGroupBeginWithDocs(TextSprite, SceneObject)

/*! Sets the image asset to use..
    @param fontName The font asset to use.
    @return Returns true on success.
*/
ConsoleMethodWithDocs(TextSprite, setFont, ConsoleBool, 3, 3, (fontAssetId))
{
    return object->setFont( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets current font asset..
    @return The current font asset.
*/
ConsoleMethodWithDocs(TextSprite, getFont, ConsoleString, 2, 2, ())
{
    return object->getFont();
}

//-----------------------------------------------------------------------------

/*! Sets the text to render.
*/
ConsoleMethodWithDocs(TextSprite, setText, ConsoleVoid, 3, 3, (text))
{
    object->setText(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Gets the text being rendered.
*/
ConsoleMethodWithDocs(TextSprite, getText, ConsoleString, 2, 2, ())
{
    return object->getText().getPtr8();
}

//-----------------------------------------------------------------------------

/*! Sets the size of the font.
@param size The distance between the top of a line of text and the bottom.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setFontSize, ConsoleVoid, 3, 3, (size))
{
   object->setFontSize(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the size of the font.
@return The size of the font.
*/
ConsoleMethodWithDocs(TextSprite, getFontSize, ConsoleFloat, 2, 2, ())
{
   return object->getFontSize();
}

//-----------------------------------------------------------------------------

/*! Sets the scale of the font in the X direction.
@param scale The amount to multiply the width of the text by.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setFontScaleX, ConsoleVoid, 3, 3, (scale))
{
   object->setFontScaleX(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the scale of the font in the X direction.
@return The scale of the of the font in the X direction.
*/
ConsoleMethodWithDocs(TextSprite, getFontScaleX, ConsoleFloat, 2, 2, ())
{
   return object->getFontScaleX();
}

//-----------------------------------------------------------------------------

/*! Sets the scale of the font in the Y direction.
@param scale The amount to multiply the height of the text by.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setFontScaleY, ConsoleVoid, 3, 3, (scale))
{
   object->setFontScaleY(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the scale of the font in the Y direction.
@return The scale of the of the font in the Y direction.
*/
ConsoleMethodWithDocs(TextSprite, getFontScaleY, ConsoleFloat, 2, 2, ())
{
   return object->getFontScaleY();
}

//-----------------------------------------------------------------------------

/*! Sets the horizontal text alignment to 'left', 'center', 'right', or 'justify'.
    @param alignment The text alignment of 'left', 'center', 'right', or 'justify'.
    @return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setTextAlignment, ConsoleVoid, 3, 3, (alignment))
{
    object->setTextAlignment( TextSprite::getTextAlignmentEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the horizontal text alignment.
    @return The text alignment of 'left', 'center', 'right', or 'justify'.
*/
ConsoleMethodWithDocs(TextSprite, getTextAlignment, ConsoleString, 2, 2, ())
{
   return TextSprite::getTextAlignmentDescription(object->getTextAlignment());
}

//-----------------------------------------------------------------------------

/*! Sets the vertical text alignment to 'top', 'middle', or 'bottom'.
@param alignment The vertical text alignment of 'top', 'middle', or 'bottom'.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setTextVAlignment, ConsoleVoid, 3, 3, (alignment))
{
   object->setTextVAlignment(TextSprite::getTextVAlignmentEnum(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the vertical text alignment.
@return The vertical text alignment of 'top', 'middle', or 'bottom'.
*/
ConsoleMethodWithDocs(TextSprite, getTextVAlignment, ConsoleString, 2, 2, ())
{
   return TextSprite::getTextVAlignmentDescription(object->getTextVAlignment());
}

//-----------------------------------------------------------------------------

/*! Sets the overflow mode X to 'wrap', 'visible', 'hidden', or 'shrink'.
@param mode The overflow mode X of 'wrap', 'visible', 'hidden', or 'shrink'.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setOverflowModeX, ConsoleVoid, 3, 3, (mode))
{
   object->setOverflowModeX(TextSprite::getOverflowModeXEnum(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the overflow mode X.
@return The overflow mode X of 'wrap', 'visible', 'hidden', or 'shrink'.
*/
ConsoleMethodWithDocs(TextSprite, getOverflowModeX, ConsoleString, 2, 2, ())
{
   return TextSprite::getOverflowModeXDescription(object->getOverflowModeX());
}

//-----------------------------------------------------------------------------

/*! Sets the overflow mode Y to 'visible', 'hidden', or 'shrink'.
@param mode The overflow mode Y of 'visible', 'hidden', or 'shrink'.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setOverflowModeY, ConsoleVoid, 3, 3, (mode))
{
   object->setOverflowModeY(TextSprite::getOverflowModeYEnum(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the overflow mode Y.
@return The overflow mode Y of 'visible', 'hidden', or 'shrink'.
*/
ConsoleMethodWithDocs(TextSprite, getOverflowModeY, ConsoleString, 2, 2, ())
{
   return TextSprite::getOverflowModeYDescription(object->getOverflowModeY());
}

//-----------------------------------------------------------------------------

/*! Sets if the line height should be automatically calculated.
@param isAuto True if the line height automatically calculated or false to use the custom line height.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setAutoLineHeight, ConsoleVoid, 3, 3, (isAuto))
{
   object->setAutoLineHeight(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets whether the line height is automatically calculated.
@return True if the line height is automatically calculated or false if the custom line height is used.
*/
ConsoleMethodWithDocs(TextSprite, getAutoLineHeight, ConsoleBool, 2, 2, ())
{
   return object->getAutoLineHeight();
}

//-----------------------------------------------------------------------------

/*! Sets the custom line height and disables the auto line height.
@param lineHeight The distance between the top and the bottom of a line before adjusting by the fontScaleY.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setCustomLineHeight, ConsoleVoid, 3, 3, (lineHeight))
{
   object->setCustomLineHeight(dAtof(argv[2]));
   object->setAutoLineHeight(false);
}

//-----------------------------------------------------------------------------

/*! Gets the custom line height.
@return The distance between the top and the bottom of a line.
*/
ConsoleMethodWithDocs(TextSprite, getCustomLineHeight, ConsoleFloat, 2, 2, ())
{
   return object->getCustomLineHeight();
}

//-----------------------------------------------------------------------------

/*! Sets kerning to be used between each character. Kerning is ignored when using alignment of 'justify'.
@param kerning The amount to decrease the distance by between each character. Positive valus move the characters closer together and negative values move them apart.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setKerning, ConsoleVoid, 3, 3, (kerning))
{
   object->setKerning(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the kerning amount.
@return The amount each character is moved closer together.
*/
ConsoleMethodWithDocs(TextSprite, getKerning, ConsoleFloat, 2, 2, ())
{
   return object->getKerning();
}

//-----------------------------------------------------------------------------

/*! Resets the blend color, scale, and offset for all characters'.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, resetCharacterSettings, ConsoleVoid, 2, 2, ())
{
   object->resetCharacterSettings();
}

//-----------------------------------------------------------------------------

/*! Sets the blend color for an individual character.
@param index The zero based index for the character.
@param color The blend color to use for character.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setCharacterBlendColor, ConsoleVoid, 4, 4, (index, color))
{
   if (argc < 3)
   {
      Con::warnf("TextSprite::setCharacterBlendColor() - Invalid number of parameters!");
      return;
   }

   const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
   if (colorCount != 4)
   {
      Con::warnf("TextSprite::setCharacterBlendColor() - Invalid color! Colors require four values (red / green / blue / alpha)!");
      return;
   }
   object->setCharacterBlendColor(dAtoi(argv[2]), ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)),
                                                         dAtof(Utility::mGetStringElement(argv[3], 1)),
                                                         dAtof(Utility::mGetStringElement(argv[3], 2)),
                                                         dAtof(Utility::mGetStringElement(argv[3], 3))));
}

//-----------------------------------------------------------------------------

/*! Gets the blend color for an individual character if set or the blend color for the sprite.
@param index The zero based index for the character.
@return The blend color for the character or the sprite.
*/
ConsoleMethodWithDocs(TextSprite, getCharacterBlendColor, ConsoleString, 3, 3, (index))
{
   U32 charNum = dAtoi(argv[2]);
   if (object->getCharacterHasBlendColor(charNum))
   {
      return object->getCharacterBlendColor(charNum).scriptThis();
   }
   else
   {
      return object->getBlendColor().scriptThis();
   }
}

//-----------------------------------------------------------------------------

/*! Gets if the character is using a custom blend color.
@param index The zero based index for the character.
@return True if the character is using a custom blend color or false if the character uses the sprite's blend color.
*/
ConsoleMethodWithDocs(TextSprite, getCharacterHasBlendColor, ConsoleBool, 3, 3, (index))
{
   return object->getCharacterHasBlendColor(dAtoi(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Resets the blend color for a character so that it uses the sprite's blend color.
@param index The zero based index for the character.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, resetCharacterBlendColor, ConsoleVoid, 3, 3, (index))
{
   object->resetCharacterBlendColor(dAtoi(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Sets the scale of a given character. This is multiplied by the scale of the font to get the total scale.
@param index The zero based index for the character.
@param (scaleX / scaleY) The amount to multiply the size of the character by. Default value is 1.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setCharacterScale, ConsoleVoid, 4, 4, (index, scale scaleX / scaleY))
{
   U32 charNum = dAtoi(argv[2]);

   const U32 count = Utility::mGetStringElementCount(argv[3]);
   if (count != 2)
   {
      Con::warnf("TextSprite::setCharacterScale() - Invalid number of values (scaleX / scaleY)!");
      return;
   }

   object->setCharacterScale(charNum, dAtof(Utility::mGetStringElement(argv[3], 0)), dAtof(Utility::mGetStringElement(argv[3], 1)));
}

//-----------------------------------------------------------------------------

/*! Gets scale of a given character.
@param index The zero based index for the character.
@return (scaleX / scaleY) The scale of the character.
*/
ConsoleMethodWithDocs(TextSprite, getCharacterScale, ConsoleString, 3, 3, (index))
{
   return object->getCharacterScale(dAtoi(argv[2])).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Resets the scale of a given character.
@param index The zero based index for the character.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, resetCharacterScale, ConsoleVoid, 3, 3, (index))
{
   object->resetCharacterScale(dAtoi(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Sets the offset of a given character.
@param index The zero based index for the character.
@param (offsetX / offsetY) The amount to move a character from it's calculated position.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, setCharacterOffset, ConsoleVoid, 4, 4, (index, offset offsetX / offsetY))
{
   U32 charNum = dAtoi(argv[2]);

   const U32 count = Utility::mGetStringElementCount(argv[3]);
   if (count != 2)
   {
      Con::warnf("TextSprite::setCharacterOffset() - Invalid number of values (offsetX / offsetY)!");
      return;
   }

   object->setCharacterOffset(charNum, dAtof(Utility::mGetStringElement(argv[3], 0)), dAtof(Utility::mGetStringElement(argv[3], 1)));
}

//-----------------------------------------------------------------------------

/*! Gets offset of a given character.
@param index The zero based index for the character.
@return (offsetX / offsetY) The offset of the character.
*/
ConsoleMethodWithDocs(TextSprite, getCharacterOffset, ConsoleString, 3, 3, (index))
{
   return object->getCharacterOffset(dAtoi(argv[2])).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Resets the offset of a given character.
@param index The zero based index for the character.
@return No return value.
*/
ConsoleMethodWithDocs(TextSprite, resetCharacterOffset, ConsoleVoid, 3, 3, (index))
{
   object->resetCharacterOffset(dAtoi(argv[2]));
}

ConsoleMethodGroupEndWithDocs(TextSprite)