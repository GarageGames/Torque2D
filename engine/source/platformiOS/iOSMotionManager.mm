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

#import "iOSMotionManager.h"

#include "platformiOS.h"
#include "string/stringBuffer.h"
#include "sim/simBase.h"
#include "game/gameInterface.h"

extern iOSPlatState platState;

static const double kFilterConst = 0.1;

static const double kUpdateInterval = 0.2;

@implementation iOSMotionManager


@synthesize referenceAttitude;
@synthesize accelerometerEnabled;
@synthesize gyroscopeEnabled;

- (id)init
{
    if (!(self = [super init])) return nil;
    
    if(self != NULL)
    {
        accelerometerEnabled = NO;
        gyroscopeEnabled = NO;
        
        motionManager = [[CMMotionManager alloc] init];
        
        if(motionManager.deviceMotionAvailable)
        {
            [motionManager setDeviceMotionUpdateInterval:kUpdateInterval];
        }
        else
        {
            [motionManager setAccelerometerUpdateInterval:kUpdateInterval];
        }
    }
    else
    {
        Con::printf("Could not initialized iOSMotionManager!");
        return 0;
    }
        
    return self;
}



double accelAxes[6];


- (void)enableAccelerometer
{
    accelerometerEnabled = YES;
}

- (void)disableAccelerometer
{
    accelerometerEnabled = NO;
}

- (bool)isAccelerometerActive
{
    return motionManager.accelerometerActive;
}

- (bool)enableGyroscope
{
    if(motionManager.gyroAvailable)
        gyroscopeEnabled = YES;
    else
    {
        Con::errorf("Gyroscope not supported on this device");
        return false;
    }
    
    return true;    
}

- (bool)disableGyroscope
{
    if(motionManager.gyroAvailable)
        gyroscopeEnabled = NO;
    else
    {
        Con::errorf("Gyroscope not supported on this device");
        return false;
    }
    
    return true;
}

- (bool)isGyroAvailable
{
    return motionManager.gyroAvailable;
}

- (bool)isGyroActive
{
    return motionManager.gyroActive;
}

static double filteredAccel[3] = {0, 0, 0};

void (^accelerometerHandler)(CMAccelerometerData*, NSError*) = ^(CMAccelerometerData *accelData, NSError *)
{
    if(gMotionManager.accelerometerEnabled)
    {
        U32 accelAxes[6] = { SI_ACCELX, SI_ACCELY, SI_ACCELZ, SI_GRAVX, SI_GRAVY, SI_GRAVZ };
        
        double userAcc[6];
        
        if(platState.portrait)
        {
        
            filteredAccel[0] = (accelData.acceleration.x * kFilterConst) + (filteredAccel[0] * (1.0 - kFilterConst));
            filteredAccel[1] = (accelData.acceleration.y * kFilterConst) + (filteredAccel[1] * (1.0 - kFilterConst));
            filteredAccel[2] = (accelData.acceleration.z * kFilterConst) + (filteredAccel[2] * (1.0 - kFilterConst));
            
            userAcc[0] = accelData.acceleration.x - filteredAccel[0];
            userAcc[1] = accelData.acceleration.y - filteredAccel[1];
            userAcc[2] = accelData.acceleration.z - filteredAccel[2];
            
            // Assign the non-filtered data to gravity
            userAcc[3] = accelData.acceleration.x;
            userAcc[4] = accelData.acceleration.y;
            userAcc[5] = accelData.acceleration.z;
        }
        else 
        {
            filteredAccel[0] = (accelData.acceleration.y * kFilterConst) + (filteredAccel[0] * (1.0 - kFilterConst));
            filteredAccel[1] = (accelData.acceleration.x * kFilterConst) + (filteredAccel[1] * (1.0 - kFilterConst));
            filteredAccel[2] = (accelData.acceleration.z * kFilterConst) + (filteredAccel[2] * (1.0 - kFilterConst));

            userAcc[0] = accelData.acceleration.y - filteredAccel[0];
            userAcc[1] = accelData.acceleration.x - filteredAccel[1];
            userAcc[2] = accelData.acceleration.z - filteredAccel[2];
            
            // Assign the non-filtered data to gravity
            userAcc[3] = accelData.acceleration.y;
            userAcc[4] = accelData.acceleration.x;
            userAcc[5] = accelData.acceleration.z;
        }
        
        for( int i = 0; i < 6; i++)
        {
            InputEvent event;
            
            event.deviceInst = 0;
            event.fValues[0] = userAcc[i];
            event.deviceType = AccelerometerDeviceType;
            event.objType = accelAxes[i];
            event.objInst = i;
            event.action = SI_MOTION;
            event.modifier = 0;
            
            Game->postEvent(event);
        }
    }
};

void (^motionHandler)(CMDeviceMotion*, NSError*) = ^(CMDeviceMotion *motionData, NSError *error)
{
    if(gMotionManager.referenceAttitude == NULL)
        [gMotionManager resetDeviceMotionReference];
    
    CMAttitude* currentAttitude = motionData.attitude;
  
    [currentAttitude multiplyByInverseOfAttitude:gMotionManager.referenceAttitude];
  
    if(gMotionManager.accelerometerEnabled)
    {
        U32 accelAxes[6] = { SI_ACCELX, SI_ACCELY, SI_ACCELZ, SI_GRAVX, SI_GRAVY, SI_GRAVZ };
        
        double userAcc[6];
        if(platState.portrait)
        {
            userAcc[0] = motionData.userAcceleration.x; 
            userAcc[1] = motionData.userAcceleration.y;
            userAcc[2] = motionData.userAcceleration.z;
            userAcc[3] = motionData.gravity.x; 
            userAcc[4] = motionData.gravity.y;
            userAcc[5] = motionData.gravity.z;
        }
        else 
        {
            userAcc[0] = motionData.userAcceleration.y; 
            userAcc[1] = motionData.userAcceleration.x;
            userAcc[2] = motionData.userAcceleration.z;
            userAcc[3] = motionData.gravity.y; 
            userAcc[4] = motionData.gravity.x;
            userAcc[5] = motionData.gravity.z;            
        }

        for( int i = 0; i < 6; i++)
        {
            InputEvent event;
        
            event.deviceInst = 0;
            event.fValues[0] = userAcc[i];
            event.deviceType = AccelerometerDeviceType;
            event.objType = accelAxes[i];
            event.objInst = i;
            event.action = SI_MOTION;
            event.modifier = 0;
        
            Game->postEvent(event);
        }
    }
    
    if(gMotionManager.gyroscopeEnabled)
    {
        double gyroData[6] = { currentAttitude.pitch, 
                               currentAttitude.yaw, 
                               currentAttitude.roll, 
                               motionData.rotationRate.x, 
                               motionData.rotationRate.y, 
                               motionData.rotationRate.z };
        
        U32 gyroAxes[6] = { SI_PITCH, SI_YAW, SI_ROLL, SI_GYROX, SI_GYROY, SI_GYROZ };
        
        for( int i = 0; i < 6; i++)
        {
            InputEvent event;
            
            event.deviceInst = 0;
            event.fValues[0] = gyroData[i];
            event.deviceType = GyroscopeDeviceType;
            event.objType = gyroAxes[i];
            event.objInst = i;
            event.action = SI_MOTION;
            event.modifier = 0;
            
            Game->postEvent(event);
        }
    }
    
};

- (bool)startDeviceMotion
{
    if(motionManager.deviceMotionAvailable)
    {
        if(referenceAttitude == NULL)
            referenceAttitude = motionManager.deviceMotion.attitude;
        
        [motionManager startDeviceMotionUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:motionHandler];
    }
    else
    {
        [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:accelerometerHandler];
    }
    
    return true;
}

- (bool)stopDeviceMotion
{
    if(motionManager.deviceMotionAvailable)
        [motionManager stopDeviceMotionUpdates];
    else
    {
        [motionManager stopAccelerometerUpdates];
    }
    return true;
}

- (bool)resetDeviceMotionReference
{
    if(motionManager.deviceMotionAvailable)
    {
        referenceAttitude = motionManager.deviceMotion.attitude;
        return true;
    }
    
    Con::errorf("Device Motion not supported on this device (check OS)");
    return false;
}

- (bool)isDeviceMotionAvailable
{
    return motionManager.deviceMotionAvailable;
}

- (bool)isDeviceMotionActive
{
    return motionManager.deviceMotionActive;
}

@end

ConsoleFunction(initMotionManager, void, 1, 1, "() Initialize the iOSMotionManager")
{
    if(gMotionManager != NULL)
    {
        Con::printf("Motion Manager already initialized");
    }
    else
        gMotionManager = [[iOSMotionManager alloc] init];
}

ConsoleFunction(enableAccelerometer, void, 1, 1, "() Allow accelerometer tracking during device motion updates")
{
    if(gMotionManager == NULL)
        gMotionManager = [[iOSMotionManager alloc] init];
    
    gMotionManager.accelerometerEnabled = YES;
    [gMotionManager startDeviceMotion];
}

ConsoleFunction(disableAccelerometer, void, 1, 1, "() Stop accelerometer tracking")
{
    if(gMotionManager != NULL)
        gMotionManager.accelerometerEnabled = NO;
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
    }
}

ConsoleFunction(isAccelerometerActive, bool, 1, 1, "() Check to see if Accelerometer is being polled\n"
                "@return True if accelerometer is on, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager isAccelerometerActive];
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
        return [gMotionManager isAccelerometerActive];
    }
}

ConsoleFunction(isGyroAvailable, bool, 1, 1, "() Check to see if this iOS device has a gyroscope\n"
                                             "@return True if gyro is on the device, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager isGyroAvailable];
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
        return [gMotionManager isGyroAvailable];
    }
}

ConsoleFunction(isGyroActive, bool, 1, 1, "() Check to see if this iOS device has a gyroscope\n"
                "@return True if gyro is on the device, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager isGyroActive];
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
        return [gMotionManager isGyroActive];
    }
}
ConsoleFunction(enableGyroscope, void, 1, 1, "() Start the gyroscope tracking\n"
                "@return True if gyroscope is supported, false otherwise")
{
    if(gMotionManager != NULL)
        gMotionManager.gyroscopeEnabled = YES;
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        
        gMotionManager = [[iOSMotionManager alloc] init];
        gMotionManager.gyroscopeEnabled = YES;
        [gMotionManager startDeviceMotion];
    }
}

ConsoleFunction(stopGyroscope, void, 1, 1, "() Stop gyroscope tracking\n"
                "@return True if gyroscope is supported, false otherwise")
{
    if(gMotionManager != NULL)
        gMotionManager.gyroscopeEnabled = NO;
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        
        gMotionManager = [[iOSMotionManager alloc] init];
    }
}

ConsoleFunction(isDeviceMotionAvailable, bool, 1, 1, "() Check to see if this iOS device supports advanced device motion (requires gyroscope\n"
                "@return True if Device Motion is supported, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager isDeviceMotionAvailable];
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
        return [gMotionManager isDeviceMotionAvailable];
    }
}

ConsoleFunction(isDeviceMotionActive, bool, 1, 1, "() Check to see if the device motion is running\n"
                                                  "@return True if device motion is being tracked, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager isDeviceMotionActive];
    
    Con::warnf("Motion Manager was not initialized. Initializing now");
    gMotionManager = [[iOSMotionManager alloc] init];
    return [gMotionManager isDeviceMotionActive];
}
ConsoleFunction(startDeviceMotion, bool, 1, 1, "() Start Device motion tracking\n"
                                               "@return True if device motion is supported, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager startDeviceMotion];
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
        return [gMotionManager startDeviceMotion];
    }
}

ConsoleFunction(stopDeviceMotion, bool, 1, 1, "() Stop Device Motion tracking\n"
                                              "@return True if device motion is supported, false otherwise")
{
    if(gMotionManager != NULL)
        return [gMotionManager stopDeviceMotion];
    else
    {
        Con::warnf("Motion Manager was not initialized. Initializing now");
        gMotionManager = [[iOSMotionManager alloc] init];
        return [gMotionManager stopDeviceMotion];
    }
}
