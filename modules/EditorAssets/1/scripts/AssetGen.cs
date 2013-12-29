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

/// <summary>
/// Call this function to scan a folder for images or sound files and create 
/// asset files for them.
/// </summary>
/// <param name="path">The path to scan - "^EditorAssets/data/images/".</param>
/// <param name="type">The asset type - "image" or "sound".</param>
/// <param name="category">The asset category.  If none, leave this blank.</param>
/// <param name="internal">Flag the asset as an internal asset.</param>
function makeAssetFiles(%path, %type, %category, %internal)
{
    switch$(%type)
    {
        case "image":
            %imageFile = findFirstFile(expandPath(%path @ "*.png"));
            while (%imageFile !$= "")
            {
                createAsset(%imageFile, %path, %type, %category, %internal);
                %imagefile = findNextFile(expandPath(%path @ "*.png"));
            }

        case "sound":
            %audioFile = findFirstFile(expandPath(%path @ "*.wav"));
            while (%audioFile !$= "")
            {
                createAsset(%audioFile, %path, %type, %category, %internal);
                %audioFile = findNextFile(expandPath(%path @ "*.wav"));
            }
    }
}


/// <summary>
/// This function creates an individual asset file for a particular image or sound.
/// </summary>
/// <param name="file">The file to create the asset for.</param>
/// <param name="path">The path to the file - "^EditorAssets/data/images/"</param>
/// <param name="type">The asset type - "image" or "sound"</param>
/// <param name="category">The asset category.  If none, leave this blank</param>
function createAsset(%file, %path, %type, %category, %internal)
{
    switch$(%type)
    {
        case "image":
            %datablock = new ImageAsset();
            %datablock.AssetName = fileBase(%file);
            %datablock.AssetInternal = %internal;
            %datablock.AutoUnload = "0";
            %datablock.ImageFile = %file;
            %datablock.AssetCategory = %category;
            
        case "sound":
            %datablock = new AudioAsset();
            %datablock.AssetName = fileBase(%file);
            %datablock.AssetInternal = %internal;
            %datablock.AutoUnload = "0";
            %datablock.AudioFile = %file;
            %datablock.AssetCategory = %category;
            
    }
    %assetFileName = expandPath(%path @ fileBase(%file) @ ".asset.taml");

    TamlWrite(%datablock, %assetFileName);
}