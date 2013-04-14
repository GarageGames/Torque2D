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

#ifndef _AMBIENT_FORCE_CONTROLLER_H_
#define _AMBIENT_FORCE_CONTROLLER_H_

#ifndef _GROUPED_SCENE_CONTROLLER_H_
#include "2d/controllers/core/GroupedSceneController.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/vector2.h"
#endif

//------------------------------------------------------------------------------

class AmbientForceController : public GroupedSceneController
{
private:
    typedef GroupedSceneController Parent;

    Vector2 mForce;

public:
    AmbientForceController();
    virtual ~AmbientForceController();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);

    /// Integration.
    virtual void integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );

    inline void setForce( const Vector2& force ) { mForce = force; }
    inline const Vector2& getForce( void ) const { return mForce; }

    /// Declare Console Object.
    DECLARE_CONOBJECT( AmbientForceController );
};

#endif // _AMBIENT_FORCE_CONTROLLER_H_
