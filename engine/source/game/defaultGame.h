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

#ifndef _TORQUEGAME_H_
#define _TORQUEGAME_H_

#ifndef _GAMEINTERFACE_H_
#include "game/gameInterface.h"
#endif

#ifndef _SIM_EVENT_H_
#include "sim/simEvent.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

//-----------------------------------------------------------------------------

class DefaultGame : public GameInterface, public virtual Tickable
{
public:
    virtual bool mainInitialize( int argc, const char **argv );
    virtual void mainLoop( void );
    virtual void mainShutdown( void );

    virtual void gameDeactivate( const bool noRender );
    virtual void gameReactivate( void );

    virtual void textureKill( void );
    virtual void textureResurrect( void );
    virtual void refreshWindow( void );

    virtual void processTick( void );
    virtual void interpolateTick( F32 delta ) {};
    virtual void advanceTime( F32 timeDelta );

    void processQuitEvent();
    void processTimeEvent(TimeEvent *event);
    void processInputEvent(InputEvent *event);
    void processMouseMoveEvent(MouseMoveEvent *event);
    void processScreenTouchEvent(ScreenTouchEvent *event);
    void processConsoleEvent(ConsoleEvent *event);
    void processPacketReceiveEvent(PacketReceiveEvent *event);
    void processConnectedAcceptEvent(ConnectedAcceptEvent *event);
    void processConnectedReceiveEvent(ConnectedReceiveEvent *event);
    void processConnectedNotifyEvent(ConnectedNotifyEvent *event);
};

#endif
