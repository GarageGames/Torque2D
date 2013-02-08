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

ConsoleMethod(Scroller, setRepeatX, void, 3, 3, "(repeatX) Sets the number of times to repeat the texture over x direction\n"
              "@return No return value.")
{
   object->setRepeatX( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setRepeatY, void, 3, 3, "(repeatY) Sets the number of times to repeat the texture in y direction.\n"
              "@return No return value.")
{
   object->setRepeatY( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, getRepeatX, F32, 2, 2, "() \n @return Returns repeat X value")
{
   return object->getRepeatX();
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, getRepeatY, F32, 2, 2, "() \n @return Returns repeat Y value")
{
   return object->getRepeatY();
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScrollX, void, 3, 3, "(ScrollX) Sets the scroll speed in x direction\n"
              "@return No return value.")
{
   object->setScroll(dAtof(argv[2]), object->getScrollY());
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScrollY, void, 3, 3, "(ScrollY) Sets the scroll speed in the Y direction\n"
              "@return No return value.")
{
   object->setScroll(object->getScrollX(), dAtof(argv[2]));
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, getScrollX, F32, 2, 2, "() \n @return Returns Scroll speed in x direction.")
{
   return object->getScrollX();
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, getScrollY, F32, 2, 2, "() \n @return Returns Scroll speed in y direction.")
{
   return object->getScrollY();
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScrollPositionX, void, 3, 3, "(ScrollPositionX) Set the texture's position in x direction\n"
              "@return No return value.")
{
   object->setScrollPosition(dAtof(argv[2]), object->getScrollPositionY());
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScrollPositionY, void, 3, 3, "(ScrollPositionY) Set the texture's position in y direction\n"
              "@return No return value.")
{
   object->setScrollPosition(object->getScrollPositionX(), dAtof(argv[2]));
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, getScrollPositionX, F32, 2, 2, "() \nReturns  texture's position in x direction")
{
   return object->getScrollPositionX();
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, getScrollPositionY, F32, 2, 2, "() \nReturns texture's position in y direction")
{
   return object->getScrollPositionY();
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setRepeat, void, 3, 4, "(float repeatX / float repeatY) Sets the Repeat X/Y repetition in each direction.\n"
                                                  "@param repeatX/Y The number of times to repeat in each direction as either (\"x y\") or (x, y)\n"
                                                  "@return No return value.")
{
   // The new position.
   F32 repeatX;
   F32 repeatY;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("repeatX repeatY")
   if ((elementCount == 2) && (argc == 3))
   {
      repeatX = dAtof(Utility::mGetStringElement(argv[2], 0));
      repeatY = dAtof(Utility::mGetStringElement(argv[2], 1));
   }

   // (repeatX, repeatY)
   else if ((elementCount == 1) && (argc == 4))
   {
      repeatX = dAtof(argv[2]);
      repeatY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("Scroller::setRepeat() - Invalid number of parameters!");
      return;
   }

   // Set Repeat.
   object->setRepeat(repeatX, repeatY);
}   

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScroll, void, 3, 4, "(offsetX / offsetY) Sets the Scroll speed."
              "@param offsetX/Y The scroll speed in each direction as either (\"x y\") or (x, y)\n"
              "@return No return value.")
{
   // The new position.
   F32 scrollX;
   F32 scrollY;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("scrollX scrollY")
   if ((elementCount == 2) && (argc == 3))
   {
      scrollX = dAtof(Utility::mGetStringElement(argv[2], 0));
      scrollY = dAtof(Utility::mGetStringElement(argv[2], 1));
   }

   // (scrollX, scrollY)
   else if ((elementCount == 1) && (argc == 4))
   {
      scrollX = dAtof(argv[2]);
      scrollY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("Scroller::setScroll() - Invalid number of parameters!");
      return;
   }

   // Set Scroll.
   object->setScroll(scrollX, scrollY);
}   

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScrollPolar, void, 4, 4, "(angle, scrollSpeed) Sets Auto-Pan Polarwise.\n"
              "@param angle Polar angle.\n"
              "@param scrollSpeed Speed as polar magnitude\n"
              "@return No return value.")
{
    // Renormalise Angle.
    F32 angle = mFmod(dAtof(argv[2]), 360.0f);
    // Fetch Speed.
    F32 scrollSpeed = dAtof(argv[3]);

    // Set Scroll.
    object->setScroll( mSin(mDegToRad(angle))*scrollSpeed, -mCos(mDegToRad(angle))*scrollSpeed );
}

//------------------------------------------------------------------------------

ConsoleMethod(Scroller, setScrollPosition, void, 3, 4, "(positionX / positionY) Sets the Scroll position X/Y."
              "@param positionX/Y The scroll texture position as either (\"x y\") or (x, y)\n"
              "@return No return value.")
{
   // The new position.
   F32 scrollX;
   F32 scrollY;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("positionX positionY")
   if ((elementCount == 2) && (argc == 3))
   {
      scrollX = dAtof(Utility::mGetStringElement(argv[2], 0));
      scrollY = dAtof(Utility::mGetStringElement(argv[2], 1));
   }

   // (positionX, positionY)
   else if ((elementCount == 1) && (argc == 4))
   {
      scrollX = dAtof(argv[2]);
      scrollY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("Scroller::setScrollPosition() - Invalid number of parameters!");
      return;
   }

   // Set Scroll Position.
   object->setScrollPosition(scrollX, scrollY);
}

