package com.garagegames.torque2d;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Hashtable;
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
	public static Vector<String> dumpDirList = new Vector<String>();
	
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
			
			for(String asset: assets) {
				if (asset.indexOf('.') == -1)
				{
					directories.get(dir).add(asset);
				}
				else
				{
					files.get(dir).add(asset);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static String[] DumpDirectories(Context context, String basePath, String path, boolean depth, boolean noBasePath)
	{
		double time = System.currentTimeMillis();
	    dumpPathVec.clear();
		dumpDirVec.clear();
		
		String dirPath = basePath;
		
		//remove any ./ from path since the apk code chokes on it
		while (dirPath.contains("./"))
			dirPath = dirPath.replace("./", "");
		
		//remove any ../ from path since the apk code chokes on it
		String search = "/../";
		while (dirPath.contains(search))
		{
			int pos = dirPath.indexOf(search);
			int posStart = dirPath.lastIndexOf("/", pos-1);
			dirPath = dirPath.substring(0, posStart+1) + dirPath.substring(pos+4);
		}
	    
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
		DumpDir2(context, dirPath);
		
		while (depth && dumpDirVec.size() > 0)
		{
			String newdir = dumpDirVec.remove(0);
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
		Log.i("torque2d", "time in dir java: " + (System.currentTimeMillis() - time) );
		
		return retStringArray;
	}
	
	public static void DumpDir2(Context context, String dir)
	{
		for (String d : dumpDirList)
		{
			String d2 = d.substring(1);
			if (d2.equals(dir))
				continue;
			
			if (d2.length() < dir.length())
				continue;
			
			String newdir = "";
			if (dir.equals(""))
			{
				if (!d2.contains("/"))
				{
					dumpPathVec.add(d2);
					newdir = d2;
				}
			}
			else
			{
				if (d2.startsWith(dir))
				{
					String asset = d2.replace(dir, "").substring(1);
					if (!asset.contains("/"))
					{
						dumpPathVec.add(d2);
						newdir = d2;
					}
				}
			}
			
			
			if (!newdir.equals("")) {
				dumpDirVec.add(newdir);
			}
		}
	}
	
	public static String[] DumpPath(Context context, String dirPath, boolean depth)
	{
		double time = System.currentTimeMillis();
		dumpPathVec.clear();
		dumpDirVec.clear();
		dumpDirList.clear();
		
		String dir = dirPath;
		
		//remove any ./ from path since the apk code chokes on it
		while (dir.contains("./"))
			dir = dir.replace("./", "");
		
		//remove any ../ from path since the apk code chokes on it
		String search = "/../";
		while (dir.contains(search))
		{
			int pos = dir.indexOf(search);
			int posStart = dir.lastIndexOf("/", pos-1);
			dir = dir.substring(0, posStart+1) + dir.substring(pos+4);
		}
		
		if (dir.startsWith("/"))
	    	dir = dir.substring(1);
		
		if (dir.endsWith("/"))
	    	dir = dir.substring(0,dir.length()-1);
	   
		//Log.i("torque2d","path start: " + dir);
		DumpDir(context, dir);
		
		while (depth && dumpDirVec.size() > 0)
		{
			String newdir = dumpDirVec.remove(0);
			//Log.i("torque2d","newpath " + newdir);
			DumpDir(context,newdir);
		}
		
		int size = dumpPathVec.size();
		if (size > 500)
			size = 500;
		//Log.i("torque2d","size " + size);
		String[] retStringArray = new String[size];
		int cnt = 0;
		for(cnt = 0; cnt < size; cnt++)
		{
			String s = dumpPathVec.remove(0);
			//Log.i("torque2d","file " + s);
			retStringArray[cnt] = "/" + s;
		}
		Log.i("torque2d", "time in java: " + (System.currentTimeMillis() - time) );
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
				
				if (dir.equals(""))
				{
					if (asset.equals("images") || asset.equals("webkit") || asset.equals("sounds") || asset.equals("kioskmode"))
						continue;
				}
				
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
					if (newdir.startsWith("/"))
						dumpDirList.add(newdir);
					else
						dumpDirList.add("/" + newdir);
						
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
	
	public static String LoadInternalFile(Context context, String fileName)
	{
		try {
			FileInputStream fis = new FileInputStream(new File(fileName));
			DataInputStream in = new DataInputStream(fis);
			BufferedReader inputReader = new BufferedReader(new InputStreamReader(in));
			String inputString;
			StringBuilder sb = new StringBuilder();
			while ((inputString = inputReader.readLine()) != null) {
				sb.append(inputString);
			}
			fis.close();
			inputReader.close();
			in.close();

			return sb.toString();

		} catch (IOException e) {
			e.printStackTrace();
			return "";
		}
	}

	public static void OpenURL(Context context, String url)
	{
		Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		context.startActivity(browserIntent);
	}
}
