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

ConsoleMethod(ImageFont, setImage, bool, 3, 3,  "(imageAssetId) - Sets the image asset to use..\n"
                                                "@param imageName The image asset to use.\n"
                                                "@return Returns true on success.")
{
    // Set Image.
    return object->setImage( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, getImage, const char*, 2, 2,   "() - Gets current image asset..\n"
                                                        "@return The current image asset.")
{
    // Get Image.
    return object->getImage();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, setText, void, 3, 3,   "(text) - Set the text to render.\n")
{
    object->setText(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, getText, const char*, 2, 2,    "() - Gets the text being rendered.\n")
{
    return object->getText().getPtr8();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, setTextAlignment, void, 3, 3,  "(alignment) - Set the text alignment to 'left', 'center' or 'right'.\n"
                                                        "@param alignment The text alignment of 'left', 'center' or 'right'.\n"
                                                        "@return No return value.")
{

    object->setTextAlignment( ImageFont::getTextAlignmentEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, getTextAlignment, const char*, 2, 2,   "() - Gets the text alignment.\n"
                                                                "@return The text alignment of 'left', 'center' or 'right'.")
{
    return ImageFont::getTextAlignmentDescription(object->getTextAlignment());
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, setFontSize, void, 3, 4,   "(width, height) - Set the size of the font characters.\n"
                                                    "@param width The width of a font character.\n"
                                                    "@param height The height of a font character.\n"
                                                    "@return No return value.")
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

ConsoleMethod(ImageFont, getFontSize, const char*, 2, 2,    "() - Gets the size of the font characters.\n"
                                                            "@return The size of the font characters.")
{
    return object->getFontSize().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, setFontPadding, void, 3, 3,    "(padding) - Set the font padding.\n"
                                                        "@param padding The space added in-between font characters.\n"
                                                        "@return No return value.")
{
   // Set character padding.
   object->setFontPadding( dAtoi(argv[2]) );

}

//-----------------------------------------------------------------------------

ConsoleMethod(ImageFont, getFontPadding, S32, 2, 2,     "() - Gets the font padding.\n"
                                                        "@return The font padding.")
{
    return object->getFontPadding();
}