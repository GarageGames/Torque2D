package com.garagegames.torque2d;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

public class T2DUtilities {
		
	public static void OpenURL(Context context, String url)
	{
		Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		context.startActivity(browserIntent);
	}
	
	public static void DisplayAlertOK(final Context context, final String title, final String message)
	{
		Log.i("torque2d", "DisplayAlertOK");
		final Activity activity = (Activity)context;
		activity.runOnUiThread(new Runnable() {			
			@Override
				public void run() {
					AlertDialog.Builder builder = new AlertDialog.Builder(context);
					builder.setMessage(message);
					builder.setTitle(title);
					builder.setCancelable(false);
					builder.setNeutralButton("OK", new DialogInterface.OnClickListener() {
					           public void onClick(DialogInterface dialog, int id) {
					                
					           }
					       });
					AlertDialog alert = builder.create();
					alert.show();
				}
		});
	}
	
	private static boolean retValue = false;
	
	public static void DisplayAlertOKCancel(final Context context, final String title, final String message)
	{
		Log.i("torque2d", "DisplayAlertOKCancel");
		final Activity activity = (Activity)context;
		activity.runOnUiThread(new Runnable() {			
			@Override
				public void run() {
					AlertDialog.Builder builder = new AlertDialog.Builder(context);
					builder.setMessage(message);
					builder.setTitle(title);
					builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = true;
				           }
				       });
					builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = false;
				           }
				       });
					AlertDialog alert = builder.create();
					alert.show();
				}
		});
	}
	
	public static boolean DisplayAlertRetry(Context context, String title, String message)
	{
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		builder.setMessage(message);
		builder.setTitle(title);
		builder.setPositiveButton("Retry", new DialogInterface.OnClickListener() {
	           public void onClick(DialogInterface dialog, int id) {
	                retValue = true;
	           }
	       });
		builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
	           public void onClick(DialogInterface dialog, int id) {
	                retValue = false;
	           }
	       });
		AlertDialog alert = builder.create();
		alert.show();
		
		while(alert.isShowing())
		{
			try {
				Thread.sleep(64);
			} catch (InterruptedException e) {
			}
		}
		
		return retValue;
	}
	
	public static boolean DisplayAlertYesNo(Context context, String title, String message)
	{
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		builder.setMessage(message);
		builder.setTitle(title);
		builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
	           public void onClick(DialogInterface dialog, int id) {
	                retValue = true;;
	           }
	       });
		builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
	           public void onClick(DialogInterface dialog, int id) {
	                retValue = false;
	           }
	       });
		AlertDialog alert = builder.create();
		alert.show();
		
		while(alert.isShowing())
		{
			try {
				Thread.sleep(64);
			} catch (InterruptedException e) {
			}
		}
		
		return retValue;
	}
}
