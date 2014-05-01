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

/*! @defgroup TextureManagerFunctions Texture Manager
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the setOpenGLTextureCompressionHint function to select the OpenGL texture compression method.
    @param hint \GL_DONT_CARE\, \GL_FASTEST\, or \GL_NICEST\. (Please refer to an OpenGL text for information on what these mean).
    @return No return value
*/
ConsoleFunctionWithDocs(setOpenGLTextureCompressionHint, ConsoleVoid, 2, 2,  ( hint ))
{
    GLenum newHint        = GL_DONT_CARE;
    const char* newString = "GL_DONT_CARE";

    if (!dStricmp(argv[1], "GL_FASTEST"))
    {
        newHint = GL_FASTEST;
        newString = "GL_FASTEST";
    }
    else if (!dStricmp(argv[1], "GL_NICEST"))
    {
        newHint = GL_NICEST;
        newString = "GL_NICEST";
    }

    TextureManager::mTextureCompressionHint = newHint;

#if !defined(TORQUE_OS_IOS)  && !defined(TORQUE_OS_ANDROID)
    if (dglDoesSupportTextureCompression())
        glHint(GL_TEXTURE_COMPRESSION_HINT_ARB, TextureManager::mTextureCompressionHint);
#endif
}

//--------------------------------------------------------------------------------------------------------------------

/*! Use the flushTextureCache function to flush the texture cache.
    @return No return value.
*/
ConsoleFunctionWithDocs( flushTextureCache, ConsoleVoid, 1, 1, ())
{
    TextureManager::flush();
}

//--------------------------------------------------------------------------------------------------------------------

/*! Dump the texture manager metrics.
*/
ConsoleFunctionWithDocs( dumpTextureManagerMetrics, ConsoleVoid, 1, 1, ())
{
    return TextureManager::dumpMetrics();
}

/*! @} */ // group TextureManagerFunctions
