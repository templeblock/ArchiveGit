using System;
using System.Drawing;

namespace TileLibrary
{
	class AllTileDefaults
	{
		// If TileFill is true, the source content is clipped to fill the tile in both dimensions
		public const bool TileFill = false;
		public const bool TileUpsizing = true;
		public const int TileMarginPercent = 10;
		public const int TileHorizontalAlignment = 0; // -1 left, 0 center, 1 right
		public const int TileVerticalAlignment = 0; // -1 top, 0 center, 1 bottom

		public const int DziTileSize = 512; // Must be a power of 2

		public const int CollectionTileMaxLevel = 8;
		public const int CollectionTileSize = 256; // Must be a power of 2 - (int)Math.Pow(2, CollectionTileMaxLevel);

		public static readonly string DziTileFormat = "jpg";
		public static readonly Brush BackgroundBrush = Brushes.BurlyWood;
	}
}
