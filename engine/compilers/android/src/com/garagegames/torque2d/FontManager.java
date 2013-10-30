package com.garagegames.torque2d;
/*
 * Copyright (C) 2011 George Yunaev @ Ulduzsoft
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */
 
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.HashMap;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
 
public class FontManager
{
	static private HashMap< String, String > fonts = null;
    // This function enumerates all fonts on Android system and returns the HashMap with the font
    // absolute file name as key, and the font literal name (embedded into the font) as value.
    static public void enumerateFonts(Context context)
    {
        String[] fontdirs = { "/system/fonts", "/system/font", "/data/fonts" };
        fonts = new HashMap< String, String >();
        TTFAnalyzer analyzer = new TTFAnalyzer();
 
        for ( String fontdir : fontdirs )
        {
            File dir = new File( fontdir );
 
            if ( !dir.exists() )
                continue;
 
            File[] files = dir.listFiles();
 
            if ( files == null )
                continue;
 
            for ( File file : files )
            {
                String fontname = analyzer.getTtfFontName( file.getAbsolutePath() );
 
                if ( fontname != null )
                    fonts.put( file.getAbsolutePath(), fontname );
            }
            
        }
        
        AssetManager assetMgr = context.getAssets();
        String[] customFonts;
        try {
			customFonts = assetMgr.list("fonts");
			for (String customFont : customFonts)
	        {
				if (customFont.endsWith(".ttf"))
				{
					String customFontPath = copyCustomFontToCache(customFont, assetMgr, context);
					String fontname = analyzer.getTtfFontName( customFontPath );
					 
	                if ( fontname != null )
	                    fonts.put( customFontPath, fontname );
				}
	        }
			
			
		} catch (IOException e) {
			
		}
        
    }
    
    static public String copyCustomFontToCache(String customFont, AssetManager assetMgr, Context context)
    {
    	InputStream fis = null;
    	File tmpFile = null;
    	String outFilePath = "";
    	try {
    	    fis = assetMgr.open("fonts/" + customFont);
    	    // We'll create a file in the application's cache directory
    	    File dir = context.getCacheDir();
    	    dir.mkdirs();
    	    tmpFile = new File(dir, customFont);
    	    if (tmpFile.exists()) {
    	        // Delete the temporary file if it already exists
    	        tmpFile.delete();
    	    }
    	    FileOutputStream fos = null;
    	    try {
    	        // Write the asset file to the temporary location
    	        fos = new FileOutputStream(tmpFile);
    	        byte[] buffer = new byte[1024];
    	        int bufferLen;
    	        while ((bufferLen = fis.read(buffer)) != -1) {
    	            fos.write(buffer, 0, bufferLen);
    	        }
    	    } finally {
    	        if (fos != null) {
    	            try {
    	                fos.close();
    	            } catch (IOException e) {}
    	        }
    	    }
    	    
    	    outFilePath = dir + "/" + customFont;
    	    
    	} catch (IOException e) {
    	    Log.e("torque2d", "Failed reading asset", e);
    	} finally {
    	    if (fis != null) {
    	        try {
    	            fis.close();
    	        } catch (IOException e) {}
    	    }
    	    
    	}
    	
    	return outFilePath;
    }
    
    static public void dumpFontList()
    {
    	//print list
        for (String key : fonts.keySet())
        {
        	Log.i("torque2d", "font: " + fonts.get(key));
        }
    }
    
    static public String getFont(String fontName)
    {
    	for (String key : fonts.keySet())
    	{
    		String data = fonts.get(key);
    		if (!fontName.contains("Italic") && !fontName.contains("Bold"))
    		{
    			if (data.contains(fontName) && !data.contains("Italic") && !data.contains("Bold"))
    				return key;
    		} else {
    			if (data.contains(fontName))
    				return key;
    		}
    	}
    	
    	return null;
    }
}
 
// The class which loads the TTF file, parses it and returns the TTF font name
class TTFAnalyzer
{
    // This function parses the TTF file and returns the font name specified in the file
    public String getTtfFontName( String fontFilename )
    {
        try
        {
            // Parses the TTF file format.
            // See http://developer.apple.com/fonts/ttrefman/rm06/Chap6.html
            m_file = new RandomAccessFile( fontFilename, "r" );
 
            // Read the version first
            int version = readDword();
 
            // The version must be either 'true' (0x74727565) or 0x00010000
            if ( version != 0x74727565 && version != 0x00010000 )
                return null;
 
            // The TTF file consist of several sections called "tables", and we need to know how many of them are there.
            int numTables = readWord();
 
            // Skip the rest in the header
            readWord(); // skip searchRange
            readWord(); // skip entrySelector
            readWord(); // skip rangeShift
 
            // Now we can read the tables
            for ( int i = 0; i < numTables; i++ )
            {
                // Read the table entry
                int tag = readDword();
                readDword(); // skip checksum
                int offset = readDword();
                int length = readDword();
 
                // Now here' the trick. 'name' field actually contains the textual string name.
                // So the 'name' string in characters equals to 0x6E616D65
                if ( tag == 0x6E616D65 )
                {
                    // Here's the name section. Read it completely into the allocated buffer
                    byte[] table = new byte[ length ];
 
                    m_file.seek( offset );
                    read( table );
 
                    // This is also a table. See http://developer.apple.com/fonts/ttrefman/rm06/Chap6name.html
                    // According to Table 36, the total number of table records is stored in the second word, at the offset 2.
                    // Getting the count and string offset - remembering it's big endian.
                    int count = getWord( table, 2 );
                    int string_offset = getWord( table, 4 );
 
                    // Record starts from offset 6
                    for ( int record = 0; record < count; record++ )
                    {
                        // Table 37 tells us that each record is 6 words -> 12 bytes, and that the nameID is 4th word so its offset is 6.
                        // We also need to account for the first 6 bytes of the header above (Table 36), so...
                        int nameid_offset = record * 12 + 6;
                        int platformID = getWord( table, nameid_offset );
                        int nameid_value = getWord( table, nameid_offset + 6 );
 
                        // Table 42 lists the valid name Identifiers. We're interested in 4 but not in Unicode encoding (for simplicity).
                        // The encoding is stored as PlatformID and we're interested in Mac encoding
                        if ( nameid_value == 4 && platformID == 1 )
                        {
                            // We need the string offset and length, which are the word 6 and 5 respectively
                            int name_length = getWord( table, nameid_offset + 8 );
                            int name_offset = getWord( table, nameid_offset + 10 );
 
                            // The real name string offset is calculated by adding the string_offset
                            name_offset = name_offset + string_offset;
 
                            // Make sure it is inside the array
                            if ( name_offset >= 0 && name_offset + name_length < table.length )
                                return new String( table, name_offset, name_length );
                        }
                    }
                }
            }
 
            return null;
        }
        catch (FileNotFoundException e)
        {
            // Permissions?
            return null;
        }
        catch (IOException e)
        {
            // Most likely a corrupted font file
            return null;
        }
    }
 
    // Font file; must be seekable
    private RandomAccessFile m_file = null;
 
    // Helper I/O functions
    private int readByte() throws IOException
    {
        return m_file.read() & 0xFF;
    }
 
    private int readWord() throws IOException
    {
        int b1 = readByte();
        int b2 = readByte();
 
        return b1 << 8 | b2;
    }
 
    private int readDword() throws IOException
    {
        int b1 = readByte();
        int b2 = readByte();
        int b3 = readByte();
        int b4 = readByte();
 
        return b1 << 24 | b2 << 16 | b3 << 8 | b4;
    }
 
    private void read( byte [] array ) throws IOException
    {
        if ( m_file.read( array ) != array.length )
            throw new IOException();
    }
 
    // Helper
    private int getWord( byte [] array, int offset )
    {
        int b1 = array[ offset ] & 0xFF;
        int b2 = array[ offset + 1 ] & 0xFF;
 
        return b1 << 8 | b2;
    }
}