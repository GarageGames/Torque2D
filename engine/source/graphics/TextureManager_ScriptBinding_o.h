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

ConsoleFunction(setOpenGLTextureCompressionHint, void, 2, 2, " ( hint ) Use the setOpenGLTextureCompressionHint function to select the OpenGL texture compression method.\n"
    "@param hint \"GL_DONT_CARE\", \"GL_FASTEST\", or \"GL_NICEST\". (Please refer to an OpenGL text for information on what these mean).\n"
    "@return No return value")
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

#if !defined(TORQUE_OS_IOS)
    if (dglDoesSupportTextureCompression())
        glHint(GL_TEXTURE_COMPRESSION_HINT_ARB, TextureManager::mTextureCompressionHint);
#endif
}

//--------------------------------------------------------------------------------------------------------------------

ConsoleFunction( flushTextureCache, void, 1, 1, "() Use the flushTextureCache function to flush the texture cache.\n"
                                                "@return No return value.\n")
{
    TextureManager::flush();
}

//--------------------------------------------------------------------------------------------------------------------

ConsoleFunction( dumpTextureManagerMetrics, void, 1, 1, "() Dump the texture manager metrics." )
{
    return TextureManager::dumpMetrics();
}


/*! @} */ // group TextureManagerFunctions
