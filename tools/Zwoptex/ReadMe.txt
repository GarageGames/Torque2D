This read me contains instructions for adding a new template to Zwoptex, allowing you to publish directly to a T2D ImageAsset format.

1. Open Zwoptex
2. Open preferences (cmd+,)
3. Click on "Coordinates Formats"
4. Click the plus symbol at the bottom
5. Rename to T2D
6. Change the extension to .asset.taml
7. Copy the following to the source box:

<ImageAsset
    AssetName="{{ metadata.target.name }}"
    ImageFile="@assetFile={{ metadata.target.textureFileName }}{{ metadata.target.textureFileExtension }}">
	<ImageAsset.Cells>
		{% for sprite in spritesAndAliases %}<Cell Offset="{{ sprite.textureRectX }} {{ sprite.textureRectY }}" Width="{{ sprite.textureRectWidth }}" Height="{{ sprite.textureRectHeight }}"/>
		{% /for %}</ImageAsset.Cells>
</ImageAsset>

And that's it! You can now publish ImageAssets directly from Zoptex