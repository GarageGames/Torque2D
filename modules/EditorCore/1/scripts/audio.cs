//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// initializeOpenAL
// Starts up the OpenAL driver.
//---------------------------------------------------------------------------------------------
function initializeOpenAL()
{
   echoSeparator();
   echo("Audio Initialization:");
   
   // Just in case it is already started.
   shutdownOpenAL();

   if($pref::Audio::driver $= "OpenAL")
   {
      if(!OpenALInitDriver())
      {
         error("   Failed to initialize driver.");
         $Audio::initFailed = true;
      }
      else
      {
         // Print out driver info.
         echo("   Vendor: " @ alGetString("AL_VENDOR"));
         echo("   Version: " @ alGetString("AL_VERSION"));  
         echo("   Renderer: " @ alGetString("AL_RENDERER"));
         echo("   Extensions: " @ alGetString("AL_EXTENSIONS"));
         
         // Set the master volume.
         alxListenerf(AL_GAIN_LINEAR, $pref::Audio::masterVolume);
         
         // Set the channel volumes.
         for (%channel = 1; %channel <= 8; %channel++)
            alxSetChannelVolume(%channel, $pref::Audio::channelVolume[%channel]);
         
         echo("");
      }
   }
   else
      error("   Failed to initialize audio system. Invalid driver.");
}

//---------------------------------------------------------------------------------------------
// shutdownOpenAL
//---------------------------------------------------------------------------------------------
function shutdownOpenAL()
{
   OpenALShutdownDriver();
}
