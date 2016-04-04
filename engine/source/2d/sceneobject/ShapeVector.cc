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

#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "2d/core/Utility.h"
#include "ShapeVector.h"

// Script bindings.
#include "ShapeVector_ScriptBinding.h"

//----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ShapeVector);

//----------------------------------------------------------------------------

ShapeVector::ShapeVector() :
    mLineColor(ColorF(1.0f,1.0f,1.0f,1.0f)),
    mFillColor(ColorF(0.5f,0.5f,0.5f,1.0f)),
    mFillMode(false),
    mIsCircle(false),
    mCircleRadius(1.0f),
    mFlipX(false),
    mFlipY(false)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mPolygonBasisList );
    VECTOR_SET_ASSOCIATION( mPolygonLocalList );

   // Use a static body by default.
   mBodyDefinition.type = b2_staticBody;
}

//----------------------------------------------------------------------------

ShapeVector::~ShapeVector()
{
}

//----------------------------------------------------------------------------

void ShapeVector::initPersistFields()
{
   addProtectedField("PolyList", TypePoint2FVector, Offset(mPolygonBasisList, ShapeVector), &setPolyList, &defaultProtectedGetFn, &writePolyList, "");
   addField("LineColor", TypeColorF, Offset(mLineColor, ShapeVector), &writeLineColor, "");
   addField("FillColor", TypeColorF, Offset(mFillColor, ShapeVector), &writeFillColor, "");
   addField("FillMode", TypeBool, Offset(mFillMode, ShapeVector), &writeFillMode, "");
   addField("IsCircle", TypeBool, Offset(mIsCircle, ShapeVector), &writeIsCircle, "");
   addField("CircleRadius", TypeF32, Offset(mCircleRadius, ShapeVector), &writeCircleRadius, "");

   Parent::initPersistFields();
}

//----------------------------------------------------------------------------

void ShapeVector::copyTo(SimObject* obj)
{
   Parent::copyTo(obj);

   AssertFatal(dynamic_cast<ShapeVector*>(obj), "ShapeVector::copyTo() - Object is not the correct type.");
   ShapeVector* object = static_cast<ShapeVector*>(obj);

   // Copy fields
   object->mFillMode = mFillMode;
   object->mFillColor = mFillColor;
   object->mLineColor = mLineColor;
   object->mIsCircle = mIsCircle;
   object->mCircleRadius = mCircleRadius;
   object->mFlipX = mFlipX;
   object->mFlipY = mFlipY;

   if (getPolyVertexCount() > 0)
       object->setPolyCustom(mPolygonBasisList.size(), getPoly());
}

//----------------------------------------------------------------------------

bool ShapeVector::onAdd()
{
   // Call Parent.
   if(!Parent::onAdd())
      return false;

   // Return Okay.
   return true;
}

//----------------------------------------------------------------------------

void ShapeVector::onRemove()
{
   // Call Parent.
   Parent::onRemove();
}

//----------------------------------------------------------------------------

void ShapeVector::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Fetch Vertex Count.
    const U32 vertexCount = mPolygonLocalList.size();

    // Finish if not vertices.
    if ( vertexCount == 0  && !mIsCircle)
        return;

    // Disable Texturing.
    glDisable       ( GL_TEXTURE_2D );

    // Save Model-view.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Fetch Position/Rotation.
    const Vector2 position = getRenderPosition();

    // Set Blend Options.
    setBlendOptions();
    
    if (mIsCircle)
    {
        glRotatef( mRadToDeg(getRenderAngle()), 0.0f, 0.0f, 1.0f );

        // Render the shape.
        bool wireFrame = (pBatchRenderer->getWireframeMode() || this->getDebugMask() & Scene::SCENE_DEBUG_WIREFRAME_RENDER) ? true : false;
        renderCircleShape(position, mCircleRadius, wireFrame);
    }
    else
    {
        // Move into Vector-Space.
        glTranslatef( position.x, position.y, 0.0f );
        glRotatef( mRadToDeg(getRenderAngle()), 0.0f, 0.0f, 1.0f );

        // Render the shape.
        bool wireFrame = (pBatchRenderer->getWireframeMode() || this->getDebugMask() & Scene::SCENE_DEBUG_WIREFRAME_RENDER) ? true : false;
        renderPolygonShape(vertexCount, wireFrame);
    }

    // Restore color.
    glColor4f( 1,1,1,1 );

    // Restore Matrix.
    glPopMatrix();
}

void ShapeVector::renderCircleShape(Vector2 position, F32 radius, const bool wireFrame)
{
    if (mFillMode)
    {
        // Set the fill color.
        glColor4f((GLfloat)mFillColor.red, (GLfloat)mFillColor.green, (GLfloat)mFillColor.blue, (GLfloat)mFillColor.alpha);

        // Draw the shape.
        const U32 k_segments = 32;
        const F32 k_increment = 2.0f * M_PI_F / k_segments;
        F32 theta = 0.0f;

        Vector<GLfloat> verts;
        for (U32 n = 0; n < k_segments; n++)
        {
            Vector2 v = position + radius * Vector2(mCos(theta), mSin(theta));

            verts.push_back((GLfloat)v.x);
            verts.push_back((GLfloat)v.y);

            theta += k_increment;
        }

        glVertexPointer(2, GL_FLOAT, 0, verts.address());
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, k_segments);

        // If mFillMode is enabled wireframe is also enabled, we'll draw a filled shape with a wire overlay.
        // Set the line color.
        if (wireFrame)
        {
            glColor4f((GLfloat)mLineColor.red, (GLfloat)mLineColor.green, (GLfloat)mLineColor.blue, (GLfloat)mLineColor.alpha);
            glDrawArrays(GL_LINE_LOOP, 0, k_segments);
        }

        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        // We emulate wireframe mode here by drawing lines between the verts.
        // Set the line color.
        glColor4f((GLfloat)mLineColor.red, (GLfloat)mLineColor.green, (GLfloat)mLineColor.blue, (GLfloat)mLineColor.alpha);

        // Draw the shape.
        const U32 k_segments = 32;
        const F32 k_increment = 2.0f * M_PI_F / k_segments;
        F32 theta = 0.0f;

        Vector<GLfloat> verts;
        for (U32 n = 0; n < k_segments; n++)
        {
            Vector2 v = position + radius * Vector2(mCos(theta), mSin(theta));

            verts.push_back((GLfloat)v.x);
            verts.push_back((GLfloat)v.y);

            theta += k_increment;
        }

        glVertexPointer(2, GL_FLOAT, 0, verts.address());
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_LINE_LOOP, 0, k_segments);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void ShapeVector::renderPolygonShape(U32 vertexCount, const bool wireFrame)
{
    // If fill mode is enabled, draw the polygon using GL_TRIANGLE_FAN, otherwise we draw with GL_LINE_LOOP.
    // We also draw with GL_LINE_LOOP if the scene/object is being rendered with wireframe enabled.
    if (mFillMode)
    {
        // Set the fill color.
        glColor4f((GLfloat)mFillColor.red, (GLfloat)mFillColor.green, (GLfloat)mFillColor.blue, (GLfloat)mFillColor.alpha);

        // Draw the shape.
        Vector<GLfloat> verts;
        for (U32 n = 0; n < vertexCount; n++)
        {
            verts.push_back((GLfloat)mPolygonLocalList[n].x);
            verts.push_back((GLfloat)mPolygonLocalList[n].y);
        }

        glVertexPointer(2, GL_FLOAT, 0, verts.address());
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

        // If mFillMode is enabled wireframe is also enabled, we'll draw a filled shape with a wire overlay.
        // Set the line color.
        if (wireFrame)
        {
            glColor4f((GLfloat)mLineColor.red, (GLfloat)mLineColor.green, (GLfloat)mLineColor.blue, (GLfloat)mLineColor.alpha);
            glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
        }

        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        // We emulate wireframe mode here by drawing lines between the verts.
        // Set the line color.
        glColor4f((GLfloat)mLineColor.red, (GLfloat)mLineColor.green, (GLfloat)mLineColor.blue, (GLfloat)mLineColor.alpha);

        // Draw the shape.
        Vector<GLfloat> verts;
        for (U32 n = 0; n < vertexCount; n++)
        {
            verts.push_back((GLfloat)mPolygonLocalList[n].x);
            verts.push_back((GLfloat)mPolygonLocalList[n].y);
        }

        glVertexPointer(2, GL_FLOAT, 0, verts.address());
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

//----------------------------------------------------------------------------

void ShapeVector::setSize( const Vector2& size )
{
    F32 xDifference = mSize.x / size.x;
    
    // Call Parent.
    Parent::setSize( size );
    
    if (mIsCircle)
    {
        mCircleRadius /= xDifference;
    }
    else
    {
        // Generate Local Polygon.
        generateLocalPoly();
    }
}

//----------------------------------------------------------------------------

void ShapeVector::setPolyPrimitive( const U32 polyVertexCount )
{
    // Check it's not zero!
    if ( polyVertexCount == 0 )
    {
        // Warn.
        Con::warnf("ShapeVector::setPolyPrimitive() - Vertex count must be greater than zero!");
        // Finish Here.
        return;
    }

    // Clear Polygon List.
    mPolygonBasisList.clear();
    mPolygonBasisList.setSize( polyVertexCount );

    // Point?
    if ( polyVertexCount == 1 )
    {
        // Set Polygon Point.
        mPolygonBasisList[0].Set(0.0f, 0.0f);
    }
    // Special-Case Quad?
    else if ( polyVertexCount == 4 )
    {
        // Yes, so set Quad.
        mPolygonBasisList[0].Set(-0.5f, -0.5f);
        mPolygonBasisList[1].Set(+0.5f, -0.5f);
        mPolygonBasisList[2].Set(+0.5f, +0.5f);
        mPolygonBasisList[3].Set(-0.5f, +0.5f);
    }
    else
    {
        // No, so calculate Regular (Primitive) Polygon Stepping.
        //
        // NOTE:-   The polygon sits on an circle that subscribes the interior
        //          of the collision box.
        F32 angle = M_PI_F / polyVertexCount;
        const F32 angleStep = M_2PI_F / polyVertexCount;

        // Calculate Polygon.
        for ( U32 n = 0; n < polyVertexCount; n++ )
        {
            // Calculate Angle.
            angle += angleStep;
            // Store Polygon Vertex.
            mPolygonBasisList[n].Set(mCos(angle), mSin(angle));
        }
    }

    // Generation Local Poly.
    generateLocalPoly();
}

//----------------------------------------------------------------------------

void ShapeVector::setPolyCustom( const U32 polyVertexCount, const char* pCustomPolygon )
{
    // Validate Polygon.
    if ( polyVertexCount < 1 )
    {
        // Warn.
        Con::warnf("ShapeVector::setPolyCustom() - Vertex count must be greater than zero!");
        return;
    }

    // Fetch Custom Polygon Value Count.
    const U32 customCount = Utility::mGetStringElementCount(pCustomPolygon);

    // Validate Polygon Custom Length.
    if ( customCount != polyVertexCount*2 )
    {
        // Warn.
        Con::warnf("ShapeVector::setPolyCustom() - Invalid Custom Polygon Items '%d'; expected '%d'!", customCount, polyVertexCount*2 );
        return;
    }
    
    //// Validate Polygon Vertices.
    //for ( U32 n = 0; n < customCount; n+=2 )
    //{
    //    // Fetch Coordinate.
    //    const Vector2 coord = Utility::mGetStringElementVector(pCustomPolygon, n);
    //    // Check Range.
    //    if ( coord.x < -1.0f || coord.x > 1.0f || coord.y < -1.0f || coord.y > 1.0f )
    //    {
    //        // Warn.
    //        Con::warnf("ShapeVector::setPolyCustom() - Invalid Polygon Coordinate range; Must be -1 to +1! '(%g,%g)'", coord.x, coord.y );
    //        return;
    //    }
    //}

    // Clear Polygon Basis List.
    mPolygonBasisList.clear();
    mPolygonBasisList.setSize( polyVertexCount );

    // Validate Polygon Vertices.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        // Fetch Coordinate.
        const F32 x = dAtof(Utility::mGetStringElement(pCustomPolygon, n*2));
        const F32 y = dAtof(Utility::mGetStringElement(pCustomPolygon, n*2+1));

        // Store Polygon Vertex.
        mPolygonBasisList[n].Set(x, y);
    }

    // Generation Local Poly.
    generateLocalPoly();
}

//----------------------------------------------------------------------------

const char* ShapeVector::getPoly( void )
{
    // Get Collision Polygon.
    const Vector2* pPoly = (getPolyVertexCount() > 0) ? getPolyBasis() : NULL;

    // Set Max Buffer Size.
    const U32 maxBufferSize = getPolyVertexCount() * 18 + 1;

    // Get Return Buffer.
    char* pReturnBuffer = Con::getReturnBuffer( maxBufferSize );

    // Check Buffer.
    if( !pReturnBuffer )
    {
        // Warn.
        Con::printf("ShapeVector::getPoly() - Unable to allocate buffer!");
        // Exit.
        return NULL;
    }

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Polygon Edges.
    for ( U32 n = 0; n < getPolyVertexCount(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pReturnBuffer + bufferCount, maxBufferSize-bufferCount, "%0.5f %0.5f ", pPoly[n].x, pPoly[n].y );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("ShapeVector::getPoly() - Error writing to buffer!");
            break;
        }
    }

    // Return Buffer.
    return pReturnBuffer;
}

//----------------------------------------------------------------------------

const char* ShapeVector::getWorldPoly( void )
{
    // Get the object space polygon
    //const Vector2* pPoly = (getPolyVertexCount() > 0) ? getPolyBasis() : NULL;

    // Set the max buffer size
    const U32 maxBufferSize = getPolyVertexCount() * 18 + 1;

    // Get the return buffer.
    char* pReturnBuffer = Con::getReturnBuffer( maxBufferSize );

    // Check the buffer.
    if( !pReturnBuffer )
    {
        // Warn.
        Con::printf("ShapeVector::getWorldPoly() - Unable to allocate buffer!");

        // Exit.
        return NULL;
    }

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Polygon Edges.
    for ( U32 n = 0; n < getPolyVertexCount(); n++ )
    {
        // Convert the poly point to a world coordinate
        Vector2 worldPoint = getWorldPoint(mPolygonLocalList[n]);

        // Output the point
        bufferCount += dSprintf( pReturnBuffer + bufferCount, maxBufferSize-bufferCount, "%0.5f %0.5f ", worldPoint.x, worldPoint.y );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("ShapeVector::getWorldPoly() - Error writing to buffer!");
            break;
        }
    }

    // Return Buffer.
    return pReturnBuffer;
}

//----------------------------------------------------------------------------

void ShapeVector::generateLocalPoly( void )
{
    // Fetch Polygon Vertex Count.
    const U32 polyVertexCount = mPolygonBasisList.size();

    // Process Collision Polygon (if we've got one).
    if ( polyVertexCount > 0 )
    {
        // Clear Polygon List.
        mPolygonLocalList.clear();
        mPolygonLocalList.setSize( polyVertexCount );

        // Scale/Orientate Polygon.
        for ( U32 n = 0; n < polyVertexCount; n++ )
        {
            // Fetch Polygon Basis.
            Vector2 polyVertex = mPolygonBasisList[n];
            // Scale.
            polyVertex.Set( polyVertex.x * mSize.x * (mFlipX ? -1.0f : 1.0f), 
                            polyVertex.y * mSize.y * (mFlipY ? -1.0f : 1.0f));
            // Set Vertex.
            mPolygonLocalList[n] = polyVertex;
        }
    }
}

//----------------------------------------------------------------------------

Vector2 ShapeVector::getBoxFromPoints()
{
    Vector2 box(1.0f, 1.0f);

     // Fetch Polygon Vertex Count.
    const U32 polyVertexCount = mPolygonBasisList.size();

    F32 minX = 0;
    F32 minY = 0;
    F32 maxX = 0;
    F32 maxY = 0;

    // Process Collision Polygon (if we've got one).
    if ( polyVertexCount > 0 )
    {
        // Scale/Orientate Polygon.
        for ( U32 n = 0; n < polyVertexCount; n++ )
        {
            // Fetch Polygon Basis.
            Vector2 polyVertex = mPolygonBasisList[n];
            
            if (polyVertex.x > maxX)
                maxX = polyVertex.x;
            else if (polyVertex.x < minX)
                minX = polyVertex.x;

            if (polyVertex.y > maxY)
                maxY = polyVertex.y;
            else if (polyVertex.y < minY)
                minY = polyVertex.y;
        }
    }

    box.x = maxX - minX;
    box.y = maxY - minY;

    return box;
}
