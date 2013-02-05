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
#import "platformOSX/osxInputManager.h"

#pragma mark ---- InputManager Handling ----

//--------------------------------------------------------------------------
osxInputManager::osxInputManager()
{
    mKeyboardEnabled = false;
    mMouseEnabled = false;
    mEnabled = false;
}

//--------------------------------------------------------------------------
bool osxInputManager::enable()
{
    mEnabled = true;
    return mEnabled;
}

//--------------------------------------------------------------------------
void osxInputManager::disable()
{
    mEnabled = false;
}

//--------------------------------------------------------------------------
void osxInputManager::process()
{
    // Nothing to process here, yet
}

#pragma mark ---- Keyboard Handling Functions ----

//--------------------------------------------------------------------------
void osxInputManager::enableKeyboard()
{
    if (!mEnabled)
        mEnabled = true;

    mKeyboardEnabled = true;
}

//--------------------------------------------------------------------------
void osxInputManager::disableKeyboard()
{
    mKeyboardEnabled = false;
}

//--------------------------------------------------------------------------
bool osxInputManager::isKeyboardEnabled()
{
    if (mEnabled)
        return mKeyboardEnabled;

    return false;
}

#pragma mark ---- Mouse Handling Functions ----

//--------------------------------------------------------------------------
void osxInputManager::enableMouse()
{
    if (!mEnabled)
        mEnabled = true;

    mMouseEnabled = true;
}

//--------------------------------------------------------------------------
void osxInputManager::disableMouse()
{
    mMouseEnabled = false;
}

//--------------------------------------------------------------------------
bool osxInputManager::isMouseEnabled()
{
    if (mEnabled)
        return mMouseEnabled;

    return false;
}

