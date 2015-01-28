using System.IO;
using System.Web;

namespace PivotServerTools.Internal
{
	// Build a Deep Zoom Collection image tile by drawing item images onto it.
	internal class CollectionImageTileBuilder
	{
		Collection _collection;
		int _level;
		int _tilePixelDimension;
		int _imageDimensionCount;
		int _mortonStart;
		int _levelBitCount;

		public CollectionImageTileBuilder(Collection collection, ImageRequest imageRequest, int maxLevel, int tilePixelDimension)
		{
			_collection = collection;
			_level = imageRequest.Level;

			_tilePixelDimension = tilePixelDimension;
			_imageDimensionCount = (1 << (maxLevel - imageRequest.Level));
			_levelBitCount = maxLevel - imageRequest.Level;

			int mortonRange;
			_mortonStart = MortonHelpers.LevelXYToMorton(imageRequest.Level, imageRequest.X, imageRequest.Y,
				maxLevel, out mortonRange);
		}

		public void Write(HttpResponse response)
		{
			response.ContentType = "image/jpeg";
			Write(response.OutputStream);
		}

		public void Write(Stream outStream)
		{
			CollectionImageTile tile = new CollectionImageTile();
			try
			{
				tile.Create(_tilePixelDimension, _level);

				//Draw the sub-images into the tile
				int subImageDimension = _tilePixelDimension / _imageDimensionCount;
				for (int y = 0; y < _imageDimensionCount; ++y)
				{
					for (int x = 0; x < _imageDimensionCount; ++x)
					{
						int itemIndex = _mortonStart + MortonHelpers.XYToMorton(_levelBitCount, x, y);
						if (itemIndex < _collection.Items.Count)
						{
							CollectionItem item = _collection.Items[itemIndex];
							tile.DrawSubImage(item.ImageProvider, x*subImageDimension, y*subImageDimension,
								subImageDimension, subImageDimension);
						}
					}
				}

				tile.WriteTo(outStream);
			}
			finally
			{
				tile.Close();
			}
		}
	}
}
