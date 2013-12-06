

#ifndef _TEXT_OBJECT_H_
#define _TEXT_OBJECT_H_

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/utility.h"
#endif

//-----------------------------------------------------------------------------

class TextObject : public SceneObject
{
  typedef SceneObject  Parent;

public:
  // Declare Console Object.
  DECLARE_CONOBJECT(TextObject);

  TextObject();
  ~TextObject();

  bool onAdd();
  void onRemove();

  virtual bool canPrepareRender( void ) const                             { return true; }
  virtual bool validRender( void ) const                                  { return true; }
  virtual bool shouldRender( void ) const                                 { return true; }
  virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );
  virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

public:

  /// This specifies how to align text within the bounds of the object.
  enum TextAlign
  {
    LEFT,             ///< Start each line at the left side of the object.
    CENTER,           ///< Center the text within the object's width.
    RIGHT,            ///< End each line at the right side of the object.
    JUSTIFY,          ///< Space characters so that each line fills the object's width.
  };

  /// @name SimObject_Overrides
  /// @{
  static void initPersistFields();
  virtual void copyTo(SimObject* object);
  /// @}

  /// @name t2dSceneObject_Overrides
  /// @{

  /// Redifined here so we can recalculate the character positions when the size is changed.
  virtual void setSize( const Vector2& size );

  /// Performs the actual rendering of the object.
  void renderObject();



  /// @name Font_Management
  /// @{

  /// Wrapper for the font resource. Allows for future expansion to add things
  /// like italic, bold, etc.
  struct Font
  {
    Resource<GFont> font; ///< The font resource.
    S32 height;           ///< The font height.
  };

  /// Sets the font to use for this text object.
  /// @param fontFace The name of the font to set.
  /// @return Whether or not the font was generated successfully.
  virtual bool setFont( const char* fontFace );

  /// Returns whether or not the object has a font to render with.
  bool hasFont() const { return !mFonts.empty(); };

  /// Gets the name of the font being used.
  StringTableEntry getFontName() const;

  /// Gets a pointer to the font being used for this text object.
  /// @param pixelSize The size of the font being requested. The returned
  /// font will be the one with the smallest height greater than this value.
  /// @return The font to use for the given pixel size. If pixelSize is 0,
  /// this will be the first font in the array. If no font is found that is
  /// greater than the requested pixel size, the largest font will be
  /// returned.
  const Font& getFont( S32 pixelSize = 0 ) const;

  /// Adds a font size to the font size list. Multiple sizes are used to make
  /// the text object look good at multiple object sizes and resolutions.
  /// @param fontSize The font size to add.
  /// @return Whether or not the creation of the font was successful.
  bool addFontSize( S32 fontSize );

  /// Removes a font size from the font size list.
  /// @param fontSize The font size to remove.
  void removeFontSize( S32 fontSize );

  /// Removes all font sizes from the font size list.
  void removeAllFontSizes();

  /// Get the number of font sizes the text object has.
  /// @return The number of font sizes.
  S32 getFontSizeCount() const { return mFonts.size(); };

  /// Get the size of a font at the given index.
  /// @param index The index of the font whose height is being retrieved.
  /// @return The size of the font.
  S32 getFontSize( U32 index ) const { if( index < (U32)getFontSizeCount() ) return mFonts[index].height; return 0; };
  /// @}

  /// @name Text_Editing
  /// @{

  /// Sets the text on the object.
  /// @param text The text to set.
  virtual void setText( const char* text );

  /// Sets the text on the object via a StringBuffer.
  /// @param text The text to set.
  virtual void setText( const StringBuffer& text );

  /// Inserts text at the cursor position.
  /// @param text The text to insert.
  virtual void insertText( UTF16* text );

  /// Removes the character before the cursor or clears the selected block.
  void backspace();

  /// Removes the character after the cursor or clears the selected block.
  void deleteCharacter();

  /// Removes all the characters in the highlight block.
  void deleteHighlighted();

  /// Gets the text set on the object.
  /// @return A reference to the text buffer.
  const StringBuffer& getText() const;

  /// Sizes the object so that the entire text string will fit within the 
  /// bounds.
  void autoSize();

  /// Sizes the object so that the entire height of the text string will
  /// fit within the bounds.
  void autoSizeHeight();

  /// Sets the position at which text will be inserted.
  /// @param pos The index of the character before which the cursor will be
  /// placed.
  void setCursorPosition( S32 pos );

  /// Sets the cursor position based on a world position.
  /// @param worldPos The position in the scene of the character at which
  /// the cursor should be placed.
  void setCursorPosition( Vector2 worldPos );

  /// Gets the cursor position.
  /// @return The index of the character at which the cursor is located.
  S32 getCursorPosition() const { return mCursorPos; };

  /// Gets the index in the text string based on a world position.
  /// @param worldPos The position in the scene to pick at.
  /// @return The index of the character at the world position.
  S32 getCharacterPosition( Vector2 worldPos );

  /// Returns the position of the character at 'index' in the text string,
  /// recalculating the bounds if necessary.
  RectF getCharacterBounds( S32 index );

  /// Returns the rectangle containing every character in the text string.
  RectF getTextBounds();

  /// Sets whether or not the cursor should be rendered.
  /// @param render True to show the cursor, false to hide it.
  void showCursor( bool render ) { mRenderCursor = render; };

  /// Gets whether or not the cursor is being rendered.
  /// @return True if the cursor is shown, false otherwise.
  bool getShowCursor() const { return mRenderCursor; };

  /// If the cursor is at a line break, this sets whether or not it should be
  /// rendered before or after the line break.
  void showCursorAtEndOfLine( bool render ) { mShowCursorAtEndOfLine = render; };

  /// Returns whether or not the cursor is being rendered at the end of
  /// the line.
  /// @return True if the cursor is rendering at the end of lines, false
  /// otherwise.
  bool getShowCursorAtEndOfLine() const { return mShowCursorAtEndOfLine; };

  /// Sets the area of text that is highlighted. The highlight block is
  /// the area of text between the indexes specified  by mCursorPos and
  /// mHighlightBlock.
  /// @param startPos The index of the first character to highlight.
  /// @param endPos The index of the last character to highlight.
  void setHighlightBlock( S32 startPos, S32 endPos );

  /// Gets the block of text that is highlighted.
  /// @return The return value is the index of the last highlighted
  /// character. This is used in conjunction with the cursor position
  /// to determine the full highlight block.
  S32 getHighlightBlock() const { return mHighlightBlock; };
  /// @}

  /// @name Property_Accessors
  /// @{

  /// Sets the editing flag on the object. This affects how the text responds
  /// to being resized.
  void setEditing( bool editing ) { mEditing = editing; };

  /// Gets the editing flag on the object.
  bool getEditing() { return mEditing; };

  /// Sets the word wrap flag.
  void setWordWrap( bool wrap );

  /// Gets the word wrap flag.
  bool getWordWrap() { return mWordWrap; };

  /// Sets the auto size flag.
  void setAutoSize( bool autoSize ) { mAutoSize = autoSize; };

  /// Gets the auto size flag.
  bool getAutoSize() { return mAutoSize; };

  /// Sets the auto size flag.
  void setClipText( bool clip ) { mClipText = clip; };

  /// Gets the auto size flag.
  bool getClipText() { return mClipText; };

  /// Sets the height of each line.
  void setLineHeight( F32 height );

  /// Gets the height of each line.
  F32 getLineHeight() { return mLineHeight; };

  /// Sets the text alignment mode.
  void setTextAlign( TextAlign align ) { mTextAlignment = align; setCharactersDirty(); };

  /// Gets the text alignment mode.
  TextAlign getTextAlign() { return mTextAlignment; };

  /// Sets the aspect ratio of the characters.
  void setAspectRatio( F32 ar );

  /// Gets the aspect ratio of the characters.
  F32 getAspectRatio() { return mAspectRatio; };

  /// Sets the spacing between each line.
  void setLineSpacing( F32 space ) { mLineSpacing = space; setCharactersDirty(); };

  /// Gets the spacing between each line.
  F32 getLineSpacing() { return mLineSpacing; };

  /// Sets the spacing between each character.
  void setCharacterSpacing( F32 space ) { mCharacterSpacing = space; setCharactersDirty(); };

  /// Gets the spacing between each character.
  F32 getCharacterSpacing() { return mCharacterSpacing; };
  /// @}

  /// @name Text_Field_Information
  /// @{

  /// Returns the height of each line in the text string.
  /// @return The line height.
  F32 getLineHeight() const;

  /// Returns the number of lines in the text string. This does not take
  /// wrapping into account.
  /// @return The number of new line characters in the text.
  S32 getLineCount() const;

  /// Returns the line number that a character is on.
  /// @param index The index of the character whose line number is being
  /// queried.
  /// @return The line number.
  S32 getLineNumber( S32 index );

  /// Gets the character index of a line.
  /// @param lineNumber The line number whose index is being retrieved.
  /// @return The index of the character at the beginning of the line.
  S32 getLineIndex( S32 lineNumber );

  /// Returns the number of lines in the text string. This does take
  /// wrapping into account.
  /// @return The number of lines in the text.
  S32 getActualLineCount();

  /// Returns the width of the longest line in the text string.
  F32 getLineWidth( GFont* font ) const;
  /// @}

protected:

  /// @name Font_Management
  /// @{

  /// Creates a font with the given face name at the given size.
  /// @param fontFace The name of the font to create.
  /// @param fontSize The size of the font to create.
  /// @param font The font object to store the newly created font in.
  /// @return Whether or not the font creation was successful.
  bool createFont( const char* fontFace, S32 fontSize, Font& font );
  /// @}

  /// @name Rendering
  /// @{

  /// Renders the text string with the specified font.
  /// @param font The font to render with.
  virtual void renderText( GFont* font );

  /// Renders the cursor at mCursorPos.
  virtual void renderCursor( GFont* font ) {};
  /// @}

private:

  /// @name Support_Methods
  /// @{

  /// Returns the width of the next wrap object, whether it be a character or
  /// word.
  /// @param text The text that starts with the wrap object.
  /// @param 
  F32 getNextWrapWidth( const StringBuffer& text ) const;

  /// Returns whether or not 'character' is a white space character.
  bool isWhiteSpace( UTF16 character ) const;

  /// Returns whether or not 'character' is renderable.
  bool isRenderable( UTF16 character ) const;

  /// Returns the ratio of the character size to the render size of the
  /// given font.
  /// @param font The font to calculate the size ratio on.
  /// @return The ratio of the actual size of each character to the size
  /// it will be rendered at.
  Vector2 getSizeRatio( GFont* font ) const;
  /// @}

  /// @name Character_Management
  /// @{

  /// If this is true, the characters bounds need to be recalculated.
  bool mCharactersDirty;

  /// Sets the characters dirty flag.
  /// @param dirty True to force recalculation of character bounds, false to
  /// indicate the bounds are valid and need not be recalculated.
  void setCharactersDirty( bool dirty = true ) { mCharactersDirty = dirty; };

  /// Calculates the position of each of the characters in the text string.
  void calculateCharacterBounds();

  /// Aligns the text on a line based on the alignment mode.
  void alignText( S32 lineStart, S32 lineEnd );
  /// @}

  /// @name Display_Properties
  /// @{

  /// Whether or not to wrap words.
  bool mWordWrap;

  /// Whether or not to clip text outside the bounding box.
  bool mClipText;

  /// The text alignment.
  TextAlign mTextAlignment;

  /// The height of each line. If this is set to 0, the text will stretch
  /// to fit the size of the object. But, this will basically destroy the
  /// line count calculations and thus several other properties will most
  /// likely not work as expected.
  F32 mLineHeight;

  /// The aspect ratio of the text. The values:
  /// = 1 - The text will render at its natural aspect ratio.
  /// < 1 - The text will render stretched vertically.
  /// > 1 - The text will render stretched horizontally.
  /// If SQUISH is set as the render mode, this parameter does nothing.
  F32 mAspectRatio;

  /// The line spacing is the amount of extra empty space between each line.
  F32 mLineSpacing;

  /// The character spacing is the amount of extra empty space between
  /// each character.
  F32 mCharacterSpacing;

  /// The font objects. Each of the fonts in this vector should be of the
  /// same face, just different sizes. The vector is not sorted by size.
  Vector<Font> mFonts;

  /// The actual text.
  StringBuffer mText;

  /// The positions of each character, stored relative to the object center,
  /// and cached for quick lookup.
  RectF* mCharacterBounds;

  /// The name of the font.
  StringTableEntry mFontName;

  /// Maximum height allowed for any font.
  static const S32 MAX_FONT_HEIGHT = 150;

  /// @}

  /// @name Editing_Properties
  /// @{

  /// The current position of the cursor.
  S32 mCursorPos;

  /// The range of characters that are highlighted. The highlighted characters
  /// are those between mCursorPos and this. If this is -1, than no characters
  /// are highlighted.
  S32 mHighlightBlock;

  /// Specifies whether or not the cursor should be rendered.
  bool mRenderCursor;

  /// Specifies the line on which the cursor should be rendered. If this is
  /// true, the cursor will be rendered at the right of the previous character
  /// as opposed to the left of the current character when it's at the end of
  /// a line.
  bool mShowCursorAtEndOfLine;

  /// The color of the cursor.
  ColorF mCursorColor;

  /// Stores the current time that the cursor has been in it's current fade
  /// state. Used to determine the time at which it should blink.
  F32 mCursorFadeDelay;

  /// Specifies whether or not the object should be autosized when the
  /// text is changed.
  bool mAutoSize;

  /// If the object is being editted, resizing will affect the word wrapping,
  /// otherwise, it will affect the character height and horizontal scale.
  bool mEditing;
  /// @}

  /// @name Dummy_Properties
  /// Dummy properties are necessary for some protected fields. The fields
  /// always need to map to a variable of the appropriate type, but in some
  /// cases, the underlying property is not of that type. So, we create these
  /// variables to appease that system. The actual value of the protected
  /// fields will still be correctly retrieved because the accessor functions
  /// will reference the real property, not these.
  /// @{

  /// Int vector for mapping to the 'fontSizes' persistent field.
  Vector<S32> mFontSizes;

  /// Single byte character buffer for mapping to the 'text' persistent
  /// field.
  const char* mConsoleBuffer;
  /// @}

  /// @name Protected_Field_Accessors
  /// @{
  static bool setText( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setText( data ); return false; }
  static const char* getText( void* obj, const char* data );

  static bool setFont( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setFont( data ); return false; }
  static const char* getFont( void* obj, const char* data ) { return static_cast<TextObject*>( obj )->getFontName(); }

  static bool setFontSizes( void* obj, const char* data );
  static const char* getFontSizes( void* obj, const char* data );

  static bool setWordWrap( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setWordWrap( dAtob( data ) ); return false; };
  static bool setTextAlign( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setCharactersDirty(); return true; };
  static bool setLineHeight( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setLineHeight( dAtof( data ) ); return false; };
  static bool setAspectRatio( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setAspectRatio( dAtof( data ) ); return false; };
  static bool setLineSpacing( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setLineSpacing( dAtof( data ) ); return false; };
  static bool setCharacterSpacing( void* obj, const char* data ) { static_cast<TextObject*>( obj )->setCharacterSpacing( dAtof( data ) ); return false; };
  /// @}


private:
  F32 mAlpha;
  bool	mFilter;
  bool	mIntegerPrecision;
  bool mNoUnicode;		//Luma:	this object will not use Unicode

public:
  void setAlpha(F32 alpha);
  F32 getAlpha();
  void getTextSize(Vector2 *pSize);


  void updateSpatialConfig(){};

};

#endif // _TEXT_OBJECT_H_
