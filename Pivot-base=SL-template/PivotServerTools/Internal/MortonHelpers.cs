using System;

namespace PivotServerTools.Internal
{
	// Deep Zoom uses a Morton fractal layout to place images on image tiles.
	// See: http://www.getpivot.com/developer-info/image-content.aspx#Deep_Zoom_Images
	public static class MortonHelpers
	{
		// Convert an x,y coordinate into the Morton number.
		// bit count is the number of bits in each of x and y.
		public static int XYToMorton(int bitCount, int x, int y)
		{
			int bitMask = 1 << bitCount;
			int morton = 0;
			for (int i = 0; i < bitCount; ++i)
			{
				bitMask >>= 1;
				morton <<= 2;
				int isY = ((y & bitMask) != 0) ? 1 : 0;
				int isX = ((x & bitMask) != 0) ? 1 : 0;
				morton |= (isY << 1) | (isX);
			}
			return morton;
		}

		// For a given level in a pyramid, convert the x,y of a tile into a morton number
		// and also return the number of items in the tile
		public static int LevelXYToMorton(int level, int x, int y, int maxLevel, out int mortonRange)
		{
			int morton = XYToMorton(level, x, y);
			morton <<= (2 * (maxLevel - level));
			mortonRange = 1 << (2 * (maxLevel - level));
			return morton;
		}

		// At a given level, return the number of items along one side of a tile
		public static int TileImageDimensionAtLevel(int level, int maxLevel)
		{
			return 1 << (maxLevel - level);
		}

		// Return the morton number for a tile and item x,y.
		public static int LevelTileXYCoordXYToMorton(int level, int tileX, int tileY, int maxLevel, int x, int y)
		{
			int mortonRange;
			int mortonStart = LevelXYToMorton(level, tileX, tileY, maxLevel, out mortonRange);

			//within this tile, convert the x and y to a morton number.
			int morton = XYToMorton(maxLevel - level, x, y);
			return mortonStart + morton;
		}
	}
}
