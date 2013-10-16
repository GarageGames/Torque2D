package com.garagegames.torque2d;

import java.io.IOException;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaPlayer;
import android.util.Log;

public class StreamingAudioPlayer {
	
	private static MediaPlayer mediaPlayer = null;

	public static void LoadMusicTrack(Context context, String filename)
	{
		mediaPlayer = new MediaPlayer();
		AssetFileDescriptor fd;
		try {
			fd = context.getAssets().openFd(filename);
			if (fd == null)
				Log.i("torque2d", "failed to load music file " + filename);
			
			mediaPlayer.setDataSource(fd.getFileDescriptor(),fd.getStartOffset(),fd.getLength());
			mediaPlayer.prepare();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	public static void UnLoadMusicTrack()
	{
		if (mediaPlayer == null)
			return;
		
		mediaPlayer.stop();
		mediaPlayer.release();
		mediaPlayer = null;
		
	}
	
	public static boolean isMusicTrackPlaying()
	{
		if (mediaPlayer == null)
			return false;
		
		return mediaPlayer.isPlaying();
	}
	
	public static void startMusicTrack()
	{
		if (mediaPlayer == null)
			return;
		
		mediaPlayer.start();
	}
	
	public static void stopMusicTrack()
	{
		if (mediaPlayer == null)
			return;
		
		mediaPlayer.stop();
	}
	
	public static void setMusicTrackVolume(float volume)
	{
		if (mediaPlayer == null)
			return;
		
		mediaPlayer.setVolume(volume, volume);
	}
	
}
