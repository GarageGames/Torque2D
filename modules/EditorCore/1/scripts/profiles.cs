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

//---------------------------------------------------------------------------------------------
// GuiEditorDefaultProfile is a special profile that all other profiles inherit defaults from. It
// must exist.
//---------------------------------------------------------------------------------------------
if (!isObject(GuiEditorDefaultProfile)) new GuiControlProfile (GuiEditorDefaultProfile)
{
    tab = false;
    canKeyFocus = false;
    hasBitmapArray = false;
    mouseOverSelected = false;

    // fill color
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "244 244 244 52";

    // border color
    border = 1;
    borderColor    = "100 100 100 255";
    borderColorHL = "128 128 128 255";
    borderColorNA = "226 226 226 52";

    // font
    fontType = "Open Sans";
    fontSize = 16;

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";

    // bitmap information
    bitmap = "^EditorAssets/data/images/window";
    bitmapBase = "";
    textOffset = "0 0";

    // used by guiTextControl
    modal = true;
    justify = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";

    // sounds
    soundButtonDown = "";
    soundButtonOver = "";
};

if (!isObject(GuiEditorSolidDefaultProfile)) new GuiControlProfile (GuiEditorSolidDefaultProfile : GuiEditorDefaultProfile)
{
    opaque = true;
    border = true;
};

if (!isObject(GuiEditorTransparentProfile)) new GuiControlProfile (GuiEditorTransparentProfile : GuiEditorDefaultProfile)
{
    opaque = false;
    border = false;
};

if (!isObject(GuiEditorToolTipProfile)) new GuiControlProfile (GuiEditorToolTipProfile : GuiEditorDefaultProfile)
{
    fillColor = "246 220 165 255";
    // font
    fontSize = 16;
};

if (!isObject(GuiEditorModelessDialogProfile)) new GuiControlProfile(GuiEditorModelessDialogProfile : GuiEditorDefaultProfile)
{
    opaque = false;
    border = false;
    modal = false;
};

if (!isObject(GuiEditorFrameSetProfile)) new GuiControlProfile (GuiEditorFrameSetProfile : GuiEditorDefaultProfile)
{
    fillColor = "232 240 248";
    borderColor    = "138 134 122";
    opaque = true;
    border = true;
};

if (!isObject(GuiEditorWindowProfile)) new GuiControlProfile (GuiEditorWindowProfile : GuiEditorDefaultProfile)
{
    opaque = true;
    border = 0;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "255 255 255 52";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    text = "untitled";
    bitmap = "^EditorAssets/data/images/window";
    textOffset = "5 5";
    hasBitmapArray = true;
    justify = "center";
    
    fontType = "Arial Bold";
    fontSize = 16;
};

// ----------------------------------------------------------------------------
// Button Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorButtonProfile)) new GuiControlProfile (GuiEditorButtonProfile)
{
    opaque = true;
    border = -1;
    fontColor = "0 0 0";
    fontColorHL = "229 229 229 255";
    fixedExtent = true;
    justify = "center";
    canKeyFocus = false;
    fontType = "Open Sans";
    bitmap = "^EditorAssets/data/images/smallButtonContainer";
};

if (!isObject(GuiEditorLargeButtonContainer)) new GuiControlProfile (GuiEditorLargeButtonContainer : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/largeButtonContainer";
};

if (!isObject(GuiEditorSelectedElementHighlight)) new GuiControlProfile (GuiEditorSelectedElementHighlight : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/selectedImageContainer";
};

if (!isObject(BlueEditorButtonProfile)) new GuiControlProfile (BlueEditorButtonProfile : GuiEditorButtonProfile)
{
    fontSize = 16;
    fontColor = "255 255 255 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^EditorAssets/data/images/blueButton";
};

if (!isObject(RedEditorButtonProfile)) new GuiControlProfile (RedEditorButtonProfile : GuiEditorButtonProfile)
{
    fontSize = 16;
    fontColor = "255 255 255 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^EditorAssets/data/images/redButton";
};

if (!isObject(GreenEditorButtonProfile)) new GuiControlProfile (GreenEditorButtonProfile : GuiEditorButtonProfile)
{
    fontSize = 16;
    fontColor = "255 255 255 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^EditorAssets/data/images/greenButton";
};

if (!isObject(GuiEditorRadioProfile)) new GuiControlProfile (GuiEditorRadioProfile : GuiEditorDefaultProfile)
{
    fillColor = "232 232 232 255";
    fixedExtent = true;
    bitmap = "^EditorAssets/data/images/radioButton";
    hasBitmapArray = true;
};

// ----------------------------------------------------------------------------
// Container/PanelEditor Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorSunkenContainerProfile)) new GuiControlProfile (GuiEditorSunkenContainerProfile)
{
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    border = -2;
    bitmap = "^EditorAssets/data/images/sunkenContainer";
    borderColor = "40 40 40 10";
};

if (!isObject(GuiEditorPictureContainerProfile)) new GuiControlProfile (GuiEditorPictureContainerProfile)
{
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 255";
    border = -2;
    bitmap = "^EditorAssets/data/images/profilePicture";
    borderColor = "40 40 40 10";
};

if (!isObject(GuiEditorScreenContainerProfile)) new GuiControlProfile (GuiEditorScreenContainerProfile)
{
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    border = -2;
    bitmap = "^EditorAssets/data/images/containerForScreens";
    borderColor = "40 40 40 10";
};

if (!isObject(GuiEditorThumbnailContainerProfile)) new GuiControlProfile (GuiEditorThumbnailContainerProfile)
{
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    border = -2;
    bitmap = "^EditorAssets/data/images/thumbnailFrame";
    borderColor = "40 40 40 10";
};

if (!isObject(GuiEditorMediumButton)) new GuiControlProfile (GuiEditorMediumButton : GuiEditorButtonProfile)
{
    bitmap = "^EditorAssets/data/images/mediumButtonContainer";
};

if (!isObject(GuiEditorLargePanelEditorContainer)) new GuiControlProfile (GuiEditorLargePanelEditorContainer : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/buttonStateContainer_atlas";
};

if (!isObject(GuiEditorLargePanelContainerInactive)) new GuiControlProfile (GuiEditorLargePanelContainerInactive : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/buttonStateContainer_inactive";
};

if (!isObject(GuiEditorNarrowPanelContainer)) new GuiControlProfile (GuiEditorNarrowPanelContainer : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/noStateContainer";
};

if (!isObject(GuiEditorNarrowPanelContainerHighlight)) new GuiControlProfile (GuiEditorNarrowPanelContainerHighlight : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/noStateContainer_highlight";
};

if (!isObject(GuiEditorWorldPaneContainer)) new GuiControlProfile (GuiEditorWorldPaneContainer : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/worldsLevelsContainer";
};

if (!isObject(GuiEditorLargeButtonContainerInactive)) new GuiControlProfile (GuiEditorLargeButtonContainerInactive : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/largeButtonContainer_inactive";
};

if (!isObject(GuiEditorLargeButtonHighlight)) new GuiControlProfile (GuiEditorLargeButtonHighlight : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/largeButtonContainer_highlightBar";
};

if (!isObject(GuiEditorLargePanelContainerHighlight)) new GuiControlProfile (GuiEditorLargePanelContainerHighlight : GuiEditorButtonProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/buttonStateContainer_highlight";
};

if (!isObject(PanelEditorLightProfile)) new GuiControlProfile (PanelEditorLightProfile) 
{
    opaque = false;
    bitmap = "^EditorAssets/data/images/tabBook";
    border = 1;
};

if (!isObject(PanelEditorMediumProfile)) new GuiControlProfile (PanelEditorMediumProfile) 
{
    opaque = false;
    bitmap = "^EditorAssets/data/images/PanelMedium";
    border = -2;
};

if (!isObject(PanelEditorDarkProfile)) new GuiControlProfile (PanelEditorDarkProfile) 
{
    opaque = false;
    bitmap = "^EditorAssets/data/images/sunkenContainer";
    border = -2;
};

if (!isObject(PanelEditorTransparentProfile)) new GuiControlProfile(EditorPanelEditorTransparent : PanelEditorDarkProfile)
{
    bitmap = "^EditorAssets/data/images/Panel_transparent";
};

if (!isObject(GuiEditorTransparentProfileModeless)) new GuiControlProfile (GuiEditorTransparentProfileModeless : GuiEditorTransparentProfile) 
{
    modal = false;
};

if (!isObject(GuiEditorLevelPanelContainer)) new GuiControlProfile (GuiEditorLevelPanelContainer : GuiEditorDefaultProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/worldsLevelsContainer";
};

if (!isObject(GuiEditorLevelPanelContainerHighlight)) new GuiControlProfile (GuiEditorLevelPanelContainerHighlight : GuiEditorDefaultProfile)
{
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/worldsLevelsContainerHighlight";
};

// ----------------------------------------------------------------------------
// CheckBox Control Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorCheckBoxProfile)) new GuiControlProfile (GuiEditorCheckBoxProfile)
{
    opaque = false;
    fillColor = "232 232 232 255";
    border = false;
    borderColor = "0 0 0 255";
    fontType = "Arial";
    fontSize = 16;
    fixedExtent = true;
    justify = "left";
    bitmap = "^EditorAssets/data/images/checkBox";
    hasBitmapArray = true;
};

// ----------------------------------------------------------------------------
// Slider Control Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorSliderProfile)) new GuiControlProfile (GuiEditorSliderProfile)
{
    bitmap = "^EditorAssets/data/images/slider";
    fontType = "Arial";
    fontSize = 16;
    fontColor = "229 229 229 255";
};

if (!isObject(GuiEditorSliderNoTextProfile)) new GuiControlProfile (GuiEditorSliderNoTextProfile)
{
    bitmap = "^EditorAssets/data/images/slider";
    fontColor = "255 255 255 255";
    fontSize = 16;
};

// ----------------------------------------------------------------------------
// TreeView Control Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorTreeViewProfile)) new GuiControlProfile (GuiEditorTreeViewProfile)
{
    fillColorHL = "0 60 150 255";
    fontType = "Arial";
    fontSize = 16;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 255";
    fontColorSEL= "10 10 10 255";
    bitmap = "^EditorAssets/data/images/treeView";
    canKeyFocus = true;
    autoSizeHeight = true;
};

// ----------------------------------------------------------------------------
// ListBox Control Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorListBoxProfile)) new GuiControlProfile (GuiEditorListBoxProfile)
{
    tab = true;
    canKeyFocus = true;
    
    fontType = "Arial";
    fontSize = 16;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";
};

// ----------------------------------------------------------------------------
// Text Control Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorTextProfile)) new GuiControlProfile (GuiEditorTextProfile)
{
    border=false;

    // font
    fontType = "Open Sans";
    fontSize = 16;

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";

    modal = true;
    justify = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";
};

if (!isObject(GuiEditorText16Profile)) new GuiControlProfile (GuiEditorText16Profile : GuiEditorTextProfile)
{
    fontSize = 16;
};

if (!isObject(GuiEditorMLTextCenteredLgPreviewProfile)) new GuiControlProfile (GuiEditorMLTextCenteredLgPreviewProfile : GuiEditorTextProfile)
{
    border = false;
    justify = "center";
};

if (!isObject(GuiEditorMLTextCenteredSmPreviewProfile)) new GuiControlProfile (GuiEditorMLTextCenteredSmPreviewProfile : GuiEditorTextProfile)
{
    border = false;
    justify = "center";
};

if (!isObject(GuiEditorTextCenteredLgPreviewProfile)) new GuiControlProfile (GuiEditorTextCenteredLgPreviewProfile : GuiEditorTextProfile)
{
    justify = "center";
};

if (!isObject(GuiEditorTextCenteredSmPreviewProfile)) new GuiControlProfile (GuiEditorTextCenteredSmPreviewProfile : GuiEditorTextProfile)
{
    justify = "center";
};

if (!isObject(GuiEditorModelessTextProfile)) new GuiControlProfile(GuiModelessTextProfile : GuiEditorTextProfile)
{
    modal = false;
};

if (!isObject(GuiEditorModelessInactiveTextProfile)) new GuiControlProfile(GuiModelessInactiveTextProfile : GuiEditorTextProfile)
{
    modal = false;
    fontColor = "0 0 0 52";
};

if (!isObject(GuiEditorDarkTextProfile)) new GuiControlProfile (GuiEditorDarkTextProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans Bold";
    fontSize = 16;
    fontColorNA = "0 0 0 52";
    fontColor = "27 59 95 255";
    border=false;
};

if (!isObject(GuiEditorPaneTextProfile)) new GuiControlProfile (GuiEditorPaneTextProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 16;
    fontColorNA = "0 0 0 52";
    Modal = false;
};

if (!isObject(GuiEditorTextAddBtnProfile)) new GuiControlProfile (GuiEditorTextAddBtnProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 16;
    fontColorNA = "0 0 0 52";
    Modal = false;
};

if (!isObject(BlueTextProfile)) new GuiControlProfile (BlueTextProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 14;
    fontColorNA = "27 95 59 255";
};

if (!isObject(GuiEditorRedTextProfile)) new GuiControlProfile (GuiEditorRedTextProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 16;
    fontColor = "158 47 47 255";
};

if (!isObject(BlueTextCenteredProfile)) new GuiControlProfile (BlueTextCenteredProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 14;
    fontColorNA = "27 95 59 255";
    justify = "center";
};

if (!isObject(GuiEditorInactiveTextProfile)) new GuiControlProfile (GuiEditorInactiveTextProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 16;
    fontColor = "0 0 0 52";
};

if (!isObject(GuiEditorFooterTextProfile)) new GuiControlProfile (GuiEditorFooterTextProfile : GuiEditorTextProfile)
{
    fontType = "Open Sans";
    fontSize = 14;
};

if (!isObject(GuiEditorMediumTextProfile)) new GuiControlProfile (GuiEditorMediumTextProfile : GuiEditorTextProfile)
{
    fontSize = 18;
};

if (!isObject(GuiEditorBigTextProfile)) new GuiControlProfile (GuiEditorBigTextProfile : GuiEditorTextProfile)
{
    fontSize = 24;
};

if (!isObject(GuiEditorText24Profile)) new GuiControlProfile (GuiEditorText24Profile : GuiEditorTextProfile)
{
    fontSize = 18;
};

if (!isObject(GuiEditorTextRightProfile)) new GuiControlProfile (GuiEditorTextRightProfile : GuiEditorTextProfile)
{
    justify = "right";
};

if (!isObject(GuiEditorTextCenterProfile)) new GuiControlProfile (GuiEditorTextCenterProfile : GuiEditorTextProfile)
{
    justify = "center";
};

if (!isObject(GuiEditorMLTextProfile)) new GuiControlProfile (GuiEditorMLTextProfile)
{
    border=false;

    // font
    fontType = "Open Sans";
    fontSize = 16;

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";

    modal = true;
    justify = "left";
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";

    fontColorLink = "255 96 96 255";
    fontColorLinkHL = "0 0 255 255";
    autoSizeWidth = true;
    autoSizeHeight = true;  
};

if (!isObject(GuiEditorMLTextCenterProfile)) new GuiControlProfile (GuiEditorMLTextCenterProfile : GuiEditorMLTextProfile)
{
    justify = "center";
    border = "0";
    fontSize = 16;
};

if (!isObject(GuiEditorMLWhiteTextProfile)) new GuiControlProfile (GuiEditorMLWhiteTextProfile : GuiEditorMLTextProfile)
{
    fontColor = "229 229 229 255";
    fontType = "Arial";
    fontSize = 16;
    border = false;
};

if (!isObject(GuiEditorConsoleProfile)) new GuiControlProfile (GuiEditorConsoleProfile)
{
    fontType = ($platform $= "macos") ? "Monaco" : "Lucida Console";
    fontSize = ($platform $= "macos") ? 13 : 12;
    fontColor = "255 255 255 255";
    fontColorHL = "155 155 155 255";
    fontColorNA = "255 0 0 52";
    fontColors[6] = "100 100 100 255";
    fontColors[7] = "100 100 0 255";
    fontColors[8] = "0 0 100 255";
    fontColors[9] = "0 100 0 255";
};

// ----------------------------------------------------------------------------
// Text Edit Box Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorTextEditProfile)) new GuiControlProfile (GuiEditorTextEditProfile)
{
    fontSize = 16;
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    border = -2;
    bitmap = "^EditorAssets/data/images/textEdit";
    borderColor = "40 40 40 10";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL = "0 0 0 255";
    textOffset = "7 2";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = true;
    returnTab = true;
};

if (!isObject(GuiEditorSpinnerProfile)) new GuiControlProfile (GuiEditorSpinnerProfile)
{
    fontSize = 16;
    opaque = false;
    justify = "center";
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    numbersOnly = true;
    border = -2;
    bitmap = "^EditorAssets/data/images/textEdit_noSides";
    borderColor = "40 40 40 10";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL = "0 0 0 255";
    textOffset = "4 2";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = true;
    returnTab = true;
};

if (!isObject(GuiEditorInactiveSpinnerProfile)) new GuiControlProfile (GuiEditorInactiveSpinnerProfile)
{
    fontSize = 16;
    opaque = false;
    justify = "center";
    fillColor = "127 127 127 52";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    numbersOnly = true;
    border = -2;
    bitmap = "^EditorAssets/data/images/textEdit_noSides_inactive";
    borderColor = "40 40 40 10";
    fontColor = "0 0 0 52";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL = "0 0 0 255";
    textOffset = "4 2";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = false;
    returnTab = false;
};

if (!isObject(GuiEditorInvalidSpinnerProfile)) new GuiControlProfile (GuiEditorInvalidSpinnerProfile)
{
    fontSize = 16;
    opaque = false;
    justify = "center";
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    numbersOnly = true;
    border = -2;
    bitmap = "^EditorAssets/data/images/textEdit_noSides";
    borderColor = "40 40 40 10";
    fontColor = "255 0 0 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL = "0 0 0 255";
    textOffset = "4 2";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = true;
    returnTab = true;
};

if (!isObject(GuiEditorFileEditBoxProfile)) new GuiControlProfile (GuiEditorFileEditBoxProfile)
{
    fontSize = 16;
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    border = -2;
    bitmap = "^EditorAssets/data/images/textEdit_noRightEdge";
    borderColor = "40 40 40 10";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL = "0 0 0 255";
    textOffset = "7 2";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = true;
    returnTab = true;
};

if (!isObject(GuiEditorObjectEditBoxProfile)) new GuiControlProfile (GuiEditorObjectEditBoxProfile)
{
    fontSize = 16;
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    border = -2;
    borderColor = "40 40 40 10";
    bitmap = "^EditorAssets/data/images/textEdit_transparent";
    textOffset = "4 2";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = false;
    returnTab = false;
    Modal=true;
    fontType = "Open Sans";
    fontSize = 16;
    justify="left";
};

if (!isObject(GuiEditorObjectEditBoxCenteredProfile)) new GuiControlProfile (GuiEditorObjectEditBoxCenteredProfile : GuiEditorObjectEditBoxProfile)
{
    justify="center";
};

if (!isObject(GuiEditorTextEditInactiveProfile)) new GuiControlProfile (GuiEditorTextEditInactiveProfile : GuiEditorTextEditProfile)
{
    bitmap = "^EditorAssets/data/images/textEdit_inactive";
};

if (!isObject(GuiEditorFileEditInactiveProfile)) new GuiControlProfile (GuiEditorFileEditInactiveProfile : GuiEditorTextEditProfile)
{
    bitmap = "^EditorAssets/data/images/textEdit_noRightEdge_inactive";
};

if (!isObject(GuiEditorSpinnerInactiveProfile)) new GuiControlProfile (GuiEditorSpinnerInactiveProfile : GuiEditorTextEditProfile)
{
    bitmap = "^EditorAssets/data/images/textEdit_noSides_inactive";
};

if (!isObject(GuiEditorTextEditNumericProfile)) new GuiControlProfile (GuiEditorTextEditNumericProfile : GuiEditorTextEditProfile)
{
    numbersOnly = true;
};

if (!isObject(GuiEditorTextEditNumericCenteredProfile)) new GuiControlProfile (GuiEditorTextEditNumericCenteredProfile : GuiEditorTextEditNumericProfile)
{
    justify = "center";
};

if (!isObject(GuiEditorTextEditCenteredProfile)) new GuiControlProfile (GuiEditorTextEditCenteredProfile : GuiEditorTextEditProfile)
{
    justify = "center";
};

if (!isObject(GuiEditorTextEditNumericInactiveCenteredProfile)) new GuiControlProfile (GuiEditorTextEditNumericInactiveCenteredProfile : GuiEditorTextEditInactiveProfile)
{
    justify = "center";
};

if (!isObject(GuiEditorConsoleTextEditProfile)) new GuiControlProfile (GuiEditorConsoleTextEditProfile : GuiEditorTextEditProfile)
{
    fontType = ($platform $= "macos") ? "Monaco" : "Lucida Console";
    fontSize = ($platform $= "macos") ? 13 : 12;
};

// ----------------------------------------------------------------------------
// Scroll Control Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorScrollProfile)) new GuiControlProfile (GuiEditorScrollProfile)
{
    tab = false;
    canKeyFocus = false;
    hasBitmapArray = true;
    mouseOverSelected = false;

    // fill color
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "244 244 244 52";

    // border color
    border = 0;
    borderColor    = "100 100 100 255";
    borderColorHL = "128 128 128 255";
    borderColorNA = "226 226 226 52";

    // font
    fontType = "Open Sans";
    fontSize = 16;

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";

    // bitmap information
    bitmap = "^EditorAssets/data/images/scroll";
    bitmapBase = "";
    textOffset = "0 0";

    // used by guiTextControl
    modal = true;
    justify = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";

    // sounds
    soundButtonDown = "";
    soundButtonOver = "";
};

if (!isObject(ConsoleScrollProfile)) new GuiControlProfile(ConsoleScrollProfile : GuiScrollProfile)
{
	opaque = true;
	fillColor = "0 0 0 120";
	border = 3;
	borderThickness = 0;
	borderColor = "0 0 0 255";
};

if (!isObject(GuiEditorLightScrollProfile)) new GuiControlProfile (GuiEditorLightScrollProfile : GuiScrollProfile)
{
    opaque = false;
    fillColor = "212 216 220";
    border = 0;
    bitmap = "^EditorAssets/data/images/scroll";
    hasBitmapArray = true;
};

if (!isObject(GuiEditorTransparentScrollProfile)) new GuiControlProfile (GuiEditorTransparentScrollProfile : GuiScrollProfile)
{
    opaque = false;
    fillColor = "0 0 0 0";
    border = false;
    borderThickness = 0;
    borderColor = "0 0 0 0";
    bitmap = "^EditorAssets/data/images/transparentScroll";
    hasBitmapArray = true;
};
 
// ----------------------------------------------------------------------------
// Popup Menu Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorPopupMenuItemBorder)) new GuiControlProfile (GuiEditorPopupMenuItemBorder : GuiEditorButtonProfile)
{
    fillColorHL = "251 170 0 255";
    borderColor = "51 51 53 200";
    borderColorHL = "51 51 53 200";

    fontType = "Open Sans";
    fontSize = 16;

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";
};

if (!isObject(GuiEditorPopUpMenuDefault)) new GuiControlProfile (GuiEditorPopUpMenuDefault)
{
    tab = false;
    canKeyFocus = false;
    hasBitmapArray = true;

    // fill color
    opaque = false;
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "244 244 244 52";

    // border color
    border = 1;
    borderColor    = "100 100 100 255";
    borderColorHL = "128 128 128 255";
    borderColorNA = "226 226 226 52";

    // font
    fontType = "Open Sans";
    fontSize = 14;

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSEL= "10 10 10 255";

    // bitmap information
    bitmap = "^EditorAssets/data/images/dropDown";

    // used by guiTextControl
    modal = true;
    justify = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";

    profileForChildren = "GuiPopupMenuItemBorder";
    // sounds
    soundButtonDown = "";
    soundButtonOver = "";
};

if (!isObject(GuiEditorPopUpMenuProfile)) new GuiControlProfile (GuiEditorPopUpMenuProfile : GuiPopUpMenuDefault)
{
    textOffset = "6 3";
    fontType = "Open Sans";
    justify = "center";
};

if (!isObject(GuiEditorPopUpMenuEmptyProfile)) new GuiControlProfile (GuiEditorPopUpMenuEmptyProfile : GuiPopUpMenuDefault)
{
    textOffset = "6 3";
    fontType = "Open Sans Italic";
    justify = "center";
};

// ----------------------------------------------------------------------------
// Separator Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorSeparatorInactiveProfile)) new GuiControlProfile (GuiEditorSeparatorInactiveProfile : GuiEditorDefaultProfile)
{
    borderMargin = -2;
    borderColor    = "100 100 100 52";
    borderColorHL = "128 128 128 52";
    borderColorNA = "226 226 226 52";
};

if (!isObject(GuiEditorSeparatorProfile)) new GuiControlProfile (GuiEditorSeparatorProfile : GuiEditorDefaultProfile)
{
    opaque = false;
    border = false;
    borderMargin = -2;
};

// ----------------------------------------------------------------------------
// Tab Book/Page Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorTabBookProfile)) new GuiControlProfile (GuiEditorTabBookProfile)
{
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "150 150 150 52";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontType = "open sans";
    fontSize = 16;
    justify = "center";
    bitmap = "^EditorAssets/data/images/tab";
    tabWidth = 64;
    tabHeight = 24;
    tabPosition = "Top";
    tabRotation = "Horizontal";
    textOffset = "0 0";
    tab = true;
    cankeyfocus = true;
};

if (!isObject(GuiEditorTabPageProfile)) new GuiControlProfile (GuiEditorTabPageProfile : GuiTransparentProfile)
{
    fillColor = "232 240 248";
    opaque = false;
    border = -2;
    bitmap = "^EditorAssets/data/images/tabContainer";
};

// ----------------------------------------------------------------------------
// Utility Profiles
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorImageEditorGridBoxProfile)) new GuiControlProfile (GuiEditorImageEditorGridBoxProfile)
{
    border = 1;
    borderColor = "100 100 255 255";
    borderColorHL = "100 100 255 255";
    borderColorNA = "100 100 255 255";
    modal = false;
};

if (!isObject(GuiEditorImageEditorGridLinesProfile)) new GuiControlProfile (GuiEditorImageEditorGridLinesProfile)
{
    border = 7; // draw border on right&bottom only
    borderColor  = "100 100 255 255";
    borderColorHL = "100 100 255 255";
    borderColorNA = "100 100 255 255";
    modal = false;
};

if (!isObject(GuiEditorImageEditorGridGreyBoxProfile)) new GuiControlProfile (GuiEditorImageEditorGridGreyBoxProfile)
{
    border = 0;
    opaque = true;
    fillColor = "100 100 100 150";
    fillColorHL = "100 100 100 150";
    fillColorNA = "100 100 100 150";
    modal = false;
};

if (!isObject(GuiEditorCollisionEditorSelectionBoxProfile)) new GuiControlProfile (GuiEditorCollisionEditorSelectionBoxProfile)
{
    border = 1;
    borderColor = "200 200 0 255";
    //modal = false;
};

// ----------------------------------------------------------------------------
// Used by GUI Editor
// ----------------------------------------------------------------------------
if (!isObject(GuiEditorBackFillProfile)) new GuiControlProfile (GuiEditorBackFillProfile)
{
   opaque = true;
   fillColor = "0 94 94 255";
   border = true;
   borderColor = "255 128 128 255";
   fontType = "Arial";
   fontSize = 16;
   fontColor = "0 0 0 255";
   fontColorHL = "32 100 100 255";
   fixedExtent = true;
   justify = "center";
};

if (!isObject(GuiEditorInspectorFieldProfile)) new GuiControlProfile (GuiEditorInspectorFieldProfile)
{
   // fill color
   opaque = false;
   fillColor = "255 255 255 255";
   fillColorHL = "128 128 128 255";
   fillColorNA = "244 244 244 52";

   // border color
   border = false;
   borderColor   = "190 190 190 255";
   borderColorHL = "156 156 156 255";
   borderColorNA = "64 64 64 52";
   
   bevelColorHL = "255 255 255 255";
   bevelColorLL = "0 0 0";
   
   bitmap = "^EditorAssets/data/images/rollout";

   // font
   fontType = "Arial";
   fontSize = 16;

   fontColor = "32 32 32 255";
   fontColorHL = "32 100 100 255";
   fontColorNA = "0 0 0 52";

   tab = true;
   canKeyFocus = true;
};

if (!isObject(GuiEditorInspectorBackgroundProfile)) new GuiControlProfile (GuiEditorInspectorBackgroundProfile : GuiEditorInspectorFieldProfile)
{
   border = 5;
   cankeyfocus=true;
   tab = true;
};
