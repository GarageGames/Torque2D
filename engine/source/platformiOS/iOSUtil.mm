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

#include "iOSUtil.h"
#include "io/stream.h"
#include "io/fileStream.h"
#include "io/memstream.h"
#include "graphics/gPalette.h"
#include "graphics/gBitmap.h"
#include "memory/frameAllocator.h"
#include "console/console.h"
#include "platformiOS/platformiOS.h"

#import <UIKit/UIGraphics.h>
#import <UIKit/UIKit.h>

//For networking bsd and IP
#import <netinet/in.h>
#import <sys/types.h>
#import <sys/socket.h>
#import <ifaddrs.h>
#import <arpa/inet.h>




//Luma:	Orientation support
int giOSGameCurrentOrientation = UIDeviceOrientationLandscapeRight;
TCPObject* gpTCPObject = NULL;
char gszTCPAddress[256];

//-----------------------------------------------------------------------------
void Platform::outputDebugString( const char *string )
{
    fprintf(stderr, "%s", string);
    fprintf(stderr, "\n" );
    fflush(stderr);
}

//--------------------------------------
bool GBitmap::readPNGiPhone(Stream& io_rStream)
{
    int filesize = io_rStream.getStreamSize();
    U8 *buff = new U8[filesize+1024];
    
    CGDataProviderRef data_provider = CGDataProviderCreateWithData(nil, buff, filesize, nil);
    CGImageRef apple_image = CGImageCreateWithPNGDataProvider(data_provider, nil, false, kCGRenderingIntentDefault);
    
    // Choose alpha strategy based on whether the source image has alpha or not.
    int width = CGImageGetWidth(apple_image);
    int height = CGImageGetHeight(apple_image);
    U32 rowBytes = width * 4;
    
    // Set up the row pointers...
    AssertISV(width <= 1024, "Error, cannot load images wider than 1024 pixels!");
    AssertISV(height <= 1024, "Error, cannot load images taller than 1024 pixels!");

    BitmapFormat format = RGBA;
    // actually allocate the bitmap space...
    allocateBitmap(width, height,
                   false,            // don't extrude miplevels...
                   format);          // use determined format...

    U8 *pBase = (U8*)getBits();
    
    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    CGContextRef texture_context = CGBitmapContextCreate(pBase, width, height, 8, rowBytes, color_space, kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(texture_context, CGRectMake(0.0, 0.0, width, height), apple_image);
    
    
    CGImageRelease(apple_image);
    CGDataProviderRelease(data_provider);

    delete [] buff;
    return true;
}


//Luma:	Orientation support
int _iOSGameGetOrientation()
{
    return giOSGameCurrentOrientation;
}
void _iOSGameSetCurrentOrientation(int iOrientation)
{
    giOSGameCurrentOrientation = iOrientation;
}

S32 _iOSGetPortraitTouchoffset()
{
// NOTE: This needs to be changed to acount for different retina sizes, but I am not sure what is going on with it yet
    S32 offset = 0;
    
    S32 deviceType = Con::getIntVariable("$pref::iOS::DeviceType");
    
    bool retinaEnabled = Con::getBoolVariable("$pref::iOS::RetinaEnabled");
    
    if (deviceType == 2)
        offset = 500;
    else if (deviceType == 1)
        offset = retinaEnabled ? 500 : 250;
    else
        offset = retinaEnabled ? 320 : 160;
    
    return offset;
}

//Luma: Ability to get the Local IP (Internal IP) for an iOS as opposed to it's External one
void _iOSGetLocalIP(unsigned char *pcIPString)
{
    int a,b,c,d ; 
    struct ifaddrs* interface;
    char* addr;
    
    if (getifaddrs(&interface) == 0) 
    {
        struct ifaddrs* allInterfaces = interface;
        while (interface != NULL) 
        {
            const struct sockaddr_in* address = (const struct sockaddr_in*) interface->ifa_addr;
            addr = inet_ntoa(address->sin_addr);
            if ((address->sin_family == AF_INET) && (strcmp(addr, "127.0.0.1" )))
            {
                break;
            }
            interface = interface->ifa_next;
        }
        freeifaddrs(allInterfaces);
    }
    
    if(interface)
    {
        sscanf( addr, "%i.%i.%i.%i", &a, &b, &c, &d);
    }
    else
    {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
        
    }
    pcIPString[0] = (unsigned char)a;
    pcIPString[1] = (unsigned char)b;
    pcIPString[2] = (unsigned char)c;
    pcIPString[3] = (unsigned char)d;
}


//Luma: Make sure that the iOS Radio is on before connection via TCP... NOTE: sometimes the Radio wont be ready for immediate use after this is processed... need to see why
static void TCPObjectConnectCallback(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *dataIn, void *info)
{
    if(type == kCFSocketConnectCallBack) 
    {
        if(dataIn) 
        {
            SInt32 error = *((SInt32*)dataIn);
            Con::printf("Error connecting with CFSocker: Error code %d\n",error);
        }
    }
    
    //regardless, we want to connect to the TCPObject if we opened the socket or not so that it can continue its process properly
    if(gpTCPObject)
    {
        gpTCPObject->connect(gszTCPAddress);
        gpTCPObject = NULL;
    }
}
 

//Luma: Make sure that the iOS Radio is on before connection via TCP... NOTE: sometimes the Radio wont be ready for immediate use after this is processed... need to see why
CFSocketRef CreateCFSocketToURLAndPort(const char *ipAddress, U16 port)
{
    CFSocketContext context;
    context.version = 0;
    context.info = NULL;
    context.retain = NULL;
    context.release = NULL;
    context.copyDescription = NULL;
                                    
    CFSocketRef socket = CFSocketCreate(kCFAllocatorDefault,
                                        PF_INET,
                                        SOCK_STREAM,
                                        IPPROTO_TCP,
                                        kCFSocketConnectCallBack,
                                        TCPObjectConnectCallback,
                                        &context);

    struct sockaddr_in addr4;
    memset(&addr4, 0, sizeof(addr4));

    addr4.sin_family = AF_INET;
    addr4.sin_len = sizeof(addr4);
    addr4.sin_port = htons(port);
    inet_aton(ipAddress, &addr4.sin_addr);

    NSData *address = [NSData dataWithBytes:&addr4 length:sizeof(addr4)];
    CFSocketConnectToAddress(socket, (__bridge CFDataRef)address, -1);

    CFRunLoopSourceRef source;
    source = CFSocketCreateRunLoopSource(NULL, socket, 1);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);
    CFRelease(source);
    
    return socket;
}


//Luma: Make sure that the iOS Radio is on before connection via TCP... NOTE: sometimes the Radio wont be ready for immediate use after this is processed... need to see why
void OpeniOSNetworkingAndConnectToTCPObject(TCPObject *psTCPObject, const char *pcAddress)
{
    
    char remoteAddr[256];
    //store TCPObject and Port in globals
    gpTCPObject = psTCPObject;
    if(psTCPObject)
    {
        dStrcpy(gszTCPAddress, pcAddress);
    }
    
    //break up url / port to pass in	
    dStrcpy(remoteAddr, pcAddress);
    U16		port = 80;
    char	*portString = dStrchr(remoteAddr, ':');
    if(portString)
    {
        *portString++ = 0;
        port = dAtoi(portString);
    }

    //call socket create function
    CreateCFSocketToURLAndPort(remoteAddr, port);
}


//Luma: ability to quickly tell if this is an iPhone or an iTouch
bool IsDeviceiPhone(void)
{
    NSString* tmp;
    
    tmp = [[UIDevice currentDevice] model];

    if([[[UIDevice currentDevice] model] isEqualToString: @"iPhone"])
    {
        return true;
    }
    
    return false;
}


ConsoleFunction(IsDeviceiPhone, bool, 1, 1, "Returns TRUE if the device is an iPhone, and FALSE if it is an iPod Touch.\n\n")
{
    return IsDeviceiPhone();
}



ConsoleFunction(OpeniOSRadio, void, 2, 2, "Forces open the iOS radio if given a Torque formatted IP:port combo (255.255.255.255:65535)\n\n")
{
    if(IsDeviceiPhone())
    {
        //don't do anything if we are already doing it with a valid TCPObject (we don't want to overwrite it, and if it is already occurring, then we don't need to do it anyways!)
        if(!gpTCPObject)
        {
            OpeniOSNetworkingAndConnectToTCPObject(NULL, argv[1]);
        }
    }
}
