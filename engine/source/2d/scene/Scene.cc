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

#ifndef _SCENE_H_
#include "Scene.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _BITSTREAM_H_
#include "io/bitStream.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _CONTACT_FILTER_H_
#include "ContactFilter.h"
#endif

#ifndef _SCENE_RENDER_OBJECT_H_
#include "2d/SceneRenderObject.h"
#endif

#ifndef _PARTICLE_SYSTEM_H_
#include "2d/core/particleSystem.h"
#endif

// Script bindings.
#include "Scene_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

SimObjectPtr<Scene> Scene::LoadingScene = NULL;

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(Scene);

//------------------------------------------------------------------------------

static ContactFilter mContactFilter;

// Scene counter.
static U32 sSceneCount = 0;
static U32 sSceneMasterIndex = 0;

// Joint property names..
static bool jointPropertiesInitialized = false;

static StringTableEntry jointCustomPropertyName;
static StringTableEntry jointCollideConnectedName;
static StringTableEntry jointObjectAName;
static StringTableEntry jointObjectBName;
static StringTableEntry jointLocalAnchorAName;
static StringTableEntry jointLocalAnchorBName;

static StringTableEntry jointDistanceTypeName;
static StringTableEntry jointDistanceLengthName;
static StringTableEntry jointDistanceFrequencyName;
static StringTableEntry jointDistanceDampingRatioName;

static StringTableEntry jointRopeTypeName;
static StringTableEntry jointRopeMaxLengthName;

static StringTableEntry jointRevoluteTypeName;
static StringTableEntry jointRevoluteLimitLowerAngleName;
static StringTableEntry jointRevoluteLimitUpperAngleName;
static StringTableEntry jointRevoluteMotorSpeedName;
static StringTableEntry jointRevoluteMotorMaxTorqueName;

static StringTableEntry jointWeldTypeName;
static StringTableEntry jointWeldFrequencyName;
static StringTableEntry jointWeldDampingRatioName;

static StringTableEntry jointWheelTypeName;
static StringTableEntry jointWheelWorldAxisName;
static StringTableEntry jointWheelMotorSpeedName;
static StringTableEntry jointWheelMotorMaxTorqueName;
static StringTableEntry jointWheelFrequencyName;
static StringTableEntry jointWheelDampingRatioName;

static StringTableEntry jointFrictionTypeName;
static StringTableEntry jointFrictionMaxForceName;
static StringTableEntry jointFrictionMaxTorqueName;

static StringTableEntry jointPrismaticTypeName;
static StringTableEntry jointPrismaticWorldAxisName;
static StringTableEntry jointPrismaticLimitLowerTransName;
static StringTableEntry jointPrismaticLimitUpperTransName;
static StringTableEntry jointPrismaticMotorSpeedName;
static StringTableEntry jointPrismaticMotorMaxForceName;

static StringTableEntry jointPulleyTypeName;
static StringTableEntry jointPulleyGroundAnchorAName;
static StringTableEntry jointPulleyGroundAnchorBName;
static StringTableEntry jointPulleyLengthAName;
static StringTableEntry jointPulleyLengthBName;
static StringTableEntry jointPulleyRatioName;

static StringTableEntry jointTargetTypeName;
static StringTableEntry jointTargetWorldTargetName;
static StringTableEntry jointTargetMaxForceName;
static StringTableEntry jointTargetFrequencyName;
static StringTableEntry jointTargetDampingRatioName;

static StringTableEntry jointMotorTypeName;
static StringTableEntry jointMotorLinearOffsetName;
static StringTableEntry jointMotorAngularOffsetName;
static StringTableEntry jointMotorMaxForceName;
static StringTableEntry jointMotorMaxTorqueName;
static StringTableEntry jointMotorCorrectionFactorName;

//-----------------------------------------------------------------------------

Scene::Scene() :
    /// World.
    mpWorld(NULL),
    mWorldGravity(0.0f, 0.0f),
    mVelocityIterations(8),
    mPositionIterations(3),

    /// Joint access.
    mJointMasterId(1),

    /// Scene time.
    mSceneTime(0.0f),
    mScenePause(false),

    /// Debug and metrics.
    mDebugMask(0X00000000),
    mpDebugSceneObject(NULL),

    /// Window rendering.
    mpCurrentRenderWindow(NULL),
    
    /// Background color.
    mBackgroundColor( 0.0f, 0.0f, 0.0f, 0.0f ),
    mUseBackgroundColor(false),   

    /// Miscellaneous.
    mIsEditorScene(0),
    mUpdateCallback(false),
    mRenderCallback(false),
    mSceneIndex(0)
{
    // Initialize joint property names.
    if ( !jointPropertiesInitialized )
    {
        jointCustomPropertyName           = StringTable->insert( "Joints" );
        jointCollideConnectedName         = StringTable->insert( "CollideConnected" );
        jointObjectAName                  = StringTable->insert( "AnchorA" );
        jointObjectBName                  = StringTable->insert( "AnchorB" );
        jointLocalAnchorAName             = StringTable->insert( "LocalAnchorA" );
        jointLocalAnchorBName             = StringTable->insert( "LocalAnchorB" );

        jointDistanceTypeName             = StringTable->insert( "Distance" );
        jointDistanceLengthName           = StringTable->insert( "Length" );
        jointDistanceFrequencyName        = StringTable->insert( "Frequency" );
        jointDistanceDampingRatioName     = StringTable->insert( "DampingRatio" );

        jointRopeTypeName                 = StringTable->insert( "Rope" );
        jointRopeMaxLengthName            = StringTable->insert( "MaxLength" );

        jointRevoluteTypeName             = StringTable->insert( "Revolute" );
        jointRevoluteLimitLowerAngleName  = StringTable->insert( "LowerAngle" );
        jointRevoluteLimitUpperAngleName  = StringTable->insert( "UpperAngle" );
        jointRevoluteMotorSpeedName       = StringTable->insert( "MotorSpeed" );
        jointRevoluteMotorMaxTorqueName   = StringTable->insert( "MaxTorque" );

        jointWeldTypeName                 = StringTable->insert( "Weld" );
        jointWeldFrequencyName            = jointDistanceFrequencyName;
        jointWeldDampingRatioName         = jointDistanceDampingRatioName;

        jointWheelTypeName                = StringTable->insert( "Wheel" );
        jointWheelWorldAxisName           = StringTable->insert( "WorldAxis" );
        jointWheelMotorSpeedName          = StringTable->insert( "MotorSpeed" );
        jointWheelMotorMaxTorqueName      = jointRevoluteMotorMaxTorqueName;
        jointWheelFrequencyName           = jointDistanceFrequencyName;
        jointWheelDampingRatioName        = jointDistanceDampingRatioName;

        jointFrictionTypeName             = StringTable->insert( "Friction" );
        jointFrictionMaxForceName         = StringTable->insert( "MaxForce" );
        jointFrictionMaxTorqueName        = jointRevoluteMotorMaxTorqueName;

        jointPrismaticTypeName            = StringTable->insert( "Prismatic" );
        jointPrismaticWorldAxisName       = jointWheelWorldAxisName;
        jointPrismaticLimitLowerTransName = StringTable->insert( "LowerTranslation" );
        jointPrismaticLimitUpperTransName = StringTable->insert( "UpperTranslation" );
        jointPrismaticMotorSpeedName      = jointRevoluteMotorSpeedName;
        jointPrismaticMotorMaxForceName   = jointFrictionMaxForceName;

        jointPulleyTypeName               = StringTable->insert( "Pulley" );
        jointPulleyGroundAnchorAName      = StringTable->insert( "GroundAnchorA" );
        jointPulleyGroundAnchorBName      = StringTable->insert( "GroundAnchorB" );
        jointPulleyLengthAName            = StringTable->insert( "LengthA" );
        jointPulleyLengthBName            = StringTable->insert( "LengthB" );
        jointPulleyRatioName              = StringTable->insert( "Ratio" );

        jointTargetTypeName               = StringTable->insert( "Target" );
        jointTargetWorldTargetName        = StringTable->insert( "WorldTarget" );
        jointTargetMaxForceName           = StringTable->insert( jointFrictionMaxForceName );
        jointTargetFrequencyName          = jointDistanceFrequencyName;
        jointTargetDampingRatioName       = jointDistanceDampingRatioName;

        jointMotorTypeName                = StringTable->insert( "Motor" );
        jointMotorLinearOffsetName        = StringTable->insert( "LinearOffset" );
        jointMotorAngularOffsetName       = StringTable->insert( "AngularOffset" );
        jointMotorMaxForceName            = jointFrictionMaxForceName;
        jointMotorMaxTorqueName           = jointRevoluteMotorMaxTorqueName;
        jointMotorCorrectionFactorName    = StringTable->insert( "CorrectionFactor" );

        // Flag as initialized.
        jointPropertiesInitialized = true;
    }

    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mSceneObjects );
    VECTOR_SET_ASSOCIATION( mDeleteRequests );
    VECTOR_SET_ASSOCIATION( mDeleteRequestsTemp );
    VECTOR_SET_ASSOCIATION( mEndContacts );
      
    // Initialize layer sort mode.
    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; ++n )
       mLayerSortModes[n] = SceneRenderQueue::RENDER_SORT_NEWEST;

    // Set debug stats for batch renderer.
    mBatchRenderer.setDebugStats( &mDebugStats );

    // Assign scene index.    
    mSceneIndex = ++sSceneMasterIndex;
    sSceneCount++;

    mNSLinkMask = LinkSuperClassName | LinkClassName;
}

//-----------------------------------------------------------------------------

Scene::~Scene()
{
    // Decrease scene count.
    --sSceneCount;
}

//-----------------------------------------------------------------------------

bool Scene::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Synchronize Namespace's
    linkNamespaces();

    // Create physics world.
    mpWorld = new b2World( mWorldGravity );

    // Set contact filter.
    mpWorld->SetContactFilter( &mContactFilter );

    // Set contact listener.
    mpWorld->SetContactListener( this );

    // Set destruction listener.
    mpWorld->SetDestructionListener( this );

    // Create ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.userData = static_cast<PhysicsProxy*>(this);
    mpGroundBody = mpWorld->CreateBody(&groundBodyDef);
    mpGroundBody->SetAwake( false );

    // Create world query.
    mpWorldQuery = new WorldQuery(this);

    // Set loading scene.
    Scene::LoadingScene = this;

    // Tell the scripts
    Con::executef(this, 1, "onAdd");

    // Turn-on tick processing.
    setProcessTicks( true );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void Scene::onRemove()
{
    // Turn-off tick processing.
    setProcessTicks( false );

    // tell the scripts
    Con::executef(this, 1, "onRemove");

    // Clear Scene.
    clearScene();

    // Process Delete Requests.
    processDeleteRequests(true);

    // Delete ground body.
    mpWorld->DestroyBody( mpGroundBody );
    mpGroundBody = NULL;

    // Delete physics world and world query.
    delete mpWorldQuery;
    delete mpWorld;
    mpWorldQuery = NULL;
    mpWorld = NULL;

    // Detach All Scene Windows.
    detachAllSceneWindows();

    // Call Parent. Clear scene handles all the object removal, so we can skip
    // that part and just do the sim-object stuff.
    SimObject::onRemove();

    // Restore NameSpace's
    unlinkNamespaces();
}

//-----------------------------------------------------------------------------

void Scene::onDeleteNotify( SimObject* object )
{
    // Ignore if we're not monitoring a debug banner scene object.
    if ( mpDebugSceneObject == NULL )
        return;

    // Ignore if it's not the one we're monitoring.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( object );
    if ( pSceneObject != mpDebugSceneObject )
        return;

    // Reset the monitored scene object.
    mpDebugSceneObject = NULL;
}

//-----------------------------------------------------------------------------

void Scene::initPersistFields()
{
    // Call Parent.
    Parent::initPersistFields();

    // Physics.
    addProtectedField("Gravity", TypeT2DVector, Offset(mWorldGravity, Scene), &setGravity, &getGravity, &writeGravity, "" );
    addField("VelocityIterations", TypeS32, Offset(mVelocityIterations, Scene), &writeVelocityIterations, "" );
    addField("PositionIterations", TypeS32, Offset(mPositionIterations, Scene), &writePositionIterations, "" );

    // Background color.
    addField("UseBackgroundColor", TypeBool, Offset(mUseBackgroundColor, Scene), &writeUseBackgroundColor, "" );
    addField("BackgroundColor", TypeColorF, Offset(mBackgroundColor, Scene), &writeBackgroundColor, "" );

    // Layer sort modes.
    char buffer[64];
    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; n++ )
    {
       dSprintf( buffer, 64, "layerSortMode%d", n );
       addField( buffer, TypeEnum, OffsetNonConst(mLayerSortModes[n], Scene), &writeLayerSortMode, 1, &SceneRenderQueue::renderSortTable, "");
    }

    // Callbacks.
    addField("UpdateCallback", TypeBool, Offset(mUpdateCallback, Scene), &writeUpdateCallback, "");
    addField("RenderCallback", TypeBool, Offset(mRenderCallback, Scene), &writeRenderCallback, "");
}

//-----------------------------------------------------------------------------

void Scene::BeginContact( b2Contact* pContact )
{
    // Ignore contact if it's not a touching contact.
    if ( !pContact->IsTouching() )
        return;

    PROFILE_SCOPE(Scene_BeginContact);

    // Fetch fixtures.
    b2Fixture* pFixtureA = pContact->GetFixtureA();
    b2Fixture* pFixtureB = pContact->GetFixtureB();

    // Fetch physics proxies.
    PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pFixtureA->GetBody()->GetUserData());
    PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pFixtureB->GetBody()->GetUserData());

    // Ignore stuff that's not a scene object.
    if (    pPhysicsProxyA->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ||
            pPhysicsProxyB->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
    {
            return;
    }

    // Fetch scene objects.
    SceneObject* pSceneObjectA = static_cast<SceneObject*>(pPhysicsProxyA);
    SceneObject* pSceneObjectB = static_cast<SceneObject*>(pPhysicsProxyB);

    // Initialize the contact.
    TickContact tickContact;
    tickContact.initialize( pContact, pSceneObjectA, pSceneObjectB, pFixtureA, pFixtureB );

    // Add contact.
    mBeginContacts.insert( pContact, tickContact );
}

//-----------------------------------------------------------------------------

void Scene::EndContact( b2Contact* pContact )
{
    PROFILE_SCOPE(Scene_EndContact);

    // Fetch fixtures.
    b2Fixture* pFixtureA = pContact->GetFixtureA();
    b2Fixture* pFixtureB = pContact->GetFixtureB();

    // Fetch physics proxies.
    PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pFixtureA->GetBody()->GetUserData());
    PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pFixtureB->GetBody()->GetUserData());

    // Ignore stuff that's not a scene object.
    if (    pPhysicsProxyA->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ||
            pPhysicsProxyB->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
    {
            return;
    }

    // Fetch scene objects.
    SceneObject* pSceneObjectA = static_cast<SceneObject*>(pPhysicsProxyA);
    SceneObject* pSceneObjectB = static_cast<SceneObject*>(pPhysicsProxyB);

    // Initialize the contact.
    TickContact tickContact;
    tickContact.initialize( pContact, pSceneObjectA, pSceneObjectB, pFixtureA, pFixtureB );

    // Add contact.
    mEndContacts.push_back( tickContact );
}

//-----------------------------------------------------------------------------

void Scene::PostSolve( b2Contact* pContact, const b2ContactImpulse* pImpulse )
{
    // Find contact mapping.
    typeContactHash::iterator contactItr = mBeginContacts.find( pContact );

    // Finish if we didn't find the contact.
    if ( contactItr == mBeginContacts.end() )
        return;

    // Fetch contact.
    TickContact& tickContact = contactItr->value;

    // Add the impulse.
    for ( U32 index = 0; index < b2_maxManifoldPoints; ++index )
    {
        tickContact.mNormalImpulses[index] += pImpulse->normalImpulses[index];
        tickContact.mTangentImpulses[index] += pImpulse->tangentImpulses[index];
    }
}

//-----------------------------------------------------------------------------

void Scene::forwardContacts( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_ForwardContacts);

    // Iterate end contacts.
    for( typeContactVector::iterator contactItr = mEndContacts.begin(); contactItr != mEndContacts.end(); ++contactItr )
    {
        // Fetch tick contact.
        TickContact& tickContact = *contactItr;

        // Inform the scene objects.
        tickContact.mpSceneObjectA->onEndCollision( tickContact );
        tickContact.mpSceneObjectB->onEndCollision( tickContact );
    }

    // Iterate begin contacts.
    for( typeContactHash::iterator contactItr = mBeginContacts.begin(); contactItr != mBeginContacts.end(); ++contactItr )
    {
        // Fetch tick contact.
        TickContact& tickContact = contactItr->value;

        // Inform the scene objects.
        tickContact.mpSceneObjectA->onBeginCollision( tickContact );
        tickContact.mpSceneObjectB->onBeginCollision( tickContact );
    }
}

//-----------------------------------------------------------------------------

void Scene::dispatchBeginContactCallbacks( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_DispatchBeginContactCallbacks);

    // Sanity!
    AssertFatal( b2_maxManifoldPoints == 2, "Scene::dispatchBeginContactCallbacks() - Invalid assumption about max manifold points." );

    // Fetch contact count.
    const U32 contactCount = mBeginContacts.size();

    // Finish if no contacts.
    if ( contactCount == 0 )
        return;

    // Finish if no collision method exists on scene.
    Namespace* pNamespace = getNamespace();
    if ( pNamespace != NULL && pNamespace->lookup( StringTable->insert( "onCollision" ) ) == NULL )
        return;

    // Iterate all contacts.
    for ( typeContactHash::iterator contactItr = mBeginContacts.begin(); contactItr != mBeginContacts.end(); ++contactItr )
    {
        // Fetch contact.
        const TickContact& tickContact = contactItr->value;

        // Fetch scene objects.
        SceneObject* pSceneObjectA = tickContact.mpSceneObjectA;
        SceneObject* pSceneObjectB = tickContact.mpSceneObjectB;

        // Skip if either object is being deleted.
        if ( pSceneObjectA->isBeingDeleted() || pSceneObjectB->isBeingDeleted() )
            continue;

        // Skip if either object does not have collision callback active.
        if ( !pSceneObjectA->getCollisionCallback() || !pSceneObjectB->getCollisionCallback() )
            continue;

        // Fetch normal and contact points.
        const U32& pointCount = tickContact.mPointCount;
        const b2Vec2& normal = tickContact.mWorldManifold.normal;
        const b2Vec2& point1 = tickContact.mWorldManifold.points[0];
        const b2Vec2& point2 = tickContact.mWorldManifold.points[1];
        const S32 shapeIndexA = pSceneObjectA->getCollisionShapeIndex( tickContact.mpFixtureA );
        const S32 shapeIndexB = pSceneObjectB->getCollisionShapeIndex( tickContact.mpFixtureB );

        // Sanity!
        AssertFatal( shapeIndexA >= 0, "Scene::dispatchBeginContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );
        AssertFatal( shapeIndexB >= 0, "Scene::dispatchBeginContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );

        // Fetch collision impulse information
        const F32 normalImpulse1 = tickContact.mNormalImpulses[0];
        const F32 normalImpulse2 = tickContact.mNormalImpulses[1];
        const F32 tangentImpulse1 = tickContact.mTangentImpulses[0];
        const F32 tangentImpulse2 = tickContact.mTangentImpulses[1];

        // Format objects.
        char* pSceneObjectABuffer = Con::getArgBuffer( 8 );
        char* pSceneObjectBuffer = Con::getArgBuffer( 8 );
        dSprintf( pSceneObjectABuffer, 8, "%d", pSceneObjectA->getId() );
        dSprintf( pSceneObjectBuffer, 8, "%d", pSceneObjectB->getId() );

        // Format miscellaneous information.
        char* pMiscInfoBuffer = Con::getArgBuffer(128);
        if ( pointCount == 2 )
        {
            dSprintf(pMiscInfoBuffer, 128,
                "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
                shapeIndexA, shapeIndexB,
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
            dSprintf(pMiscInfoBuffer, 128,
                "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
                shapeIndexA, shapeIndexB,
                normal.x, normal.y,
                point1.x, point1.y,
                normalImpulse1,
                tangentImpulse1 );
        }
		else
		{
            dSprintf(pMiscInfoBuffer, 64,
                "%d %d",
                shapeIndexA, shapeIndexB );
		}

        // Script callback.
        Con::executef( this, 4, "onCollision",
            pSceneObjectABuffer,
            pSceneObjectBuffer,
            pMiscInfoBuffer );
    }
}

//-----------------------------------------------------------------------------

void Scene::dispatchEndContactCallbacks( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_DispatchEndContactCallbacks);

    // Sanity!
    AssertFatal( b2_maxManifoldPoints == 2, "Scene::dispatchEndContactCallbacks() - Invalid assumption about max manifold points." );

    // Fetch contact count.
    const U32 contactCount = mEndContacts.size();

    // Finish if no contacts.
    if ( contactCount == 0 )
        return;

    // Finish if no collision method exists on scene.
    Namespace* pNamespace = getNamespace();
    if ( pNamespace != NULL && pNamespace->lookup( StringTable->insert( "onEndCollision" ) ) == NULL )
        return;

    // Iterate all contacts.
    for ( typeContactVector::iterator contactItr = mEndContacts.begin(); contactItr != mEndContacts.end(); ++contactItr )
    {
        // Fetch contact.
        const TickContact& tickContact = *contactItr;

        // Fetch scene objects.
        SceneObject* pSceneObjectA = tickContact.mpSceneObjectA;
        SceneObject* pSceneObjectB = tickContact.mpSceneObjectB;

        // Skip if either object is being deleted.
        if ( pSceneObjectA->isBeingDeleted() || pSceneObjectB->isBeingDeleted() )
            continue;

        // Skip if either object does not have collision callback active.
        if ( !pSceneObjectA->getCollisionCallback() || !pSceneObjectB->getCollisionCallback() )
            continue;

        // Fetch shape index.
        const S32 shapeIndexA = pSceneObjectA->getCollisionShapeIndex( tickContact.mpFixtureA );
        const S32 shapeIndexB = pSceneObjectB->getCollisionShapeIndex( tickContact.mpFixtureB );

        // Sanity!
        AssertFatal( shapeIndexA >= 0, "Scene::dispatchEndContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );
        AssertFatal( shapeIndexB >= 0, "Scene::dispatchEndContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );

        // Format objects.
        char* pSceneObjectABuffer = Con::getArgBuffer( 8 );
        char* pSceneObjectBBuffer = Con::getArgBuffer( 8 );
        dSprintf( pSceneObjectABuffer, 8, "%d", pSceneObjectA->getId() );
        dSprintf( pSceneObjectBBuffer, 8, "%d", pSceneObjectB->getId() );

        // Format miscellaneous information.
        char* pMiscInfoBuffer = Con::getArgBuffer(32);
        dSprintf(pMiscInfoBuffer, 32, "%d %d", shapeIndexA, shapeIndexB );

        // Script callback.
        Con::executef( this, 4, "onEndCollision",
            pSceneObjectABuffer,
            pSceneObjectBBuffer,
            pMiscInfoBuffer );
    }
}

//-----------------------------------------------------------------------------

void Scene::processTick( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_ProcessTick);

    // Finish if the Scene is not added to the simulation.
    if ( !isProperlyAdded() )
        return;

    // Process Delete Requests.
    processDeleteRequests(false);

    // Update debug stats.
    mDebugStats.fps           = Con::getFloatVariable("fps::framePeriod", 0.0f);
    mDebugStats.frameCount    = (U32)Con::getIntVariable("fps::frameCount", 0);
    mDebugStats.bodyCount     = (U32)mpWorld->GetBodyCount();
    mDebugStats.jointCount    = (U32)mpWorld->GetJointCount();
    mDebugStats.contactCount  = (U32)mpWorld->GetContactCount();
    mDebugStats.proxyCount    = (U32)mpWorld->GetProxyCount();
    mDebugStats.objectsCount  = (U32)mSceneObjects.size();
    mDebugStats.worldProfile  = mpWorld->GetProfile();

    // Set particle stats.
    mDebugStats.particlesAlloc = ParticleSystem::Instance->getAllocatedParticleCount();
    mDebugStats.particlesUsed = ParticleSystem::Instance->getActiveParticleCount();
    mDebugStats.particlesFree = mDebugStats.particlesAlloc - mDebugStats.particlesUsed;

    // Finish if scene is paused.
    if ( !getScenePause() )
    {
        // Reset object stats.
        U32 objectsEnabled = 0;
        U32 objectsVisible = 0;
        U32 objectsAwake   = 0;

        // Fetch if a "normal" i.e. non-editor scene.
        const bool isNormalScene = !getIsEditorScene();

        // Update scene time.
        mSceneTime += Tickable::smTickSec;

        // Clear ticked scene objects.
        mTickedSceneObjects.clear();

        // Iterate scene objects.
        for( S32 n = 0; n < mSceneObjects.size(); ++n )
        {
            // Fetch scene object.
            SceneObject* pSceneObject = mSceneObjects[n];

            // Update awake/asleep counts.
            if ( pSceneObject->getAwake() )
                objectsAwake++;

            // Update visible.
            if ( pSceneObject->getVisible() )
                objectsVisible++;

            // Push scene object if it's eligible for ticking.
            if ( pSceneObject->isEnabled() )
            {
                // Update enabled.
                objectsEnabled++;

                // Add to ticked objects if object is not being deleted and this is a "normal" scene or
                // the object is marked as allowing editor ticks.
                if ( !pSceneObject->isBeingDeleted() && (isNormalScene || pSceneObject->getIsEditorTickAllowed() )  )
                    mTickedSceneObjects.push_back( pSceneObject );
            }
        }

        // Update object stats.
        mDebugStats.objectsEnabled = objectsEnabled;
        mDebugStats.objectsVisible = objectsVisible;
        mDebugStats.objectsAwake   = objectsAwake;

        // Debug Status Reference.
        DebugStats* pDebugStats = &mDebugStats;

        // ****************************************************
        // Pre-integrate objects.
        // ****************************************************

        // Iterate ticked scene objects.
        for ( S32 n = 0; n < mTickedSceneObjects.size(); ++n )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_PreIntegrate);

            // Pre-integrate.
            mTickedSceneObjects[n]->preIntegrate( mSceneTime, Tickable::smTickSec, pDebugStats );
        }

        // Debug Profiling.
        PROFILE_START(Scene_IntegratePhysicsSystem);

        // Reset contacts.
        mBeginContacts.clear();
        mEndContacts.clear();

        // Only step the physics if a "normal" scene.
        if ( isNormalScene )
        {
            // Step the physics.
            mpWorld->Step( Tickable::smTickSec, mVelocityIterations, mPositionIterations );
        }

        // Debug Profiling.
        PROFILE_END();   // Scene_IntegratePhysicsSystem

        // Forward the contacts.
        forwardContacts();


        // ****************************************************
        // Integrate objects.
        // ****************************************************

        // Iterate ticked scene objects.
        for ( S32 n = 0; n < mTickedSceneObjects.size(); ++n )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_IntegrateObject);

            // Integrate.
            mTickedSceneObjects[n]->integrateObject( mSceneTime, Tickable::smTickSec, pDebugStats );
        }

        // ****************************************************
        // Post-Integrate Stage.
        // ****************************************************

        // Iterate ticked scene objects.
        for ( S32 n = 0; n < mTickedSceneObjects.size(); ++n )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_PostIntegrate);

            // Post-integrate.
            mTickedSceneObjects[n]->postIntegrate( mSceneTime, Tickable::smTickSec, pDebugStats );
        }

        // Scene update callback.
        if( mUpdateCallback )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_OnSceneUpdatetCallback);

            Con::executef( this, 1, "onSceneUpdate" );
        }

        // Only dispatch contacts if a "normal" scene.
        if ( isNormalScene )
        {
            // Dispatch contacts callbacks.
            dispatchEndContactCallbacks();
            dispatchBeginContactCallbacks();
        }

        // Clear ticked scene objects.
        mTickedSceneObjects.clear();
    }

    // Update debug stat ranges.
    mDebugStats.updateRanges();
}

//-----------------------------------------------------------------------------

void Scene::interpolateTick( F32 timeDelta )
{
    // Finish if scene is paused.
    if ( getScenePause() ) return;

    // Debug Profiling.
    PROFILE_SCOPE(Scene_InterpolateTick);

    // Iterate scene objects.
    for( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mSceneObjects[n];

        // Skip interpolation of scene object if it's not eligible.
        if ( !pSceneObject->isEnabled() || pSceneObject->isBeingDeleted() )
            continue;

        pSceneObject->interpolateObject( timeDelta );
    }
}

//-----------------------------------------------------------------------------

void Scene::sceneRender( const SceneRenderState* pSceneRenderState )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_RenderSceneTotal);

    // Fetch debug stats.
    DebugStats* pDebugStats = pSceneRenderState->mpDebugStats;

    // Reset Render Stats.
    pDebugStats->renderPicked                   = 0;
    pDebugStats->renderRequests                 = 0;
    pDebugStats->renderFallbacks                = 0;
    pDebugStats->batchTrianglesSubmitted        = 0;
    pDebugStats->batchDrawCallsStrictSingle     = 0;
    pDebugStats->batchDrawCallsStrictMultiple   = 0;
    pDebugStats->batchDrawCallsSorted           = 0;
    pDebugStats->batchFlushes                   = 0;
    pDebugStats->batchBlendStateFlush           = 0;
    pDebugStats->batchColorStateFlush           = 0;
    pDebugStats->batchAlphaStateFlush           = 0;
    pDebugStats->batchTextureChangeFlush        = 0;
    pDebugStats->batchBufferFullFlush           = 0;
    pDebugStats->batchIsolatedFlush             = 0;
    pDebugStats->batchLayerFlush                = 0;
    pDebugStats->batchNoBatchFlush              = 0;
    pDebugStats->batchAnonymousFlush            = 0;

    // Set batch renderer wireframe mode.
    mBatchRenderer.setWireframeMode( getDebugMask() & SCENE_DEBUG_WIREFRAME_RENDER );

    // Clear the background color if requested.
    if ( mUseBackgroundColor )
    {
        glClearColor( mBackgroundColor.red, mBackgroundColor.green, mBackgroundColor.blue, mBackgroundColor.alpha );
        glClear(GL_COLOR_BUFFER_BIT);	
    }

    // Debug Profiling.
    PROFILE_START(Scene_RenderSceneVisibleQuery);

    // Rotate the render AABB by the camera angle.
    b2AABB cameraAABB;
    CoreMath::mRotateAABB( pSceneRenderState->mRenderAABB, pSceneRenderState->mRenderAngle, cameraAABB );

    // Rotate the world matrix by the camera angle.
    const Vector2& cameraPosition = pSceneRenderState->mRenderPosition;
    glTranslatef( cameraPosition.x, cameraPosition.y, 0.0f );
    glRotatef( mRadToDeg(pSceneRenderState->mRenderAngle), 0.0f, 0.0f, 1.0f );
    glTranslatef( -cameraPosition.x, -cameraPosition.y, 0.0f );

    // Clear world query.
    mpWorldQuery->clearQuery();

    // Set filter.
    WorldQueryFilter queryFilter( pSceneRenderState->mRenderLayerMask, pSceneRenderState->mRenderGroupMask, true, true, false, false );
    mpWorldQuery->setQueryFilter( queryFilter );

    // Query render AABB.
    mpWorldQuery->renderQueryArea( cameraAABB );

    // Debug Profiling.
    PROFILE_END();  //Scene_RenderSceneVisibleQuery

    // Are there any query results?
    if ( mpWorldQuery->getQueryResultsCount() > 0 )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Scene_RenderSceneCompileRenderRequests);

        // Fetch the primary scene render queue.
        SceneRenderQueue* pSceneRenderQueue = SceneRenderQueueFactory.createObject();      

        // Yes so step through layers.
        for ( S32 layer = MAX_LAYERS_SUPPORTED-1; layer >= 0 ; layer-- )
        {
            // Fetch layer.
            typeWorldQueryResultVector& layerResults = mpWorldQuery->getLayeredQueryResults( layer );

            // Fetch layer object count.
            const U32 layerObjectCount = layerResults.size();

            // Are there any objects to render in this layer?
            if ( layerObjectCount > 0 )
            {
                // Yes, so increase render picked.
                pDebugStats->renderPicked += layerObjectCount;

                // Iterate query results.
                for( typeWorldQueryResultVector::iterator worldQueryItr = layerResults.begin(); worldQueryItr != layerResults.end(); ++worldQueryItr )
                {
                    // Fetch scene object.
                    SceneObject* pSceneObject = worldQueryItr->mpSceneObject;

                    // Skip if the object should not render.
                    if ( !pSceneObject->shouldRender() )
                        continue;

                    // Can the scene object prepare a render?
                    if ( pSceneObject->canPrepareRender() )
                    {
                        // Yes. so is it batch isolated.
                        if ( pSceneObject->getBatchIsolated() )
                        {
                            // Yes, so create a default render request  on the primary queue.
                            SceneRenderRequest* pIsolatedSceneRenderRequest = Scene::createDefaultRenderRequest( pSceneRenderQueue, pSceneObject );

                            // Create a new isolated render queue.
                            pIsolatedSceneRenderRequest->mpIsolatedRenderQueue = SceneRenderQueueFactory.createObject();

                            // Prepare in the isolated queue.
                            pSceneObject->scenePrepareRender( pSceneRenderState, pIsolatedSceneRenderRequest->mpIsolatedRenderQueue );

                            // Increase render request count.
                            pDebugStats->renderRequests += (U32)pIsolatedSceneRenderRequest->mpIsolatedRenderQueue->getRenderRequests().size();

                            // Adjust for the extra private render request.
                            pDebugStats->renderRequests -= 1;
                        }
                        else
                        {
                            // No, so prepare in primary queue.
                            pSceneObject->scenePrepareRender( pSceneRenderState, pSceneRenderQueue );
                        }
                    }
                    else
                    {
                        // No, so create a default render request for it.
                        Scene::createDefaultRenderRequest( pSceneRenderQueue, pSceneObject );
                    }
                }

                // Fetch render requests.
                SceneRenderQueue::typeRenderRequestVector& sceneRenderRequests = pSceneRenderQueue->getRenderRequests();

                // Fetch render request count.
                const U32 renderRequestCount = (U32)sceneRenderRequests.size();

                // Increase render request count.
                pDebugStats->renderRequests += renderRequestCount;

                // Do we have more than a single render request?
                if ( renderRequestCount > 1 )
                {
                    // Debug Profiling.
                    PROFILE_SCOPE(Scene_RenderSceneLayerSorting);

                    // Yes, so fetch layer sort mode.
                    SceneRenderQueue::RenderSort& mode = mLayerSortModes[layer];

                    // Temporarily switch to normal sort if batch sort but batcher disabled.
                    if ( !mBatchRenderer.getBatchEnabled() && mode == SceneRenderQueue::RENDER_SORT_BATCH )
                        mode = SceneRenderQueue::RENDER_SORT_NEWEST;

                    // Set render queue mode.
                    pSceneRenderQueue->setSortMode( mode );

                    // Sort the render requests.
                    pSceneRenderQueue->sort();
                }

                // Iterate render requests.
                for( SceneRenderQueue::typeRenderRequestVector::iterator renderRequestItr = sceneRenderRequests.begin(); renderRequestItr != sceneRenderRequests.end(); ++renderRequestItr )
                {
                     // Debug Profiling.
                    PROFILE_SCOPE(Scene_RenderSceneRequests);

                    // Fetch render request.
                    SceneRenderRequest* pSceneRenderRequest = *renderRequestItr;

                    // Fetch scene render object.
                    SceneRenderObject* pSceneRenderObject = pSceneRenderRequest->mpSceneRenderObject;
             
                    // Flush if the object is not render batched and we're in strict order mode.
                    if ( !pSceneRenderObject->isBatchRendered() && mBatchRenderer.getStrictOrderMode() )
                    {
                        mBatchRenderer.flush( pDebugStats->batchNoBatchFlush );
                    }
                    // Flush if the object is batch isolated.
                    else if ( pSceneRenderObject->getBatchIsolated() )
                    {
                        mBatchRenderer.flush( pDebugStats->batchIsolatedFlush );
                    }

                    // Yes, so is the object batch rendered?
                    if ( pSceneRenderObject->isBatchRendered() )
                    {
                        // Yes, so set the blend mode.
                        mBatchRenderer.setBlendMode( pSceneRenderRequest );

                        // Set the alpha test mode.
                        mBatchRenderer.setAlphaTestMode( pSceneRenderRequest );
                    }

                    // Set batch strict order mode.
                    // NOTE:    We keep reasserting this because an object is free to change it during rendering.
                    mBatchRenderer.setStrictOrderMode( pSceneRenderQueue->getStrictOrderMode() );

                    // Is the object batch isolated?
                    if ( pSceneRenderObject->getBatchIsolated() )
                    {
                        // Yes, so fetch isolated render queue.
                        SceneRenderQueue* pIsolatedRenderQueue = pSceneRenderRequest->mpIsolatedRenderQueue;

                        // Sanity!
                        AssertFatal( pIsolatedRenderQueue != NULL, "Cannot render batch isolated with an isolated render queue." );

                        // Sort the isolated render requests.
                        pIsolatedRenderQueue->sort();

                        // Fetch isolated render requests.
                        SceneRenderQueue::typeRenderRequestVector& isolatedRenderRequests = pIsolatedRenderQueue->getRenderRequests();

                        // Can the object render?
                        if ( pSceneRenderObject->validRender() )
                        {
                            // Yes, so iterate isolated render requests.
                            for( SceneRenderQueue::typeRenderRequestVector::iterator isolatedRenderRequestItr = isolatedRenderRequests.begin(); isolatedRenderRequestItr != isolatedRenderRequests.end(); ++isolatedRenderRequestItr )
                            {
                                pSceneRenderObject->sceneRender( pSceneRenderState, *isolatedRenderRequestItr, &mBatchRenderer );
                            }
                        }
                        else
                        {
                            // No, so iterate isolated render requests.
                            for( SceneRenderQueue::typeRenderRequestVector::iterator isolatedRenderRequestItr = isolatedRenderRequests.begin(); isolatedRenderRequestItr != isolatedRenderRequests.end(); ++isolatedRenderRequestItr )
                            {
                                pSceneRenderObject->sceneRenderFallback( pSceneRenderState, *isolatedRenderRequestItr, &mBatchRenderer );
                            }

                            // Increase render fallbacks.
                            pDebugStats->renderFallbacks++;
                        }

                        // Flush isolated batch.
                        mBatchRenderer.flush( pDebugStats->batchIsolatedFlush );
                    }
                    else
                    {
                        // No, so can the object render?
                        if ( pSceneRenderObject->validRender() )
                        {
                            // Yes, so render object.
                            pSceneRenderObject->sceneRender( pSceneRenderState, pSceneRenderRequest, &mBatchRenderer );
                        }
                        else
                        {
                            // No, so render using fallback.
                            pSceneRenderObject->sceneRenderFallback( pSceneRenderState, pSceneRenderRequest, &mBatchRenderer );

                            // Increase render fallbacks.
                            pDebugStats->renderFallbacks++;
                        }
                    }
                }

                // Flush.
                // NOTE:    We cannot batch between layers as we adhere to a strict layer render order.
                mBatchRenderer.flush( pDebugStats->batchLayerFlush );

                // Iterate query results.
                for( typeWorldQueryResultVector::iterator worldQueryItr = layerResults.begin(); worldQueryItr != layerResults.end(); ++worldQueryItr )
                {
                    // Debug Profiling.
                    PROFILE_SCOPE(Scene_RenderObjectOverlays);

                    // Fetch scene object.
                    SceneObject* pSceneObject = worldQueryItr->mpSceneObject;

                    // Render object overlay.
                    pSceneObject->sceneRenderOverlay( pSceneRenderState );
                }
            }

            // Reset render queue.
            pSceneRenderQueue->resetState();
        }

        // Cache render queue..
        SceneRenderQueueFactory.cacheObject( pSceneRenderQueue );
    }

    // Draw Joints.
    if ( getDebugMask() & Scene::SCENE_DEBUG_JOINTS )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Scene_RenderSceneJointOverlays);

        mDebugDraw.DrawJoints( mpWorld );
    }

    // Update debug stat ranges.
    mDebugStats.updateRanges();

    // Are we using the render callback?
    if( mRenderCallback )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Scene_OnSceneRendertCallback);

        // Yes, so perform callback.
        Con::executef( this, 1, "onSceneRender" );
    }
}

//-----------------------------------------------------------------------------

void Scene::clearScene( bool deleteObjects )
{
    while( mSceneObjects.size() > 0 )
    {
        // Fetch first scene object.
        SceneObject* pSceneObject = mSceneObjects[0];

        // Remove Object from Scene.
        removeFromScene( pSceneObject );

        // Queue Object for deletion.
        if ( deleteObjects )
            pSceneObject->safeDelete();
    }
}

//-----------------------------------------------------------------------------

void Scene::addToScene( SceneObject* pSceneObject )
{
    if ( pSceneObject == NULL )
        return;

    // Fetch current scene.
    Scene* pCurrentScene = pSceneObject->getScene();

    // Ignore if already in the scene.
    if ( pCurrentScene == this )
        return;

#if defined(TORQUE_DEBUG)
    // Sanity!
    for ( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        AssertFatal( mSceneObjects[n] != pSceneObject, "A scene object has become corrupt." );
    }
#endif

    // Check that object is not already in a scene.
    if ( pCurrentScene )
    {
        // Remove from scene.
        pCurrentScene->removeFromScene( pSceneObject );
    }

    // Add scene object.
    mSceneObjects.push_back( pSceneObject );

    // Register with the scene.
    pSceneObject->OnRegisterScene( this );

    // Perform callback only if properly added to the simulation.
    if ( pSceneObject->isProperlyAdded() )
    {
        Con::executef(pSceneObject, 2, "onAddToScene", getIdString());
    }
    else
    {
        // Warning.
        Con::warnf("Scene object added to scene but the object is not registered with the simulation.  No 'onAddToScene' can be performed!  Use Target scene.");
    }
}

//-----------------------------------------------------------------------------

void Scene::removeFromScene( SceneObject* pSceneObject )
{
    if ( pSceneObject == NULL )
        return;

    // Check if object is actually in a scene.
    if ( !pSceneObject->getScene() )
    {
        Con::warnf("Scene::removeFromScene() - Object '%s' is not in a scene!.", pSceneObject->getIdString());
        return;
    }

    // Remove as debug-object if set.
    if ( pSceneObject == getDebugSceneObject() )
        setDebugSceneObject( NULL );

    // Process Destroy Notifications.
    pSceneObject->processDestroyNotifications();

    // Dismount Any Camera.
    pSceneObject->dismountCamera();

    // Remove from the SceneWindow last pickers
    for( U32 i = 0; i < (U32)mAttachedSceneWindows.size(); ++i )
    {
        (dynamic_cast<SceneWindow*>(mAttachedSceneWindows[i]))->removeFromInputEventPick(pSceneObject);
    }

    // Unregister from scene.
    pSceneObject->OnUnregisterScene( this );

    // Find scene object and remove it quickly.
    for ( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        if ( mSceneObjects[n] == pSceneObject )
        {
            mSceneObjects.erase_fast( n );
            break;
        }
    }

    // Perform callback.
    Con::executef( pSceneObject, 2, "onRemoveFromScene", getIdString() );
}

//-----------------------------------------------------------------------------

SceneObject* Scene::getSceneObject( const U32 objectIndex ) const
{
    // Sanity!
    AssertFatal( objectIndex < getSceneObjectCount(), "Scene::getSceneObject() - Invalid object index." );

    return mSceneObjects[objectIndex];
}

//-----------------------------------------------------------------------------

U32 Scene::getSceneObjects( typeSceneObjectVector& objects ) const
{
    // No objects if scene is empty!
    if ( getSceneObjectCount() == 0 )
        return 0;

    // Merge with objects.
    objects.merge( mSceneObjects );

    return getSceneObjectCount();
}

//-----------------------------------------------------------------------------

U32 Scene::getSceneObjects( typeSceneObjectVector& objects, const U32 sceneLayer ) const
{
    // No objects if scene is empty!
    if ( getSceneObjectCount() == 0 )
        return 0;

    // Reset object count.
    U32 count = 0;

    // Iterate scene objects.
    for( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mSceneObjects[n];

        // Skip if not the correct layer.
        if ( pSceneObject->getSceneLayer() != sceneLayer )
            continue;

        // Add to objects.
        objects.push_back( pSceneObject );

        // Increase count.
        count++;
    }

    return count;
}

//-----------------------------------------------------------------------------

void Scene::mergeScene( const Scene* pScene )
{
    // Fetch the scene object count.
    const U32 count = pScene->getSceneObjectCount();

    // Finish if there are not objects to copy.
    if ( count == 0 )
        return;


    for( U32 index = 0; index < count; ++index )
    {
        // Fetch a clone of the scene object.
        SceneObject* pSceneObject = (SceneObject*)pScene->getSceneObject( index )->clone( true );

        // Add the clone.
        addToScene( pSceneObject );
    }
}

//-----------------------------------------------------------------------------

b2Joint* Scene::findJoint( const U32 jointId )
{
    // Find joint.
    typeJointHash::iterator itr = mJoints.find( jointId );

    return itr == mJoints.end() ? NULL : itr->value;
}

//-----------------------------------------------------------------------------

b2JointType Scene::getJointType( const U32 jointId )
{
    // Sanity!
    if ( jointId >= mJointMasterId )
    {
        Con::warnf("The joint Id of %d is invalid.", jointId);
        return e_unknownJoint;
    }

    return findJoint( jointId )->GetType();
}

//-----------------------------------------------------------------------------

U32 Scene::findJointId( b2Joint* pJoint )
{
    // Sanity!
    AssertFatal( pJoint != NULL, "Joint cannot be NULL." );

    // Find joint.
    typeReverseJointHash::iterator itr = mReverseJoints.find( (U32)pJoint );

    if ( itr == mReverseJoints.end() )
    {
        Con::warnf("The joint Id could not be found via a joint reference of %x", (U32)pJoint);
        return 0;
    }

    return itr->value;
}

//-----------------------------------------------------------------------------

U32 Scene::createJoint( b2JointDef* pJointDef )
{
    // Sanity!
    AssertFatal( pJointDef != NULL, "Joint definition cannot be NULL." );

    // Create Joint.
    b2Joint* pJoint = mpWorld->CreateJoint( pJointDef );

    // Allocate joint Id.
    const U32 jointId = mJointMasterId++;

    // Insert joint.
    typeJointHash::iterator itr = mJoints.insert( jointId, pJoint );

    // Sanity!
    AssertFatal( itr != mJoints.end(), "Joint already in hash table." );

    // Insert reverse joint.
    mReverseJoints.insert( (U32)pJoint, jointId );

    return jointId;
}

//-----------------------------------------------------------------------------

bool Scene::deleteJoint( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Finish if no joint.
    if ( pJoint == NULL )
        return false;

    // Destroy joint.
    // This should result in the joint references also being destroyed
    // as the scene is a destruction listener.
    mpWorld->DestroyJoint( pJoint );

    return true;
}

//-----------------------------------------------------------------------------

bool Scene::hasJoints( SceneObject* pSceneObject )
{
    // Sanity!
    AssertFatal( pSceneObject != NULL, "Scene object cannot be NULL!" );
    AssertFatal( pSceneObject->getScene() != this, "Scene object is not in this scene" );

    // Fetch body.
    b2Body* pBody = pSceneObject->getBody();

    // Fetch joint edge.
    b2JointEdge* pJointEdge = pBody->GetJointList();

    if ( pJointEdge == NULL || pJointEdge->joint == NULL )
        return false;

    // Found at least one joint.
    return true;
}

//-----------------------------------------------------------------------------

U32 Scene::createDistanceJoint(
    const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
    const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
    const F32 length,
    const F32 frequency,
    const F32 dampingRatio,
    const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2DistanceJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.length           = length < 0.0f ? (pBodyB->GetWorldPoint( localAnchorB ) - pBodyA->GetWorldPoint( localAnchorA )).Length() : length;
    jointDef.frequencyHz      = frequency;
    jointDef.dampingRatio     = dampingRatio;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setDistanceJointLength(
        const U32 jointId,
        const F32 length )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLength( length );
}

//-----------------------------------------------------------------------------

F32 Scene::getDistanceJointLength( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetLength();
}

//-----------------------------------------------------------------------------

void Scene::setDistanceJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetFrequency( frequency );
}

//-----------------------------------------------------------------------------

F32 Scene::getDistanceJointFrequency( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetFrequency();
}

//-----------------------------------------------------------------------------

void Scene::setDistanceJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getDistanceJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetDampingRatio();
}

//-----------------------------------------------------------------------------

U32 Scene::createRopeJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const F32 maxLength,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2RopeJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.maxLength        = maxLength < 0.0f ? (pBodyB->GetWorldPoint( localAnchorB ) - pBodyA->GetWorldPoint( localAnchorA )).Length() : maxLength;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setRopeJointMaxLength(
        const U32 jointId,
        const F32 maxLength )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_ropeJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2RopeJoint* pRealJoint = static_cast<b2RopeJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxLength( maxLength );
}

//-----------------------------------------------------------------------------

F32 Scene::getRopeJointMaxLength( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_ropeJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2RopeJoint* pRealJoint = static_cast<b2RopeJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxLength();
}

//-----------------------------------------------------------------------------

U32 Scene::createRevoluteJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2RevoluteJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.referenceAngle   = pBodyB->GetAngle() - pBodyA->GetAngle();
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setRevoluteJointLimit(
        const U32 jointId,
        const bool enableLimit,
        const F32 lowerAngle, const F32 upperAngle )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLimits( lowerAngle, upperAngle );
    pRealJoint->EnableLimit( enableLimit );
}

//-----------------------------------------------------------------------------

bool Scene::getRevoluteJointLimit(
        const U32 jointId,
        bool& enableLimit,
        F32& lowerAngle, F32& upperAngle )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    enableLimit = pRealJoint->IsLimitEnabled();
    lowerAngle = pRealJoint->GetLowerLimit();
    upperAngle = pRealJoint->GetUpperLimit();

    return true;
}

//-----------------------------------------------------------------------------

void Scene::setRevoluteJointMotor(
        const U32 jointId,
        const bool enableMotor,
        const F32 motorSpeed,
        const F32 maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMotorSpeed( motorSpeed );
    pRealJoint->SetMaxMotorTorque( maxMotorTorque );
    pRealJoint->EnableMotor( enableMotor );    
}

//-----------------------------------------------------------------------------

bool Scene::getRevoluteJointMotor(
        const U32 jointId,
        bool& enableMotor,
        F32& motorSpeed,
        F32& maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    enableMotor = pRealJoint->IsMotorEnabled();
    motorSpeed = pRealJoint->GetMotorSpeed();
    maxMotorTorque = pRealJoint->GetMaxMotorTorque();

    return true;
}

//-----------------------------------------------------------------------------

U32 Scene::createWeldJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const F32 frequency,
        const F32 dampingRatio,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2WeldJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.referenceAngle   = pBodyB->GetAngle() - pBodyA->GetAngle();
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.frequencyHz      = frequency;
    jointDef.dampingRatio     = dampingRatio;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setWeldJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetFrequency( frequency );
}


//-----------------------------------------------------------------------------

F32 Scene::getWeldJointFrequency( const U32 jointId  )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetFrequency();
}

//-----------------------------------------------------------------------------

void Scene::setWeldJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getWeldJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetDampingRatio();
}

//-----------------------------------------------------------------------------

U32 Scene::createWheelJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const b2Vec2& worldAxis,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2WheelJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.localAxisA       = pBodyA->GetLocalVector( worldAxis );
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setWheelJointMotor(
        const U32 jointId,
        const bool enableMotor,
        const F32 motorSpeed,
        const F32 maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMotorSpeed( motorSpeed );
    pRealJoint->SetMaxMotorTorque( maxMotorTorque );
    pRealJoint->EnableMotor( enableMotor ); 
}

//-----------------------------------------------------------------------------

bool Scene::getWheelJointMotor(
        const U32 jointId,
        bool& enableMotor,
        F32& motorSpeed,
        F32& maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    enableMotor = pRealJoint->IsMotorEnabled();
    motorSpeed = pRealJoint->GetMotorSpeed();
    maxMotorTorque = pRealJoint->GetMaxMotorTorque();

    return true;
}

//-----------------------------------------------------------------------------

void Scene::setWheelJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetSpringFrequencyHz( frequency );
}

//-----------------------------------------------------------------------------

F32 Scene::getWheelJointFrequency( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetSpringFrequencyHz();
}

//-----------------------------------------------------------------------------

void Scene::setWheelJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetSpringDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getWheelJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetSpringDampingRatio();
}

//-----------------------------------------------------------------------------

U32 Scene::createFrictionJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const F32 maxForce,
        const F32 maxTorque,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2FrictionJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.maxForce         = maxForce;
    jointDef.maxTorque        = maxTorque;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setFrictionJointMaxForce(
        const U32 jointId,
        const F32 maxForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxForce( maxForce );
}

//-----------------------------------------------------------------------------

F32 Scene::getFrictionJointMaxForce( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxForce();
}

//-----------------------------------------------------------------------------

void Scene::setFrictionJointMaxTorque(
        const U32 jointId,
        const F32 maxTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxTorque( maxTorque );
}


//-----------------------------------------------------------------------------

F32 Scene::getFrictionJointMaxTorque( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxTorque();
}

//-----------------------------------------------------------------------------

U32 Scene::createPrismaticJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const b2Vec2& worldAxis,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2PrismaticJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.referenceAngle   = pBodyB->GetAngle() - pBodyA->GetAngle();
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.localAxisA       = pBodyA->GetLocalVector( worldAxis );
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setPrismaticJointLimit(
        const U32 jointId,
        const bool enableLimit,
        const F32 lowerTranslation, const F32 upperTranslation )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLimits( lowerTranslation, upperTranslation );
    pRealJoint->EnableLimit( enableLimit );
}

//-----------------------------------------------------------------------------

bool Scene::getPrismaticJointLimit(
        const U32 jointId,
        bool& enableLimit,
        F32& lowerTranslation, F32& upperTranslation )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    enableLimit = pRealJoint->IsLimitEnabled();
    lowerTranslation = pRealJoint->GetLowerLimit();
    upperTranslation = pRealJoint->GetUpperLimit();

    return true;
}

//-----------------------------------------------------------------------------

void Scene::setPrismaticJointMotor(
        const U32 jointId,
        const bool enableMotor,
        const F32 motorSpeed,
        const F32 maxMotorForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMotorSpeed( motorSpeed );
    pRealJoint->SetMaxMotorForce( maxMotorForce );
    pRealJoint->EnableMotor( enableMotor ); 
}

//-----------------------------------------------------------------------------

bool Scene::getPrismaticJointMotor(
        const U32 jointId,
        bool& enableMotor,
        F32& motorSpeed,
        F32& maxMotorForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    enableMotor = pRealJoint->IsMotorEnabled();
    motorSpeed = pRealJoint->GetMotorSpeed();
    maxMotorForce = pRealJoint->GetMaxMotorForce();

    return true;
}

//-----------------------------------------------------------------------------

U32 Scene::createPulleyJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const b2Vec2& worldGroundAnchorA, const b2Vec2& worldGroundAnchorB,
        const F32 ratio,
        const F32 lengthA, const F32 lengthB,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2PulleyJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.groundAnchorA    = worldGroundAnchorA;
    jointDef.groundAnchorB    = worldGroundAnchorB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.lengthA          = lengthA < 0.0f ? (pBodyA->GetWorldPoint( localAnchorA ) - worldGroundAnchorA).Length() : lengthA;
    jointDef.lengthB          = lengthB < 0.0f ? (pBodyA->GetWorldPoint( localAnchorB ) - worldGroundAnchorB).Length() : lengthB;
    jointDef.ratio            = ratio > b2_epsilon ? ratio : b2_epsilon + b2_epsilon;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

U32 Scene::createTargetJoint(
        const SceneObject* pSceneObject,
        const b2Vec2& worldTarget,
        const F32 maxForce,
        const bool useCenterOfMass,
        const F32 frequency,
        const F32 dampingRatio,
        const bool collideConnected )
{
    // Sanity!
    if ( pSceneObject != NULL && pSceneObject->getScene() == NULL )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Sanity!
    AssertFatal( pSceneObject != NULL, "Invalid scene object." );

    // Fetch bodies.
    b2Body* pBody = pSceneObject->getBody();
    
    // Populate definition.
    b2MouseJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = getGroundBody();
    jointDef.bodyB            = pBody;
    jointDef.target           = useCenterOfMass ? pBody->GetWorldCenter() : worldTarget;
    jointDef.maxForce         = maxForce;
    jointDef.frequencyHz      = frequency;
    jointDef.dampingRatio     = dampingRatio;

    // Create joint.
    const U32 jointId = createJoint( &jointDef );

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( findJoint( jointId ) );

    // Are we using the center of mass?
    if ( !useCenterOfMass )
    {
        // No, so set the target as the world target.
        // NOTE:-   This is done because initially the target (mouse) joint assumes the target 
        //          coincides with the body anchor.
        pRealJoint->SetTarget( worldTarget );
    }

    return jointId;
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointTarget(
        const U32 jointId,
        const b2Vec2& worldTarget )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetTarget( worldTarget );
}

//-----------------------------------------------------------------------------
b2Vec2 Scene::getTargetJointTarget( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return b2Vec2_zero;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return b2Vec2_zero;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetTarget();
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointMaxForce(
        const U32 jointId,
        const F32 maxForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxForce( maxForce );
}

//-----------------------------------------------------------------------------

F32 Scene::getTargetJointMaxForce( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxForce();
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetFrequency( frequency );
}

//-----------------------------------------------------------------------------

F32 Scene::getTargetJointFrequency( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetFrequency();
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getTargetJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetDampingRatio();
}

//-----------------------------------------------------------------------------

U32 Scene::createMotorJoint(
            const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
            const b2Vec2 linearOffset,
            const F32 angularOffset,
            const F32 maxForce,
            const F32 maxTorque,
            const F32 correctionFactor,
            const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2MotorJointDef jointDef;
    jointDef.userData           = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected   = collideConnected;
    jointDef.bodyA              = pBodyA;
    jointDef.bodyB              = pBodyB;
    jointDef.linearOffset       = linearOffset;
    jointDef.angularOffset      = angularOffset;
    jointDef.correctionFactor   = correctionFactor;
    jointDef.maxForce           = maxForce;
    jointDef.maxTorque          = maxTorque;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointLinearOffset(
        const U32 jointId,
        const b2Vec2& linearOffset )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLinearOffset( linearOffset );
}

//-----------------------------------------------------------------------------

b2Vec2 Scene::getMotorJointLinearOffset( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return b2Vec2_zero;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return b2Vec2_zero;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetLinearOffset();
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointAngularOffset(
        const U32 jointId,
        const F32 angularOffset )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetAngularOffset( angularOffset );
}

//-----------------------------------------------------------------------------

F32 Scene::getMotorJointAngularOffset( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetAngularOffset();
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointMaxForce(
        const U32 jointId,
        const F32 maxForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxForce( maxForce );
}

//-----------------------------------------------------------------------------

F32 Scene::getMotorJointMaxForce( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxForce();
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointMaxTorque(
        const U32 jointId,
        const F32 maxTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxTorque( maxTorque );
}


//-----------------------------------------------------------------------------

F32 Scene::getMotorJointMaxTorque( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxTorque();
}

//-----------------------------------------------------------------------------

void Scene::setDebugSceneObject( SceneObject* pSceneObject )
{
    // Ignore no change.
    if ( mpDebugSceneObject == pSceneObject )
        return;

    // Remove delete notification for existing monitored object.
    if ( mpDebugSceneObject != NULL )
        clearNotify( mpDebugSceneObject );

    // Set monitored scene object.
    mpDebugSceneObject = pSceneObject;

    // Finish if resetting monitored object.
    if ( pSceneObject == NULL )
        return;

    // Add delete notification for new monitored object.
    deleteNotify( pSceneObject );
}

//-----------------------------------------------------------------------------

void Scene::setLayerSortMode( const U32 layer, const SceneRenderQueue::RenderSort sortMode )
{
    // Is the layer valid?
    if ( layer > MAX_LAYERS_SUPPORTED )
    {
        // No, so warn.
        Con::warnf( "Scene::setLayerSortMode() - Layer '%d' is out of range.", layer );

        return;
    }

    // Is the sort mode valid?
    if ( sortMode == SceneRenderQueue::RENDER_SORT_INVALID )
    {
        // No, so warn.
        Con::warnf( "Scene::setLayerSortMode() - Sort mode is invalid for layer '%d'.", layer );

        return;
    }

    mLayerSortModes[layer] = sortMode;
}

//-----------------------------------------------------------------------------

SceneRenderQueue::RenderSort Scene::getLayerSortMode( const U32 layer )
{
    // Is the layer valid?
    if ( layer > MAX_LAYERS_SUPPORTED )
    {
        // No, so warn.
        Con::warnf( "Scene::getLayerSortMode() - Layer '%d' is out of range.", layer );

        return SceneRenderQueue::RENDER_SORT_INVALID;
    }

    return mLayerSortModes[layer];
}

//-----------------------------------------------------------------------------

void Scene::attachSceneWindow( SceneWindow* pSceneWindow2D )
{
    // Ignore if already attached.
    if ( isSceneWindowAttached( pSceneWindow2D ) )
        return;

    // Add to Attached List.
    mAttachedSceneWindows.addObject( pSceneWindow2D );
}

//-----------------------------------------------------------------------------

void Scene::detachSceneWindow( SceneWindow* pSceneWindow2D )
{
    // Ignore if not attached.
    if ( !isSceneWindowAttached( pSceneWindow2D ) )
        return;

    // Add to Attached List.
    mAttachedSceneWindows.removeObject( pSceneWindow2D );
}

//-----------------------------------------------------------------------------

void Scene::detachAllSceneWindows( void )
{
    // Detach All Scene Windows.
    while( mAttachedSceneWindows.size() > 0 )
        dynamic_cast<SceneWindow*>(mAttachedSceneWindows[mAttachedSceneWindows.size()-1])->resetScene();
}

//-----------------------------------------------------------------------------

bool Scene::isSceneWindowAttached( SceneWindow* pSceneWindow2D )
{
    for( SimSet::iterator itr = mAttachedSceneWindows.begin(); itr != mAttachedSceneWindows.end(); itr++ )
        if ( pSceneWindow2D == dynamic_cast<SceneWindow*>(*itr) )
            // Found.
            return true;

    // Not Found.
    return false;
}

//-----------------------------------------------------------------------------

void Scene::addDeleteRequest( SceneObject* pSceneObject )
{
    // Ignore if it's already being safe-deleted.
    if ( pSceneObject->isBeingDeleted() )
        return;

    // Populate Delete Request.
    tDeleteRequest deleteRequest = { pSceneObject->getId(), NULL, false };

    // Push Delete Request.
    mDeleteRequests.push_back( deleteRequest );

    // Flag Delete in Progress.
    pSceneObject->mBeingSafeDeleted = true;
}


//-----------------------------------------------------------------------------

void Scene::processDeleteRequests( const bool forceImmediate )
{
    // Ignore if there's no delete requests!
    if ( mDeleteRequests.size() == 0 )
        return;

    // Validate All Delete Requests.
    U32 safeDeleteReadyCount = 0;
    for ( U32 requestIndex = 0; requestIndex < (U32)mDeleteRequests.size(); )
    {
        // Fetch Reference to Delete Request.
        tDeleteRequest& deleteRequest = mDeleteRequests[requestIndex];

        // Fetch Object.
        // NOTE:- Let's be safer and check that it's definitely a scene-object.
        SceneObject* pSceneObject = dynamic_cast<SceneObject*>( Sim::findObject( deleteRequest.mObjectId ) );

        // Does this object exist?
        if ( pSceneObject )
        {
            // Yes, so write object.
            deleteRequest.mpSceneObject = pSceneObject;

            // Calculate Safe-Ready Flag.
            deleteRequest.mSafeDeleteReady = forceImmediate || pSceneObject->getSafeDelete();

            // Is it ready to safe-delete?
            if ( deleteRequest.mSafeDeleteReady )
            {
                // Yes, so increase safe-ready count.
                ++safeDeleteReadyCount;
            }         
        }
        else
        {
            // No, so it looks like the object got deleted prematurely; let's just remove
            // the request instead.
            mDeleteRequests.erase_fast( requestIndex );
            
            // Repeat this item.
            continue;
        }

        // Skip to next request index.
        ++requestIndex;
    }

    // Stop if there's no delete requests!
    if ( mDeleteRequests.size() == 0 )
        return;

    // Transfer Delete-Requests to Temporary version.
    // NOTE:-   We do this because we may delete objects which have dependencies.  This would
    //          cause objects to be added to the safe-delete list.  We don't want to work on
    //          the list whilst this is happening so we'll transfer it to a temporary list.
    mDeleteRequestsTemp = mDeleteRequests;

    // Can we process all remaining delete-requests?
    if ( safeDeleteReadyCount == (U32)mDeleteRequestsTemp.size() )
    {
        // Yes, so process ALL safe-ready delete-requests.
        for ( U32 requestIndex = 0; requestIndex < (U32)mDeleteRequestsTemp.size(); ++requestIndex )
        {
            // Yes, so fetch object.
            SceneObject* pSceneObject = mDeleteRequestsTemp[requestIndex].mpSceneObject;

            // Do script callback.
            Con::executef(this, 2, "onSafeDelete", pSceneObject->getIdString() );

            // Destroy the object.
            pSceneObject->deleteObject();
        }

        // Remove All delete-requests.
        mDeleteRequestsTemp.clear();
    }
    else
    {
        // No, so process only safe-ready delete-requests.
        for ( U32 requestIndex = 0; requestIndex <(U32) mDeleteRequestsTemp.size(); )
        {
            // Fetch Reference to Delete Request.
            tDeleteRequest& deleteRequest = mDeleteRequestsTemp[requestIndex];

            // Is the Object Safe-Ready?
            if ( deleteRequest.mSafeDeleteReady )
            {
                // Yes, so fetch object.
                SceneObject* pSceneObject = deleteRequest.mpSceneObject;

                // Do script callback.
                Con::executef(this, 2, "onSafeDelete", pSceneObject->getIdString() );

                // Destroy the object.
                pSceneObject->deleteObject();

                // Quickly remove delete-request.
                mDeleteRequestsTemp.erase_fast( requestIndex );

                // Repeat this item.
                continue;
            }

            // Skip to next request index.
            ++requestIndex;
        }
    }
}

//-----------------------------------------------------------------------------

void Scene::SayGoodbye( b2Joint* pJoint )
{
    // Find the joint id.
    const U32 jointId = findJointId( pJoint );

    // Ignore a bad joint.
    if ( jointId == 0 )
        return;

    // Remove joint references.
    mJoints.erase( jointId );
    mReverseJoints.erase( (U32)pJoint );
}

//-----------------------------------------------------------------------------

void Scene::SayGoodbye( b2Fixture* pFixture )
{
    // The scene is not currently interested in tracking fixtures
    // so we do nothing here for now.
}

//-----------------------------------------------------------------------------

void Scene::onTamlPreRead( void )
{
    // Call parent.
    Parent::onTamlPreRead();
}

//-----------------------------------------------------------------------------

void Scene::onTamlPostRead( const TamlCustomProperties& customProperties )
{
    // Call parent.
    Parent::onTamlPostRead( customProperties );

    // Reset the loading scene.
    Scene::LoadingScene = NULL;

    // Find joint custom property.
    const TamlCustomProperty* pJointProperty = customProperties.findProperty( jointCustomPropertyName );

    // Finish if no joints.
    if ( pJointProperty == NULL )
        return;

    // Iterate joints.
    for( TamlCustomProperty::const_iterator propertyAliasItr = pJointProperty->begin(); propertyAliasItr != pJointProperty->end(); ++propertyAliasItr )
    {
        // Fetch property alias.
        TamlPropertyAlias* pPropertyAlias = *propertyAliasItr;

        // Fetch alias name.
        StringTableEntry aliasName = pPropertyAlias->mAliasName;

        // Is this a distance joint?
        if ( aliasName == jointDistanceTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            F32 length = -1.0f;
            F32 frequency = 0.0f;
            F32 dampingRatio = 0.0f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointDistanceLengthName )
                {
                    pPropertyField->getFieldValue( length );
                }
                else if ( fieldName == jointDistanceFrequencyName )
                {
                    pPropertyField->getFieldValue( frequency );
                }
                else if ( fieldName == jointDistanceDampingRatioName )
                {
                    pPropertyField->getFieldValue( dampingRatio );
                }
            }

            // Create joint.
            createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, length, frequency, dampingRatio, collideConnected );
        }
        // is this a rope joint?
        else if ( aliasName == jointRopeTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            F32 maxLength = -1.0f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointRopeMaxLengthName )
                {
                    pPropertyField->getFieldValue( maxLength );
                }
            }

            // Create joint.
            createRopeJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxLength, collideConnected );
        }
        // Is this a revolute joint?
        else if ( aliasName == jointRevoluteTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            bool enableLimit = false;
            F32 lowerAngle = 0.0f;
            F32 upperAngle = 0.0f;

            bool enableMotor = false;
            F32 motorSpeed = b2_pi;
            F32 maxMotorTorque = 0.0f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointRevoluteLimitLowerAngleName )
                {
                    pPropertyField->getFieldValue( lowerAngle );
                    lowerAngle = mDegToRad( lowerAngle );
                    enableLimit = true;
                }
                else if ( fieldName == jointRevoluteLimitUpperAngleName )
                {
                    pPropertyField->getFieldValue( upperAngle );
                    upperAngle = mDegToRad( upperAngle );
                    enableLimit = true;
                }
                else if ( fieldName == jointRevoluteMotorSpeedName )
                {
                    pPropertyField->getFieldValue( motorSpeed );
                    motorSpeed = mDegToRad( motorSpeed );
                    enableMotor = true;
                }
                else if ( fieldName == jointRevoluteMotorMaxTorqueName )
                {
                    pPropertyField->getFieldValue( maxMotorTorque );
                    enableMotor = true;
                }
            }

            // Create joint.
            const U32 jointId = createRevoluteJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, collideConnected );
            
            if ( enableLimit )
                setRevoluteJointLimit( jointId, true, lowerAngle, upperAngle );

            if ( enableMotor )
                setRevoluteJointMotor( jointId, true, motorSpeed, maxMotorTorque );
        }
        // is this a weld joint?
        else if ( aliasName == jointWeldTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            F32 frequency = 0.0f;
            F32 dampingRatio = 0.0f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointWeldFrequencyName )
                {
                    pPropertyField->getFieldValue( frequency );
                }
                else if ( fieldName == jointWeldDampingRatioName )
                {
                    pPropertyField->getFieldValue( dampingRatio );
                }
            }

            // Create joint.
            createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, frequency, dampingRatio, collideConnected );
        }
        // Is this a wheel joint?
        else if ( aliasName == jointWheelTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            bool enableMotor = false;
            F32 motorSpeed = b2_pi;
            F32 maxMotorTorque = 0.0f;

            F32 frequency = 0.0f;
            F32 dampingRatio = 0.0f;
            b2Vec2 worldAxis( 0.0f, 1.0f );

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointWheelMotorSpeedName )
                {
                    pPropertyField->getFieldValue( motorSpeed );
                    motorSpeed = mDegToRad( motorSpeed );
                    enableMotor = true;
                }
                else if ( fieldName == jointWheelMotorMaxTorqueName )
                {
                    pPropertyField->getFieldValue( maxMotorTorque );
                    enableMotor = true;
                }
                else if ( fieldName == jointWheelFrequencyName )
                {
                    pPropertyField->getFieldValue( frequency );
                }
                else if ( fieldName == jointWheelDampingRatioName )
                {
                    pPropertyField->getFieldValue( dampingRatio );
                }
                else if ( fieldName == jointWheelWorldAxisName )
                {
                    pPropertyField->getFieldValue( worldAxis );                    
                }
            }

            // Create joint.
            const U32 jointId = createWheelJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis, collideConnected );

            if ( enableMotor )
                setWheelJointMotor( jointId, true, motorSpeed, maxMotorTorque );

            setWheelJointFrequency( jointId, frequency );
            setWheelJointDampingRatio( jointId, dampingRatio );
        }
        // Is this a friction joint?
        else if ( aliasName == jointFrictionTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            F32 maxForce = 0.0f;
            F32 maxTorque = 0.0f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointFrictionMaxForceName )
                {
                    pPropertyField->getFieldValue( maxForce );
                }
                else if ( fieldName == jointFrictionMaxTorqueName )
                {
                    pPropertyField->getFieldValue( maxTorque );
                }
            }

            // Create joint.
            createFrictionJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxForce, maxTorque, collideConnected );
        }
        // Is this a prismatic joint?
        else if ( aliasName == jointPrismaticTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            bool enableLimit;
            F32 lowerTransLimit = 0.0f;
            F32 upperTransLimit = 1.0f;

            bool enableMotor = false;
            F32 motorSpeed = b2_pi;
            F32 maxMotorForce = 0.0f;

            b2Vec2 worldAxis( 0.0f, 1.0f );

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointPrismaticLimitLowerTransName )
                {
                    pPropertyField->getFieldValue( lowerTransLimit );
                    enableLimit = true;
                }
                else if ( fieldName == jointPrismaticLimitUpperTransName )
                {
                    pPropertyField->getFieldValue( upperTransLimit );
                    enableLimit = true;
                }
                else if ( fieldName == jointPrismaticMotorSpeedName )
                {
                    pPropertyField->getFieldValue( motorSpeed );
                    motorSpeed = mDegToRad( motorSpeed );
                    enableMotor = true;
                }
                else if ( fieldName == jointPrismaticMotorMaxForceName )
                {
                    pPropertyField->getFieldValue( maxMotorForce );
                    enableMotor = true;
                }
                else if ( fieldName == jointPrismaticWorldAxisName )
                {
                    pPropertyField->getFieldValue( worldAxis );
                }
            }

            // Create joint.
            const U32 jointId = createPrismaticJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis, collideConnected );

            if ( enableLimit )
                setPrismaticJointLimit( jointId, true, lowerTransLimit, upperTransLimit );

            if ( enableMotor )
                setPrismaticJointMotor( jointId, true, motorSpeed, maxMotorForce );
        }
        // Is this a pulley joint?
        else if ( aliasName == jointPulleyTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            b2Vec2 localAnchorA = b2Vec2_zero;
            b2Vec2 localAnchorB = b2Vec2_zero;
            bool collideConnected = false;

            F32 lengthA = -1.0f;
            F32 lengthB = -1.0f;
            F32 ratio = 0.5f;
            b2Vec2 worldGroundAnchorA = b2Vec2_zero;
            b2Vec2 worldGroundAnchorB = b2Vec2_zero;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointLocalAnchorAName )
                {
                    pPropertyField->getFieldValue( localAnchorA );
                }
                else if ( fieldName == jointLocalAnchorBName )
                {
                    pPropertyField->getFieldValue( localAnchorB );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointPulleyLengthAName )
                {
                    pPropertyField->getFieldValue( lengthA );
                }
                else if ( fieldName == jointPulleyLengthBName )
                {
                    pPropertyField->getFieldValue( lengthB );
                }
                else if ( fieldName == jointPulleyRatioName )
                {
                    pPropertyField->getFieldValue( ratio );
                }
                else if ( fieldName == jointPulleyGroundAnchorAName )
                {
                    pPropertyField->getFieldValue( worldGroundAnchorA );
                }
                else if ( fieldName == jointPulleyGroundAnchorBName )
                {
                    pPropertyField->getFieldValue( worldGroundAnchorB );
                }
            }

            // Create joint.
            createPulleyJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldGroundAnchorA, worldGroundAnchorB, ratio, lengthA, lengthB, collideConnected );

        }
        // Is this a target joint?
        else if ( aliasName == jointTargetTypeName )
        {
            SceneObject* pSceneObject = NULL;
            bool collideConnected = false;
            b2Vec2 worldTarget = b2Vec2_zero;
            F32 maxForce = 1.0f;
            F32 frequency = 5.0f;
            F32 dampingRatio = 0.7f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObject = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointTargetWorldTargetName )
                {
                    pPropertyField->getFieldValue( worldTarget );
                }
                else if ( fieldName == jointTargetMaxForceName )
                {
                    pPropertyField->getFieldValue( maxForce );
                }
                else if ( fieldName == jointTargetFrequencyName )
                {
                    pPropertyField->getFieldValue( frequency );
                }
                else if ( fieldName == jointTargetDampingRatioName )
                {
                    pPropertyField->getFieldValue( dampingRatio );
                }
            }

            // Create joint.
            createTargetJoint( pSceneObject, worldTarget, maxForce, frequency, dampingRatio, collideConnected );
        }
        // Is this a motor joint?
        else if ( aliasName == jointMotorTypeName )
        {
            SceneObject* pSceneObjectA = NULL;
            SceneObject* pSceneObjectB = NULL;
            bool collideConnected = false;

            b2Vec2 linearOffset = b2Vec2_zero;
            F32 angularOffset = 0.0f;
            F32 maxForce = 1.0f;
            F32 maxTorque = 1.0f;
            F32 correctionFactor = 0.3f;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pPropertyField->getFieldName();

                // Fetch fields.
                if ( fieldName == jointObjectAName )
                {
                    pSceneObjectA = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointObjectBName )
                {
                    pSceneObjectB = dynamic_cast<SceneObject*>( pPropertyField->getFieldObject() );
                }
                else if ( fieldName == jointCollideConnectedName )
                {
                    pPropertyField->getFieldValue( collideConnected );
                }
                else if ( fieldName == jointMotorLinearOffsetName )
                {
                    pPropertyField->getFieldValue( linearOffset );
                }
                else if ( fieldName == jointMotorAngularOffsetName )
                {
                    pPropertyField->getFieldValue( angularOffset );
                    angularOffset = mDegToRad( angularOffset );
                }
                else if ( fieldName == jointMotorMaxForceName )
                {
                    pPropertyField->getFieldValue( maxForce );
                }
                else if ( fieldName == jointMotorMaxTorqueName )
                {
                    pPropertyField->getFieldValue( maxTorque );
                }
                else if ( fieldName == jointMotorCorrectionFactorName )
                {
                    pPropertyField->getFieldValue( correctionFactor );
                }
            }

            // Create joint.
            createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset, angularOffset, maxForce, maxTorque, correctionFactor, collideConnected );

        }
        // Unknown joint type!
        else
        {
            // Warn.
            Con::warnf( "Unknown joint type of '%s' encountered.", aliasName );

            // Sanity!
            AssertFatal( false, "Scene::onTamlCustomRead() - Unknown joint type detected." );

            continue;
        }
    }
}

//-----------------------------------------------------------------------------

void Scene::onTamlCustomWrite( TamlCustomProperties& customProperties )
{
    // Call parent.
    Parent::onTamlCustomWrite( customProperties );

    // Fetch joint count.
    const U32 jointCount = getJointCount();

    // Finish if no joints.
    if ( jointCount == 0 )
        return;

    // Add joint property.
    TamlCustomProperty* pJointProperty = customProperties.addProperty( jointCustomPropertyName );

    // Iterate joints.
    for( typeJointHash::iterator jointItr = mJoints.begin(); jointItr != mJoints.end(); ++jointItr )
    {
        // Fetch base joint.
        b2Joint* pBaseJoint = jointItr->value;

        // Add joint alias.
        // NOTE:    The name of the alias will get updated shortly.
        TamlPropertyAlias* pJointAlias = pJointProperty->addAlias( StringTable->EmptyString );

        // Fetch common details.
        b2Body* pBodyA = pBaseJoint->GetBodyA();
        b2Body* pBodyB = pBaseJoint->GetBodyB();

        // Fetch physics proxies.
        PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pBodyA->GetUserData());
        PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pBodyB->GetUserData());

        // Fetch physics proxy type.
        PhysicsProxy::ePhysicsProxyType proxyTypeA = static_cast<PhysicsProxy*>(pBodyA->GetUserData())->getPhysicsProxyType();
        PhysicsProxy::ePhysicsProxyType proxyTypeB = static_cast<PhysicsProxy*>(pBodyB->GetUserData())->getPhysicsProxyType();

        // Fetch scene objects.
        SceneObject* pSceneObjectA = proxyTypeA == PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ? static_cast<SceneObject*>(pPhysicsProxyA) : NULL;
        SceneObject* pSceneObjectB = proxyTypeB == PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ? static_cast<SceneObject*>(pPhysicsProxyB) : NULL;

        // Populate joint appropriately.
        switch( pBaseJoint->GetType() )
        {
            case e_distanceJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointDistanceTypeName );

                    // Fetch joint.
                    const b2DistanceJoint* pJoint = dynamic_cast<const b2DistanceJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid distance joint type returned." );

                    // Add length.
                    pJointAlias->addField( jointDistanceLengthName, pJoint->GetLength() );

                    // Add frequency.
                    if ( mNotZero( pJoint->GetFrequency() ) )
                        pJointAlias->addField( jointDistanceFrequencyName, pJoint->GetFrequency() );

                    // Add damping ratio.
                    if ( mNotZero( pJoint->GetDampingRatio() ) )
                        pJointAlias->addField( jointDistanceDampingRatioName, pJoint->GetDampingRatio() );

                    // Add local anchors.
                    if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_ropeJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointRopeTypeName );

                    // Fetch joint.
                    const b2RopeJoint* pJoint = dynamic_cast<const b2RopeJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid rope joint type returned." );

                    // Add max length.
                    if ( mNotZero( pJoint->GetMaxLength() ) )
                        pJointAlias->addField( jointRopeMaxLengthName, pJoint->GetMaxLength() );

                    // Add local anchors.
                    if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_revoluteJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointRevoluteTypeName );

                    // Fetch joint.
                    const b2RevoluteJoint* pJoint = dynamic_cast<const b2RevoluteJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid revolute joint type returned." );

                    // Add limit.
                    if ( pJoint->IsLimitEnabled() )
                    {
                        // Add limits.
                        pJointAlias->addField( jointRevoluteLimitLowerAngleName, mRadToDeg(pJoint->GetLowerLimit()) );
                        pJointAlias->addField( jointRevoluteLimitUpperAngleName, mRadToDeg(pJoint->GetUpperLimit()) );
                    }

                    // Add motor.
                    if ( pJoint->IsMotorEnabled() )
                    {
                        // Add motor.
                        pJointAlias->addField( jointRevoluteMotorSpeedName, mRadToDeg(pJoint->GetMotorSpeed()) );
                        pJointAlias->addField( jointRevoluteMotorMaxTorqueName, pJoint->GetMaxMotorTorque() );
                    }

                    // Add local anchors.
                    if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_weldJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointWeldTypeName );

                    // Fetch joint.
                    const b2WeldJoint* pJoint = dynamic_cast<const b2WeldJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid weld joint type returned." );

                    // Add frequency.
                    if ( mNotZero( pJoint->GetFrequency() ) )
                        pJointAlias->addField( jointWeldFrequencyName, pJoint->GetFrequency() );

                    // Add damping ratio.
                    if ( mNotZero( pJoint->GetDampingRatio() ) )
                        pJointAlias->addField( jointWeldDampingRatioName, pJoint->GetDampingRatio() );

                    // Add local anchors.
                    if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_wheelJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointWheelTypeName );

                    // Fetch joint.
                    b2WheelJoint* pJoint = dynamic_cast<b2WheelJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid wheel joint type returned." );

                    // Add motor.
                    if ( pJoint->IsMotorEnabled() )
                    {
                        // Add motor.
                        pJointAlias->addField( jointWheelMotorSpeedName, mRadToDeg(pJoint->GetMotorSpeed()) );
                        pJointAlias->addField( jointWheelMotorMaxTorqueName, pJoint->GetMaxMotorTorque() );
                    }

                    // Add frequency.
                    if ( mNotZero( pJoint->GetSpringFrequencyHz() ) )
                        pJointAlias->addField( jointWheelFrequencyName, pJoint->GetSpringFrequencyHz() );

                    // Add damping ratio.
                    if ( mNotZero( pJoint->GetSpringDampingRatio() ) )
                        pJointAlias->addField( jointWheelDampingRatioName, pJoint->GetSpringDampingRatio() );

                    // Add world axis.
                    pJointAlias->addField( jointWheelWorldAxisName, pJoint->GetBodyA()->GetWorldVector( pJoint->GetLocalAxisA() ) );

                    // Add local anchors.
                    pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_frictionJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointFrictionTypeName );

                    // Fetch joint.
                    const b2FrictionJoint* pJoint = dynamic_cast<const b2FrictionJoint*>( pBaseJoint );

                    // Add max force.
                    if ( mNotZero( pJoint->GetMaxForce() ) )
                        pJointAlias->addField( jointFrictionMaxForceName, pJoint->GetMaxForce() );

                    // Add max torque.
                    if ( mNotZero( pJoint->GetMaxTorque() ) )
                        pJointAlias->addField( jointFrictionMaxTorqueName, pJoint->GetMaxTorque() );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid friction joint type returned." );

                    // Add local anchors.
                    if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                        pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_prismaticJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointPrismaticTypeName );

                    // Fetch joint.
                    b2PrismaticJoint* pJoint = dynamic_cast<b2PrismaticJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid prismatic joint type returned." );

                    // Add limit.
                    if ( pJoint->IsLimitEnabled() )
                    {
                        // Add limits.
                        pJointAlias->addField( jointPrismaticLimitLowerTransName, pJoint->GetLowerLimit() );
                        pJointAlias->addField( jointPrismaticLimitUpperTransName, pJoint->GetUpperLimit() );
                    }

                    // Add motor.
                    if ( pJoint->IsMotorEnabled() )
                    {
                        // Add motor.
                        pJointAlias->addField( jointPrismaticMotorSpeedName, mRadToDeg(pJoint->GetMotorSpeed()) );
                        pJointAlias->addField( jointPrismaticMotorMaxForceName, pJoint->GetMaxMotorForce() );
                    }

                    // Add world axis.
                    pJointAlias->addField( jointPrismaticWorldAxisName, pJoint->GetBodyA()->GetWorldVector( pJoint->GetLocalAxisA() ) );

                    // Add local anchors.
                    pJointAlias->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                    pJointAlias->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_pulleyJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointPulleyTypeName );

                    // Fetch joint.
                    b2PulleyJoint* pJoint = dynamic_cast<b2PulleyJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid pulley joint type returned." );

                    // Add lengths.
                    pJointAlias->addField( jointPulleyLengthAName, pJoint->GetLengthA() );
                    pJointAlias->addField( jointPulleyLengthBName, pJoint->GetLengthB() );

                    // Add ratio,
                    pJointAlias->addField( jointPulleyRatioName, pJoint->GetRatio() );

                    // Add ground anchors.
                    pJointAlias->addField( jointPulleyGroundAnchorAName, pJoint->GetGroundAnchorA() );
                    pJointAlias->addField( jointPulleyGroundAnchorBName, pJoint->GetGroundAnchorB() );

                    // Add local anchors.
                    pJointAlias->addField( jointLocalAnchorAName, pJoint->GetBodyA()->GetLocalPoint( pJoint->GetAnchorA() ) );
                    pJointAlias->addField( jointLocalAnchorBName, pJoint->GetBodyB()->GetLocalPoint( pJoint->GetAnchorB() ) );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

            case e_mouseJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointTargetTypeName );

                    // Fetch joint.
                    const b2MouseJoint* pJoint = dynamic_cast<const b2MouseJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid target joint type returned." );

                    // Add target.
                    pJointAlias->addField( jointTargetWorldTargetName, pJoint->GetTarget() );

                    // Add max force.
                    pJointAlias->addField( jointTargetMaxForceName, pJoint->GetMaxForce() );

                    // Add frequency
                    pJointAlias->addField( jointTargetFrequencyName, pJoint->GetFrequency() );

                    // Add damping ratio.
                    pJointAlias->addField( jointTargetDampingRatioName, pJoint->GetDampingRatio() );

                    // Add body.
                    // NOTE: This joint uses BODYB as the object, BODYA is the ground-body however for easy of use
                    // we'll refer to this as OBJECTA in the persisted format.
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectB );
                }
                break;

            case e_motorJoint:
                {
                    // Set alias name.
                    pJointAlias->mAliasName = StringTable->insert( jointMotorTypeName );

                    // Fetch joint.
                    const b2MotorJoint* pJoint = dynamic_cast<const b2MotorJoint*>( pBaseJoint );

                    // Sanity!
                    AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid motor joint type returned." );

                    // Add linear offset.
                    if ( mNotZero( pJoint->GetLinearOffset().LengthSquared() ) )
                        pJointAlias->addField( jointMotorLinearOffsetName, pJoint->GetLinearOffset() );

                    // Add angular offset.
                    if ( mNotZero( pJoint->GetAngularOffset() ) )
                        pJointAlias->addField( jointMotorAngularOffsetName, mRadToDeg( pJoint->GetAngularOffset() ) );

                    // Add max force.
                    pJointAlias->addField( jointMotorMaxForceName, pJoint->GetMaxForce() );

                    // Add max torque.
                    pJointAlias->addField( jointMotorMaxTorqueName, pJoint->GetMaxTorque() );

                    // Add correction factor.
                    pJointAlias->addField( jointMotorCorrectionFactorName, pJoint->GetCorrectionFactor() );

                    // Add bodies.
                    if ( pSceneObjectA != NULL )
                        pJointAlias->addField( jointObjectAName, pSceneObjectA );
                    if ( pSceneObjectB != NULL )
                        pJointAlias->addField( jointObjectBName, pSceneObjectB );
                }
                break;

        default:
            // Sanity!
            AssertFatal( false, "Scene::onTamlCustomWrite() - Unknown joint type detected." );
        }

        // Add collide connected flag.
        if ( pBaseJoint->GetCollideConnected() )
            pJointAlias->addField( jointCollideConnectedName, pBaseJoint->GetCollideConnected() );
    }
}

//-----------------------------------------------------------------------------

void Scene::onTamlCustomRead( const TamlCustomProperties& customProperties )
{
    // Call parent.
    Parent::onTamlCustomRead( customProperties );
}

//-----------------------------------------------------------------------------

U32 Scene::getGlobalSceneCount( void )
{
    return sSceneCount;
}

//-----------------------------------------------------------------------------

SceneRenderRequest* Scene::createDefaultRenderRequest( SceneRenderQueue* pSceneRenderQueue, SceneObject* pSceneObject )
{
    // Create a render request and populate it with the default details.
    SceneRenderRequest* pSceneRenderRequest = pSceneRenderQueue->createRenderRequest()->set(
        pSceneObject,
        pSceneObject->getRenderPosition(),
        pSceneObject->getSceneLayerDepth(),
        pSceneObject->getSortPoint(),
        pSceneObject->getSerialId(),
        pSceneObject->getRenderGroup() );

    // Prepare the blending for it.
    pSceneRenderRequest->mBlendMode = pSceneObject->getBlendMode();
    pSceneRenderRequest->mBlendColor = pSceneObject->getBlendColor();
    pSceneRenderRequest->mSrcBlendFactor = pSceneObject->getSrcBlendFactor();
    pSceneRenderRequest->mDstBlendFactor = pSceneObject->getDstBlendFactor();
    pSceneRenderRequest->mAlphaTest = pSceneObject->getAlphaTest();

    return pSceneRenderRequest;
}

//-----------------------------------------------------------------------------

SimObject* Scene::getTamlChild( const U32 childIndex ) const
{
    // Sanity!
    AssertFatal( childIndex < (U32)mSceneObjects.size(), "Scene::getTamlChild() - Child index is out of range." );

    // For when the assert is not used.
    if ( childIndex >= (U32)mSceneObjects.size() )
        return NULL;

    return mSceneObjects[ childIndex ];
}

//-----------------------------------------------------------------------------

void Scene::addTamlChild( SimObject* pSimObject )
{
    // Sanity!
    AssertFatal( pSimObject != NULL, "Scene::addTamlChild() - Cannot add a NULL child object." );

    // Fetch as a scene object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( pSimObject );

    // Is it a scene object?
    if ( pSceneObject == NULL )
    {
        // No, so warn.
        Con::warnf( "Scene::addTamlChild() - Cannot add a child object that isn't a scene object." );
        return;
    }

    // Add the scene object.
    addToScene( pSceneObject );
}

//-----------------------------------------------------------------------------

static EnumTable::Enums DebugOptionsLookup[] =
                {
                { Scene::SCENE_DEBUG_METRICS,           "metrics" },
                { Scene::SCENE_DEBUG_FPS_METRICS,       "fps" },
                { Scene::SCENE_DEBUG_JOINTS,            "joints" },
                { Scene::SCENE_DEBUG_WIREFRAME_RENDER,  "wireframe" },
                ///
                { Scene::SCENE_DEBUG_AABB,              "aabb" },
                { Scene::SCENE_DEBUG_OOBB,              "oobb" },
                { Scene::SCENE_DEBUG_SLEEP,             "sleep" },
                { Scene::SCENE_DEBUG_COLLISION_SHAPES,  "collision" },
                { Scene::SCENE_DEBUG_POSITION_AND_COM,  "position" },
                { Scene::SCENE_DEBUG_SORT_POINTS,       "sort" },
                };

//-----------------------------------------------------------------------------

Scene::DebugOption Scene::getDebugOptionEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(DebugOptionsLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(DebugOptionsLookup[i].label, label) == 0)
            return((Scene::DebugOption)DebugOptionsLookup[i].index);

    // Warn.
    Con::warnf( "Scene::getDebugOptionEnum() - Invalid debug option '%s'.", label );

    return Scene::SCENE_DEBUG_INVALID;
}

//-----------------------------------------------------------------------------

const char* Scene::getDebugOptionDescription( Scene::DebugOption debugOption )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(DebugOptionsLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( DebugOptionsLookup[i].index == debugOption )
            return DebugOptionsLookup[i].label;
    }

    // Warn.
    Con::warnf( "Scene::getDebugOptionDescription() - Invalid debug option." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

static EnumTable::Enums jointTypeLookup[] =
                {
                { e_distanceJoint,  "distance"  },
                { e_ropeJoint,      "rope"      },
                { e_revoluteJoint,  "revolute"  },
                { e_weldJoint,      "weld"      },
                { e_wheelJoint,     "wheel"     },
                { e_frictionJoint,  "friction"  },
                { e_prismaticJoint, "prismatic" },
                { e_pulleyJoint,    "pulley"    },
                { e_mouseJoint,     "target"    },
                { e_motorJoint,     "motor"     },
                };

EnumTable jointTypeTable(sizeof(jointTypeLookup) / sizeof(EnumTable::Enums), &jointTypeLookup[0]);

//-----------------------------------------------------------------------------

const char* Scene::getJointTypeDescription( b2JointType jointType )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(jointTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( jointTypeLookup[i].index == jointType )
            return jointTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "Scene::getJointTypeDescription() - Invalid joint type." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

b2JointType Scene::getJointTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(jointTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(jointTypeLookup[i].label, label) == 0)
            return (b2JointType)jointTypeLookup[i].index;
    }

    // Warn.
    Con::warnf( "Scene::getJointTypeEnum() - Invalid joint of '%s'", label );

    return e_unknownJoint;
}

//-----------------------------------------------------------------------------

static EnumTable::Enums pickModeLookup[] =
                {
                { Scene::PICK_ANY,          "ANY" },
                { Scene::PICK_SIZE,         "SIZE" },
                { Scene::PICK_COLLISION,    "COLLISION" },
                };

//-----------------------------------------------------------------------------

Scene::PickMode Scene::getPickModeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(pickModeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(pickModeLookup[i].label, label) == 0)
            return((Scene::PickMode)pickModeLookup[i].index);

    // Warn.
    Con::warnf( "Scene::getPickModeEnum() - Invalid pick mode '%s'.", label );

    return Scene::PICK_INVALID;
}

//-----------------------------------------------------------------------------

const char* Scene::getPickModeDescription( Scene::PickMode pickMode )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(pickModeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( pickModeLookup[i].index == pickMode )
            return pickModeLookup[i].label;
    }

    // Warn.
    Con::warnf( "Scene::getPickModeDescription() - Invalid pick mode.");

    return StringTable->EmptyString;
}
