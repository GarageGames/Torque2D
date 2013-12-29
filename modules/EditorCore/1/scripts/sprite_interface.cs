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

function SpriteBase::setAsset(%this, %asset)
{
    %assetType = AssetDatabase.getAssetType(%asset);
    
    if (%assetType $= "ImageAsset")
    {
        %this.setImageMap(%asset);
    }
    else if (%assetType $= "AnimationAsset")
    {
        %this.animation = %asset;
        %this.playAnimation(%asset);
    }
    else
    {
        warn("Sprite::setAsset -- invalid asset: " @ %asset); 
        warn("Asset type is " @ %assetType @ ", must be either ImageAsset or AnimationAsset"); 
        return; 
    }
}

function SpriteBase::getAsset(%this)
{
    if (%this.animation !$= "")
        return strchr(%this.Animation, "{");
    else if (%this.getImageMap() !$= "")
        return strchr(%this.getImageMap(), "{");
    else
        warn("Sprite::getAsset -- No asset found on Sprite " @ %this);
}

function Sprite::setSizeFromAsset(%this, %asset, %metersPerPixel)
{
    %sizePixels = "";
    
    %assetType = AssetDatabase.getAssetType(%asset);
    
    if (%assetType $= "ImageAsset")
    {
        %imageMapAsset = AssetDatabase.acquireAsset(%asset);
        %sizePixels = %imageMapAsset.getFrameSize(0);
        AssetDatabase.releaseAsset(%imageMapAsset.getAssetId());
    }
    else if (%assetType $= "AnimationAsset")
    {
        %animationAsset = AssetDatabase.acquireAsset(%asset);
        %animationImageMapAsset = AssetDatabase.acquireAsset(strchr(%animationAsset.imagemap, "{"));
        %sizePixels = %animationImageMapAsset.getFrameSize(0);
        AssetDatabase.releaseAsset(%animationImageMapAsset.getAssetId());
        AssetDatabase.releaseAsset(%animationAsset.getAssetId());
    }
    else
    {
        warn("Sprite::setSizeFromAsset -- invalid asset: " @ %asset); 
        warn("Asset type is " @ %assetType @ ", must be either ImageAsset or AnimationAsset"); 
        return; 
    }   
    
    // Set the size of the sprite by converting the pixel size to world units
    %sizeWorld = Vector2Scale(%sizePixels, %metersPerPixel);
    %this.setSize(%sizeWorld);
}