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


#include "platformiOS/platformiOS.h"
#include "platform/platformSemaphore.h"
#include "platform/platformVideo.h"
#include "platform/threads/thread.h"
#include "console/console.h"
#include "platformiOS/iOSEvents.h"
#include "platform/nativeDialogs/msgBox.h"

#include "platformiOS/iOSAlerts.h"


@implementation iOSAlertDelegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	buttonNumber = buttonIndex;
}

//Luma: Added delegate for dismissed call by system
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
	buttonNumber = buttonIndex;
}


- (void)didPresentAlertView:(UIAlertView *)alertView {
	
}

@end



bool iOSButtonBox(const char *windowTitle, const char *message, int numButtons = 0, NSArray *buttons = nil, iOSAlertDelegate *delegate = nil)
{
    
	UIAlertView *Alert =  [[UIAlertView alloc] initWithTitle: @(windowTitle)
													 message: @(message)
													delegate: delegate
										   cancelButtonTitle: nil
										   otherButtonTitles: nil ];
	
	if(numButtons > 0)
	{
		NSString *current = nil;
		for( int i = 1;  i < numButtons ; i++ )
		{
			current = buttons[i];
			[Alert addButtonWithTitle: current ];
		}
	}
	else
	{
		[Alert addButtonWithTitle: @"OK" ];
	}
    
	
	[Alert show];
	
	// PUAP -Mat NOTE: NSRunLoop is not Thread-Safe, see documentation
    
	while (Alert.visible)
	{
		[[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: [NSDate dateWithTimeIntervalSinceNow: 0.100]];
	}
	
	
	return true;
}



//-----------------------------------------------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
	iOSAlertDelegate *delegate = [[iOSAlertDelegate alloc] init];
	
	iOSButtonBox( windowTitle, message, 0, nil, delegate );
	
}
//-----------------------------------------------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
	iOSAlertDelegate *delegate = [[iOSAlertDelegate alloc] init];
	
    
	NSArray *buttons = @[ @"OK", @"Cancel" ];
	
	//Luma:	Need to pass the delegate in as well
	iOSButtonBox( windowTitle, message, 2, buttons, delegate );
	
	//Luma: Zero is NOT the cancel button index... it is based on the order of the buttons in the above array
	bool returnValue = (delegate->buttonNumber != 1 );
	return returnValue;
}

//-----------------------------------------------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{//retry/cancel
	iOSAlertDelegate *delegate = [[iOSAlertDelegate alloc] init];
	
	//Luma:	Should be Retry / Cancel, not Cancel / Retry
    NSArray  *buttons = @[@"Retry", @"Cancel"];
    
	//Luma:	Need to pass the delegate in as well
	iOSButtonBox( windowTitle, message, 2, buttons, delegate );
	
	//Luma: Zero is NOT the cancel button index... it is based on the order of the buttons in the above array
	bool returnValue = (delegate->buttonNumber != 1 );
	return returnValue;
}


bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{
	iOSAlertDelegate *delegate = [[iOSAlertDelegate alloc] init];
	
    NSArray  *buttons = @[@"Yes", @"No"];
	
	iOSButtonBox( windowTitle, message, 2, buttons, delegate );
	bool returnValue = (delegate->buttonNumber != 1 );
	
	return returnValue;
}
