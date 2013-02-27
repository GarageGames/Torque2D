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

#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMGL_H_
#include "platform/platformGL.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

//-----------------------------------------------------------------------------
// Miscellaneous Defines.
//-----------------------------------------------------------------------------

#define MASK_ALL                        (U32_MAX)
#define MASK_BITCOUNT                   (32)
#define DEBUG_MODE_COUNT                (8)
#define MAX_LAYERS_SUPPORTED            (32)
#define CANNOT_RENDER_PROXY_NAME        "CannotRenderProxy"
#define b2_pi2                          (b2_pi * 2.0f)

//-----------------------------------------------------------------------------

class SceneObject;
struct Vector2;

typedef Vector<SceneObject*> typeSceneObjectVector;
typedef const Vector<SceneObject*>& typeSceneObjectVectorConstRef;

///-----------------------------------------------------------------------------

DefineConsoleType( Typeb2AABB )

//-----------------------------------------------------------------------------

namespace Utility
{

//-----------------------------------------------------------------------------

#define STATIC_VOID_CAST_TO( pointerType, castToType, obj ) static_cast<castToType*>( reinterpret_cast<pointerType*>(obj) )
#define DYNAMIC_VOID_CAST_TO( pointerType, castToType, obj ) dynamic_cast<castToType*>( reinterpret_cast<pointerType*>(obj) )

//-----------------------------------------------------------------------------

/// String helpers.
const char* mGetFirstNonWhitespace( const char* inString );
Vector2 mGetStringElementVector( const char* inString, const U32 index = 0 );
VectorF mGetStringElementVector3D( const char* inString, const U32 index = 0 );
const char* mGetStringElement( const char* inString, const U32 index, const bool copyBuffer = true );
U32 mGetStringElementCount( const char *string );

} // Namespace Utility.

#endif // _UTILITY_H_
