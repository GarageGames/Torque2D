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

ConsoleMethodGroupBeginWithDocs(ShapeVector, SceneObject)

/*! Sets the polygon scale.
    @param width/heightScale The scale values of the given polygon. If no height is specified, the widthScale value is repeated.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setPolyScale, ConsoleVoid, 3, 3, (widthScale / [heightScale]))
{
    // Calculate Element Count.
    const U32 elementCount = Utility::mGetStringElementCount( argv[2] );

    // Check Parameters.
    if ( elementCount < 1 )
    {
        Con::warnf("ShapeVector::setPolyScale() - Invalid number of parameters!");
        return;
    }

    // Fetch Width Scale.
    Vector2 scale;
    scale.x = dAtof(Utility::mGetStringElement(argv[2],0));
    // Use Fixed-Aspect for Scale if Height-Scale not specified.
    if ( elementCount == 2 )
    {
        // Specified Height Scale.
        scale.y = dAtof(Utility::mGetStringElement(argv[2],1));
    }
    else
    {
        // Fixed-Aspect Scale.
        scale.y = scale.x;
    }

    // Set Polygon Scale.
    object->setPolyScale( scale );
}

//----------------------------------------------------------------------------

/*! Sets a regular polygon primitive.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setPolyPrimitive, ConsoleVoid, 3, 3, (vertexCount))
{
    // Set Polygon Primitive.
    object->setPolyPrimitive( dAtoi(argv[2]) );
}

//----------------------------------------------------------------------------

/*! Sets Custom Polygon.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setPolyCustom, ConsoleVoid, 4, 4, (poly-count, poly-Definition$))
{
    // Set Collision Poly Custom.
    object->setPolyCustom( dAtoi(argv[2]), argv[3] );
}

//----------------------------------------------------------------------------

/*! Gets Polygon.
    @return (poly-Definition) The vertices of the polygon in object space.
*/
ConsoleMethodWithDocs(ShapeVector, getPoly, ConsoleString, 2, 2, ())
{
   // Get Collision Poly Count.
    
   return object->getPoly();
}

//----------------------------------------------------------------------------

/*! Gets Polygon points in world space.
    @return (poly-Definition) The vertices of the polygon in world space.
*/
ConsoleMethodWithDocs(ShapeVector, getWorldPoly, ConsoleString, 2, 2, ())
{
   // Get Collision Poly Count.
    
   return object->getWorldPoly();
}

//----------------------------------------------------------------------------

/*! or ( stockColorName ) - Sets the line color.
    @param red The red value.
    @param green The green value.
    @param blue The blue value.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(ShapeVector, setLineColor, ConsoleVoid, 3, 6, (float red, float green, float blue, [float alpha = 1.0]))
{
    // The colors.
    F32 red;
    F32 green;
    F32 blue;
    F32 alpha = 1.0f;

    // Space separated.
    if (argc == 3 )
    {
        // Grab the element count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

        // Has a single argument been specified?
        if ( elementCount == 1 )
        {
            // Set color.
            Con::setData( TypeColorF, &const_cast<ColorF&>(object->getBlendColor()), 0, 1, &(argv[2]) );
            return;
        }

        // ("R G B [A]")
        if ((elementCount == 3) || (elementCount == 4))
        {
            // Extract the color.
            red   = dAtof(Utility::mGetStringElement(argv[2], 0));
            green = dAtof(Utility::mGetStringElement(argv[2], 1));
            blue  = dAtof(Utility::mGetStringElement(argv[2], 2));

            // Grab the alpha if it's there.
            if (elementCount > 3)
                alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
        }

        // Invalid.
        else
        {
            Con::warnf("ShapeVector::setLineColor() - Invalid Number of parameters!");
            return;
        }
    }

    // (R, G, B)
    else if (argc >= 5)
    {
        red   = dAtof(argv[2]);
        green = dAtof(argv[3]);
        blue  = dAtof(argv[4]);

        // Grab the alpha if it's there.
        if (argc > 5)
            alpha = dAtof(argv[5]);
    }

    // Invalid.
    else
    {
        Con::warnf("ShapeVector::setLineColor() - Invalid Number of parameters!");
        return;
    }

    object->setLineColor( ColorF(red, green, blue, alpha) );
}

//----------------------------------------------------------------------------

/*! Gets the fill color.
    @param allowColorNames Whether to allow stock color names to be returned or not.  Optional: Defaults to false.
    @return (float red / float green / float blue / float alpha) The sprite blend color.
*/
ConsoleMethodWithDocs(ShapeVector, getLineColor, ConsoleString, 2, 3, (allowColorNames))
{
    // Get line color.
    ColorF color = object->getLineColor();

    // Fetch allow color names flag.
    const bool allowColorNames = (argc > 2) ? dAtob(argv[2] ) : false;

    // Are color names allowed?
    if ( allowColorNames )
    {
        // Yes, so fetch the field value.
        return Con::getData( TypeColorF, &color, 0 );
    }

    // No, so fetch the raw color values.
    return color.scriptThis();
}

//----------------------------------------------------------------------------

/*! Sets the Rendering Line Alpha (transparency).
    @param alpha The alpha value.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setLineAlpha, ConsoleVoid, 3, 3, (alpha))
{
    // Set Line Alpha.
    object->setLineAlpha( dAtof(argv[2]) );
}

//----------------------------------------------------------------------------

/*! or ( stockColorName ) - Sets the fill color.
    @param red The red value.
    @param green The green value.
    @param blue The blue value.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(ShapeVector, setFillColor, ConsoleVoid, 3, 3, (float red, float green, float blue, [float alpha = 1.0]))
{
    // The colors.
    F32 red;
    F32 green;
    F32 blue;
    F32 alpha = 1.0f;

    // Space separated.
    if (argc == 3 )
    {
        // Grab the element count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

        // Has a single argument been specified?
        if ( elementCount == 1 )
        {
            // Set color.
            Con::setData( TypeColorF, &const_cast<ColorF&>(object->getBlendColor()), 0, 1, &(argv[2]) );
            return;
        }

        // ("R G B [A]")
        if ((elementCount == 3) || (elementCount == 4))
        {
            // Extract the color.
            red   = dAtof(Utility::mGetStringElement(argv[2], 0));
            green = dAtof(Utility::mGetStringElement(argv[2], 1));
            blue  = dAtof(Utility::mGetStringElement(argv[2], 2));

            // Grab the alpha if it's there.
            if (elementCount > 3)
                alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
        }

        // Invalid.
        else
        {
            Con::warnf("ShapeVector::setFillColor() - Invalid Number of parameters!");
            return;
        }
    }

    // (R, G, B)
    else if (argc >= 5)
    {
        red   = dAtof(argv[2]);
        green = dAtof(argv[3]);
        blue  = dAtof(argv[4]);

        // Grab the alpha if it's there.
        if (argc > 5)
            alpha = dAtof(argv[5]);
    }

    // Invalid.
    else
    {
        Con::warnf("ShapeVector::setFillColor() - Invalid Number of parameters!");
        return;
    }

    object->setFillColor( ColorF(red, green, blue, alpha) );
}

//----------------------------------------------------------------------------

/*! Gets the fill color.
    @param allowColorNames Whether to allow stock color names to be returned or not.  Optional: Defaults to false.
    @return (float red / float green / float blue / float alpha) The sprite blend color.
*/
ConsoleMethodWithDocs(ShapeVector, getFillColor, ConsoleString, 2, 3, (allowColorNames))
{
    // Get line color.
    ColorF color = object->getFillColor();

    // Fetch allow color names flag.
    const bool allowColorNames = (argc > 2) ? dAtob(argv[2] ) : false;

    // Are color names allowed?
    if ( allowColorNames )
    {
        // Yes, so fetch the field value.
        return Con::getData( TypeColorF, &color, 0 );
    }

    // No, so fetch the raw color values.
    return color.scriptThis();
}

//----------------------------------------------------------------------------

/*! Sets the Rendering Fill Alpha (transparency).
    @param alpha The alpha value.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setFillAlpha, ConsoleVoid, 3, 3, (alpha))
{
    // Set Fill Alpha.
    object->setFillAlpha( dAtof(argv[2]) );
}

//----------------------------------------------------------------------------

/*! Sets the Rendering Fill Mode.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setFillMode, ConsoleVoid, 3, 3, (fillMode?))
{
    // Set Fill Mode.
    object->setFillMode( dAtob(argv[2]) );
}

//----------------------------------------------------------------------------

/*! Gets the Rendering Fill Mode.
    @return The fill mode as a boolean value.
*/
ConsoleMethodWithDocs(ShapeVector, getFillMode, ConsoleBool, 2, 2, ())
{
    return object->getFillMode();
}

//----------------------------------------------------------------------------

/*! Sets whether this shape is a circle or not.
    @return The fill mode as a boolean value.
*/
ConsoleMethodWithDocs(ShapeVector, setIsCircle, ConsoleVoid, 3, 3, (isCircle?))
{
    object->setIsCircle(dAtob(argv[2]));
}

//----------------------------------------------------------------------------

/*! Returns whether this shape is a circle or not.
    @return The fill mode as a boolean value.
*/
ConsoleMethodWithDocs(ShapeVector, getIsCircle, ConsoleBool, 2, 2, ())
{
    return object->getIsCircle();
}

//----------------------------------------------------------------------------

/*! Returns the radius of the shape if it is a circle.
    @return The fill mode as a boolean value.
*/
ConsoleMethodWithDocs(ShapeVector, getCircleRadius, ConsoleBool, 2, 2, ())
{
    return object->getCircleRadius();
}

//----------------------------------------------------------------------------

/*! Changes the radius of the shape if it is a circle.
    @return The fill mode as a boolean value.
*/
ConsoleMethodWithDocs(ShapeVector, setCircleRadius, ConsoleVoid, 3, 3, (radius))
{
    object->setCircleRadius(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the number of vertices on a polygon shape.
*/
ConsoleMethodWithDocs(ShapeVector, getVertexCount, ConsoleInt, 2, 2, ())
{
    return object->getPolyVertexCount();
}

//-----------------------------------------------------------------------------

/*! Get a box (\width height\ that wraps around the poly vertices
*/
ConsoleMethodWithDocs(ShapeVector, getBoxFromPoints, ConsoleString, 2, 2, ())
{
    Vector2 box = object->getBoxFromPoints();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    
    // Format Buffer.
    dSprintf(pBuffer, 32, "%g %g", box.x, box.y);
    
    // Return box width and height.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets shape flipping for each axis.
    @param flipX Whether or not to flip the shape along the x (horizontal) axis.
    @param flipY Whether or not to flip the shape along the y (vertical) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setFlip, ConsoleVoid, 4, 4, (bool flipX, bool flipY))
{
    // Set Flip.
    object->setFlip( dAtob(argv[2]), dAtob(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets the flip for each axis.
    @return (bool flipX/bool flipY) Whether or not the shape is flipped along the x and y axis.
*/
ConsoleMethodWithDocs(ShapeVector, getFlip, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getFlipX(), object->getFlipY());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the shape is flipped horizontally.
    @param flipX Whether or not to flip the shape along the x (horizontal) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setFlipX, ConsoleVoid, 3, 3, (bool flipX))
{
    // Set Flip.
    object->setFlipX( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the shape is flipped vertically.
    @param flipY Whether or not to flip the shape along the y (vertical) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(ShapeVector, setFlipY, ConsoleVoid, 3, 3, (bool flipY))
{
    // Set Flip.
    object->setFlipY( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the shape is flipped horizontally.
    @return (bool flipX) Whether or not the shape is flipped along the x axis.
*/
ConsoleMethodWithDocs(ShapeVector, getFlipX, ConsoleBool, 2, 2, ())
{
   return object->getFlipX();
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the shape is flipped vertically.
    @return (bool flipY) Whether or not the shape is flipped along the y axis.
*/
ConsoleMethodWithDocs(ShapeVector, getFlipY, ConsoleBool, 2, 2, ())
{
   return object->getFlipY();
}

ConsoleMethodGroupEndWithDocs(ShapeVector)
