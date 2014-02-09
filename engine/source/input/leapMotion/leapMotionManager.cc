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

#ifndef _LEAPMOTIONMANAGER_H_
#include "input/leapMotion/leapMotionManager.h"
#endif

#ifndef _PLATFORM_MEMORY_H_
#include "platform/platformMemory.h"
#endif 

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

#ifndef _GAMEINTERFACE_H_
#include "game/gameInterface.h"
#endif

#ifndef _GUICANVAS_H_
#include "gui/guiCanvas.h"
#endif

#ifndef _LEAPMOTIONUTIL_H_
#include "leapMotionUtil.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#include "input/leapMotion/LeapMotionManager_ScriptBinding.h"

//-----------------------------------------------------------------------------

bool LeapMotionManager::smEnableDevice = true;

bool LeapMotionManager::smGenerateIndividualEvents = true;
bool LeapMotionManager::smKeepHandIndexPersistent = false;
bool LeapMotionManager::smKeepPointableIndexPersistent = false;

bool LeapMotionManager::smGenerateSingleHandRotationAsAxisEvents = false;

F32 LeapMotionManager::smMaximumHandAxisAngle = 25.0f;

bool LeapMotionManager::smGenerateWholeFrameEvents = false;

U32 LeapMotionManager::LM_FRAMEVALIDDATA = 0;
U32 LeapMotionManager::LM_HAND[LeapMotionConstants::MaxHands] = {0};
//U32 LeapMotionManager::LM_HANDROT[LeapMotionConstants::MaxHands] = {0};
U32 LeapMotionManager::LM_HANDAXISX = 0;
U32 LeapMotionManager::LM_HANDAXISY = 0;
U32 LeapMotionManager::LM_HANDPOINTABLE[LeapMotionConstants::MaxHands][LeapMotionConstants::MaxPointablesPerHand] = {0};
U32 LeapMotionManager::LM_HANDPOINTABLEROT[LeapMotionConstants::MaxHands][LeapMotionConstants::MaxPointablesPerHand] = {0};
U32 LeapMotionManager::LM_FRAME = 0;

//-----------------------------------------------------------------------------

LeapMotionManager::LeapMotionManager()
{
    // Initialize the console variables
    staticInit();

    // Create our controller and listener
    mListener = new MotionListener();
    mController = new Leap::Controller();
    mController->addListener(*mListener);

    // Allocate a mutex to use later
    mActiveMutex = Mutex::createMutex();

    // Nothing is ready yet
    mEnabled = false;
    mActive = false;
    mMouseControl = false;
    mMinCircleProgress = 0.0;
}

//-----------------------------------------------------------------------------

LeapMotionManager::~LeapMotionManager()
{
    // Disable and delete internal members
    disable();

    // Get rid of the mutex
    Mutex::destroyMutex(mActiveMutex);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::staticInit()
{
    // If true, the Leap Motion device will be enabled, if present
    Con::addVariable("pref::LeapMotion::EnableDevice", TypeBool, &smEnableDevice);
   
    // Indicates that events for each hand and pointable will be created.
    Con::addVariable("LeapMotion::GenerateIndividualEvents", TypeBool, &smGenerateIndividualEvents);
      
	// Indicates that we track hand IDs and will ensure that the same hand will remain at the same index between frames.   
    Con::addVariable("LeapMotion::KeepHandIndexPersistent", TypeBool, &smKeepHandIndexPersistent);
    
	// Indicates that we track pointable IDs and will ensure that the same pointable will remain at the same index between frames.   
    Con::addVariable("LeapMotion::KeepPointableIndexPersistent", TypeBool, &smKeepPointableIndexPersistent);
    
	// If true, broadcast single hand rotation as axis events.
    Con::addVariable("LeapMotion::GenerateSingleHandRotationAsAxisEvents", TypeBool, &smGenerateSingleHandRotationAsAxisEvents);
    
	// The maximum hand angle when used as an axis event as measured from a vector pointing straight up (in degrees).
    // Shoud range from 0 to 90 degrees.   
    Con::addVariable("LeapMotion::MaximumHandAxisAngle", TypeF32, &smMaximumHandAxisAngle);
    
    // Indicates that a whole frame event should be generated and frames should be buffered.
    Con::addVariable("LeapMotion::GenerateWholeFrameEvents", TypeBool, &smGenerateWholeFrameEvents);   
}

//-----------------------------------------------------------------------------

void LeapMotionManager::enable(bool enabledState)
{
    Mutex::lockMutex(mActiveMutex);
    mEnabled = enabledState;
    Mutex::unlockMutex(mActiveMutex);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::disable()
{
    if (mController)
    {
        delete mController;
        mController = NULL;

        if (mListener)
        {
            delete mListener;
            mListener = NULL;
        }
    }

    setActive(false);
    mEnabled = false;
}

//-----------------------------------------------------------------------------

bool LeapMotionManager::getActive()
{
    Mutex::lockMutex(mActiveMutex);
    bool active = mActive;
    Mutex::unlockMutex(mActiveMutex);

    return active;
}

//-----------------------------------------------------------------------------

void LeapMotionManager::setActive(bool state)
{
    Mutex::lockMutex(mActiveMutex);
    mActive = state;
    Mutex::unlockMutex(mActiveMutex);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::toggleMouseControl(bool enabledState)
{
    Mutex::lockMutex(mActiveMutex);
    mMouseControl = enabledState;
    Mutex::unlockMutex(mActiveMutex);
}

//-----------------------------------------------------------------------------

bool LeapMotionManager::getMouseControlToggle()
{
    Mutex::lockMutex(mActiveMutex);
    bool mouseControlled = mMouseControl;
    Mutex::unlockMutex(mActiveMutex);

    return mouseControlled;
}

//-----------------------------------------------------------------------------
// You can get and set gesture configuration parameters using the Config object
// obtained from a connected Controller object. The key strings required to
// * identify a configuration parameter include:
//*
//* Key string | Value type | Default value | Units
//* -----------|------------|---------------|------
//* Gesture.Circle.MinRadius | float | 5.0 | mm
//* Gesture.Circle.MinArc | float | 1.5 | radians
//* Gesture.Swipe.MinLength | float | 150 | mm
//* Gesture.Swipe.MinVelocity | float | 1000 | mm/s
//* Gesture.KeyTap.MinDownVelocity | float | 50 | mm/s
//* Gesture.KeyTap.HistorySeconds | float | 0.1 | s
//* Gesture.KeyTap.MinDistance | float | 3.0 | mm
//* Gesture.ScreenTap.MinForwardVelocity  | float | 50 | mm/s
//* Gesture.ScreenTap.HistorySeconds | float | 0.1 | s
//* Gesture.ScreenTap.MinDistance | float | 5.0 | mm
bool LeapMotionManager::configureLeapGesture(const char* configString, const F32 value)
{
    // Get this controller's config.
    Leap::Config config = mController->config();
    
    // Convert and pass the key, along with the value.
    std::string *keyString = new std::string(configString);
    bool success = config.setFloat(*keyString, value);
    
    // Free memory and return the result.
    delete keyString;
    return success;
}

//-----------------------------------------------------------------------------

bool LeapMotionManager::setMinCircleProgress(const F32 value)
{
    mMinCircleProgress = value;
    return true;
}

//-----------------------------------------------------------------------------

void LeapMotionManager::processHand(const Leap::Hand& hand, S32 id)
{
    // Get hand (palm) position
    /*Point3F rawHandPosition;
    Point3I convertedHandPosition;
    
    LeapMotionUtil::convertPosition(hand.palmPosition(), rawHandPosition);
    convertedHandPosition.x = (S32)mFloor(rawHandPosition.x);
    convertedHandPosition.y = (S32)mFloor(rawHandPosition.y);
    convertedHandPosition.z = (S32)mFloor(rawHandPosition.z);*/

    // Get the hand's normal vector and direction
    const Leap::Vector normal = hand.palmNormal();
    const Leap::Vector direction = hand.direction();

    F32 pitch = direction.pitch() * Leap::RAD_TO_DEG;
    F32 roll = normal.roll() * Leap::RAD_TO_DEG;
    F32 yaw = direction.yaw() * Leap::RAD_TO_DEG;

    // Position Event
    InputEvent handPosEvent;

    handPosEvent.deviceInst = 0;
    handPosEvent.iValue = id;
    handPosEvent.fValues[0] = hand.palmPosition().x;
    handPosEvent.fValues[1] = hand.palmPosition().y;
    handPosEvent.fValues[2] = hand.palmPosition().z;
    handPosEvent.deviceType = LeapMotionDeviceType;
    handPosEvent.objType = LM_HANDPOS;
    handPosEvent.objInst = 0;
    handPosEvent.action = SI_LEAP;
    handPosEvent.modifier = 0;

    InputEvent handRotEvent;

    handRotEvent.deviceInst = 0;
    handRotEvent.iValue = id;
    handRotEvent.fValues[0] = yaw;
    handRotEvent.fValues[1] = pitch;
    handRotEvent.fValues[2] = roll;
    handRotEvent.objType = LM_HANDROT;
    handRotEvent.deviceType = LeapMotionDeviceType;
    handRotEvent.objInst = 0;
    handRotEvent.action = SI_LEAP;
    handRotEvent.modifier = 0;

    Game->postEvent(handPosEvent);
    Game->postEvent(handRotEvent);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::processHandPointables(const Leap::PointableList& pointables)
{
    InputEvent pointablePositionEvent;
    pointablePositionEvent.deviceInst = 0;
    pointablePositionEvent.objInst = 0;
    pointablePositionEvent.modifier = 0;
    pointablePositionEvent.deviceType = LeapMotionDeviceType;
    pointablePositionEvent.objType = LM_FINGERPOS;    
    pointablePositionEvent.action = SI_LEAP;
    
    for (int f = 0; f < pointables.count(); ++f)
    {
        Leap::Pointable pointable = pointables[f];
        
        char charHolder[10];
        Leap::Vector tipPosition = pointables[f].tipPosition();

        dItoa((S32)tipPosition.x, charHolder);
        dStrcat(pointablePositionEvent.fingersX, charHolder);
        dStrcat(pointablePositionEvent.fingersX, " ");

        dItoa((S32)tipPosition.y, charHolder);
        dStrcat(pointablePositionEvent.fingersY, charHolder);
        dStrcat(pointablePositionEvent.fingersY, " ");

        dItoa((S32)tipPosition.z, charHolder);
        dStrcat(pointablePositionEvent.fingersZ, charHolder);
        dStrcat(pointablePositionEvent.fingersZ, " ");

        dItoa(pointables[f].id(), charHolder);
        dStrcat(pointablePositionEvent.fingerIDs, charHolder);
        dStrcat(pointablePositionEvent.fingerIDs, " ");        
    }
    
    // Post
    Game->postEvent(pointablePositionEvent);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::processGestures(const Leap::GestureList& gestures)
{
    for (int g = 0; g < gestures.count(); ++g)
    {
        Leap::Gesture gesture = gestures[g];

        switch (gesture.type())
        {
            case Leap::Gesture::TYPE_CIRCLE:
            {
                Leap::CircleGesture circle = gesture;

                if (circle.progress() < mMinCircleProgress)
                    break;

                bool clockWise;

                if (circle.pointable().direction().angleTo(circle.normal()) <= Leap::PI/4) 
                    clockWise = true;
                else 
                    clockWise = false;

                InputEvent event;
            
                event.deviceInst = 0;
                event.iValue = g;
                event.fValues[0] = circle.progress();
                event.fValues[1] = circle.radius();
                event.fValues[2] = clockWise;
                event.fValues[3] = (F32)circle.state();
                event.deviceType = LeapMotionDeviceType;
                event.objType = SI_CIRCLE_GESTURE;
                event.objInst = 0;
                event.action = SI_GESTURE;
                event.modifier = 0;
            
                Game->postEvent(event);
                break;
            }
            case Leap::Gesture::TYPE_SWIPE:
            {
                Leap::SwipeGesture swipe = gesture;

                // Comment to post begin and update swipes
                if (swipe.state() != Leap::Gesture::STATE_STOP)
                {
                    break;
                }

                InputEvent event;
            
                event.deviceInst = 0;
                event.iValue = g;
                event.fValues[0] = (F32)swipe.state();
                event.fValues[1] = swipe.direction().x;
                event.fValues[2] = swipe.direction().y;
                event.fValues[3] = swipe.direction().z;
                event.fValues[4] = swipe.speed();
                event.deviceType = LeapMotionDeviceType;
                event.objType = SI_SWIPE_GESTURE;
                event.objInst = 0;
                event.action = SI_GESTURE;
                event.modifier = 0;
            
                Game->postEvent(event);
                break;
            }
            case Leap::Gesture::TYPE_KEY_TAP:
            {
                Leap::KeyTapGesture tap = gesture;

                InputEvent event;
            
                event.deviceInst = 0;
                event.iValue = g;
                event.fValues[0] = tap.position().x;
                event.fValues[1] = tap.position().y;
                event.fValues[2] = tap.position().z;
                event.fValues[3] = tap.direction().x;
                event.fValues[4] = tap.direction().y;
                event.fValues[5] = tap.direction().z;
                event.deviceType = LeapMotionDeviceType;
                event.objType = SI_KEYTAP_GESTURE;
                event.objInst = 0;
                event.action = SI_GESTURE;
                event.modifier = 0;
            
                Game->postEvent(event);
                break;
            }
            case Leap::Gesture::TYPE_SCREEN_TAP:
            {
                Leap::ScreenTapGesture screentap = gesture;

                InputEvent event;
            
                event.deviceInst = 0;
                event.iValue = g;
                event.fValues[0] = screentap.position().x;
                event.fValues[1] = screentap.position().y;
                event.fValues[2] = screentap.position().z;
                event.fValues[3] = screentap.direction().x;
                event.fValues[4] = screentap.direction().y;
                event.fValues[5] = screentap.direction().z;
                event.deviceType = LeapMotionDeviceType;
                event.objType = SI_SCREENTAP_GESTURE;
                event.objInst = 0;
                event.action = SI_GESTURE;
                event.modifier = 0;
            
                Game->postEvent(event);
                break;
            }
            default:
                Con::warnf("LeapMotionManager::process() - Unknown gesture detected");
                break;
        }
    }
}

//-----------------------------------------------------------------------------

void LeapMotionManager::process(const Leap::Controller& controller)
{
    // Is the manager enabled?
    if (!mEnabled)
        return;

    // Was the leap device activated
    if (!getActive())
        return;

    // Get the current frame
    const Leap::Frame frame = controller.frame();

    if (!frame.isValid())
        return;

    mLastFrame = frame;

    // Get gestures
    const Leap::GestureList gestures = frame.gestures();

    if (!gestures.isEmpty())
        processGestures(gestures);

    if (getMouseControlToggle())
    {
        generateMouseEvent(controller);
        return;
    }

    // Is a hand present?
    if ( !frame.hands().isEmpty() ) 
    {
        for (int h = 0; h < frame.hands().count(); ++h)
        {
            const Leap::Hand hand = frame.hands()[h];

            processHand(hand, h);
            
        }
    }
    
    if (frame.pointables().count())
    {
        const Leap::PointableList pointables = frame.pointables();
        
        if (pointables.count())
            processHandPointables(pointables);
    }
}

//-----------------------------------------------------------------------------

void LeapMotionManager::generateMouseEvent(Leap::Controller const & controller)
{
    
    const Leap::ScreenList screens = controller.locatedScreens();

    // make sure we have a detected screen
    if (screens.isEmpty())
        return;

    const Leap::Screen screen = screens[0];

    // find the first finger or tool
    const Leap::Frame frame = controller.frame();
    const Leap::HandList hands = frame.hands();

    if (hands.isEmpty())
        return;

    const Leap::PointableList pointables = hands[0].pointables();

    if (pointables.isEmpty())
        return;

    const Leap::Pointable firstPointable = pointables[0];

    // get x, y coordinates on the first screen
    const Leap::Vector intersection = screen.intersect( firstPointable, true, 1.0f );

    // if the user is not pointing at the screen all components of
    // the returned vector will be Not A Number (NaN)
    // isValid() returns true only if all components are finite
    if (!intersection.isValid())
        return;

    F32 x = screen.widthPixels() * intersection.x;

    // flip y coordinate to standard top-left origin
    F32 y = screen.heightPixels() * (1.0f - intersection.y);

    // Move the cursor
    Point2I location((S32)x, (S32)y);
    Canvas->setCursorPos(location);

    // Build and postthe mouse event
    MouseMoveEvent TorqueEvent;
    TorqueEvent.xPos = (S32) location.x;
    TorqueEvent.yPos = (S32) location.y;
    Game->postEvent(TorqueEvent);
}

//-----------------------------------------------------------------------------

Vector2 LeapMotionManager::getPointFromProjection(Point3F position)
{
    // Get the screen and projection
    const Leap::Vector pointablePosition(position.x, position.y, position.z);
    const Leap::Screen screen = mController->locatedScreens()[0];
    const Leap::Vector projectedPosition = screen.project(pointablePosition, true, 1.0f);    

    // if the user is not pointing at the screen all components of
    // the returned vector will be Not A Number (NaN)
    // isValid() returns true only if all components are finite
    if (!projectedPosition.isValid())
        return Vector2("");
        
    // Get the screen coordinates
    F32 x = screen.widthPixels() * projectedPosition.x;
        
    // flip y coordinate to standard top-left origin
    F32 y = screen.heightPixels() * (1.0f - projectedPosition.y);
    
    // Build the screenPosition and return it
    Vector2 screenPosition;
    screenPosition.x = x;
    screenPosition.y = y;

    return screenPosition;
}

//-----------------------------------------------------------------------------

Vector2 LeapMotionManager::getPointFromIntersection(S32 pointableID)
{
    // Get the finger via ID and check for validity
    Leap::Pointable lastPointable = mLastFrame.pointable(pointableID);

    if (!lastPointable.isValid())
        return Vector2("");

    // Get the screen and intersection
    const Leap::Screen screen = mController->locatedScreens()[0];
    const Leap::Vector intersection = screen.intersect( lastPointable, true, 1.0f );
    
    // if the user is not pointing at the screen all components of
    // the returned vector will be Not A Number (NaN)
    // isValid() returns true only if all components are finite
    if (!intersection.isValid())
        return Vector2("");
        
    // Get the screen coordinates
    F32 x = screen.widthPixels() * intersection.x;
        
    // flip y coordinate to standard top-left origin
    F32 y = screen.heightPixels() * (1.0f - intersection.y);
    
    // Build the screenPosition and return it
    Vector2 screenPosition;
    screenPosition.x = x;
    screenPosition.y = y;

    return screenPosition;
}

//-----------------------------------------------------------------------------

void LeapMotionManager::MotionListener::onInit(const Leap::Controller& controller)
{
    //Con::printf("MotionListener::onInit()");
}

//-----------------------------------------------------------------------------

void LeapMotionManager::MotionListener::onFrame(const Leap::Controller& controller)
{
    gLeapMotionManager->process(controller);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::MotionListener::onConnect(const Leap::Controller& controller)
{
    gLeapMotionManager->setActive(true);
    controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
    controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::MotionListener::onDisconnect (const Leap::Controller& controller)
{
    gLeapMotionManager->setActive(false);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::MotionListener::onFocusGained(const Leap::Controller& controller)
{
    gLeapMotionManager->setActive(true);
}

//-----------------------------------------------------------------------------

void LeapMotionManager::MotionListener::onFocusLost(const Leap::Controller& controller)
{
    gLeapMotionManager->setActive(false);
}