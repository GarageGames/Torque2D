//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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



#include "platformEmscripten/platformEmscripten.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/console.h"
#include "platformEmscripten/EmscriptenInputManager.h"

#include <SDL/SDL.h>

#ifdef LOG_INPUT
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#endif

// Static class variables:
InputManager*  Input::smManager = NULL;

// smActive is not maintained under unix.  Use Input::isActive()
// instead
bool           Input::smActive = false;
CursorManager* Input::smCursorManager = 0;

//extern AsciiData AsciiTable[NUM_KEYS];

#ifdef LOG_INPUT
S32 gInputLog = -1;
#endif 

//------------------------------------------------------------------------------
void Input::init()
{
   Con::printf( "Input Init:" );

   destroy();

   smActive = false;
   smManager = NULL;

   UInputManager *uInputManager = new UInputManager;
   if ( !uInputManager->enable() )
   {
      Con::errorf( "   Failed to enable Input Manager." );
      delete uInputManager;
      return;
   }

   uInputManager->init();

   smManager = uInputManager;

   Con::printf("   Input initialized");
   Con::printf(" ");
}

//------------------------------------------------------------------------------
ConsoleFunction( isJoystickDetected, bool, 1, 1, "isJoystickDetected()" )
{
   argc; argv;
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (manager)
      return manager->joystickDetected();
   else
      return false;
}

//------------------------------------------------------------------------------
ConsoleFunction( getJoystickAxes, const char*, 2, 2, "getJoystickAxes( instance )" )
{
   argc; argv;
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (manager)
      return manager->getJoystickAxesString(dAtoi(argv[1]));
   else
      return "";
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

//-----------------------------------------------------------------------------
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
   if ( smManager && !smManager->isEnabled() )
      return( smManager->enable() );
   
   return( false );
}

//------------------------------------------------------------------------------
void Input::disable()
{
   if ( smManager && smManager->isEnabled() )
      smManager->disable();
}

//------------------------------------------------------------------------------
void Input::activate()
{
   if ( smManager && smManager->isEnabled() && !isActive())
   {
#ifdef LOG_INPUT
      Input::log( "Activating Input...\n" );
#endif
      UInputManager* uInputManager = dynamic_cast<UInputManager*>( smManager );
      if ( uInputManager )
         uInputManager->activate();
   }
}

//------------------------------------------------------------------------------
void Input::deactivate()
{
   if ( smManager && smManager->isEnabled() && isActive() )
   {
#ifdef LOG_INPUT
      Input::log( "Deactivating Input...\n" );
#endif
      UInputManager* uInputManager = dynamic_cast<UInputManager*>( smManager );
      if ( uInputManager )
         uInputManager->deactivate();
   }
}

//------------------------------------------------------------------------------
void Input::reactivate()
{
   Input::deactivate();
   Input::activate();
}

//------------------------------------------------------------------------------
bool Input::isEnabled()
{
   if ( smManager )
      return smManager->isEnabled();
   return false;
}

//------------------------------------------------------------------------------
bool Input::isActive()
{
   UInputManager* uInputManager = dynamic_cast<UInputManager*>( smManager );
   if ( uInputManager )
      return uInputManager->isActive();
   return false;
}

//------------------------------------------------------------------------------
void Input::process()
{
   if ( smManager )
      smManager->process();
}

//------------------------------------------------------------------------------
InputManager* Input::getManager()
{
   return smManager;
}

#ifdef LOG_INPUT
//------------------------------------------------------------------------------
void Input::log( const char* format, ... )
{
}

ConsoleFunction( inputLog, void, 2, 2, "inputLog( string )" )
{
   argc;
   Input::log( "%s\n", argv[1] );
}
#endif // LOG_INPUT

//------------------------------------------------------------------------------
const char* Platform::getClipboard()
{
   return "";
}

//------------------------------------------------------------------------------
bool Platform::setClipboard(const char *text)
{
   return false;
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
   return ( 50 * 60.0f * 1000.0f);
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
void Input::setCursorPos(S32 x, S32 y)
{
   SDL_WarpMouse((S16)x, (S16)y);
}

