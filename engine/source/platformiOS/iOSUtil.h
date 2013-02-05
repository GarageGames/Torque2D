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

#ifndef _iOSUTIL_H_
#define _iOSUTIL_H_


#include "network/tcpObject.h"



//Luma:	Orientation support
/*enum iOSOrientation
{
	iOSOrientationUnkown,				//All applications start in this state
	iOSOrientationLandscapeLeft,			//The home button is on the RIGHT
	iOSOrientationLandscapeRight,		//The home button is on the LEFT
	iOSOrientationPortrait,				//The home button is on the bottom
	iOSOrientationPortraitUpsideDown		//The home button is on the top
};*/

int _iOSGameGetOrientation();	
void _iOSGameSetCurrentOrientation(int iOrientation);	
S32 _iOSGetPortraitTouchoffset();

//Luma: Ability to get the Local IP (Internal IP) for an iOS as opposed to it's External one
void _iOSGetLocalIP(unsigned char *pcIPString);

//Luma: Make sure that the iOS Radio is on before connection via TCP... NOTE: sometimes the Radio wont be ready for immediate use after this is processed... need to see why
void OpeniOSNetworkingAndConnectToTCPObject(TCPObject *psTCPObject, const char *pcAddress);

//Luma: ability to quickly tell if this is an iOS or an iTouch
bool IsDeviceiPhone(void);


#endif // _iOSUTIL_H_
