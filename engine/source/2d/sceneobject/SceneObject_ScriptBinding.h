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

ConsoleFunction( getGlobalSceneObjectCount, S32, 1, 1,  "() - Gets the system-wide scene-object count.\n"
                                                        "@return The system-wide scene-object count." )
{
    return SceneObject::getGlobalSceneObjectCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, addToScene, void, 3, 3, "(Scene scene) Add the object to a scene.\n"
                                                      "@param scene the scene you wish to add this object to."
                                                      "@return No return value.")
{
    // Find Scene Object.
    Scene* pScene = dynamic_cast<Scene*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pScene )
    {
        Con::warnf("SceneObject::addToScene() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Add to Scene.
    pScene->addToScene( object );
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, removeFromScene, void, 2, 2, "() Remove the object from the scene."
              "@return No return value.")
{
    // Check we're in a scene.
    if ( !object->getScene() )
    {
        Con::warnf("SceneObject::removeFromScene() - Object is not in Scene! (%s)", object->getIdString());
        return;
    }

    // Remove from Scene.
    object->getScene()->removeFromScene( object );
}  

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getScene, S32, 2, 2, "() Get the scene the object is in.\n"
                                                         "@return (Scene scene) The scene this object is currently in.")
{
   return object->getScene() ? object->getScene()->getId() : 0;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setEnabled, void, 3, 3, "(bool status) Enables or disables the object.\n"
                                                      "@param status Whether to enable or disable the object."
                                                      "@return No return value.")
{
    // Set Enabled.
    object->setEnabled( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setLifetime, void, 3, 3, "(float lifetime) - Set Objects' Lifetime.\n"
                                                       "@param lifetime The amount of time, in seconds, before the object is automatically deleted.\n"
                                                                 "@return No return Value.")
{
    // Set Lifetime.
    object->setLifetime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLifetime, F32, 2, 2, "() Gets the Objects Lifetime.\n"
                                                      "@return (float lifetime) The objects lifetime left before it is automatically deleted.")
{
    // Return Lifetime.
    return object->getLifetime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneLayer, void, 3, 3, "(integer layer) Sets the objects scene layer (0-31).\n"
                                                    "The layer on which to place the object.\n"
                                                    "@param layer Integer in the range [0-31].\n"
                                                    "@return No return Value.")
{
    // Set Layer.
    object->setSceneLayer( dAtoi(argv[2]) );
}  

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSceneLayer, S32, 2, 2, "() Gets the Objects Layer.\n"
                                                        "@return (integer layer) The scene layer the object is on.")
{
    // Return Layer.
    return object->getSceneLayer();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneLayerDepth, void, 3, 3,  "(float layerDepth) Sets the objects scene layer depth.\n"
                                                            "@param layerDepth The layer depth can be any value.\n"
                                                            "@return No return Value.")
{
    // Set scene layer depth.
    object->setSceneLayerDepth( dAtof(argv[2]) );
}  

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSceneLayerDepth, F32, 2, 2,   "() Gets the scene layer depth.\n"
                                                            "@return (float layerDepth) The scene layer depth.")
{
    // Return Layer.
    return object->getSceneLayerDepth();
}


//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneLayerDepthFront, bool, 2, 2, "() Sets the layer depth to be the front-most within the current layer..\n"
                                                                "@return Whether the scene layer depth was adjusted or not.")
{
    return object->setSceneLayerDepthFront();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneLayerDepthBack, bool, 2, 2, "() Sets the layer depth to be the back-most within the current layer..\n"
                                                                "@return Whether the scene layer depth was adjusted or not.")
{
    return object->setSceneLayerDepthBack();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneLayerDepthForward, bool, 2, 2,   "() Sets the layer depth to be in-front of the object currently in-front within the current layer.\n"
                                                                    "@return Whether the scene layer depth was adjusted or not.")
{
    return object->setSceneLayerDepthForward();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneLayerDepthBackward, bool, 2, 2,  "() Sets the layer depth to be behind of the object currently behind within the current layer.\n"
                                                                    "@return Whether the scene layer depth was adjusted or not.")
{
    return object->setSceneLayerDepthBackward();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSceneGroup, void, 3, 3, "(integer group) - Sets the objects group (0-31).\n"
                                                         "The scene group to place the object in.\n"
                                                         "@param group Integer in the range [0-31].\n"
                                                         "@return No return Value.")
{
    // Set Group.
    object->setSceneGroup( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSceneGroup, S32, 2, 2, "() Gets the Objects Group.\n"
                                                        "@return (integer group) The scene group the object is on.")
{
    // Return Group.
    return object->getSceneGroup();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setArea, void, 3, 6, "(float x1, float y1, float x2, float y2) Sets the objects area.\n"
                                                   "@param x1 The lower left corner x position.\n"
                                                   "@param y1 The lower left corner y position.\n"
                                                   "@param x2 The upper right corner x position.\n"
                                                   "@param y2 The upper right corner y position.\n"
                                                   "@return No return value.")
{
   // Upper left and lower right bound.
   Vector2 v1, v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = Utility::mGetStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = Utility::mGetStringElementVector(argv[2]);
       v2 = Utility::mGetStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = Utility::mGetStringElementVector(argv[2]);
      v2 = Utility::mGetStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
       v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("SceneObject::setArea() - Invalid number of parameters!");
      return;
   }

   // Set Area.
   object->setArea( v1, v2 );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getArea, const char*, 2, 2, "() Gets the object's area.\n"
                                                          "@return (float x1/float y1/float x2/float y2) A space separated list of the lower left corner x and y position and the upper right corner x and y positions.")
{
    // Fetch position and half-size.
    const Vector2 position = object->getPosition();
    const Vector2 halfSize = object->getHalfSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%g %g %g %g", position.x-halfSize.x, position.y-halfSize.y, position.x+halfSize.x, position.y+halfSize.y);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAreaMin, const char*, 2, 2, "() Gets the upper left point of the object.\n"
                                                             "@return (float x1/float y1) The lower left corner x and y position of the object.")
{
    // Fetch position and half-size.
    const Vector2 position = object->getPosition();
    const Vector2 halfSize = object->getHalfSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 256, "%g %g", position.x-halfSize.x, position.y-halfSize.y);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAreaMax, const char*, 2, 2, "Gets the lower right point of the object.\n"
                                                             "@return (float x2/float y2) The upper right corner x and y position of the object.")
{
    // Fetch position and half-size.
    const Vector2 position = object->getPosition();
    const Vector2 halfSize = object->getHalfSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%g %g", position.x+halfSize.x, position.y+halfSize.y);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAutoSizing, bool, 2, 2,   "() - Gets whether the object automatically sizes itself or not.\n"
                                                        "You cannot set the size of an object that automatically sizes itself.\n"
                                                        "@return Whether the object automatically sizes itself or not." )
{
    return object->getAutoSizing();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSize, void, 3, 4, "(float width, float height) Sets the objects size.\n"
                                                   "@param width The width of the object.\n"
                                                   "@param height The height of the object.\n"
                                                     "@return No return value.")
{
    F32 width, height;

    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

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
        Con::warnf("SceneObject::setSize() - Invalid number of parameters!");
        return;
    }

    // Is the object auto-sizing?
    if ( object->getAutoSizing() )
    {
        // Yes, so warn.
        Con::warnf( "Cannot set the size of a type '%s' as it automatically sizes itself.", object->getClassName() );
        return;
    }

    // Set Size.
    object->setSize(Vector2(width, height));
}  

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSize, const char*, 2, 2, "() Gets the objects size.\n"
                                                          "@return (float width/float height) The width and height of the object.")
{
    return object->getSize().scriptThis();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSizeX, void, 3, 3, "(float width) Sets the width of the object.\n"
                                                    "@param width The width of the object."
                                                     "@return No return value.")
{
    // Set Size X-Component.
    object->setSize( Vector2( dAtof(argv[2]), object->getSize().y ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSizeY, void, 3, 3, "(float height) Sets the height of the object.\n"
                                                    "@param height The height of the object."
                                                      "@return No return value.")
{
    // Set Size Y-Component.
    object->setSize( Vector2( object->getSize().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSizeX, F32, 2, 2, "() Gets the width of the object.\n"
                                                   "@return (float width) The width of the object.")
{
    // Get Size X-Component.
    return object->getSize().x;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSizeY, F32, 2, 2, "() Gets the height of the object.\n"
                                                   "@return (float height) The height of the object.")
{
    // Get Size Y-Component.
    return object->getSize().y;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setWidth, void, 3, 3, "(float width) Sets the width of the object.\n"
                                                    "@param width The width of the object."
                                                      "@return No return value.")
{
    // Set Size Width Component.
    object->setSize( Vector2( dAtof(argv[2]), object->getSize().y ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setHeight, void, 3, 3, "(float height) Sets the height of the object."
                                                     "@param height The height of the object."
                                                      "@return No return value.")
{
    // Set Size Height Component.
    object->setSize( Vector2( object->getSize().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getWidth, F32, 2, 2, "() Gets the width of the object.\n"
                                                   "@return (float width) The width of the object.")
{
    // Get Size Width Component.
    return object->getSize().x;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getHeight, F32, 2, 2, "() Gets the height of the object.\n"
                                                    "@return (float height) The height of the object.")
{
    // Get Size Height Component.
    return object->getSize().y;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAABB, const char*, 2, 2,   "() Gets the axis-aligned bounding-box of the object.\n"
                                                            "@return (lowerXY upperXY) A space separated list of the lower left and upper right bounds.")
{
    // Fetch the AABB
    const b2AABB aabb = object->getAABB();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer( 64 );

    // Format Buffer.
    dSprintf( pBuffer, 64, "%g %g %g %g", aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y );
    
    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setPosition, void, 3, 4, "(float x, float y) - Sets the objects position.\n"
                                                       "@param x The position of the object along the horizontal axis.\n"
                                                       "@param y The position of the object along the vertical axis.\n"
                                                      "@return No return value.")
{
    // The new position.
    b2Vec2 position;

    // Elements in the first argument.
    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc == 3))
        position = Utility::mGetStringElementVector(argv[2]);

    // (x, y)
    else if ((elementCount == 1) && (argc == 4))
        position.Set(dAtof(argv[2]), dAtof(argv[3]));

    // Invalid
    else
    {
        Con::warnf("SceneObject::setPosition() - Invalid number of parameters!");
        return;
    }

    // Set Position.
    object->setPosition(position);
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setPositionX, void, 3, 3, "(float x) Sets the objects x position.\n"
                                                        "@param x The horizontal position of the object."
                                                      "@return No return value.")
{
    // Set Position X-Component.
    object->setPosition( b2Vec2( dAtof(argv[2]), object->getPosition().y ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setPositionY, void, 3, 3, "(float y) Sets the objects y position.\n"
                                                        "@param y The vertical position of the object."
                                                      "@return No return value.")
{
    // Set Position Y-Component.
    object->setPosition( b2Vec2( object->getPosition().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getPosition, const char*, 2, 2, "() Gets the object's position.\n"
                                                              "@return (float x/float y) The x and y (horizontal and vertical) position of the object.")
{
    // Get position.
    return object->getPosition().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getRenderPosition, const char*, 2, 2, "() Gets the current render position.\n"
                                                                    "@return (float x/float y) The x and y (horizontal and vertical) render position of the object.")
{
   // Get render position.
    return object->getRenderPosition().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setAngle, void, 3, 3,     "(float angle) - Sets the objects angle.\n"
                                                        "@param angle The angle of the object."
                                                        "@return No return value.")
{
    // Set Rotation.
    object->setAngle( mDegToRad( dAtof(argv[2]) ) );
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAngle, F32, 2, 2, "() Gets the object's angle.\n"
                                                      "@return (float angle) The object's current angle.")
{
    // Return angle.
    return mRadToDeg( object->getAngle());
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getRenderAngle, F32, 2, 2, "() Gets the object's render angle.\n"
                                                            "@return (float rotation) The object's current render angle.")
{
    // Return Rotation.
    return mRadToDeg( object->getRenderAngle() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setFixedAngle, void, 3, 3,    "(bool status?) - Whether the object angle is fixed or not.\n"
                                                            "@return No return Value.")
{
    // Set fixed angle.
    object->setFixedAngle( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getFixedAngle, bool, 2, 2, "() - Gets whether the angle is fixed or not.\n"
                                                             "@return (bool status?) Whether the angle is fixed or not.")
{
    // Get fixed angle.
    return object->getFixedAngle();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLocalCenter, const char*, 2, 2,    "() - Gets the local center of mass.\n"
                                                                    "@return (float localCenter) Gets the local center of mass.")
{
    // Get local center.
    return object->getLocalCenter().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getWorldCenter, const char*, 2, 2,    "() - Gets the world center of mass.\n"
                                                                    "@return (float worldCenter) Gets the world center of mass.")
{
    // Get world center.
    return object->getWorldCenter().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLocalPoint, const char*, 3, 4, "(float worldPointX/Y) - Returns a local point from the world point.\n"
                                                                "Converts a point in world space to local space.\n"
                                                                "@param worldPointX/Y The world point to convert into a local point."
                                                                "@return (float localPointX/Y) The local point that was converted from the world point passed.")
{
   // The new position.
   Vector2 worldPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      worldPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      worldPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getLocalPoint() - Invalid number of parameters!");
      return NULL;
   }

    // Calculate local coordinate.
    Vector2 localPoint = object->getLocalPoint(worldPoint);

    return localPoint.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getWorldPoint, const char*, 3, 4, "(float localPointX/Y) - Returns a world point from the local point.\n"
                                                                "Converts a point in local space to world space.\n"
                                                                "@param localPointX/Y The local point to convert into a world point."
                                                                "@return (float worldPointX/Y) The world point that was converted from the local point passed.")
{
   // The new position.
   Vector2 localPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      localPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      localPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getWorldPoint() - Invalid number of parameters!");
      return false;
   }

    // Calculate world coordinate.
    Vector2 worldPoint = object->getWorldPoint(localPoint);

    return worldPoint.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLocalVector, const char*, 3, 4,    "(float worldVectorX/Y) - Returns a local vector from the world vector.\n"
                                                                    "Converts a vector in world space to local space.\n"
                                                                    "@param worldVectorX/Y The world vector to convert into a vector point."
                                                                    "@return (float localVectorX/Y) The local vector that was converted from the world vector passed.")
{
   // The new vector.
   Vector2 worldVector;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      worldVector = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      worldVector = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getLocalVector() - Invalid number of parameters!");
      return NULL;
   }

    // Calculate local vector.
    Vector2 localVector = object->getLocalVector(worldVector);

    return localVector.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getWorldVector, const char*, 3, 4,    "(float localVectorX/Y) - Returns a world vector from the local vector.\n"
                                                                    "Converts a vector in local space to world space.\n"
                                                                    "@param localVectorX/Y The local vector to convert into a world vector."
                                                                    "@return (float worldVectorX/Y) The world vector that was converted from the local vector passed.")
{
   // The new vector.
   Vector2 localVector;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      localVector = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      localVector = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getWorldVector() - Invalid number of parameters!");
      return false;
   }

    // Calculate world vector.
    Vector2 worldVector = object->getWorldVector(localVector);

    return worldVector.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getIsPointInOOBB, bool, 3, 4,     "(worldPointX/Y) - Returns whether the world point intersects with the objects OOBB or not.\n"
                                                                "@param worldPointX/Y The world point to check.\n"
                                                                "@return (bool isInside) Whether the world point intersects with the objects OOBB or not.")
{
   Vector2 worldPoint;

   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("X Y")
   if ((elementCount == 2) && (argc == 3))
      worldPoint = Utility::mGetStringElementVector(argv[2]);

   // (X, Y)
   else if ((elementCount == 1) && (argc == 4))
      worldPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getIsPointInOOBB() - Invalid number of parameters!");
      return false;
   }

    // Calculate if point intersects the OOBB.
    return object->getIsPointInOOBB( worldPoint );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getIsPointInCollisionShape, bool, 4, 5,   "(int shapeIndex, worldPointX/Y) - Returns whether the world point intersects with the specified collision shape or not.\n"
                                                                        "@param worldPointX/Y The world point to check.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@return (bool isInside) Whether the world point intersects with the specified collision shape or not.")
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getIsPointInCollisionShape() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Fetch shape index.
    Vector2 worldPoint;

    U32 elementCount = Utility::mGetStringElementCount(argv[3]);

    // ("X Y")
    if ((elementCount == 2) && (argc == 4))
        worldPoint = Utility::mGetStringElementVector(argv[2]);

    // (X, Y)
    else if ((elementCount == 1) && (argc == 5))
        worldPoint = Vector2(dAtof(argv[3]), dAtof(argv[4]));

    // Invalid
    else
    {
        Con::warnf("SceneObject::getIsPointInCollisionShape() - Invalid number of parameters!");
        return false;
    }

    // Calculate if point intersects the collision shape.
    return object->getIsPointInCollisionShape( shapeIndex, worldPoint );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setBodyType, void, 3, 3, "(bodyType type) Sets the body type.\n"
                                                      "@return No return value.")
{
    // Fetch body type.
    const b2BodyType type = SceneObject::getBodyTypeEnum(argv[2]);

    // Check for error.
    if ( type != b2_staticBody && type != b2_kinematicBody && type != b2_dynamicBody )
        return;

    // Set body type.
    object->setBodyType( type );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getBodyType, const char*, 2, 2,   "() Gets the body type.\n"
                                                                "@return The body type." )
{
    return SceneObject::getBodyTypeDescription( object->getBodyType() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setActive, void, 2, 3,            "([bool status?]) - Sets the body active status.\n"
                                                                "@param status - Whether the body should be active or not (defaults to true)."
                                                                 "@return No return Value.")
{
    object->setActive( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getActive, bool, 2, 2,            "() Gets the body active status.\n"
                                                                "@return (bool status) Whether the body is active or not.")
{
    return object->getActive();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setAwake, void, 2, 3,            "([bool status?]) - Sets whether the body is awake or not.\n"
                                                                "@param status - Whether the body should be awake or not (defaults to true)."
                                                                 "@return No return Value.")
{
    object->setAwake( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAwake, bool, 2, 2,            "() Gets whether the body is awake or not.\n"
                                                                "@return (bool status) Whether the body is awake or not.")
{
    return object->getAwake();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setBullet, void, 2, 3,            "([bool status?]) - Sets whether the body is a bullet or not.\n"
                                                                "@param status - Whether the body should be a bullet (fast moving body) or not (defaults to true)."
                                                                 "@return No return Value.")
{
    object->setBullet( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getBullet, bool, 2, 2,            "() Gets whether the body is a bullet or not.\n"
                                                                "@return (bool status) Whether the body is a bullet or not.")
{
    return object->getBullet();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSleepingAllowed, void, 2, 3,   "([bool status?]) - Sets whether the body is allowed to sleep or not.\n"
                                                                "@param status - Whether sleeping is allowed on the body or not (defaults to true)."
                                                                "@return No return Value.")
{
    object->setSleepingAllowed( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSleepingAllowed, bool, 2, 2,   "() Gets whether the body is allowed to sleep or not.\n"
                                                                "@return (bool status) Whether the body is allowed to sleep or not.")
{
    return object->getSleepingAllowed();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getMass, F32, 2, 2,               "() Gets the total mass of the body.\n"
                                                                "@return (float mass) The total mass of the body.  If object is not in a scene then mass is always zero.")
{
    return object->getMass();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getInertia, F32, 2, 2,            "() Gets the rotation inertia of the body around the local origin.\n"
                                                                "@return (float inertia) rotation inertia of the body around the local origin (Kg-m^2).  If object is not in a scene then mass is always zero.")
{
    return object->getInertia();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setCollisionSuppress, void, 2, 3, "([bool status?]) - Sets the collision suppress status.\n"
                                                                "@param status Whether to suppress the collision or not (defaults to true).\n"
                                                                "@return No return Value.")
{
    // Set Collision Suppress.
    object->setCollisionSuppress( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionSuppress, bool, 2, 2, "() Gets the collision suppress status.\n"
                                                                "@return (bool status) Whether to supress the collision or not.")
{
    // Get Collision Callback.
    return object->getCollisionSuppress();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setGatherContacts, void, 3, 3,    "(bool gatherContacts) - Sets whether to gather contacts or not.\n"
                                                                "@param gatherContacts Whether to gather contacts or not.  By default contact gather is off as it can become expensive if a lot of contacts are being processed.\n"
                                                                "@return No return value.")
{
    object->setGatherContacts( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getGatherContacts, bool, 2, 2,    "() - Gets whether the gathering of contacts is active or not.\n"
                                                                "@return Whether the gathering of contacts is active or not.")
{
    return object->getGatherContacts();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getContactCount, S32, 2, 2,   "() Gets the current contact count.  This will always be zero if contact gathering is off.\n"
                                                            "@return (int count) - Gets the current contact count.")
{
    return object->getCurrentContactCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getContact, const char*, 3, 3,    "(contactIndex) Gets the contact at the specified contact index.\n"
                                                                "@param contactIndex The contact at the specified index.\n"
                                                                "@return The contact at the specified index.\n" )
{
    // Fetch contact index.
    const S32 contactIndex = dAtoi(argv[2]);

    // Is the contact index valid?
    if ( contactIndex < 0 || contactIndex >= (S32)object->getCurrentContactCount() )
    {
        // No, so warn.
        Con::warnf("SceneObject::getContact() - The contact index '%d' is out of bounds.", contactIndex );
        return NULL;
    }

    // Fetch contacts.
    const Scene::typeContactVector* pCurrentContacts = object->getCurrentContacts();

    // Sanity!
    AssertFatal( pCurrentContacts != NULL, "SceneObject::getContact() - Contacts not initialized correctly." );

    // Fetch contact.
    const TickContact& tickContact = (*pCurrentContacts)[contactIndex];

    // Fetch scene objects.
    SceneObject* pSceneObjectThis;
    SceneObject* pSceneObjectCollider;
    S32 shapeIndexThis;
    S32 shapeIndexCollider;

    // Calculate the respective colliding objects.
    if ( tickContact.mpSceneObjectA == object )
    {
        pSceneObjectThis = tickContact.mpSceneObjectA;
        pSceneObjectCollider = tickContact.mpSceneObjectB;
        shapeIndexThis = pSceneObjectThis->getCollisionShapeIndex( tickContact.mpFixtureA );
        shapeIndexCollider = pSceneObjectCollider->getCollisionShapeIndex( tickContact.mpFixtureB );
    }
    else
    {
        pSceneObjectThis = tickContact.mpSceneObjectB;
        pSceneObjectCollider = tickContact.mpSceneObjectA;
        shapeIndexThis = pSceneObjectThis->getCollisionShapeIndex( tickContact.mpFixtureB );
        shapeIndexCollider = pSceneObjectCollider->getCollisionShapeIndex( tickContact.mpFixtureA );
    }

    // Sanity!
    AssertFatal( shapeIndexThis >= 0, "SceneObject::getContact() - Cannot find shape index reported on physics proxy of a fixture." );
    AssertFatal( shapeIndexCollider >= 0, "SceneObject::getContact() - Cannot find shape index reported on physics proxy of a fixture." );

    // Fetch normal and contact points.
    const U32& pointCount = tickContact.mPointCount;
    const b2Vec2& normal = tickContact.mWorldManifold.normal;
    const b2Vec2& point1 = tickContact.mWorldManifold.points[0];
    const b2Vec2& point2 = tickContact.mWorldManifold.points[1];

    // Fetch collision impulse information
    const F32 normalImpulse1 = tickContact.mNormalImpulses[0];
    const F32 normalImpulse2 = tickContact.mNormalImpulses[1];
    const F32 tangentImpulse1 = tickContact.mTangentImpulses[0];
    const F32 tangentImpulse2 = tickContact.mTangentImpulses[1];

    // Format buffer.
    char* pReturnBuffer = Con::getReturnBuffer(128);
    if ( pointCount == 2 )
    {
        dSprintf(pReturnBuffer, 128,
            "%d %d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
            pSceneObjectCollider,
            shapeIndexThis, shapeIndexCollider,
            normal.x, normal.y,
            point1.x, point1.y,
            normalImpulse1,
            tangentImpulse1,
            point2.x, point2.y,
            normalImpulse2,
            tangentImpulse2 );
    }
    else if ( pointCount == 1 )
    {
        dSprintf(pReturnBuffer, 128,
            "%d %d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
            pSceneObjectCollider,
            shapeIndexThis, shapeIndexCollider,
            normal.x, normal.y,
            point1.x, point1.y,
            normalImpulse1,
            tangentImpulse1 );
    }
	else
	{
        dSprintf(pReturnBuffer, 64,
            "%d %d %d",
            pSceneObjectCollider,
            shapeIndexThis, shapeIndexCollider );
	}

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setCollisionMasks, void, 3, 4,    "(groupMask, [layerMask]) - Sets the collision masks.\n"
                                                                "Collision masks limit the objects that are collided with based on their scene group and scene Layer. In order for two objects to collide, the sending object must have the receiving object's Graph Group and Rendering Layer in its masks. The receiving object's masks, however, do not have to contain the sending object's Graph Group and Rendering Layer.\n"
                                                                "@param sceneGroupMask The scene group(s) to enable collision with.\n"
                                                                "@param sceneLayerMask The scene layer(s) to enable collision with. If sceneLayerMask is not specified then all scene layers will be included."
                                                                "@return No return value.")
{
    // Calculate Group-Mask.
    const U32 groupMask = dAtoi(argv[2]);
    // Calculate Layer-Mask.
    const U32 layerMask = (argc > 3) ? dAtoi(argv[3]) : MASK_ALL;

    // Set Collision Masks.
    object->setCollisionGroupMask( groupMask );
    object->setCollisionLayerMask( layerMask );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionMasks, const char*, 2, 2, "Gets the collision group/layer masks.\n"
                                                                    "@return (groupMask/layerMask) The scene group(s) and scene layer(s) the object is enabled to collide with.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getCollisionGroupMask(), (U32)object->getCollisionLayerMask());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setCollisionAgainst, void, 3, 4, "(SceneObject object, [clearMasks? = false]) - Sets the collision masks against a specific object.\n"
                                                               "@param object The SceneObject to set collision masks against\n"
                                                               "@param clearMasks Whether or not to clear the collision masks before setting them against the object."
                                                                "@return No return value.")
{
    // Find SceneObject Object.
    const SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject )
    {
        Con::warnf("SceneObject::setCollisionAgainst() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Fetch ClearMasks Option.
    bool clearMasks = argc >= 4 ? dAtob(argv[3]) : false;

    // Set Collision Against.
    object->setCollisionAgainst( pSceneObject, clearMasks );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setCollisionLayers, void, 2, 2 + MASK_BITCOUNT,  "(layers) - Sets the collision layers(s).\n"
                                                                            "@param layers A list of layers to collide with.\n"
                                                                            "@return No return value.")
{
    // Set to all if no arguments.
    if ( argc == 2 )
    {
        object->setCollisionLayerMask(MASK_ALL);
        return;
    }

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        object->setCollisionLayerMask(MASK_ALL);
        return;
    }
    else if ( elementCount == 1 )
    {
        if ( dStricmp( argv[2], "all" ) == 0 )
        {
            object->setCollisionLayerMask(MASK_ALL);
            return;
        }
        else if ( dStricmp( argv[2], "none" ) == 0 || dStricmp( argv[2], "off" ) == 0 )
        {
            object->setCollisionLayerMask(0);
            return;
        }

        return;
    }

    // The mask.
    U32 mask = 0;

    // Space separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
            }
         
            mask |= (1 << bit);
        }
    }

    // Comma separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            S32 bit = dAtoi(argv[i]);
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
            }

            mask |= (1 << bit);
        }
    }
    // Set Collision Layers
    object->setCollisionLayerMask(mask);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setCollisionGroups, void, 2, 2 + MASK_BITCOUNT,  "(groups) - Sets the collision group(s).\n"
                                                                            "@param groups A list of collision groups to collide with.\n"
                                                                            "@return No return value.")
{
    // Set to all if no arguments.
    if ( argc == 2 )
    {
        object->setCollisionGroupMask(MASK_ALL);
        return;
    }

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        object->setCollisionGroupMask(MASK_ALL);
        return;
    }
    else if ( elementCount == 1 )
    {
        if ( dStricmp( argv[2], "all" ) == 0 )
        {
            object->setCollisionGroupMask(MASK_ALL);
            return;
        }
        else if ( dStricmp( argv[2], "none" ) == 0 || dStricmp( argv[2], "off" ) == 0 )
        {
            object->setCollisionGroupMask(0);
            return;
        }
    }

    // The mask.
    U32 mask = 0;

    // Space separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
            }
         
            mask |= (1 << bit);
        }
    }

    // Comma separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            S32 bit = dAtoi(argv[i]);
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
            }

            mask |= (1 << bit);
        }
    }
    // Set Collision Groups.
    object->setCollisionGroupMask(mask);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionLayers, const char*, 2, 2, "() - Gets the collision layers.\n"
                                                                     "@return (collisionLayers) A list of collision layers.")
{
    U32 mask = object->getCollisionLayerMask();

    bool first = true;
    char* bits = Con::getReturnBuffer(128);
    bits[0] = '\0';
    for (S32 i = 0; i < MASK_BITCOUNT; i++)
    {
        if (mask & BIT(i))
        {
            char bit[4];
            dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
            first = false;
            dStrcat(bits, bit);
        }
    }

    return bits;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionGroups, const char*, 2, 2, "() - Gets the collision groups.\n"
                                                                     "@return (collisionGroups) A list of collision groups.")
{
    U32 mask = object->getCollisionGroupMask();

    bool first = true;
    char* bits = Con::getReturnBuffer(128);
    bits[0] = '\0';
    for (S32 i = 0; i < MASK_BITCOUNT; i++)
    {
        if (mask & BIT(i))
        {
            char bit[4];
            dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
            first = false;
            dStrcat(bits, bit);
        }
    }

    return bits;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setDefaultDensity, void, 3, 4,    "(float density, [bool updateShapes]) - Sets the default density to use when creating collision shapes.\n"
                                                                "@param density The default density to use when creating collision shapes."
                                                                "@param updateShapes Whether to update the density on any existing shapes or not (default is true)."
                                                                "@return No return Value.")
{
    // Fetch density.
    const F32 density = dAtof(argv[2]);
    if ( density < 0.0f )
    {
        Con::warnf("SceneObject::setDefaultDensity() - Invalid density of %g.", density);
    }

    // Set default density.
    object->setDefaultDensity( density, argc > 3 ? dAtob(argv[3]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getDefaultDensity, F32, 2, 2,  "() - Gets the default density.\n"
                                                            "@return (float density) The default density.")
{
    // Get default density.
    return object->getDefaultDensity();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setDefaultFriction, void, 3, 4,   "(float friction, [bool updateShapes]) - Sets the default friction to use when creating collision shapes.\n"
                                                                "@param friction The default friction to use when creating collision shapes."
                                                                "@param updateShapes Whether to update the friction on any existing shapes or not (default is true)."
                                                                "@return No return Value.")
{
    // Fetch friction.
    const F32 friction = dAtof(argv[2]);
    if ( friction < 0.0f )
    {
        Con::warnf("SceneObject::setDefaultFriction() - Invalid friction of %g.", friction);
    }

    // Set default friction.
    object->setDefaultFriction( friction, argc > 3 ? dAtob(argv[3]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getDefaultFriction, F32, 2, 2,     "() - Gets the default friction.\n"
                                                                "@return (float friction) The default friction.")
{
    // Get default friction.
    return object->getDefaultFriction();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setDefaultRestitution, void, 3, 4,    "(float restitution, [bool updateShapes]) - Sets the default restitution to use when creating collision shapes..\n"
                                                                    "@param restitution The default restitution to use when creating collision shapes."
                                                                    "@param updateShapes Whether to update the restitution on any existing shapes or not (default is true)."
                                                                    "@return No return Value.")
{
    // Fetch restitution.
    const F32 restitution = dAtof(argv[2]);
    if ( restitution < 0.0f )
    {
        Con::warnf("SceneObject::setDefaultRestitution() - Invalid restitution of %g.", restitution);
    }

    // Set default restitution.
    object->setDefaultRestitution( restitution, argc > 3 ? dAtob(argv[3]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getDefaultRestitution, F32, 2, 2,  "() - Gets the default restitution.\n"
                                                            "@return (float density) The default restitution.")
{
    // Get default restitution.
    return object->getDefaultRestitution();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setLinearVelocity, void, 3, 4, "(float velocityX, float velocityY) - Sets Objects Linear Velocity.\n"
                                                             "Sets the velocity at which the object will move.\n"
                                                             "@param velocityX The x component of the velocity.\n"
                                                             "@param velocityY The y component of the velocity.\n\n"
                                                            "@return No return Value.")
{
   // The velocity.
   Vector2 velocity;

   // Grab the element count.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // (x, y)
   if ((elementCount == 1) && (argc > 3))
      velocity = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // ("x y")
   else if ((elementCount == 2) && (argc < 4))
      velocity = Utility::mGetStringElementVector(argv[2]);

   // Invalid
   else
   {
      Con::warnf("SceneObject::setLinearVelocity() - Invalid number of parameters!");
      return;
   }

   // Set Linear Velocity.
   object->setLinearVelocity(velocity);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setLinearVelocityX, void, 3, 3,   "(float velocityX) - Sets Objects Linear Velocity X-Component.\n"
                                                                "@param velocityX The x component of the velocity.\n"
                                                                "@return No return Value.")
{
    // Set Linear Velocity X-Component.
    object->setLinearVelocity( Vector2( dAtof(argv[2]), object->getLinearVelocity().y ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setLinearVelocityY, void, 3, 3,   "(float velocityY) - Sets Objects Linear Velocity Y-Component.\n"
                                                                "@param velocityY The y component of the velocity.\n"
                                                                "@return No return Value.")
{
    // Set Linear Velocity Y-Component.
    object->setLinearVelocity( Vector2( object->getLinearVelocity().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setLinearVelocityPolar, void, 4, 4,   "(float angle, float speed) - Sets Objects Linear Velocity using Polar-speed.\n"
                                                                    "Sets the speed and direction at which the object will move.\n"
                                                                    "This has the same effect as setLinearVelocity(), it just sets the velocity in a different way.\n"
                                                                    "@param angle The angle of the direction at which the object to move.\n"
                                                                    "@param speed The speed at which the object will move.\n"
                                                                    "@return No return Value.")
{
    // Renormalize Angle.
    F32 angle = mDegToRad(mFmod(dAtof(argv[2]), 360.0f));

    // Fetch Speed.
    F32 speed = dAtof(argv[3]);

    // Calculate Angle.
    F32 sin, cos;
    mSinCos( angle, sin, cos );

    // Set Gross Linear Velocity.
    object->setLinearVelocity( Vector2( sin*speed, -cos*speed ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearVelocityPolar, const char*, 2, 2, "() - Gets Objects Linear Velocity using Polar angle/speed.\n"
                                                                         "@return (float angle, float speed) The angle and speed the object is moving at.")
{
    // Get Linear Velocity.
    Vector2 linearVelocity = object->getLinearVelocity();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%g %g", mRadToDeg(mAtan(linearVelocity.x, -linearVelocity.y)), linearVelocity.Length() );

    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearVelocity, const char*, 2, 2, "() - Gets Object Linear Velocity.\n"
                                                                    "@return (float velocityX, float velocityY) The x and y velocities of the object.")
{
    // Get Linear Velocity.
    Vector2 linearVelocity = object->getLinearVelocity();

    return linearVelocity.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearVelocityX, F32, 2, 2, "() - Gets Object Linear Velocity X.\n"
                                                             "@return (float velocityX) The object's velocity along the x axis.")
{
    // Get Linear Velocity X.
    return object->getLinearVelocity().x;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearVelocityY, F32, 2, 2, "() - Gets Object Linear Velocity Y.\n"
                                                             "@return (float velocityY) The object's velocity along the y axis.")
{
    // Get Linear Velocity Y.
    return object->getLinearVelocity().y;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearVelocityFromWorldPoint, const char*, 3, 4,   "(worldPointX/Y) - Gets the linear velocity from a world point.\n"
                                                                                    "@return (float linearVelocityX/Y) The linear velocity at the world point.")
{
    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[2]);

    Vector2 worldPoint;

    S32 nextArg = 3;
    if ( worldPointElementCount == 1 && argc > 3 )
    {
        worldPoint.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::getLinearVelocityFromWorldPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch linear velocity.
    Vector2 linearVelocity = object->getLinearVelocityFromWorldPoint( worldPoint );
    return linearVelocity.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearVelocityFromLocalPoint, const char*, 3, 4,   "(localPointX/Y) - Gets the linear velocity from a local point.\n"
                                                                                    "@return (float linearVelocityX/Y) The linear velocity at the local point.")
{
    // Local point.
    const U32 localPointElementCount = Utility::mGetStringElementCount(argv[2]);

    Vector2 localPoint;

    S32 nextArg = 3;
    if ( localPointElementCount == 1 && argc > 3 )
    {
        localPoint.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( localPointElementCount == 2 )
    {
        localPoint = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::getLinearVelocityFromLocalPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch linear velocity.
    Vector2 linearVelocity = object->getLinearVelocityFromWorldPoint( localPoint );
    return linearVelocity.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setAngularVelocity, void, 3, 3,   "(float velocity) - Sets Objects Angular Velocity.\n"
                                                                "@param velocity The speed at which the object will rotate.\n"
                                                                "@return No return Value.")
{
    // Set Angular Velocity.
    object->setAngularVelocity( mDegToRad( dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAngularVelocity, F32, 2, 2, "() - Gets Object Angular Velocity.\n"
                                                             "@return (float velocity) The speed at which the object is rotating.")
{
    // Get Angular Velocity.
    return mRadToDeg( object->getAngularVelocity() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setLinearDamping, void, 3, 3,     "(float damping) - Sets the linear damping.\n"
                                                                "@param damping - The linear damping scale."
                                                                "@return No return Value.")
{
    // Set linear damping.
    object->setLinearDamping( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getLinearDamping, F32, 2, 2,  "() - Gets the linear damping.\n"
                                                            "@return (float damping) The linear damping.")
{
    // Get linear damping.
    return object->getLinearDamping();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setAngularDamping, void, 3, 3,    "(float damping) - Sets the angular damping.\n"
                                                                "@param damping - The angular damping scale."
                                                                "@return No return Value.")
{
    // Set angular damping.
    object->setAngularDamping( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAngularDamping, F32, 2, 2, "() - Gets the angular damping.\n"
                                                            "@return (float damping) The angular damping.")
{
    // Get angular damping.
    return object->getAngularDamping();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, moveTo, bool, 4, 7,  "(worldPoint X/Y, speed, [autoStop = true], [warpToTarget = true]) - Moves the object to the specified world point.\n"
                                                "The point is moved by calculating the initial linear velocity required and applies it.\n"
                                                "The object may never reach the point if it has linear damping applied or collides with another object.\n"
                                                "@param worldPoint/Y The world point to move the object to.\n"
                                                "@param speed The speed (in m/s) to use to move to the specified point."
                                                "@param autoStop? Whether to automatically set the linear velocity to zero when time has elapsed or not\n"
                                                "@param warpToTarget? Whether to move instantly to the target point after the specified time or not in-case the target was not quite reached.\n"
                                                "@return Whether the move could be started or not.")
{
    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[2]);

    Vector2 worldPoint;

    S32 nextArg = 3;
    if ( worldPointElementCount == 1 && argc > 3 )
    {
        worldPoint.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::moveTo() - Invalid number of parameters!");
        return false;
    }

    // Speed.
    const F32 speed = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->moveTo( worldPoint, speed );
    }

    // Auto stop?
    const bool autoStop = dAtob(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->moveTo( worldPoint, speed, autoStop );
    }

    // Warp to target?
    const bool warpToTarget = dAtob(argv[nextArg++]);

    return object->moveTo( worldPoint, speed, autoStop, warpToTarget );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, rotateTo, bool, 4, 6,    "(angle, speed, [autoStop = true], [warpToTarget = true]) - Rotates the object to the specified angle.\n"
                                                    "The angle is rotated to by calculating the initial angular velocity required and applies it.\n"
                                                    "The object may never reach the point if it has angular damping applied or collides with another object.\n"
                                                    "@param angle The angle to rotate the object to.\n"
                                                    "@param speed The speed (in degree/s) to use to rotate to the specified angle."
                                                    "@param autoStop? Whether to automatically set the angular velocity to zero when time has elapsed or not\n"
                                                    "@param warpToTarget? Whether to rotate instantly to the target angle after the specified time or not in-case the target was not quite reached.\n"
                                                    "@return Whether the rotation could be started or not.")
{
    // Fetch angle.
    const F32 angle = mDegToRad(dAtof(argv[2]));

    // Speed.
    const F32 speed = mDegToRad(dAtof(argv[3]));

    if ( argc == 4 )
    {
        return object->rotateTo( angle, speed );
    }

    // Auto stop?
    const bool autoStop = dAtob(argv[4]);

    if ( argc == 5 )
    {
        return object->rotateTo( angle, speed, autoStop );
    }

    // Warp to target.
    const bool warpToTarget = dAtob(argv[5]);

    return object->rotateTo( angle, speed, autoStop, warpToTarget );

}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, cancelMoveTo, void, 2, 3,     "([autoStop = true]) - Stop a previous 'moveTo' command.\n"
                                                            "@param autoStop? - Whether to automatically set the linear velocity to zero or not\n"
                                                            "@return No return value.")
{
    if ( argc == 2 )
    {
        object->cancelMoveTo();
    }
    else
    {
        const bool autoStop = dAtob(argv[2]);
        object->cancelMoveTo( autoStop );
    }
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, cancelRotateTo, void, 2, 3,   "([autoStop = true]) - Stop a previous 'rotateTo' command.\n"
                                                            "@param autoStop? - Whether to automatically set the angular velocity to zero or not\n"
                                                            "@return No return value.")
{
    if ( argc == 2 )
    {
        object->cancelRotateTo();
    }
    else
    {
        const bool autoStop = dAtob(argv[2]);
        object->cancelRotateTo( autoStop );
    }
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, isMoveToComplete, bool, 2, 2,     "() - Gets whether a previous 'moveTo' command has completed or not.\n"
                                                                "@return No return value.")
{
    return object->isMoveToComplete();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, isRotateToComplete, bool, 2, 2,   "() - Gets whether a previous 'rotateTo' command has completed or not.\n"
                                                                "@return No return value.")
{
    return object->isRotateToComplete();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, applyForce, void, 4, 6,       "(worldForce X/Y, [worldPoint X/Y]) - Applies a force at a world point.\n"
                                                            "If the force is not applied at the center of mass, it will generate a torque and affect the angular velocity.\n"
                                                            "@param worldForceX/Y - The world force vector in Newtons (N)."
                                                            "@param worldPointX/Y - The world point where the force is applied.  If world point is not specified, the center of mass is used."
                                                            "@return No return Value.")
{
    // World force.
    const U32 worldForceElementCount = Utility::mGetStringElementCount(argv[2]);

    b2Vec2 worldForce;

    S32 nextArg = 3;
    if ( worldForceElementCount == 1 && argc > 3 )
    {
        worldForce.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldForceElementCount == 2 )
    {
        worldForce = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::applyForce() - Invalid number of parameters!");
        return;
    }

    if ( argc <= nextArg )
    {
        // Apply force.
        object->applyForce( worldForce );
    }

    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldPoint;

    if ( worldPointElementCount == 1 && argc > (nextArg+1) )
    {
        worldPoint.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::applyForce() - Invalid number of parameters!");
        return;
    }

    // Apply force.
    object->applyForce( worldForce, worldPoint );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, applyTorque, void, 3, 3,      "(torque) - Applies a torque.\n"
                                                            "This affects the angular velocity without affecting the linear velocity of the center of mass.\n"
                                                            "@param torque - The torque in Newton-metres (N-m)."
                                                            "@return No return Value.")
{
    // Fetch torque.
    const F32 torque = dAtof(argv[2]);

    // Apply torque.
    object->applyTorque( torque );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, applyLinearImpulse, void, 4, 6,   "(worldImpulse X/Y, [worldPoint X/Y]) - Applies an impulse at a world point.\n"
                                                                "This immediately modifies the linear velocity.  It also modifies the angular velocity if the point of application is not the center of mass.\n"
                                                                "@param worldImpulse/Y - The world impulse vector in Newtons (N-seconds) or Kg-m/s."
                                                                "@param worldPointX/Y - The world point where the force is applied.  If world point is not specified, the center of mass is used."
                                                                "@return No return Value.")
{
    // World impulse.
    const U32 worldImpulseElementCount = Utility::mGetStringElementCount(argv[2]);

    b2Vec2 worldImpulse;

    S32 nextArg = 3;
    if ( worldImpulseElementCount == 1 && argc > 3 )
    {
        worldImpulse.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldImpulseElementCount == 2 )
    {
        worldImpulse = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::applyLinearImpulse() - Invalid number of parameters!");
        return;
    }

    if ( argc <= nextArg )
    {
        // Apply force.
        object->applyForce( worldImpulse );
    }

    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldPoint;

    if ( worldPointElementCount == 1 && argc > (nextArg+1) )
    {
        worldPoint.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::applyLinearImpulse() - Invalid number of parameters!");
        return;
    }

    // Apply linear impulse.
    object->applyLinearImpulse( worldImpulse, worldPoint );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, applyAngularImpulse, void, 3, 3,  "(impulse) - Applies an angular impulse.\n"
                                                                "@param impulse - The impulse in Kg*m*m/s."
                                                                "@return No return Value.")
{
    // Fetch impulse.
    const F32 impulse = dAtof(argv[2]);

    // Apply impulse.
    object->applyAngularImpulse( impulse );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setGravityScale, void, 3, 3,  "(float scale) - Sets the scaled world gravity.\n"
                                                            "@param scale - The scaled world gravity."
                                                            "@return No return Value.")
{
    // Set gravity scale.
    object->setGravityScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getGravityScale, F32,2, 2,    "() - Gets the gravity scale.\n"
                                                            "@return (float scale) The gravity scale.")
{
    // Get gravity scale.
    return object->getGravityScale();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, deleteCollisionShape, bool, 3, 3,        "(int shapeIndex) - Deletes the specified collision shape.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@return Whether the collision shape was successfully deleted or not." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::deleteCollisionShape() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Delete collision shape.
    object->deleteCollisionShape( shapeIndex );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, clearCollisionShapes, void, 2, 2,        "() - Deletes all collision shapes.\n"
                                                                        "@return No return value." )
{
    object->clearCollisionShapes();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionShapeCount, S32, 2, 2,    "() - Gets the number of collision shapes.\n"
                                                                    "@return (int count) The number of collision shapes.")
{
    // Get collision shape count.
    return object->getCollisionShapeCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionShapeType, const char*, 3, 3, "(int shapeIndex) - Gets the type of collision shape at the specified index.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@return (type) The type of collision shape at the specified index.")
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeType() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    return SceneObject::getCollisionShapeTypeDescription( object->getCollisionShapeType( shapeIndex ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionShapeArea, F32, 3, 3,    "(int shapeIndex) - Gets the area of collision shape at the specified index.\n"
                                                                "@param shapeIndex - The index of the collision shape."
                                                                "@return (area) The area of collision shape at the specified index.")
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeArea() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Calculate area if the shape type is circle
    if (object->getCollisionShapeType( shapeIndex ) == b2Shape::e_circle)
    {
        // Get the radius
        const F32 radius = object->getCircleCollisionShapeRadius( shapeIndex );

        // Return the area
        return b2_pi * radius * radius;
    }

    // Calculate area if the shape type is polygon
    if (object->getCollisionShapeType( shapeIndex ) == b2Shape::e_polygon)
    {
        const b2PolygonShape* pShape = object->getCollisionPolygonShape( shapeIndex );

        // Sanity
        if ( pShape->GetVertexCount() < 3 )
        {
            Con::warnf("SceneObject::getCollisionShapeArea() - Fewer than 3 vertices in polygon shape.");
            return 0.0f;
        }

        // Calculate the sum of cross-products
        F32 crossProductSum = 0.0f;

        // Because the cross-produce will have a different sign for triangles 
        // outside of the polygon this method will work for non-convex polygons
        for ( S32 i = 1; i < pShape->GetVertexCount() - 1; ++i )
        {
            b2Vec2 edge1 = pShape->GetVertex(i) - pShape->GetVertex(0);
            b2Vec2 edge2 = pShape->GetVertex(i+1) - pShape->GetVertex(0);
            crossProductSum += b2Cross(edge1, edge2);
        }

        crossProductSum *= 0.5f;

        // Return the area, which is the magnitude of the sum
        return fabs(crossProductSum);
    }

    // Return area of zero for any other shape type
    return 0.0f;
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, setCollisionShapeDensity, void, 4, 4,    "(int shapeIndex, float density) - Sets the density of the collision shape at the specified index.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@param density - The collision shape density."
                                                                        "@return No return value." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeDensity() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Fetch density.
    const F32 density = dAtof(argv[3]);
    if ( density < 0.0f )
    {
        Con::warnf("SceneObject::setCollisionShapeDensity() - Invalid density of %g.", density);
    }

    // Set density.
    object->setCollisionShapeDensity( shapeIndex, density );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getCollisionShapeDensity, F32, 3, 3, "(int shapeIndex) - Gets the density of the collision shape at the specified index.\n"
                                                                    "@param shapeIndex - The index of the collision shape."
                                                                    "@return The density of the collision shape at the specified index (-1 if error)." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeDensity() - Invalid shape index of %d.", shapeIndex);
        return -1.0f;
    }

    // Get density.
    return object->getCollisionShapeDensity( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, setCollisionShapeFriction, void, 4, 4,   "(int shapeIndex, float friction) - Sets the friction of the collision shape at the specified index.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@param friction - The collision shape friction."
                                                                        "@return No return value." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeFriction() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Fetch friction.
    const F32 friction = dAtof(argv[3]);
    if ( friction < 0.0f )
    {
        Con::warnf("SceneObject::setCollisionShapeFriction() - Invalid friction of %g.", friction);
    }

    // Set friction.
    object->setCollisionShapeFriction( shapeIndex, friction );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getCollisionShapeFriction, F32, 3, 3, "(int shapeIndex) - Gets the friction of the collision shape at the specified index.\n"
                                                                    "@param shapeIndex - The index of the collision shape."
                                                                    "@return The friction of the collision shape at the specified index (-1 if error)." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeFriction() - Invalid shape index of %d.", shapeIndex);
        return -1.0f;
    }

    // Get friction.
    return object->getCollisionShapeFriction( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, setCollisionShapeRestitution, void, 4, 4,    "(int shapeIndex, float restitution) - Sets the restitution of the collision shape at the specified index.\n"
                                                                            "@param shapeIndex - The index of the collision shape."
                                                                            "@param density - The collision shape restitution."
                                                                            "@return No return value." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeRestitution() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Fetch restitution.
    const F32 restitution = dAtof(argv[3]);
    if ( restitution < 0.0f )
    {
        Con::warnf("SceneObject::setCollisionShapeRestitution() - Invalid restitution of %g.", restitution);
    }

    // Set restitution.
    object->setCollisionShapeRestitution( shapeIndex, restitution );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getCollisionShapeRestitution, F32, 3, 3, "(int shapeIndex) - Gets the restitution of the collision shape at the specified index.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@return The restitution of the collision shape at the specified index (-1 if error)." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeRestitution() - Invalid shape index of %d.", shapeIndex);
        return -1.0f;
    }

    // Get restitution.
    return object->getCollisionShapeRestitution( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, setCollisionShapeIsSensor, void, 4, 4,   "(int shapeIndex, bool status ) - Sets whether the collision shape at the specified index is a sensor or not.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@param status - Whether the collision shape at the specified index is a sensor or not."
                                                                        "@return No return value." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeIsSensor() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Set is sensor.
    object->setCollisionShapeIsSensor( shapeIndex, dAtob(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getCollisionShapeIsSensor, bool, 3, 3,   "(int shapeIndex) - Gets whether the collision shape at the specified index is a sensor or not.\n"
                                                                        "@param shapeIndex - The index of the collision shape."
                                                                        "@return Whether the collision shape at the specified index is a sensor or not." )
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeIsSensor() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Get is sensor.
    return object->getCollisionShapeIsSensor( shapeIndex );
}

//-----------------------------------------------------------------------------
ConsoleMethod( SceneObject, createCircleCollisionShape, S32, 3, 5,   "(radius, [localPositionX, localPositionY]) Creates a circle collision shape.\n"
                                                                        "@param radius The radius of the circle."
                                                                        "@return (int shapeIndex) The index of the collision shape or (-1) if not created.")
{
    const F32 radius = dAtof(argv[2]);

    if ( argc == 3 )
    {
        return object->createCircleCollisionShape( radius );
    }

    b2Vec2 localPosition;

    // Fetch local position element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[3]);

    if ( elementCount == 2 && argc == 4 )
    {
        localPosition = Utility::mGetStringElementVector(argv[3]);
    }
    else if ( elementCount == 1 && argc == 5 )
    {
        localPosition.Set( dAtof(argv[3]), dAtof(argv[4]) );
    }
    else
    {
        Con::warnf("SceneObject::createCircleCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    return object->createCircleCollisionShape( radius, localPosition );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getCircleCollisionShapeRadius, F32, 3, 3,    "(int shapeIndex) - Gets the radius of a circle collision shape at the specified index.\n"
                                                                            "@param shapeIndex - The index of the collision shape."
                                                                            "@return (F32 radius) The radius of a circle collision shape at the specified index or (0) if an invalid shape." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeRadius() - Invalid shape index of %d.", shapeIndex);
        return 0.0f;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_circle )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeRadius() - Not a circle shape at index of %d.", shapeIndex);
        return 0.0f;
    }

    return object->getCircleCollisionShapeRadius( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getCircleCollisionShapeLocalPosition, const char*, 3, 3,     "(int shapeIndex) - Gets the local position of a circle collision shape at the specified index.\n"
                                                                                            "@param shapeIndex - The index of the collision shape."
                                                                                            "@return (localPositionXY) The local position of a circle collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeLocalPosition() - Invalid shape index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_circle )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeLocalPosition() - Not a circle shape at index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    return object->getCircleCollisionShapeLocalPosition( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, createPolygonCollisionShape, S32, 3, 3,  "(localPointsX/Y) Creates a convex polygon collision shape.\n"
                                                                        "@param localPointsX/Y A space separated list of local points for the convex polygon collision shape (must be at least three points)."
                                                                        "@return (int shapeIndex) The index of the collision shape or (-1) if not created.")
{
    const U32 pointElements = Utility::mGetStringElementCount(argv[2]);

    // Check even number of elements and at least 6 (3 points) exist.
    if ( ( pointElements % 2 ) != 0 || pointElements < 6 || pointElements > (2*b2_maxPolygonVertices) )
    {
        Con::warnf("SceneObject::createPolygonCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 localPoints[b2_maxPolygonVertices];
    for ( U32 elementIndex = 0, pointIndex = 0; elementIndex < pointElements; elementIndex += 2, pointIndex++ )
    {
        localPoints[pointIndex] = Utility::mGetStringElementVector( argv[2], elementIndex );
    }

    const U32 pointCount = pointElements / 2;
    return object->createPolygonCollisionShape( pointCount, localPoints );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, createPolygonBoxCollisionShape, S32, 2, 7,  "(width, height, [localCentroidX, localCentroidY], [angle]) Creates a polygon box collision shape.\n"
                                                                            "@param width The width of the box."
                                                                            "@param height The height of the box."
                                                                            "@param localCentroidX/Y The local position of the box centroid."
                                                                            "@param angle The angle of the box."
                                                                            "@return (int shapeIndex) The index of the collision shape or (-1) if not created.")
{
    // Were any dimensions specified?
    if( argc == 2 )
    {
        // No, so fetch the objects size.
        const Vector2& size = object->getSize();

        // Create a box surrounding the object.
        return object->createPolygonBoxCollisionShape( size.x, size.y );
    }

    // Width and height.
    const U32 widthHeightElementCount = Utility::mGetStringElementCount(argv[2]);

    F32 width;
    F32 height;

    S32 nextArg = 3;
    if ( widthHeightElementCount == 1 && argc > 3 )
    {
        width = dAtof(argv[2]);
        height = dAtof(argv[3]);
        nextArg = 4;
    }
    else if ( widthHeightElementCount == 2 )
    {
        Vector2 vector = Utility::mGetStringElementVector(argv[2]);
        width = vector.x;
        height = vector.y;
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createPolygonBoxCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createPolygonBoxCollisionShape( width, height );
    }

    // Local centroid.
    const U32 localCentroidElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localCentroid;
    if ( localCentroidElementCount == 1 && argc > (nextArg+1) )
    {
        localCentroid.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( localCentroidElementCount == 2 )
    {
        localCentroid = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createPolygonBoxCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createPolygonBoxCollisionShape( width, height, localCentroid );
    }

    // Angle.
    const F32 angle = mDegToRad(dAtof(argv[nextArg++]));
    
    return object->createPolygonBoxCollisionShape( width, height, localCentroid, angle );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getPolygonCollisionShapePointCount, S32, 3, 3,   "(int shapeIndex) - Gets the point count of a polygon collision shape at the specified index.\n"
                                                                                "@param shapeIndex - The index of the collision shape."
                                                                                "@return (int pointCount) The point count of a polygon collision shape at the specified index or (0) if an invalid shape." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapePointCount() - Invalid shape index of %d.", shapeIndex);
        return 0;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_polygon )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapePointCount() - Not a polygon shape at index of %d.", shapeIndex);
        return 0;
    }

    return object->getPolygonCollisionShapePointCount( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getPolygonCollisionShapeLocalPoint, const char*, 4, 4,   "(int shapeIndex, int pointIndex) - Gets the local point of a polygon collision shape at the specified index.\n"
                                                                                        "@param shapeIndex - The index of the collision shape."
                                                                                        "@param pointIndex - The index of the local point."
                                                                                        "@return (localPointXY) The local point of a polygon collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch point index.
    const U32 pointIndex = dAtoi(argv[3]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Invalid shape index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_polygon )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Not a polygon shape at index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Fetch point count.
    const U32 pointCount = object->getPolygonCollisionShapePointCount( shapeIndex );

    // Sanity!
    if ( pointIndex >= pointCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Invalid point index of %d (only %d available) on shape index of %d.", pointIndex, pointCount, shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    return object->getPolygonCollisionShapeLocalPoint( shapeIndex, pointIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, createChainCollisionShape, S32, 3, 7,    "(localPointsX/Y, [adjacentLocalPositionStartX, adjacentLocalPositionStartY], [adjacentLocalPositionEndX, adjacentLocalPositionEndY]) Creates a chain collision shape.\n"
                                                                        "@param localPointsX/Y A space separated list of local points for the chain collision shape (must be at least two points)."
                                                                        "@param adjacentLocalPositionStartXY The adjacent local position of the start of the edge."
                                                                        "@param adjacentLocalPositionEndXY The adjacent local position of the end of the edge."
                                                                        "@return (int shapeIndex) The index of the collision shape or (-1) if not created.")
{
    const U32 pointElements = Utility::mGetStringElementCount(argv[2]);

    // Check even number of elements and at least 4 (2 points) exist.
    if ( ( pointElements % 2 ) != 0 || pointElements < 4 )
    {
        Con::warnf("SceneObject::createChainCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 localPoints[b2_maxPolygonVertices];
    for ( U32 elementIndex = 0, pointIndex = 0; elementIndex < pointElements; elementIndex += 2, pointIndex++ )
    {
        localPoints[pointIndex] = Utility::mGetStringElementVector( argv[2], elementIndex );
    }

    const U32 pointCount = pointElements / 2;

    if ( argc == 3 )
    {
        return object->createChainCollisionShape( pointCount, localPoints );
    }

    S32 nextArg = 3;

    // Adjacent local position start.
    const U32 adjacentLocalPositionStartElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 adjacentLocalPositionStart;
    if ( adjacentLocalPositionStartElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionStart.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionStartElementCount == 2 )
    {
        adjacentLocalPositionStart = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createChainCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 adjacentLocalPositionEnd( 0.0f, 0.0f );

    if ( argc <= nextArg )
    {
        return object->createChainCollisionShape( pointCount, localPoints, true, false, adjacentLocalPositionStart, adjacentLocalPositionEnd );
    }

    // Adjacent local position end.
    const U32 adjacentLocalPositionEndElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    if ( adjacentLocalPositionEndElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionEnd.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionEndElementCount == 2 )
    {
        adjacentLocalPositionEnd = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createChainCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    return object->createChainCollisionShape( pointCount, localPoints, true, true, adjacentLocalPositionStart, adjacentLocalPositionEnd );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getChainCollisionShapePointCount, S32, 3, 3,     "(int shapeIndex) - Gets the point count of a chain collision shape at the specified index.\n"
                                                                                "@param shapeIndex - The index of the collision shape."
                                                                                "@return (int pointCount) The point count of a chain collision shape at the specified index or (0) if an invalid shape." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapePointCount() - Invalid shape index of %d.", shapeIndex);
        return 0;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapePointCount() - Not a chain shape at index of %d.", shapeIndex);
        return 0;
    }

    return object->getChainCollisionShapePointCount( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getChainCollisionShapeLocalPoint, const char*, 4, 4,   "(int shapeIndex, int pointIndex) - Gets the local point of a polygon collision shape at the specified index.\n"
                                                                                        "@param shapeIndex - The index of the collision shape."
                                                                                        "@param pointIndex - The index of the local point."
                                                                                        "@return (localPointXY) The local point of a polygon collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch point index.
    const U32 pointIndex = dAtoi(argv[3]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeLocalPoint() - Invalid shape index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeLocalPoint() - Not a chain shape at index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Fetch point count.
    const U32 pointCount = object->getPolygonCollisionShapePointCount( shapeIndex );

    // Sanity!
    if ( pointIndex >= pointCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Invalid point index of %d (only %d available) on shape index of %d.", pointIndex, pointCount, shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    return object->getChainCollisionShapeLocalPoint( shapeIndex, pointIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getChainCollisionShapeHasAdjacentStart, bool, 3, 3,  "(int shapeIndex) - Gets whether the chain collision shape at the specified index has an adjacent start point or not.\n"
                                                                                    "@param shapeIndex - The index of the collision shape."
                                                                                    "@return (bool adjacentStart) Whether the chain collision shape at the specified index has an adjacent start point or not." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentStart() - Not a chain shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getChainCollisionShapeHasAdjacentStart( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getChainCollisionShapeHasAdjacentEnd, bool, 3, 3,    "(int shapeIndex) - Gets whether the chain collision shape at the specified index has an adjacent end point or not.\n"
                                                                                    "@param shapeIndex - The index of the collision shape."
                                                                                    "@return (bool adjacentEnd) Whether the chain collision shape at the specified index has an adjacent end point or not." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentEnd() - Not a chain shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getChainCollisionShapeHasAdjacentEnd( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getChainCollisionShapeAdjacentStart, const char*, 3, 3,  "(int shapeIndex) - Gets the adjacent start point of the chain collision shape at the specified index.\n"
                                                                                        "@param shapeIndex - The index of the collision shape."
                                                                                        "@return (adjacentStartPointXY) The adjacent start point of the chain collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentStart() - Not a chain shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getChainCollisionShapeAdjacentStart( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getChainCollisionShapeAdjacentEnd, const char*, 3, 3,    "(int shapeIndex) - Gets whether the chain collision shape at the specified index has an adjacent end point or not.\n"
                                                                                        "@param shapeIndex - The index of the collision shape."
                                                                                        "@return (bool adjacentEnd) Whether the chain collision shape at the specified index has an adjacent end point or not." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentEnd() - Not a chain shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getChainCollisionShapeAdjacentEnd( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, createEdgeCollisionShape, S32, 4, 10, "(localPositionStartX, localPositionStartY, localPositionEndX, localPositionEndY, [adjacentLocalPositionStartX, adjacentLocalPositionStartY], [adjacentLocalPositionEndX, adjacentLocalPositionEndY]) - Creates an edge collision shape.\n"
                                                                    "@param localPositionStartXY The local position of the start of the edge."
                                                                    "@param localPositionEndXY The local position of the end of the edge."
                                                                    "@param adjacentLocalPositionStartXY The adjacent local position of the start of the edge."
                                                                    "@param adjacentLocalPositionEndXY The adjacent local position of the end of the edge."
                                                                    "@return (int shapeIndex) The index of the collision shape or (-1) if not created.")
{
    // Local position start.
    const U32 localPositionStartElementCount = Utility::mGetStringElementCount(argv[2]);

    b2Vec2 localPositionStart;
    S32 nextArg = 3;
    if ( localPositionStartElementCount == 1 && argc > 3 )
    {
        localPositionStart.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( localPositionStartElementCount == 2 )
    {
        localPositionStart = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    // Local position end.
    const U32 localPositionEndElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localPositionEnd;
    if ( localPositionEndElementCount == 1 && argc > (nextArg+1) )
    {
        localPositionEnd.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( localPositionEndElementCount == 2 )
    {
        localPositionEnd = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createEdgeCollisionShape( localPositionStart, localPositionEnd );
    }

    // Adjacent local position start.
    const U32 adjacentLocalPositionStartElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 adjacentLocalPositionStart;
    if ( adjacentLocalPositionStartElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionStart.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionStartElementCount == 2 )
    {
        adjacentLocalPositionStart = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 adjacentLocalPositionEnd(0.0f, 0.0f);

    if ( argc <= nextArg )
    {
        return object->createEdgeCollisionShape( localPositionStart, localPositionEnd, true, false, adjacentLocalPositionStart, adjacentLocalPositionEnd );
    }

    // Adjacent local position end.
    const U32 adjacentLocalPositionEndElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    if ( adjacentLocalPositionEndElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionEnd.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionEndElementCount == 2 )
    {
        adjacentLocalPositionEnd = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    return object->createEdgeCollisionShape( localPositionStart, localPositionEnd, true, true, adjacentLocalPositionStart, adjacentLocalPositionEnd );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getEdgeCollisionShapeLocalPositionStart, const char*, 3, 3,  "(int shapeIndex) - Gets the local position start of the edge collision shape at the specified index.\n"
                                                                                            "@param shapeIndex - The index of the collision shape."
                                                                                            "@return (localPositionStartXY) The local position start of the edge collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionStart() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeLocalPositionStart( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getEdgeCollisionShapeLocalPositionEnd, const char*, 3, 3,    "(int shapeIndex) - Gets the local position end of the edge collision shape at the specified index.\n"
                                                                                            "@param shapeIndex - The index of the collision shape."
                                                                                            "@return (localPositionEndXY) The local position end of the edge collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionEnd() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeLocalPositionEnd( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getEdgeCollisionShapeHasAdjacentStart, bool, 3, 3,  "(int shapeIndex) - Gets whether the chain collision shape at the specified index has an adjacent start point or not.\n"
                                                                                    "@param shapeIndex - The index of the collision shape."
                                                                                    "@return (bool adjacentStart) Whether the chain collision shape at the specified index has an adjacent start point or not." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentStart() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeHasAdjacentStart( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getEdgeCollisionShapeHasAdjacentEnd, bool, 3, 3,    "(int shapeIndex) - Gets whether the edge collision shape at the specified index has an adjacent end point or not.\n"
                                                                                    "@param shapeIndex - The index of the collision shape."
                                                                                    "@return (bool adjacentEnd) Whether the edge collision shape at the specified index has an adjacent end point or not." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentEnd() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeHasAdjacentEnd( shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getEdgeCollisionShapeAdjacentStart, const char*, 3, 3,  "(int shapeIndex) - Gets the adjacent start point of the edge collision shape at the specified index.\n"
                                                                                        "@param shapeIndex - The index of the collision shape."
                                                                                        "@return (adjacentStartPointXY) The adjacent start point of the edge collision shape at the specified index." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentStart() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeAdjacentStart( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod( SceneObject, getEdgeCollisionShapeAdjacentEnd, const char*, 3, 3,    "(int shapeIndex) - Gets whether the edge collision shape at the specified index has an adjacent end point or not.\n"
                                                                                        "@param shapeIndex - The index of the collision shape."
                                                                                        "@return (bool adjacentEnd) Whether the edge collision shape at the specified index has an adjacent end point or not." )
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentEnd() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeAdjacentEnd( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, copyAllCollisionShapes, void, 3, 4,  "(targetObject [, clearTargetShapes?] ) - Copies all collision shapes to the target object.\n"
                                                                "@param targetObject - The target object to receive the collision shapes.\n"
                                                                "@param clearTargetShapes - Whether to clear the current collision shapes on the target or not.  Optional: Defaults to true.\n"
                                                                "@return No return value.")
{
    // Fetch target object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>( argv[2] );

    // Sanity!
    if ( pSceneObject == NULL )
    {
        Con::warnf( "SceneObject::copyAllCollisionShapes() - Invalid target object." );
        return;
    }

    // Fetch clear target shapes flag.
    const bool clearTargetShapes = argc >= 4 ? dAtob( argv[3] ) : true;
    
    // Copy collision shapes.
    object->copyCollisionShapes( pSceneObject, clearTargetShapes );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, copyCollisionShape, S32, 4, 4,   "(int shapeIndex, targetObject) - Copies a collision shape at the specified index to the target object.\n"
                                                            "@param shapeIndex - The index of the collision shape.\n"
                                                            "@param targetObject - The target object to receive the collision shape copy.\n"
                                                            "@return The shape index of the copied collision shape on the target object or (-1) if not copied.")
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf( "SceneObject::copyCollisionShape() - Invalid shape index of %d.", shapeIndex );
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Fetch target object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>( argv[3] );

    // Sanity!
    if ( pSceneObject == NULL )
    {
        Con::warnf( "SceneObject::copyCollisionShape() - Invalid target object." );
        return INVALID_COLLISION_SHAPE_INDEX;
    }
    
    // Copy collision shape.
    return object->copyCollisionShapes( pSceneObject, false, shapeIndex );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setVisible, void, 3, 3, "(bool status) - Show or hide the object.\n"
                                                      "@param status Whether to enable or disable visibility on the object."
                                                      "@return No return value.")
{
    // Set Visible.
    object->setVisible( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getVisible, bool, 2, 2, "() - Gets the object's visible status.\n"
                                                      "@return (bool status) Whether or not the object is visible.")
{
    // Get Visible Status.
    return object->getVisible();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setBlendMode, void, 3, 3,    "(bool blendMode) - Sets whether blending is on or not.\n"
                                                        "@blendMode Whether blending is on or not.\n"
                                                        "@return No return Value.")
{
    // Fetch blend mode.
    const bool blendMode = dAtob(argv[2]);

    object->setBlendMode( blendMode );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getBlendMode, bool, 2, 2,    "() - Gets whether blending is on or not.\n"
                                                        "@return (bool blendMode) Whether blending is on or not.")
{
   return object->getBlendMode();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSrcBlendFactor, void, 3, 3,   "(srcBlend) - Sets the source blend factor.\n"
                                                            "@param srcBlend The source blend factor.\n"
                                                            "@return No return Value.")
{
    // Fetch source blend factor.
    GLenum blendFactor = SceneObject::getSrcBlendFactorEnum(argv[2]);

    object->setSrcBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSrcBlendFactor, const char*, 2, 2, "() - Gets the source blend factor.\n"
                                                                    "@return (srcBlend) The source blend factor.")
{
   return SceneObject::getSrcBlendFactorDescription(object->getSrcBlendFactor());
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setDstBlendFactor, void, 3, 3,   "(dstBlend) - Sets the destination blend factor.\n"
                                                            "@param dstBlend The destination blend factor.\n"
                                                            "@return No return Value.")
{
    // Fetch destination blend factor.
    GLenum blendFactor = SceneObject::getDstBlendFactorEnum(argv[2]);

    object->setDstBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getDstBlendFactor, const char*, 2, 2, "() - Gets the destination blend factor.\n"
                                                                    "@return (dstBlend) The destination blend factor.")
{
   return SceneObject::getDstBlendFactorDescription(object->getDstBlendFactor());
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setBlendColor, void, 3, 6,   "(float red, float green, float blue, [float alpha = 1.0]) or ( stockColorName ) - Sets the blend color."
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
    object->setBlendColor(ColorF(red, green, blue, alpha));
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getBlendColor, const char*, 2, 3,    "(allowColorNames) Gets the Rendering Blend color.\n"
                                                                "@param allowColorNames Whether to allow stock color names to be returned or not.  Optional: Defaults to false.\n"
                                                                "@return (float red / float green / float blue / float alpha) The sprite blend color.")
{
    // Get Blend color.
    ColorF blendColor = object->getBlendColor();

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

ConsoleMethod(SceneObject, setBlendAlpha, void, 3, 3,    "(float alpha) - Sets the Rendering Alpha (transparency).\n"
                                                            "The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent.\n"
                                                            "@param alpha The alpha value.\n"
                                                            "@return No return Value.")
{
    // Set Blend Alpha.
    object->setBlendAlpha( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getBlendAlpha, F32, 2, 2,     "() - Gets the Rendering Alpha (transparency).\n"
                                                            "@return (float alpha) The alpha value, a range from 0.0 to 1.0.  Less than zero if alpha testing is disabled.")
{
    // Get Blend Alpha.
    return object->getBlendAlpha();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setAlphaTest, void, 3, 3,    "(float alpha) - Set the render alpha test threshold.\n"
                                                        "@param alpha The alpha test threshold in the range of 0.0 to 1.0.  Less than zero to disable alpha testing.\n"
                                                        "@return No return value." )

{
    object->setAlphaTest(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getAlphaTest, F32, 2, 2,  "() - Gets the render alpha test threshold.\n"
                                                        "@return The render alpha test threshold in the range of 0.0f to 1.0.  Less than zero represents disabled alpha testing.")
{
    return object->getAlphaTest();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSortPoint, void, 3, 4, "(float x, float y) Sets the layer draw order sorting point.\n"
                                                        "@param x The x position local to the object of the sort point.\n"
                                                        "@param y The y position local to the object of the sort point.\n"
                                                        "@return No return value.")
{
   // The new sort point.
   Vector2 sortPt;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      sortPt = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      sortPt = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::setSortPoint() - Invalid number of parameters!");
      return;
   }

   // Set Position.
   object->setSortPoint(sortPt);
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSortPoint, const char*, 2, 2, "() Gets the layer draw order sorting point.\n"
                                                               "@return (float x/float y) The local x and y position of the sort point.")
{
    // Get sort point.
    return object->getSortPoint().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSortPointX, void, 3, 3, "(float x) Sets the x-component of the layer draw order sorting point.\n"
                                                         "@param x The x position local to the object of the sort point.\n"
                                                        "@return No return value.")
{
    // Set sort point X-component.
    object->setSortPoint( Vector2( dAtof(argv[2]), object->getSortPoint().y ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSortPointX, F32, 2, 2, "() Gets the x component of the layer draw order sorting point.\n"
                                                        "@return (float x) The local x position of the sort point.")
{
    // Get sort point X-component.
    return object->getSortPoint().x;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSortPointY, void, 3, 3, "(float y) Sets the y-component of the layer draw order sorting point.\n"
                                                         "@param y The y position local to the object of the sort point.\n"
                                                         "@return No return value.")
{
    // Set sort point Y-component.
    object->setSortPoint( Vector2( object->getSortPoint().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSortPointY, F32, 2, 2, "() Gets the y component of the layer draw order sorting point.\n"
                                                        "@return (float y) The local y position of the sort point.")
{
    // Get sort point Y-component.
    return object->getSortPoint().y;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setRenderGroup, void, 3, 3,  "(renderGroup) Sets the name of the render group used to sort the object during rendering.\n"
                                                        "@param renderGroup The name of the render group to use.  Defaults to nothing.\n"
                                                        "@return No return value.")
{
    object->setRenderGroup( argv[2] );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getRenderGroup, const char*, 2, 2,   "() Gets the name of the render group used to sort the object during rendering.\n"
                                                                "@return The render group used to sort the object during rendering.")
{
    return object->getRenderGroup();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setUseInputEvents, void, 3, 3, "(bool inputStatus) - Sets whether input events are passed from the scene window are monitored by this object or not.\n"
                                                             "@param inputStatus Whether input events are passed from the scene window are monitored by this object or not.\n"
                                                                 "@return No return Value.")
{
    object->setUseInputEvents( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getUseInputEvents, bool, 2, 2, "() - Gets whether input events are passed from the scene window are monitored by this object or not.\n"
                                                             "@return (bool inputStatus) Whether input events are passed from the scene window are monitored by this object or not.")
{
    return object->getUseInputEvents();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setUpdateCallback, void, 2, 3,    "([bool status?]) - Sets whether the 'onUpdate' callback is called or not.\n"
                                                                "@param status Whether the 'onUpdate' callback is called or not (default is false).\n"
                                                                "@return No return Value.")
{
   object->setUpdateCallback( argc > 2 ? dAtob(argv[2]) : true);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getUpdateCallback, bool, 2, 2,    "() - Gets whether the 'onUpdate' callback is called or not.\n"
                                                                "@return (bool status ) Whether the 'onUpdate' callback is called or not.")
{
   return object->getUpdateCallback();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setSleepingCallback, void, 2, 3,  "([bool status?]) - Sets whether the 'onWake' and 'onSleep' callbacks are called or not when the object changes sleep state.\n"
                                                                "The object must be able to sleep (setSleepingAllowed) and have a 'dynamic' body mode for this callback to occur.\n"
                                                                "@param status Whether the 'onWake' and 'onSleep' callbacks are called or not (default is false).\n"
                                                                "@return No return Value.")
{
   object->setSleepingCallback( argc > 2 ? dAtob(argv[2]) : true);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getSleepingCallback, bool, 2, 2,    "() - Gets whether the 'onWake' and 'onSleep' callbacks are called or not when the object changes sleep state.\n"
                                                                "@return (bool status ) Whether the 'onWake' and 'onSleep' callbacks are called or not.")
{
   return object->getSleepingCallback();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setCollisionCallback, void, 2, 3,     "([bool status?]) - Sets whether the 'Scene::onCollision' callback is called or not.\n"
                                                                    "Sets whether whether the 'Scene::onCollision' callback is called or not.\n"
                                                                    "@param status Whether the 'Scene::onCollision' callback is called or not (default is false).\n"
                                                                    "@return No return Value.")
{
    // Set collision callback.
    object->setCollisionCallback( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getCollisionCallback, bool, 2, 2,  "() Gets whether the 'Scene::onCollision' callback is called or not.\n"
                                                                    "@return (bool status) the 'Scene::onCollision' callback is called or not.")
{
    // Get collision callback.
    return object->getCollisionCallback();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setDebugOn, void, 3, 2 + DEBUG_MODE_COUNT,   "(debugOptions) Sets Debug option(s) on.\n"
                                                                        "@param debugOptions Either a list of debug modes (comma-separated), or a string with the modes (space-separated)\n"
                                                                        "@return No return value.")
{
    // Reset the mask.
    U32 mask = 0;

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        Con::warnf( "SceneObject::setDebugOn() - Invalid number of parameters!" );
        return;
    }

    // Space-separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = Utility::mGetStringElement( argv[2], i );
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( pDebugOption );
        
            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOn() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }
         
            // Merge into mask.
            mask |= debugOption;
        }
    }
    // Comma-separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = argv[i];
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( argv[i] );

            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOn() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }

            // Merge into mask.
            mask |= debugOption;
        }
    }

    // Set debug mask.
    object->setDebugOn(mask);
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setDebugOff, void, 3, 2 + DEBUG_MODE_COUNT,  "(debugOptions) Sets Debug options(s) off.\n"
                                                                        "@param debugOptions Either a list of debug modes to turn off (comma-separated) or a string (space-separated)\n"
                                                                        "@return No return value.")
{
    // Reset the mask.
    U32 mask = 0;

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        Con::warnf( "SceneObject::setDebugOff() - Invalid number of parameters!" );
        return;
    }

    // Space-separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = Utility::mGetStringElement( argv[2], i );
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( pDebugOption );
        
            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOff() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }
         
            // Merge into mask.
            mask |= debugOption;
        }
    }
    // Comma-separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = argv[i];
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( argv[i] );

            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOff() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }

            // Merge into mask.
            mask |= debugOption;
        }
    }

    // Set debug mask.
    object->setDebugOff(mask);
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, attachGui, void, 4, 5, "(guiControl guiObject, SceneWindow window, [sizeControl? = false]) - Attach a GUI Control to the object.\n"
                                                     "@param guiObject The GuiControl to attach.\n"
                                                     "@param window The SceneWindow to bind the GuiControl to.\n"
                                                     "@param sizeControl Whether or not to size the GuiControl to the size of this object.\n"
                                                                 "@return No return Value.")
{
    // Find GuiControl Object.
    GuiControl* pGuiControl = dynamic_cast<GuiControl*>(Sim::findObject(argv[2]));

    // Check for invalid Gui Control.
    if ( !pGuiControl )
    {
        Con::warnf("SceneObject::attachGui() - Couldn't find GuiControl %s!", argv[2]);
        return;
    }

    // Find SceneWindow Object.
    SceneWindow* pSceneWindow = dynamic_cast<SceneWindow*>(Sim::findObject(argv[3]));

    // Check for invalid SceneWindow Object.
    if ( !pSceneWindow )
    {
        Con::warnf("SceneObject::attachGui() - Couldn't find SceneWindow %s!", argv[3]);
        return;
    }

    // Calculate Send to Mount.
    const bool sizeControl = argc >= 5 ? dAtob(argv[4]) : false;

    // Attach GUI Control.
    object->attachGui( pGuiControl, pSceneWindow, sizeControl );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, detachGui, void, 2, 2, "() - Detach any GUI Control.\n"
                                                                 "@return No return Value.")
{
    // Detach GUI Control.
    object->detachGui();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, copyFrom, bool, 3, 4, "(SceneObject object, [copyDynamicFields? = false]) - Copies one scene object from another scene object.\n"
                                                    "The object being copied to needs to be of the same class as the object being copied from.\n"
                                                    "@param object The SceneObject to copy this object to.\n"
                                                    "@param copyDynamicFields Whether the dynamic fields should be copied or not.  Optional: Defaults to false.\n"
                                                    "@return (bool) Whether or not the copy was successful.")
{
    // Find scene object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( Sim::findObject(argv[2]) );

    // Did we find the object?
    if ( !pSceneObject )
    {
        // No, so warn.
        Con::warnf("SceneObject::copyFrom() - Could not find object '%s'.", argv[2] );
        return false;
    }

    // Fetch copy dynamic fields flag.
    const bool copyDynamicFields = ( argc >= 4 ) ? dAtob( argv[3] ) : false;

    // Copy object.
    object->copyFrom( pSceneObject, copyDynamicFields );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, setPickingAllowed, void, 3, 3,   "(bool pickingAllowed) - Sets whether picking is allowed or not.\n"
                                                            "@param pickingAllowed Whether picking is allowed or not.\n"
                                                            "@return No return Value.")
{
    // Fetch flag.
    const bool pickingAllowed = dAtob(argv[2]);

    object->setPickingAllowed( pickingAllowed );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, getPickingAllowed, bool, 2, 2,   "() - Gets whether picking is allowed or not.\n"
                                                            "@return Whether picking is allowed or not.")
{
    return object->getPickingAllowed();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneObject, safeDelete, void, 2, 2, "() - Safely deletes object.\n"
                                                                 "@return No return Value.")
{
    // Script Delete.
    object->safeDelete();
}

