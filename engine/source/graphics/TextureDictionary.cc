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

#include "graphics/TextureDictionary.h"
#include "graphics/TextureManager.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "collection/vector.h"

//-----------------------------------------------------------------------------

TextureObject **TextureDictionary::smTable = NULL;
TextureObject *TextureDictionary::TextureObjectChain = NULL;
U32 TextureDictionary::smHashTableSize = 0;

//--------------------------------------------------------------------------------------------------------------------

void TextureDictionary::create()
{
    TextureObjectChain = NULL;
    smHashTableSize = 1023;
    smTable = new TextureObject *[smHashTableSize];
    for(U32 i = 0; i < smHashTableSize; i++)
        smTable[i] = NULL;
}

//-----------------------------------------------------------------------------

TextureObject* TextureDictionary::find( StringTableEntry textureKey )
{
    U32 key = HashPointer(textureKey) % smHashTableSize;
    TextureObject *walk = smTable[key];
    for(; walk; walk = walk->hashNext)
    {
        if(walk->mTextureKey == textureKey)
            break;
    }
    return walk;
}

//-----------------------------------------------------------------------------

TextureObject *TextureDictionary::find(StringTableEntry textureKey, TextureHandle::TextureHandleType type, bool clamp)
{
    // Sanity!
    AssertISV( type != TextureHandle::InvalidTexture, "Invalid texture type." );

    U32 key = HashPointer(textureKey) % smHashTableSize;
    TextureObject *walk = smTable[key];
    for(; walk; walk = walk->hashNext)
        if(walk->mTextureKey == textureKey && walk->mHandleType == type && walk->mClamp == clamp)
            break;
    return walk;
}


//--------------------------------------------------------------------------------------------------------------------

void TextureDictionary::remove(TextureObject *object)
{
    if(object->next)
        object->next->prev = object->prev;

    if(object->prev)
        object->prev->next = object->next;
    else
        TextureObjectChain = object->next;

    if( object->mTextureKey == NULL )
        return;

    U32 key = HashPointer(object->mTextureKey) % smHashTableSize;
    TextureObject **walk = &smTable[key];
    while(*walk)
    {
        if(*walk == object)
        {
            *walk = object->hashNext;
            break;
        }
        walk = &((*walk)->hashNext);
    }
}


//--------------------------------------------------------------------------------------------------------------------

void TextureDictionary::insert(TextureObject *object)
{
    object->next = TextureObjectChain;
    object->prev = NULL;
    if(TextureObjectChain)
        TextureObjectChain->prev = object;
    TextureObjectChain = object;

    if(object->mTextureKey)
    {
        U32 key = HashPointer(object->mTextureKey) % smHashTableSize;

        object->hashNext = smTable[key];
        smTable[key] = object;
    }
}

//--------------------------------------------------------------------------------------------------------------------

void TextureDictionary::destroy()
{
    while(TextureObjectChain)
        TextureManager::freeTexture(TextureObjectChain);
    delete[] smTable;
}
