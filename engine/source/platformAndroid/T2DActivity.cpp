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

#include "platformAndroid/T2DActivity.h"
#include "platformAndroid/platformGL.h"
#include "platformAndroid/AndroidWindow.h"
#include "platformAndroid/platformAndroid.h"
#include "graphics/dgl.h"
#include "platform/event.h"
#include "game/gameInterface.h"

extern AndroidPlatState platState;

static void engine_term_display(struct engine* engine, bool shutdown);
extern void android_StartupTime();

T2DActivity activity;

bool keyboardShowing = false;
float keyboardTransition = 1.0f;
bool bSuspended = false;
bool SetupCompleted = false;
double lastSystemTime = 0;

#define USE_DEPTH_BUFFER 0

extern int _AndroidRunTorqueMain(engine *eng);
extern bool createMouseMoveEvent(S32 i, S32 x, S32 y, S32 lastX, S32 lastY);
extern bool createMouseDownEvent(S32 touchNumber, S32 x, S32 y, U32 numTouches);
extern bool createMouseUpEvent(S32 touchNumber, S32 x, S32 y, S32 lastX, S32 lastY, U32 numTouches); //EFM
extern void createMouseTapEvent( S32 nbrTaps, S32 x, S32 y );
extern void _AndroidGameInnerLoop();
extern void _AndroidGameResignActive();
extern void _AndroidGameBecomeActive();
extern void _AndroidGameWillTerminate();

S32 _AndroidGameGetOrientation()
{
	return AConfiguration_getOrientation(platState.engine->app->config);
}

bool _AndroidTorqueFatalError = false;

extern void _AndroidGameInnerLoop();

double timeGetTime() {

    struct timeval  tv;
    gettimeofday(&tv, NULL);

    return ((tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0);

}

void toggleSplashScreen(bool show)
{
	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;

	JavaVM* lJavaVM = platState.engine->app->activity->vm;
	JNIEnv* lJNIEnv = platState.engine->app->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if (lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = platState.engine->app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.SplashScreen");
	jclass SplashScreenClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodSplashScreen = lJNIEnv->GetStaticMethodID(SplashScreenClass, "ToggleSplashScreen", "(Landroid/content/Context;ZII)V");
	lJNIEnv->CallStaticVoidMethod(SplashScreenClass, MethodSplashScreen, lNativeActivity, (jboolean)show, (jint)platState.engine->width, (jint)platState.engine->height);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void ChangeVolume(bool up) {

    // Attaches the current thread to the JVM.
    jint lResult;
    jint lFlags = 0;
    JavaVM* lJavaVM = platState.engine->app->activity->vm;
    JNIEnv* lJNIEnv = platState.engine->app->activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
    if (lResult == JNI_ERR) {
        return;
    }

    // Retrieves NativeActivity.
    jobject lNativeActivity = platState.engine->app->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

    // Retrieves Context.AUDIO_SERVICE.
    jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
    jfieldID FieldAUDIO_SERVICE =lJNIEnv->GetStaticFieldID(ClassContext,"AUDIO_SERVICE", "Ljava/lang/String;");
    jobject AUDIO_SERVICE = lJNIEnv->GetStaticObjectField(ClassContext, FieldAUDIO_SERVICE);

    // Runs getSystemService(AUDIO_SERVICE)
    jclass ClassAudioManager = lJNIEnv->FindClass("android/media/AudioManager");
    jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(ClassNativeActivity, "getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");
    jobject lAudioManager = lJNIEnv->CallObjectMethod(lNativeActivity, MethodGetSystemService, AUDIO_SERVICE);

    //get AudioManager.STREAM_MUSIC
    jfieldID FieldSTREAM_MUSIC =lJNIEnv->GetStaticFieldID(ClassAudioManager,"STREAM_MUSIC", "I");
    jint STREAM_MUSIC = lJNIEnv->GetStaticIntField(ClassAudioManager, FieldSTREAM_MUSIC);

    //get AudioManager.FLAG_SHOW_UI
    jfieldID FieldFLAG_SHOW_UI =lJNIEnv->GetStaticFieldID(ClassAudioManager,"FLAG_SHOW_UI", "I");
    jint FLAG_SHOW_UI = lJNIEnv->GetStaticIntField(ClassAudioManager, FieldFLAG_SHOW_UI);

    if (up) {
    	//get AudioManager.ADJUST_RAISE
    	jfieldID FieldADJUST_RAISE =lJNIEnv->GetStaticFieldID(ClassAudioManager,"ADJUST_RAISE", "I");
    	jint ADJUST_RAISE = lJNIEnv->GetStaticIntField(ClassAudioManager, FieldADJUST_RAISE);

    	//audio.adjustStreamVolume(AudioManager.STREAM_MUSIC, AudioManager.ADJUST_RAISE, AudioManager.FLAG_SHOW_UI);
        jmethodID MethodAdjustStreamVolume = lJNIEnv->GetMethodID(ClassAudioManager, "adjustStreamVolume", "(III)V");
        lJNIEnv->CallVoidMethod(lAudioManager,MethodAdjustStreamVolume, STREAM_MUSIC, ADJUST_RAISE, FLAG_SHOW_UI);

    } else {
    	//get AudioManager.ADJUST_LOWER
    	jfieldID FieldADJUST_LOWER =lJNIEnv->GetStaticFieldID(ClassAudioManager,"ADJUST_LOWER", "I");
    	jint ADJUST_LOWER = lJNIEnv->GetStaticIntField(ClassAudioManager, FieldADJUST_LOWER);

    	//audio.adjustStreamVolume(AudioManager.STREAM_MUSIC, AudioManager.ADJUST_LOWER, AudioManager.FLAG_SHOW_UI);
    	jmethodID MethodAdjustStreamVolume = lJNIEnv->GetMethodID(ClassAudioManager, "adjustStreamVolume", "(III)V");
    	lJNIEnv->CallVoidMethod(lAudioManager,MethodAdjustStreamVolume, STREAM_MUSIC, ADJUST_LOWER, FLAG_SHOW_UI);
    }

    // Finished with the JVM.
    lJavaVM->DetachCurrentThread();
}

int convertAndroidToWindowsKeyCode(int key) {

	switch(key) {
	case AKEYCODE_0:
		return 48;
	case AKEYCODE_1:
		return 49;
	case AKEYCODE_2:
		return 50;
	case AKEYCODE_3:
		return 51;
	case AKEYCODE_4:
		return 52;
	case AKEYCODE_5:
		return 53;
	case AKEYCODE_6:
		return 54;
	case AKEYCODE_7:
		return 55;
	case AKEYCODE_8:
		return 56;
	case AKEYCODE_9:
		return 57;
	case AKEYCODE_A:
		return 65;
	case AKEYCODE_ALT_LEFT:
		return 0xA4;
	case AKEYCODE_ALT_RIGHT:
		return 0xA5;
	case AKEYCODE_APOSTROPHE:
		return 39;
	case AKEYCODE_AT:
		return 64;
	case AKEYCODE_B:
		return 66;
	case AKEYCODE_BACKSLASH:
		return 92;
	case AKEYCODE_C:
		return 67;
	//case KEYCODE_CAPS_LOCK:
	//	return 0x14;
	case AKEYCODE_COMMA:
		return 44;
	case AKEYCODE_D:
		return 68;
	case AKEYCODE_DEL:
		return 8;
	case AKEYCODE_E:
		return 69;
	case AKEYCODE_ENTER:
		return 13;
	case AKEYCODE_EQUALS:
		return 61;
	//case AKEYCODE_ESCAPE:
	//	return 0x1B;
	case AKEYCODE_F:
		return 70;
	//case AKEYCODE_F1:
	//	return 0x70;
	//case AKEYCODE_F10:
	//	return 0x79;
	//case AKEYCODE_F11:
	//	return 0x7A;
	//case AKEYCODE_F12:
	//	return 0x7B;
	//case AKEYCODE_F2:
	//	return 0x71;
	//case AKEYCODE_F3:
	//	return 0x72;
	//case AKEYCODE_F4:
	//	return 0x73;
	//case AKEYCODE_F5:
	//	return 0x74;
	//case AKEYCODE_F6:
	//	return 0x75;
	//case AKEYCODE_F7:
	//	return 0x76;
	//case AKEYCODE_F8:
	//	return 0x77;
	//case AKEYCODE_F9:
	//	return 0x78;
	case AKEYCODE_G:
		return 71;
	case AKEYCODE_GRAVE:
		return 96;
	case AKEYCODE_H:
		return 72;
	case AKEYCODE_I:
		return 73;
	//case AKEYCODE_INSERT:
	//	return 0x2D;
	case AKEYCODE_J:
		return 74;
	case AKEYCODE_K:
		return 75;
	case AKEYCODE_L:
		return 76;
	case AKEYCODE_LEFT_BRACKET:
		return 91;
	case AKEYCODE_M:
		return 77;
	case AKEYCODE_MINUS:
		return 45;
	case AKEYCODE_N:
		return 78;
	case AKEYCODE_O:
		return 79;
	case AKEYCODE_P:
		return 80;
	case AKEYCODE_PAGE_DOWN:
		return 0x22;
	case AKEYCODE_PAGE_UP:
		return 0x21;
	case AKEYCODE_PERIOD:
		return 46;
	case AKEYCODE_PLUS:
		return 43;
	case AKEYCODE_POUND:
		return 35;
	case AKEYCODE_Q:
		return 81;
	case AKEYCODE_R:
		return 82;
	case AKEYCODE_RIGHT_BRACKET:
		return 93;
	case AKEYCODE_S:
		return 83;
	case AKEYCODE_SEMICOLON:
		return 59;
	case AKEYCODE_SHIFT_LEFT:
		return 0xA0;
	case AKEYCODE_SHIFT_RIGHT:
		return 0xA1;
	case AKEYCODE_SLASH:
		return 47;
	case AKEYCODE_SPACE:
		return 32;
	case AKEYCODE_STAR:
		return 42;
	case AKEYCODE_T:
		return 84;
	case AKEYCODE_TAB:
		return 0x09;
	case AKEYCODE_U:
		return 85;
	case AKEYCODE_V:
		return 86;
	case AKEYCODE_W:
		return 87;
	case AKEYCODE_X:
		return 88;
	case AKEYCODE_Y:
		return 89;
	case AKEYCODE_Z:
		return 90;
	default:
		return 0;
	}

	return 0;

}


void androidKeyboardEvent(int keyval, bool make) {

   S32 keyCode = convertAndroidToWindowsKeyCode(keyval);

   InputEvent event;
   event.deviceInst = 0;
   event.deviceType = KeyboardDeviceType;
   event.objType    = SI_KEY;
   event.objInst    = keyCode;
   event.action     = make ? SI_MAKE : SI_BREAK;
   event.modifier   = 0;
   event.ascii      = keyCode;
   event.fValues[0]  = make ? 1.0f : 0.0f;

   Game->postEvent(event);
}

void displayKeyboard(bool pShow) {
    // Attaches the current thread to the JVM.
    jint lResult;
    jint lFlags = 0;

    JavaVM* lJavaVM = platState.engine->app->activity->vm;
    JNIEnv* lJNIEnv = platState.engine->app->activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
    if (lResult == JNI_ERR) {
        return;
    }

    // Retrieves NativeActivity.
    jobject lNativeActivity = platState.engine->app->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

    // Retrieves Context.INPUT_METHOD_SERVICE.
    jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
    jfieldID FieldINPUT_METHOD_SERVICE =
        lJNIEnv->GetStaticFieldID(ClassContext,
            "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
    jobject INPUT_METHOD_SERVICE =
        lJNIEnv->GetStaticObjectField(ClassContext,
            FieldINPUT_METHOD_SERVICE);
    //jniCheck(INPUT_METHOD_SERVICE);

    // Runs getSystemService(Context.INPUT_METHOD_SERVICE).
    jclass ClassInputMethodManager = lJNIEnv->FindClass(
        "android/view/inputmethod/InputMethodManager");
    jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(
        ClassNativeActivity, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject lInputMethodManager = lJNIEnv->CallObjectMethod(
        lNativeActivity, MethodGetSystemService,
        INPUT_METHOD_SERVICE);

    // Runs getWindow().getDecorView().
    jmethodID MethodGetWindow = lJNIEnv->GetMethodID(
        ClassNativeActivity, "getWindow",
        "()Landroid/view/Window;");
    jobject lWindow = lJNIEnv->CallObjectMethod(lNativeActivity,
        MethodGetWindow);
    jclass ClassWindow = lJNIEnv->FindClass(
        "android/view/Window");
    jmethodID MethodGetDecorView = lJNIEnv->GetMethodID(
        ClassWindow, "getDecorView", "()Landroid/view/View;");
    jobject lDecorView = lJNIEnv->CallObjectMethod(lWindow,
        MethodGetDecorView);

    if (pShow) {
        // Runs lInputMethodManager.showSoftInput(...).
        jmethodID MethodShowSoftInput = lJNIEnv->GetMethodID(
            ClassInputMethodManager, "showSoftInput",
            "(Landroid/view/View;I)Z");
        jboolean lResult = lJNIEnv->CallBooleanMethod(
            lInputMethodManager, MethodShowSoftInput,
            lDecorView, lFlags);
    } else {
        // Runs lWindow.getViewToken()
        jclass ClassView = lJNIEnv->FindClass(
            "android/view/View");
        jmethodID MethodGetWindowToken = lJNIEnv->GetMethodID(
            ClassView, "getWindowToken", "()Landroid/os/IBinder;");
        jobject lBinder = lJNIEnv->CallObjectMethod(lDecorView,
            MethodGetWindowToken);

        // lInputMethodManager.hideSoftInput(...).
        jmethodID MethodHideSoftInput = lJNIEnv->GetMethodID(
            ClassInputMethodManager, "hideSoftInputFromWindow",
            "(Landroid/os/IBinder;I)Z");
        jboolean lRes = lJNIEnv->CallBooleanMethod(
            lInputMethodManager, MethodHideSoftInput,
            lBinder, lFlags);
    }

    // Finished with the JVM.
    lJavaVM->DetachCurrentThread();
    keyboardShowing = pShow;
}

Point2I rawLastTouches[10];

// Handle touch and keyboard input from android OS
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {

	struct engine* engine = (struct engine*)app->userData;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

        int32_t eventValue = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK);

        if (eventValue == AMOTION_EVENT_ACTION_DOWN || eventValue == AMOTION_EVENT_ACTION_POINTER_DOWN) {

            size_t touchCount = AMotionEvent_getPointerCount(event);
            for (U8 i = 0; i < touchCount; i++)
            {
                Point2I point;
                point.x = AMotionEvent_getX(event, i);
                point.y = AMotionEvent_getY(event, i);

                rawLastTouches[i].x = point.x;
                rawLastTouches[i].y = point.y;

                createMouseDownEvent(i, point.x, point.y, touchCount);
            }

        }

        if (eventValue == AMOTION_EVENT_ACTION_UP || eventValue == AMOTION_EVENT_ACTION_POINTER_UP) {

            size_t touchCount = AMotionEvent_getPointerCount(event);
            for (U8 i = 0; i < touchCount; i++)
            {
                Point2I point;
                point.x = AMotionEvent_getX(event, i);
                point.y = AMotionEvent_getY(event, i);
                Point2I prevPoint = rawLastTouches[i];

                createMouseUpEvent(i, point.x, point.y, prevPoint.x, prevPoint.y, touchCount);

                if (touchCount > 0)
                {
                    // this was a tap, so create a tap event
                    createMouseTapEvent(touchCount, (int) point.x, (int) point.y);
                }
            }
        }

        if (eventValue == AMOTION_EVENT_ACTION_MOVE) {

            size_t touchCount = AMotionEvent_getPointerCount(event);
            for (U8 i = 0; i < touchCount; i++)
            {
                Point2I point;
                point.x = AMotionEvent_getX(event, i);
                point.y = AMotionEvent_getY(event, i);
                Point2I prevPoint = rawLastTouches[i];

                createMouseMoveEvent(i, point.x, point.y, prevPoint.x, prevPoint.y);

                rawLastTouches[i].x = point.x;
                rawLastTouches[i].y = point.y;

            }
        }

        if (eventValue == AMOTION_EVENT_ACTION_CANCEL) {

            size_t touchCount = AMotionEvent_getPointerCount(event);
            for (U8 i = 0; i < touchCount; i++)
            {
                Point2I point;
                point.x = AMotionEvent_getX(event, i);
                point.y = AMotionEvent_getY(event, i);
                Point2I prevPoint = rawLastTouches[i];

                createMouseUpEvent(i, point.x, point.y, prevPoint.x, prevPoint.y, touchCount);

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
					ChangeVolume(true);
				} else if (key_val == AKEYCODE_VOLUME_DOWN) {
					ChangeVolume(false);
				} else if (key_val == AKEYCODE_BACK) {
					if (!keyboardShowing)
						Con::executef(2, "androidBackButton", Con::getBoolArg(true));
			    } else {
			    	androidKeyboardEvent(key_val, true);
			    }
				break;

			case AKEY_EVENT_ACTION_UP:

				if (key_val == AKEYCODE_VOLUME_UP) {
					ChangeVolume(true);
				} else if (key_val == AKEYCODE_VOLUME_DOWN) {
					ChangeVolume(false);
				} else if (key_val == AKEYCODE_BACK) {
					if (keyboardShowing)
						displayKeyboard(false);
					else
						Con::executef(2, "androidBackButton", Con::getBoolArg(false));
				} else {
					androidKeyboardEvent(key_val, false);
				}
				break;

		}

    	return 1;
    }

    return 0;
}

int _AndroidGetScreenWidth() {

	return platState.engine->width;
}

int _AndroidGetScreenHeight() {

	return platState.engine->height;
}

char* _AndroidLoadFile(const char* fn, U32 *size) {

	char fileName[255];

	//Check for .. in path and remove them so apk reading code doesnt choke on it
	std::string filepath = fn;
	//first remove any cases of /./
	std::string find = "/./";
	size_t start_pos = filepath.find(find);
	while (start_pos != std::string::npos)
	{
		filepath.replace(start_pos+1, find.length()-1, "");
		start_pos = filepath.find(find);
	}

	find = "/../";
	start_pos = filepath.find(find);
	while (start_pos != std::string::npos)
	{
		size_t begin_pos = filepath.rfind("/", start_pos-1);
		filepath.replace(begin_pos, (start_pos - begin_pos - 1) + find.length(), "");
		start_pos = filepath.find(find);

	}
	if (filepath.find("/") == 0)
	{
		strcpy(fileName, filepath.substr(1).c_str());
		fileName[filepath.size()-1] = '\0';
	}
	else
	{
		strcpy(fileName, filepath.c_str());
		fileName[filepath.size()] = '\0';
	}

	AAsset *asset;
	uint8_t buf[1024];
	char* buffer = NULL;
	*size = 0;
	asset = AAssetManager_open(platState.engine->app->activity->assetManager, fileName, AASSET_MODE_UNKNOWN);
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
	 strcpy(address, "error");

	 // Attaches the current thread to the JVM.
	 jint lResult;
	 jint lFlags = 0;
	 JavaVM* lJavaVM = platState.engine->app->activity->vm;
	 JNIEnv* lJNIEnv = platState.engine->app->activity->env;

	 JavaVMAttachArgs lJavaVMAttachArgs;
	 lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	 lJavaVMAttachArgs.name = "NativeThread";
	 lJavaVMAttachArgs.group = NULL;

	 lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	 if (lResult == JNI_ERR) {
		 return;
	 }

	 // Retrieves NativeActivity.
	 jobject lNativeActivity = platState.engine->app->activity->clazz;
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
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_NONE
    };

    static const EGLint ctx_attribs[] = {
          EGL_CONTEXT_CLIENT_VERSION, 1,
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

    if (SetupCompleted == false)
    {
    	toggleSplashScreen(true);
    	if (_AndroidRunTorqueMain(engine) == 0)
    	{
    		engine->app->destroyRequested = 1;
    		engine_term_display(engine, true);
			return -1;
    	}
    	activity.finishGLSetup();
    	SetupCompleted = true;
    }
    else
    {
    	if(!_AndroidTorqueFatalError)
    	        _AndroidGameBecomeActive();
    }

    return 0;
}

void T2DActivity::finishGLSetup()
{
    platState.multipleTouchesEnabled = true;

	_AndroidTorqueFatalError = false;
}

void T2DActivity::finishShutdown()
{

}

void T2DActivity::enableAccelerometer(bool enable) {

	accelerometerActive = enable;

	if (enable) {

		if (platState.engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(platState.engine->sensorEventQueue,
					platState.engine->accelerometerSensor);
			// We'd like to get 30 events per second (in us).
			ASensorEventQueue_setEventRate(platState.engine->sensorEventQueue,
					platState.engine->accelerometerSensor, (1000L/30)*1000);
		}

	} else {

		if (platState.engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(platState.engine->sensorEventQueue,
					platState.engine->accelerometerSensor);
		}

	}

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

	lastSystemTime = thisSysTime;

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

	if (SetupCompleted == true)
		_AndroidGameInnerLoop();

}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine, bool shutdown) {

	if (shutdown == true) {
		_AndroidGameWillTerminate();
	} else {
		 _AndroidGameResignActive();
	}

	activity.finishShutdown();

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

void keepScreenOn() {

    // Attaches the current thread to the JVM.
    jint lResult;
    jint lFlags = 0;
    JavaVM* lJavaVM = platState.engine->app->activity->vm;
    JNIEnv* lJNIEnv = platState.engine->app->activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
    if (lResult == JNI_ERR) {
        return;
    }

    // Retrieves NativeActivity.
    jobject lNativeActivity = platState.engine->app->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

    // Runs getWindow().getDecorView().
    jmethodID MethodGetWindow = lJNIEnv->GetMethodID(
        ClassNativeActivity, "getWindow",
        "()Landroid/view/Window;");
    jobject lWindow = lJNIEnv->CallObjectMethod(lNativeActivity,
        MethodGetWindow);
    jclass ClassWindow = lJNIEnv->FindClass(
        "android/view/Window");
    jmethodID MethodGetDecorView = lJNIEnv->GetMethodID(
        ClassWindow, "getDecorView", "()Landroid/view/View;");
    jobject lDecorView = lJNIEnv->CallObjectMethod(lWindow,
        MethodGetDecorView);

    jclass ClassView = lJNIEnv->FindClass("android/view/View");
    jmethodID MethodSetKeepScreenOn = lJNIEnv->GetMethodID(
            ClassView, "setKeepScreenOn", "(Z)V");
    jboolean on = true;
    lJNIEnv->CallVoidMethod(lDecorView, MethodSetKeepScreenOn, on);

    // Finished with the JVM.
    lJavaVM->DetachCurrentThread();
}

void T2DActivity::loadCacheDir() {

    // Attaches the current thread to the JVM.
    jint lResult;
    jint lFlags = 0;
    JavaVM* lJavaVM = platState.engine->app->activity->vm;
    JNIEnv* lJNIEnv = platState.engine->app->activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
    if (lResult == JNI_ERR) {
        return;
    }

    // Retrieves NativeActivity.
    jobject lNativeActivity = platState.engine->app->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

    jmethodID getCacheDir = lJNIEnv->GetMethodID(ClassNativeActivity, "getCacheDir", "()Ljava/io/File;");
    jobject file = lJNIEnv->CallObjectMethod(platState.engine->app->activity->clazz, getCacheDir);
    jclass fileClass = lJNIEnv->FindClass("java/io/File");
    jmethodID getAbsolutePath = lJNIEnv->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring jpath = (jstring)lJNIEnv->CallObjectMethod(file, getAbsolutePath);
    const char* app_dir = lJNIEnv->GetStringUTFChars(jpath, NULL);

    strcpy(cacheDir, app_dir);
    cacheDir[strlen(app_dir)] = '\0';

    lJNIEnv->ReleaseStringUTFChars(jpath, app_dir);

    // Finished with the JVM.
    lJavaVM->DetachCurrentThread();

}

void T2DActivity::enumerateFonts() {

	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;
	JavaVM* lJavaVM = platState.engine->app->activity->vm;
	JNIEnv* lJNIEnv = platState.engine->app->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if (lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = platState.engine->app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FontManager");
	jclass FontManagerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodFontManager = lJNIEnv->GetStaticMethodID(FontManagerClass, "enumerateFonts", "(Landroid/content/Context;)V");
	lJNIEnv->CallStaticVoidMethod(FontManagerClass, MethodFontManager, lNativeActivity);
	lJNIEnv->DeleteLocalRef(strClassName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void T2DActivity::dumpFontList() {

	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;
	JavaVM* lJavaVM = platState.engine->app->activity->vm;
	JNIEnv* lJNIEnv = platState.engine->app->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if (lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = platState.engine->app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FontManager");
	jclass FontManagerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodFontManager = lJNIEnv->GetStaticMethodID(FontManagerClass, "dumpFontList", "()V");
	lJNIEnv->CallStaticVoidMethod(FontManagerClass, MethodFontManager);
	lJNIEnv->DeleteLocalRef(strClassName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void T2DActivity::getFontPath(const char* fontName, char* fontPath) {

	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;
	JavaVM* lJavaVM = platState.engine->app->activity->vm;
	JNIEnv* lJNIEnv = platState.engine->app->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if (lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = platState.engine->app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FontManager");
	jstring strFontName = lJNIEnv->NewStringUTF(fontName);
	jclass FontManagerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodFontManager = lJNIEnv->GetStaticMethodID(FontManagerClass, "getFont", "(Ljava/lang/String;)Ljava/lang/String;");
	jstring jpath = (jstring)lJNIEnv->CallStaticObjectMethod(FontManagerClass, MethodFontManager, strFontName);
	if (jpath != NULL)
	{
		const char* path = lJNIEnv->GetStringUTFChars(jpath, NULL);
		strcpy(fontPath, path);
		fontPath[strlen(path)] = '\0';
		lJNIEnv->ReleaseStringUTFChars(jpath, path);
	}
	else
	{
		strcpy(fontPath,"");
	}


	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strFontName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {

                engine_init_display(engine);

                if (bSuspended == true) {
					glViewport(0, 0, engine->width, engine->height);
					Game->textureResurrect();
					bSuspended = false;
				}
            }
            break;
        case APP_CMD_TERM_WINDOW:
			bSuspended = true;
			Game->textureKill();
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine, false);
            break;
        case APP_CMD_RESUME:
            break;
        case APP_CMD_GAINED_FOCUS:
        	// When our app gains focus, we start monitoring the accelerometer.
			if (engine->accelerometerSensor != NULL && activity.isAccelerometerActive()) {
				ASensorEventQueue_enableSensor(engine->sensorEventQueue,
						engine->accelerometerSensor);
				// We'd like to get 30 events per second (in us).
				ASensorEventQueue_setEventRate(engine->sensorEventQueue,
						engine->accelerometerSensor, (1000L/30)*1000);
			}

            engine->animating = 1;
            break;
        case APP_CMD_LOST_FOCUS:
        	// When our app loses focus, we stop monitoring the accelerometer.
			// This is to avoid consuming battery while not being used.
			if (engine->accelerometerSensor != NULL) {
				ASensorEventQueue_disableSensor(engine->sensorEventQueue,
						engine->accelerometerSensor);
			}

            // stop animating.
        	engine->animating = 0;
            break;
        case APP_CMD_PAUSE:
            break;
    }
}

struct engine engine;

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {

	//init startup time so U32 doesnt overflow
	android_StartupTime();

    // Make sure glue isn't stripped.
    app_dummy();

    platState.engine = &engine;

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
			ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
			state->looper, LOOPER_ID_USER, NULL, NULL);


    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    keepScreenOn();

    //This is to help the debugger catch up.  If you dont have this you cant debug this early in the execution
    //so only uncomment when needed as it adds 10 seconds to startup
    //sleep(10);

    //store the cache dir
    activity.loadCacheDir();
    activity.loadInternalDir();

    //enumerate fonts
    activity.enumerateFonts();

    platState.argc = 0;

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL && activity.isAccelerometerActive()) {
					ASensorEvent event;
					int i = 0;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
							&event, 1) > 0) {

						InputEvent inputEvent;
						U32 accelAxes[6] = { SI_ACCELX, SI_ACCELY, SI_ACCELZ, SI_GRAVX, SI_GRAVY, SI_GRAVZ };
						double userAcc[6] = { event.acceleration.x, event.acceleration.y, event.acceleration.z,0,0,0};

						inputEvent.deviceInst = 0;
						inputEvent.fValues[0] = userAcc[i];
						inputEvent.deviceType = AccelerometerDeviceType;
						inputEvent.objType = accelAxes[i];
						inputEvent.objInst = i;
						inputEvent.action = SI_MOTION;
						inputEvent.modifier = 0;

						Game->postEvent(inputEvent);

						i++;
					}
				}
			}

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine, true);
                return;
            }
        }

        engine_update_frame(&engine);

        /*if (engine.animating) {

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }*/
    }

    engine_term_display(&engine, true);
}

struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};

bool android_DumpDirectoriesExtra(Vector<StringTableEntry> &directoryVector)
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodExtraPaths = lJNIEnv->GetStaticMethodID(FileWalkerClass, "getRestOfDump", "()[Ljava/lang/String;");
	jobjectArray stringArray = (jobjectArray)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodExtraPaths);

	bool ret = true;
	if (stringArray)
	{
		int stringCount = lJNIEnv->GetArrayLength(stringArray);

		if (stringCount < 500)
			ret = false;

		for (int i=0; i<stringCount; i++) {
			jstring string = (jstring) lJNIEnv->GetObjectArrayElement(stringArray, i);
			const char *rawString = lJNIEnv->GetStringUTFChars(string, 0);
			char str[255];
			strcpy(str, rawString);
			lJNIEnv->ReleaseStringUTFChars(string, rawString);

			if (!Platform::isExcludedDirectory(str))
			{
				directoryVector.push_back(StringTable->insert(str));
			}
		}
		lJNIEnv->DeleteLocalRef(stringArray);
	}
	else
	{
		ret = false;
	}

	lJNIEnv->DeleteLocalRef(strClassName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();

	return ret;
}

static Vector<Platform::FileInfo> dumpPathBackup;

bool android_DumpDirectories(const char *basePath, const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath)
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(basePath);
	jstring strDirName2 = lJNIEnv->NewStringUTF(path);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "DumpDirectories", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;ZZ)[Ljava/lang/String;");
	jobjectArray stringArray = (jobjectArray)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, strDirName, strDirName2, depth != 0, noBasePath);

	bool ret = false;

	if (stringArray)
	{
		int stringCount = lJNIEnv->GetArrayLength(stringArray);

		for (int i=0; i<stringCount; i++) {
			jstring string = (jstring) lJNIEnv->GetObjectArrayElement(stringArray, i);
			const char *rawString = lJNIEnv->GetStringUTFChars(string, 0);
			char str[255];
			strcpy(str, rawString);
			lJNIEnv->ReleaseStringUTFChars(string, rawString);

			if (!Platform::isExcludedDirectory(str))
			{
				directoryVector.push_back(StringTable->insert(str));
			}


		}

		lJNIEnv->DeleteLocalRef(stringArray);

		bool keepGoing = false;

		if (stringCount == 500)
		{
			keepGoing = true;
		}

		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);
		lJNIEnv->DeleteLocalRef(strDirName2);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();

		while (keepGoing == true)
			keepGoing = android_DumpDirectoriesExtra(directoryVector);

		ret = true;
	}
	else
	{
		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);
		lJNIEnv->DeleteLocalRef(strDirName2);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();

		ret = false;
	}

	return ret;
}

bool android_DumpPathExtra(Vector<Platform::FileInfo>& fileVector)
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodExtraPaths = lJNIEnv->GetStaticMethodID(FileWalkerClass, "getRestOfDump", "()[Ljava/lang/String;");
	jobjectArray stringArray = (jobjectArray)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodExtraPaths);

	bool ret = true;
	if (stringArray)
	{
		int stringCount = lJNIEnv->GetArrayLength(stringArray);

		if (stringCount < 500)
			ret = false;

		for (int i=0; i<stringCount; i++) {
			jstring string = (jstring) lJNIEnv->GetObjectArrayElement(stringArray, i);
			const char *rawString = lJNIEnv->GetStringUTFChars(string, 0);
			std::string str = rawString;
			lJNIEnv->ReleaseStringUTFChars(string, rawString);

			const U32 fileSize = Platform::getFileSize(str.c_str());
			fileVector.increment();
			Platform::FileInfo& rInfo = fileVector.last();
			std::string fileName = std::string(
					std::find_if( str.rbegin(), str.rend(),
								  MatchPathSeparator() ).base(),
					str.end() );
			rInfo.pFullPath = StringTable->insert(str.substr(0,str.find(fileName)-1).c_str());
			rInfo.pFileName = StringTable->insert(fileName.c_str());
			rInfo.fileSize  = fileSize;

			dumpPathBackup.increment();
			Platform::FileInfo& rInfo2 = dumpPathBackup.last();
			rInfo2.pFullPath = rInfo.pFullPath;
			rInfo2.pFileName = rInfo.pFileName;
			rInfo2.fileSize  = rInfo.fileSize;

		}
		lJNIEnv->DeleteLocalRef(stringArray);
	}
	else
	{
		ret = false;
	}

	lJNIEnv->DeleteLocalRef(strClassName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();

	return ret;

}

bool loadDumpPathFromCache(const char* dirPath, Vector<Platform::FileInfo>& fileVector, U32 depth)
{
	char dir[255];

	//Check for .. in path and remove them so apk reading code doesnt choke on it
	std::string filepath = dirPath;
	//first remove any cases of /./
	std::string find = "/./";
	size_t start_pos = filepath.find(find);
	while (start_pos != std::string::npos)
	{
		filepath.replace(start_pos+1, find.length()-1, "");
		start_pos = filepath.find(find);
	}

	find = "/../";
	start_pos = filepath.find(find);
	while (start_pos != std::string::npos)
	{
		size_t begin_pos = filepath.rfind("/", start_pos-1);
		filepath.replace(begin_pos, (start_pos - begin_pos - 1) + find.length(), "");
		start_pos = filepath.find(find);

	}

	strcpy(dir, filepath.c_str());
	dir[filepath.size()] = '\0';


	bool foundPaths = false;
	for (int i = 0; i < dumpPathBackup.size(); i++)
	{
		if (depth == 0)
		{
			Platform::FileInfo fi = dumpPathBackup[i];
			if (strcmp(fi.pFullPath, dir) == 0)
			{
				fileVector.increment();
				Platform::FileInfo& rInfo = fileVector.last();
				rInfo.pFileName = fi.pFileName;
				rInfo.pFullPath = fi.pFullPath;
				rInfo.fileSize = fi.fileSize;
				foundPaths = true;
			}
		}
		else
		{
			Platform::FileInfo fi = dumpPathBackup[i];
			std::string fullPath = fi.pFullPath;

			if (fullPath.find(dir) == 0)
			{
				fileVector.increment();
				Platform::FileInfo& rInfo = fileVector.last();
				rInfo.pFileName = fi.pFileName;
				rInfo.pFullPath = fi.pFullPath;
				rInfo.fileSize = fi.fileSize;
				foundPaths = true;
			}
		}
	}

	return true;
}

bool android_DumpPath(const char* dir, Vector<Platform::FileInfo>& fileVector, U32 depth)
{
	if (dumpPathBackup.size() > 0)
	{
		return loadDumpPathFromCache(dir, fileVector, depth);
	}

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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(dir);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "DumpPath", "(Landroid/content/Context;Ljava/lang/String;Z)[Ljava/lang/String;");
	jobjectArray stringArray = (jobjectArray)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, strDirName, depth != 0);

	bool ret = false;

	if (stringArray)
	{
		int stringCount = lJNIEnv->GetArrayLength(stringArray);

		for (int i=0; i<stringCount; i++) {
			jstring string = (jstring) lJNIEnv->GetObjectArrayElement(stringArray, i);
			const char *rawString = lJNIEnv->GetStringUTFChars(string, 0);
			std::string str = rawString;
			lJNIEnv->ReleaseStringUTFChars(string, rawString);

			const U32 fileSize = Platform::getFileSize(str.c_str());
			fileVector.increment();
			Platform::FileInfo& rInfo = fileVector.last();
			std::string fileName = std::string(
			        std::find_if( str.rbegin(), str.rend(),
			                      MatchPathSeparator() ).base(),
			        str.end() );
			rInfo.pFullPath = StringTable->insert(str.substr(0,str.find(fileName)-1).c_str());
			rInfo.pFileName = StringTable->insert(fileName.c_str());
			rInfo.fileSize  = fileSize;

			dumpPathBackup.increment();
			Platform::FileInfo& rInfo2 = dumpPathBackup.last();
			rInfo2.pFullPath = rInfo.pFullPath;
			rInfo2.pFileName = rInfo.pFileName;
			rInfo2.fileSize  = rInfo.fileSize;

		}
		lJNIEnv->DeleteLocalRef(stringArray);
		stringArray = NULL;

		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();

		bool keepGoing = false;

		if (stringCount == 500)
		{
			keepGoing = true;
		}

		while (keepGoing == true)
			keepGoing = android_DumpPathExtra(fileVector);

		ret = true;
	}
	else
	{
		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();
		ret = false;
	}

	return ret;

}

void android_InitDirList(const char* dir)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(dir);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "InitDirList", "(Landroid/content/Context;Ljava/lang/String;)V");
	lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, strDirName);
	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strDirName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();


}

void android_GetNextDir(const char* pdir, char *dir)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(pdir);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "GetNextDir", "(Ljava/lang/String;)Ljava/lang/String;");
	jstring jpath = (jstring)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodFileWalker, strDirName);
	if (jpath != NULL)
	{
		const char* path = lJNIEnv->GetStringUTFChars(jpath, NULL);
		strcpy(dir, path);
        dir[strlen(path)] = '\0';
        lJNIEnv->ReleaseStringUTFChars(jpath, path);
	}
	else
	{
		strcpy(dir,"");
	}

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strDirName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_GetNextFile(const char* pdir, char *file)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(pdir);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "GetNextFile", "(Ljava/lang/String;)Ljava/lang/String;");
	jstring jpath = (jstring)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodFileWalker, strDirName);
	if (jpath != NULL)
	{
		const char* path = lJNIEnv->GetStringUTFChars(jpath, NULL);
		strcpy(file, path);
        file[strlen(path)] = '\0';
        lJNIEnv->ReleaseStringUTFChars(jpath, path);
	}
	else
	{
		strcpy(file,"");
	}

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strDirName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

bool android_IsFile(const char* path)
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(path);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "IsFile", "(Landroid/content/Context;Ljava/lang/String;)Z");
	jboolean jpath = lJNIEnv->CallStaticBooleanMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, strDirName);
	if (jpath)
	{
		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();
		return true;
	}
	else
	{
		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();
		return false;
	}
}

bool android_IsDir(const char* path)
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strDirName = lJNIEnv->NewStringUTF(path);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "IsDir", "(Landroid/content/Context;Ljava/lang/String;)Z");
	jboolean jpath = lJNIEnv->CallStaticBooleanMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, strDirName);
	if (jpath)
	{
		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();
		return true;
	}
	else
	{
		lJNIEnv->DeleteLocalRef(strClassName);
		lJNIEnv->DeleteLocalRef(strDirName);

		// Finished with the JVM.
		lJavaVM->DetachCurrentThread();
		return false;
	}
}

U32 android_GetFileSize(const char* pFilePath)
{
	// Attaches the current thread to the JVM.
	/*jint lResult;
	jint lFlags = 0;

	JavaVM* lJavaVM = engine.app->activity->vm;
	JNIEnv* lJNIEnv = engine.app->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if (lResult == JNI_ERR) {
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strFileName = lJNIEnv->NewStringUTF(pFilePath);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "GetFileSize", "(Landroid/content/Context;Ljava/lang/String;)I");
	jint jsize = lJNIEnv->CallStaticIntMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, strFileName);
	long size = jsize;
	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strFileName);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();

	return size;
	*/
	return 0;
}

ConsoleFunction(dumpFontList, void, 1, 1, "Print device fonts to console")
{
	activity.dumpFontList();
}

ConsoleFunction(hideSplashScreen, void, 1, 1, "hide the splash screen")
{
	toggleSplashScreen(false);
}

ConsoleFunction(GetAndroidResolution, const char*, 1, 1, "Returns the resolution for the android device")
{
    S32 width = _AndroidGetScreenWidth();
    S32 height = _AndroidGetScreenHeight();
	S32 bitdepth = ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;

	char buf[80];
	dSprintf(buf, sizeof(buf), "%d %d %d", width, height, bitdepth);

	char* buffer = Con::getReturnBuffer(strlen(buf) + 1);
	dStrcpy(buffer, buf);

	return buffer;
}

bool Platform::openWebBrowser(const char *webAddress)
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.T2DUtilities");
	jclass T2DUtilitiesClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strURL = lJNIEnv->NewStringUTF(webAddress);
	jmethodID MethodT2DUtilities = lJNIEnv->GetStaticMethodID(T2DUtilitiesClass, "OpenURL", "(Landroid/content/Context;Ljava/lang/String;)V");
	lJNIEnv->CallStaticVoidMethod(T2DUtilitiesClass, MethodT2DUtilities, lNativeActivity, strURL);
	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strURL);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();

	return true;
}

void android_AlertOK(const char *title, const char *message)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.T2DUtilities");
	jclass T2DUtilitiesClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strTitle = lJNIEnv->NewStringUTF(title);
	jstring strMessage = lJNIEnv->NewStringUTF(message);
	jmethodID MethodT2DUtilities = lJNIEnv->GetStaticMethodID(T2DUtilitiesClass, "DisplayAlertOK", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V");
	lJNIEnv->CallStaticVoidMethod(T2DUtilitiesClass, MethodT2DUtilities, lNativeActivity, strTitle, strMessage);
	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strTitle);
	lJNIEnv->DeleteLocalRef(strMessage);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

int android_checkAlert()
{
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
		return 0;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.T2DUtilities");
	jclass T2DUtilitiesClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodT2DUtilities = lJNIEnv->GetStaticMethodID(T2DUtilitiesClass, "CheckAlert", "()I");
	jint jret = lJNIEnv->CallStaticIntMethod(T2DUtilitiesClass, MethodT2DUtilities);

	int ret = jret;

	lJNIEnv->DeleteLocalRef(strClassName);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();

	return ret;
}

void android_AlertOKCancel(const char *title, const char *message)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.T2DUtilities");
	jclass T2DUtilitiesClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strTitle = lJNIEnv->NewStringUTF(title);
	jstring strMessage = lJNIEnv->NewStringUTF(message);
	jmethodID MethodT2DUtilities = lJNIEnv->GetStaticMethodID(T2DUtilitiesClass, "DisplayAlertOKCancel", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V");
	lJNIEnv->CallStaticVoidMethod(T2DUtilitiesClass, MethodT2DUtilities, lNativeActivity, strTitle, strMessage);

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strTitle);
	lJNIEnv->DeleteLocalRef(strMessage);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_AlertRetry(const char *title, const char *message)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.T2DUtilities");
	jclass T2DUtilitiesClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strTitle = lJNIEnv->NewStringUTF(title);
	jstring strMessage = lJNIEnv->NewStringUTF(message);
	jmethodID MethodT2DUtilities = lJNIEnv->GetStaticMethodID(T2DUtilitiesClass, "DisplayAlertRetry", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V");
	lJNIEnv->CallStaticVoidMethod(T2DUtilitiesClass, MethodT2DUtilities, lNativeActivity, strTitle, strMessage);

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strTitle);
	lJNIEnv->DeleteLocalRef(strMessage);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_AlertYesNo(const char *title, const char *message)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.T2DUtilities");
	jclass T2DUtilitiesClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strTitle = lJNIEnv->NewStringUTF(title);
	jstring strMessage = lJNIEnv->NewStringUTF(message);
	jmethodID MethodT2DUtilities = lJNIEnv->GetStaticMethodID(T2DUtilitiesClass, "DisplayAlertYesNo", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V");
	lJNIEnv->CallStaticVoidMethod(T2DUtilitiesClass, MethodT2DUtilities, lNativeActivity, strTitle, strMessage);

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strTitle);
	lJNIEnv->DeleteLocalRef(strMessage);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_LoadMusicTrack( const char *mFilename )
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.StreamingAudioPlayer");
	jclass StreamingAudioPlayerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring strFilename = lJNIEnv->NewStringUTF(mFilename);
	jmethodID MethodStreamingAudioPlayer = lJNIEnv->GetStaticMethodID(StreamingAudioPlayerClass, "LoadMusicTrack", "(Landroid/content/Context;Ljava/lang/String;)V");
	lJNIEnv->CallStaticVoidMethod(StreamingAudioPlayerClass, MethodStreamingAudioPlayer, lNativeActivity, strFilename);

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(strFilename);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_UnLoadMusicTrack()
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.StreamingAudioPlayer");
	jclass StreamingAudioPlayerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodStreamingAudioPlayer = lJNIEnv->GetStaticMethodID(StreamingAudioPlayerClass, "UnLoadMusicTrack", "()V");
	lJNIEnv->CallStaticVoidMethod(StreamingAudioPlayerClass, MethodStreamingAudioPlayer);

	lJNIEnv->DeleteLocalRef(strClassName);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

bool android_isMusicTrackPlaying()
{
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
		return false;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.StreamingAudioPlayer");
	jclass StreamingAudioPlayerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodStreamingAudioPlayer = lJNIEnv->GetStaticMethodID(StreamingAudioPlayerClass, "isMusicTrackPlaying", "()Z");
	jboolean jret = lJNIEnv->CallStaticBooleanMethod(StreamingAudioPlayerClass, MethodStreamingAudioPlayer);

	lJNIEnv->DeleteLocalRef(strClassName);

	bool ret = jret;

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();

	return ret;
}

void android_StartMusicTrack()
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.StreamingAudioPlayer");
	jclass StreamingAudioPlayerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodStreamingAudioPlayer = lJNIEnv->GetStaticMethodID(StreamingAudioPlayerClass, "startMusicTrack", "()V");
	lJNIEnv->CallStaticVoidMethod(StreamingAudioPlayerClass, MethodStreamingAudioPlayer);

	lJNIEnv->DeleteLocalRef(strClassName);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_StopMusicTrack()
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.StreamingAudioPlayer");
	jclass StreamingAudioPlayerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodStreamingAudioPlayer = lJNIEnv->GetStaticMethodID(StreamingAudioPlayerClass, "stopMusicTrack", "()V");
	lJNIEnv->CallStaticVoidMethod(StreamingAudioPlayerClass, MethodStreamingAudioPlayer);

	lJNIEnv->DeleteLocalRef(strClassName);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void android_setMusicTrackVolume(F32 volume)
{
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

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.StreamingAudioPlayer");
	jclass StreamingAudioPlayerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jmethodID MethodStreamingAudioPlayer = lJNIEnv->GetStaticMethodID(StreamingAudioPlayerClass, "setMusicTrackVolume", "(F)V");
	lJNIEnv->CallStaticVoidMethod(StreamingAudioPlayerClass, MethodStreamingAudioPlayer, (jfloat)volume);

	lJNIEnv->DeleteLocalRef(strClassName);

		// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

ConsoleFunction(doDeviceVibrate, void, 1, 1, "Makes the device do a quick vibration. Only works on devices with vibration functionality.")
{
	// Vibrate for 500 milliseconds
	long vibrateTimeMS = 500L;
	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;

	JavaVM* lJavaVM = platState.engine->app->activity->vm;
	JNIEnv* lJNIEnv = platState.engine->app->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if (lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = platState.engine->app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	// Retrieves Context.VIBRATOR_SERVICE.
	jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
	jfieldID FieldVIBRATOR_SERVICE =lJNIEnv->GetStaticFieldID(ClassContext,"VIBRATOR_SERVICE", "Ljava/lang/String;");
	jobject VIBRATOR_SERVICE = lJNIEnv->GetStaticObjectField(ClassContext, FieldVIBRATOR_SERVICE);

	// Runs getSystemService(VIBRATOR_SERVICE)
	jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(ClassNativeActivity, "getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");
	jobject lVibrator = lJNIEnv->CallObjectMethod(lNativeActivity, MethodGetSystemService, VIBRATOR_SERVICE);

	//Runs v.vibrate(ms)
	jclass ClassVibrator = lJNIEnv->FindClass("android/os/Vibrator");
	jmethodID MethodVibrate = lJNIEnv->GetMethodID(ClassVibrator, "vibrate", "(J)V");
	lJNIEnv->CallVoidMethod(lVibrator, MethodVibrate, vibrateTimeMS);

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

ConsoleFunction(enableAccelerometer, void, 1, 1, "() Allow accelerometer tracking during device motion updates")
{
	activity.enableAccelerometer(true);
}

ConsoleFunction(disableAccelerometer, void, 1, 1, "() Stop accelerometer tracking")
{
	activity.enableAccelerometer(false);
}

ConsoleFunction(isAccelerometerActive, bool, 1, 1, "() Check to see if Accelerometer is being polled\n"
                "@return True if accelerometer is on, false otherwise")
{

	return activity.isAccelerometerActive();
}

ConsoleFunction(toggleAndroidKeyboard, void, 2, 2, "(show) show or hide android virtual keyboard")
{
	displayKeyboard(dAtob(argv[1]));
}

ConsoleFunction(isAndroidKeyboardShowing, bool, 1, 1, "() returns if the keyboard is showing")
{
	return keyboardShowing;
}

void adprintf(const char* fmt,...) {

	va_list argptr;
	int cnt;
	char s[4096];
	time_t now;

	va_start(argptr,fmt);
	cnt = vsprintf(s,fmt,argptr);
	va_end(argptr);

	time(&now);
	tm* t = localtime(&now);

	std::stringstream ss;
	ss.clear();
	ss << "[";
	ss << (t->tm_year + 1900);
	ss << "/";
	if (t->tm_mon < 9)
		ss << "0";
	ss << (t->tm_mon+1);
	ss << "/";
	if (t->tm_mday < 10)
		ss << "0";
	ss << t->tm_mday;
	ss << " ";
	if (t->tm_hour < 10)
		ss << "0";
	ss << t->tm_hour;
	ss << ":";
	if (t->tm_min < 10)
		ss << "0";
	ss << t->tm_min;
	ss << ":";
	if (t->tm_sec < 10)
		ss << "0";
	ss << t->tm_sec;
	ss << "] ";
	ss << s;

    __android_log_print(ANDROID_LOG_INFO, "Torque2D", "%s", ss.str().c_str());
}

void T2DActivity::loadInternalDir(){

    // Attaches the current thread to the JVM.
    jint lResult;
    jint lFlags = 0;
    JavaVM* lJavaVM = platState.engine->app->activity->vm;
    JNIEnv* lJNIEnv = platState.engine->app->activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
    if (lResult == JNI_ERR) {
        return;
    }

    // Retrieves NativeActivity.
    jobject lNativeActivity = platState.engine->app->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

    jmethodID getFilesDir = lJNIEnv->GetMethodID(ClassNativeActivity, "getFilesDir", "()Ljava/io/File;");
    jobject file = lJNIEnv->CallObjectMethod(platState.engine->app->activity->clazz, getFilesDir);
    jclass fileClass = lJNIEnv->FindClass("java/io/File");
    jmethodID getAbsolutePath = lJNIEnv->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring jpath = (jstring)lJNIEnv->CallObjectMethod(file, getAbsolutePath);
    const char* app_dir = lJNIEnv->GetStringUTFChars(jpath, NULL);

    strcpy(internalDir, app_dir);
    internalDir[strlen(app_dir)] = '\0';

    lJNIEnv->ReleaseStringUTFChars(jpath, app_dir);

    // Finished with the JVM.
    lJavaVM->DetachCurrentThread();
}


char* _AndroidLoadInternalFile(const char* fn, U32 *size)
{
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
		return "";
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = engine.app->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jmethodID getClassLoader = lJNIEnv->GetMethodID(ClassNativeActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = lJNIEnv->NewStringUTF("com.garagegames.torque2d.FileWalker");
	jclass FileWalkerClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
	jstring fileName = lJNIEnv->NewStringUTF(fn);
	jmethodID MethodFileWalker = lJNIEnv->GetStaticMethodID(FileWalkerClass, "LoadInternalFile", "(Landroid/content/Context;Ljava/lang/String;)Ljava/lang/String;");
	jstring jcontent = (jstring)lJNIEnv->CallStaticObjectMethod(FileWalkerClass, MethodFileWalker, lNativeActivity, fileName);

	lJNIEnv->DeleteLocalRef(strClassName);
	lJNIEnv->DeleteLocalRef(fileName);
	*size = 0;
	char *buffer = NULL;
	if (jcontent != NULL)
	{
		const char* value = lJNIEnv->GetStringUTFChars(jcontent, NULL);
		*size = strlen(value);
		buffer = new char[*size + 1];
		strcpy(buffer, value);
		buffer[*size] = '\0';

		lJNIEnv->ReleaseStringUTFChars(jcontent, value);
		lJavaVM->DetachCurrentThread();
		return buffer;
	}

	lJavaVM->DetachCurrentThread();
	return buffer;
}


