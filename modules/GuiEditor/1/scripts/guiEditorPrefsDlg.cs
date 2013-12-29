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

$Gui::defaultGridSize = 8;

//-----------------------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------------------
function GuiEditorPrefsDlgOkBtn::onAction(%this)
{
    $pref::guiEditor::snap2gridsize = GuiEditorPrefsDlgGridEdit.getValue();
    
    if ($pref::guiEditor::snap2grid)
        GuiEditor.setSnapToGrid($pref::guiEditor::snap2gridsize);
    
    Canvas.popDialog(GuiEditorPrefsDlg);
}

function GuiEditorPrefsDlgCancelBtn::onAction(%this)
{
    Canvas.popDialog(GuiEditorPrefsDlg);
}

function GuiEditorPrefsDlgDefaultsBtn::onAction(%this)
{
    GuiEditorPrefsDlgGridSlider.setValue($Gui::defaultGridSize);
}

//-----------------------------------------------------------------------------------------
// Grid
//-----------------------------------------------------------------------------------------
function GuiEditorPrefsDlgGridEdit::onWake(%this)
{
    %this.setValue($pref::guiEditor::snap2gridsize);
}

function GuiEditorPrefsDlgGridSlider::onWake(%this)
{
    %this.setValue($pref::guiEditor::snap2gridsize);
}

function GuiEditorPrefsDlgGridSlider::onAction(%this)
{
    %val = %this.value;
    
    if(%val == 0)
        %val = 1;
        
    GuiEditorPrefsDlgGridEdit.setvalue(%val);
}