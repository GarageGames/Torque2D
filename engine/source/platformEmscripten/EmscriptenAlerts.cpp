//-----------------------------------------------------------------------------
// Copyright (c) James S Urquhart 2014
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

extern "C"
{
	extern int js_AlertOK(const char *title, const char *message);
	extern int js_AlertOKCancel(const char *title, const char *message);
	extern int js_AlertRetry(const char *title, const char *message);
	extern int js_AlertYesNo(const char *title, const char *message);
}

//-----------------------------------------------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
	js_AlertOK(windowTitle, message);
}
//-----------------------------------------------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
	return js_AlertOKCancel(windowTitle, message) == 1;
}

//-----------------------------------------------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{//retry/cancel
	return js_AlertRetry(windowTitle, message) == 1;
}

//-----------------------------------------------------------------------------
bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{
	return js_AlertYesNo(windowTitle, message) == 1;
}
