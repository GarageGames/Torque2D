package com.garagegames.torque2d;

import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;

public class SplashScreen extends Dialog {
	
	public SplashScreen(Context context, int theme) {
		super(context, theme);
	}

	private static SplashScreen _instance = null;
	
	public static void ToggleSplashScreen(final Context context, final boolean show, final int screenWidth, final int screenHeight) 
	{
		if (show)
		{
			final Activity activity = (Activity)context;
			activity.runOnUiThread(new Runnable() {			
				@Override
					public void run() {
						
						_instance = new SplashScreen(context, android.R.style.Theme_Black_NoTitleBar_Fullscreen);
						
						try {
							AssetManager assetMgr = context.getAssets();
							InputStream stream = assetMgr.open("splash.png");
							Bitmap splashBitmap = Bitmap.createScaledBitmap(BitmapFactory.decodeStream(stream), screenWidth, screenHeight, true);
							
							ImageView view = new ImageView(context);
							view.setPadding(0, 0, 0, 0);
							view.setImageBitmap(splashBitmap);
							view.setScaleType(ScaleType.FIT_CENTER);
							view.setBackgroundColor(Color.BLACK);
							_instance.getWindow().setFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM,
							         WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
							_instance.requestWindowFeature(Window.FEATURE_NO_TITLE); 
							_instance.setCancelable(false);
							_instance.setContentView(view);
							_instance.show();
							
						} catch (IOException e) {
							
						}
					}
			});
		} 
		else
		{
			if (_instance != null)
			{
				_instance.dismiss();
				_instance = null;
			}
		}
	}
}
