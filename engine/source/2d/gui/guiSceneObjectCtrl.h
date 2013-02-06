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

#ifndef _GUI_SCENE_OBJECT_CTRL_H_
#define _GUI_SCENE_OBJECT_CTRL_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

//-----------------------------------------------------------------------------

class GuiSceneObjectCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

protected:
   StringTableEntry                 mSceneObjectName;
   SimObjectPtr<SceneObject>     mSelectedSceneObject;
   S32                              mMargin;
   StringTableEntry                 mCaption;
   bool                             mHasTexture;
   BatchRender                   mBatchRenderer;
   DebugStats                    mDebugStats;
   
public:
    GuiSceneObjectCtrl();
    static void initPersistFields();

    void setSceneObject( const char *name  );
    inline SceneObject* getSceneObject() { return mSelectedSceneObject; };

    void setCaption( const char* caption );

    /// GuiControl
    bool onWake();
    void onSleep();
    void inspectPostApply();
    void onRender(Point2I offset, const RectI &updateRect);

    void onMouseEnter(const GuiEvent &event);
    void onMouseLeave(const GuiEvent &event);
    void onMouseUp(const GuiEvent &event);
    void onMouseDragged(const GuiEvent &event);

    /// Declare Console Object.
    DECLARE_CONOBJECT( GuiSceneObjectCtrl );
};

#endif // _GUI_SCENE_OBJECT_CTRL_H_
