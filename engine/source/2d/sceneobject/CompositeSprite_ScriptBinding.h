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

ConsoleMethod(CompositeSprite, addSprite, S32, 2, 3,    "( [a] [b] [c] [d] [e] [f] ) - Adds a sprite at the specified logical position.\n"
                                                        "You must specify the correct number of arguments for the selected layout mode.\n"
                                                        "The created sprite will be automatically selected.\n"
                                                        "@param a b c d e f Logical positions #1 & #2 and four additional and optional arguments.\n"
                                                        "@return The batch Id of the added sprite or zero if not successful." )
{
    if ( argc == 2 )
        return object->addSprite( SpriteBatchItem::LogicalPosition() );
    else
        return object->addSprite( SpriteBatchItem::LogicalPosition(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, removeSprite, bool, 2, 2,    "() - Removes the selected sprite.\n"
                                                            "@return Whether the sprite was removed or not." )
{
    return object->removeSprite();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, clearSprites, void, 2, 2,    "() - Removes all sprites.\n"
                                                            "@return No return value." )
{
    return object->clearSprites();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteCount, S32, 2, 2,   "() - Gets a count of sprites in the composite.\n"
                                                            "@return The count of sprites in the composite." )
{
    return object->getSpriteCount();
}


//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setBatchLayout, void, 3, 3,      "(batchLayoutType) - Sets the batch layout type.\n"
                                                                "The render sort mode is used when isolated batch mode is on.\n"
                                                                "@param batchLayoutType 'none', 'rect' or 'iso' layout types are valid.\n"
                                                                "@return No return value." )
{
    // Fetch the batch layout type/
    CompositeSprite::BatchLayoutType batchLayoutType = CompositeSprite::getBatchLayoutTypeEnum( argv[2] );

    // Sanity!
    if ( batchLayoutType == CompositeSprite::INVALID_LAYOUT )
    {
        // Warn.
        Con::warnf( "CompositeSprite::setBatchLayout() - Unknown batch layout type of '%s'.", argv[2] );
        return;
    }

    object->setBatchLayout( batchLayoutType );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getBatchLayout, const char*, 2, 2, "() - Gets the batch layout type.\n"
                                                                    "@return The batch layout type." )
{
    return CompositeSprite::getBatchLayoutTypeDescription( object->getBatchLayout() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setBatchIsolated, void, 3, 3,    "(bool batchIsolated) - Sets whether the sprites are rendered, isolated from other renderings as one batch or not.\n"
                                                                "When in batch isolated mode, the sprites can be optionally sorted.\n"
                                                                "@return No return value." )
{
    // Fetch batch isolated.
    const bool batchIsolated = dAtob(argv[2]);

    object->setBatchIsolated( batchIsolated );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getBatchIsolated, bool, 2, 2,    "() - Gets whether the sprites are rendered, isolated from other renderings as one batch or not.\n"
                                                                "@return Whether the sprites are rendered, isolated from other renderings as one batch or not." )
{
    return object->getBatchIsolated();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setBatchCulling, void, 3, 3,     "(bool batchCulling) - Sets whether the sprites are culled.\n"
                                                                "For sprites that are off-screen this is considerably faster during render at the expense of memory.\n"
                                                                "For small composites with a few sprites, the overhead is probably not worth it.\n"
                                                                "@return No return value." )
{
    // Fetch batch culling..
    const bool batchCulling = dAtob(argv[2]);

    STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, object)->setBatchCulling( batchCulling );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getBatchCulling, bool, 2, 2,    "() - Gets whether the sprites are render culled or not\n"
                                                                "@return Whether the sprites are rendered culled or not." )
{
    return object->getBatchCulling();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setBatchSortMode, void, 3, 3,    "(renderSortMode) - Sets the batch render sort mode.\n"
                                                                "The render sort mode is used when isolated batch mode is on.\n"
                                                                "@return No return value." )
{
    // Fetch render sort mode.
    SceneRenderQueue::RenderSort batchSortMode = SceneRenderQueue::getRenderSortEnum( argv[2] );

    // Sanity!
    if ( batchSortMode == SceneRenderQueue::RENDER_SORT_INVALID )
    {
        // Warn.
        Con::warnf( "CompositeSprite::setBatchSortMode() - Unknown batch sort mode of '%s'.", argv[2] );
        return;
    }

    object->setBatchSortMode( batchSortMode );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getBatchSortMode, const char*, 2, 2, "() - Gets the batch render sort mode.\n"
                                                                    "@return The render sort mode." )
{
    return SceneRenderQueue::getRenderSortDescription( object->getBatchSortMode() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setDefaultSpriteStride, void, 3, 4,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param strideX The default stride of the local X axis.\n"
                                                                    "@param strideY The default stride of the local Y axis.\n"
                                                                    "@return No return value.")
{
    Vector2 stride;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("strideX strideY")
    if ( (elementCount == 2) && (argc == 3) )
    {
        stride.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        stride.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (strideX, [strideY])
    else if (elementCount == 1)
    {
        stride.x = dAtof(argv[2]);

        if (argc > 3)
            stride.y = dAtof(argv[3]);
        else
            stride.y = stride.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setDefaultSpriteStride() - Invalid number of parameters!");
        return;
    }

    object->setDefaultSpriteStride( stride );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getDefaultSpriteStride, const char*, 2, 2,   "() - Gets the stride which scales the position at which sprites are created.\n"
                                                                            "@return (float strideX/float strideY) The stride which scales the position at which sprites are created.")
{
    return object->getDefaultSpriteStride().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setDefaultSpriteSize, void, 3, 4,    "(float width, [float height]) - Sets the size at which sprites are created.\n"
                                                                    "@param width The default width of sprites.\n"
                                                                    "@param height The default height of sprites\n"
                                                                    "@return No return value.")
{
    Vector2 size;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ( (elementCount == 2) && (argc == 3) )
    {
        size.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        size.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (width, [height])
    else if (elementCount == 1)
    {
        size.x = dAtof(argv[2]);

        if (argc > 3)
            size.y = dAtof(argv[3]);
        else
            size.y = size.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setDefaultSpriteSize() - Invalid number of parameters!");
        return;
    }

    object->setDefaultSpriteSize( size );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getDefaultSpriteSize, const char*, 2, 2, "() - Gets the size at which sprites are created.\n"
                                                                        "@return (float width/float height) The size at which sprites are created.")
{
    return object->getDefaultSpriteSize().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setDefaultSpriteAngle, void, 3, 3,   "(float angle) - Sets the angle at which sprites are created.\n"
                                                                    "@param angle The angle at which sprites are created.\n"
                                                                    "@return No return value.")
{
    // Fetch angle.
    const F32 angle = mDegToRad( dAtof(argv[2]) );

    static_cast<SpriteBatch*>(object)->setDefaultSpriteAngle( angle );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getDefaultSpriteAngle, F32, 3, 3,   "() - Gets the angle at which sprites are created.\n"
                                                                    "@return (float angle) The angle at which sprites are created.")
{
    return mRadToDeg( static_cast<SpriteBatch*>(object)->getDefaultSpriteAngle() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, selectSprite, bool, 3, 3,    "( a b [c] [d] [e] [f] ) - Selects a sprite at the specified logical position.\n"
                                                            "@param a b c d e f Logical positions #1 & #2 and four additional and optional arguments.\n"
                                                            "@return Whether the sprite was selected or not." )
{
    return object->selectSprite( SpriteBatchItem::LogicalPosition(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, selectSpriteId, bool, 3, 3,  "( int batchId ) - Selects a sprite with the specified batch Id.\n"
                                                            "@param batchId The batch Id of the sprite to select.\n"
                                                            "@return Whether the sprite was selected or not." )
{
    return object->selectSpriteId( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, selectSpriteName, bool, 3, 3,    "( name ) - Selects a sprite with the specified name.\n"
                                                                "@param name The name of the sprite.\n"
                                                                "@return Whether the sprite was selected or not." )
{
    return object->selectSpriteName( argv[2] );
}


//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, deselectSprite, void, 2, 2,  "() - Deselects any selected sprite.\n"
                                                            "This is not required but can be used to stop accidental changes to sprites.\n"
                                                            "@return No return value." )
{
    return object->deselectSprite();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, isSpriteSelected, bool, 2, 2,    "() - Checks whether a sprite is selected or not.\n"
                                                                "@return Whether a sprite is selected or not." )
{
    return object->isSpriteSelected();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteImage, void, 3, 4,  "(imageAssetId, [int imageFrame]) - Sets the sprite image and optional frame.\n"
                                                            "@param imageAssetId The image to set the sprite to.\n"
                                                            "@param imageFrame The image frame of the imageAssetId to set the sprite to.\n"
                                                            "@return No return value." )
{
    // Fetch frame.
    const U32 frame = argc >=4 ? dAtoi(argv[3]) : 0;

    object->setSpriteImage( argv[2], frame );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteImage, const char*, 2, 2,   "() - Gets the sprite image.\n"
                                                                    "@return The sprite image." )
{
    return object->getSpriteImage();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteImageFrame, void, 3, 3,  "(int imageFrame) - Sets the sprite image frame.\n"
                                                            "@param imageFrame The image frame to set the sprite to.\n"
                                                            "@return No return value." )
{
    // Fetch frame.
    const U32 frame = dAtoi(argv[2]);

    object->setSpriteImageFrame( frame );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteImageFrame, S32, 2, 2,  "() - Gets the sprite image frame.\n"
                                                                "@return The sprite image frame." )
{
    return object->getSpriteImageFrame();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteAnimation, void, 3, 4,  "(animationAssetId) - Sets the sprite animation.\n"
                                                                "@param imageAssetId The animation to set the sprite to.\n"
                                                                "@return No return value." )
{
    object->setSpriteAnimation( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteAnimation, const char*, 2, 2,   "() - Gets the sprite animation.\n"
                                                                        "@return The sprite animation." )
{
    return object->getSpriteAnimation();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, clearSpriteAsset, void, 2, 2,    "() - Clears any image or animation asset from the sprite.\n"
                                                                "@return No return value." )
{
    return object->clearSpriteAsset();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteVisible, void, 3, 3,    "(bool visible) - Sets whether the sprite is visible or not.\n"
                                                                "@param visible Whether the sprite is visible or not.\n"
                                                                "@return No return value." )
{
    // Fetch visible.
    const bool visible = dAtob(argv[2]);

    object->setSpriteVisible( visible );

}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteVisible, bool, 2, 2,    "() - Gets whether the sprite is visible or not.\n"
                                                                "@return Whether the sprite is visible or not." )
{
    return object->getSpriteVisible();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteLocalPosition, void, 3, 4,  "(float localX, float localY) - Sets the sprites local position.\n"
                                                                    "@param localX The local position X.\n"
                                                                    "@param localY The local position Y.\n"
                                                                    "@return No return value." )
{
    Vector2 localPosition;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ( (elementCount == 2) && (argc == 3) )
    {
        localPosition.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        localPosition.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (x, y)
    else if ( elementCount == 1 && (argc > 3) )
    {
        localPosition.x = dAtof(argv[2]);
        localPosition.y = dAtof(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setSpriteLocalPosition() - Invalid number of parameters!");
        return;
    }

    object->setSpriteLocalPosition( localPosition );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteLocalPosition, const char*, 2, 2,   "() - Gets the sprite local position.\n"
                                                                            "@return The sprite local position." )
{
    return object->getSpriteLocalPosition().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteAngle, void, 3, 3,  "(float localAngle) - Sets the sprites local angle.\n"
                                                            "@param localAngle The sprite local angle.\n"
                                                            "@return No return value." )
{
    // Fetch angle.
    const F32 angle = mDegToRad( dAtof(argv[2]) );

    object->setSpriteAngle( angle );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteAngle, F32, 2, 2,   "() - Gets the sprite local angle.\n"
                                                            "@return The sprite local angle." )
{
    return mRadToDeg( object->getSpriteAngle() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteDepth, void, 3, 3,  "(float depth) - Sets the sprites depth.\n"
                                                            "@param depth The sprite depth.\n"
                                                            "@return No return value." )
{
    // Fetch depth.
    const F32 depth = dAtof(argv[2]);

    object->setSpriteDepth( depth );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteDepth, F32, 2, 2,   "() - Gets the sprite depth.\n"
                                                            "@return The sprite depth." )
{
    return object->getSpriteDepth();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteSize, void, 3, 4,   "(float width, [float height]) - Sets the sprite size.\n"
                                                            "@param width The sprite width.\n"
                                                            "@param height The sprite height\n"
                                                            "@return No return value.")
{
    Vector2 size;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ( (elementCount == 2) && (argc == 3) )
    {
        size.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        size.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (width, [height])
    else if (elementCount == 1)
    {
        size.x = dAtof(argv[2]);

        if (argc > 3)
            size.y = dAtof(argv[3]);
        else
            size.y = size.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setSpriteSize() - Invalid number of parameters!");
        return;
    }

    object->setSpriteSize( size );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteSize, const char*, 2, 2,    "() - Gets the sprite size.\n"
                                                                    "@return (float width/float height) The sprite size.")
{
    return object->getSpriteSize().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteFlipX, void, 3, 3,  "(bool flipX) - Sets whether the sprite is flipped along its local X axis or not.\n"
                                                            "@param flipX Whether the sprite is flipped along its local X axis or not.\n"
                                                            "@return No return value." )
{
    // Fetch flipX.
    const bool flipX = dAtob(argv[2]);

    object->setSpriteFlipX( flipX );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteFlipX, bool, 2, 2,   "() - Gets whether the sprite is flipped along its local X axis or not.\n"
                                                            "@return Whether the sprite is flipped along its local X axis or not." )
{
    return object->getSpriteFlipX();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteFlipY, void, 3, 3,  "(bool flipY) - Sets whether the sprite is flipped along its local Y axis or not.\n"
                                                            "@param flipY Whether the sprite is flipped along its local Y axis or not.\n"
                                                            "@return No return value." )
{
    const bool flipY = dAtob(argv[2]);

    object->setSpriteFlipY( flipY );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteFlipY, bool, 2, 2,   "() - Gets whether the sprite is flipped along its local Y axis or not.\n"
                                                                "@return Whether the sprite is flipped along its local Y axis or not." )
{
    return object->getSpriteFlipY();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteSortPoint, void, 3, 4,  "(float localX, float localY) - Sets the sprites sort point.\n"
                                                                "@param localX The local sort point X.\n"
                                                                "@param localY The local sort point Y.\n"
                                                                "@return No return value." )
{
    Vector2 sortPoint;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ( (elementCount == 2) && (argc == 3) )
    {
        sortPoint.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        sortPoint.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (x, y)
    else if ( elementCount == 1 && (argc > 3) )
    {
        sortPoint.x = dAtof(argv[2]);
        sortPoint.y = dAtof(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setSpriteSortPoint() - Invalid number of parameters!");
        return;
    }

    object->setSpriteSortPoint( sortPoint );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteSortPoint, const char*, 2, 2,   "() - Gets the sprite local sort point.\n"
                                                                        "@return The sprite local sort point." )
{
    return object->getSpriteSortPoint().scriptThis();
}


//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteRenderGroup, void, 3, 3,    "(renderGroup) Sets the name of the render group used to sort the sprite during rendering.\n"
                                                                    "@param renderGroup The name of the render group to use.  Defaults to nothing.\n"
                                                                    "@return No return value.")
{
    object->setSpriteRenderGroup( argv[2] );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteRenderGroup, const char*, 2, 2, "() Gets the name of the render group used to sort the sprite during rendering.\n"
                                                                        "@return The render group used to sort the object during rendering.")
{
    return object->getSpriteRenderGroup();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteBlendMode, void, 3, 3,   "(bool blendMode) - Sets whether sprite blending is on or not.\n"
                                                                "@blendMode Whether sprite blending is on or not.\n"
                                                                "@return No return Value.")
{
    // Fetch blend mode.
    const bool blendMode = dAtob(argv[2]);

    object->setSpriteBlendMode( blendMode );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteBlendMode, bool, 2, 2,  "() - Gets whether sprite blending is on or not.\n"
                                                                "@return (bool blendMode) Whether sprite blending is on or not.")
{
   return object->getSpriteBlendMode();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteSrcBlendFactor, void, 3, 3, "(srcBlend) - Sets the sprite source blend factor.\n"
                                                                    "@param srcBlend The sprite source blend factor.\n"
                                                                    "@return No return Value.")
{
    // Fetch source blend factor.
    GLenum blendFactor = SceneObject::getSrcBlendFactorEnum(argv[2]);

    object->setSpriteSrcBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteSrcBlendFactor, const char*, 2, 2,  "() - Gets the sprite source blend factor.\n"
                                                                            "@return (srcBlend) The sprite source blend factor.")
{
   return SceneObject::getSrcBlendFactorDescription( object->getSpriteSrcBlendFactor() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteDstBlendFactor, void, 3, 3, "(dstBlend) - Sets the sprite destination blend factor.\n"
                                                                    "@param dstBlend The sprite destination blend factor.\n"
                                                                    "@return No return Value.")
{
    // Fetch destination blend factor.
    GLenum blendFactor = SceneObject::getDstBlendFactorEnum(argv[2]);

    object->setSpriteDstBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteDstBlendFactor, const char*, 2, 2,  "() - Gets the sprite destination blend factor.\n"
                                                                            "@return (dstBlend) The sprite destination blend factor.")
{
   return SceneObject::getDstBlendFactorDescription( object->getSpriteDstBlendFactor() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteBlendColor, void, 3, 6, "(float red, float green, float blue, [float alpha = 1.0]) or ( stockColorName ) - Sets the sprite blend color."
                                                                "@param red The red value.\n"
                                                                "@param green The green value.\n"
                                                                "@param blue The blue value.\n"
                                                                "@param alpha The alpha value.\n"
                                                                "@return No return Value.")
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
            // Is a sprite selected?
            if ( !object->isSpriteSelected() )
            {
                // No, so warn.
                Con::warnf("CompositeSprite::setSpriteBlendColor() - Cannot set sprite blend color as no sprite is selected." );
                return;
            }

            Con::setData( TypeColorF, &const_cast<ColorF&>(object->getSpriteBlendColor()), 0, 1, &(argv[2]) );
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
            Con::warnf("SceneObject::setBlendColor() - Invalid Number of parameters!");
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
        Con::warnf("SceneObject::setBlendColor() - Invalid Number of parameters!");
        return;
    }

    // Set blend color.
    object->setSpriteBlendColor(ColorF(red, green, blue, alpha));
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteBlendColor, const char*, 2, 3,  "(allowColorNames) Gets the sprite blend color\n"
                                                                        "@param allowColorNames Whether to allow stock color names to be returned or not.  Optional: Defaults to false.\n"
                                                                        "@return (float red / float green / float blue / float alpha) The sprite blend color.")
{
    // Get Blend color.
    ColorF blendColor = object->getSpriteBlendColor();

    // Fetch allow color names flag.
    const bool allowColorNames = (argc > 2) ? dAtob(argv[2] ) : false;

    // Are color names allowed?
    if ( allowColorNames )
    {
        // Yes, so fetch the field value.
        return Con::getData( TypeColorF, &blendColor, 0 );
    }

    // No, so fetch the raw color values.
    return blendColor.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteBlendAlpha, void, 3, 3, "(float alpha) - Sets the sprite color alpha (transparency).\n"
                                                                "The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent.\n"
                                                                "@param alpha The alpha value.\n"
                                                                "@return No return Value.")
{
    object->setSpriteBlendAlpha( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteBlendAlpha, F32, 2, 2,  "() - Gets the sprite color alpha (transparency).\n"
                                                                "@return (float alpha) The alpha value, a range from 0.0 to 1.0.  Less than zero if alpha testing is disabled.")
{
    return object->getSpriteBlendAlpha();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteAlphaTest, void, 3, 3,  "(float alpha) - Set the sprite alpha test.\n"
                                                                "@param value Numeric value of 0.0 to 1.0 to turn on alpha testing. Less than zero to disable alpha testing."
                                                                "@return No return Value.")
{
    object->setSpriteAlphaTest(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteAlphaTest, F32, 2, 2,   "() - Gets the sprite alpha test.\n"
                                                                "@return (S32) A value of 0 to 255 if alpha testing is enabled. <0 represents disabled alpha testing.")
{
    return object->getSpriteAlphaTest();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteDataObject, void, 3, 3, "(object) - Set the sprite data object.\n"
                                                                "NOTE: This object will be persisted alongside the composite sprite.\n"
                                                                "To clear the object you can pass an empty string.\n"
                                                                "@return No return Value.")
{
    object->setSpriteDataObject( Sim::findObject( argv[2] ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteDataObject, const char*, 2, 2,  "() - Gets the sprite data object.\n"
                                                                        "@return The sprite data object.")
{
    return object->getSpriteDataObject()->getIdString();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, setSpriteName, void, 3, 3,   "(name) - Set the sprite name.\n"
                                                            "This must be unique within this composite sprite instance.  To clear the name you can pass an empty string.\n"
                                                            "@return No return Value.")
{
    object->setSpriteName( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, getSpriteName, const char*, 2, 2,    "() - Gets the sprite name.\n"
                                                                    "@return The sprite name.")
{
    return object->getSpriteName();
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, pickPoint, const char*, 3, 4,    "(x / y ) Picks sprites intersecting the specified point with optional group/layer masks.\n"
                                                                "@param x/y The coordinate of the point as either (\"x y\") or (x,y)\n"
                                                                "@return Returns list of sprite Ids.")
{
    // Fetch sprite batch query and clear results.
    SpriteBatchQuery* pSpriteBatchQuery = object->getSpriteBatchQuery( true );

    // Is the sprite batch query available?
    if ( pSpriteBatchQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "CompositeSprite::pickPoint() - Cannot pick sprites if clipping mode is off." );

        // Return nothing.
        return NULL;
    }

    // The point.
    Vector2 point;

    // The index of the first optional parameter.
    U32 firstArg;

    // Grab the number of elements in the first parameter.
    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc < 8))
    {
        point = Utility::mGetStringElementVector(argv[2]);
        firstArg = 3;
    }
   
    // (x, y)
    else if ((elementCount == 1) && (argc > 3))
    {
        point = Vector2(dAtof(argv[2]), dAtof(argv[3]));
        firstArg = 4;
    }
   
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::pickPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch the render transform.
    const b2Transform& renderTransform = object->getRenderTransform();

    // Transform into local space.
    point = b2MulT( renderTransform, point );

    // Perform query.
    pSpriteBatchQuery->queryPoint( point, true );

    // Fetch result count.
    const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

    // Finish if no results.
    if (resultCount == 0 )
        return NULL;

    // Fetch results.
    typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add picked sprites.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSpriteBatchItem->getBatchId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("CompositeSprite::pickPoint() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear sprite batch query.
    pSpriteBatchQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, pickArea, const char*, 4, 6, "(startx/y, endx/y ) Picks sprites intersecting the specified area with optional group/layer masks.\n"
                                                            "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
                                                            "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
                                                            "@return Returns list of sprite Ids.")
{
    // Fetch sprite batch query and clear results.
    SpriteBatchQuery* pSpriteBatchQuery = object->getSpriteBatchQuery( true );

    // Is the sprite batch query available?
    if ( pSpriteBatchQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "CompositeSprite::pickArea() - Cannot pick sprites if clipping mode is off." );

        // Return nothing.
        return NULL;
    }

    // Upper left and lower right bound.
    Vector2 v1, v2;

    // The index of the first optional parameter.
    U32 firstArg;

    // Grab the number of elements in the first two parameters.
    U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
    U32 elementCount2 = 1;
    if (argc > 3)
        elementCount2 = Utility::mGetStringElementCount(argv[3]);

    // ("x1 y1 x2 y2")
    if ((elementCount1 == 4) && (argc < 9))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[2], 2);
        firstArg = 3;
    }
   
    // ("x1 y1", "x2 y2")
    else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 10))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[3]);
        firstArg = 4;
    }
   
    // (x1, y1, x2, y2)
    else if (argc > 5)
    {
        v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
        v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
        firstArg = 6;
    }
   
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::pickArea() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch the render transform.
    const b2Transform& renderTransform = object->getRenderTransform();
    
    // Translate into local space.
    v1 -= renderTransform.p;
    v2 -= renderTransform.p;

    // Calculate normalized AABB.
    b2AABB aabb;
    aabb.lowerBound.x = getMin( v1.x, v2.x );
    aabb.lowerBound.y = getMin( v1.y, v2.y );
    aabb.upperBound.x = getMax( v1.x, v2.x );
    aabb.upperBound.y = getMax( v1.y, v2.y );

    // Rotate the AABB into local space.
    CoreMath::mRotateAABB( aabb, -renderTransform.q.GetAngle(), aabb );

    // Perform query.
    pSpriteBatchQuery->queryArea( aabb, true );

    // Fetch result count.
    const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

    // Finish if no results.
    if (resultCount == 0 )
        return NULL;

    // Fetch results.
    typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add picked objects.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSpriteBatchItem->getBatchId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("CompositeSprite::pickArea() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear sprite batch query.
    pSpriteBatchQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(CompositeSprite, pickRay, const char*, 4, 6,  "(startx/y, endx/y) Picks sprites intersecting the specified ray with optional group/layer masks.\n"
                                                            "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
                                                            "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
                                                            "@return Returns list of sprite Ids")
{
    // Fetch sprite batch query and clear results.
    SpriteBatchQuery* pSpriteBatchQuery = object->getSpriteBatchQuery( true );

    // Is the sprite batch query available?
    if ( pSpriteBatchQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "CompositeSprite::pickRay() - Cannot pick sprites if clipping mode is off." );

        // Return nothing.
        return NULL;
    }

    // Upper left and lower right bound.
    Vector2 v1, v2;

    // The index of the first optional parameter.
    U32 firstArg;

    // Grab the number of elements in the first two parameters.
    U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
    U32 elementCount2 = 1;
    if (argc > 3)
        elementCount2 = Utility::mGetStringElementCount(argv[3]);

    // ("x1 y1 x2 y2")
    if ((elementCount1 == 4) && (argc < 9))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[2], 2);
        firstArg = 3;
    }
   
    // ("x1 y1", "x2 y2")
    else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 10))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[3]);
        firstArg = 4;
    }
   
    // (x1, y1, x2, y2)
    else if (argc > 5)
    {
        v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
        v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
        firstArg = 6;
    }
   
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::pickRay() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch the render transform.
    const b2Transform& renderTransform = object->getRenderTransform();

    // Transform into local space.
    v1 = b2MulT( renderTransform, v1 );
    v2 = b2MulT( renderTransform, v2 );

    // Perform query.
    pSpriteBatchQuery->queryRay( v1, v2, true );

    // Sanity!
    AssertFatal( pSpriteBatchQuery->getIsRaycastQueryResult(), "Invalid non-ray-cast query result returned." );

    // Fetch result count.
    const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

    // Finish if no results.
    if (resultCount == 0 )
        return NULL;

    // Sort ray-cast result.
    pSpriteBatchQuery->sortRaycastQueryResult();

    // Fetch results.
    typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSpriteBatchItem->getBatchId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("CompositeSprite::pickRay() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear sprite batch query.
    pSpriteBatchQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}
