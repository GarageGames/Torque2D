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

function fpsMetricsCallback()
{
   return " FPS: " @ $fps::real @ 
          "  mspf: " @ 1000 / $fps::real;
}

function videoMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Video -" @
          "  TC: " @ $OpenGL::triCount0 + $OpenGL::triCount1 + $OpenGL::triCount2 + $OpenGL::triCount3 @ 
          "  PC: " @ $OpenGL::primCount0 + $OpenGL::primCount1 + $OpenGL::primCount2 + $OpenGL::primCount3 @ 
          "  T_T: " @ $OpenGL::triCount1 @ 
          "  T_P: " @ $OpenGL::primCount1 @ 
          "  I_T: " @ $OpenGL::triCount2 @ 
          "  I_P: " @ $OpenGL::primCount2 @ 
          "  TS_T: " @ $OpenGL::triCount3 @ 
          "  TS_P: " @ $OpenGL::primCount3 @ 
          "  ?_T: " @ $OpenGL::triCount0 @ 
          "  ?_P: " @ $OpenGL::primCount0;
}

function textureMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Texture --"@
          "  NTL: " @ $Video::numTexelsLoaded @
          "  TRP: " @ $Video::texResidentPercentage @
          "  TCM: " @ $Video::textureCacheMisses;
}

function timeMetricsCallback()
{
   return fpsMetricsCallback() @ 
         "  Time -- " @ 
         "  Sim Time: " @ getSimTime() @ 
         "  Mod: " @ getSimTime() % 32;
}

function audioMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Audio --"@
          " OH:  " @ $Audio::numOpenHandles @
          " OLH: " @ $Audio::numOpenLoopingHandles @
          " AS:  " @ $Audio::numActiveStreams @
          " NAS: " @ $Audio::numNullActiveStreams @
          " LAS: " @ $Audio::numActiveLoopingStreams @
          " LS:  " @ $Audio::numLoopingStreams @
          " ILS: " @ $Audio::numInactiveLoopingStreams @
          " CLS: " @ $Audio::numCulledLoopingStreams;
}

function debugMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Debug --"@
          "  NTL: " @ $Video::numTexelsLoaded @
          "  TRP: " @ $Video::texResidentPercentage @
          "  NP:  " @ $Metrics::numPrimitives @
          "  NT:  " @ $Metrics::numTexturesUsed @
          "  NO:  " @ $Metrics::numObjectsRendered;
}

function metrics(%expr)
{
   switch$(%expr)
   {
      case "audio":     %cb = "audioMetricsCallback()";
      case "debug":     %cb = "debugMetricsCallback()";
      case "fps":       %cb = "fpsMetricsCallback()";
      case "time":      %cb = "timeMetricsCallback()";
      case "texture":   
         GLEnableMetrics(true);
         %cb = "textureMetricsCallback()";

      case "video":     %cb = "videoMetricsCallback()";
   }
   
   if (%cb !$= "")
   {
      Canvas.pushDialog(FrameOverlayGui, 1000);
      TextOverlayControl.setValue(%cb);
   }
   else
   {
      GLEnableMetrics(false);
      Canvas.popDialog(FrameOverlayGui);
   }
}
