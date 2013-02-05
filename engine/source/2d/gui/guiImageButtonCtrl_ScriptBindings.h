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

ConsoleMethod( GuiImageButtonCtrl, setNormalImage, void, 3, 3,  "(imageAssetId) Sets the asset Id the button \"up\" state.\n"
                                                                "@return No return value.")
{
   object->setNormalImage( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( GuiImageButtonCtrl, setHoverImage, void, 3, 3,       "(imageAssetId) Sets the asset Id the button \"hover\" state.\n"
                                                                    "@return No return value.")
{
   object->setHoverImage( argv[2]  );
}

//-----------------------------------------------------------------------------

ConsoleMethod( GuiImageButtonCtrl, setDownImage, void, 3, 3,   "(imageAssetId) Sets the asset Id the button \"down\" state.\n"
                                                                    "@return No return value.")
{
   object->setDownImage( argv[2]  );
}

//-----------------------------------------------------------------------------

ConsoleMethod( GuiImageButtonCtrl, setInactiveImage, void, 3, 3,    "(imageAssetId) Sets the asset Id the button \"inactive\" state.\n"
                                                                    "@return No return value.")
{
   object->setInactiveImage( argv[2]  );
}

//-----------------------------------------------------------------------------

ConsoleMethod( GuiImageButtonCtrl, setActive, void, 3, 3,    "(imageAssetId) Sets the asset Id the button \"inactive\" state.\n"
                                                                    "@return No return value.")
{
    bool flag = dAtob( argv[2] );
    object->setActive( flag  );
}
