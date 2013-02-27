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

#include "2d/core/Vector2.h"

// Script bindings.
#include "Vector2_ScriptBinding.h"

//-----------------------------------------------------------------------------

ConsoleType( Vector2, TypeVector2, sizeof(Vector2), "" )

ConsoleGetType( TypeVector2 )
{
    return ((Vector2*)dptr)->scriptThis();
}

ConsoleSetType( TypeVector2 )
{
    // Fetch vector.
    Vector2* pVector = (Vector2*)dptr;

    // "x y".
    if( argc == 1 )
    {
        pVector->setString( argv[0] );
        return;
    }

    // "x,y".
    if( argc == 2 )
    {
        pVector->Set(dAtof(argv[0]), dAtof(argv[1]));
        return;
    }

    // Warn.
    Con::printf("Vector2 must be set as { x, y } or \"x y\"");
}
