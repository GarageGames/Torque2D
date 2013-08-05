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

#import "T2DViewController.h"
#import "platformAndroid/platformGL.h"
#include "platformAndroid/AndroidWindow.h"
#include "platformAndroid/platformAndroid.h"
#include "graphics/dgl.h"

extern AndroidPlatState platState;

#define USE_DEPTH_BUFFER 0

extern int _AndroidRunTorqueMain();
extern bool createMouseMoveEvent(S32 i, S32 x, S32 y, S32 lastX, S32 lastY);
extern bool createMouseDownEvent(S32 touchNumber, S32 x, S32 y, U32 numTouches);
extern bool createMouseUpEvent(S32 touchNumber, S32 x, S32 y, S32 lastX, S32 lastY, U32 numTouches); //EFM
extern void createMouseTapEvent( S32 nbrTaps, S32 x, S32 y );
extern void _AndroidGameInnerLoop();
extern void _AndroidGameResignActive();
extern void _AndroidGameBecomeActive();
extern void _AndroidGameWillTerminate();

// Store current orientation for easy access
extern void _AndroidGameChangeOrientation(S32 newOrientation);
//TODO: android
/*
UIDeviceOrientation currentOrientation;
*/
bool _AndroidTorqueFatalError = false;

//-Mat we should update the accelereometer once per frame
extern U32  AccelerometerUpdateMS;
extern void _AndroidGameInnerLoop();

bool createFramebuffer() {
	
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	//TODO: android
	//[self.context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.view.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if (USE_DEPTH_BUFFER) {
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}
	
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		//TODO: android
		//NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}


void destroyFramebuffer() {
	
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	if(depthRenderbuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
void finishGLSetup()
{
    //TODO: android
    //self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    //if (!self.context) {
      //  NSLog(@"Failed to create ES context");
    //}
    
	if( AccelerometerUpdateMS <= 0 ) {
        //Luma:	This variable needs to be store MS value, not Seconds value
        AccelerometerUpdateMS = 33; // 33 ms
	}
	
	//[EAGLContext setCurrentContext:self.context];
	createFramebuffer();
	
    platState.multipleTouchesEnabled = true;
    //[self.view setMultipleTouchEnabled:YES];
    
	_AndroidTorqueFatalError = false;
	if(!_AndroidRunTorqueMain( appDelegate, self.view, self ))
	{
		_AndroidTorqueFatalError = true;
		return;
	}
}

void finishShutdown()
{
	//TODO: android
    // Release any retained subviews of the main view.
    //if ([EAGLContext currentContext] == self.context) {
      //  [EAGLContext setCurrentContext:nil];
    //}
    
    //self.context = nil;
}

void update()
{
    _AndroidGameInnerLoop();
}

extern Vector<Event *> TouchMoveEvents;
Vector<Point2I> lastTouches;

// Handle touch and keyboard input from android OS
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {

	struct engine* engine = (struct engine*)app->userData;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

        if ((AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_DOWN) {

        	size_t touchCount = AMotionEvent_getPointerCount(event);
        	for (U8 i = 0; i < touchCount; i++)
        	{
        		Point2I point;
        		point.x = AMotionEvent_getX(event, i);
        		point.y = AMotionEvent_getY(event, i);

        		if (lastTouches.size() < i)
        			lastTouches.push_back(point);
        		else
        		{
        			lastTouches[i].x = point.x;
        			lastTouches[i].y = point.y;
        		}

        	    S32 orientation = _AndroidGameGetOrientation();
        	    if (UIDeviceOrientationIsPortrait(orientation))
        	    {
        	    	point.y -= _AndroidGetPortraitTouchoffset();
        	    }
        	    createMouseDownEvent(i, point.x, point.y, touchCount);
        	}

        }

        if ((AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_UP) {

        	size_t touchCount = AMotionEvent_getPointerCount(event);
			for (U8 i = 0; i < touchCount; i++)
			{
				Point2I point;
				point.x = AMotionEvent_getX(event, i);
				point.y = AMotionEvent_getY(event, i);
				Point2I prevPoint = lastTouches[i];

				S32 orientation = _AndroidGameGetOrientation();
				if (UIDeviceOrientationIsPortrait(orientation))
				{
					point.y -= _AndroidGetPortraitTouchoffset();
					prevPoint.y -= _AndroidGetPortraitTouchoffset();
				}
				createMouseUpEvent(i, point.x, point.y, prevPoint.x, prevPoint.y, touchCount);

    	        //Luma: Tap support

    	        if (touchCount > 0)
    	        {
    	            // this was a tap, so create a tap event
    	            createMouseTapEvent(touchCount, (int) point.x, (int) point.y);
    	        }
			}
        }

        if ((AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_MOVE) {

        	size_t touchCount = AMotionEvent_getPointerCount(event);
			for (U8 i = 0; i < touchCount; i++)
			{
				Point2I point;
				point.x = AMotionEvent_getX(event, i);
				point.y = AMotionEvent_getY(event, i);
				Point2I prevPoint = lastTouches[i];

				S32 orientation = _AndroidGameGetOrientation();
				if (UIDeviceOrientationIsPortrait(orientation))
				{
					point.y -= _AndroidGetPortraitTouchoffset();
					prevPoint.y -= _AndroidGetPortraitTouchoffset();
				}
				createMouseMoveEvent(i, point.x, point.y, prevPoint.x, prevPoint.y);

				lastTouches[i].x = point.x;
				lastTouches[i].y = point.y;

			}
        }

        if ((AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_CANCEL) {

        	size_t touchCount = AMotionEvent_getPointerCount(event);
			for (U8 i = 0; i < touchCount; i++)
			{
				Point2I point;
				point.x = AMotionEvent_getX(event, i);
				point.y = AMotionEvent_getY(event, i);
				Point2I prevPoint = lastTouches[i];

				S32 orientation = _AndroidGameGetOrientation();
				if (UIDeviceOrientationIsPortrait(orientation))
				{
					point.y -= _AndroidGetPortraitTouchoffset();
					prevPoint.y -= _AndroidGetPortraitTouchoffset();
				}
				createMouseUpEvent(i, point.x, point.y, prevPoint.x, prevPoint.y, touchCount);

				//Luma: Tap support

				if (touchCount > 0)
				{
					// this was a tap, so create a tap event
					createMouseTapEvent(touchCount, (int) point.x, (int) point.y);
				}
			}
        }

        return 1;

    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {

    	int action = AKeyEvent_getAction(event);
    	int key_val = AKeyEvent_getKeyCode(event);
    	int metastate = AKeyEvent_getMetaState(event);

		switch(action)
		{
			case AKEY_EVENT_ACTION_DOWN:

				if (key_val == AKEYCODE_VOLUME_UP) {
					//TODO: android
					//ChangeVolume(true);
				} else if (key_val == AKEYCODE_VOLUME_DOWN) {
					//TODO: android
					//ChangeVolume(false);
				} else if (key_val == AKEYCODE_BACK) {
					//android back button
			    } else {
			        //TODO: android
			    	//convertAndroidToWindowsKeyCode(key_val);
			    }
				break;

			case AKEY_EVENT_ACTION_UP:

				if (key_val == AKEYCODE_VOLUME_UP) {
					//TODO: android
					//ChangeVolume(true);
				} else if (key_val == AKEYCODE_VOLUME_DOWN) {
					//TODO: android
					//ChangeVolume(false);
				} else if (key_val == AKEYCODE_BACK) {
					//android back button
				} else {
					//TODO: android
					//convertAndroidToWindowsKeyCode(key_val);
				}
				break;

		}

    	return 1;
    }

    return 0;
}

int _AndroidGetScreenWidth() {

	return engine.width;
}

int _AndroidGetScreenHeight() {

	return engine.height;
}

bool _AndroidGetFileDescriptor(const char* fileName, int32_t *mDescriptor, off_t *mStart, off_t* mLength) {

	AAsset* asset = AAssetManager_open(engine.app->activity->assetManager, fileName, AASSET_MODE_UNKNOWN);
	if (asset != NULL) {
		*mDescriptor = AAsset_openFileDescriptor(asset, mStart, mLength);
		AAsset_close(asset);
		return true;
	}

	*mDescriptor = 0;
	*mStart = 0;
	*mLength = 0;

	return false;
}

char* _AndroidLoadFile(const char* fileName, int *size) {

	AAsset *asset;
	uint8_t buf[1024];
	char* buffer = NULL;
	*size = 0;
	asset = AAssetManager_open(engine.app->activity->assetManager, fileName, AASSET_MODE_UNKNOWN);
	if(asset != NULL){
		*size = AAsset_getLength(asset);
		if (*size <= 0)
			return NULL;
		buffer = new char[*size + 1];
		int count = 0;
		while(true)
		{
			int read = AAsset_read(asset, buf, 1024);
			if (read <= 0)
				break;

			memcpy(buffer+count,buf, read);
			count += read;

	    }
		buffer[*size] = '\0';

		AAsset_close(asset);
	}

	return buffer;

}

void _AndroidGetDeviceIPAddress(char* address) {

	 int fd;
	 struct ifreq ifr;

	 strcpy(address, "error");

	 // Attaches the current thread to the JVM.
	 jint lResult;
	 jint lFlags = 0;

	 JavaVM* lJavaVM = engine.app->activity->vm;
	 JNIEnv* lJNIEnv = engine.app->activity->env;

	 JavaVMAttachArgs lJavaVMAttachArgs;
	 lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	 lJavaVMAttachArgs.name = "NativeThread";
	 lJavaVMAttachArgs.group = NULL;

	 lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	 if (lResult == JNI_ERR) {
		 return;
	 }

	 // Retrieves NativeActivity.
	 jobject lNativeActivity = engine.app->activity->clazz;
	 jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	 // Retrieves Context.INPUT_METHOD_SERVICE.
	 jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
	 jfieldID FieldWIFI_SERVICE =
		 lJNIEnv->GetStaticFieldID(ClassContext,
			 "WIFI_SERVICE", "Ljava/lang/String;");
	 jobject WIFI_SERVICE =
		 lJNIEnv->GetStaticObjectField(ClassContext,
			 FieldWIFI_SERVICE);

	 // Runs getSystemService(Context.WIFI_SERVICE).
	 jclass ClassWifiManager = lJNIEnv->FindClass(
		 "android/net/wifi/WifiManager");
	 jclass ClassWifiInfo = lJNIEnv->FindClass(
	 		 "android/net/wifi/WifiInfo");
	 jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(
		 ClassNativeActivity, "getSystemService",
		 "(Ljava/lang/String;)Ljava/lang/Object;");
	 jobject lWifiManager = lJNIEnv->CallObjectMethod(
		 lNativeActivity, MethodGetSystemService,
		 WIFI_SERVICE);

	 // Runs wifiManager.getConnectionInfo()
     jmethodID MethodGetConnectionInfo = lJNIEnv->GetMethodID(
			 ClassWifiManager, "getConnectionInfo",
			 "()Landroid/net/wifi/WifiInfo;");
     jobject lWifiInfo = lJNIEnv->CallObjectMethod(
			 lWifiManager, MethodGetConnectionInfo);

     // Runs wifiInfo.getIpAddress()
     jmethodID MethodGetIPAddress = lJNIEnv->GetMethodID(
    		 ClassWifiInfo, "getIpAddress",
     		"()I");
     jint lIPAddress = lJNIEnv->CallIntMethod(
     	     lWifiInfo, MethodGetIPAddress);

     int ip = lIPAddress;

	 // Finished with the JVM.
	 lJavaVM->DetachCurrentThread();

	 char buffer[32];
	 sprintf(buffer, "%d.%d.%d.%d", (ip & 0xFF), (ip >> 8 & 0xFF), (ip >> 16 & 0xFF), (ip >> 24 & 0xFF));
	 strcpy(address, buffer);

}

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            //EGL_ALPHA_SIZE, 8,
            //EGL_DEPTH_SIZE, 24,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    static const EGLint ctx_attribs[] = {
          EGL_CONTEXT_CLIENT_VERSION, 2,
          EGL_NONE
        };

    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    	adprintf("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    engine->animating = 1;

    glViewport(0, 0, engine->width, engine->height);

    if (SetupCompleted == false) {

    	thePVDMan    = new PandaVideoDataManager();
    	thePandaBase = new PandaBase();
    	thePandaBase->SetupVideoData();

    	PandaShader::CreateShaders();

    	loadSplashScreen();

    	PandaAudio::SetupSoundSystem();
    }

    return 0;
}

/**
 * update callback
 */
static void engine_update_frame(struct engine* engine) {

	if (bSuspended == true)
	    return;

	double thisSysTime = timeGetTime();
	float timeElapsed = (thisSysTime-lastSystemTime)/1000.0f;
	if (timeElapsed > 1.0f)
		timeElapsed = 1.0f; // clamp it


#ifdef GOOGLE_PLAY
	if (gOBBPollData == true) {
		gPollElapsed += timeElapsed;
		//check every 10 seconds
		if (gPollElapsed > 10.0f) {
			PollOBBData();
			gPollElapsed = 0.0f;
		}
	}
#endif

	if (SetupCompleted == false) {
		if (timeElapsed > 0.25f) {
			PandaSetup();
			PandaRegainedDevice();
			backBufferFBO[0].Setup(engine_screenWidth(),engine_screenHeight());
			loadDeviceType();
			SetupCompleted = true;
			lastSystemTime = timeGetTime();
		}
	} else {

		lastSystemTime = thisSysTime;

		if (isDeviceiPhone()) {
			if (currentActiveBufferPerc < 1.0f) {
				if (currentBufferInTransition == false) {
					currentActiveBufferPerc += timeElapsed * 2.0f;
					if (currentActiveBufferPerc > 1.0f)
						currentActiveBufferPerc = 1.0f;
				}
			}
		}

		if (keyboardShowing) {
			if (keyboardTransition > 0.0f) {
				keyboardTransition -= timeElapsed * 2.0f;
				if (keyboardTransition < 0.0f)
					keyboardTransition = 0.0f;
			}
		} else {
			if (keyboardTransition < 1.0f) {
				keyboardTransition += timeElapsed * 2.0f;
				if (keyboardTransition > 1.0f)
					keyboardTransition = 1.0f;
			}
		}


		PandaTick(timeElapsed);
		PandaTickMusic();

	}

}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        // No display.
        return;
    }

    if (bSuspended == true)
    	return;

	if (SetupCompleted == false) {
		return;
	}

	if (keyboardShowing == true) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	backBufferFBO[currentActiveBuffer].AcceptRender(true);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	PandaDraw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int height = engine_screenHeight() - (keyboardTransition * (engine_screenHeight()/2));

	//draw tp screen
	if (isDeviceiPhone()) {

		float scale = 1.0f;
		//shrink it to fit screen if we have blown it up
		//if (engine->height > 0 && engine->width == 1024 && engine->height < 768) {
		//	scale = 1.0f / ARTIF_SCALE;
		//}

		if (currentActiveBufferPerc < 1.0f)
			backBufferFBO[otherBuffer].bitmap->DrawBuffer((engine_screenWidth()/2 - ((engine_screenWidth()*currentActiveBufferPerc) * currentBufferDirection)) * scale,height * scale, scale);
		backBufferFBO[currentActiveBuffer].bitmap->DrawBuffer((engine_screenWidth()/2 + ((engine_screenWidth()-(engine_screenWidth()*currentActiveBufferPerc)) * currentBufferDirection)) * scale,height * scale, scale);

	} else {
		float scale = 1.0f;
		float width = engine_screenWidth()/2;
		//shrink it to fit screen if we have blown it up
		//if (engine->height > 0 && engine->width == 1024 && engine->height < 768) {
		//	scale = 1.0f / ARTIF_SCALE;
		//}

		backBufferFBO[currentActiveBuffer].bitmap->DrawBuffer(width*scale,height*scale, scale);
	}

    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine, bool shutdown) {

	if (shutdown == true) {

	}

    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {

	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	//Also we set the currentRotation up so its not invalid
	currentOrientation = [UIDevice currentDevice].orientation;
	//So we make a selector to handle that, called didRotate (lower down in the code)
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didRotate:)
												 name:UIDeviceOrientationDidChangeNotification
											   object:nil];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    _AndroidGameResignActive();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    if(!_AndroidTorqueFatalError)
        _AndroidGameBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    _AndroidGameWillTerminate();

	[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
}

- (void)didRotate:(NSNotification *)notification
{
    //Default to landscape left
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	if(currentOrientation != orientation)
	{
		//Change the orientation
		currentOrientation = orientation;
		//Tell the rest of the engine
		_AndroidGameChangeOrientation(currentOrientation);
	}
}

- (void) runMainLoop
{
	_AndroidGameInnerLoop();
}
*/


void supportLandscape( bool enable)
{
	//TODO: android
    //platState.viewController->mOrientationLandscapeLeftSupported = enable;
    //platState.viewController->mOrientationLandscapeRightSupported = enable;
}

ConsoleFunction(supportLandscape, void, 2, 2, "supportLandscape( bool ) "
                "enable or disable Landscape")
{
    bool enable = true;
    if( argc > 1 )
        enable = dAtob( argv[1] );
    
    supportLandscape(enable);
}

void supportPortrait( bool enable )
{
	//TODO: android
    //platState.viewController->mOrientationPortraitSupported = enable;
    //platState.viewController->mOrientationPortraitUpsideDownSupported = enable;
}

ConsoleFunction(supportPortrait, void, 2, 2, "supportPortrait( bool ) "
                "enable or disable portrait")
{
    bool enable = true;
    if( argc > 1 )
        enable = dAtob( argv[1] );
    
    supportPortrait(enable);
}

