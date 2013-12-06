
#include "TextObject.h"
#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "io/bitStream.h"
#include "string/stringBuffer.h"
#include "2d/assets/ImageAsset.h"
#include "memory/frameAllocator.h"

U32 getStringElementCount( const char* inString );
const char* getStringElement( const char* inString, const U32 index );
bool getInitialUpdate( void ) { return true; }

//-----------------------------------------------------------------------------
/// Table lookup for the text alignment.
//-----------------------------------------------------------------------------
static EnumTable::Enums textAlignLookup[] =
{
  { TextObject::LEFT,    "Left" },
  { TextObject::CENTER,  "Center" },
  { TextObject::RIGHT,   "Right" },
  { TextObject::JUSTIFY, "Justify" },
};

static EnumTable textAlignTable(sizeof(textAlignLookup) /  sizeof(EnumTable::Enums), &textAlignLookup[0]);

//-----------------------------------------------------------------------------
// Console Methods
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

ConsoleMethod(TextObject, setText, void, 3, 3,   "(text) - Set the text to render.\n")
{
  object->setText(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethod( TextObject, addFontSize, bool, 3, 3, "(S32 fontSize)\n"
  "Adds a font size to the text object. Additional font sizes can make the font "
  "look better at more display sizes and resolutions."
  "@param fontSize The font size to add to the text object.\n"
  "@return Whether or not the creation of the font was successful." )
{
  return object->addFontSize( dAtoi( argv[2] ) );
}

//------------------------------------------------------------------------------

ConsoleMethod( TextObject, removeFontSize, void, 3, 3, "(S32 fontSize)\n"
  "Removes a font from the text object.\n"
  "@param fontSize The font size to remove from the text object.\n"
  "@return No return value." )
{
  object->removeFontSize( dAtoi( argv[2] ) );
}

//------------------------------------------------------------------------------

ConsoleMethod( TextObject, removeAllFontSizes, void, 2, 2, "()\n"
  "Removes all of the font sizes from the text object. Effectively, "
  "all fonts will be removed, so, nothing will be rendered until a font size "
  "is added.\n"
  "@return No return value." )
{
  object->removeAllFontSizes();
}

//------------------------------------------------------------------------------

ConsoleMethod( TextObject, addAutoFontSize, bool, 5, 5, "(S32 screenHeight, F32 cameraHeight, F32 characterHeight)\n"
  "Adds a font size based on a screen height, camera height, and character height. The resulting font "
  "size is automatically calculated.\n"
  "@param screenHeight The height of the screen to generate the font for.\n"
  "@param cameraHeight The height of the camera to generate the font for.\n"
  "@param characterHeight The height of the text in world units to generate the size for.\n"
  "@return Whether or not the font was created successfully." )
{
  S32 screenHeight = dAtoi( argv[2] );
  F32 cameraHeight = dAtof( argv[3] );
  F32 characterHeight = dAtof( argv[4] );

  // [neo, 5/7/2007 - #3001]
  // Make sure camera size, etc are valid or addFontSize() will do nasty things and crash
  // as well as the obv div by zero.
  if( cameraHeight > 0 && screenHeight > 0 && characterHeight > 0 )
    return object->addFontSize( S32( characterHeight / cameraHeight * screenHeight ) );

  Con::errorf( "TextObject::addAutoFontSize() - invalid screenHeight, cameraHeight or characterHeight" );

  return false;
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(TextObject);

//-----------------------------------------------------------------------------

TextObject::TextObject(): mConsoleBuffer( NULL ),
  mFontName( NULL ),
  mLineHeight( 12 ),
  mAspectRatio( 1.0f ),
  mWordWrap( false ),
  mClipText( true ),
  mLineSpacing( 0.0f ),
  mCharacterSpacing( 0.0f ),
  mAutoSize( true ),
  mRenderCursor( false ),
  mCharacterBounds( NULL ),
  mCharactersDirty( true ),
  mTextAlignment( LEFT ),
  mHighlightBlock( -1 ),
  mCursorPos( 0 ),
  mCursorColor( 0.0f, 0.0f, 0.0f, 1.0f ),
  mCursorFadeDelay( 0.0f ),
  mShowCursorAtEndOfLine( true ),
  mAlpha(1.0f),
  mFilter(false),
  mIntegerPrecision(false),
  mNoUnicode(false),
  mEditing( false )
{
  VECTOR_SET_ASSOCIATION( mFonts );
  setText( "" );

  Vector2 size(10,10); //iTorque
  Parent::setSize(size);

  // Use a static body by default.
  mBodyDefinition.type = b2_staticBody;

}

//-----------------------------------------------------------------------------

TextObject::~TextObject()
{
  //Luma:	make sure to kill the fonts on object destruction
  while(mFonts.size())
  {
    mFonts.last().font.purge();
    mFonts.decrement();
  }

  SAFE_DELETE_ARRAY( mCharacterBounds );
}

//-----------------------------------------------------------------------------

void TextObject::initPersistFields()
{    
  // Call parent.
  Parent::initPersistFields();

  addProtectedField( "text", TypeString, Offset( mConsoleBuffer, TextObject ), setText, getText,
    "The text that is displayed by the object." );

  addProtectedField( "font", TypeString, Offset( mFontName, TextObject ), setFont, getFont,
    "The name of the font to render the text with." );
  addProtectedField( "wordWrap", TypeBool, Offset( mWordWrap, TextObject ), setWordWrap, defaultProtectedGetFn,
    "Whether or not text should automatically wrap to the next line." );
  addProtectedField( "hideOverflow", TypeBool, Offset( mClipText, TextObject ), defaultProtectedSetFn, defaultProtectedGetFn,
    "Whether or not text outside the object bounds should be hidden." );
  addProtectedField( "textAlign", TypeEnum, Offset( mTextAlignment, TextObject ), setTextAlign, defaultProtectedGetFn, 1, &textAlignTable,
    "The alignment of the text within the object bounds.<br>"
    "Possible Values<br>"
    " -Left: Each line begins at the left bounds of the object.<br>"
    " -Right: Each line ends at the right bounds of the object.<br>"
    " -Center: Each line of text is centered within the object bounds.<br>"
    " -Justify: Text on complete lines is spaced out to fill the object bounds.<br>" );
  addProtectedField( "lineHeight", TypeF32, Offset( mLineHeight, TextObject ), setLineHeight, defaultProtectedGetFn,
    "The height, in world units, of each line. Normally this would be specified by the font "
    "size, but the text object is designed to scale with resolution changes, so this parameter "
    "becomes necessary." );
  addProtectedField( "aspectRatio", TypeF32, Offset( mAspectRatio, TextObject ), setAspectRatio, defaultProtectedGetFn,
    "The ratio of a characters width to its height." );
  addProtectedField( "lineSpacing", TypeF32, Offset( mLineSpacing, TextObject ), setLineSpacing, defaultProtectedGetFn,
    "The amount of extra space to include between each line." );
  addProtectedField( "characterSpacing", TypeF32, Offset( mCharacterSpacing, TextObject ), setCharacterSpacing, defaultProtectedGetFn,
    "The amount of extra space to include between each character." );
  addProtectedField( "autoSize", TypeBool, Offset( mAutoSize, TextObject ), defaultProtectedSetFn, defaultProtectedGetFn,
    "Whether or not the object should be scaled when the text is changed." );
  addProtectedField( "fontSizes", TypeF32Vector, Offset( mFontSizes, TextObject ), setFontSizes, getFontSizes,
    "Space separated list of font sizes the text object should use." );

  addField( "filter", TypeBool, Offset( mFilter, TextObject), "Whether or not the font is filtered" );
  addField( "integerPrecision", TypeBool, Offset( mIntegerPrecision, TextObject), "Whether or not the font destination is clamped to integer values" );

  //Luma:	allow text to not use Unicode
  addField( "noUnicode", TypeBool, Offset( mNoUnicode, TextObject), "Whether or not the text doesn't need to support Unicode conversions (ie. know that it is just numbers, etc.)" );

}

//-----------------------------------------------------------------------------

bool TextObject::onAdd()
{
  // Call Parent.
  if(!Parent::onAdd())
    return false;

  // Return Okay.
  return true;
}

//-----------------------------------------------------------------------------

void TextObject::onRemove()
{
  // Call Parent.
  Parent::onRemove();
}

//------------------------------------------------------------------------------

void TextObject::copyTo(SimObject* object)
{
  // Fetch font object.
  TextObject* pFontObject = dynamic_cast<TextObject*>(object);

  // Sanity.
  AssertFatal(pFontObject != NULL, "TextObject::copyTo() - Object is not the correct type.");

  // Call parent.
  Parent::copyTo(object);
}

//------------------------------------------------------------------------------

void TextObject::scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
  // Create a default render request.
  Scene::createDefaultRenderRequest( pSceneRenderQueue, this );
}

//------------------------------------------------------------------------------

void TextObject::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
  renderObject();
}



//------------------------------------------------------------------------------

void TextObject::renderObject()
{
  // Fetch scene windows.
  SimSet& sceneWindows = getScene()->getAttachedSceneWindows();

  // Calculate pixel size of the object.
  S32 pixelSize = 0;
  SceneWindow* sceneWindow = (SceneWindow*)sceneWindows.at(0);

  // This is the size of the world over the pixel size of the scene window.
  F32 ratioY = sceneWindow->getCameraWindowScale().y;
  pixelSize = (S32)( ( mLineHeight / ratioY ) + 0.5f );

  // Get the font to render with.
  GFont* font = getFont( pixelSize ).font;

  // Set blending.
  setBlendOptions();

  // Rotate the view based on the object's rotation.
  //IPHONE WARNING: tilelayer 2 deep in the projection matrix is all that is allowed
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();

  // Translate to (0, 0), rotate, and translate back.

  Vector2 pos = getRenderPosition();
  glTranslatef( pos.x, pos.y, 0.0f );
  glRotatef( getAngle(), 0.0f, 0.0f, 1.0f );
  glTranslatef( -pos.x, -pos.y, 0.0f );

  //Invert texture 
  glScalef(1.0f, -1.0f, 1.0f); 

  // Render the text.
  if( mText.length() > 0 )
    renderText( font );

  glPopMatrix();
}


//------------------------------------------------------------------------------

void TextObject::renderText( GFont* font )
{
  // Grab some object info.
  RectF worldRect = getAABBRectangle();
  Vector2 sizeRatio = getSizeRatio( font );
#ifndef TORQUE_OS_IPHONE

  // Get info for all the characters to make sure the necessary textures are
  // properly generated prior to the glBegin call.
  for( S32 i = 0; i < (S32)mText.length(); i++ )
  {
    const UTF16 c = mText.getChar( i );
    if(font->isValidChar( c ) )
      const PlatformFont::CharInfo& ci = font->getCharInfo( c );
  }
#endif
  // The last used texture is cached so we don't do unnecessary texture
  // swaps (Expensive).
  TextureObject* lastTexture = NULL;

  // Set up GL.
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Render!
#ifndef TORQUE_OS_IPHONE
  glBegin(GL_QUADS);
#endif
  for( S32 i = 0; i < (S32)mText.length(); i++ )
  {
    const UTF16 c = mText.getChar( i );

    // Grab the character bounds.
    const RectF& charBounds = getCharacterBounds( i );

    // Draw a solid quad if the character is in the highlight block.
    if( ( i >= mCursorPos ) && ( i < mHighlightBlock ) && ( c != dT( '\n' ) ) )
    {
#ifndef TORQUE_OS_IPHONE
      // End the render batch since we need to disable texturing.
      glEnd();
#endif

      // Get the area.
      RectF bounds = charBounds;
      // Account for character spacing.
      bounds.extent.x += mCharacterSpacing * sizeRatio.x;

      // Don't render outside the object bounds.
      if( mClipText )
        bounds.intersect( worldRect );

      if( bounds.isValidRect() )
      {
        // Grab rectangle corners.
        Point2F upperLeft = bounds.point;
        Point2F upperRight = Point2F( bounds.point.x + bounds.extent.x, bounds.point.y );
        Point2F lowerLeft = Point2F( bounds.point.x, bounds.point.y + bounds.extent.y );
        Point2F lowerRight = bounds.point + bounds.extent;

        if(mIntegerPrecision)
        {
          //round the locations of the text to be the nearest pixels
          upperLeft.round();
          upperRight.round();
          lowerLeft.round();
          lowerRight.round();
        }

        // Draw the black rect.
        glDisable(GL_TEXTURE_2D);

        //set alpha of the text
        glColor4f( 0.0f, 0.0f, 0.0f, mAlpha );

#ifdef TORQUE_OS_IPHONE
        //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
        dglDrawTextureQuadiPhone(upperLeft.x, upperLeft.y, upperRight.x, upperRight.y, lowerRight.x, lowerRight.y, lowerLeft.x, lowerLeft.y, 
          0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

        // Re-enable texturing, but set the blend color to white.
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
        glEnable(GL_TEXTURE_2D);
#else
        glBegin( GL_QUADS );
        glVertex2fv( (GLfloat*)(&upperLeft) );
        glVertex2fv( (GLfloat*)(&upperRight) );
        glVertex2fv( (GLfloat*)(&lowerRight) );
        glVertex2fv( (GLfloat*)(&lowerLeft) );
        glEnd();

        // Re-enable texturing, but set the blend color to white.
        //set the alpha of the text
        glColor4f( 0.0f, 0.0f, 0.0f, mAlpha );

        glEnable(GL_TEXTURE_2D);
        glBegin( GL_QUADS );
#endif
      }
    }
    else if( i == mHighlightBlock )
    {
      // End of the highlight block. Reset the blend color.
#ifdef TORQUE_OS_IPHONE
      glColor4f( mBlendColor.red, mBlendColor.green, mBlendColor.blue, mBlendColor.alpha );
#else
      glEnd();
      glColor4fv( (GLfloat*)&mBlendColor );
      glBegin( GL_QUADS );
#endif
    }

    // Make sure we can render this character.
    if( !isRenderable( c ) )
      continue;

    // Grab the character info.
    const PlatformFont::CharInfo& ci = font->getCharInfo( c );

    // Swap the texture if necessary.
    TextureObject* newTexture = font->getTextureHandle( ci.bitmapIndex );
    if( newTexture != lastTexture )
    {
#ifndef TORQUE_OS_IPHONE
      glEnd();
#endif
      glBindTexture(GL_TEXTURE_2D, newTexture->getGLTextureName());

      //set proper texture filtering on the text object (linear or nearest)
      if(mFilter)
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      else
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }

      lastTexture = newTexture;
#ifndef TORQUE_OS_IPHONE
      glBegin(GL_QUADS);
#endif
    }

    // The position of the character is the render position, plus various
    // offsets, depending on the character.
    Vector2 charPosition = charBounds.point;
    charPosition.x += ci.xOrigin * sizeRatio.x;
    charPosition.y += ( (F32)font->getBaseline() - ci.yOrigin ) * sizeRatio.y;

    // The size of the character is dependent on the previously computed
    // size ratio.
    Vector2 charSize;
    charSize.x = ci.width * sizeRatio.x;
    charSize.y = ci.height * sizeRatio.y;

    // Grab the bounding rectangle.
    RectF bounds = RectF( charPosition.x, charPosition.y, charSize.x, charSize.y );

    // Unless we are rendering beyond, make sure the character is within the
    // object bounds.
    if( mClipText )
    {
      bounds.intersect( worldRect );
      if( !bounds.isValidRect() )
        continue;
    }

    // The texture scale is based on the amount the character was clipped.
    // It keeps the textures from squishing in the case that the character
    // was clipped.
    Vector2 texScale;
    texScale.x = bounds.extent.x / charSize.x;
    texScale.y = bounds.extent.y / charSize.y;

    Vector2 texOffset = Vector2::getZero();
    if( mIsEqual( worldRect.point.x, bounds.point.x ) )
    {
      texOffset.x = 1.0f - texScale.x;
      texScale.x = 1.0f;
    }

    Point2F upperLeft = bounds.point;
    Point2F upperRight = Point2F( bounds.point.x + bounds.extent.x, bounds.point.y );
    Point2F lowerLeft = Vector2( bounds.point.x, bounds.point.y + bounds.extent.y );
    Point2F lowerRight = bounds.point + bounds.extent;

    if(mIntegerPrecision)
    {
      //round the locations of the text to be at the nearest pixels
      upperLeft.round();
      upperRight.round();
      lowerLeft.round();
      lowerRight.round();
    }

    // Texture coords.
    F32 texLeft = ( F32( ci.xOffset ) + ( ci.width * texOffset.x ) ) / F32( lastTexture->getTextureWidth() );
    F32 texRight = F32( ci.xOffset + ( ci.width * texScale.x ) ) / F32( lastTexture->getTextureWidth() );
    F32 texTop = F32( ci.yOffset ) / F32( lastTexture->getTextureHeight() );
    F32 texBottom = F32( ci.yOffset + ( ci.height * texScale.y ) ) / F32( lastTexture->getTextureHeight() );


#ifdef TORQUE_OS_IPHONE
    //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
    dglDrawTextureQuadiPhone(lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y, upperLeft.x, upperLeft.y, upperRight.x, upperRight.y, 
      texLeft, texBottom, texRight, texBottom, texLeft, texTop, texRight, texTop);
#else

    glTexCoord2f( texLeft, texTop );
    glVertex2fv( (GLfloat*)(&upperLeft) );

    glTexCoord2f( texRight, texTop );
    glVertex2fv( (GLfloat*)(&upperRight) );

    glTexCoord2f( texRight, texBottom );
    glVertex2fv( (GLfloat*)(&lowerRight) );

    glTexCoord2f( texLeft, texBottom );
    glVertex2fv( (GLfloat*)(&lowerLeft) );
#endif
  }
#ifndef TORQUE_OS_IPHONE
  glEnd();
#endif
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}



//------------------------------------------------------------------------------

bool TextObject::setFont( const char* fontName )
{
  // Set the font name.
  mFontName = StringTable->insert( fontName );

  // Grab a list of all the font heights.
  S32 sizeCount = mFonts.size();

  // If their are no sizes, we can't generate any fonts.
  if( sizeCount < 1 )
    return false;

  // Save off the old font sizes for recreation.
  FrameTemp<S32> fontSizes( sizeCount );
  for( S32 i = 0; i < sizeCount; i++ )
    fontSizes[i] = mFonts[i].height;

  // Delete the old font references.
  mFonts.clear();

  // And regenerate the new fonts at the same sizes.
  bool result = false;
  for( S32 i = 0; i < sizeCount; i++ )
  {
    // If we create even one successfully, return true.
    if( addFontSize( fontSizes[i] ) )
      result = true;
  }

  // Recalculate character and object bounds.
  setCharactersDirty();
  if( mAutoSize )
    autoSize();

  // Reset the font name if we failed.
  if( !result )
    mFontName = StringTable->insert( "" );

  return result;
}

//------------------------------------------------------------------------------

bool TextObject::addFontSize( S32 fontSize )
{
  // Can't really do anything if the font name hasn't been set yet.
  if( !mFontName || !*mFontName )
  {
    Con::warnf( "TextObject::addFontSize - Unable to create font at size %d. The font name hasn't been set yet.", fontSize );
    return false;
  }

  // Make sure it doesn't already exist.
  for( Vector<Font>::const_iterator iter = mFonts.begin(); iter != mFonts.end(); iter++ )
  {
    if( iter->height == fontSize )
    {
      Con::warnf( "TextObject::addFontSize - Unable to create font at size %d. The font size already exists.", fontSize );
      return false;
    }
  }

  // Create the font.
  Font newFont;
  if( !createFont( mFontName, fontSize, newFont ) )
  {
    Con::warnf( "TextObject::addFontSize - Unable to create font %s at size %d. Font generation failed.", mFontName, fontSize );
    return false;
  }

  // Make sure default characters are in the cache.
  for( UTF16 i = 32; i < 128; i++ )
    newFont.font->getCharInfo( i );

  // Allocate space, and construct the new object since Resources require
  // construction.
  mFonts.increment();
  constructInPlace( &mFonts.last() );
  mFonts.last() = newFont;

  return true;
}

//------------------------------------------------------------------------------

void TextObject::removeFontSize( S32 fontSize )
{
  // Find the font size.
  for( Vector<Font>::iterator iter = mFonts.begin(); iter != mFonts.end(); iter++ )
  {
    if( iter->height == fontSize )
    {
      // Erase. No need to keep the vector sorted.
      mFonts.erase_fast( iter );
      return;
    }
  }

  Con::warnf( "TextObject::removeFontSize - Unable to remove font size %d. This size doesn't exist.", fontSize );
}

//------------------------------------------------------------------------------

void TextObject::removeAllFontSizes()
{
  mFonts.clear();
}

//------------------------------------------------------------------------------

const TextObject::Font& TextObject::getFont( S32 pixelSize ) const
{
  AssertFatal( !mFonts.empty(), "TextObject::getFont - The object has no fonts" );

  if( pixelSize <= 0 )
    return mFonts[0];

  // Find the smallest font that is greater than the pixel size.
  S32 index = 0;
  S32 size = mFonts[index].height;
  for( S32 i = 1; i < mFonts.size(); i++ )
  {
    S32 newSize = mFonts[i].height;

    // If the new size is between the pixel size and old size or
    // if the old size is smaller than the pixel size and the new size is
    // greater than the old size.
    if( ( ( newSize >= pixelSize ) && ( newSize < size ) ) ||
      ( ( size < pixelSize ) && ( newSize > size ) ) )
    {
      index = i;
      size = newSize;
    }
  }

  return mFonts[index];
}

//------------------------------------------------------------------------------

void TextObject::autoSize()
{
  // Gotta have a font and auto sizing doesn't work with word wrapping.
  if( !hasFont() || mWordWrap )
    return;

  GFont* font = getFont().font;

  // Store the old size for positioning.
  Vector2 oldSize = getSize();

  // Calculate the size.
  Vector2 size;
  // The width is the width of the longest line, times the aspect ratio,
  // times the size ratio.
  size.x = getLineWidth( font ) * mAspectRatio * getSizeRatio( font ).y;

  // The height is the number of lines times the height of each line.
  size.y = getLineCount() * ( mLineHeight + mLineSpacing );

  // Position based on alignment.
  Vector2 newPosition = getPosition();
  switch( mTextAlignment )
  {
  case LEFT:
  case JUSTIFY:
    // Move by half the amount the size changed down and to the right.
    newPosition += ( size - oldSize ) * 0.5f;
    break;

  case RIGHT:
    // Move by half the amount the size changed down and to the left.
    newPosition.x -= ( size.x - oldSize.x ) * 0.5f;
    newPosition.y += ( size.y - oldSize.y ) * 0.5f;
    break;

  case CENTER:
    // Move by half the amount the size changed down.
    newPosition.y += ( size.y - oldSize.y ) * 0.5f;
    break;
  };

  // Update size.
  Parent::setSize( size );
  setPosition( newPosition );
  updateSpatialConfig();

  // Reset auto size flag. setSize turned it off.
  mAutoSize = true;
}

//------------------------------------------------------------------------------

F32 TextObject::getLineWidth( GFont* font ) const
{
  U32 width = 0;
  U32 longest = 0;
  for( S32 i = 0; ; i++ )
  {
    // The end of a line is either the null terminator or the new line char.
    if( !mText.getChar( i ) || ( mText.getChar( i ) == dT( '\n' ) ) )
    {
      // Found a new longest line.
      if( width > longest )
        longest = width;

      // Reset the width.
      width = 0;

      // Break if this is the last line.
      if( !mText.getChar( i ) )
        break;

      continue;
    }

    UTF16 c = mText.getChar( i );
    if( c == '\t' )
      width += font->getCharXIncrement( ' ' ) * GFont::TabWidthInSpaces;
    else
      width += font->getCharXIncrement( c );

  }

  return (F32)longest;
}

//------------------------------------------------------------------------------

Vector2 TextObject::getSizeRatio( GFont* font ) const
{
  F32 lineHeight = getLineHeight();

  // The size ratio stores the amount to scale the font in each direction.
  Vector2 sizeRatio;
  sizeRatio.y = lineHeight / (F32)font->getHeight();
  sizeRatio.x = mAspectRatio * sizeRatio.y;

  return sizeRatio;
}


//------------------------------------------------------------------------------

void TextObject::setText( const char* text )
{
  // Convert to a string buffer and set.

  //Luma:	allow text to not use Unicode
  setText(StringBuffer(text, mNoUnicode));
}


//------------------------------------------------------------------------------

void TextObject::setText( const StringBuffer& text )
{
  // Clean up the old character bounds.
  SAFE_DELETE_ARRAY( mCharacterBounds );

  // Allocate space for the new character bounds.
  if(mCharacterBounds)
    delete[] mCharacterBounds;

  S32 len = text.length();
  mCharacterBounds = new RectF[len + 2];

  // Set the text.
  mText.set( &text );

  // Force recalculation of character positions.
  setCharactersDirty();

  // Auto size, if applicable.
  if( hasFont() && !mWordWrap )
    autoSize();

  // Set the cursor position to the end of the string.
  setCursorPosition( len );
  showCursorAtEndOfLine( false );
}


//------------------------------------------------------------------------------


void TextObject::insertText( UTF16* text )
{
  mText.insert( mCursorPos, text );

  // Save off the cursor pos because setText will change it.
  S32 oldCursorPos = mCursorPos;

  // Set the new text.
  setText( mText );

  // Move the cursor to the end of the inserted text.
  mCursorPos = oldCursorPos + dStrlen( (char *)text );
}


//------------------------------------------------------------------------------

bool TextObject::createFont( const char* fontFace, S32 fontSize, Font& font )
{
  // Get the font cache directory.
  const char* fontCacheDirectory = Con::getVariable( "$GUI::fontCacheDirectory" );

  // Create the font.
  Resource<GFont> fontResource = GFont::create( fontFace, fontSize, fontCacheDirectory );

  // Check for success.
  if( fontResource.isNull() )
    return false;

  font.font = fontResource;
  font.height = fontSize;
  return true;
}

//------------------------------------------------------------------------------

S32 TextObject::getLineCount() const
{
  // Always have at least 1 line.
  S32 lineCount = 1;
  for( S32 i = 0; i < (S32)mText.length(); i++ )
  {
    // Increment the count on every new line.
    if( mText.getChar( i ) == dT( '\n' ) )
      lineCount++;
  }

  return lineCount;
}


//------------------------------------------------------------------------------

F32 TextObject::getLineHeight() const
{
  // Determine the line height. If mLineHeight is 0, then the line height is
  // whatever fills the object bounds.
  F32 lineHeight = mLineHeight;
  if( lineHeight <= 0 )
    lineHeight = getSize().y / getLineCount();

  return lineHeight;
}

//------------------------------------------------------------------------------

void TextObject::setCursorPosition( Vector2 worldPos )
{
  setCursorPosition( getCharacterPosition( worldPos ) );
}

//------------------------------------------------------------------------------

void TextObject::setCursorPosition( S32 pos )
{
  setHighlightBlock( pos, -1 );
}

//------------------------------------------------------------------------------

S32 TextObject::getCharacterPosition( Vector2 worldPos )
{
  // Rotate the world position relative to the object.
  worldPos.rotate( getRenderPosition(), -getRenderAngle() ); //getRenderRotation()

  // Iterate through the characters, and find the one at worldPos.
  for( S32 i = 0; i < (S32)mText.length(); i++ )
  {
    RectF bounds = getCharacterBounds( i );
    if( ( bounds.point.x <= worldPos.x ) && ( worldPos.x <= ( bounds.point.x + bounds.extent.x ) ) &&
      ( bounds.point.y <= worldPos.y ) && ( worldPos.y <= ( bounds.point.y + bounds.extent.y ) ) )
    {
      if( ( worldPos.x - bounds.point.x ) > ( bounds.extent.x / 2.0f ) )
        return i + 1;
      else
        return i;
    }
  }

  // Well, the position is not in any character. What we want now is the
  // position at the beginning or end of the line at the y position.

  // Position is above the object.
  F32 top = getRenderPosition().y - getHalfSize().y;
  F32 bottom = getRenderPosition().y + getHalfSize().y;
  if( worldPos.y <= top )
    return 0;

  // Position is below the object.
  if( worldPos.y >= bottom )
    return mText.length();

  // Find the first character on the line.
  for( S32 i = 0; i < (S32)mText.length(); i++ )
  {
    const RectF cBounds = getCharacterBounds( i );
    if( ( cBounds.point.y <= worldPos.y ) && ( worldPos.y <= ( cBounds.point.y + cBounds.extent.y ) ) )
    {
      S32 index, line = getLineNumber(i);

      // if the position is to the right of the object, then we want the
      // first character on this line. Otherwise, we want the last one on
      // this line.
      if( worldPos.x <= cBounds.point.x )
        index = getLineIndex(line);
      else
        index = getLineIndex(line + 1) - 1;

      return index;
    }
  }

  // The only time we should ever get here is if the position is within the
  // bounds of the object but past the last line.
  return mText.length();
}

//-----------------------------------------------------------------------------

void TextObject::setHighlightBlock( S32 startPos, S32 endPos )
{
  // The start position must be less than the end position. Fix if necessary.
  if( ( endPos >= 0 ) && ( endPos < startPos ) )
  {
    S32 temp = endPos;
    endPos = startPos;
    startPos = temp;
  }

  // Bind to the text bounds, just in case.
  if( startPos < 0 )
    startPos = 0;
  else if( startPos > (S32)mText.length() )
    startPos = mText.length();

  if( endPos > (S32)mText.length() )
    endPos = mText.length();

  // If they're the same, nothing should be highlighted.
  if( endPos == startPos )
  {
    mCursorPos = startPos;
    mHighlightBlock = -1;
  }
  else
  {
    mCursorPos = startPos;
    mHighlightBlock = endPos;
  }
}

//-----------------------------------------------------------------------------

S32 TextObject::getLineIndex( S32 lineNumber )
{
  // Line number 1 starts at index 0.
  if( lineNumber <= 1 )
    return 0;

  // Grab the y position of the first line.
  F32 yPos = getCharacterBounds( 0 ).point.y;
  S32 lineCount = 1;
  for( S32 i = 1; i < (S32)mText.length(); i++ )
  {
    F32 newYPos = getCharacterBounds( i ).point.y;
    if( mNotEqual( newYPos, yPos ) )
    {
      yPos = newYPos;
      lineCount++;
    }

    // If we reached the requested line, then the current index is the
    // first character in that line.
    if( lineCount == lineNumber )
      return i;
  }

  // Only way to get here is if the requested line number is greater than
  // the number of lines in the string.
  return mText.length();
}

//-----------------------------------------------------------------------------

S32 TextObject::getLineNumber( S32 index )
{
  // Grab the y position of the first line.
  F32 yPos = getCharacterBounds( 0 ).point.y;

  // The first line is line 1.
  S32 lineCount = 1;

  // Iterate from the start of the text to the requested index.
  for( S32 i = 1; i <= index; i++ )
  {
    // Each time the y position of a character changes, we're at a new line.
    F32 newYPos = getCharacterBounds( i ).point.y;
    if( mNotEqual( newYPos, yPos ) )
    {
      yPos = newYPos;
      lineCount++;
    }
  }
  return lineCount;
}

//-----------------------------------------------------------------------------

RectF TextObject::getCharacterBounds( S32 index )
{
  // If the character bounds are dirty, recompute.
  if( mCharactersDirty )
    calculateCharacterBounds();

  return RectF( mCharacterBounds[index].point + Point2F( getRenderPosition().x, getRenderPosition().y ),  mCharacterBounds[index].extent );
}

//-----------------------------------------------------------------------------

void TextObject::calculateCharacterBounds()
{
  // Keep track of whether or not this function is currently executing. Since
  // this gets called from several different places, incorrectly structured
  // code could cause infinite recursion. This assert should make that problem
  // easy to track down.
  static bool calculatingBounds = false;
  AssertFatal( !calculatingBounds, "TextObject::calculateCharacterBounds - This function cannot be called recursively." );
  calculatingBounds = true;

  GFont* font = getFont().font;

  F32 lineHeight = getLineHeight();
  Vector2 sizeRatio = getSizeRatio( font );

  // The position at which rendering starts. This is saved so new lines know
  // the x position at which to start rendering.
  const Vector2 startPosition = -getHalfSize();

  // The end position is the bottom right of the bounds and the point outside
  // of which nothing should be rendered.
  const Vector2 endPosition = getHalfSize();

  // The upper left of the current render position.
  Vector2 renderPosition = startPosition;

  // Specifies whether or not we are at the first character after a line
  // wrap. In wrap modes, we don't want to render spaces at the beginning
  // of wrapped lines.
  bool wrappedLine = false;

  // The width of the next wrap object. In the case of character wrap, this
  // will be the next character width. In the case of word wrap, this will
  // be the next word width.
  F32 nextWrapWidth = 0.0f;

  S32 lineStart = 0;

  bool firstWord = true;

  // Loop through each character, calculating the position at each step.
  for( U32 i = 0; i < mText.length(); i++ )
  {
    const UTF16 c = mText.getChar( i );

    // Grab the bounding rectangle.
    mCharacterBounds[i] = RectF( renderPosition.x, renderPosition.y, 0, lineHeight );

    if( isWhiteSpace( c ) )
      firstWord = false;

    // Deal with special characters.
    // Tab.
    if ( c == dT('\t') ) 
    {
      const PlatformFont::CharInfo &space = font->getCharInfo( ' ' );
      renderPosition.x += space.xIncrement * GFont::TabWidthInSpaces * sizeRatio.x;
      mCharacterBounds[i].extent.x = renderPosition.x;
      continue;
    }

    // New line.
    else if( c == dT('\n') )
    {
      alignText( lineStart, i );
      lineStart = i + 1;
      renderPosition.y += lineHeight + ( mLineSpacing * sizeRatio.y );
      renderPosition.x = startPosition.x;
      firstWord = true;
      continue;
    }

    // Grab the character info.
    const PlatformFont::CharInfo &ci = font->getCharInfo( c );

    // Update the bounding rectangle.
    mCharacterBounds[i].extent.x = ci.xIncrement * sizeRatio.x ;


    if( mWordWrap && isWhiteSpace( mText.getChar( i ) ) )
    {
      StringBuffer text = mText.substring( i + 1, mText.length() - i - 1 );
      nextWrapWidth = ( getNextWrapWidth( text ) + font->getCharInfo( dT( ' ' ) ).xIncrement ) * sizeRatio.x;
      if( ( renderPosition.x + nextWrapWidth ) > endPosition.x )
      {
        alignText( lineStart, i );
        lineStart = i + 1;

        renderPosition.y += lineHeight + ( mLineSpacing * sizeRatio.y );
        renderPosition.x = startPosition.x;
        wrappedLine = true;
        firstWord = true;
      }

      else
        renderPosition.x += ( ci.xIncrement + mCharacterSpacing ) * sizeRatio.x;
    }

    // Don't print spaces at the beginning of wrapped lines.
    else if( wrappedLine && ( c == dT( ' ' ) ) && mWordWrap )
    {
      // Don't change render position.
    }

    // If we're on the first word of a line and passing the end position, we
    // need to wrap in the middle of the word.
    else if( mWordWrap && firstWord && ( i + 1 < mText.length() ) && !isWhiteSpace( mText.getChar( i + 1 ) ) )
    {
      const PlatformFont::CharInfo &nextInfo = font->getCharInfo( mText.getChar( i + 1 ) );

      F32 charWidth = ci.xIncrement * sizeRatio.x;
      F32 nextWidth = nextInfo.xIncrement * sizeRatio.x;
      if( ( renderPosition.x + charWidth + nextWidth ) > endPosition.x )
      {
        alignText( lineStart, i );
        lineStart = i + 1;

        renderPosition.y += lineHeight + ( mLineSpacing * sizeRatio.y );
        renderPosition.x = startPosition.x;
        wrappedLine = true;
        firstWord = true;
      }

      else
        renderPosition.x += ( ci.xIncrement + mCharacterSpacing ) * sizeRatio.x;
    }

    else
    {
      wrappedLine = false;
      renderPosition.x += ( ci.xIncrement + mCharacterSpacing ) * sizeRatio.x;
    }
  }

  // Set the position of the last character.
  mCharacterBounds[mText.length()] = RectF( renderPosition.x, renderPosition.y, 0.0f, lineHeight );

  // Align the last line.
  alignText( lineStart, mText.length() + 1 );

  setCharactersDirty( false );
  calculatingBounds = false;
}

//-----------------------------------------------------------------------------

F32 TextObject::getNextWrapWidth( const StringBuffer& text ) const
{
  F32 width = 0.0f;
  S32 i = 0;

  GFont* font = getFont().font;

  // Nothing doing if the first character is a new line.
  if( text.getChar( 0 ) == dT( '\n' ) )
    return width;

  // Find the number of characters until the next whitespace.
  for( i = 0; !isWhiteSpace( text.getChar( i ) ); i++ );

  // Calculate the width.
  width = (F32)font->getStrNWidth( text.getPtr(), i - 1 );

  return width;
}

//-----------------------------------------------------------------------------

void TextObject::alignText( S32 lineStart, S32 lineEnd )
{
  // The bounds are still dirty, since this is called from calculate bounds,
  // but these characters should have their bounds properly computed.
  RectF firstRect = mCharacterBounds[lineStart];
  RectF lastRect = mCharacterBounds[lineEnd - 1];

  F32 lineWidth = getSize().x;
  F32 leftPoint = firstRect.point.x;
  F32 rightPoint = lastRect.point.x + lastRect.extent.x;
  F32 textWidth = rightPoint - leftPoint;

  F32 offset = 0.0f;
  switch( mTextAlignment )
  {
    // Set the start of the line to the left side of the object.
  case LEFT:
    offset = -getHalfSize().x - leftPoint;
    break;

    // Set the end of the line to the right side of the object.
  case RIGHT:
    offset = getHalfSize().x - rightPoint;
    break;

    // Center the line within the object bounds.
  case CENTER:
    offset = ( -getHalfSize().x + ( ( lineWidth - textWidth ) / 2.0f ) ) - leftPoint;
    break;

    // Space out the characters so the line fills the object bounds.
  case JUSTIFY:
    // Don't justify lines with a line break at the end or the last line.
    if( ( lineEnd > (S32)mText.length() ) || !mText.getChar( lineEnd ) || ( mText.getChar( lineEnd ) == dT( '\n' ) ) )
      return;

    // Do nothing if the line is too long.
    if( ( lineWidth - textWidth ) < 0.0f )
      return;

    // Space out characters on full lines.
    {
      S32 spaces = 0;
      for( S32 i = lineStart; i < lineEnd; i++ )
      {
        if( mText.getChar( i ) == dT( ' ' ) )
          spaces++;
      }

      if( spaces < 1 )
      {
        F32 whiteSpace = ( lineWidth - textWidth ) / ( lineEnd - lineStart - 1 );
        F32 offset = whiteSpace;
        for( S32 i = lineStart + 1; i < lineEnd; i++ )
        {
          // Stretch the previous character to fill the empty space.
          mCharacterBounds[i - 1].extent.x += whiteSpace;
          // Move the current character.
          mCharacterBounds[i].point.x += offset;
          offset += whiteSpace;
        }
      }

      else
      {
        F32 whiteSpace = ( lineWidth - textWidth ) / spaces;
        F32 offset = 0.0f;
        for( S32 i = lineStart + 1; i < lineEnd; i++ )
        {
          // Move the current character.
          mCharacterBounds[i].point.x += offset;
          if( mText.getChar( i ) == dT( ' ' ) )
          {
            // Stretch spaces.
            offset += whiteSpace;
            mCharacterBounds[i - 1].extent.x += offset;
          }
        }
      }
    }
    return;
  }

  // Move each character in the line based on the computed offset.
  for( S32 i = lineStart; i <= lineEnd; i++ )
    mCharacterBounds[i].point.x += offset;
}

//-----------------------------------------------------------------------------

bool TextObject::isWhiteSpace( UTF16 c ) const
{
  // Whitespace is space, null terminator, tab, or new line.
  return !c || ( c == dT( ' ' ) ) || ( c == dT( '\t' ) ) || ( c == dT( '\n' ) );
}

//-----------------------------------------------------------------------------

void TextObject::setSize( const Vector2& size )
{
  Vector2 sizeScale = size;
  sizeScale.div( getSize() );

  Parent::setSize( size );

  if( !mEditing )
  {
    mAspectRatio *= sizeScale.x;
    mAspectRatio /= sizeScale.y;
    mLineHeight *= sizeScale.y;
  }

  setCharactersDirty();
}

//-----------------------------------------------------------------------------

const StringBuffer& TextObject::getText() const
{
  return mText;
}

//-----------------------------------------------------------------------------

const char* TextObject::getText( void* obj, const char* data )
{
  const StringBuffer& text = static_cast<TextObject*>( obj )->getText();   
  // we don't need to copy the data to the con return buffer.
  // the purpose of the return buffer is to ensure that we have a buffer that
  // lives past the life of this function.

  // [neo, 5/7/2007]
  // Need this const_cast() nonsense as StringBuffer::getPtr8() is not const!
  //return text.getPtr8();
  return const_cast<StringBuffer&>(text).getPtr8();
}

//-----------------------------------------------------------------------------
// Protected Field Accessors
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

bool TextObject::setFontSizes( void* obj, const char* data )
{
  TextObject* textObject = static_cast<TextObject*>( obj );
  textObject->removeAllFontSizes();

  U32 count = getStringElementCount( data );
  for( U32 i = 0; i < count; i++ )
  {
    S32 size = dAtoi( getStringElement( data, i ) );
    textObject->addFontSize( size );
  }

  return false;
}

//-----------------------------------------------------------------------------

const char* TextObject::getFontSizes( void* obj, const char* data )
{
  TextObject* textObject = static_cast<TextObject*>( obj );

  S32 count = textObject->getFontSizeCount();
  char* sizes = Con::getReturnBuffer( count * 8 );
  sizes[0] = '\0';
  char sizeString[8];

  for( S32 i = 0; i < count; i++ )
  {
    S32 size = textObject->getFontSize( i );
    dSprintf( sizeString, 8, "%s%d", i > 0 ? " " : "", size );
    dStrcat( sizes, sizeString );
  }

  return sizes;
}

//-----------------------------------------------------------------------------

StringTableEntry TextObject::getFontName() const
{
  return mFontName;
}

//-----------------------------------------------------------------------------

void TextObject::setWordWrap( bool wrap )
{
  mWordWrap = wrap;
  setCharactersDirty();

  if( wrap )
    mAutoSize = false;
  else
    autoSize();
}

//----------------------------------------------------------------------------

void TextObject::setLineHeight( F32 height )
{
  if( !mWordWrap && getInitialUpdate() )
  {
    F32 scale = height / mLineHeight;
    Parent::setSize( getSize() * scale );
    updateSpatialConfig();
  }

  mLineHeight = height;
  setCharactersDirty();
}

//-----------------------------------------------------------------------------

void TextObject::setAspectRatio( F32 ar )
{
  if( !mWordWrap && getInitialUpdate() )
  {
    F32 scale = ar / mAspectRatio;
    Parent::setSize( Vector2( getSize().x * scale, getSize().y ) );
    updateSpatialConfig();
  }

  mAspectRatio = ar;
  setCharactersDirty();
}

//-----------------------------------------------------------------------------

bool TextObject::isRenderable( UTF16 c ) const
{
  // Renderable characters are anything valid as determined by the font that
  // has a size and isn't tab or newline.

  GFont* font = getFont().font;

  if( !font || !font->isValidChar( c ) )
    return false;

  if( ( c == dT( '\t' ) ) || ( c == dT( '\n' ) ) )
    return false;

  const PlatformFont::CharInfo& ci = font->getCharInfo( c );
  if( ( ci.width == 0 ) || ( ci.height == 0 ) )
    return false;

  return true;
}

//------------------------------------------------------------------------------
// Get String Element Count.
//------------------------------------------------------------------------------
U32 getStringElementCount( const char* inString )
{
  // Non-whitespace chars.
  static const char* set = " \t\n";

  // End of string.
  if ( *inString == 0 )
    return 0;

  U32 wordCount = 0;
  U8 search = 0;

  // Search String.
  while( *inString )
  {
    // Get string element.
    search = *inString;

    // End of string?
    if ( search == 0 )
      break;

    // Move to next element.
    inString++;

    // Search for seperators.
    for( U32 i = 0; set[i]; i++ )
    {
      // Found one?
      if( search == set[i] )
      {
        // Yes...
        search = 0;
        break;
      }   
    }

    // Found a seperator?
    if ( search == 0 )
      continue;

    // We've found a non-seperator.
    wordCount++;

    // Search for end of non-seperator.
    while( 1 )
    {
      // Get string element.
      search = *inString;

      // End of string?
      if ( search == 0 )
        break;

      // Move to next element.
      inString++;

      // Search for seperators.
      for( U32 i = 0; set[i]; i++ )
      {
        // Found one?
        if( search == set[i] )
        {
          // Yes...
          search = 0;
          break;
        }   
      }

      // Found Seperator?
      if ( search == 0 )
        break;
    }

    // End of string?
    if ( *inString == 0 )
    {
      // Bah!
      break;
    }
  }

  // We've finished.
  return wordCount;
}


//------------------------------------------------------------------------------
// Get String Element.
//------------------------------------------------------------------------------
const char* getStringElement( const char* inString, const U32 index )
{
  // Non-whitespace chars.
  static const char* set = " \t\n";

  U32 wordCount = 0;
  U8 search = 0;
  const char* pWordStart = NULL;

  // End of string?
  if ( *inString != 0 )
  {
    // No, so search string.
    while( *inString )
    {
      // Get string element.
      search = *inString;

      // End of string?
      if ( search == 0 )
        break;

      // Move to next element.
      inString++;

      // Search for seperators.
      for( U32 i = 0; set[i]; i++ )
      {
        // Found one?
        if( search == set[i] )
        {
          // Yes...
          search = 0;
          break;
        }   
      }

      // Found a seperator?
      if ( search == 0 )
        continue;

      // Found are word?
      if ( wordCount == index )
      {
        // Yes, so mark it.
        pWordStart = inString-1;
      }

      // We've found a non-seperator.
      wordCount++;

      // Search for end of non-seperator.
      while( 1 )
      {
        // Get string element.
        search = *inString;

        // End of string?
        if ( search == 0 )
          break;

        // Move to next element.
        inString++;

        // Search for seperators.
        for( U32 i = 0; set[i]; i++ )
        {
          // Found one?
          if( search == set[i] )
          {
            // Yes...
            search = 0;
            break;
          }   
        }

        // Found Seperator?
        if ( search == 0 )
          break;
      }

      // Have we found our word?
      if ( pWordStart )
      {
        // Yes, so we've got our word...

        // Result Buffer.
        static char buffer[4096];

        // Calculate word length.
        const U32 length = inString - pWordStart - ((*inString)?1:0);

        // Copy Word.
        dStrncpy( buffer, pWordStart, length);
        buffer[length] = '\0';

        // Return Word.
        return buffer;
      }

      // End of string?
      if ( *inString == 0 )
      {
        // Bah!
        break;
      }
    }
  }

  // Sanity!
  AssertFatal( false, "t2dSceneObject::getStringElement() - Couldn't find specified string element!" );
  // Didn't find it
  return " ";
}   
