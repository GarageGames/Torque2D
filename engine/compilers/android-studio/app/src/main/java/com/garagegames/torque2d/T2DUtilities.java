package com.garagegames.torque2d;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;

public class T2DUtilities {
		
	public static void OpenURL(Context context, String url)
	{
		Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		context.startActivity(browserIntent);
	}
	
	public static void DisplayAlertOK(final Context context, final String title, final String message)
	{
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
					        	   alert = null; 
					           }
					       });
					AlertDialog alert = builder.create();
					alert.show();
				}
		});
	}
	
	private static int retValue = -1;
	private static AlertDialog alert = null;
	
	public static void DisplayAlertOKCancel(final Context context, final String title, final String message)
	{
		retValue = -1;
		final Activity activity = (Activity)context;
		activity.runOnUiThread(new Runnable() {			
			@Override
				public void run() {
					AlertDialog.Builder builder = new AlertDialog.Builder(context);
					builder.setMessage(message);
					builder.setTitle(title);
					builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = 1;
				                alert = null;
				           }
				       });
					builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = 0;
				                alert = null;
				           }
				       });
					alert = builder.create();
					alert.show();
				}
		});
	}
	
	public static int CheckAlert()
	{
		if (alert == null && retValue == -1)
			return -1;
		
		if (retValue == -1)
			return -1;
		
		if (alert != null && alert.isShowing())
			return -1;
		
		return retValue;
	}
	
	public static void DisplayAlertRetry(final Context context, final String title, final String message)
	{
		retValue = -1;
		final Activity activity = (Activity)context;
		activity.runOnUiThread(new Runnable() {			
			@Override
				public void run() {
					AlertDialog.Builder builder = new AlertDialog.Builder(context);
					builder.setMessage(message);
					builder.setTitle(title);
					builder.setPositiveButton("Retry", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = 1;
				                alert = null;
				           }
				       });
					builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = 0;
				                alert = null;
				           }
				       });
					AlertDialog alert = builder.create();
					alert.show();
				}
		});
	}
	
	public static void DisplayAlertYesNo(final Context context, final String title, final String message)
	{
		retValue = -1;
		final Activity activity = (Activity)context;
		activity.runOnUiThread(new Runnable() {			
			@Override
				public void run() {
					AlertDialog.Builder builder = new AlertDialog.Builder(context);
					builder.setMessage(message);
					builder.setTitle(title);
					builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = 1;
				                alert = null;
				           }
				       });
					builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
				           public void onClick(DialogInterface dialog, int id) {
				                retValue = 0;
				                alert = null;
				           }
				       });
					AlertDialog alert = builder.create();
					alert.show();
				}
		});
	}
}
