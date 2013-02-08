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

#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/editor/guiImageList.h"

GuiImageList::GuiImageList()
{
  VECTOR_SET_ASSOCIATION(mTextures);
  mTextures.clear();
  mUniqueId = 0;
}

U32 GuiImageList::Insert( const char* texturePath , TextureHandle::TextureHandleType type )
{
    // Sanity!
    AssertISV( type != TextureHandle::InvalidTexture, "Invalid texture type." );

  TextureEntry *t = new TextureEntry;

  if ( ! t ) return -1;

  t->TexturePath = StringTable->insert(texturePath);
  if ( *t->TexturePath ) 
  {
    t->Handle = TextureHandle(t->TexturePath, type);

    if ( t->Handle )
    {
      t->Object = (TextureObject *) t->Handle;

      if(t->Object == NULL) 
      { 
        return -1;
      }
      t->id = ++mUniqueId;

      mTextures.push_back( t );

      return t->id;

    }
  }

  // Free Texture Entry.
  delete t;

  // Return Failure.
  return -1;

}


bool GuiImageList::Clear()
{
  while ( mTextures.size() )
    FreeTextureEntry( mTextures[0] );
  mTextures.clear();

  mUniqueId = 0;

  return true;
}

bool GuiImageList::FreeTextureEntry( U32 Index )
{
  U32 Id = IndexFromId( Index );
  if ( Id != -1 )
     return FreeTextureEntry( mTextures[ Id ] );
  else
    return false;
}

bool GuiImageList::FreeTextureEntry( PTextureEntry Entry )
{
  if ( ! Entry )
    return false;

  U32 id = IndexFromId( Entry->id );

  delete Entry;

  mTextures.erase ( id );

  return true;
}

U32 GuiImageList::IndexFromId ( U32 Id )
{
  if ( !mTextures.size() ) return -1;
  Vector<PTextureEntry>::iterator i = mTextures.begin();
  U32 j = 0;
  for ( ; i != mTextures.end(); i++ )
  {
    if ( i )
    {
    if ( (*i)->id == Id )
      return j;
    j++;
    }
  }

  return -1;
}

U32 GuiImageList::IndexFromPath ( const char* Path )
{
  if ( !mTextures.size() ) return -1;
  Vector<PTextureEntry>::iterator i = mTextures.begin();
  for ( ; i != mTextures.end(); i++ )
  {
    if ( dStricmp( Path, (*i)->TexturePath ) == 0 )
      return (*i)->id;
  }

  return -1;
}


void GuiImageList::initPersistFields()
{
  Parent::initPersistFields();
}

ConsoleMethod(GuiImageList, getImage,const char *, 3, 3, "(int index) Get a path to the texture at the specified index\n"
              "@param index The index of the desired image\n"
              "@return The ID of the image or -1 on failure")
{
  return object->GetTexturePath(dAtoi(argv[2]));
}


ConsoleMethod(GuiImageList, clear,bool, 2, 2, "() Clears the imagelist\n"
              "@return Returns true on success, and false otherwise")
{
  return object->Clear();
}

ConsoleMethod(GuiImageList, count,S32, 2, 2, "Gets the number of images in the list\n"
              "@return Return number of images as an integer")
{
  return object->Count();
}


ConsoleMethod(GuiImageList, remove, bool, 3,3, "(index) Removes an image from the list by index\n"
              "@param index The index of the image to remove\n"
              "@return Returns true on success, false otherwise")
{
  return object->FreeTextureEntry( dAtoi(argv[2] ) );
}

ConsoleMethod(GuiImageList, getIndex, S32, 3,3, "(char* path) Retrieves the imageindex of a specified texture in the list\n"
              "@param path Thge path of the image file to retrieve the indexs of\n"
              "@return The index of the image as an integer or -1 for failure")
{
  return object->IndexFromPath( argv[2] );
}


ConsoleMethod(GuiImageList, insert, S32, 3, 3, "(image path) Insert an image into imagelist\n"
              "@param path The path of the image file\n"
              "@return returns the image index or -1 for failure")
{
  return object->Insert( argv[2] );
}



TextureObject *GuiImageList::GetTextureObject( U32 Index )
{
  U32 ItemIndex = IndexFromId(Index);
  if ( ItemIndex != -1 )
    return mTextures[ItemIndex]->Object;
  else
    return NULL;
}
TextureObject *GuiImageList::GetTextureObject( const char* TexturePath )
{
  Vector<PTextureEntry>::iterator i = mTextures.begin();
  for ( ; i != mTextures.end(); i++ )
  {
    if ( dStricmp( TexturePath, (*i)->TexturePath ) == 0 )
      return (*i)->Object;
  }

  return NULL;
}

TextureHandle GuiImageList::GetTextureHandle( U32 Index )
{
  U32 ItemIndex = IndexFromId(Index);
  if ( ItemIndex != -1 )
    return mTextures[ItemIndex]->Handle;
  else
    return NULL;

}
TextureHandle GuiImageList::GetTextureHandle( const char* TexturePath )
{
  Vector<PTextureEntry>::iterator i = mTextures.begin();
  for ( ; i != mTextures.end(); i++ )
  {
    if ( dStricmp( TexturePath, (*i)->TexturePath ) == 0 )
      return (*i)->Handle;
  }

  return NULL;
}


const char *GuiImageList::GetTexturePath( U32 Index )
{
  U32 ItemIndex = IndexFromId(Index);
  if ( ItemIndex != -1 )
    return mTextures[ItemIndex]->TexturePath;
  else
    return "";
}


// Used to derive from GuiControl, but no visual representation was needed
//void GuiImageList::onRender(Point2I offset, const RectI &updateRect)
//{
//  return;
//
//  //make sure we have a parent
//  GuiCanvas *Canvas = getRoot();
//  if (! Canvas)
//    return;
//
//  U32 IconSize = 32;
//
//  RectI IconRect;
//
//  IconRect.point = offset;
//
//  IconRect.extent.x =  32;
//  IconRect.extent.y =  32;
//
//  //debugging, checking insertion by rendering textures in list
//  if( mTextures.size() )
//  {
//    Point2I TexPoint = offset;
//
//    Vector<PTextureEntry>::iterator i = mTextures.begin();
//
//    ColorI color;
//
//    dglGetBitmapModulation(&color);
//
//    dglClearBitmapModulation();
//
//    for ( ; i != mTextures.end(); i++ )
//    {
//      dglDrawBitmap((*i)->Object,TexPoint);
//      TexPoint.y += (*i)->Object->getBitmapHeight();
//    }
//
//    dglSetBitmapModulation(color);
//  }
//}



IMPLEMENT_CONOBJECT(GuiImageList);