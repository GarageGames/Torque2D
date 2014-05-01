package com.garagegames.torque2d;

import android.app.NativeActivity;

public class MyNativeActivity extends NativeActivity {
  static {
    System.loadLibrary("openal"); 
  }
}
