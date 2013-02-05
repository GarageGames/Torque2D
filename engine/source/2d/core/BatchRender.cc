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

#include "BatchRender.h"

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

BatchRender::BatchRender() :
    mQuadCount( 0 ),
    mVertexCount( 0 ),
    mTextureResidentCount( 0 ),
    mIndexCount( 0 ),
    mColorCount( 0 ),
    NoColor( -1.0f, -1.0f, -1.0f ),
    mStrictOrderMode( false ),
    mpDebugStats( NULL ),
    mBlendMode( true ),
    mSrcBlendFactor( GL_SRC_ALPHA ),
    mDstBlendFactor( GL_ONE_MINUS_SRC_ALPHA ),
    mBlendColor( ColorF(1.0f,1.0f,1.0f,1.0f) ),
    mAlphaTestMode( -1.0f ),
    mWireframeMode( false ),
    mBatchEnabled( true )
{
}

//-----------------------------------------------------------------------------

BatchRender::~BatchRender()
{
    // Destroy index vectors in texture batch map.
    for ( textureBatchType::iterator itr = mTextureBatchMap.begin(); itr != mTextureBatchMap.end(); ++itr )
    {
        delete itr->value;
    }
    mTextureBatchMap.clear();

    // Destroy index vectors in index vector pool.
    for ( VectorPtr< indexVectorType* >::iterator itr = mIndexVectorPool.begin(); itr != mIndexVectorPool.end(); ++itr )
    {
        delete (*itr);
    }
    mIndexVectorPool.clear();
}

//-----------------------------------------------------------------------------

void BatchRender::setBlendMode( const SceneRenderRequest* pSceneRenderRequest )
{
    // Are we blending?
    if ( pSceneRenderRequest->mBlendMode )
    {
        // Yes, so set blending to standard alpha-blending.
        setBlendMode(
            pSceneRenderRequest->mSrcBlendFactor,
            pSceneRenderRequest->mDstBlendFactor,
            pSceneRenderRequest->mBlendColor );                            
    }
    else
    {
        // No, so turn-off blending.
        setBlendOff();
    }
}

//-----------------------------------------------------------------------------

void BatchRender::setAlphaTestMode( const SceneRenderRequest* pSceneRenderRequest )
{
    // Set alpha-test mode.
    setAlphaTestMode( pSceneRenderRequest->mAlphaTest );
}

//-----------------------------------------------------------------------------

void BatchRender::SubmitQuad(
        const Vector2& vertexPos0,
        const Vector2& vertexPos1,
        const Vector2& vertexPos2,
        const Vector2& vertexPos3,
        const Vector2& texturePos0,
        const Vector2& texturePos1,
        const Vector2& texturePos2,
        const Vector2& texturePos3,
        TextureHandle& texture,
        const ColorF& color )
{
    // Sanity!
    AssertFatal( mpDebugStats != NULL, "Debug stats have not been configured." );

    PROFILE_START(BatchRender_SubmitQuad);

    // Do we have anything batched?
    if ( mQuadCount > 0 )
    {
        // Yes, so do we have any existing colors?
        if ( mColorCount == 0 )
        {
            // No, so flush if color is specified.
            if ( color != NoColor  )
                flush( mpDebugStats->batchColorStateFlush );
        }
        else
        {
            // Yes, so flush if color is not specified.
            if ( color == NoColor  )
                flush( mpDebugStats->batchColorStateFlush );
        }
    }

    // Is a color specified?
    if ( color != NoColor )
    {
        // Yes, so add colors.
        mColorBuffer[mColorCount++] = color;
        mColorBuffer[mColorCount++] = color;
        mColorBuffer[mColorCount++] = color;
        mColorBuffer[mColorCount++] = color;
    }

    // Strict order mode?
    if ( mStrictOrderMode )
    {
        // Is there is a texture change.
        if ( texture != mStrictOrderTextureHandle )
        {
            // Yes, so flush.
            flush( mpDebugStats->batchTextureChangeFlush );
        }

        // Add new indices.
        mIndexBuffer[mIndexCount++] = (U16)mVertexCount++;
        mIndexBuffer[mIndexCount++] = (U16)mVertexCount++;
        mIndexBuffer[mIndexCount++] = (U16)mVertexCount++;
        mIndexBuffer[mIndexCount++] = (U16)mVertexCount--;
        mIndexBuffer[mIndexCount++] = (U16)mVertexCount--;
        mIndexBuffer[mIndexCount++] = (U16)mVertexCount--;

        // Set strict order mode texture handle.
        mStrictOrderTextureHandle = texture;
    }
    else
    {
        // No, so fetch texture binding.
        const U32 textureBinding = texture.getGLName();

        indexVectorType* pIndexVector = NULL;

        // Find texture binding.
        textureBatchType::iterator itr = mTextureBatchMap.find( textureBinding );

        // Did we find a texture binding?
        if ( itr == mTextureBatchMap.end() )
        {
            // No, so fetch index vector pool count.
            const U32 indexVectorPoolCount = mIndexVectorPool.size();

            // Do we have any in the index vector pool?
            if ( indexVectorPoolCount > 0 )
            {
                // Yes, so use it.
                pIndexVector = mIndexVectorPool[indexVectorPoolCount-1];
                mIndexVectorPool.pop_back();
            }
            else
            {
                // No, so generate one.
                pIndexVector = new indexVectorType( 6 * 6 );
            }

            // Insert into texture batch map.
            mTextureBatchMap.insert( textureBinding, pIndexVector );
        }
        else
        {
            // Yes, so fetch it.
            pIndexVector = itr->value;
        }

        // Add vertex start.
        pIndexVector->push_back( mVertexCount );       
    }

    // Add textured vertices.
    // NOTE: We swap #2/#3 here.
    mVertexBuffer[mVertexCount++]   = vertexPos0;
    mVertexBuffer[mVertexCount++]   = vertexPos1;
    mVertexBuffer[mVertexCount++]   = vertexPos3;
    mVertexBuffer[mVertexCount++]   = vertexPos2;
    mTextureBuffer[mTextureResidentCount++] = texturePos0;
    mTextureBuffer[mTextureResidentCount++] = texturePos1;
    mTextureBuffer[mTextureResidentCount++] = texturePos3;
    mTextureBuffer[mTextureResidentCount++] = texturePos2;

    // Stats.
    mpDebugStats->batchTrianglesSubmitted+=2;

    // Increase quad count.
    mQuadCount++;

    // Have we reached the buffer limit?
    if ( mQuadCount == BATCHRENDER_MAXQUADS )
    {
        // Yes, so flush.
        flush( mpDebugStats->batchBufferFullFlush );
    }

    // Is batching enabled?
    if ( !mBatchEnabled )
    {
        // No, so flush immediately.
        // NOTE: Technically this is still batching but will still revert to using
        // more draw calls therefore can be used in comparison.
        flushInternal();
    }

    PROFILE_END();   // BatchRender_SubmitQuad
}

//-----------------------------------------------------------------------------

void BatchRender::flush( U32& reasonMetric )
{
    // Finish if no quads to flush.
    if ( mQuadCount == 0 )
        return;

    // Increase reason metric.
    reasonMetric++;

    // Flush.
    flushInternal();
}

//-----------------------------------------------------------------------------

void BatchRender::flush( void )
{
    // Finish if no quads to flush.
    if ( mQuadCount == 0 )
        return;

    // Increase reason metric.
    mpDebugStats->batchAnonymousFlush++;

    // Flush.
    flushInternal();
}

//-----------------------------------------------------------------------------

void BatchRender::flushInternal( void )
{
    // Finish if no quads to flush.
    if ( mQuadCount == 0 )
        return;

    PROFILE_START(T2D_BatchRender_flush);

    // Stats.
    mpDebugStats->batchFlushes++;

    if ( mWireframeMode )
    {
        // Disable texturing.    
        glDisable( GL_TEXTURE_2D );

        // Set the polygon mode to line.
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        // Enable texturing.    
        glEnable( GL_TEXTURE_2D );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

        // Set the polygon mode to fill.
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    // Set blend mode.
    if ( mBlendMode )
    {
        glEnable( GL_BLEND );
        glBlendFunc( mSrcBlendFactor, mDstBlendFactor );
        glColor4f(mBlendColor.red, mBlendColor.green, mBlendColor.blue, mBlendColor.alpha );
    }
    else
    {
        glDisable( GL_BLEND );
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    }

    // Set alpha-blend mode.
    if ( mAlphaTestMode >= 0.0f )
    {
        glEnable( GL_ALPHA_TEST );
        glAlphaFunc( GL_GREATER, mAlphaTestMode );
    }
    else
    {
        glDisable( GL_ALPHA_TEST );
    }

    // Enable vertex and texture arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 0, mVertexBuffer );
    glTexCoordPointer( 2, GL_FLOAT, 0, mTextureBuffer );

    // Use the texture coordinates if not in wireframe mode.
    if ( !mWireframeMode )
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // Do we have any colors?
    if ( mColorCount > 0 )
    {
        // Yes, so enable color array.
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 4, GL_FLOAT, 0, mColorBuffer );
    }

    // Strict order mode?
    if ( mStrictOrderMode )
    {
        // Bind the texture if not in wireframe mode.
        if ( !mWireframeMode )
            glBindTexture( GL_TEXTURE_2D, mStrictOrderTextureHandle.getGLName() );

        // Yes, so do we have a single quad?
        if ( mQuadCount == 1 )
        {
            // Yes, so draw the quad using a triangle-strip with indexes.
            glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );    

            // Stats.
            mpDebugStats->batchDrawCallsStrictSingle++;

            // Stats.
            if ( mpDebugStats->batchMaxTriangleDrawn < 2 )
                mpDebugStats->batchMaxTriangleDrawn = 2;
        }
        else
        {
            // Draw the quads using triangles with indexes.
            glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, mIndexBuffer );

            // Stats.
            mpDebugStats->batchDrawCallsStrictMultiple++;

            // Stats.
            const U32 trianglesDrawn = mIndexCount / 3;
            if ( trianglesDrawn > mpDebugStats->batchMaxTriangleDrawn )
                mpDebugStats->batchMaxTriangleDrawn = trianglesDrawn;
        }

        // Stats.
        if ( mVertexCount > mpDebugStats->batchMaxVertexBuffer )
            mpDebugStats->batchMaxVertexBuffer = mVertexCount;
    }
    else
    {
        // No, so iterate texture batch map.
        for( textureBatchType::iterator batchItr = mTextureBatchMap.begin(); batchItr != mTextureBatchMap.end(); ++batchItr )
        {
            // Fetch texture binding.
            const U32 textureBinding = batchItr->key;

            // Fetch index vector.
            indexVectorType* pIndexVector = batchItr->value;

            // Reset index count.
            mIndexCount = 0;

            // Iterate indexes.
            for( indexVectorType::iterator indexItr = pIndexVector->begin(); indexItr != pIndexVector->end(); ++indexItr )
            {
                // Fetch quad index.
                U32 quadIndex = (*indexItr);

                // Add new indices.
                mIndexBuffer[mIndexCount++] = (U16)quadIndex++;
                mIndexBuffer[mIndexCount++] = (U16)quadIndex++;
                mIndexBuffer[mIndexCount++] = (U16)quadIndex++;
                mIndexBuffer[mIndexCount++] = (U16)quadIndex--;
                mIndexBuffer[mIndexCount++] = (U16)quadIndex--;
                mIndexBuffer[mIndexCount++] = (U16)quadIndex;
            }

            // Sanity!
            AssertFatal( mIndexCount > 0, "No batching indexes are present." );

            // Bind the texture if not in wireframe mode.
            if ( !mWireframeMode )
                glBindTexture( GL_TEXTURE_2D, textureBinding );

            // Draw the quads using triangles with indexes.
            glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, mIndexBuffer );

            // Stats.
            mpDebugStats->batchDrawCallsSorted++;

            // Stats.
            if ( mVertexCount > mpDebugStats->batchMaxVertexBuffer )
                mpDebugStats->batchMaxVertexBuffer = mVertexCount;

            // Stats.
            const U32 trianglesDrawn = mIndexCount / 3;
            if ( trianglesDrawn > mpDebugStats->batchMaxTriangleDrawn )
                mpDebugStats->batchMaxTriangleDrawn = trianglesDrawn;

            // Return index vector to pool.
            pIndexVector->clear();
            mIndexVectorPool.push_back( pIndexVector );
        }

        // Clear texture batch map.
        mTextureBatchMap.clear();
    }

    // Reset common render state.
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisable( GL_ALPHA_TEST );
    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // Reset batch state.
    mQuadCount = 0;
    mVertexCount = 0;
    mTextureResidentCount = 0;
    mIndexCount = 0;
    mColorCount = 0;

    PROFILE_END();   // T2D_BatchRender_flush
}

//-----------------------------------------------------------------------------

void BatchRender::RenderQuad(
        const Vector2& vertexPos0,
        const Vector2& vertexPos1,
        const Vector2& vertexPos2,
        const Vector2& vertexPos3,
        const Vector2& texturePos0,
        const Vector2& texturePos1,
        const Vector2& texturePos2,
        const Vector2& texturePos3 )
{
    glBegin( GL_TRIANGLE_STRIP );
        glTexCoord2f( texturePos0.x, texturePos0.y );
        glVertex2f( vertexPos0.x, vertexPos0.y );
        glTexCoord2f( texturePos1.x, texturePos1.y );
        glVertex2f( vertexPos1.x, vertexPos1.y );
        glTexCoord2f( texturePos3.x, texturePos3.y );
        glVertex2f( vertexPos3.x, vertexPos3.y );
        glTexCoord2f( texturePos2.x, texturePos2.y );
        glVertex2f( vertexPos2.x, vertexPos2.y );
    glEnd();
}


