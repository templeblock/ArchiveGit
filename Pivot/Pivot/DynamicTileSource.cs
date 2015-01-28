using System;
using System.Collections.Generic;
using System.Windows.Media;

namespace Pivot
{
	public class DynamicTileSource : MultiScaleTileSource
	{
		private Uri _imageUri;
		private int _tileWidth = 128;
		private int _tileHeight = 128;

		public DynamicTileSource(Uri imageUri, int imageWidth, int imageHeight, int tileWidth, int tileHeight)
			: base(imageWidth, imageHeight, tileWidth, tileHeight, 0)
		{
			_imageUri = imageUri;
			_tileWidth = tileWidth;
			_tileHeight = tileHeight;
		}

		protected override void GetTileLayers(int tileLevel, int tilePositionX, int tilePositionY, IList<object> tileImageLayerSources)
		{
			string source = string.Format("{0}_tile?tileLevel={1}&tilePositionX={2}&tilePositionY={3}&tileWidth={4}&tileHeight={5}",
				_imageUri.OriginalString, tileLevel, tilePositionX, tilePositionY, _tileWidth, _tileHeight);

			Uri uri = new Uri(source, UriKind.Absolute);
			tileImageLayerSources.Add(uri);
		}
	}
}
