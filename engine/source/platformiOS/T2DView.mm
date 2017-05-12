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

#import "platformiOS/T2DView.h"
//#import <QuartzCore/QuartzCore.h>
//#include "platformiPhone/iPhoneEvents.h"

#import "platformiOS/platformiOS.h"
#import "platformiOS/platformGL.h"
#import "platformiOS/iOSOGLVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/fileStream.h"
#include "platformiOS/iOSUtil.h"
#include "platformiOS/iOSEvents.h"
#include "graphics/dgl.h"
#include "debug/profiler.h"

#define USE_DEPTH_BUFFER 0

extern bool createMouseMoveEvent(S32 i, S32 x, S32 y, S32 lastX, S32 lastY);
extern bool createMouseDownEvent(S32 touchNumber, S32 x, S32 y, U32 numTouches);
extern bool createMouseUpEvent(S32 touchNumber, S32 x, S32 y, S32 lastX, S32 lastY, U32 numTouches); //EFM
extern void createMouseTapEvent( S32 nbrTaps, S32 x, S32 y );

bool retinaEnabled;

void ConvertToRetina(CGPoint *p)
{
    CGFloat screenScale = [[UIScreen mainScreen] scale];
    p->x *= screenScale;
    p->y *= screenScale;
}

@implementation T2DView

@synthesize context = _context;
@synthesize currentAngle;
@synthesize isLayedOut;

-(void)swapBuffers {
	if( self.isLayedOut == true ) {
		[self.context presentRenderbuffer:GL_RENDERBUFFER_OES];
	}
}

- (void)layoutSubviews {
}

-(void)rotateByAngle:(CGFloat)angle {
	
	CGAffineTransform transform = self.transform;
    
	transform = CGAffineTransformRotate(transform, angle);
	self.transform = transform;
}

-(void)rotateToAngle:(CGFloat)angle {
	CGFloat rotateAmount = (angle - currentAngle);//need to make this work better
	if( rotateAmount == 0 ) {
		return;
	}
	currentAngle = angle;
	[self rotateByAngle:rotateAmount];
}

-(void)centerOnPoint:(CGPoint)point {
	self.center = point;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    
    NSUInteger touchCount = 0;
    // Enumerates through all touch objects
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self];
        
        if (retinaEnabled)
        {
            ConvertToRetina(&point);
        }
        
        S32 orientation = _iOSGameGetOrientation();
        if (UIDeviceOrientationIsPortrait((UIDeviceOrientation)orientation))
        {
            point.y -= _iOSGetPortraitTouchoffset();
        }
        
        int numTouches = [touch tapCount];
        createMouseDownEvent(touchCount, point.x, point.y, numTouches);
        touchCount++;
    }
}


extern Vector<Event *> TouchMoveEvents;


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    
    NSUInteger touchCount = 0;
    // Enumerates through all touch objects
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self];
        CGPoint prevPoint = [touch previousLocationInView:self]; //EFM
        
        if (retinaEnabled)
        {
            ConvertToRetina(&point);
            ConvertToRetina(&prevPoint);
        }
        
        S32 orientation = _iOSGameGetOrientation();
        if (UIDeviceOrientationIsPortrait((UIDeviceOrientation)orientation))
        {
            point.y -= _iOSGetPortraitTouchoffset();
            prevPoint.y -= _iOSGetPortraitTouchoffset();
        }
        
        createMouseMoveEvent(touchCount, point.x, point.y, prevPoint.x, prevPoint.y);
        touchCount++;
    }
    
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    
    NSUInteger touchCount = 0;
    // Enumerates through all touch objects
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self];
        CGPoint prevPoint = [touch previousLocationInView:self]; //EFM
        
        if (retinaEnabled)
        {
            ConvertToRetina(&point);
            ConvertToRetina(&prevPoint);
        }
        
        S32 orientation = _iOSGameGetOrientation();
        if (UIDeviceOrientationIsPortrait((UIDeviceOrientation)orientation))
        {
            point.y -= _iOSGetPortraitTouchoffset();
            prevPoint.y -= _iOSGetPortraitTouchoffset();
        }
        
        int tc = [touch tapCount];
        createMouseUpEvent(touchCount, point.x, point.y, prevPoint.x, prevPoint.y, tc);
        touchCount++;
        
        //Luma: Tap support
        
        if (tc > 0)
        {
            // this was a tap, so create a tap event
            createMouseTapEvent(tc, (int) point.x, (int) point.y);
        }
    }
    
}

- (void)touchesChangedWithEvent:(UIEvent *)event
{
    Con::printf("Touches Changed with Event");
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchesEnded:touches withEvent:event];
}

bool gScreenAutoRotate = false;
int currentRotate = 0;
#define ROTATE_LEFT		0
#define ROTATE_RIGHT	1
#define ROTATE_UP		2
@end
