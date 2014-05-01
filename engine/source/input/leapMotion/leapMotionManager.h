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
#define _LEAPMOTIONMANAGER_H_

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _LEAPMOTIONCONSTANTS_H_
#include "input/leapMotion/leapMotionConstants.h"
#endif

#ifndef __Leap_h__
#include "Leap.h"
#endif


class LeapMotionManager
{
protected:
    
    bool mEnabled;
    F32 mMinCircleProgress;

    struct FingerEvent
    {
        S32 id;
        F32 x;
        F32 y;
        F32 z;

        FingerEvent( S32 ID, F32 X, F32 Y, F32 Z)
        {
            id = ID;
            x = X;
            y = Y;
            z = Z;
        }
    };

    class MotionListener : public Leap::Listener
    {
    public:
        MotionListener() {}
        virtual ~MotionListener() {}

        virtual void onConnect (const Leap::Controller& controller);
        virtual void onDisconnect (const Leap::Controller& controller);
        virtual void onInit(const Leap::Controller& controller);
        virtual void onFrame(const Leap::Controller& controller);
        virtual void onFocusGained(const Leap::Controller& controller);
        virtual void onFocusLost(const Leap::Controller& controller);
   };

    /// The connection to the Leap Motion
    Leap::Controller* mController;

    /// Our Leap Motion listener class
    MotionListener* mListener;

    /// Used with the LM listener object
    void* mActiveMutex;

    /// Is the Leap Motion active
    bool mActive;

    /// Is the Manager acting like a mouse
    bool mMouseControl;

    /// Last stored frame
    Leap::Frame mLastFrame;

public:
    static bool smEnableDevice;

    // Indicates that events for each hand and pointable will be created
    static bool smGenerateIndividualEvents;

    // Indicates that we track hand IDs and will ensure that the same hand
    // will remain at the same index between frames.
    static bool smKeepHandIndexPersistent;

    // Indicates that we track pointable IDs and will ensure that the same
    // pointable will remain at the same index between frames.
    static bool smKeepPointableIndexPersistent;

    // Broadcast single hand rotation as axis
    static bool smGenerateSingleHandRotationAsAxisEvents;

    // The maximum hand angle when used as an axis event
    // as measured from a vector pointing straight up (in degrees)
    static F32 smMaximumHandAxisAngle;

    // Indicates that a whole frame event should be generated and frames
    // should be buffered.
    static bool smGenerateWholeFrameEvents;

    // Frame action codes
    static U32 LM_FRAMEVALIDDATA;    // SI_BUTTON

    // Hand action codes
    static U32 LM_HAND[LeapMotionConstants::MaxHands];    // SI_POS
    //static U32 LM_HANDROT[LeapMotionConstants::MaxHands]; // SI_ROT

    static U32 LM_HANDAXISX;   // SI_AXIS
    static U32 LM_HANDAXISY;

    // Pointables action codes
    static U32 LM_HANDPOINTABLE[LeapMotionConstants::MaxHands][LeapMotionConstants::MaxPointablesPerHand];    // SI_POS
    static U32 LM_HANDPOINTABLEROT[LeapMotionConstants::MaxHands][LeapMotionConstants::MaxPointablesPerHand]; // SI_ROT

    // Whole frame
    static U32 LM_FRAME;    // SI_INT

public:
    LeapMotionManager();
    ~LeapMotionManager();

    static void staticInit();

    void enable(bool enabledState);
    void disable();
    bool getEnabled() { return mEnabled; }
    bool getActive();
    void setActive(bool enabledState);
    void toggleMouseControl(bool enabledState);
    bool getMouseControlToggle();
    bool setMinCircleProgress(const F32 value);
    F32 getMinCircleProgress() { return mMinCircleProgress; }
    bool configureLeapGesture(const char* configString, const F32 value);
    void process(const Leap::Controller& controller);
    void processHand(const Leap::Hand& hand, S32 id);
    void processHandPointables(const Leap::PointableList& pointables);
    void processGestures(const Leap::GestureList& gestures);
    void generateMouseEvent(const Leap::Controller& controller);
    Vector2 getPointFromProjection(Point3F position);
    Vector2 getPointFromIntersection(S32 pointableID);
};

static LeapMotionManager* gLeapMotionManager;

#endif