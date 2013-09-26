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

ConsoleMethodGroupBeginWithDocs(ImageFont, SceneObject)

/*! Sets the image asset to use..
    @param imageName The image asset to use.
    @return Returns true on success.
*/
ConsoleMethodWithDocs(ImageFont, setImage, ConsoleBool, 3, 3, (imageAssetId))
{
    // Set Image.
    return object->setImage( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets current image asset..
    @return The current image asset.
*/
ConsoleMethodWithDocs(ImageFont, getImage, ConsoleString, 2, 2, ())
{
    // Get Image.
    return object->getImage();
}

//-----------------------------------------------------------------------------

/*! Set the text to render.
*/
ConsoleMethodWithDocs(ImageFont, setText, ConsoleVoid, 3, 3, (text))
{
    object->setText(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Gets the text being rendered.
*/
ConsoleMethodWithDocs(ImageFont, getText, ConsoleString, 2, 2, ())
{
    return object->getText().getPtr8();
}

//-----------------------------------------------------------------------------

/*! Set the text alignment to 'left', 'center' or 'right'.
    @param alignment The text alignment of 'left', 'center' or 'right'.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageFont, setTextAlignment, ConsoleVoid, 3, 3, (alignment))
{

    object->setTextAlignment( ImageFont::getTextAlignmentEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the text alignment.
    @return The text alignment of 'left', 'center' or 'right'.
*/
ConsoleMethodWithDocs(ImageFont, getTextAlignment, ConsoleString, 2, 2, ())
{
    return ImageFont::getTextAlignmentDescription(object->getTextAlignment());
}

//-----------------------------------------------------------------------------

/*! Set the size of the font characters.
    @param width The width of a font character.
    @param height The height of a font character.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageFont, setFontSize, ConsoleVoid, 3, 4, (width, height))
{
    F32 width, height;

    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ((elementCount == 2) && (argc == 3))
    {
        width = dAtof(Utility::mGetStringElement(argv[2], 0));
        height = dAtof(Utility::mGetStringElement(argv[2], 1));
    }

    // (width, [height])
    else if (elementCount == 1)
    {
        width = dAtof(argv[2]);

        if (argc > 3)
            height = dAtof(argv[3]);
        else
            height = width;
    }
    // Invalid
    else
    {
        Con::warnf("ImageFont::setFontSize() - Invalid number of parameters!");
        return;
    }

    // Set character size.
    object->setFontSize(Vector2(width, height));

}

//-----------------------------------------------------------------------------

/*! Gets the size of the font characters.
    @return The size of the font characters.
*/
ConsoleMethodWithDocs(ImageFont, getFontSize, ConsoleString, 2, 2, ())
{
    return object->getFontSize().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Set the font padding.
    @param padding The space added in-between font characters.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageFont, setFontPadding, ConsoleVoid, 3, 3, (padding))
{
   // Set character padding.
   object->setFontPadding( dAtoi(argv[2]) );

}

//-----------------------------------------------------------------------------

/*! Gets the font padding.
    @return The font padding.
*/
ConsoleMethodWithDocs(ImageFont, getFontPadding, ConsoleInt, 2, 2, ())
{
    return object->getFontPadding();
}

ConsoleMethodGroupEndWithDocs(ImageFont)