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

#include "2d/core/ParticleSystem.h"

//------------------------------------------------------------------------------

ParticleSystem* ParticleSystem::Instance = NULL;

//------------------------------------------------------------------------------

void ParticleSystem::Init( void )
{
    // Create the particle system.
    Instance = new ParticleSystem();
}

//------------------------------------------------------------------------------

void ParticleSystem::destroy( void )
{
    // Delete the particle system.
    delete Instance;
    Instance = NULL;
}

//------------------------------------------------------------------------------

ParticleSystem::ParticleSystem() :
                    mParticlePoolBlockSize(512)
{
    // Reset the free particle head.
    mpFreeParticleNodes = NULL;

    // Reset the active particle count.
    mActiveParticleCount = 0;
}

//------------------------------------------------------------------------------

ParticleSystem::~ParticleSystem()
{
    // Destroy all the particle pool blocks.
    for ( U32 n = 0; n < (U32)mParticlePool.size(); n++ )
        delete [] mParticlePool[n];

    // Clear the particle pool.
    mParticlePool.clear();

    // Reset the free particle head.
    mpFreeParticleNodes = NULL;
}

//------------------------------------------------------------------------------

ParticleSystem::ParticleNode* ParticleSystem::createParticle( void )
{
    // Have we got any free particle nodes?
    if ( mpFreeParticleNodes == NULL )
    {
        // No, so generate a new free pool block.
        ParticleNode* pFreePoolBlock = new ParticleNode[mParticlePoolBlockSize];

        // Store new free pool block.
        mParticlePool.push_back( pFreePoolBlock );

        // Initialise Free Pool Block.
        for ( U32 n = 0; n < (mParticlePoolBlockSize-1); n++ )
        {
            pFreePoolBlock[n].mPreviousNode = NULL;
            pFreePoolBlock[n].mNextNode     = pFreePoolBlock+n+1;
        }

        // Insert Last Node Preceding any existing free nodes.
        pFreePoolBlock[mParticlePoolBlockSize-1].mPreviousNode = NULL;
        pFreePoolBlock[mParticlePoolBlockSize-1].mNextNode = mpFreeParticleNodes;

        // Set Free References.
        mpFreeParticleNodes = pFreePoolBlock;
    }

    // Fetch a free node,
    ParticleNode* pFreeParticleNode = mpFreeParticleNodes;

    // Set the new free node reference.
    mpFreeParticleNodes = mpFreeParticleNodes->mNextNode;

    // Reset any previous or next node references.
    pFreeParticleNode->mNextNode        = NULL;
    pFreeParticleNode->mPreviousNode    = NULL;

    // Increase the active particle count.
    mActiveParticleCount++;

    return pFreeParticleNode;
}

//------------------------------------------------------------------------------

void ParticleSystem::freeParticle( ParticleNode* pParticleNode )
{
    // Reset the particle.
    pParticleNode->resetState();

    // Remove the previous node reference.
    pParticleNode->mPreviousNode = NULL;
    
    // Insert the node into the free pool.
    pParticleNode->mNextNode = mpFreeParticleNodes;
    mpFreeParticleNodes = pParticleNode;

    // Decrease the active particle count.
    mActiveParticleCount--;
}

