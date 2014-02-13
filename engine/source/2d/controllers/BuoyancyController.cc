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

#ifndef _BUOYANCY_CONTROLLER_H_
#include "2d/controllers/BuoyancyController.h"
#endif

// Script bindings.
#include "BuoyancyController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(BuoyancyController);

//------------------------------------------------------------------------------

BuoyancyController::BuoyancyController() :
    mFlowVelocity( 0.0f, 0.0f ),
    mFluidDensity( 2.0f ),
    mLinearDrag( 0.0f ),
    mAngularDrag( 0.0f ),
    mFluidGravity( 0.0f, -9.8f ),
    mUseShapeDensity( true ),
    mSurfaceNormal( 0.0f, 1.0f )
{
}

//------------------------------------------------------------------------------

BuoyancyController::~BuoyancyController()
{
}


//------------------------------------------------------------------------------

void BuoyancyController::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addField( "FluidArea", Typeb2AABB, Offset(mFluidArea, BuoyancyController), "The fluid area." );
    addField( "FluidDensity", TypeF32, Offset(mFluidDensity, BuoyancyController), "The fluid density." );
    addField( "FlowVelocity", TypeVector2, Offset(mFlowVelocity, BuoyancyController), "The fluid flow velocity for drag calculations." );
    addField( "LinearDrag", TypeF32, Offset(mLinearDrag, BuoyancyController), "The linear drag co-efficient for the fluid." );
    addField( "AngularDrag", TypeF32, Offset(mAngularDrag, BuoyancyController), "The angular drag co-efficient for the fluid." );
    addField( "FluidGravity", TypeVector2, Offset(mFluidGravity, BuoyancyController), "The gravity to use inside the fluid." );
    addField( "UseShapeDensity", TypeBool, Offset(mUseShapeDensity, BuoyancyController), "Whether to use the collision shape densities or assume a uniform density." );
}

//------------------------------------------------------------------------------

void BuoyancyController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    BuoyancyController* pController;
    pController = static_cast<BuoyancyController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "BuoyancyController::copyTo() - Object is not the correct type.");
}

//------------------------------------------------------------------------------
/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
//------------------------------------------------------------------------------

void BuoyancyController::integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Prepare query filter.
    WorldQuery* pWorldQuery = prepareQueryFilter( pScene );

    // Query for candidate objects.
    pWorldQuery->anyQueryAABB( mFluidArea ); 

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

    // Iterate the results.
    for ( U32 n = 0; n < (U32)queryResults.size(); n++ )
    {
        // Fetch the scene object.
        SceneObject* pSceneObject = queryResults[n].mpSceneObject;

        // Skip if asleep.
        if ( !pSceneObject->getAwake() )
            continue;

        // Ignore if it's a static body.
        if ( pSceneObject->getBodyType() == b2_staticBody )
            continue;

        // Fetch the shape count.
        const U32 shapeCount = pSceneObject->getCollisionShapeCount();

        // Skip if no collision shapes.
        if ( shapeCount == 0 )
            continue;

        // Fetch the body transform.
        const b2Transform& bodyTransform = pSceneObject->getBody()->GetTransform();;

        Vector2 areaCenter(0.0f, 0.0f);
        Vector2 massCenter(0.0f, 0.0f);
        F32 area = 0.0f;
        F32 mass = 0.0f;

        // Yes, so iterate them.
        for( U32 i = 0; i < shapeCount; ++i )
        {
            // Fetch the fixture definition.
            const b2FixtureDef fixtureDef = pSceneObject->getCollisionShapeDefinition( i );

            // Fetch the shape.
            const b2Shape* pShape = fixtureDef.shape;

            Vector2 shapeCenter(0.0f, 0.0f);
            
            F32 shapeArea = 0.0f;

            // Calculate the area for the shape type.
            if ( pShape->GetType() == b2Shape::e_circle )
            {
                shapeArea = ComputeCircleSubmergedArea( bodyTransform, dynamic_cast<const b2CircleShape*>(pShape), shapeCenter );
            }
            else if ( pShape->GetType() == b2Shape::e_polygon)
            {
                shapeArea = ComputePolygonSubmergedArea( bodyTransform, dynamic_cast<const b2PolygonShape*>(pShape), shapeCenter );
            }
            else if ( pShape->GetType() == b2Shape::e_edge || pShape->GetType() == b2Shape::e_chain )
            {
                shapeArea = 0.0f;
            }
            else
            {
                // Skip if unknown shape type.
                continue;
            }

            // Calculate area.
            area += shapeArea;
            areaCenter.x += shapeArea * shapeCenter.x;
            areaCenter.y += shapeArea * shapeCenter.y;

            // Calculate mass.
            const F32 shapeDensity = mUseShapeDensity ? fixtureDef.density : 1.0f;
            mass += shapeArea*shapeDensity;
            massCenter.x += shapeArea * shapeCenter.x * shapeDensity;
            massCenter.y += shapeArea * shapeCenter.y * shapeDensity;
        }

        // Skip not in water.
        if( area < b2_epsilon )
            continue;

        // Calculate area/mass centers.
        areaCenter.x /= area;
        areaCenter.y /= area;
        massCenter.x /= mass;
        massCenter.y /= mass;

        // Buoyancy
        const Vector2 buoyancyForce = -mFluidDensity * area * mFluidGravity;
        pSceneObject->applyForce(buoyancyForce, massCenter);

        // Linear drag
        const Vector2 dragForce = (pSceneObject->getLinearVelocityFromWorldPoint(areaCenter) - mFlowVelocity) * (-mLinearDrag * area);
        pSceneObject->applyForce(dragForce, areaCenter );

        // Angular drag
        pSceneObject->applyTorque( -pSceneObject->getInertia() / pSceneObject->getMass() * area * pSceneObject->getAngularVelocity()*mAngularDrag );
    }
}

//------------------------------------------------------------------------------

F32 BuoyancyController::ComputeCircleSubmergedArea( const b2Transform& bodyTransform, const b2CircleShape* pShape, Vector2& center )
{
    // Sanity!
    AssertFatal( pShape != NULL, "BuoyancyController::ComputeCircleSubmergedArea() - Invalid shape." );

    // Calculate the world shape center.
    const b2Vec2 worldShapeCenter = b2Mul( bodyTransform, pShape->m_p );

    const F32 l = -(b2Dot( mSurfaceNormal, worldShapeCenter ) - mFluidArea.upperBound.y);

    // Fetch the circle radius.
    const F32 radius = pShape->m_radius;

    // Submerged?
    if (l < - radius + FLT_MIN)
    {
        // No, so return zero area submerged.
        return 0.0f;
    }

    // Completely wet?
    if (l > pShape->m_radius)
    {
        // Yes!
        center = worldShapeCenter;
        return  b2_pi * radius * radius;
    }
        
    // Partial submersion.
    const F32 r2 = radius * radius;
    const F32 l2 = l * l;
    const F32 area = r2 *( mAsin(l / radius) + b2_pi / 2.0f) + l * mSqrt( r2 - l2 );
    const F32 com = -2.0f / 3.0f * mPow(r2 - l2, 1.5f) / area;
    
    // Calculate center.
    center.x = worldShapeCenter.x + mSurfaceNormal.x * com;
    center.y = worldShapeCenter.y + mSurfaceNormal.y * com;
        
    return area;
}

//------------------------------------------------------------------------------

F32 BuoyancyController::ComputePolygonSubmergedArea( const b2Transform& bodyTransform, const b2PolygonShape* pShape, Vector2& center )
{
    // Sanity!
    AssertFatal( pShape != NULL, "BuoyancyController::ComputePolygonSubmergedArea() - Invalid shape." );

    // Transform plane into shape co-ordinates
    b2Vec2 normalL = b2MulT( bodyTransform.q, mSurfaceNormal);
    F32 offsetL = mFluidArea.upperBound.y - b2Dot(mSurfaceNormal, bodyTransform.p);
        
    F32 depths[b2_maxPolygonVertices];
    S32 diveCount = 0;
    S32 intoIndex = -1;
    S32 outoIndex = -1;
    
    const S32 vertexCount = pShape->GetVertexCount();
    const b2Vec2* pVertices = pShape->m_vertices;

    bool lastSubmerged = false;
    for ( S32 i = 0; i < vertexCount; ++i )
    {
        depths[i] = b2Dot(normalL, pVertices[i]) - offsetL;
        const bool isSubmerged = depths[i]<-FLT_EPSILON;

        if (i > 0)
        {
            if (isSubmerged)
            {
                if (!lastSubmerged)
                {
                    intoIndex = i-1;
                    diveCount++;
                }
            }
            else
            {
                if (lastSubmerged)
                {
                    outoIndex = i-1;
                    diveCount++;
                }
            }
        }
        lastSubmerged = isSubmerged;
    }

    switch(diveCount)
    {
        case 0:
            if (lastSubmerged)
            {
                // Completely submerged
                b2MassData md;
                pShape->ComputeMass(&md, 1.0f);
                center = b2Mul(bodyTransform, md.center);
                return md.mass;
            }
            else
            {
                // Completely dry
                return 0.0;
            }
            break;

        case 1:
            if( intoIndex==-1 )
            {
                intoIndex = vertexCount-1;
            }
            else
            {
                outoIndex = vertexCount-1;
            }
            break;
    }

    const S32 intoIndex2 = (intoIndex+1) % vertexCount;
    const S32 outoIndex2 = (outoIndex+1) % vertexCount;
        
    const F32 intoLambda = (0 - depths[intoIndex]) / (depths[intoIndex2] - depths[intoIndex]);
    const F32 outoLambda = (0 - depths[outoIndex]) / (depths[outoIndex2] - depths[outoIndex]);
        
    const b2Vec2 intoVec( pVertices[intoIndex].x*(1-intoLambda)+pVertices[intoIndex2].x*intoLambda,
                                    pVertices[intoIndex].y*(1-intoLambda)+pVertices[intoIndex2].y*intoLambda);
    const b2Vec2 outoVec( pVertices[outoIndex].x*(1-outoLambda)+pVertices[outoIndex2].x*outoLambda,
                                    pVertices[outoIndex].y*(1-outoLambda)+pVertices[outoIndex2].y*outoLambda);
        
    // Initialize accumulator
    F32 area = 0.0f;
    center.SetZero();
    b2Vec2 p2 = pVertices[intoIndex2];
    b2Vec2 p3;
        
    const F32 k_inv3 = 1.0f / 3.0f;
        
    // An awkward loop from intoIndex2+1 to outIndex2
    S32 i = intoIndex2;
    while (i != outoIndex2)
    {
        i = (i+1) % vertexCount;
        if (i == outoIndex2)
            p3 = outoVec;
        else
            p3 = pVertices[i];
                
        // Add the triangle formed by intoVec,p2,p3
        const b2Vec2 e1 = p2 - intoVec;
        const b2Vec2 e2 = p3 - intoVec;                        
        const F32 D = b2Cross(e1, e2);                        
        const F32 triangleArea = 0.5f * D;
        area += triangleArea;
                        
        // Area weighted centroid
        center += triangleArea * k_inv3 * (intoVec + p2 + p3);

        p2 = p3;
    }
        
    // Normalize and transform centroid
    center *= 1.0f / area;        
    center = b2Mul(bodyTransform, center);
        
    return area;
}


//------------------------------------------------------------------------------

void BuoyancyController::renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer )
{
    // Call parent.
    Parent::renderOverlay( pScene, pSceneRenderState, pBatchRenderer );

    // Draw fluid area.
    pScene->mDebugDraw.DrawAABB( mFluidArea, ColorF(0.7f, 0.7f, 0.9f) );
}