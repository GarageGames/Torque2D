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


#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/AndroidUtil.h"
#include "platform/platformInput.h"
#include "platform/event.h"
#include "console/console.h"
#include "game/gameInterface.h"
#include "string/unicode.h"
#include "gui/guiCanvas.h"


// <Mat> just some random number 50, we'll get a proper value later
#define Android_DOUBLE_CLICK_TIME	( 50 * 60.0f * 1000.0f)

// Static class variables:
InputManager* Input::smManager;
bool           Input::smActive;
CursorManager* Input::smCursorManager = 0;


bool gInputEnabled = false;
bool gMouseEnabled = false;
bool gKBEnabled = false;
bool gMouseActive = false;
bool gKBActive = false;

//------------------------------------------------------------------------------
// Helper functions.  Should migrate to an InputManager object at some point.
bool enableKeyboard(void);
void disableKeyboard(void);
bool activateKeyboard(void);
void deactivateKeyboard(void);
bool enableMouse(void);
void disableMouse(void);
bool activateMouse(void);
void deactivateMouse(void);



static void fillAsciiTable();


struct touchEvent {
	S32 number;
	S32 x;
	S32 y;
	touchEvent( S32 a, S32 b, S32 c ) {
		number = a;
		x = b;
		y = c;
	}
};


Vector<touchEvent> TouchMoveEvents;//<Mat> to make sure we don't have multiple events per frame
Vector<touchEvent> TouchDownEvents;
Vector<touchEvent> TouchUpEvents;

// EFM - BEGIN TOUCH CHANGES
#define MAX_TOUCH_EVENTS 5

struct touchTracker {
	S32 lastX;
	S32 lastY;
	Vector<touchEvent> downEvents;
	Vector<touchEvent> moveEvents;
	Vector<touchEvent> upEvents;
};

touchTracker CombinedTouchEvents[MAX_TOUCH_EVENTS];

struct touchCorrelation {
	S32 lastX;
	S32 lastY;
};

touchCorrelation lastTouches[MAX_TOUCH_EVENTS];

// EFM - END TOUCH CHANGES

//Luma: Tap support
Vector<touchEvent> TouchTapEvents;
int processMultipleTouches();


//------------------------------------------------------------------------------
//
// This function gets the standard ASCII code corresponding to our key code
// and the existing modifier key state.
//
//------------------------------------------------------------------------------
struct AsciiData
{
   struct KeyData
   {
      U16   ascii;
      bool  isDeadChar;
   };

   KeyData upper;
   KeyData lower;
   KeyData goofy;
};


#define NUM_KEYS ( KEY_OEM_102 + 1 )
#define KEY_FIRST KEY_ESCAPE
static AsciiData AsciiTable[NUM_KEYS];

void Input::enableMouse()
{
    // Do nothing on Android
}

void Input::disableMouse()
{
    // Do nothing on Android
}

void Input::enableKeyboard()
{
    // Do nothing on Android
}

void Input::disableKeyboard()
{
    // Do nothing on Android
}

bool Input::isMouseEnabled()
{
    return true;
}

bool Input::isKeyboardEnabled()
{
    return false;
}
 
//--------------------------------------------------------------------------
void Input::init()
{
    Con::printf( "Input Init:" );
   destroy();

   smManager = NULL;
	//smManager = new AndroidInputManager();
   smActive = false;

   // stop the double-cursor thing
   Con::setBoolVariable("$pref::Gui::noClampTorqueCursorToWindow", true);
   
   // enable main input
   Input::enable();

   // Startup the Cursor Manager
   if(!smCursorManager)
   {
      smCursorManager = new CursorManager();
      if(smCursorManager)
      {
         // Add the arrow cursor to the stack
         smCursorManager->pushCursor(CursorManager::curArrow);
      }
      else
      {
         Con::printf("   Cursor Manager not enabled.");
      }
   }
	
	
	for(int i = 0 ; i < MAX_TOUCH_EVENTS; i++ )
	{
		lastTouches[i].lastX = -1;
		lastTouches[i].lastY = -1;
	}
	
	
   
   
   Con::printf( "" );
}

//------------------------------------------------------------------------------
ConsoleFunction( isJoystickDetected, bool, 1, 1, "Always false on the Android." )
{
/*
   argc; argv;
   return( DInputDevice::joystickDetected() );
*/
   return(false);
}

//------------------------------------------------------------------------------
ConsoleFunction( getJoystickAxes, const char*, 2, 2, "(handle instance)" )
{

   return( "" );
}

//------------------------------------------------------------------------------
static void fillAsciiTable()
{

}

//------------------------------------------------------------------------------
U16 Input::getKeyCode( U16 asciiCode )
{
   U16 keyCode = 0;
   U16 i;
   
   // This is done three times so the lowerkey will always
   // be found first. Some foreign keyboards have duplicate
   // chars on some keys.
   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].lower.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].upper.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].goofy.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   return( keyCode );
}

//------------------------------------------------------------------------------
U16 Input::getAscii( U16 keyCode, KEY_STATE keyState )
{
   if ( keyCode >= NUM_KEYS )
      return 0;

   switch ( keyState )
   {
      case STATE_LOWER:
         return AsciiTable[keyCode].lower.ascii;
      case STATE_UPPER:
         return AsciiTable[keyCode].upper.ascii;
      case STATE_GOOFY:
         return AsciiTable[keyCode].goofy.ascii;
      default:
         return(0);
            
   }
}

//------------------------------------------------------------------------------
void Input::destroy()
{
   // turn us off.
   if (gInputEnabled)
      disable();
   
   if ( smManager && smManager->isEnabled() )
   {
      smManager->disable();
      delete smManager;
      smManager = NULL;
   }
}

//------------------------------------------------------------------------------
bool Input::enable()
{
	Con::printf( "[]Input::enable." );

   gInputEnabled = true;

   if ( smManager && !smManager->isEnabled() )
      return( smManager->enable() );

   enableMouse();
   //enableKeyboard();

   return( gInputEnabled );
}

//------------------------------------------------------------------------------
void Input::disable()
{
	Con::printf( "[]Input::disable." );

   gInputEnabled = false;

  if ( smManager && smManager->isEnabled() )
      smManager->disable();

   disableMouse();
   //disableKeyboard();
}

//------------------------------------------------------------------------------
void Input::activate()
{
   smActive = true;

   enableMouse();
// enableKeyboard();
}

//------------------------------------------------------------------------------
void Input::deactivate()
{
//Con::printf( "[]Input::deactivate." );

   deactivateMouse();
   //deactivateKeyboard();

   smActive = false;

}

//------------------------------------------------------------------------------
void Input::reactivate()
{
   // don't think mac needs to do anything right now!!!!!! TBD
   
   // This is soo hacky...
//   SetForegroundWindow( winState.appWindow );
//   PostMessage( winState.appWindow, WM_ACTIVATE, WA_ACTIVE, NULL );
}

//------------------------------------------------------------------------------
bool Input::isEnabled()
{
   if ( smManager )
      return smManager->isEnabled();

   return(gInputEnabled);
}

//------------------------------------------------------------------------------
bool Input::isActive()
{
   return smActive;
}

//------------------------------------------------------------------------------
void Input::process()
{
	//only gets called once per frame, create touches and accelerometer events here
	//post, then pop each event

	if(platState.multipleTouchesEnabled) processMultipleTouches();
	
   if (!smActive || !gInputEnabled)
      return;

   if (!gMouseEnabled || !gMouseActive)
      return;
      
 
   if ( smManager && smManager->isEnabled() && smActive )
      smManager->process();
}

//------------------------------------------------------------------------------
InputManager* Input::getManager()
{
   return( smManager );
}

#pragma mark ---- Clipboard functions ----
//-----------------------------------------------------------------------------
const char* Platform::getClipboard()
{
	return NULL;//no clipboard on Android
}

//-----------------------------------------------------------------------------
bool Platform::setClipboard(const char *text)
{
	return NULL;//no clipboard on Android
}

#pragma mark ---- Cursor Functions ----
//------------------------------------------------------------------------------
void Input::pushCursor(S32 cursorID)
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->pushCursor(cursorID);
}

//------------------------------------------------------------------------------
void Input::popCursor()
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->popCursor();
}

//------------------------------------------------------------------------------
void Input::refreshCursor()
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->refreshCursor();
}

#pragma mark ---- DoubleClick Functions ----
//------------------------------------------------------------------------------
U32 Input::getDoubleClickTime()
{
	return Android_DOUBLE_CLICK_TIME;
}

//------------------------------------------------------------------------------
S32 Input::getDoubleClickWidth()
{
   // this is an arbitrary value.
   return 10;
}

//------------------------------------------------------------------------------
S32 Input::getDoubleClickHeight()
{
   return getDoubleClickWidth();
}

#pragma mark -

//------------------------------------------------------------------------------
bool enableKeyboard()
{
   if ( !gInputEnabled )
      return( false );

   if ( gKBEnabled && gKBActive )
      return( true );

   gKBEnabled = true;
   if ( Input::isActive() )
      gKBEnabled = activateKeyboard();

   if ( gKBEnabled )
   {
      Con::printf( "Hardware-direct keyboard enabled." );
   }
   else
   {
      Con::warnf( "Hardware-direct keyboard failed to enable!" );
   }

   return( gKBEnabled );
}

//------------------------------------------------------------------------------
void disableKeyboard()
{
   if ( !gInputEnabled || !gKBEnabled )
      return;

   deactivateKeyboard();
   gKBEnabled = false;

   Con::printf( "Hardware-direct keyboard disabled." );
}

//------------------------------------------------------------------------------
bool activateKeyboard()
{
   if ( !gInputEnabled || !Input::isActive() || !gKBEnabled )
      return( false );

   gKBActive = true;

   return( gKBActive );
}

//------------------------------------------------------------------------------
void deactivateKeyboard()
{
   if ( gInputEnabled && gKBActive )
   {
      gKBActive = false;
   }
}

//------------------------------------------------------------------------------
bool enableMouse()
{
   if ( !gInputEnabled )
      return( false );

   if ( gMouseEnabled && gMouseActive )
      return( true );

   gMouseEnabled = activateMouse();

   return( gMouseEnabled );
}

//------------------------------------------------------------------------------
void disableMouse()
{
   if ( !gInputEnabled || !gMouseEnabled )
      return;

   deactivateMouse();
   gMouseEnabled = false;

   bool hwMouse = false;
   Con::printf( "%s disabled", hwMouse?"Hardware-direct mouse":"Basic mouse capture");
}

//------------------------------------------------------------------------------
bool activateMouse()
{
   if ( !gInputEnabled || !Input::isActive() || !gMouseEnabled )
      return( false );
   
   if (gMouseActive)
      return(true);

   gMouseActive = true;

   return( gMouseActive );
}

//------------------------------------------------------------------------------
void deactivateMouse()
{
   if ( !gInputEnabled || !gMouseActive )
       return;
   
   gMouseActive = false;
}


//------------------------------------------------------------------------------
ConsoleFunction( enableMouse, bool, 1, 1, "enableMouse()" )
{
   return( enableMouse() );
}

//------------------------------------------------------------------------------
ConsoleFunction( disableMouse, void, 1, 1, "disableMouse()" )
{
   disableMouse();
}

//------------------------------------------------------------------------------
void printInputState(void)
{
   if ( gInputEnabled )
   {
         Con::printf( "Low-level input system is enabled." );
      
      Con::printf( "- Keyboard is %sabled and %sactive.", 
            gKBEnabled ? "en" : "dis",
            gKBActive ? "" : "in" );
      Con::printf( "- Mouse is %sabled and %sactive.", 
            gMouseEnabled ? "en" : "dis",
            gMouseActive ? "" : "in" );
/*
      Con::printf( "- Joystick is %sabled and %sactive.", 
            gJoystickEnabled() ? "en" : "dis",
            gJoystickActive() ? "" : "in" );
*/
   }
   else
   {
      Con::printf( "Low-level input system is disabled." );
   }
}

//------------------------------------------------------------------------------
ConsoleFunction( echoInputState, void, 1, 1, "echoInputState()" )
{
   printInputState();
}

//------------------------------------------------------------------------------
ConsoleFunction( toggleInputState, void, 1, 1, "toggleInputState()" )
{
   if (gInputEnabled)
      Input::disable();
   else
      Input::enable();

   printInputState();
}

//------------------------------------------------------------------------------
ConsoleFunction( deactivateKeyboard, void, 1, 1, "deactivateKeyboard();")
{
   // these are only useful on the windows side. They deal with some vagaries of win32 DirectInput.
}

ConsoleFunction( activateKeyboard, void, 1, 1, "activateKeyboard();")
{
   // these are only useful on the windows side. They deal with some vagaries of win32 DirectInput.
}


//------------------------------------------------------------------------------
void Input::setCursorPos(S32 x, S32 y)
{
	//this gets called from GuiCanvas to set the game mouse cursor
}

int processMultipleTouches()
{	
	char posX[256], posY[256], temp[10], touchNums[256];
    
	dMemset(posX, 0, sizeof(posX));
    dMemset(posY, 0, sizeof(posY));
    dMemset(touchNums, 0, sizeof(touchNums));

	touchEvent *currentEvent;
	
	//Down Events
	int numTouchDownEvents = TouchDownEvents.size();
	while( TouchDownEvents.size() > 0 )
    {
		currentEvent = &TouchDownEvents.last();
		dItoa( currentEvent->x, temp );
		dStrcat( posX, temp );
		dStrcat( posX, " " );
		
		dItoa( currentEvent->y, temp );
		dStrcat( posY, temp );
		dStrcat( posY, " " );
		
		dItoa( currentEvent->number, temp );
		dStrcat( touchNums, temp );
		dStrcat( touchNums, " " );
		
		TouchDownEvents.pop_back();
	}
	dItoa( numTouchDownEvents, temp );
    
    if( numTouchDownEvents > 0 )
    {
        InputEvent touchEvent;
        
        touchEvent.deviceInst = 0;
        touchEvent.objInst = SI_TOUCHDOWN;
        
        touchEvent.deviceType = ScreenTouchDeviceType;
        touchEvent.action = SI_TOUCH;
        touchEvent.objType = SI_TOUCHDOWN;
        
        dStrcpy(touchEvent.fingersX, posX);
        dStrcpy(touchEvent.fingersY, posY);
        dStrcpy(touchEvent.fingerIDs, touchNums);
        
        touchEvent.modifier = 0;
        
        Game->postEvent(touchEvent);        
    }
    
    // Deprecated in 1.5
	//if( numTouchDownEvents > 0 )
    //    Con::executef( 4, "onAndroidTouchDown", touchNums , posX, posY );

	//Move events
	int numTouchMoveEvents = TouchMoveEvents.size();
	while( TouchMoveEvents.size() > 0 ) 
    {
		currentEvent = &TouchMoveEvents.last();
		dItoa( currentEvent->x, temp );
		dStrcat( posX, temp );
		dStrcat( posX, " " );
		
		dItoa( currentEvent->y, temp );
		dStrcat( posY, temp );
		dStrcat( posY, " " );
		
		dItoa( currentEvent->number, temp );
		dStrcat( touchNums, temp );
		dStrcat( touchNums, " " );		
		
		TouchMoveEvents.pop_back();
	}
	dItoa( numTouchMoveEvents, temp );
    
    if( numTouchMoveEvents > 0 )
    {
        InputEvent touchEvent;
        
        touchEvent.deviceInst = 0;
        touchEvent.objInst = SI_TOUCHMOVE;
        
        touchEvent.deviceType = ScreenTouchDeviceType;
        touchEvent.action = SI_TOUCH;
        touchEvent.objType = SI_TOUCHMOVE;
        
        dStrcpy(touchEvent.fingersX, posX);
        dStrcpy(touchEvent.fingersY, posY);
        dStrcpy(touchEvent.fingerIDs, touchNums);
        
        touchEvent.modifier = 0;
        
        Game->postEvent(touchEvent);        
    }
    
    // Deprecated in 1.5 -MP
	//if( numTouchMoveEvents > 0 )
    //    Con::executef( 4, "onAndroidTouchMove", touchNums , posX, posY );
	
	//Up events
	int numTouchUpEvents = TouchUpEvents.size();
    
    dMemset(posX, 0, sizeof(posX));
    dMemset(posY, 0, sizeof(posY));
    dMemset(touchNums, 0, sizeof(touchNums));
    
	while( TouchUpEvents.size() > 0 ) 
    {
		currentEvent = &TouchUpEvents.last();
		dItoa( currentEvent->x, temp );
		dStrcat( posX, temp );
		dStrcat( posX, " " );
		
		dItoa( currentEvent->y, temp );
		dStrcat( posY, temp );
		dStrcat( posY, " " );
		
		dItoa( currentEvent->number, temp );
		dStrcat( touchNums, temp );
		dStrcat( touchNums, " " );		
		
		lastTouches[currentEvent->number].lastX = -1;
		lastTouches[currentEvent->number].lastY	 = -1;
		
        int x;
        x = -1;
        
        lastTouches[currentEvent->number].lastX = -1;
		lastTouches[currentEvent->number].lastY	 = -1;
        
		TouchUpEvents.pop_back();
	}
    
	dItoa( numTouchUpEvents, temp );
	
    if( numTouchUpEvents > 0 )
    {
        InputEvent touchEvent;
        
        touchEvent.deviceInst = 0;
        touchEvent.objInst = SI_TOUCHUP;
        
        touchEvent.deviceType = ScreenTouchDeviceType;
        touchEvent.action = SI_TOUCH;
        touchEvent.objType = SI_TOUCHUP;
        
        dStrcpy(touchEvent.fingersX, posX);
        dStrcpy(touchEvent.fingersY, posY);
        dStrcpy(touchEvent.fingerIDs, touchNums);
        
        touchEvent.modifier = 0;
        
        Game->postEvent(touchEvent);        
    }
    
    // Deprecated in 1.5 -MP
    //if( numTouchUpEvents > 0 )
    //    Con::executef( 4, "onAndroidTouchUp", touchNums , posX, posY );
	
	//Luma: Tap support
	posX[0] = '\0';
	posY[0] = '\0';
    
	int numTapEvents = TouchTapEvents.size();
	
    while( TouchTapEvents.size() > 0 ) 
    {
		currentEvent = &TouchTapEvents.last();
		sprintf( temp, "%d ", currentEvent->x );
		dStrcat( posX, temp );
		
		sprintf( temp, "%d ", currentEvent->y );
		dStrcat( posY, temp );
		
		TouchTapEvents.pop_back();
	}
    
	sprintf( temp, "%d", numTapEvents );
    
	//if( numTapEvents > 0 )
    //    Con::executef( 4, "onAndroidTouchTap", temp , posX, posY );
	
	return numTouchDownEvents + numTouchMoveEvents + numTouchUpEvents;
}



//we want these to only be called once per frame!!
bool createMouseMoveEvent( S32 touchNumber, S32 x, S32 y, S32 lastX, S32 lastY ) //EFM
{	
	S32 currentSlot = -1;
	
	if (Canvas == NULL)
		return false;

	for( int i = 0 ; (i < MAX_TOUCH_EVENTS) && (currentSlot == -1) ; i++ )
	{
		if( (lastX == lastTouches[i].lastX ) &&
		   (lastY == lastTouches[i].lastY ) )
		{
			currentSlot = i;
		}
	}

	if( currentSlot == -1 ) return false;
	
	ScreenTouchEvent event;
	event.xPos = x;
	event.yPos = y;
	event.action = SI_MOVE;
	event.touchID = currentSlot;
    event.numTouches = 0;
    
	//Luma: Mouse not moving (no hover for mouse fingers!)
	Canvas->setCursorPos( Point2I( x, y ) );  

	if( currentSlot != -1 )
	{
		lastTouches[currentSlot].lastX = x;
		lastTouches[currentSlot].lastY = y;
	}		
	
	TouchMoveEvents.push_back( touchEvent( currentSlot, x, y ) );	
	Game->postEvent(event);
	
	return true;//return false if we get bad values or something
}


bool createMouseDownEvent( S32 touchNumber, S32 x, S32 y, U32 numTouches ) 
{
	S32 vacantSlot = -1;
	
	if (Canvas == NULL)
		return false;

	for( int i = 0 ; (i < MAX_TOUCH_EVENTS) && (vacantSlot == -1) ; i++ )
	{
		if( lastTouches[i].lastX == -1 )
		{
			vacantSlot = i;
		}
	}
	
	if( vacantSlot == -1 ) 
        return false;
		
	ScreenTouchEvent event;
	event.xPos = x;
	event.yPos = y;
    event.touchID = vacantSlot;
	event.action = SI_MAKE;
	event.numTouches = numTouches;
    
	//Luma: Update position
	Canvas->setCursorPos( Point2I( x, y ) );  		
	
	if( vacantSlot != -1 )
	{
		lastTouches[vacantSlot].lastX = x;
		lastTouches[vacantSlot].lastY = y;
	}	

	TouchDownEvents.push_back( touchEvent( vacantSlot, x, y ) );
	Game->postEvent(event);
	
	return true;//return false if we get bad values or something
}

bool createMouseUpEvent( S32 touchNumber, S32 x, S32 y, S32 lastX, S32 lastY, U32 numTouches ) //EFM
{	
	S32 currentSlot = -1;
	
	for( int i = 0 ; (i < MAX_TOUCH_EVENTS) && (currentSlot == -1) ; i++ )
	{
		if(( (x == lastTouches[i].lastX) && (y == lastTouches[i].lastY )) ||
		   ( (lastX == lastTouches[i].lastX ) && (lastY == lastTouches[i].lastY )))
		{
			currentSlot = i;
		}	
	}
	
	if( currentSlot == -1 ) 
        return false;

	ScreenTouchEvent event;
	event.xPos = x;
	event.yPos = y;
	event.action = SI_BREAK;
	event.touchID = currentSlot;
	event.numTouches = numTouches;
    
	TouchUpEvents.push_back( touchEvent( currentSlot, x, y ) );	
	
	Game->postEvent(event);
	
	return true;//return false if we get bad values or something
}

void clearPendingMultitouchEvents( void ) //EFM
{	
    for( int i = 0 ; i < MAX_TOUCH_EVENTS ; i++ )
	{
		lastTouches[i].lastX = -1;
		lastTouches[i].lastY = -1;
	}

	TouchMoveEvents.clear();
	TouchDownEvents.clear();
	TouchUpEvents.clear();
}


//Luma: Tap support
void createMouseTapEvent( S32 nbrTaps, S32 x, S32 y ) {

	TouchTapEvents.push_back( touchEvent( nbrTaps, x, y ) );
}

