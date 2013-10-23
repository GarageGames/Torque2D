package com.garagegames.torque2d;

import android.app.NativeActivity;
import android.util.Log;

public class MyNativeActivity extends NativeActivity {
  static {
    System.loadLibrary("openal"); 
  }
}
