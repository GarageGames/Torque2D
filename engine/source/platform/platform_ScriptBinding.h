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

ConsoleFunction( setMouseLock, void, 2, 2, "( isLocked ) Use the setMouseLock function to un/lock the mouse.\n"
                                                                "@param isLocked A boolean value.\n"
                                                                "@return No return value")
{
    Platform::setMouseLock(dAtob(argv[1]));
}

//-----------------------------------------------------------------------------

ConsoleFunction( getRealTime, S32, 1, 1, "() Use the getRealTime function to the computer time in milliseconds.\n"
                                                                "@return Returns the current real time in milliseconds.\n"
                                                                "@sa getSimTime")
{
    return Platform::getRealMilliseconds();
}

//-----------------------------------------------------------------------------

ConsoleFunction( getLocalTime, const char*, 1, 1,   "() Get the local time\n"
                                                    "@return the local time formatted as: monthday/month/year hour/minute/second\n")
{
    char* buf = Con::getReturnBuffer(128);

    Platform::LocalTime lt;
    Platform::getLocalTime(lt);

    dSprintf(buf, 128, "%d/%d/%d %02d:%02d:%02d",
        lt.monthday,
        lt.month + 1,
        lt.year + 1900,
        lt.hour,
        lt.min,
        lt.sec);

    return buf;
}