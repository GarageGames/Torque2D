package com.garagegames.torque2d;

import java.io.IOException;
import java.io.InputStream;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import android.content.Context;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.net.Uri;
import android.util.Log;

public class FileWalker
{
	public static Hashtable<String,Vector<String>> directories = new Hashtable<String,Vector<String>>();
	public static Hashtable<String,Vector<String>> files = new Hashtable<String,Vector<String>>();
	public static Vector<String> dumpPathVec = new Vector<String>();
	public static Vector<String> dumpDirVec = new Vector<String>();
	
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
	
	public static String[] DumpDirectories(Context context, String basePath, String path, boolean depth, boolean noBasePath)
	{

	    dumpPathVec.clear();
		dumpDirVec.clear();
		
		String dirPath = basePath;
	    
		if (dirPath.startsWith("/"))
	    	dirPath = dirPath.substring(1);
		
		if (dirPath.endsWith("/"))
	    	dirPath = dirPath.substring(0,dirPath.length()-1);
	   
	    if ( !noBasePath )
	        dumpPathVec.add(dirPath);
	   
	    if (!path.equals(""))
	    {
		    dirPath = basePath + "/" + path; 
		    
		    if (dirPath.endsWith("/"))
		    	dirPath = dirPath.substring(0,dirPath.length()-1);
		   
	    }
	   
		//Log.i("torque2d", "Dump first dir: " + dirPath);
		DumpDir2(context, dirPath);
		
		while (depth && dumpDirVec.size() > 0)
		{
			String newdir = dumpDirVec.remove(0);
			//Log.i("torque2d", "Dump dir again: " + newdir);
			DumpDir2(context,newdir);
		}
		
		int size = dumpPathVec.size();
		if (size > 500)
			size = 500;
		String[] retStringArray = new String[size];
		int cnt = 0;
		for(cnt = 0; cnt < size; cnt++)
		{
			String s = dumpPathVec.remove(0);
			if (noBasePath)
				s = s.replace(basePath + "/", "");
			retStringArray[cnt] = "/" + s;
		}
		return retStringArray;
	}
	
	public static void DumpDir2(Context context, String dir)
	{
		AssetManager assetMgr = context.getAssets();
		try {
			String[] assets = assetMgr.list(dir);
			for (String asset : assets)
			{
				if (asset.equals(".") || asset.equals(".."))
					continue;
				
				if (!asset.contains("."))
				{
					if (dir.equals(""))
						dumpPathVec.add(asset);
					else
						dumpPathVec.add(dir + "/" + asset);
					
					String newdir = asset;
					if (!dir.equals(""))
						newdir = dir + "/" + asset;
					
					dumpDirVec.add(newdir);
				}
			}
				
		} catch (IOException e) {
			
		}
	}
	
	public static String[] DumpPath(Context context, String dirPath, boolean depth)
	{
		dumpPathVec.clear();
		dumpDirVec.clear();
		
		String dir = dirPath;
		
		if (dir.startsWith("/"))
	    	dir = dir.substring(1);
		
		if (dir.endsWith("/"))
	    	dir = dir.substring(0,dir.length()-1);
	   
		//Log.i("torque2d", "Dump first dir: " + dir);
		DumpDir(context, dir);
		
		while (depth && dumpDirVec.size() > 0)
		{
			String newdir = dumpDirVec.remove(0);
			//Log.i("torque2d", "Dump dir again: " + newdir);
			DumpDir(context,newdir);
		}
		
		int size = dumpPathVec.size();
		if (size > 500)
			size = 500;
		String[] retStringArray = new String[size];
		int cnt = 0;
		for(cnt = 0; cnt < size; cnt++)
		{
			String s = dumpPathVec.remove(0);
			retStringArray[cnt] = "/" + s;
		}
		return retStringArray;
	}
	
	public static String[] getRestOfDump()
	{
		int size = dumpPathVec.size();
		if (size > 500)
			size = 500;
		String[] retStringArray = new String[size];
		int cnt = 0;
		for(cnt = 0; cnt < size; cnt++)
		{
			String s = dumpPathVec.remove(0);
			retStringArray[cnt] = "/" + s;
		}
		return retStringArray;
	}
	
	public static void DumpDir(Context context, String dir)
	{
		AssetManager assetMgr = context.getAssets();
		try {
			String[] assets = assetMgr.list(dir);
			for (String asset : assets)
			{
				if (asset.equals(".") || asset.equals(".."))
					continue;
				
				if (asset.contains("."))
				{
					if (dir.equals(""))
						dumpPathVec.add(asset);
					else
						dumpPathVec.add(dir + "/" + asset);
				}
				else
				{
					String newdir = asset;
					if (!dir.equals(""))
						newdir = dir + "/" + asset;
					
					dumpDirVec.add(newdir);
						
				}
			}
				
		} catch (IOException e) {
			
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
	
	public static int GetFileSize(Context context, String file)
	{
		int ret = 0;
		try {
			if (file.startsWith("/"))
				file = file.substring(1);
			
			AssetFileDescriptor afd = context.getAssets().openFd(file);
			ret = (int)afd.getLength();
			
			afd.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return ret;
	}
	
	public static void OpenURL(Context context, String url)
	{
		Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		context.startActivity(browserIntent);
	}
}
