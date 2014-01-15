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

ConsoleFunction( getGlobalSceneCount, S32, 1, 1,    "() - Gets the system-wide scene count.\n"
                                                    "@return The system-wide scene count." )
{
    return Scene::getGlobalSceneCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setGravity, void, 3, 4, "(forceX / forceY) The gravity force to apply to all objects in the scene.\n"
              "@param forceX/forceY The direction and magnitude of the force in each direction. Formatted as either (\"forceX forceY\") or (forceX, forceY)\n"
              "@return No return value.\n")
{
   // The force.
   Vector2 force;

   // Grab the element count.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("forceX forceY")
   if (elementCount == 1)
   {
      force = Vector2(dAtof(argv[2]), dAtof(argv[3]));
   }
   else if (elementCount == 2)
   {
      force = Utility::mGetStringElementVector(argv[2]);
   }
   // Invalid
   else
   {
      Con::warnf("SceneObject::setGravity() - Invalid number of parameters!");
      return;
   }

    // Set gravity.
   object->setGravity(force);
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getGravity, const char*, 2, 2, "() Gets the gravity force applied to all objects in the scene.\n"
                                                            "@return The gravity force applied to all objects in the scene." )
{
    return Vector2(object->getGravity()).scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setVelocityIterations, void, 3, 3, "(int iterations) Sets the number of velocity iterations the physics solver uses.\n"
                                                                "@return No return value.")
{
    object->setVelocityIterations( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getVelocityIterations, S32, 2, 2,  "() Gets the number of velocity iterations the physics solver uses.\n"
                                                                "(@return The number of velocity iterations the physics solver uses." )
{
    return object->getVelocityIterations();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setPositionIterations, void, 3, 3, "(int iterations) Sets the number of position iterations the physics solver uses.\n"
                                                                "@return No return value.")
{
    object->setPositionIterations( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getPositionIterations, S32, 2, 2,  "() Gets the number of position iterations the physics solver uses.\n"
                                                                "(@return The number of position iterations the physics solver uses." )
{
    return object->getPositionIterations();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, add, void, 3, 3,   "(sceneObject) Add the SceneObject to the scene.\n"
                                        "@param sceneObject The SceneObject to add to the scene.\n"
                                        "@return No return value.")
{
    // Find the specified object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

    // Did we find the object?
    if ( !pSceneObject )
    {
        // No, so warn.
        Con::warnf("Scene::addToScene() - Could not find the specified object '%s'.", argv[2]);
        return;
    }

    // Add to Scene.
    object->addToScene( pSceneObject );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, remove, void, 3, 3,    "(sceneObject) Remove the SceneObject from the scene.\n"
                                            "@param sceneObject The SceneObject to remove from the scene.\n"
                                            "@return No return value.")
{
    // Find the specified object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

    // Did we find the object?
    if ( !pSceneObject )
    {
        // No, so warn.
        Con::warnf("Scene::removeFromScene() - Could not find the specified object '%s'.", argv[2]);
        return;
    }


    // Remove from Scene.
    object->removeFromScene( pSceneObject );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, clear, void, 2, 3, "([deleteObjects]) Clear the scene of all scene objects.\n"
                                        "@param deleteObjects A boolean flag that sets whether to delete the objects as well as remove them from the scene (default is true).\n"
                                        "@return No return value.")
{
    // Calculate 'Delete Objects' flag.
    bool deleteObjects;
    if ( argc >= 3 )
        deleteObjects = dAtob( argv[2] );
    else
        deleteObjects = true;

    // Clear Scene.
    object->clearScene( deleteObjects );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getCount, S32, 2, 2,   "() Gets the count of scene objects in the scnee.\n"
                                            "@return Returns the number of scene objects in current scene as an integer.")
{
    // Get Scene Object-Count.
    return object->getSceneObjectCount();
}  


//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getObject, S32, 3, 3,  "(sceneObjectIndex) Gets the scene object at the selected index.\n"
                                            "@param sceneObjectIndex The index of the desired object\n"
                                            "@return The scene object at the specified index.")
{
    // Fetch Object Index.
    const U32 objectIndex = dAtoi(argv[2]);

    // Fetch scene object count.
    const U32 sceneObjectCount = object->getSceneObjectCount();

    // Sanity!
    if ( objectIndex >= sceneObjectCount )
    {
        // Error so warn.
        Con::warnf("Scene::getObject() - Cannot retrieve specified object index (%d) as there are only (%d) object(s) in the scene!", objectIndex, sceneObjectCount );
        // Return no object.
        return 0;
    }

    // Fetch Scene Object.
    const SceneObject* pSceneObject = object->getSceneObject( objectIndex );

    // Check Object.
    if ( pSceneObject != NULL )
    {
        // No error so return object id.
        return pSceneObject->getId();
    }
    else
    {
        // Error so warn.
        Con::warnf("Scene::getObject() - Cannot retrieve specified object index (%d)!", objectIndex);
        // Return no object.
        return 0;
    }
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getSceneObjectList, const char*, 2, 2, "() Gets the Scene Object-List.\n"
                                                            "@return Returns a string with a list of object IDs")
{
    // Scene Object-List.
    Vector<SceneObject*> objList;

    // Finish here if there are no scene objects.
    U32 objCount = object->getSceneObjects( objList );
    if( objCount == 0 )
        return NULL;

    // Our return buffer will be 6 times the size of our object list (4 for Id (+1 for future size?) + 1 for space).
    U32 maxBufferSize = objCount * 12;

    // Create Returnable Buffer.
    char *pBuffer = Con::getReturnBuffer( maxBufferSize ); 

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Iterate through the list and generate an id string list to return
    for ( S32 n = 0; n < objList.size(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", objList[n]->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("Scene::getSceneObjectList() - Not enough space to return all %d objects!", objList.size());
            break;
        }
    }

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getAssetPreloadCount, S32, 2, 2,   "() Gets the number of assets set to preload for this scene.\n"
                                                        "@return The number of assets set to preload for this scene.")
{
    return object->getAssetPreloadCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getAssetPreload, const char*, 3, 3,    "(index) Gets the asset to be preloaded at the specified index.\n"
                                                            "@param index The index of the preloaded asset.\n"
                                                            "@return The asset to be preloaded at the specified index.")
{
    // Fetch preload index.
    const S32 index = dAtoi(argv[2]);

    // Fetch the asset pointer.
    const AssetPtr<AssetBase>* pAssetPtr = object->getAssetPreload( index );

    return pAssetPtr == NULL ? NULL : pAssetPtr->getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, addAssetPreload, void, 3, 3,   "(assetId) Adds the asset Id so that it is preloaded when the scene is loaded.\n"
                                                    "The asset loaded immediately by this operation.  Duplicate assets are ignored.\n"
                                                    "@param assetId The asset Id to be added.\n"
                                                    "@return No return value.")
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Add asset preload.
    object->addAssetPreload( pAssetId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, removeAssetPreload, void, 3, 3,    "(assetId) Removes the asset Id from being preloaded when the scene is loaded.\n"
                                                        "The asset may be unloaded immediately by this operation if it has no other references.\n"
                                                        "@param assetId The asset Id to be removed.\n"
                                                        "@return No return value.")
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Remove asset preload.
    object->removeAssetPreload( pAssetId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, clearAssetPreloads, void, 2, 2,    "() Clears all assets added as a preload.\n"
                                                        "@return No return value.")
{
    // Clear asset preloads.
    object->clearAssetPreloads();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, mergeScene, void, 3, 3,    "(scene) Merges the specified scene into this scene by cloning the scenes contents.")
{
    // Find the specified scene.
    Scene* pScene = Sim::findObject<Scene>( argv[2] );

    // Did we find the scene?
    if ( pScene == NULL )
    {
        // No, so warn.
        Con::warnf( "Scene::mergeScene() - Could not find the specified scene '%s'.", argv[2] );
        return;
    }

    object->mergeScene( pScene );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getControllers, const char*, 2, 2,	"() Gets the Scene Controllers.\n"
                                                        "@return Gets the scene controllers.")
{
    // Fetch the scene controllers.
    SimSet* pControllerSet = object->getControllers();

    return ( pControllerSet == NULL ) ? StringTable->EmptyString : pControllerSet->getIdString();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getSceneTime, F32, 2, 2,   "() Gets the Scene Time.\n"
                                                        "@return Returns the time as a floating point number\n")
{
    // Get Scene Time.
    return object->getSceneTime();
}   

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setScenePause, void, 3, 3, "(status) Sets scene pause status.\n"
              "@return No return value.")
{
    // Set Scene Pause.
    object->setScenePause( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getScenePause, bool, 2, 2, "() Gets scene pause status.\n"
              "@return Returns a boolean value. True if pause status, false otherwise.")
{
    // Get Scene Pause.
    return object->getScenePause();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getJointCount, S32, 2, 2,  "() Gets the joint count.\n"
                                                        "@return Returns no value")
{
    return object->getJointCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, isJoint, bool, 3, 3,   "(int jointId) Gets whether the joint Id is valid or not.\n"
                                            "@param jointId The Id of the joint.\n"
                                            "@return whether the joint Id is valid or not." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi( argv[2] );

    return object->findJoint( jointId ) != NULL;
}                                                                  


//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getJointType, const char*, 3, 3,   "(int jointId) Gets the joint type of the specified joint Id.\n"
                                                                "@param jointId The Id of the joint.\n"
                                                                "@return The type of joint of the specified joint Id." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi( argv[2] );

    // Fetch joint type.
    const b2JointType jointType = object->getJointType( jointId );

    // Ignore if invalid joint.
    if ( jointType == e_unknownJoint )
        return StringTable->EmptyString;

    return Scene::getJointTypeDescription( jointType );
}                                                                  

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, deleteJoint, bool, 3, 3,           "(int jointId) Deletes the specified joint Id.\n"
                                                                "@param jointId The Id of the joint.\n"
                                                                "@return Whether the joint was successfully deleted or not." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi( argv[2] );

    return object->deleteJoint( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createDistanceJoint, S32, 4, 12,   "(sceneObjectA, sceneObjectB, [localAnchorA X/Y], [localAnchorB X/Y], [distance], [frequency], [dampingRatio], [collideConnected]) Creates a distance joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param distance The distance the joint should maintain between scene objects.  The default is the distance currently between the scene objects.\n"
                                                                "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness (default).\n"
                                                                "@param dampingRatio The damping ratio. 0 = no damping (default), 1 = critical damping.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createDistanceJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if (*sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createDistanceJoint() - Could not find scene object %d.", sceneObjectB);
    }

    if ( argc == 4 )
    {
        return object->createDistanceJoint( pSceneObjectA, pSceneObjectB );
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createDistanceJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA );
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createDistanceJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB );
    }

    // Fetch length.
    const F32 length = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, length );
    }

    // Fetch frequency (Hertz).
    const F32 frequency = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, length, frequency );
    }

    // Fetch damping ratio.
    const F32 dampingRatio = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, length, frequency, dampingRatio );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, length, frequency, dampingRatio, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setDistanceJointLength, void, 4, 4,    "(jointId, length) Sets the distance the joint should maintain between scene objects.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param length The length the joint should maintain between scene objects.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 length = dAtof(argv[3]);

    // Access joint.
    object->setDistanceJointLength( jointId, length );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getDistanceJointLength, F32, 3, 3,     "(jointId) Gets the distance the joint should maintain between scene objects.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the distance the joint should maintain between scene objects (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getDistanceJointLength( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setDistanceJointFrequency, void, 4, 4, "(jointId, frequency) Sets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 frequency = dAtof(argv[3]);

    // Access joint.
    object->setDistanceJointFrequency( jointId, frequency );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getDistanceJointFrequency, F32, 3, 3,  "(jointId) Gets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the mass-spring-damper frequency in Hertz (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getDistanceJointFrequency( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setDistanceJointDampingRatio, void, 4, 4,  "(jointId, dampingRatio) Sets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@param dampingRatio The damping ratio. 0 = no damping, 1 = critical damping.\n"
                                                                        "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 dampingRatio = dAtof(argv[3]);

    // Access joint.
    object->setDistanceJointDampingRatio( jointId, dampingRatio );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getDistanceJointDampingRatio, F32, 3, 3,   "(jointId) Gets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns the damping ratio (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getDistanceJointDampingRatio( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createRopeJoint, S32, 4, 10,       "(sceneObjectA, sceneObjectB, [localAnchorA X/Y], [localAnchorB X/Y], [maxLength], [collideConnected]) Creates a rope joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param maxLength The maximum rigid length of the rope.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createRopeJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if (*sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createRopeJoint() - Could not find scene object %d.", sceneObjectB);
    }

    if ( argc == 4 )
    {
        return object->createRopeJoint( pSceneObjectA, pSceneObjectB );
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createRopeJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createRopeJoint( pSceneObjectA, pSceneObjectB, localAnchorA );
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createRopeJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createRopeJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB );
    }

    // Fetch maximum length.
    const F32 maxLength = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createRopeJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxLength );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createRopeJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxLength, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setRopeJointMaxLength, void, 4, 4,     "(jointId, maxLength) Sets the maximum rigid length of the rope.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param maxLength The maximum rigid length of the rope.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 maxLength = dAtof(argv[3]);

    // Access joint.
    object->setRopeJointMaxLength( jointId, maxLength );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getRopeJointMaxLength, F32, 3, 3,     "(jointId) Gets the maximum rigid length of the rope.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the maximum rigid length of the rope (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getRopeJointMaxLength( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createRevoluteJoint, S32, 4, 9,    "(sceneObjectA, sceneObjectB, [localAnchorA X/Y], [localAnchorB X/Y], [collideConnected]) Creates a revolute joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createRevoluteJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createRevoluteJoint() - Could not find scene object %d.", sceneObjectB);
    }

    if ( argc == 4 )
    {
        return object->createRevoluteJoint( pSceneObjectA, pSceneObjectB );
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createRevoluteJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createRevoluteJoint( pSceneObjectA, pSceneObjectB, localAnchorA );
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createRevoluteJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createRevoluteJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createRevoluteJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setRevoluteJointLimit, void, 4, 6,     "(jointId, enableLimit, [lowerAngle], [upperAngle]) Sets whether the joint has angular limits or not and the limits themselves.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param enableLimit Whether the joint has angular limits or not.\n"
                                                                    "@param lowerAngle The lower angle of the angular limit.\n"
                                                                    "@param upperAngle The upper angle of the angular limit.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const bool enableLimit = dAtob(argv[3]);
    const F32 lowerAngle = argc > 4 ? mDegToRad( dAtof(argv[4]) ) : 0.0f;
    const F32 upperAngle = argc > 5 ? mDegToRad( dAtof(argv[5]) ) : lowerAngle;

    // Access joint.
    object->setRevoluteJointLimit( jointId, enableLimit, lowerAngle, upperAngle );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getRevoluteJointLimit, const char*, 3, 3,  "(jointId) Gets whether the joint has angular limits or not and the limits themselves.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns whether the joint has angular limits or not and the limits themselves (empty string indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Args.
    bool enableLimit;
    F32 lowerAngle;
    F32 upperAngle;

    // Access joint.
    if ( !object->getRevoluteJointLimit( jointId, enableLimit, lowerAngle, upperAngle ) )
    {
        return NULL;
    }

    // Format output.
    char* pBuffer = Con::getReturnBuffer(64);
    dSprintf( pBuffer, 64, "%d %g %g", enableLimit, mRadToDeg(lowerAngle), mRadToDeg(upperAngle) );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setRevoluteJointMotor, void, 4, 6,     "(jointId, enableMotor, [motorSpeed], [maxMotorTorque]) Sets whether the joint has a motor or not and the motor settings.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param enableMotor Whether the joint has a motor or not.\n"
                                                                    "@param motorSpeed The motor speed (degrees per/sec).\n"
                                                                    "@param maxMotorTorque The maximum motor torque used to achieve the specified motor speed (N-m).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const bool enableMotor = dAtob(argv[3]);
    const F32 motorSpeed = argc > 4 ? mDegToRad( dAtof(argv[4]) ) : 0.0f;
    const F32 maxMotorTorque = argc > 5 ? dAtof(argv[5]) : 0.0f;

    // Access joint.
    object->setRevoluteJointMotor( jointId, enableMotor, motorSpeed, maxMotorTorque );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getRevoluteJointMotor, const char*, 3, 3,  "(jointId) Gets whether the joint has a motor or not and the motor settings.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns whether the joint has a motor or not and the motor settings (empty string indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Args.
    bool enableMotor;
    F32 motorSpeed;
    F32 maxMotorTorque;

    // Access joint.
    if ( !object->getRevoluteJointMotor( jointId, enableMotor, motorSpeed, maxMotorTorque ) )
    {
        return NULL;
    }

    // Format output.
    char* pBuffer = Con::getReturnBuffer(64);
    dSprintf( pBuffer, 64, "%d %g %g", enableMotor, mRadToDeg(motorSpeed), maxMotorTorque );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getRevoluteJointAngle, F32, 3, 3,      "(jointId) Gets the current angle of a revolute joint.\n"
                                                            "@param jointId The Id of the joint to use.\n"
                                                            "@return Returns the joint angle." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
	return object->getRevoluteJointAngle( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getRevoluteJointSpeed, F32, 3, 3,  "(jointId) Gets the current speed of a revolute joint.\n"
                                                        "@param jointId The Id of the joint to use.\n"
                                                        "@return Returns the joint speed as Angular Velocity" )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
	return object->getRevoluteJointSpeed( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createWeldJoint, S32, 4, 11,       "(sceneObjectA, sceneObjectB, [localAnchorA X/Y], [localAnchorB X/Y], [frequency], [dampingRatio], [collideConnected]) Creates a weld joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness (default).\n"
                                                                "@param dampingRatio The damping ratio. 0 = no damping (default), 1 = critical damping.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createWeldJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createWeldJoint() - Could not find scene object %d.", sceneObjectB);
    }

    if ( argc == 4 )
    {
        return object->createWeldJoint( pSceneObjectA, pSceneObjectB );
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createWeldJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA );
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createWeldJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB );
    }

    // Fetch frequency (Hertz).
    const F32 frequency = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, frequency );
    }

    // Fetch damping ratio.
    const F32 dampingRatio = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, frequency, dampingRatio );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, frequency, dampingRatio, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setWeldJointFrequency, void, 4, 4,     "(jointId, frequency) Sets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 frequency = dAtof(argv[3]);

    // Access joint.
    object->setWeldJointFrequency( jointId, frequency );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getWeldJointFrequency, F32, 3, 3,     "(jointId) Gets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the mass-spring-damper frequency in Hertz (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getWeldJointFrequency( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setWeldJointDampingRatio, void, 4, 4,      "(jointId, dampingRatio) Sets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@param dampingRatio The damping ratio. 0 = no damping, 1 = critical damping.\n"
                                                                        "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 dampingRatio = dAtof(argv[3]);

    // Access joint.
    object->setWeldJointDampingRatio( jointId, dampingRatio );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getWeldJointDampingRatio, F32, 3, 3,       "(jointId) Gets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns the damping ratio (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getWeldJointDampingRatio( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createWheelJoint, S32, 7, 11,      "(sceneObjectA, sceneObjectB, localAnchorA X/Y, localAnchorB X/Y, worldAxis X/Y, [collideConnected]) Creates a wheel joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param worldAxis The world axis of the wheel suspension spring.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createWheelJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createWheelJoint() - Could not find scene object %d.", sceneObjectB);
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createWheelJoint() - Invalid number of parameters!");
        return -1;
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createWheelJoint() - Invalid number of parameters!");
        return -1;
    }

    // World axis.
    const U32 worldAxisElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldAxis;

    if ( worldAxisElementCount == 1 && argc > (nextArg+1) )
    {
        worldAxis.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldAxisElementCount == 2 )
    {
        worldAxis = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createWheelJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createWheelJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createWheelJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setWheelJointMotor, void, 4, 6,        "(jointId, enableMotor, [motorSpeed], [maxMotorTorque]) Sets whether the joint has a motor or not and the motor settings.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param enableMotor Whether the joint has a motor or not.\n"
                                                                    "@param motorSpeed The motor speed (degrees per/sec).\n"
                                                                    "@param maxMotorTorque The maximum motor torque used to achieve the specified motor speed (N-m).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const bool enableMotor = dAtob(argv[3]);
    const F32 motorSpeed = argc > 4 ? mDegToRad( dAtof(argv[4]) ) : 0.0f;
    const F32 maxMotorTorque = argc > 5 ? dAtof(argv[5]) : 0.0f;

    // Access joint.
    object->setWheelJointMotor( jointId, enableMotor, motorSpeed, maxMotorTorque );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getWheelJointMotor, const char*, 3, 3, "(jointId) Gets whether the joint has a motor or not and the motor settings.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns whether the joint has a motor or not and the motor settings (empty string indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Args.
    bool enableMotor;
    F32 motorSpeed;
    F32 maxMotorTorque;

    // Access joint.
    if ( !object->getWheelJointMotor( jointId, enableMotor, motorSpeed, maxMotorTorque ) )
    {
        return NULL;
    }

    // Format output.
    char* pBuffer = Con::getReturnBuffer(64);
    dSprintf( pBuffer, 64, "%d %g %g", enableMotor, mRadToDeg(motorSpeed), maxMotorTorque );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setWheelJointFrequency, void, 4, 4,    "(jointId, frequency) Sets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 frequency = dAtof(argv[3]);

    // Access joint.
    object->setWheelJointFrequency( jointId, frequency );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getWheelJointFrequency, F32, 3, 3,     "(jointId) Gets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the mass-spring-damper frequency in Hertz (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getWheelJointFrequency( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setWheelJointDampingRatio, void, 4, 4,     "(jointId, dampingRatio) Sets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@param dampingRatio The damping ratio. 0 = no damping, 1 = critical damping.\n"
                                                                        "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 dampingRatio = dAtof(argv[3]);

    // Access joint.
    object->setWheelJointDampingRatio( jointId, dampingRatio );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getWheelJointDampingRatio, F32, 3, 3,      "(jointId) Gets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns the damping ratio (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getWheelJointDampingRatio( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createFrictionJoint, S32, 4, 11,   "(sceneObjectA, sceneObjectB, [localAnchorA X/Y], [localAnchorB X/Y], [maxForce], [maxTorque], [collideConnected]) Creates a friction joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param maxForce The maximum friction force (N).\n"
                                                                "@param maxTorque The maximum torque force (N-m).\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createFrictionJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createFrictionJoint() - Could not find scene object %d.", sceneObjectB);
    }

    if ( argc == 4 )
    {
        return object->createFrictionJoint( pSceneObjectA, pSceneObjectB );
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createFrictionJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createFrictionJoint( pSceneObjectA, pSceneObjectB, localAnchorA );
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createFrictionJoint() - Invalid number of parameters!");
        return -1;
    }

    // Fetch maximum force.
    const F32 maxForce = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createFrictionJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxForce );
    }

    // Fetch maximum torque.
    const F32 maxTorque = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createFrictionJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxForce, maxTorque );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createFrictionJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxForce, maxTorque, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setFrictionJointMaxForce, void, 4, 4,  "(jointId, maxForce) Sets the maximum friction force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param maxForce The maximum friction force (N).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 maxForce = dAtof(argv[3]);

    // Access joint.
    object->setFrictionJointMaxForce( jointId, maxForce );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getFrictionJointMaxForce, F32, 3, 3,   "(jointId) Sets the maximum friction force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the maximum friction force (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getFrictionJointMaxForce( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setFrictionJointMaxTorque, void, 4, 4, "(jointId, maxTorque) Sets the maximum torque force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param maxTorque The maximum torque force (N).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 maxTorque = dAtof(argv[3]);

    // Access joint.
    object->setFrictionJointMaxTorque( jointId, maxTorque );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getFrictionJointMaxTorque, F32, 3, 3,  "(jointId) Gets the maximum torque force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the maximum torque force (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getFrictionJointMaxTorque( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createPrismaticJoint, S32, 7, 11,  "(sceneObjectA, sceneObjectB, localAnchorA X/Y, localAnchorB X/Y, worldAxis X/Y, [collideConnected]) Creates a prismatic joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param worldAxis The world axis defining the translational degree of freedom.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createPrismaticJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createPrismaticJoint() - Could not find scene object %d.", sceneObjectB);
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPrismaticJoint() - Invalid number of parameters!");
        return -1;
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPrismaticJoint() - Invalid number of parameters!");
        return -1;
    }

    // World axis.
    const U32 worldAxisElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldAxis;

    if ( worldAxisElementCount == 1 && argc > (nextArg+1) )
    {
        worldAxis.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldAxisElementCount == 2 )
    {
        worldAxis = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPrismaticJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createPrismaticJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createPrismaticJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setPrismaticJointLimit, void, 4, 6,    "(jointId, enableLimit, [lowerTranslation], [upperTranslation]) Sets whether the joint has translational limits or not and the limits themselves.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param enableLimit Whether the joint has angular limits or not.\n"
                                                                    "@param lowerTranslation The lower translation limit.\n"
                                                                    "@param upperTranslation The upper translation limit.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const bool enableLimit = dAtob(argv[3]);
    const F32 lowerTranslation = argc > 4 ? dAtof(argv[4]) : 0.0f;
    const F32 upperTranslation = argc > 5 ? dAtof(argv[5]) : lowerTranslation;

    // Access joint.
    object->setPrismaticJointLimit( jointId, enableLimit, lowerTranslation, upperTranslation );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getPrismaticJointLimit, const char*, 3, 3, "(jointId) Gets whether the joint has translational limits or not and the limits themselves.\n"
                                                                        "@return Returns whether the joint has translational limits or not and the limits themselves (empty string indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Args.
    bool enableLimit;
    F32 lowerTranslation;
    F32 upperTranslation;

    // Access joint.
    if ( !object->getPrismaticJointLimit( jointId, enableLimit, lowerTranslation, upperTranslation ) )
    {
        return NULL;
    }

    // Format output.
    char* pBuffer = Con::getReturnBuffer(64);
    dSprintf( pBuffer, 64, "%d %g %g", enableLimit, lowerTranslation, upperTranslation );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setPrismaticJointMotor, void, 4, 6,    "(jointId, enableMotor, [motorSpeed], [maxMotorForce]) Sets whether the joint has a motor or not and the motor settings.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param enableMotor Whether the joint has a motor or not.\n"
                                                                    "@param motorSpeed The motor speed (degrees per/sec).\n"
                                                                    "@param maxMotorForce The maximum motor force used to achieve the specified motor speed (N-m).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const bool enableMotor = dAtob(argv[3]);
    const F32 motorSpeed = argc > 4 ? mDegToRad( dAtof(argv[4]) ) : 0.0f;
    const F32 maxMotorForce = argc > 5 ? dAtof(argv[5]) : 0.0f;

    // Access joint.
    object->setPrismaticJointMotor( jointId, enableMotor, motorSpeed, maxMotorForce );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getPrismaticJointMotor, const char*, 3, 3,    "(jointId) Gets whether the joint has a motor or not and the motor settings.\n"
                                                                            "@return Returns whether the joint has a motor or not and the motor settings (empty string indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Args.
    bool enableMotor;
    F32 motorSpeed;
    F32 maxMotorForce;

    // Access joint.
    if ( !object->getPrismaticJointMotor( jointId, enableMotor, motorSpeed, maxMotorForce ) )
    {
        return NULL;
    }

    // Format output.
    char* pBuffer = Con::getReturnBuffer(64);
    dSprintf( pBuffer, 64, "%d %g %g", enableMotor, mRadToDeg(motorSpeed), maxMotorForce );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createPulleyJoint, S32, 9, 16,     "(sceneObjectA, sceneObjectB, localAnchorA X/Y, localAnchorB X/Y, worldGroundAnchorA X/Y, worldGroundAnchorB X/Y, ratio, [lengthA], [lengthB], [collideConnected]) Creates a prismatic joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param localAnchorA The local point of the first scene object where the joint connects.\n"
                                                                "@param localAnchorB The local point of the second scene object where the joint connects.\n"
                                                                "@param worldGroundAnchorA The world point of the first ground anchor.  This point never moves.\n"
                                                                "@param worldGroundAnchorB The world point of the second ground anchor.  This point never moves.\n"
                                                                "@param ratio The pulley ratio used to simulate a block-and-tackle pulley.\n"
                                                                "@param lengthA The reference pulley length for the segment attached to scene object A.  Defaults to the distance between the first scene object and the first ground anchor.\n"
                                                                "@param lengthB The reference pulley length for the segment attached to scene object B.  Defaults to the distance between the second scene object and the second ground anchor.\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createPulleyJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createPulleyJoint() - Could not find scene object %d.", sceneObjectB);
    }

    // Local anchor A.
    const U32 anchorAElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 localAnchorA;

    S32 nextArg = 5;
    if ( anchorAElementCount == 1 && argc > 5 )
    {
        localAnchorA.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( anchorAElementCount == 2 )
    {
        localAnchorA = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPulleyJoint() - Invalid number of parameters!");
        return -1;
    }

    // Local anchor B.
    const U32 anchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localAnchorB;

    if ( anchorBElementCount == 1 && argc > (nextArg+1) )
    {
        localAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( anchorBElementCount == 2 )
    {
        localAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPulleyJoint() - Invalid number of parameters!");
        return -1;
    }

    // World ground anchor A.
    const U32 worldGroundAnchorAElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldGroundAnchorA;

    if ( worldGroundAnchorAElementCount == 1 && argc > 5 )
    {
        worldGroundAnchorA.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg++]) );
        nextArg += 2;
    }
    else if ( worldGroundAnchorAElementCount == 2 )
    {
        worldGroundAnchorA = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPulleyJoint() - Invalid number of parameters!");
        return -1;
    }

    // World ground anchor B.
    const U32 worldGroundAnchorBElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldGroundAnchorB;

    if ( worldGroundAnchorBElementCount == 1 && argc > (nextArg+1) )
    {
        worldGroundAnchorB.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldGroundAnchorBElementCount == 2 )
    {
        worldGroundAnchorB = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createPulleyJoint() - Invalid number of parameters!");
        return -1;
    }

    // Fetch maximum ratio.
    const F32 ratio = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createPulleyJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldGroundAnchorA, worldGroundAnchorB, ratio );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createPulleyJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldGroundAnchorA, worldGroundAnchorB, ratio, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createTargetJoint, S32, 5, 10,     "(sceneObject, worldTarget X/Y, maxForce, [useCenterOfMass?], [frequency], [dampingRatio], [collideConnected]) Creates a target joint.\n"
                                                        "@param sceneObject The scene object to connect to the joint.\n"
                                                        "@param worldTarget The world point target to move the scene object to.\n"
                                                        "@param maxForce The maximum force the joint should use to position the scene object at the target.\n"
                                                        "@param useCenterOfMass Whether to use the center of mass as the point which the joint is attached or not.  Defaults to false.\n"
                                                        "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness (default=0.7).\n"
                                                        "@param dampingRatio The damping ratio. 0 = no damping (default), 1 = critical damping.\n"
                                                        "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                        "@return The joint Id (-1 if error).")
{
    // Fetch scene object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>(argv[2]);

    // Check scene object.
    if ( !pSceneObject )
    {
        Con::warnf("Scene::createTargetJoint() - Could not find scene object %d.", argv[2]);
        return -1;
    }

    // World target.
    const U32 worldTargetElementCount = Utility::mGetStringElementCount(argv[3]);

    b2Vec2 worldTarget;

    S32 nextArg = 4;
    if ( worldTargetElementCount == 1 && argc > 5 )
    {
        worldTarget.Set( dAtof(argv[3]), dAtof(argv[4]) );
        nextArg = 5;
    }
    else if ( worldTargetElementCount == 2 )
    {
        worldTarget = Utility::mGetStringElementVector(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createTargetJoint() - Invalid number of parameters!");
        return -1;
    }

    // Fetch maximum force.
    const F32 maxForce = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createTargetJoint( pSceneObject, worldTarget, maxForce );
    }

    // Fetch the center-of-mass flag.
    const bool centerOfMass = dAtob(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createTargetJoint( pSceneObject, worldTarget, maxForce, centerOfMass );
    }

    // Fetch frequency (Hertz).
    const F32 frequency = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createTargetJoint( pSceneObject, worldTarget, maxForce, centerOfMass, frequency );
    }

    // Fetch damping ratio.
    const F32 dampingRatio = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createTargetJoint( pSceneObject, worldTarget, maxForce, centerOfMass, frequency, dampingRatio );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createTargetJoint( pSceneObject, worldTarget, maxForce, centerOfMass, frequency, dampingRatio, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setTargetJointTarget, void, 4, 5,      "(jointId, worldTarget X/Y) Sets the target world point for the scene object.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param worldTarget The target world point to move the scene object to.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // World target.
    const U32 worldTargetElementCount = Utility::mGetStringElementCount(argv[3]);

    b2Vec2 worldTarget;

    if ( worldTargetElementCount == 1 && argc > 5 )
    {
        worldTarget.Set( dAtof(argv[3]), dAtof(argv[4]) );
    }
    else if ( worldTargetElementCount == 2 )
    {
        worldTarget = Utility::mGetStringElementVector(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::setTargetJointTarget() - Invalid number of parameters!");
    }

    // Access joint.
    object->setTargetJointTarget( jointId, worldTarget );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getTargetJointTarget, const char*, 3, 3,   "(jointId) Gets the target world point for the scene object.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns the target world point for the scene object (always 0,0 if error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    const Vector2 worldTarget = object->getTargetJointTarget( jointId );

    return worldTarget.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setTargetJointFrequency, void, 4, 4,   "(jointId, frequency) Sets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param frequency The mass-spring-damper frequency in Hertz. A value of 0 disables softness.\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 frequency = dAtof(argv[3]);

    // Access joint.
    object->setTargetJointFrequency( jointId, frequency );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getTargetJointFrequency, F32, 3, 3,   "(jointId) Gets the mass-spring-damper frequency in Hertz.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the mass-spring-damper frequency in Hertz (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getTargetJointFrequency( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setTargetJointDampingRatio, void, 4, 4,    "(jointId, dampingRatio) Sets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@param dampingRatio The damping ratio. 0 = no damping, 1 = critical damping.\n"
                                                                        "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 dampingRatio = dAtof(argv[3]);

    // Access joint.
    object->setTargetJointDampingRatio( jointId, dampingRatio );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getTargetJointDampingRatio, F32, 3, 3,    "(jointId) Sets the damping ratio.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns the damping ratio (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getTargetJointDampingRatio( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, createMotorJoint, S32, 4, 11,      "(sceneObjectA, sceneObjectB, [linearOffset X/Y], [angularOffset], [maxForce], [maxTorque], [correctionFactor], [collideConnected]) Creates a motor joint.\n"
                                                                "@param sceneObjectA The first scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param sceneObjectB The second scene object to connect to the joint.  Use an empty string to indicate the Scene ground body.\n"
                                                                "@param linearOffsetX/Y The linear offset in sceneObjectA space.\n"
                                                                "@param angularOffset The angularOffset between the bodies.\n"
                                                                "@param maxForce The maximum friction force (N).\n"
                                                                "@param maxTorque The maximum torque force (N-m).\n"
                                                                "@param correctionFactor The correction factor (tolerance).\n"
                                                                "@param collideConnected Whether the scene objects can collide with each other while connected with this joint.\n"
                                                                "@return The joint Id (-1 if error).")
{
    // Fetch scene object references.
    const char* sceneObjectA = argv[2];
    const char* sceneObjectB = argv[3];

    SceneObject* pSceneObjectA = NULL;
    SceneObject* pSceneObjectB = NULL;

    // Fetch scene object.
    if ( *sceneObjectA != 0 )
    {
        pSceneObjectA = Sim::findObject<SceneObject>(sceneObjectA);

        if ( !pSceneObjectA )
            Con::warnf("Scene::createMotorJoint() - Could not find scene object %d.", sceneObjectA);
    }

    // Fetch scene object.
    if ( *sceneObjectB != 0 )
    {
        pSceneObjectB = Sim::findObject<SceneObject>(sceneObjectB);

        if ( !pSceneObjectB )
            Con::warnf("Scene::createMotorJoint() - Could not find scene object %d.", sceneObjectB);
    }

    if ( argc == 4 )
    {
        return object->createMotorJoint( pSceneObjectA, pSceneObjectB );
    }

    // Linear offset.
    const U32 linearOffsetElementCount = Utility::mGetStringElementCount(argv[4]);

    b2Vec2 linearOffset;

    S32 nextArg = 5;
    if ( linearOffsetElementCount == 1 && argc > 5 )
    {
        linearOffset.Set( dAtof(argv[4]), dAtof(argv[5]) );
        nextArg = 6;
    }
    else if ( linearOffsetElementCount == 2 )
    {
        linearOffset = Utility::mGetStringElementVector(argv[4]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::createMotorJoint() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset );
    }

    // Fetch angular offset.
    const F32 angularOffset = mDegToRad( dAtof(argv[nextArg++]) );

    if ( argc <= nextArg )
    {
        return object->createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset, angularOffset );
    }

    // Fetch maximum force.
    const F32 maxForce = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset, angularOffset, maxForce );
    }

    // Fetch maximum torque.
    const F32 maxTorque = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset, angularOffset, maxForce, maxTorque );
    }

    // Fetch collide connected.
    const bool collideConnected = dAtob(argv[nextArg++]);

    return object->createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset, angularOffset, maxForce, maxTorque, collideConnected );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setMotorJointLinearOffset, void, 4, 5,     "(jointId, linearOffset X/Y) Sets the linear offset in sceneObjectA space.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@param linearOffsetX/Y The linear offset in sceneObjectA space.\n"
                                                                        "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Linear offset.
    const U32 linearOffsetElementCount = Utility::mGetStringElementCount(argv[3]);

    b2Vec2 linearOffset;

    if ( linearOffsetElementCount == 1 && argc > 5 )
    {
        linearOffset.Set( dAtof(argv[3]), dAtof(argv[4]) );
    }
    else if ( linearOffsetElementCount == 2 )
    {
        linearOffset = Utility::mGetStringElementVector(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("Scene::setMotorJointLinearOffset() - Invalid number of parameters!");
    }

    // Access joint.
    object->setMotorJointLinearOffset( jointId, linearOffset );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getMotorJointLinearOffset, const char*, 3, 3,  "(jointId) Gets the linear offset in sceneObjectA space.\n"
                                                                            "@param jointId The Id of the joint to use.\n"
                                                                            "@return Returns the linear offset in sceneObjectA space (always 0,0 if error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    const Vector2 linearOffset = object->getMotorJointLinearOffset( jointId );

    return linearOffset.scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setMotorJointAngularOffset, void, 4, 4,    "(jointId, angularOffset) Sets the angularOffset between the bodies.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@param angularOffset The angularOffset between the bodies.\n"
                                                                        "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 angularOffset = mDegToRad(dAtof(argv[3]));

    // Access joint.
    object->setMotorJointAngularOffset( jointId, angularOffset );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getMotorJointAngularOffset, F32, 3, 3,     "(jointId) Gets angularOffset between the bodies.\n"
                                                                        "@param jointId The Id of the joint to use.\n"
                                                                        "@return Returns the angularOffset between the bodies (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return mRadToDeg( object->getMotorJointAngularOffset( jointId ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setMotorJointMaxForce, void, 4, 4,     "(jointId, maxForce) Sets the maximum motor force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param maxForce The maximum motor force (N).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 maxForce = dAtof(argv[3]);

    // Access joint.
    object->setMotorJointMaxForce( jointId, maxForce );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getMotorJointMaxForce, F32, 3, 3,   "(jointId) Sets the maximum motor force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the maximum motor force (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getMotorJointMaxForce( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setMotorJointMaxTorque, void, 4, 4, "(jointId, maxTorque) Sets the maximum motor torque force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@param maxTorque The maximum motor torque force (N).\n"
                                                                    "@return Returns no value." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Fetch args.
    const F32 maxTorque = dAtof(argv[3]);

    // Access joint.
    object->setMotorJointMaxTorque( jointId, maxTorque );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getMotorJointMaxTorque, F32, 3, 3,  "(jointId) Gets the maximum motor torque force.\n"
                                                                    "@param jointId The Id of the joint to use.\n"
                                                                    "@return Returns the maximum motor torque force (-1 indicates error)." )
{
    // Fetch joint Id.
    const S32 jointId = dAtoi(argv[2]);

    // Access joint.
    return object->getMotorJointMaxTorque( jointId );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, pickArea, const char*, 4, 9, "(startx/y, endx/y, [sceneGroupMask], [sceneLayerMask], [pickMode] ) Picks objects intersecting the specified area with optional group/layer masks.\n"
              "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
              "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
              "@param sceneGroupMask Optional scene group mask.  (-1) or empty string selects all groups.\n"
              "@param sceneLayerMask Optional scene layer mask.  (-1) or empty string selects all layers.\n"
              "@param pickMode Optional mode 'any', 'aabb', 'oobb' or 'collision' (default is 'oobb').\n"
              "@return Returns list of object IDs.")
{
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
        Con::warnf("Scene::pickArea() - Invalid number of parameters!");
        return NULL;
    }

    // Calculate scene group mask.
    U32 sceneGroupMask = MASK_ALL;
    if ( (U32)argc > firstArg )
    {
        if ( *argv[firstArg] != 0 )
            sceneGroupMask = dAtoi(argv[firstArg]);
    }

    // Calculate scene layer mask.
    U32 sceneLayerMask = MASK_ALL;
    if ( (U32)argc > (firstArg + 1) )
    {
        if ( *argv[firstArg + 1] != 0 )
            sceneLayerMask = dAtoi(argv[firstArg + 1]);
    }

    // Calculate pick mode.
    Scene::PickMode pickMode = Scene::PICK_OOBB;
    if ( (U32)argc > (firstArg + 2))
    {
        pickMode = Scene::getPickModeEnum(argv[firstArg + 2]);
    }
    if ( pickMode == Scene::PICK_INVALID )
    {
        Con::warnf("Scene::pickArea() - Invalid pick mode of %s", argv[firstArg + 2]);
        pickMode = Scene::PICK_OOBB;
    }


    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = object->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( sceneLayerMask, sceneGroupMask, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Calculate normalized AABB.
    b2AABB aabb;
    aabb.lowerBound.x = getMin( v1.x, v2.x );
    aabb.lowerBound.y = getMin( v1.y, v2.y );
    aabb.upperBound.x = getMax( v1.x, v2.x );
    aabb.upperBound.y = getMax( v1.y, v2.y );

    // Perform query.
    if ( pickMode == Scene::PICK_ANY )
    {
        pWorldQuery->anyQueryAABB( aabb );    
    }
    else if ( pickMode == Scene::PICK_AABB )
    {
        pWorldQuery->aabbQueryAABB( aabb );    
    }
    else if ( pickMode == Scene::PICK_OOBB )
    {
        pWorldQuery->oobbQueryAABB( aabb );    
    }
    else if ( pickMode == Scene::PICK_COLLISION )
    {
        pWorldQuery->collisionQueryAABB( aabb );    
    }
    else
    {
        AssertFatal( false, "Unsupported pick mode." );
    }

    // Fetch result count.
    const U32 resultCount = pWorldQuery->getQueryResultsCount();

    // Finish if no results.
    if ( resultCount == 0 )
        return NULL;

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

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
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSceneObject->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("Scene::pickArea() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear world query.
    pWorldQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, pickRay, const char*, 4, 9, "(startx/y, endx/y, [sceneGroupMask], [sceneLayerMask], [pickMode] ) Picks objects intersecting the specified ray with optional group/layer masks.\n"
              "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
              "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
              "@param sceneGroupMask Optional scene group mask.  (-1) or empty string selects all groups.\n"
              "@param sceneLayerMask Optional scene layer mask.  (-1) or empty string selects all layers.\n"
              "@param pickMode Optional mode 'any', 'aabb', 'oobb' or 'collision' (default is 'oobb').\n"
              "@return Returns list of object IDs.")
{
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
        Con::warnf("Scene::pickRay() - Invalid number of parameters!");
        return NULL;
    }

    // Calculate scene group mask.
    U32 sceneGroupMask = MASK_ALL;
    if ( (U32)argc > firstArg )
    {
        if ( *argv[firstArg] != 0 )
            sceneGroupMask = dAtoi(argv[firstArg]);
    }

    // Calculate scene layer mask.
    U32 sceneLayerMask = MASK_ALL;
    if ( (U32)argc > (firstArg + 1) )
    {
        if ( *argv[firstArg + 1] != 0 )
            sceneLayerMask = dAtoi(argv[firstArg + 1]);
    }

    // Calculate pick mode.
    Scene::PickMode pickMode = Scene::PICK_OOBB;
    if ( (U32)argc > (firstArg + 2))
    {
        pickMode = Scene::getPickModeEnum(argv[firstArg + 2]);
    }
    if ( pickMode == Scene::PICK_INVALID )
    {
        Con::warnf("Scene::pickRay() - Invalid pick mode of %s", argv[firstArg + 2]);
        pickMode = Scene::PICK_OOBB;
    }


    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = object->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( sceneLayerMask, sceneGroupMask, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Perform query.
    if ( pickMode == Scene::PICK_ANY )
    {
        pWorldQuery->anyQueryRay( v1, v2 );    
    }
    else if ( pickMode == Scene::PICK_AABB )
    {
        pWorldQuery->aabbQueryRay( v1, v2 );    
    }
    else if ( pickMode == Scene::PICK_OOBB )
    {
        pWorldQuery->oobbQueryRay( v1, v2 );    
    }
    else if ( pickMode == Scene::PICK_COLLISION )
    {
        pWorldQuery->collisionQueryRay( v1, v2 );    
    }
    else
    {
        AssertFatal( false, "Unsupported pick mode." );
    }

    // Sanity!
    AssertFatal( pWorldQuery->getIsRaycastQueryResult(), "Invalid non-ray-cast query result returned." );

    // Fetch result count.
    const U32 resultCount = pWorldQuery->getQueryResultsCount();

    // Finish if no results.
    if ( resultCount == 0 )
        return NULL;

    // Sort ray-cast result.
    pWorldQuery->sortRaycastQueryResult();

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

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
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSceneObject->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("Scene::pickRay() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear world query.
    pWorldQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, pickPoint, const char*, 3, 7, "(x / y, [sceneGroupMask], [sceneLayerMask], [pickMode] ) Picks objects intersecting the specified point with optional group/layer masks.\n"
              "@param x/y The coordinate of the point as either (\"x y\") or (x,y)\n"
              "@param sceneGroupMask Optional scene group mask.  (-1) or empty string selects all groups.\n"
              "@param sceneLayerMask Optional scene layer mask.  (-1) or empty string selects all layers.\n"
              "@param pickMode Optional mode 'any', 'aabb', 'oobb' or 'collision' (default is 'ooabb').\n"
              "@return Returns list of object IDs.")
{
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
        Con::warnf("Scene::pickPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Calculate scene group mask.
    U32 sceneGroupMask = MASK_ALL;
    if ( (U32)argc > firstArg )
    {
        if ( *argv[firstArg] != 0 )
            sceneGroupMask = dAtoi(argv[firstArg]);
    }

    // Calculate scene layer mask.
    U32 sceneLayerMask = MASK_ALL;
    if ( (U32)argc > (firstArg + 1) )
    {
        if ( *argv[firstArg + 1] != 0 )
            sceneLayerMask = dAtoi(argv[firstArg + 1]);
    }

    // Calculate pick mode.
    Scene::PickMode pickMode = Scene::PICK_OOBB;
    if ( (U32)argc > (firstArg + 2 ))
    {
        pickMode = Scene::getPickModeEnum(argv[firstArg + 2]);
    }
    if ( pickMode == Scene::PICK_INVALID )
    {
        Con::warnf("Scene::pickPoint() - Invalid pick mode of %s", argv[firstArg + 2]);
        pickMode = Scene::PICK_OOBB;
    }


    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = object->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( sceneLayerMask, sceneGroupMask, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Perform query.
    if ( pickMode == Scene::PICK_ANY )
    {
        pWorldQuery->anyQueryPoint( point );    
    }
    else if ( pickMode == Scene::PICK_AABB )
    {
        pWorldQuery->aabbQueryPoint( point );    
    }
    else if ( pickMode == Scene::PICK_OOBB )
    {
        pWorldQuery->oobbQueryPoint( point );    
    }
    else if ( pickMode == Scene::PICK_COLLISION )
    {
        pWorldQuery->collisionQueryPoint( point );    
    }
    else
    {
        AssertFatal( false, "Unsupported pick mode." );
    }

    // Fetch result count.
    const U32 resultCount = pWorldQuery->getQueryResultsCount();

    // Finish if no results.
    if ( resultCount == 0 )
        return NULL;

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

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
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSceneObject->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("Scene::pickPoint() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear world query.
    pWorldQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, pickCircle, const char*, 4, 8, "(x / y, radius, [sceneGroupMask], [sceneLayerMask], [pickMode] ) Picks objects intersecting the specified circle with optional group/layer masks.\n"
              "@param x/y The coordinate of the point as either (\"x y\") or (x,y)\n"
              "@param radius The radius of the circle.\n"
              "@param sceneGroupMask Optional scene group mask.  (-1) or empty string selects all groups.\n"
              "@param sceneLayerMask Optional scene layer mask.  (-1) or empty string selects all layers.\n"
              "@param pickMode Optional mode 'any', 'aabb', 'oobb' or 'collision' (default is 'ooabb').\n"
              "@return Returns list of object IDs.")
{
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
        Con::warnf("Scene::pickPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch radius.
    const F32 radius = dAtof(argv[firstArg++]);

    // Check radius.
    if ( radius <= 0.0f )
    {
        Con::warnf( "Scene::pickCircle()  Radius must be greater than zero." );
        return StringTable->EmptyString;
    }

    // Calculate scene group mask.
    U32 sceneGroupMask = MASK_ALL;
    if ( (U32)argc > firstArg )
    {
        if ( *argv[firstArg] != 0 )
            sceneGroupMask = dAtoi(argv[firstArg]);
    }

    // Calculate scene layer mask.
    U32 sceneLayerMask = MASK_ALL;
    if ( (U32)argc > (firstArg + 1) )
    {
        if ( *argv[firstArg + 1] != 0 )
            sceneLayerMask = dAtoi(argv[firstArg + 1]);
    }

    // Calculate pick mode.
    Scene::PickMode pickMode = Scene::PICK_OOBB;
    if ( (U32)argc > (firstArg + 2 ))
    {
        pickMode = Scene::getPickModeEnum(argv[firstArg + 2]);
    }
    if ( pickMode == Scene::PICK_INVALID )
    {
        Con::warnf("Scene::pickPoint() - Invalid pick mode of %s", argv[firstArg + 2]);
        pickMode = Scene::PICK_OOBB;
    }


    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = object->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( sceneLayerMask, sceneGroupMask, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Perform query.
    if ( pickMode == Scene::PICK_ANY )
    {
        pWorldQuery->anyQueryCircle( point, radius );    
    }
    else if ( pickMode == Scene::PICK_AABB )
    {
        pWorldQuery->aabbQueryCircle( point, radius );    
    }
    else if ( pickMode == Scene::PICK_OOBB )
    {
        pWorldQuery->oobbQueryCircle( point, radius );    
    }
    else if ( pickMode == Scene::PICK_COLLISION )
    {
        pWorldQuery->collisionQueryCircle( point, radius );    
    }
    else
    {
        AssertFatal( false, "Unsupported pick mode." );
    }

    // Fetch result count.
    const U32 resultCount = pWorldQuery->getQueryResultsCount();

    // Finish if no results.
    if ( resultCount == 0 )
        return NULL;

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

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
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSceneObject->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("Scene::pickPoint() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear world query.
    pWorldQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------

ConsoleMethod(Scene, pickRayCollision, const char*, 4, 8, "(startx/y, endx/y, [sceneGroupMask], [sceneLayerMask] ) Picks objects with collision shapes intersecting the specified ray with optional group/layer masks.\n"
                "Unlike other pick methods, this returns the complete detail for each object encountered, returning the collision point, normal and fraction of the ray intersection.\n"
                "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
                "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
                "@param sceneGroupMask Optional scene group mask.  (-1) or empty string selects all groups.\n"
                "@param sceneLayerMask Optional scene layer mask.  (-1) or empty string selects all layers.\n"
                "@return Returns a list of objects in blocks of detail items where each block represents a single object and its collision detail in the format:"
                "<ObjectId PointX PointY NormalX NormalY RayFraction ShapeIndex> <ObjectId PointX PointY NormalX NormalY RayFraction ShapeIndex> <ObjectId PointX PointY NormalX NormalY RayFraction ShapeIndex> etc.\n")
{
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
    else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 9))
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
        Con::warnf("Scene::pickRayCollision() - Invalid number of parameters!");
        return NULL;
    }

    // Calculate scene group mask.
    U32 sceneGroupMask = MASK_ALL;
    if ( (U32)argc > firstArg )
    {
        if ( *argv[firstArg] != 0 )
            sceneGroupMask = dAtoi(argv[firstArg]);
    }

    // Calculate scene layer mask.
    U32 sceneLayerMask = MASK_ALL;
    if ( (U32)argc > (firstArg + 1) )
    {
        if ( *argv[firstArg + 1] != 0 )
            sceneLayerMask = dAtoi(argv[firstArg + 1]);
    }

    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = object->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( sceneLayerMask, sceneGroupMask, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Perform query.
    pWorldQuery->collisionQueryRay( v1, v2 );    

    // Sanity!
    AssertFatal( pWorldQuery->getIsRaycastQueryResult(), "Invalid non-ray-cast query result returned." );

    // Fetch result count.
    const U32 resultCount = pWorldQuery->getQueryResultsCount();

    // Finish if no results.
    if ( resultCount == 0 )
        return NULL;

    // Sort ray-cast result.
    pWorldQuery->sortRaycastQueryResult();

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Fetch query result.
        const WorldQueryResult& queryResult = queryResults[n];

        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d %g %g %g %g %g %d ",
            queryResult.mpSceneObject->getId(),
            queryResult.mPoint.x, queryResult.mPoint.y,
            queryResult.mNormal.x, queryResult.mNormal.y,
            queryResult.mFraction,
            queryResult.mShapeIndex );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("Scene::pickRayCollision() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear world query.
    pWorldQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setDebugOn, void, 3, 2 + DEBUG_MODE_COUNT, "(debugOptions) Sets Debug option(s) on.\n"
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
        Con::warnf( "Scene::setDebugOn() - Invalid number of parameters!" );
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
                Con::warnf( "Scene::setDebugOn() - Invalid debug option '%s' specified.", pDebugOption );
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
                Con::warnf( "Scene::setDebugOn() - Invalid debug option '%s' specified.", pDebugOption );
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

ConsoleMethod(Scene, setDebugOff, void, 3, 2 + DEBUG_MODE_COUNT,    "(debugOptions) Sets Debug options(s) off.\n"
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
        Con::warnf( "Scene::setDebugOff() - Invalid number of parameters!" );
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
                Con::warnf( "Scene::setDebugOff() - Invalid debug option '%s' specified.", pDebugOption );
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
                Con::warnf( "Scene::setDebugOff() - Invalid debug option '%s' specified.", pDebugOption );
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

ConsoleMethod(Scene, getDebugOn, const char*, 2, 2, "() Gets the state of the debug modes.\n"
                                                    "@return Returns a space separated list of debug modes that are active.")
{
    // Fetch debug mask,.
    const U32 debugMask = object->getDebugMask();

    // Fetch a return buffer.
    S32 bufferSize = 1024;
    char* pReturnBuffer = Con::getReturnBuffer(bufferSize);
    *pReturnBuffer = 0;
    char* pWriteCursor = pReturnBuffer;

    // Iterate debug mask.
    for( U32 bit = 0; bit < 32; ++bit )
    {
        // Calculate debug mask bit.
        const S32 debugBit = 1 << bit;
        if ( (debugMask & debugBit) == 0 )
            continue;

        // Format option.
        const S32 size = dSprintf( pWriteCursor, bufferSize, "%s ", object->getDebugOptionDescription( (Scene::DebugOption)debugBit ) );
        bufferSize -= size;
        pWriteCursor += size;
    }

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setDebugSceneObject, void, 3, 3,   "(sceneObject) Sets the scene object to monitor in the debug metrics.\n"
                                                                "@param SceneObject The scene object to monitor in the debug metrics.\n"
                                                                "@return No return value.\n")
{
    // Fetch scene object Id.
    const SimObjectId sceneObjectId = dAtoi(argv[2]);

    // Find scene object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( Sim::findObject(sceneObjectId) );

    // Sanity!
    if ( !pSceneObject )
    {
        Con::warnf("Scene::setDebugSceneObject() - Could not find scene object %d.", sceneObjectId);
        return;
    }

    // Set debug object.
    object->setDebugSceneObject( pSceneObject );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getDebugSceneObject, S32, 2, 2,   "( Gets the scene object being monitored in the debug metrics.\n"
                                                                "@return The scene object being monitored in the debug metrics (zero if none being monitored).\n")
{
    // Fetch scene object.
    SceneObject* pSceneObject = object->getDebugSceneObject();

    if ( pSceneObject == NULL )
        return 0;

    return pSceneObject->getId();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setLayerSortMode, void, 4, 4,  "(layer, sortMode) Sets the layer to use the specified render sort mode.\n"
                                                    "@param layer The layer to modify.\n"
                                                    "@param sortMode The sort mode to use on the specified layer.\n"
                                                    "@return No return value." )
{
    // Fetch the layer.
    const U32 layer = dAtoi(argv[2]);

    // Fetch the sort mode.
    const SceneRenderQueue::RenderSort sortMode = SceneRenderQueue::getRenderSortEnum( argv[3] );

    object->setLayerSortMode( layer, sortMode );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getLayerSortMode, const char*, 3, 3,   "(layer) Gets the render sort mode for the specified layer.\n"
                                                            "@param layer The layer to retrieve.\n"
                                                            "@return The render sort mode for the specified layer." )
{
    // Fetch the layer.
    const U32 layer = dAtoi(argv[2]);

    // Fetch the sort mode.
    return SceneRenderQueue::getRenderSortDescription( object->getLayerSortMode( layer ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, resetDebugStats, void, 2, 2,   "() Resets the debug statistics.\n"
                                                            "@return No return value." )
{
    object->resetDebugStats();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getFPS, F32, 2, 2,  "() Gets the current average frames-per-second.\n"
                                                    "@return The current average frames-per-second.")
{
    return object->getDebugStats().fps;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getMinFPS, F32, 2, 2,  "() Gets the minimum average frames-per-second.\n"
                                                    "@return The minimum Faverage frames-per-second.")
{
    return object->getDebugStats().minFPS;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getMaxFPS, F32, 2, 2,  "() Gets the maximum average frames-per-second.\n"
                                                    "@return The average frames-per-second.")
{
    return object->getDebugStats().maxFPS;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getFrameCount, S32, 2, 2,  "() Gets the current rendered frame count.\n"
                                                    "@return The current rendered frame count.")
{
    return (S32)object->getDebugStats().frameCount;
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setBatchingEnabled, void, 3, 3,    "( bool enabled ) Sets whether render batching is enabled or not.\n"
                                                                "@param enabled Whether render batching is enabled or not.\n"
                                                                "return No return value.\n" )
{
    // Fetch args.
    const bool enabled = dAtob(argv[2]);

    // Sets batching enabled.
    object->setBatchingEnabled( enabled );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, getBatchingEnabled, bool, 2, 2,    "() Gets whether render batching is enabled or not.\n"
                                                                "return Whether render batching is enabled or not.\n" )
{
    // Gets batching enabled.
    return object->getBatchingEnabled();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, setIsEditorScene, void, 3, 3, "() Sets whether this is an editor scene.\n"
                                                            "@return No return value.")
{
   object->setIsEditorScene(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod(Scene, create, const char*, 3, 3, "(type) Creates the specified scene-object derived type and adds it to the scene.\n"
                                                "@return The scene-object or NULL if not created.")
{
    // Create the scene object.
    SceneObject* pSceneObject = object->create( argv[2] );

    return pSceneObject == NULL ? NULL : pSceneObject->getIdString();
}

