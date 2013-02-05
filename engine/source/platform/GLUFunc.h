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

GL_FUNCTION(void,		gluOrtho2D, (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top), return; )
GL_FUNCTION(void,		gluPerspective, (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar), return; )
GL_FUNCTION(void,		gluPickMatrix, (GLdouble x, GLdouble y, GLdouble width, GLdouble height, GLint viewport[4]), return; )
GL_FUNCTION(void,		gluLookAt, (GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz), return; )
GL_FUNCTION(int,		gluProject, (GLdouble objx, GLdouble objy, GLdouble objz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *winx, GLdouble *winy, GLdouble *winz), return 0; )
GL_FUNCTION(int,		gluUnProject, (GLdouble winx, GLdouble winy, GLdouble winz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *objx, GLdouble *objy, GLdouble *objz), return 0; )
GL_FUNCTION(int,		gluScaleImage, (GLenum format, GLint widthin, GLint heightin, GLenum typein, const void *datain, GLint widthout, GLint heightout, GLenum typeout, void *dataout), return 0; )
GL_FUNCTION(int,		gluBuild1DMipmaps, (GLenum target, GLint components, GLint width, GLenum format, GLenum type, const void *data), return 0; )
GL_FUNCTION(int,		gluBuild2DMipmaps, (GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void *data), return 0; )
