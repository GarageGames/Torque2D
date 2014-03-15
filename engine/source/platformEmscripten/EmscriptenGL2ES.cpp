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

#include "EmscriptenGL2ES.h"
#include "platform/platformAssert.h"
#include "EmscriptenOGLVideo.h"

#include "math/mMatrix.h"
#include "math/mPoint.h"


#define	INV_255		0.0039215686f
#define	INV_32767	3.0518509476e-5f

/* EAGL and GL functions calling wrappers that log on error */
#define CALL_EAGL_FUNCTION(__FUNC__, ...) ({ EAGLError __error = __FUNC__( __VA_ARGS__ ); if(__error != kEAGLErrorSuccess) Con::printf("%s() called from %s returned error %i\n", #__FUNC__, __FUNCTION__, __error); (__error ? 0 : 1); })
#define CHECK_GL_ERROR() ({ int __error = glGetError(); if(__error) Con::printf("OpenGL error 0x%04X in %s\n", __error, __FUNCTION__); (__error ? 0 : 1); })
// define this to print out glErrors, un-define to get rid of it
#define TEST_FOR_OPENGL_ERRORS CHECK_GL_ERROR();

    
/////////////////////////////////////////////////////////////////////////////
//
//    Wrapper around the matrix stack ops, to see if ever have over/underflow
//
extern "C"
{
static GLenum currentMatrixMode = GL_MODELVIEW;
static int currentProjStackDepth = 0;
    static GLfloat projStack[256];
static int currentTexStackDepth = 0;
    static GLfloat texStack[256];
static int currentModelStackDepth = 0;
    static GLfloat modStack[256];
    
#undef glPushMatrix // temporarily
void EmscriptenGLPushMatrix() {
    GLint depth;
    switch (currentMatrixMode) {
        case GL_MODELVIEW:
            if (currentModelStackDepth > 15) {
                AssertFatal( 0, "ModelView Stack overflow" );
            } else {
                glGetFloatv( GL_MODELVIEW_MATRIX, &modStack[currentModelStackDepth*16] );
                currentModelStackDepth++;
                glGetIntegerv( GL_MODELVIEW_STACK_DEPTH, &depth );
//				AssertWarn( (currentModelStackDepth > depth), "Native ModelView stack depth has been exceeded" );
            }
            break;
        case GL_PROJECTION:
            if (currentProjStackDepth > 15) {
                AssertFatal( 0, "Projection Stack overflow" );
            } else {
                glGetFloatv( GL_PROJECTION_MATRIX, &projStack[currentProjStackDepth*16] );
                currentProjStackDepth++;
                glGetIntegerv( GL_PROJECTION_STACK_DEPTH, &depth );
//				AssertWarn( (currentProjStackDepth > depth), "Native Projection stack depth has been exceeded" );
            }
            break;
        case GL_TEXTURE:
            if (currentTexStackDepth > 15) {
                AssertFatal( 0, "Texture Stack overflow" );
            } else {
                glGetFloatv( GL_TEXTURE_MATRIX, &texStack[currentTexStackDepth*16] );
                currentTexStackDepth++;
                glGetIntegerv( GL_TEXTURE_STACK_DEPTH, &depth );
//				AssertWarn( (currentTexStackDepth > depth), "Native Texture stack depth has been exceeded" );
            }
            break;
        default:
            glPushMatrix();
            GLenum err;
            err = glGetError();
            AssertFatal( !(err == GL_STACK_OVERFLOW), "GL Stack overflow" );
            break;
    }
}
#define glPushMatrix EmscriptenGLPushMatrix
    
#undef glPopMatrix // temporarily
void EmscriptenGLPopMatrix() {
    switch (currentMatrixMode) {
        case GL_MODELVIEW:
            if (currentModelStackDepth <= 0) {
                AssertFatal( 0, "ModelView Stack underflow" );
            } else {
                currentModelStackDepth--;
                glLoadMatrixf( &modStack[currentModelStackDepth*16] );
            }
            break;
        case GL_PROJECTION:
            if (currentProjStackDepth <= 0) {
                AssertFatal( 0, "Projection Stack underflow" );
            } else {
                currentProjStackDepth--;
                glLoadMatrixf( &projStack[currentProjStackDepth*16] );
            }
            break;
        case GL_TEXTURE:
            if (currentTexStackDepth <= 0) {
                AssertFatal( 0, "Texture Stack underflow" );
            } else {
                currentTexStackDepth--;
                glLoadMatrixf( &texStack[currentTexStackDepth*16] );
            }
            break;
        default:
            glPopMatrix();
            GLenum err;
            err = glGetError();
            AssertFatal( !(err == GL_STACK_UNDERFLOW), "GL Stack underflow" );
            break;
    }
}
#define glPopMatrix EmscriptenGLPopMatrix

#undef glMatrixMode // temporarily
void EmscriptenGLMatrixMode( GLenum mode ) {
    currentMatrixMode = mode;
    glMatrixMode( mode );
}
#define glMatrixMode EmscriptenGLMatrixMode // redefine for everyone else
}	

/////////////////////////////////////////////////////////////////////////////
//
//    The following API functions are wrappers to add opengl functionality to opengl-es
//
static GLint beginEndMode = -1;
// tex coord array
static GLfloat * beginEndTexCoord2f = NULL; // 2 entries per coord
static int beginEndTexCoord2f_alloc = 0; // number of bytes allocated to the array
static int beginEndTexCoord2f_size = 0; // number of coords in the array
// vertex array
static GLfloat * beginEndVertex = NULL; // 4 entries per vertex
static int beginEndVertex_alloc = 0; // number of bytes allocated to the array
static int beginEndVertex_size = 0; // number of vertexes in the array
// color array -- needed if glColor is called inside begin/end
static GLfloat * beginEndColor = NULL; // 4 entries per color
static int beginEndColor_size = 0; // number of colors in the array
// normal array
static GLfloat * beginEndNormal = NULL; // 3 entries per normal
static int beginEndNormal_alloc = 0; // number of bytes allocated to the array
static int beginEndNormal_size = 0; // number of normals in the array

// macros to handle re-sizing the arrays as needed
#define CHECK_ARRAY_SIZE( ppArray, pAlloc, pSize, nType, nGroup ) \
    if (*ppArray == NULL) { \
        *pAlloc =  32 * sizeof(nType) * nGroup; \
        *ppArray = (nType*)dMalloc( 32 * sizeof(nType) * nGroup ); \
    } \
    if (*pSize >= *pAlloc / (nGroup * sizeof(nType))) { \
        *ppArray = (nType*)dRealloc( *ppArray, 2 * (*pAlloc) ); \
        *pAlloc *= 2; \
    }
    
    
void glBegin( GLint mode )
{
    if (beginEndMode >= 0) {
        AssertFatal(0, "glBegin(): called without a glEnd");
    }
    beginEndMode = mode;
    beginEndTexCoord2f_size = 0;
    beginEndVertex_size = 0;
    beginEndColor_size = 0;
    beginEndNormal_size = 0;
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}

void glEnd()
{
    if (beginEndMode < 0) {
        AssertFatal(0, "glEnd(): called without a glBegin");
    }
    
    if (beginEndVertex_size > 0) {
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 4, GL_FLOAT, 4*sizeof(GL_FLOAT), beginEndVertex );
    }
    if (beginEndNormal_size > 0) {
        glEnableClientState( GL_NORMAL_ARRAY );
        glNormalPointer( GL_FLOAT, 3*sizeof(GL_FLOAT), beginEndNormal );
    }
    if (beginEndColor_size > 0) {
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 4, GL_FLOAT, 4*sizeof(GL_FLOAT), beginEndColor );
    }
    if (beginEndTexCoord2f_size > 0) {
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 2*sizeof(GL_FLOAT), beginEndTexCoord2f );
    }

    switch (beginEndMode) {
        case GL_POINTS:
            glDrawArrays( GL_POINTS, 0, beginEndVertex_size );
            break;
        case GL_LINES:
            glDrawArrays( GL_LINES, 0, beginEndVertex_size );
            break;
        case GL_LINE_STRIP:
            glDrawArrays( GL_LINE_STRIP, 0, beginEndVertex_size );
            break;
        case GL_LINE_LOOP:
            glDrawArrays( GL_LINE_LOOP, 0, beginEndVertex_size );
            break;
        case GL_TRIANGLES:
            glDrawArrays( GL_TRIANGLES, 0, beginEndVertex_size );
            break;
        case GL_TRIANGLE_STRIP:
            glDrawArrays( GL_TRIANGLE_STRIP, 0, beginEndVertex_size );
            break;
        case GL_TRIANGLE_FAN:
            glDrawArrays( GL_TRIANGLE_FAN, 0, beginEndVertex_size );
            break;
        case GL_QUADS:
#if 1 // %%PUAP%% TESTING
            // for now, just draw a couple lines to indicate the edges of the quads
            glDrawArrays( GL_LINES, 0, beginEndVertex_size );
#else
            // draw these as individual pairs of triangle_strips
            glDrawArrays( GL_TRIANGLE_STRIP, 0, beginEndVertex_size );
#endif
            break;
        case GL_QUAD_STRIP:
#if 1 // %%PUAP%% TESTING
            glDrawArrays( GL_LINES, 0, beginEndVertex_size );
#endif
            break;
        case GL_POLYGON:
            // see if it is really just a triangle...
            if (beginEndVertex_size == 3) {
                glDrawArrays( GL_TRIANGLES, 0, 3 );
            } else 
            // see if it is really just a quad...
            if (beginEndVertex_size == 4) {
                glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
            } else
                AssertFatal( 0, "glBegin(GL_POLYGON): this method call needs to be implemented");
            break;
    }
    
    if (beginEndVertex_size > 0) {
        glDisableClientState( GL_VERTEX_ARRAY );
    }
    if (beginEndNormal_size > 0) {
        glDisableClientState( GL_NORMAL_ARRAY );
    }
    if (beginEndColor_size > 0) {
        glDisableClientState( GL_COLOR_ARRAY );
    }
    if (beginEndTexCoord2f_size > 0) {
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    }
    beginEndMode = -1;	
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}

void glTexCoord2f( GLfloat x, GLfloat y)
{
    AssertFatal( (beginEndMode >= 0), "glTexCoord2f(): called outside glBegin/glEnd");
    CHECK_ARRAY_SIZE( &beginEndTexCoord2f, &beginEndTexCoord2f_alloc, &beginEndTexCoord2f_size, GLfloat, 2 );
    beginEndTexCoord2f[ beginEndTexCoord2f_size*2 ] = x;
    beginEndTexCoord2f[ beginEndTexCoord2f_size*2+1 ] = y;
    beginEndTexCoord2f_size++;
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    AssertFatal( (beginEndMode >= 0), "glVertex3f(): called outside glBegin/glEnd");
    CHECK_ARRAY_SIZE( &beginEndVertex, &beginEndVertex_alloc, &beginEndVertex_size, GLfloat, 4 );
    beginEndVertex[ beginEndVertex_size*4 ] = x;
    beginEndVertex[ beginEndVertex_size*4+1 ] = y;
    beginEndVertex[ beginEndVertex_size*4+2 ] = z;
    beginEndVertex[ beginEndVertex_size*4+3 ] = w;
    beginEndVertex_size++;
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex2f( GLfloat x, GLfloat y )
{
    AssertFatal( (beginEndMode >= 0), "glVertex2f(): called outside glBegin/glEnd");
    glVertex4f( x, y, 0.0f, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
    AssertFatal( (beginEndMode >= 0), "glVertex3f(): called outside glBegin/glEnd");
    glVertex4f( x, y, z, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex2fv( const F32 * pv )
{
    AssertFatal( (beginEndMode >= 0), "glVertex2fv(): called outside glBegin/glEnd");
    glVertex4f( pv[0], pv[1], 0.0f, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex2i( GLint x, GLint y)
{
    AssertFatal( (beginEndMode >= 0), "glVertex2i(): called outside glBegin/glEnd");
    glVertex4f( GLfloat(x), GLfloat(y), 0.0f, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex3fv( const F32 * pv )
{
    AssertFatal( (beginEndMode >= 0), "glVertex3fv(): called outside glBegin/glEnd");
    glVertex4f( pv[0], pv[1], pv[2], 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glVertex4fv( const F32 * pv )
{
    AssertFatal( (beginEndMode >= 0), "glVertex3fv(): called outside glBegin/glEnd");
    glVertex4f( pv[0], pv[1], pv[2], pv[3] );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glNormal3f( GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    AssertFatal( (beginEndMode >= 0), "glNormal3f(): called outside glBegin/glEnd");
    CHECK_ARRAY_SIZE( &beginEndNormal, &beginEndNormal_alloc, &beginEndNormal_size, GLfloat, 3 );
    beginEndNormal[ beginEndNormal_size*3 ] = x;
    beginEndNormal[ beginEndNormal_size*3+1 ] = y;
    beginEndNormal[ beginEndNormal_size*3+2 ] = z;
    beginEndNormal_size++;
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glNormal3fv( const F32 * pv)
{
    AssertFatal( (beginEndMode >= 0), "glNormal3fv(): called outside glBegin/glEnd");
    glNormal3f( pv[0], pv[1], pv[2] );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glColor3fv( const F32 * pv)
{
    glColor4f( pv[0], pv[1], pv[2], 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glColor4fv( const F32 * pv)
{
    glColor4f( pv[0], pv[1], pv[2], pv[3] );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glActiveTextureARB( GLint index )
{
    glActiveTexture( index );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glMultiTexCoord2fARB( GLint, GLfloat, GLfloat)
{
    AssertWarn( 0, "glMultiTexCoord2fARB(): this method call needs to be implemented");
}
void glPushAttrib( GLint )
{
    AssertWarn( 0, "glPushAttrib(): this method call needs to be implemented");
}
void glPopAttrib()
{
    AssertWarn( 0, "glPopAttrib(): this method call needs to be implemented");
}
//void glOrtho( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
//{
//	GLfloat tx = -(right+left)/(right-left);
//	GLfloat ty = -(top+bottom)/(top-bottom);
//	GLfloat tz = -(zFar+zNear)/(zFar-zNear);
//	MatrixF m = MatrixF(true);
//	((F32*)m)[0] = 2.0f/(right-left);
//	((F32*)m)[3] = tx;
//	((F32*)m)[5] = 2.0f/(top-bottom);
//	((F32*)m)[7] = ty;
//	((F32*)m)[10] = -2.0f/(zFar-zNear);
//	((F32*)m)[11] = tz;
//	
//	glMultMatrixf( (F32*)(m.transpose())); // make col major for opengl
//}
//void glFrustum( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar )
//{
//	MatrixF m = MatrixF(true);
//	((F32*)m)[0] = 2.0f*zNear/(right-left);
//	((F32*)m)[2] = (right+left)/(right-left);
//	((F32*)m)[5] = 2.0f*zNear/(top-bottom);
//	((F32*)m)[6] = (top+bottom)/(top-bottom);
//	((F32*)m)[10] = -(zFar+zNear)/(zFar-zNear);
//	((F32*)m)[11] = -2.0f*zFar*zNear/(zFar-zNear);
//	((F32*)m)[14] = -1.0f;
//	((F32*)m)[15] = 0.0f;
//	
//	glMultMatrixf( (F32*)(m.transpose())); // make col major for opengl
//}
void glReadBuffer( GLint )
{
    AssertWarn( 0, "glReadBuffer(): this method call needs to be implemented");
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glClientActiveTextureARB( GLint texture )
{
    glClientActiveTexture( texture );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glColor3i( GLint  r, GLint g, GLint b )
{
    glColor4f( ((GLfloat)r) * INV_32767, ((GLfloat)g) * INV_32767, ((GLfloat)b) * INV_32767, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glColor3ubv( const GLubyte *v )
{
    glColor4f( ((GLfloat)v[0]) * INV_255, ((GLfloat)v[1]) * INV_255, ((GLfloat)v[2]) * INV_255, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glColor4ubv( const GLubyte *v )
{
    glColor4f( ((GLfloat)v[0]) * INV_255, ((GLfloat)v[1]) * INV_255, ((GLfloat)v[2]) * INV_255, ((GLfloat)v[3]) * INV_255 );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glRecti( GLint x1, GLint y1, GLint x2, GLint y2 )
{
    AssertFatal( beginEndMode<0, "glRecti(): called inside glBegin/glEnd");
    glBegin(GL_LINE_LOOP);
    glVertex2i(x1, y1);
    glVertex2i(x2, y1);
    glVertex2i(x2, y2);
    glVertex2i(x1, y2);
    glEnd();
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}

void glGetDoublev( GLint pname, GLdouble * params )
{
    int i;
    GLfloat fparams[16];
    glGetFloatv( pname, fparams );
    AssertFatal((glGetError() == GL_NO_ERROR), "glGetDoublev parameter invalid");
    params[0] = (double)fparams[0]; 
    if ((pname == GL_ALIASED_POINT_SIZE_RANGE) ||
        (pname == GL_ALIASED_LINE_WIDTH_RANGE) ||
        (pname == GL_DEPTH_RANGE) ||
        (pname == GL_MAX_VIEWPORT_DIMS) ||
        (pname == GL_SMOOTH_LINE_WIDTH_RANGE) ||
        (pname == GL_SMOOTH_POINT_SIZE_RANGE)) {
        // 2 items
        params[1] = (double)fparams[1];
        return;
    }
    if ((pname == GL_COLOR_CLEAR_VALUE) ||
        (pname == GL_COLOR_WRITEMASK) ||
        (pname == GL_FOG_COLOR) ||
        (pname == GL_LIGHT_MODEL_AMBIENT) ||
        (pname == GL_SCISSOR_BOX) ||
        (pname == GL_VIEWPORT) ) {
        // 4 items
        params[1] = (double)fparams[1];
        params[2] = (double)fparams[2];
        params[3] = (double)fparams[3];
        return;
    }
    if ((pname == GL_PROJECTION_MATRIX) ||
        //Luma: Added MODDELVIEW support
        (pname == GL_MODELVIEW_MATRIX) ||
        //(pname == GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS) ||
        //(pname == GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS) ||  
        (pname == GL_TEXTURE_MATRIX) ) {
        // 16 items
        for (i=1; i<16; i++) {
            params[i] = (double)fparams[i];
        }
        return;
    }
}
void glPolygonMode( GLint, GLint )
{
    //AssertFatal( 0, "glPolygonMode(): this method call needs to be implemented");
}
void glLockArraysEXT( GLint, GLint )
{
    AssertFatal( 0, "glLockArraysEXT(): this method call needs to be implemented");
}
void glUnlockArraysEXT()
{
    AssertFatal( 0, "glUnlockArraysEXT(): this method call needs to be implemented");
}
void glColor3ub( GLint r, GLint g, GLint b )
{
    glColor4f( ((GLfloat)r) * INV_255, ((GLfloat)g) * INV_255, ((GLfloat)b) * INV_255, 1.0f );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
void glFogi( GLint pname, GLint param )
{
    glFogf( pname, (GLfloat)param );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
/*	-Mat note: only guiCanvas uses this, and before adding this GL2ES stuff we didn't need to use it (it was one of the first things if'ed out)
void glDrawBuffer( GLint buffid )
{
    ((EAGLView *)platState.ctx).currentRenderBuffer = buffid;
    TEST_FOR_OPENGL_ERRORS
}
*/
void glColorTableEXT( GLint, GLint, GLint, GLint, GLint, const ColorI * )
{
    AssertFatal( 0, "glColorTableEXT(): this method call needs to be implemented");
}
void glBlendColorEXT( GLfloat, GLfloat, GLfloat, GLfloat )
{
    AssertFatal( 0, "glBlendColorEXT(): this method call needs to be implemented");
}
void glBlendEquationEXT( GLint )
{
    AssertFatal( 0, "glBlendEquationEXT(): this method call needs to be implemented");
}
void glArrayElement( GLint )
{
    AssertFatal( 0, "glArrayElement(): this method call needs to be implemented");
}
void glFogCoordPointerEXT(GLenum, GLsizei, void *)
{
    AssertFatal( 0, "glFogCoordPointerEXT(): this method call needs to be implemented");
}
//void glDepthRange( GLfloat, GLfloat )
//{
//	AssertFatal( 0, "glDepthRange(): this method call needs to be implemented");
//}
void glTexGeni( GLenum coord, GLenum pname, GLint param )
{
    AssertFatal( 0, "glTexGeni(): this method call needs to be implemented");
}
void glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params )
{
    AssertFatal( 0, "glTexGenfv(): this method call needs to be implemented");
}
void glClipPlane( GLuint plane, GLdouble * equation )
{
    GLfloat fequ[4];
    fequ[0] = (GLfloat)equation[0];
    fequ[1] = (GLfloat)equation[1];
    fequ[2] = (GLfloat)equation[2];
    fequ[3] = (GLfloat)equation[3];
    glClipPlanef( plane, fequ );
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
}
GLboolean glAreTexturesResident( GLsizei, const GLuint *, GLboolean*)
{
    int glError;
    glError = TEST_FOR_OPENGL_ERRORS
    return GL_FALSE;
}

