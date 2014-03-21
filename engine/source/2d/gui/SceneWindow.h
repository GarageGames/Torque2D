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

#ifndef _SCENE_WINDOW_H_
#define _SCENE_WINDOW_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

#ifndef _SCENE_H_
#include "2d/scene/Scene.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

//-----------------------------------------------------------------------------

class SceneWindow : public GuiControl, public virtual Tickable
{
    typedef GuiControl Parent;

public:
    /// Camera View.
    struct CameraView
    {
        // Source.
        RectF           mSourceArea;
        F32             mCameraZoom;
        F32             mCameraAngle;

        // Destination.
        RectF           mDestinationArea;
        Point2F         mSceneMin;
        Point2F         mSceneMax;
        Point2F         mSceneWindowScale;
        
        CameraView()
        {
           mSourceArea = RectF(0.0f, 0.0f, 10.0f, 10.0f);
           mCameraZoom = 1.0f;
           mCameraAngle = 0.0f;
           mDestinationArea = RectF(0.0f, 0.0f, 10.0f, 10.0f);
           mSceneMin = Point2F(0.0f, 0.0f);
           mSceneMax = Point2F(10.0f, 10.0f);
           mSceneWindowScale = Point2F(1.0f, 1.0f);
        }
    };

private:
    /// Cameras.
    CameraView mCameraCurrent, mCameraSource, mCameraTarget;

    // Camera Interpolation.
    Vector<CameraView>  mCameraQueue;
    S32                 mMaxQueueItems;
    F32                 mCameraTransitionTime;
    F32                 mPreCameraTime;
    F32                 mPostCameraTime;
    F32                 mRenderCameraTime;
    F32                 mCurrentCameraTime;
    bool                mMovingCamera;

    /// Tick Properties.
    Point2F             mPreTickPosition;
    Point2F             mPostTickPosition;

    /// Background color.
    ColorF                      mBackgroundColor;
    bool                        mUseBackgroundColor;

    /// Camera Attachment.
    bool                mCameraMounted;
    SceneObject*        mpMountedTo;
    Vector2             mMountOffset;
    U32                 mMountToID;
    F32                 mMountForce;
    bool                mMountAngle;

    /// View Limit.
    bool                mViewLimitActive;
    Vector2             mViewLimitMin;
    Vector2             mViewLimitMax;
    Vector2             mViewLimitArea;

    /// Camera Shaking.
    bool                mCameraShaking;
    F32                 mShakeLife;
    F32                 mCurrentShake;
    F32                 mShakeRamp;
    Vector2             mCameraShakeOffset;

    /// Misc.
    Scene*              mpScene;
    S32                 mLastRenderTime;
    bool                mLockMouse;
    bool                mWindowDirty;

    // Input Events.
    bool                mUseWindowInputEvents;
    bool                mUseObjectInputEvents;
    U32                 mInputEventGroupMaskFilter;
    U32                 mInputEventLayerMaskFilter;
    bool                mInputEventInvisibleFilter;
    typeWorldQueryResultVector mInputEventQuery;
    typeSceneObjectVector mInputEventEntering;
    typeSceneObjectVector mInputEventLeaving;
    SimSet              mInputEventWatching;
    SimSet              mInputListeners;

    /// Render Masks.
    U32                 mRenderLayerMask;
    U32                 mRenderGroupMask;

    char                mDebugText[256];

    /// Handling Input Events.
    void dispatchInputEvent( StringTableEntry name, const GuiEvent& event );
    void sendWindowInputEvent( StringTableEntry name, const GuiEvent& event );
    void sendObjectInputEvent( StringTableEntry, const GuiEvent& event );

    void calculateCameraView( CameraView* pCameraView );

public:

    /// Camera Interpolation Mode.
    enum CameraInterpolationMode
    {
        INVALID_INTERPOLATION_MODE,

        LINEAR,             ///< Standard Linear.
        SIGMOID             ///< Slow Start / Slow Stop.

    } mCameraInterpolationMode;

    SceneWindow();
    virtual ~SceneWindow();

    virtual bool onAdd();
    virtual void onRemove();

    /// Initialization.
    virtual void setScene( Scene* pScene );
    virtual void resetScene( void );
    inline void setRenderGroups( const U32 groupMask) { mRenderGroupMask = groupMask; }
    inline void setRenderLayers( const U32 layerMask) { mRenderLayerMask = layerMask; }
    inline void setRenderMasks( const U32 layerMask,const  U32 groupMask ) { mRenderLayerMask = layerMask; mRenderGroupMask = groupMask; }
    inline U32 getRenderLayerMask( void ) { return mRenderLayerMask; }
    inline U32 getRenderGroupMask( void ) { return mRenderGroupMask; }

    /// Get scene.
    inline Scene* getScene( void ) const { return mpScene; };

    /// Mouse.
    void setLockMouse( bool lockStatus ) { mLockMouse = lockStatus; };
    bool getLockMouse( void ) { return mLockMouse; };
    Vector2 getMousePosition( void );
    void setMousePosition( const Vector2& mousePosition );

    /// Background color.
    inline void             setBackgroundColor( const ColorF& backgroundColor ) { mBackgroundColor = backgroundColor; }
    inline const ColorF&    getBackgroundColor( void ) const            { return mBackgroundColor; }
    inline void             setUseBackgroundColor( const bool useBackgroundColor ) { mUseBackgroundColor = useBackgroundColor; }
    inline bool             getUseBackgroundColor( void ) const         { return mUseBackgroundColor; }

    /// Input.
    void setObjectInputEventFilter( const U32 groupMask, const U32 layerMask, const bool useInvisible = false );
    void setObjectInputEventGroupFilter( const U32 groupMask );
    void setObjectInputEventLayerFilter( const U32 layerMask );
    void setObjectInputEventInvisibleFilter( const bool useInvisible );
    inline void setUseWindowInputEvents( const bool inputStatus ) { mUseWindowInputEvents = inputStatus; };
    inline void setUseObjectInputEvents( const bool inputStatus ) { mUseObjectInputEvents = inputStatus; };
    inline bool getUseWindowInputEvents( void ) const { return mUseWindowInputEvents; };
    inline bool getUseObjectInputEvents( void ) const { return mUseObjectInputEvents; };
    inline void clearWatchedInputEvents( void ) { mInputEventWatching.clear(); }
    inline void removeFromInputEventPick(SceneObject* pSceneObject ) { mInputEventWatching.removeObject((SimObject*)pSceneObject); }

    void addInputListener( SimObject* pSimObject );
    void removeInputListener( SimObject* pSimObject );

    /// Coordinate Conversion.
    void windowToScenePoint( const Vector2& srcPoint, Vector2& dstPoint ) const;
    void sceneToWindowPoint( const Vector2& srcPoint, Vector2& dstPoint ) const;

    /// Mounting.
    void mount( SceneObject* pSceneObject, const Vector2& mountOffset, const F32 mountForce, const bool sendToMount, const bool mountAngle );
    void dismount( void );
    void dismountMe( SceneObject* pSceneObject );
    void calculateCameraMount( const F32 elapsedTime );
    void interpolateCameraMount( const F32 timeDelta );

    /// View Limit.
    void setViewLimitOn( const Vector2& limitMin, const Vector2& limitMax );
    inline void setViewLimitOff( void ) { mViewLimitActive = false; };
    inline bool isViewLimitOn( void ) const { return mViewLimitActive; }
    inline Vector2 getViewLimitMin( void ) const { return mViewLimitMin; }
    inline Vector2 getViewLimitMax( void ) const { return mViewLimitMax; }
    inline void clampCameraViewLimit( void );

    /// Tick Processing.
    void zeroCameraTime( void );
    void resetTickCameraTime( void );
    void updateTickCameraTime( void );
    void resetTickCameraPosition( void );

    virtual void interpolateTick( F32 delta );
    virtual void processTick();
    virtual void advanceTime( F32 timeDelta ) {};

    /// Camera,
    virtual void setCameraPosition( const Vector2& position );
    inline Vector2 getCameraPosition( void ) const                      { return mCameraCurrent.mSourceArea.centre(); }
    void setCameraSize( const Vector2& size );
    inline Vector2 getCameraSize( void ) const                          { return Vector2( mCameraCurrent.mSourceArea.extent ); }
    virtual void setCameraArea( const RectF& cameraWindow );
    inline RectF getCameraArea( void ) const                            { return mCameraCurrent.mSourceArea; }
    void setCameraZoom( const F32 zoomFactor );
    inline F32 getCameraZoom( void ) const                               { return mCameraCurrent.mCameraZoom; }
    void setCameraAngle( const F32 cameraAngle );
    inline F32 getCameraAngle( void ) const                             { return mRadToDeg(mCameraCurrent.mCameraAngle); }

    /// Target Camera.
    virtual void setTargetCameraPosition( const Vector2& position );
    inline Vector2 getTargetCameraPosition( void ) const                { return mCameraTarget.mSourceArea.centre(); }
    void setTargetCameraSize( const Vector2& size );
    inline Vector2 getTargetCameraSize( void ) const                    { return Vector2( mCameraTarget.mSourceArea.extent ); }
    virtual void setTargetCameraArea( const RectF& cameraWindow );
    inline RectF getTargetCameraArea( void ) const                      { return mCameraTarget.mSourceArea; }
    void setTargetCameraZoom( const F32 zoomFactor );
    inline F32 getTargetCameraZoom( void ) const                        { return mCameraTarget.mCameraZoom; }
    void setTargetCameraAngle( const F32 cameraAngle );
    inline F32 getTargetCameraAngle( void ) const                       { return mRadToDeg(mCameraTarget.mCameraAngle); }

    /// Camera Interpolation Time/Mode.
    void setCameraInterpolationTime( const F32 interpolationTime );
    void setCameraInterpolationMode( const CameraInterpolationMode interpolationMode );

    /// Camera Movement.
    void startCameraMove( const F32 interpolationTime );
    void stopCameraMove( void );
    void completeCameraMove( void );
    void undoCameraMove( const F32 interpolationTime );
    F32 interpolate( F32 from, F32 to, F32 delta );
    void updateCamera( void );

    inline Vector2 getCameraRenderPosition( void )                      { calculateCameraView( &mCameraCurrent ); return mCameraCurrent.mDestinationArea.centre(); }
    inline RectF getCameraRenderArea( void )                            { calculateCameraView( &mCameraCurrent ); return mCameraCurrent.mDestinationArea; }
    inline const Vector2 getCameraWindowScale( void ) const             { return mCameraCurrent.mSceneWindowScale; }
    inline F32 getCameraInterpolationTime( void )                       { return mCameraTransitionTime; }
    inline const CameraView& getCamera(void) const                      { return mCameraCurrent; }
    inline const Vector2& getCameraShake(void) const                    { return mCameraShakeOffset; }
    inline bool isCameraMounted( void ) const                           { return mCameraMounted; }
    inline bool isCameraMoving( void ) const                            { return mMovingCamera; }

    /// Camera Shake.
    void startCameraShake( const F32 magnitude, const F32 time );
    void stopCameraShake( void );

    static void initPersistFields();

    /// GuiControl
    virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
    virtual void onRender( Point2I offset, const RectI& updateRect );

    virtual void onMouseEnter( const GuiEvent& event );
    virtual void onMouseLeave( const GuiEvent& event );

    virtual void onMouseDown( const GuiEvent& event );
    virtual void onMouseUp( const GuiEvent& event );
    virtual void onMouseMove( const GuiEvent& event );
    virtual void onMouseDragged( const GuiEvent& event );

    virtual void onMiddleMouseDown(const GuiEvent &event);
    virtual void onMiddleMouseUp(const GuiEvent &event);
    virtual void onMiddleMouseDragged(const GuiEvent &event);

    virtual void onRightMouseDown( const GuiEvent& event );
    virtual void onRightMouseUp( const GuiEvent& event );
    virtual void onRightMouseDragged( const GuiEvent& event );

    virtual bool onMouseWheelDown( const GuiEvent &event );
    virtual bool onMouseWheelUp( const GuiEvent &event );

    void renderMetricsOverlay( Point2I offset, const RectI& updateRect );

    static CameraInterpolationMode getInterpolationModeEnum(const char* label);

    /// Declare Console Object.
    DECLARE_CONOBJECT(SceneWindow);

protected:
    static bool writeLockMouse( void* obj, StringTableEntry pFieldName ) { return static_cast<SceneWindow*>(obj)->mLockMouse == true; }
    static bool writeUseWindowInputEvents( void* obj, StringTableEntry pFieldName ) { return static_cast<SceneWindow*>(obj)->mUseWindowInputEvents == false; }
    static bool writeUseObjectInputEvents( void* obj, StringTableEntry pFieldName ) { return static_cast<SceneWindow*>(obj)->mUseObjectInputEvents == true; }
    static bool writeBackgroundColor( void* obj, StringTableEntry pFieldName )      { return static_cast<SceneWindow*>(obj)->mUseBackgroundColor == true; }
    static bool writeUseBackgroundColor( void* obj, StringTableEntry pFieldName )   { return static_cast<SceneWindow*>(obj)->mUseBackgroundColor == true; }
};

#endif // _SCENE_WINDOW_H_
