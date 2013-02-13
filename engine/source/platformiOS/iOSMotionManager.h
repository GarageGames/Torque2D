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

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>
#import <CoreMotion/CoreMotion.h>

@interface iOSMotionManager: NSObject
{
    // The sole CMMotionManager reference
    CMMotionManager* motionManager;
    
    // The starting attitude reference DeviceMotion will use
    CMAttitude* referenceAttitude;
    
    bool accelerometerEnabled;
    bool gyroscopeEnabled;
}

@property (readwrite, assign) bool accelerometerEnabled;
@property (readwrite, assign) bool gyroscopeEnabled;

@property (strong) CMAttitude* referenceAttitude;

// Accelerometer related functions
- (void) enableAccelerometer;
- (void) disableAccelerometer;
- (bool) isAccelerometerActive;

// Gyroscope related functions
- (bool) enableGyroscope;
- (bool) disableGyroscope;
- (bool) isGyroAvailable;
- (bool) isGyroActive;

// Motion device related functions
- (bool) startDeviceMotion;
- (bool) stopDeviceMotion;
- (bool) resetDeviceMotionReference;
- (bool) isDeviceMotionAvailable;
- (bool) isDeviceMotionActive;

@end

static iOSMotionManager* gMotionManager;