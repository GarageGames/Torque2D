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
#ifndef T2DACTIVITY_H
#define T2DACTIVITY_H
#include "platformAndroid/AndroidGL2ES.h"
#include "graphics/DynamicTexture.h"
#include <errno.h>
#include <EGL/egl.h>
//#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/sensor.h>
#include <android/configuration.h>
#include <sstream>
#include <list>
#include <unistd.h>
#include <time.h>

extern void adprintf(const char* fmt,...);
extern int _AndroidGetScreenWidth();
extern int _AndroidGetScreenHeight();
extern S32 _AndroidGameGetOrientation();
extern char* _AndroidLoadFile(const char* fileName, U32 *size);
extern char* _AndroidLoadInternalFile(const char* fileName, U32 *size);
extern void android_InitDirList(const char* dir);
extern void android_GetNextDir(const char* pdir, char *dir);
extern void android_GetNextFile(const char* pdir, char *file);
extern bool android_IsDir(const char* path);
extern bool android_IsFile(const char* path);
extern U32 android_GetFileSize(const char* pFilePath);
extern bool android_DumpPath(const char* dir, Vector<Platform::FileInfo>& fileVector, U32 depth);
extern bool android_DumpDirectories(const char *basePath, const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath);
extern void toggleSplashScreen(bool show);

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
};

class T2DActivity {

private:
	// The pixel dimensions of the backbuffer
	S32 backingWidth;
	S32 backingHeight;
    
    GLuint _vertexBuffer;
    GLuint _indexBuffer;
    GLuint _vertexArray;
    
    bool isLayedOut;
    
    char cacheDir[2048];
    char internalDir[2048];

    bool accelerometerActive;

public:

    T2DActivity() {

    	accelerometerActive = false;
    }

    void enableAccelerometer(bool enable);
    bool isAccelerometerActive() {

    	return accelerometerActive;
    }

    void loadCacheDir();
    char *getCacheDir() {
    	return cacheDir;
    }
    void loadInternalDir();
    char *getInternalDir() {
    	return internalDir;
    }

    void update();
    void finishShutdown();
    void finishGLSetup();
    void enumerateFonts();
    void dumpFontList();
    void getFontPath(const char* fontName, char* fontPath);
};

extern T2DActivity activity;
#endif
