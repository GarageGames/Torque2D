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
/// @file
/// Library-wide input events
///
/// All external events are converted into system events, which are defined
/// in this file.
#ifndef _EVENT_H_
#define _EVENT_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif

#ifndef _PLATFORM_MEMORY_H_
#include "platform/platformMemory.h"
#endif

typedef int NetConnectionId;

/// Generic network address
///
/// This is used to represent IP addresses.
struct NetAddress {
   int type;        ///< Type of address (IPAddress currently)

   /// Acceptable NetAddress types.
   enum {
      IPAddress,
      IPXAddress
   };

   U8 netNum[4];    ///< For IP:  sin_addr<br>
                    ///  For IPX: sa_netnum

   U8 nodeNum[6];   ///< For IP:  Not used.<br>
                    ///  For IPX: sa_nodenum (not used by IP)

   U16  port;       ///< For IP:  sin_port<br>
                    ///  For IPX: sa_socket
};

enum EventConstants
{
   MaxPacketDataSize = 1500,    ///< Maximum allowed size of a packet.
   MaxConsoleLineSize = 512     ///< Maximum allowed size of a console line.
};

/// Available event types.
enum EventTypes
{
   InputEventType,
   MouseMoveEventType,
   ScreenTouchEventType,
   PacketReceiveEventType,
   TimeEventType,
   QuitEventType,
   ConsoleEventType,
   ConnectedReceiveEventType,
   ConnectedAcceptEventType,
   ConnectedNotifyEventType
};

/// Base event structure (also used for FrameEvent and QuitEvent)
struct Event
{
   U16 type, size;
   Event() { size = sizeof(Event); }
};

/// The time event causes the simulation to advance.
struct TimeEvent : public Event
{
   U32 elapsedTime; ///< Indicates elapsed time in simulation.

   TimeEvent() { type = TimeEventType; size = sizeof(TimeEvent); }
};

/// Notify simulation of state of a connection.
struct ConnectedNotifyEvent : public Event
{
   /// Valid connection states
   enum State {
      DNSResolved,
      DNSFailed,
      Connected,
      ConnectFailed,
      Disconnected
   };

   U32 state;   ///< Indicates current connection state.
   U32 tag;     ///< Identifies connection.
   ConnectedNotifyEvent() { type = ConnectedNotifyEventType; size = sizeof(ConnectedNotifyEvent); }
};

/// Triggered when we receive data from a connected entity.
struct ConnectedReceiveEvent : public Event
{
   U32 tag;     ///< Identifies connection
   U8 data[MaxPacketDataSize];  /// Payload
   ConnectedReceiveEvent() { type = ConnectedReceiveEventType; }
};

/// Triggered when we accept a new connection.
struct ConnectedAcceptEvent : public Event
{
   U32 portTag;         ///< Identifies port we received this connection on.
   U32 connectionTag;   ///< Identifies the connection.
   NetAddress address;  ///< Originating address of connection.
   ConnectedAcceptEvent() { type = ConnectedAcceptEventType; size = sizeof(ConnectedAcceptEvent); }
};


/// Triggered on incoming (UDP) packet
///
/// packetType is what type of packet it is.
struct PacketReceiveEvent : public Event
{
   NetAddress sourceAddress;   ///< Originating address.
   U8 data[MaxPacketDataSize]; ///< Payload
   PacketReceiveEvent() { type = PacketReceiveEventType; }
};

/// Represents a line of console input.
struct ConsoleEvent : public Event
{
   char data[MaxConsoleLineSize];   ///< Payload
   ConsoleEvent() { type = ConsoleEventType; }
};

/// Header sizes for events defined later on.
/// Byte offset to payload of a PacketReceiveEvent
const U32 PacketReceiveEventHeaderSize = Offset(data,PacketReceiveEvent);
/// Byte offset to payload of a ConnectedReceiveEvent
const U32 ConnectedReceiveEventHeaderSize = Offset(data,ConnectedReceiveEvent);
/// Byte offset to payload of a ConsoleEvent
const U32 ConsoleEventHeaderSize = Offset(data,ConsoleEvent);


/// Mouse input event.
struct MouseMoveEvent : public Event
{
   S32 xPos, yPos;
   U8 modifier;

   MouseMoveEvent() { type = MouseMoveEventType; size = sizeof(MouseMoveEvent); }
};
struct ScreenTouchEvent : public Event  
{  
    S32 xPos, yPos;  
    S32 touchID;
    U8    action;
    U32 numTouches;
    
    ScreenTouchEvent() { type = ScreenTouchEventType; size = sizeof(ScreenTouchEvent); }  
};
/// Generic input event.
struct InputEvent : public Event
{
   U32   deviceInst;  ///< Device instance: joystick0, joystick1, etc
   S32 iValue;        ///< Handy for tracking IDs of things like fingers, hands, etc
   float fValues[7];  ///< Stores the evemt data. Sometimes only one with a range of -1.0 - 1.0 is needed, other times it might be multiple vectors
   U16   deviceType;  ///< One of mouse, keyboard, joystick, unknown
   U16   objType;     ///< One of SI_XAXIS, SI_BUTTON, SI_KEY ...
   U16   ascii;       ///< ASCII character code if this is a keyboard event.
   U16   objInst;     ///< Which type instance or a KeyCode
   U8    action;      ///< What was the action? (MAKE/BREAK/MOVE)
   U8    modifier;    ///< Modifier to action: SI_LSHIFT, SI_LCTRL, etc.

   // iOS specific
   char fingersX[256];    ///< Collection of x-coordinates for fingers
   char fingersY[256];    ///< Collection of y-coordinates for fingers
   char fingersZ[256];    ///< Collection of Z-coordinates for fingers

   char fingerIDs[256];    ///< Collection of touch IDs
    
    InputEvent()
    { 
        type = InputEventType; 
        size = sizeof(InputEvent); 
        deviceInst = 0;
        iValue     = -1;
        objType    = 0;
        ascii      = 0;
        objInst    = 0;
        action     = 0;
        modifier   = 0;
        dMemset(fValues, 0, sizeof(fValues));
        dMemset(fingersX, 0, sizeof(fingersX));
        dMemset(fingersY, 0, sizeof(fingersY));
        dMemset(fingersZ, 0, sizeof(fingersZ));
        dMemset(fingerIDs, 0, sizeof(fingerIDs));
    }

};

/// @defgroup input_constants Input system constants
/// @{

/// Input event constants:
enum KeyCodes {
   KEY_NULL          = 0x000,     ///< Invalid KeyCode
   KEY_BACKSPACE     = 0x001,
   KEY_TAB           = 0x002,
   KEY_RETURN        = 0x003,
   KEY_CONTROL       = 0x004,
   KEY_ALT           = 0x005,
   KEY_SHIFT         = 0x006,
   KEY_PAUSE         = 0x007,
   KEY_CAPSLOCK      = 0x008,
   KEY_ESCAPE        = 0x009,
   KEY_SPACE         = 0x00a,
   KEY_PAGE_DOWN     = 0x00b,
   KEY_PAGE_UP       = 0x00c,
   KEY_END           = 0x00d,
   KEY_HOME          = 0x00e,
   KEY_LEFT          = 0x00f,
   KEY_UP            = 0x010,
   KEY_RIGHT         = 0x011,
   KEY_DOWN          = 0x012,
   KEY_PRINT         = 0x013,
   KEY_INSERT        = 0x014,
   KEY_DELETE        = 0x015,
   KEY_HELP          = 0x016,

   KEY_0             = 0x017,
   KEY_1             = 0x018,
   KEY_2             = 0x019,
   KEY_3             = 0x01a,
   KEY_4             = 0x01b,
   KEY_5             = 0x01c,
   KEY_6             = 0x01d,
   KEY_7             = 0x01e,
   KEY_8             = 0x01f,
   KEY_9             = 0x020,

   KEY_A             = 0x021,
   KEY_B             = 0x022,
   KEY_C             = 0x023,
   KEY_D             = 0x024,
   KEY_E             = 0x025,
   KEY_F             = 0x026,
   KEY_G             = 0x027,
   KEY_H             = 0x028,
   KEY_I             = 0x029,
   KEY_J             = 0x02a,
   KEY_K             = 0x02b,
   KEY_L             = 0x02c,
   KEY_M             = 0x02d,
   KEY_N             = 0x02e,
   KEY_O             = 0x02f,
   KEY_P             = 0x030,
   KEY_Q             = 0x031,
   KEY_R             = 0x032,
   KEY_S             = 0x033,
   KEY_T             = 0x034,
   KEY_U             = 0x035,
   KEY_V             = 0x036,
   KEY_W             = 0x037,
   KEY_X             = 0x038,
   KEY_Y             = 0x039,
   KEY_Z             = 0x03a,

   KEY_TILDE         = 0x03b,
   KEY_MINUS         = 0x03c,
   KEY_EQUALS        = 0x03d,
   KEY_LBRACKET      = 0x03e,
   KEY_RBRACKET      = 0x03f,
   KEY_BACKSLASH     = 0x040,
   KEY_SEMICOLON     = 0x041,
   KEY_APOSTROPHE    = 0x042,
   KEY_COMMA         = 0x043,
   KEY_PERIOD        = 0x044,
   KEY_SLASH         = 0x045,
   KEY_NUMPAD0       = 0x046,
   KEY_NUMPAD1       = 0x047,
   KEY_NUMPAD2       = 0x048,
   KEY_NUMPAD3       = 0x049,
   KEY_NUMPAD4       = 0x04a,
   KEY_NUMPAD5       = 0x04b,
   KEY_NUMPAD6       = 0x04c,
   KEY_NUMPAD7       = 0x04d,
   KEY_NUMPAD8       = 0x04e,
   KEY_NUMPAD9       = 0x04f,
   KEY_MULTIPLY      = 0x050,
   KEY_ADD           = 0x051,
   KEY_SEPARATOR     = 0x052,
   KEY_SUBTRACT      = 0x053,
   KEY_DECIMAL       = 0x054,
   KEY_DIVIDE        = 0x055,
   KEY_NUMPADENTER   = 0x056,

   KEY_F1            = 0x057,
   KEY_F2            = 0x058,
   KEY_F3            = 0x059,
   KEY_F4            = 0x05a,
   KEY_F5            = 0x05b,
   KEY_F6            = 0x05c,
   KEY_F7            = 0x05d,
   KEY_F8            = 0x05e,
   KEY_F9            = 0x05f,
   KEY_F10           = 0x060,
   KEY_F11           = 0x061,
   KEY_F12           = 0x062,
   KEY_F13           = 0x063,
   KEY_F14           = 0x064,
   KEY_F15           = 0x065,
   KEY_F16           = 0x066,
   KEY_F17           = 0x067,
   KEY_F18           = 0x068,
   KEY_F19           = 0x069,
   KEY_F20           = 0x06a,
   KEY_F21           = 0x06b,
   KEY_F22           = 0x06c,
   KEY_F23           = 0x06d,
   KEY_F24           = 0x06e,

   KEY_NUMLOCK       = 0x06f,
   KEY_SCROLLLOCK    = 0x070,
   KEY_LCONTROL      = 0x071,
   KEY_RCONTROL      = 0x072,
   KEY_LALT          = 0x073,
   KEY_RALT          = 0x074,
   KEY_LSHIFT        = 0x075,
   KEY_RSHIFT        = 0x076,
   KEY_WIN_LWINDOW   = 0x077,
   KEY_WIN_RWINDOW   = 0x078,
   KEY_WIN_APPS      = 0x079,
   KEY_OEM_102       = 0x080,

   KEY_MAC_OPT       = 0x090,
   KEY_MAC_LOPT      = 0x091,
   KEY_MAC_ROPT      = 0x092,

   KEY_BUTTON0       = 0x0100,
   KEY_BUTTON1       = 0x0101,
   KEY_BUTTON2       = 0x0102,
   KEY_BUTTON3       = 0x0103,
   KEY_BUTTON4       = 0x0104,
   KEY_BUTTON5       = 0x0105,
   KEY_BUTTON6       = 0x0106,
   KEY_BUTTON7       = 0x0107,
   KEY_BUTTON8       = 0x0108,
   KEY_BUTTON9       = 0x0109,
   KEY_BUTTON10      = 0x010A,
   KEY_BUTTON11      = 0x010B,
   KEY_BUTTON12      = 0x010C,
   KEY_BUTTON13      = 0x010D,
   KEY_BUTTON14      = 0x010E,
   KEY_BUTTON15      = 0x010F,
   KEY_BUTTON16      = 0x0110,
   KEY_BUTTON17      = 0x0111,
   KEY_BUTTON18      = 0x0112,
   KEY_BUTTON19      = 0x0113,
   KEY_BUTTON20      = 0x0114,
   KEY_BUTTON21      = 0x0115,
   KEY_BUTTON22      = 0x0116,
   KEY_BUTTON23      = 0x0117,
   KEY_BUTTON24      = 0x0118,
   KEY_BUTTON25      = 0x0119,
   KEY_BUTTON26      = 0x011A,
   KEY_BUTTON27      = 0x011B,
   KEY_BUTTON28      = 0x011C,
   KEY_BUTTON29      = 0x011D,
   KEY_BUTTON30      = 0x011E,
   KEY_BUTTON31      = 0x011F,
   KEY_ANYKEY         = 0xfffe
};

/// Joystick event codes.
enum JoystickCodes {
   SI_XPOV           = 0x204,
   SI_YPOV           = 0x205,
   SI_UPOV           = 0x206,
   SI_DPOV           = 0x207,
   SI_LPOV           = 0x208,
   SI_RPOV           = 0x209,
   SI_XAXIS          = 0x20B,
   SI_YAXIS          = 0x20C,
   SI_ZAXIS          = 0x20D,
   SI_RXAXIS         = 0x20E,
   SI_RYAXIS         = 0x20F,
   SI_RZAXIS         = 0x210,
   SI_SLIDER         = 0x211,
   SI_XPOV2          = 0x212,
   SI_YPOV2          = 0x213,
   SI_UPOV2          = 0x214,
   SI_DPOV2          = 0x215,
   SI_LPOV2          = 0x216,
   SI_RPOV2          = 0x217,

//Xinput specific

   XI_CONNECT        = 0x300,
   XI_THUMBLX        = 0x301,
   XI_THUMBLY        = 0x302,
   XI_THUMBRX        = 0x303,
   XI_THUMBRY        = 0x304,
   XI_LEFT_TRIGGER   = 0x305,
   XI_RIGHT_TRIGGER  = 0x306,

   XI_DPAD_UP        = 0x206,
   XI_DPAD_DOWN      = 0x207,
   XI_DPAD_LEFT      = 0x208,
   XI_DPAD_RIGHT     = 0x209,
   
   XI_START          = 0x311,
   XI_BACK           = 0x312,
   XI_LEFT_THUMB     = 0x313,
   XI_RIGHT_THUMB    = 0x314,
   XI_LEFT_SHOULDER  = 0x315,
   XI_RIGHT_SHOULDER = 0x316,

   XI_A              = 0x317,
   XI_B              = 0x318,
   XI_X              = 0x319,
   XI_Y              = 0x320
};

enum AccelerometerCodes
{
   SI_ACCELX        = 0x300,
   SI_ACCELY        = 0x301,
   SI_ACCELZ        = 0x302,
   SI_GRAVX         = 0x303,
   SI_GRAVY         = 0x304,
   SI_GRAVZ         = 0x305
};

enum GyroCodes 
{
   SI_GYROX       = 0x306,
   SI_GYROY       = 0x307,
   SI_GYROZ       = 0x308,
   SI_YAW         = 0x309,
   SI_PITCH       = 0x30A,
   SI_ROLL        = 0x30B
};

enum TouchCodes
{
   SI_TOUCHDOWN   = 0x30C,
   SI_TOUCHUP     = 0x30D,
   SI_TOUCHMOVE   = 0x30E,
};

enum GestureCodes
{
    SI_CIRCLE_GESTURE    = 0x403,
    SI_SWIPE_GESTURE     = 0x404,
    SI_KEYTAP_GESTURE    = 0x405,
    SI_SCREENTAP_GESTURE = 0x406,
    SI_PINCH_GESTURE     = 0x407,
    SI_SCALE_GESTURE     = 0x408
};

enum LeapMotionCodes
{
    LM_HANDAXIS          = 0x409,
    LM_HANDROT           = 0x40A,
    LM_HANDPOS           = 0x40B,
    LM_FINGERPOS         = 0x40C,
};

/// Input device types
enum InputDeviceTypes
{
   UnknownDeviceType,
   MouseDeviceType,
   KeyboardDeviceType,
   JoystickDeviceType,
   GamepadDeviceType,
   XInputDeviceType,
   ScreenTouchDeviceType,
   AccelerometerDeviceType,
   GyroscopeDeviceType,
   LeapMotionDeviceType
};

/// Device Event Action Types
#define SI_MAKE      0x01
#define SI_BREAK     0x02
#define SI_MOVE      0x03
#define SI_REPEAT    0x04
#define SI_VALUE	 0x05

///Device Event Types
#define SI_UNKNOWN           0x01
#define SI_BUTTON            0x02
#define SI_POV               0x03
#define SI_KEY               0x0A
#define SI_TEXT              0x0B
#define SI_TOUCH             0x0C
#define SI_GESTURE           0x0D
#define SI_MOTION            0x0F
#define SI_LEAP              0x11

/// Event SubTypes
#define SI_ANY       0xff

// Modifier Keys
/// shift and ctrl are the same between platforms.
#define SI_LSHIFT    (1<<0)
#define SI_RSHIFT    (1<<1)
#define SI_SHIFT     (SI_LSHIFT|SI_RSHIFT)
#define SI_LCTRL     (1<<2)
#define SI_RCTRL     (1<<3)
#define SI_CTRL      (SI_LCTRL|SI_RCTRL)
/// win altkey, mapped to mac cmdkey.
#define SI_LALT      (1<<4)
#define SI_RALT      (1<<5)
#define SI_ALT       (SI_LALT|SI_RALT)
/// mac optionkey
#define SI_MAC_LOPT  (1<<6)
#define SI_MAC_ROPT  (1<<7)
#define SI_MAC_OPT   (SI_MAC_LOPT|SI_MAC_ROPT)

/// @}

//Xinput structs

typedef U32 InputObjectInstances;

enum XInputEventType
{
   XI_UNKNOWN = 0x01,
   XI_BUTTON  = 0x02,   // Button press/release
   XI_POV     = 0x03,   // Point of View hat
   XI_AXIS    = 0x04,   // Axis in range -1.0..1.0
   XI_POS     = 0x05,   // Absolute position value (Point3F)
   XI_ROT     = 0x06,   // Absolute rotation value (QuatF)
   XI_INT     = 0x07,   // Integer value (S32)
   XI_FLOAT   = 0x08,   // Float value (F32)
   XI_KEY     = 0x0A,   // Keyboard key
};

enum InputActionType
{
   /// Button was depressed.
   XI_MAKE    = 0x01,

   /// Button was released.
   XI_BREAK   = 0x02,

   /// An axis moved.
   XI_MOVE    = 0x03,

   /// A key repeat occurred. Happens in between a SI_MAKE and SI_BREAK.
   XI_REPEAT  = 0x04,

   /// A value of some type.  Matched with SI_FLOAT or SI_INT.
   XI_VALUE   = 0x05,
};

#endif
