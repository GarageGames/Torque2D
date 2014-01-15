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


ConsoleMethod(SceneWindow, getWindowExtents, const char*, 2, 2, "() Fetch Window Extents (Position/Size)."
              "@return Returns the window dimensions as a string formatted as follows: <position.x> <position.y> <width> <height>")
{
    // Get Size Argument Buffer.
    char* pExtentsBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf( pExtentsBuffer, 64, "%d %d %d %d", object->getPosition().x, object->getPosition().y, object->getExtent().x, object->getExtent().y );

    // Return Buffer.
    return pExtentsBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getScene, const char*, 2, 2, "() - Returns the Scene associated with this window."
              "@return Returns the scene ID as a string")
{
   Scene* pScene = object->getScene();

   char* id = Con::getReturnBuffer(8);
   if (pScene)
   {
        dItoa(pScene->getId(), id );
   }
   else
   {
      id[0] = '\0';
   }

   return id;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setScene, void, 2, 3, "(Scene) Associates Scene Object."
              "@param Scene The scene ID or name.\n"
              "@return No return value.")
{
    // No scene specified?
    if ( argc < 3 )
    {
        // No, so reset the scene.
        object->resetScene();
        // Finish here.
        return;
    }

    // Find Scene Object.
    Scene* pScene = (Scene*)(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pScene )
    {
        Con::warnf("SceneWindow::setScene() - Couldn't find object '%s'.", argv[2]);
        return;
    }

    // Set Scene.
    object->setScene( pScene );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, resetScene, void, 2, 2, "() Detaches the window from any Scene Object.\n"
              "@return No return value")
{
    // Reset Scene.
    object->resetScene();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraPosition, void, 3, 4,    "(x , y) - Set the current camera position.\n"
                                                                    "@param X Position along the X axis.\n"
                                                                    "@param Y Position along the Y axis.\n"
                                                                    "@return No return value.")
{
   if ( argc == 3 )
   {
       object->setCameraPosition( Vector2(argv[2]) );
       return;
   }

   object->setCameraPosition( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraPosition, const char*, 2, 2, "() Get the current camera position.\n"
                                                                        "@return The current camera position.")
{
    return object->getCameraPosition().scriptThis();
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraSize, void, 3, 4,    "(width , height) - Set the current camera position.\n"
                                                                "@param width Size along the X axis.\n"
                                                                "@param height Size along the Y axis.\n"
                                                                "@return No return value.")
{
   if ( argc == 3 )
   {
       object->setCameraSize( Vector2(argv[2]) );
       return;
   }

   object->setCameraSize( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraSize, const char*, 2, 2, "() Get the current camera size.\n"
                                                                    "@return The current camera width and height.")
{
    return object->getCameraSize().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraArea, void, 3, 6, "(x1 / y1 / x2 / y2) - Set the current camera area."
              "@param x1,y1,x2,y2 The coordinates of the minimum and maximum points (top left, bottom right)\n"
              "The input can be formatted as either \"x1 y1 x2 y2\", \"x1 y1, x2 y2\", \"x1, y1, x2, y2\"\n"
              "@return No return value.")
{
   // Upper left bound.
   Vector2 v1;
   // Lower right bound.
   Vector2 v2;

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
      Con::warnf("SceneWindow::setCameraArea() - Invalid number of parameters!");
      return;
   }

    // Calculate Normalised Rectangle.
    Vector2 topLeft( (v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y );
    Vector2 bottomRight( (v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y );

    // Set Current Camera Area.
    object->setCameraArea( RectF(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraArea, const char*, 2, 2, "() Get the current camera Area.\n"
              "@return The camera area formatted as \"x1 y1 x2 y2\"")
{
    // Fetch Camera Window.
    const RectF cameraWindow = object->getCameraArea();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf(pBuffer, 64, "%.5g %.5g %.5g %.5g", cameraWindow.point.x, cameraWindow.point.y, cameraWindow.point.x+cameraWindow.extent.x, cameraWindow.point.y+cameraWindow.extent.y);

    // Return Buffer.
    return pBuffer;
}     

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraZoom, void, 3, 3,    "(zoomFactor) - Set the current camera Zoom Factor.\n"
                                                                "@param zoomFactor A float value representing the zoom factor\n"
                                                                "@return No return value.")
{
    object->setCameraZoom( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraZoom, F32, 2, 2, "() Get the current camera Zoom.\n"
                                                            "@return The current camera zoom.")
{
    return object->getCameraZoom();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraAngle, void, 3, 3, "(angle) - Sets the current camera angle.\n"
                                                              "@param angle The current camera angle in degrees.\n"
                                                              "@return No return value.")
{
    object->setCameraAngle( mDegToRad(dAtof(argv[2])) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraAngle, F32, 2, 2,    "() Gets the current camera angle.\n"
                                                                "@return The current camera angle in degrees.")
{
    return object->getCameraAngle();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraWorldScale, const char*, 2, 2, "() Get current camera scale to world.\n"
              "@return Returns the cameras window width/height scale to world as a string formatted as \"widthScale heightScale\"")
{
    // Fetch camera window
    const Vector2 cameraWindowScale = object->getCameraWindowScale();

    return cameraWindowScale.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraRenderPosition, const char*, 2, 2,   "() Get current camera position post-view-limit clamping.\n"
                                                                                "@return The current camera render position.")
{
    return object->getCameraRenderPosition().scriptThis();
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCameraRenderScale, const char*, 2, 2, "() Get current camera scale to render.\n"
              "@return Returns the cameras window width/height scale to render as a string formatted as \"widthScale heightScale\"")
{
    // Fetch camera window scale.
    Vector2 cameraWindowScale = object->getCameraWindowScale();

    // Inverse scale.
    cameraWindowScale.receiprocate();

    return cameraWindowScale.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setTargetCameraPosition, void, 3, 4,     "(x , y) - Set the target camera position.\n"
                                                                    "@param X Position along the X axis.\n"
                                                                    "@param Y Position along the Y axis.\n"
                                                                    "@return No return value.")
{
   if ( argc == 3 )
   {
       object->setTargetCameraPosition( Vector2(argv[2]) );
       return;
   }

   object->setTargetCameraPosition( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}


//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getTargetCameraPosition, const char*, 2, 2,  "() Get the target camera position.\n"
                                                                        "@return The target camera position.")
{
    return object->getTargetCameraPosition().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setTargetCameraSize, void, 3, 4,    "(width , height) - Set the target camera position.\n"
                                                                "@param width Size along the X axis.\n"
                                                                "@param height Size along the Y axis.\n"
                                                                "@return No return value.")
{
   if ( argc == 3 )
   {
       object->setTargetCameraSize( Vector2(argv[2]) );
       return;
   }

   object->setTargetCameraSize( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getTargetCameraSize, const char*, 2, 2, "() Get the target camera size.\n"
                                                                    "@return The target camera width and height.")
{
    return object->getTargetCameraSize().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setTargetCameraArea, void, 3, 6, "(x / y / width / height) - Set the target camera area."
              "@return No return value.")
{
   // Upper left bound.
   Vector2 v1;
   // Lower right bound.
   Vector2 v2;

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
      Con::warnf("SceneWindow::setTargetCameraArea() - Invalid number of parameters!");
      return;
   }

    // Calculate Normalised Rectangle.
    Vector2 topLeft( (v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y );
    Vector2 bottomRight( (v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y );

    // Set Target Camera Area.
    object->setTargetCameraArea( RectF(topLeft.x, topLeft.y, bottomRight.x-topLeft.x+1, bottomRight.y-topLeft.y+1) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getTargetCameraArea, const char*, 2, 2, "() Get the target camera Area.\n"
              "@return The camera area formatted as \"x1 y1 x2 y2\"")
{
    // Fetch Camera Window.
    const RectF cameraWindow = object->getTargetCameraArea();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf(pBuffer, 64, "%.5g %.5g %.5g %.5g", cameraWindow.point.x, cameraWindow.point.y, cameraWindow.point.x+cameraWindow.extent.x, cameraWindow.point.y+cameraWindow.extent.y);

    // Return Buffer.
    return pBuffer;
} 

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setTargetCameraZoom, void, 3, 3, "(zoomFactor) - Set the target camera Zoom Factor."
                                                            "@return No return value.")
{
    object->setTargetCameraZoom( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getTargetCameraZoom, F32, 2, 2, "() Get the target camera Zoom.\n"
                                                            "@return The current camera zoom.")
{
    return object->getTargetCameraZoom();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setTargetCameraAngle, void, 3, 3,    "(angle) - Sets the target camera angle.\n"
                                                                "@param angle The target camera angle in degrees.\n"
                                                              " @return No return value.")
{
    object->setTargetCameraAngle( mDegToRad(dAtof(argv[2])) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getTargetCameraAngle, F32, 2, 2,    "() Gets the target camera angle.\n"
                                                                "@return The target camera angle in degrees.")
{
    return object->getTargetCameraAngle();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraInterpolationTime, void, 3, 3, "(interpolationTime) - Set camera interpolation time."
              "@return No return value")
{
    // Set Camera Interpolation Time.
    object->setCameraInterpolationTime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setCameraInterpolationMode, void, 3, 3, "(interpolationMode) - Set camera interpolation mode."
              "@return No return value.")
{
    // Set Camera Interpolation Mode.
    object->setCameraInterpolationMode( SceneWindow::getInterpolationModeEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, startCameraMove, void, 2, 3, "([interpolationTime]) - Start Camera Move."
              "@return No return value.")
{
    F32 interpolationTime;

    // Interpolation Time?
    if ( argc >= 3 )
        interpolationTime = dAtof(argv[2]);
    else
        interpolationTime = object->getCameraInterpolationTime();

    // Start Camera Move.
    object->startCameraMove( interpolationTime );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, stopCameraMove, void, 2, 2, "() Stops current camera movement"
              "@return No return value.")
{
    // Stop Camera Move.
    object->stopCameraMove();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, completeCameraMove, void, 2, 2, "() Moves camera directly to target.\n"
              "@return No return value.")
{
    // Complete Camera Move.
    object->completeCameraMove();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, undoCameraMove, void, 2, 3, "([interpolationTime]) - Reverses previous camera movement."
              "@return No return value.")
{
    F32 interpolationTime;

    // Interpolation Time?
    if ( argc >= 3 )
        interpolationTime = dAtof(argv[2]);
    else
        interpolationTime = object->getCameraInterpolationTime();

    // Undo Camera Move.
    object->undoCameraMove( interpolationTime );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getIsCameraMoving, bool, 2, 2, "() Check the camera moving status.\n"
              "@return Returns a boolean value as to whether or not the camera is moving.")
{
    // Is Camera Moving?
    return object->isCameraMoving();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getIsCameraMounted, bool, 2, 2, "() Check the camera mounted status.\n"
              "@return Returns a boolean value as to whether or not the camera is mounted.")
{
    // Is Camera Mounted.
    return object->isCameraMounted();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, startCameraShake, void, 4, 4, "(shakeMagnitude, time) - Starts the camera shaking."
              "@param shakeMagnitude The intensity of the shaking\n"
              "@param time The length of the shake"
              "@return No return value")
{
    // Start Camera Shake.
    object->startCameraShake( dAtof(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, stopCameraShake, void, 2, 2, "() Stops the camera shaking."
              "@return No return value")
{
    // Stop Camera Shake.
    object->stopCameraShake();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, mount, void, 3, 8,   "(sceneObject, [offsetX / offsetY], [mountForce], [sendToMount?], [mountAngle?]) - Mounts the camera onto the specified object."
                                                "@param sceneObject The scene object to mount the camera to.\n"
                                                "@param offsetX / offsetY The offset from the objects position to mount the camera to.  Optional: Defaults to no offset.\n"
                                                "@param mountForce The force to use to keep the camera mounted to the object.  Zero is a rigid mount.  Optional: Defaults to zero.\n"
                                                "@param sendToMount Whether to immediately move the camera to the objects position or not.  Optional: Defaults to true.\n"
                                                "@param mountAngle Whether to mount the cameras angle to the objects angle or not.  Optional: Defaults to false.\n"
                                                "@return No return value")
{
    // Grab the object. Always specified.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if (!pSceneObject)
    {
        Con::warnf("SceneWindow::mount() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Set defaults.
    Vector2 mountOffset(0.0f, 0.0f);
    F32 mountForce = 0.0f;
    bool sendToMount = true;
    bool mountAngle = false;
    
    U32 nextArg = 3;
    if ( (U32)argc > nextArg )
    {
        // Fetch Element Count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[nextArg]);

        // (object, "offsetX offsetY", ...)
        if ( elementCount == 2 )
        {
            mountOffset = Utility::mGetStringElementVector(argv[nextArg++]);
        }
        // (object, offsetX, offsetY, ...)
        else if ( elementCount == 1 && (U32)argc >= nextArg+1 )
        {
            mountOffset = Vector2(dAtof(argv[nextArg]), dAtof(argv[nextArg+1]));
            nextArg += 2;
        }
        // Invalid.
        else
        {
            Con::warnf("SceneWindow::mount() - Invalid number of parameters!");
            return;
        }
    }

    // Grab the mount force - if it's specified.
    if ( (U32)argc > nextArg )
        mountForce = dAtof(argv[nextArg++]);

    // Grab the send to mount flag.

    if ( (U32)argc > nextArg )
        sendToMount = dAtob(argv[nextArg++]);


    if ( (U32)argc > nextArg )
        mountAngle = dAtob(argv[nextArg++]);

    // Mount Object.
    object->mount( pSceneObject, mountOffset, mountForce, sendToMount, mountAngle );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, dismount, void, 2, 2, "() Dismounts Camera from object."
              "@return No return value")
{
    // Dismount Object.
    object->dismount();
}

//-----------------------------------------------------------------------------

void SceneWindow::dismountMe( SceneObject* pSceneObject )
{
    // Are we mounted to the specified object?
    if ( isCameraMounted() && pSceneObject != mpMountedTo )
    {
        // No, so warn.
        Con::warnf("SceneWindow::dismountMe() - Object is not mounted by the camera!");
        return;
    }

    // Dismount Object.
    dismount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setViewLimitOn, void, 3, 6, "([minX / minY / maxX / maxY]) - Set View Limit On."
              "@return No return value")
{
   // Upper left bound.
   Vector2 v1;
   // Lower right bound.
   Vector2 v2;

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
      Con::warnf("SceneWindow::setViewLimitOn() - Invalid number of parameters!");
      return;
   }

   // Calculate Normalised Rectangle.
   Vector2 topLeft((v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y);
   Vector2 bottomRight((v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y);

    // Set the View Limit On.
    object->setViewLimitOn(topLeft, bottomRight);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setViewLimitOff, void, 2, 2, "() Set View Limit Off."
              "@return No return value")
{
    // Set View Limit Off.
    object->setViewLimitOff();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, clampCameraViewLimit, void, 2, 2, "() Clamps the current camera to the current view limit.\n"
                                                                    "Nothing will happen if the view-limit is not active or the camera is moving.\n"
                                                                    "@return No return value")
{
    object->clampCameraViewLimit();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setRenderGroups, void, 3, 2 + MASK_BITCOUNT, "(groups$) - Sets the render group(s).\n"
              "@param groups The list of groups you wish to set.\n"
              "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setRenderGroups() - Invalid number of parameters!");
      return;
   }

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
            Con::warnf("SceneWindow::setRenderGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setRenderGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setRenderGroups(mask);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setRenderLayers, void, 3, 2 + MASK_BITCOUNT, "(layers$) - Sets the render layers(s)."
              "@param The layer numbers you wish to set.\n"
              "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setRenderLayers() - Invalid number of parameters!");
      return;
   }

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
            Con::warnf("SceneWindow::setRenderLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setRenderLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setRenderLayers(mask);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setRenderMasks, void, 3, 4, "(layerMask, groupMask) - Sets the layer/group mask which control what is rendered."
              "@param layermask The bitmask for setting the layers to render\n"
              "@param groupmask The bitmask for setting the groups to render\n"
              "@return No return value.")
{
    // Set Render Masks.
   if( argc < 4 )
      object->setRenderLayers( dAtoi( argv[2] ) );
   else
      object->setRenderMasks( dAtoi(argv[2]), dAtoi(argv[3]) );
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getRenderLayerMask, S32, 2, 2, "() - Gets the layer mask which controls what is rendered."
              "@returns The bit mask corresponding to the layers which are to be rendered")
{
   return object->getRenderLayerMask();
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getRenderGroupMask, S32, 2, 2, "() - Gets the group mask which controls what is rendered."
              "@returns The bit mask corresponding to the groups which are to be rendered")
{
   return object->getRenderGroupMask();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setBackgroundColor, void, 3, 6,  "(float red, float green, float blue, [float alpha = 1.0]) or ( stockColorName )  - Sets the background color for the scene."
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
    if (argc == 3)
    {
        // Grab the element count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

        // Has a single argument been specified?
        if ( elementCount == 1 )
        {
            object->setDataField( StringTable->insert("BackgroundColor"), NULL, argv[2] );
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
            Con::warnf("SceneWindow::setBackgroundColor() - Invalid Number of parameters!");
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
        Con::warnf("SceneWindow::setBackgroundColor() - Invalid Number of parameters!");
        return;
    }

    // Set background color.
    object->setBackgroundColor(ColorF(red, green, blue, alpha) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getBackgroundColor, const char*, 2, 2,   "Gets the background color for the scene.\n"
                                                                    "@return (float red / float green / float blue / float alpha) The background color for the scene.")
{
    // Get the background color.
    const ColorF& color = object->getBackgroundColor();

    // Fetch color name.
    StringTableEntry colorName = StockColor::name( color );

    // Return the color name if it's valid.
    if ( colorName != StringTable->EmptyString )
        return colorName;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf(pBuffer, 64, "%g %g %g %g", color.red, color.green, color.blue, color.alpha );

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setUseBackgroundColor, void, 3, 3,   "Sets whether to use the scene background color or not.\n"
                                                                "@param useBackgroundColor Whether to use the scene background color or not.\n"
                                                                "@return No return value." )
{
    // Fetch flag.
    const bool useBackgroundColor = dAtob(argv[2]);

    // Set the flag.
    object->setUseBackgroundColor( useBackgroundColor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getUseBackgroundColor, bool, 2, 2,   "Gets whether the scene background color is in use or not.\n"
                                                                "@return Whether the scene background color is in use or not." )
{
    // Get the flag.
    return object->getUseBackgroundColor();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setUseWindowInputEvents, void, 3, 3, "(inputStatus) Sets whether input events are monitored by the window or not.\n"
              "@param inputStatus Whether input events are processed by the window or not.\n"
              "@return No return value.")
{
    object->setUseWindowInputEvents( dAtob(argv[2]) );
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getUseWindowInputEvents, bool, 2, 2, "() Gets whether input events are monitored by the window or not.\n"
              "@return Whether input events are monitored by the window or not.")
{
    return object->getUseWindowInputEvents();
}   

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setUseObjectInputEvents, void, 3, 3, "(inputStatus) Sets whether input events are passed onto scene objects or not.\n"
              "@param mouseStatus Whether input events are passed onto scene objects or not.\n")
{
    object->setUseObjectInputEvents( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getUseObjectInputEvents, bool, 2, 2, "() Gets whether input events are passed onto scene objects or not.\n"
              "@return Whether input events are passed onto scene objects or not..")
{
    return object->getUseObjectInputEvents();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setObjectInputEventGroupFilter, void, 3, 2 + MASK_BITCOUNT, "(groups$) Sets the input events group filter.\n"
              "@param List of groups to filter input events with.\n"
              "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setObjectInputEventGroupFilter() - Invalid number of parameters!");
      return;
   }

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
            Con::warnf("SceneWindow::setObjectInputEventGroupFilter() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setObjectInputEventGroupFilter() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }

   // Set group filter.
   object->setObjectInputEventGroupFilter(mask);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setObjectInputEventLayerFilter, void, 3, 2 + MASK_BITCOUNT, "(layers$) Sets the input events layer filter."
              "@param The list of layers to filter input events with.\n"
              "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setObjectInputEventLayerFilter() - Invalid number of parameters!");
      return;
   }

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
            Con::warnf("SceneWindow::setObjectInputEventLayerFilter() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setObjectInputEventLayerFilter() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }

   // Set layer filter.
   object->setObjectInputEventLayerFilter(mask);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setObjectInputEventFilter, void, 4, 5, "(groupMask, layerMask, [useInvisibleFilter?]) Sets input filter for input events.")
{
    // Calculate Use Invisible Flag.
    bool useInvisible = argc >= 5 ? dAtob(argv[4]) : true;

    // Set input event filter.
    object->setObjectInputEventFilter( dAtoi(argv[2]), dAtoi(argv[3]), useInvisible );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setObjectInputEventInvisibleFilter, void, 3, 3, "(bool useInvisibleFilter) Sets whether invisible objects should be filtered for input events or not.")
{
   object->setObjectInputEventInvisibleFilter(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, addInputListener, bool, 3, 3,    "(inputListener) Adds an object as an input listener.\n"
                                                            "@param inputListener The object to add as an input listener.\n"
                                                            "@return Whether the object was added as an input event listener or not.")
{
    // Find the SimObject
    SimObject* pSimObject = Sim::findObject( argv[2] );

    // Did we find the SimObject?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "SceneWindow::addInputListener() - Could not find the object '%s' to add as an input event listener.", argv[2] );
        return false;
    }

    // Add input listener.
    object->addInputListener( pSimObject );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, removeInputListener, bool, 3, 3, "(inputListener) Removes an object from being an input listener.\n"
                                                            "@param inputListener The object to remove as an input listener.\n"
                                                            "@return Whether the object was removed as an input event listener or not.")
{
    // Find the SimObject
    SimObject* pSimObject = Sim::findObject( argv[2] );

    // Did we find the SimObject?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "SceneWindow::removeInputListener() - Could not find the object '%s' to remove from being an input event listener.", argv[2] );
        return false;
    }

    // Remove input listener.
    object->removeInputListener( pSimObject );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setLockMouse, void, 3, 3, "(bool lockSet) Sets the window mouse-lock status."
              "@return No return value.")
{
    // Set Lock Mouse.
    object->setLockMouse( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getLockMouse, bool, 2, 2, "() Gets the window mouse-lock status.")
{
    // Get Lock Mouse.
    return object->getLockMouse();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, setMousePosition, void, 3, 4, "(x/y) Sets Current Mouse Position."
              "@param x,y The coordinates to set the mouse cursor. Accepts either (x,y) or (\"x y\")")
{
   // The new position.
   Vector2 position;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      position = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      position = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::setMousePosition() - Invalid number of parameters!");
      return;
   }

    // Set Mouse Position.
    object->setMousePosition( position );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getMousePosition, const char*, 2, 2, "() Gets Current Mouse Position."
              "@return Returns a string with the current mouse cursor coordinates formatted as \"x y\"")
{
    // Fetch Mouse Position.
    Vector2 worldMousePoint = object->getMousePosition();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Generate Script Parameters.
    dSprintf(pBuffer, 32, "%g %g", worldMousePoint.x, worldMousePoint.y);

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getWorldPoint, const char*, 3, 4, "(X / Y) - Returns World coordinate of Window coordinate."
              "@param x,y The coordinates in window coordinates you wish to convert to world coordinates. Accepts either (x,y) or (\"x y\")"
              "@return Returns the desired world coordinates as a string formatted as \"x y\"")
{
   // The new position.
   Vector2 srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getWorldPoint() - Invalid number of parameters!");
      return false;
   }
   
   // Destination Point.
    Vector2 dstPoint;

    // Do Conversion.
    object->windowToScenePoint( srcPoint, dstPoint );

    return dstPoint.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getWindowPoint, const char*, 3, 4, "(X / Y) - Returns Window coordinate of World coordinate."
              "@param x,y The coordinates in world coordinates you wish to convert to window coordinates. Accepts either (x,y) or (\"x y\")"
              "@return Returns the desired window coordinates as a string formatted as \"x y\"")
{
   // The new position.
   Vector2 srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getWindowPoint() - Invalid number of parameters!");
      return NULL;
   }
   
   // Destination Point.
    Vector2 dstPoint;

    // Do Conversion.
    object->sceneToWindowPoint( srcPoint, dstPoint );

    return dstPoint.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getCanvasPoint, const char*, 3, 4, "(X / Y) - Returns Canvas coordinate of Window coordinate."
               "@param x,y The coordinates in world coordinates you wish to convert to window coordinates. Accepts either (x,y) or (\"x y\")"
              "@return Returns the desired canvas coordinates as a string formatted as \"x y\"")
{
   // The new position.
   Point2I srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
   {
      srcPoint.x = dAtoi(Utility::mGetStringElement(argv[2], 0));
      srcPoint.y = dAtoi(Utility::mGetStringElement(argv[2], 1));
   }

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Point2I(dAtoi(argv[2]), dAtoi(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getCanvasPoint() - Invalid number of parameters!");
      return NULL;
   }

    // Do Conversion.
    Point2I dstPoint = object->localToGlobalCoord( srcPoint );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", dstPoint.x, dstPoint.y);
    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SceneWindow, getIsWindowPoint, bool, 3, 4, "(X / Y) Checks if World coordinate is inside Window."
               "@param x,y The coordinates in world coordinates you wish to check. Accepts either (x,y) or (\"x y\")"
              "@return Returns true if the coordinates are within the window, and false otherwise.")
{
   // The new position.
   Vector2 srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getIsWindowPoint() - Invalid number of parameters!");
      return false;
   }

   // Destination Point.
    Vector2 dstPoint;
    // Do Conversion.
    object->sceneToWindowPoint( srcPoint, dstPoint );

    // Check if point is in window bounds.
    return object->mBounds.pointInRect( Point2I( S32(mFloor(dstPoint.x)+object->mBounds.point.x), S32(mFloor(dstPoint.y)+object->mBounds.point.y )) );
}
