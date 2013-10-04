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

ConsoleMethodGroupBeginWithDocs(Scroller, SpriteBase)

/*! Sets the number of times to repeat the texture over x direction
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setRepeatX, ConsoleVoid, 3, 3, (repeatX))
{
   object->setRepeatX( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Sets the number of times to repeat the texture in y direction.
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setRepeatY, ConsoleVoid, 3, 3, (repeatY))
{
   object->setRepeatY( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

/*!  @return Returns repeat X value
*/
ConsoleMethodWithDocs(Scroller, getRepeatX, ConsoleFloat, 2, 2, ())
{
   return object->getRepeatX();
}

//------------------------------------------------------------------------------

/*!  @return Returns repeat Y value
*/
ConsoleMethodWithDocs(Scroller, getRepeatY, ConsoleFloat, 2, 2, ())
{
   return object->getRepeatY();
}

//------------------------------------------------------------------------------

/*! Sets the scroll speed in x direction
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScrollX, ConsoleVoid, 3, 3, (ScrollX))
{
   object->setScroll(dAtof(argv[2]), object->getScrollY());
}

//------------------------------------------------------------------------------

/*! Sets the scroll speed in the Y direction
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScrollY, ConsoleVoid, 3, 3, (ScrollY))
{
   object->setScroll(object->getScrollX(), dAtof(argv[2]));
}

//------------------------------------------------------------------------------

/*!  @return Returns Scroll speed in x direction.
*/
ConsoleMethodWithDocs(Scroller, getScrollX, ConsoleFloat, 2, 2, ())
{
   return object->getScrollX();
}

//------------------------------------------------------------------------------

/*!  @return Returns Scroll speed in y direction.
*/
ConsoleMethodWithDocs(Scroller, getScrollY, ConsoleFloat, 2, 2, ())
{
   return object->getScrollY();
}

//------------------------------------------------------------------------------

/*! Set the texture's position in x direction
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScrollPositionX, ConsoleVoid, 3, 3, (ScrollPositionX))
{
   object->setScrollPosition(dAtof(argv[2]), object->getScrollPositionY());
}

//------------------------------------------------------------------------------

/*! Set the texture's position in y direction
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScrollPositionY, ConsoleVoid, 3, 3, (ScrollPositionY))
{
   object->setScrollPosition(object->getScrollPositionX(), dAtof(argv[2]));
}

//------------------------------------------------------------------------------

/*! Returns  texture's position in x direction
*/
ConsoleMethodWithDocs(Scroller, getScrollPositionX, ConsoleFloat, 2, 2, ())
{
   return object->getScrollPositionX();
}

//------------------------------------------------------------------------------

/*! Returns texture's position in y direction
*/
ConsoleMethodWithDocs(Scroller, getScrollPositionY, ConsoleFloat, 2, 2, ())
{
   return object->getScrollPositionY();
}

//------------------------------------------------------------------------------

/*! Sets the Repeat X/Y repetition in each direction.
    @param repeatX/Y The number of times to repeat in each direction as either (\x y\ or (x, y)
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setRepeat, ConsoleVoid, 3, 4, (float repeatX / float repeatY))
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

/*! Sets the Scroll speed.
    @param offsetX/Y The scroll speed in each direction as either (\x y\ or (x, y)
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScroll, ConsoleVoid, 3, 4, (offsetX / offsetY))
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

/*! Sets Auto-Pan Polarwise.
    @param angle Polar angle.
    @param scrollSpeed Speed as polar magnitude
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScrollPolar, ConsoleVoid, 4, 4, (angle, scrollSpeed))
{
    // Renormalise Angle.
    F32 angle = mFmod(dAtof(argv[2]), 360.0f);
    // Fetch Speed.
    F32 scrollSpeed = dAtof(argv[3]);

    // Set Scroll.
    object->setScroll( mCos(mDegToRad(angle))*scrollSpeed, mSin(mDegToRad(angle))*scrollSpeed );
}

//------------------------------------------------------------------------------

/*! Sets the Scroll position X/Y.
    @param positionX/Y The scroll texture position as either (\x y\ or (x, y)
    @return No return value.
*/
ConsoleMethodWithDocs(Scroller, setScrollPosition, ConsoleVoid, 3, 4, (positionX / positionY))
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

ConsoleMethodGroupEndWithDocs(Scroller)
