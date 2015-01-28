using System;
using System.Drawing.Imaging;
using System.IO;
using System.Text.RegularExpressions;
using System.Web;
using CollectionLibrary;
using ImageProviderForItem;
using TileLibrary;

namespace JITHttpRequestLibrary
{
	// Translates the URL query parameters for a tile image request into useful properties
	internal class CollectionTileRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*)_files/(.*)/(.*)_(.*).jpg", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);

		public string CachePath { get; private set; }
		public string CollectionKey { get; private set; }
		public int Level { get; private set; }
		public int X { get; private set; }
		public int Y { get; private set; }

		public CollectionTileRequest(HttpContext context)
		{
			try
			{
				Uri url = context.Request.Url;
				Match match = s_matcher.Match(url.AbsolutePath);
				if (match.Groups.Count != 5)
					return; // This object will be rejected due to a bad url format

				CachePath = context.Request.PhysicalPath;
				CollectionKey = match.Groups[1].Value;
				Level = int.Parse(match.Groups[2].Value);
				X = int.Parse(match.Groups[3].Value);
				Y = int.Parse(match.Groups[4].Value);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public bool Serve(PivotCollection collection, HttpContext context, bool cacheToDisk)
		{
			if (collection == null)
				return false;
			using (MemoryStream memoryStream = new MemoryStream())
			{
				Build(collection, memoryStream);
				memoryStream.Position = 0; // Rewind the stream, ready for reading
				memoryStream.WriteTo(context.Response.OutputStream);
				context.Response.ContentType = "image/jpeg";
				if (cacheToDisk)
					CacheFile.Save(memoryStream, CachePath);
			}

			return true;
		}

		private void Build(PivotCollection collection, Stream stream)
		{
			try
			{
				int mortonStart = MortonHelpers.LevelXYToMortonStart(Level, X, Y, AllTileDefaults.CollectionTileMaxLevel);

				// Draw the sub-tiles into the tile
				int subTileCount = MortonHelpers.LevelToSubTileCount(Level, AllTileDefaults.CollectionTileMaxLevel);
				int subTileSize = AllTileDefaults.CollectionTileSize / subTileCount;
				using (CollectionTile tile = new CollectionTile(AllTileDefaults.CollectionTileSize, subTileSize))
				{
					for (int subTileX = 0; subTileX < subTileCount; ++subTileX)
					{
						for (int subTileY = 0; subTileY < subTileCount; ++subTileY)
						{
							int uniqueID = mortonStart + MortonHelpers.XYToMorton(subTileX, subTileY);
							Item item = collection.FindItem(uniqueID);
							if (item == null)
								continue;
							ImageProviderBase imageProvider = item.ItemImage.GetImageProvider();
							tile.Draw(imageProvider, subTileX * subTileSize, subTileY * subTileSize);
						}
					}

					tile.Save(stream, ImageFormat.Png);
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}
	}
}