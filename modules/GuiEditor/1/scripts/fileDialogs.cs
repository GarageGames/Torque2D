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

$GUI::FileSpec = "Torque Gui Files (*.gui.taml)|*.gui.taml|All Files (*.*)|*.*|";

/// GuiBuilder::getSaveName - Open a Native File dialog and retrieve the
///  location to save the current document.
/// @arg defaultFileName   The FileName to default in the field and to be selected when a path is opened
function GuiBuilder::getSaveName(%defaultFileName)
{
    // if we're editing a game, we want to default to the games dir.
    // if we're not, then we default to the tools directory or the base.
    if (%defaultFileName !$= "" && isPathExpando("project"))
        %prefix = "^project";
    else if (isPathExpando("modules"))
        %prefix = "^modules";
    else
        %prefix = "";

    if (%defaultFileName $= "")
        %defaultFileName = expandPath(%prefix @ "/gui/untitled.gui.taml");
    else 
        %defaultFileName = expandPath(%prefix @ "/" @ %defaultFileName);

    %path = filePath(%defaultFileName) @ "/";
    %dlg = new SaveFileDialog()
    {
        Filters           = $GUI::FileSpec;
        DefaultPath       = %path;
        DefaultFile       = getSaveDefaultFile(%defaultFileName);
        ChangePath        = true;
        OverwritePrompt   = true;
    };

    if (%dlg.Execute())
    {
        $Pref::GuiEditor::LastPath = filePath(%dlg.FileName);
        %filename = %dlg.FileName;

        // force .gui.taml extension - eliminates the growing .gui.gui.gui.taml problem.
		%temp = fileBase(%dlg.FileName);
		%filename =  $Pref::GuiEditor::LastPath @ "/" @ %temp @ ".gui.taml";
    }
    else
        %filename = "";

    %dlg.delete();

    return %filename;
}

function GuiBuilder::getOpenName(%defaultFileName)
{
    if (%defaultFileName $= "")
        %defaultFileName = expandPath("^project/gui/untitled.gui.taml");

    %dlg = new OpenFileDialog()
    {
        Filters        = $GUI::FileSpec;
        DefaultPath    = $Pref::GuiEditor::LastPath;
        DefaultFile    = %defaultFileName;
        ChangePath     = false;
        MustExist      = true;
    };

    if (%dlg.Execute())
    {
        $Pref::GuiEditor::LastPath = filePath(%dlg.FileName);
        %filename = %dlg.FileName;      
        %dlg.delete();
        return %filename;
    }

    %dlg.delete();
    return "";   
}
