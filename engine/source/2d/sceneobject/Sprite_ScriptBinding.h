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


ConsoleMethod(Sprite, setFlip, void, 4, 4,  "(bool flipX, bool flipY) Sets the sprite texture flipping for each axis.\n"
                                                "@param flipX Whether or not to flip the texture along the x (horizontal) axis.\n"
                                                "@param flipY Whether or not to flip the texture along the y (vertical) axis.\n"
                                                "@return No return value.")
{
    // Set Flip.
    object->setFlip( dAtob(argv[2]), dAtob(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Sprite, getFlip, const char*, 2, 2,   "() Gets the flip for each axis.\n"
                                                        "@return (bool flipX/bool flipY) Whether or not the texture is flipped along the x and y axis.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getFlipX(), object->getFlipY());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Sprite, setFlipX, void, 3, 3,     "(bool flipX) Sets whether or not the texture is flipped horizontally.\n"
                                                    "@param flipX Whether or not to flip the texture along the x (horizontal) axis."
                                                    "@return No return value.")
{
    // Set Flip.
    object->setFlipX( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Sprite, setFlipY, void, 3, 3,     "(bool flipY) Sets whether or not the texture is flipped vertically.\n"
                                                    "@param flipY Whether or not to flip the texture along the y (vertical) axis."
                                                    "@return No return value.")
{
    // Set Flip.
    object->setFlipY( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Sprite, getFlipX, bool, 2, 2,     "() Gets whether or not the texture is flipped horizontally.\n"
                                                    "@return (bool flipX) Whether or not the texture is flipped along the x axis.")
{
   return object->getFlipX();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Sprite, getFlipY, bool, 2, 2,     "() Gets whether or not the texture is flipped vertically."
                                                    "@return (bool flipY) Whether or not the texture is flipped along the y axis.")
{
   return object->getFlipY();
}