package com.garagegames.torque2d;

import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

public class FileWalker
{
	public static Hashtable<String,Vector<String>> directories = new Hashtable<String,Vector<String>>();
	public static Hashtable<String,Vector<String>> files = new Hashtable<String,Vector<String>>();
	
	public static void InitDirList(Context context, String dir)
	{
		AssetManager assetMgr = context.getAssets();
		try {
			String[] assets = assetMgr.list(dir);
			if (directories.containsKey(dir))
				directories.remove(dir);
			directories.put(dir, new Vector<String>());
			if (files.containsKey(dir))
				files.remove(dir);
			files.put(dir, new Vector<String>());
			//Log.i("torque2d", "PARENT DIRECTORY: " + dir);
			for(String asset: assets) {
				if (asset.indexOf('.') == -1)
				{
					directories.get(dir).add(asset);
					//Log.i("torque2d", "DIRECTORY ASSET: " + asset);
				}
				else
				{
					files.get(dir).add(asset);
					//Log.i("torque2d", "FILE ASSET: " + asset);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static String GetNextDir(String dir)
	{
		if (directories.get(dir).size() == 0)
			return null;
		 
		return directories.get(dir).remove(0);
		
	}
	
	public static String GetNextFile(String dir)
	{
		if (files.get(dir).size() == 0)
			return null;
		 
		return files.get(dir).remove(0);
	}
	
	public static boolean IsDir(Context context, String dir)
	{
		AssetManager assetMgr = context.getAssets();
		String[] assets;
		try {
			assets = assetMgr.list(dir);
			if (assets != null && assets.length > 0) 
			{
				return true;
			}
			
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
	}
	
	public static boolean IsFile(Context context, String dir)
	{
		AssetManager assetMgr = context.getAssets();
		String[] assets;
		try {
			assets = assetMgr.list(dir);
			if (assets != null && assets.length > 0) 
			{
				return false;
			}
			
			return true;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
	}
}
