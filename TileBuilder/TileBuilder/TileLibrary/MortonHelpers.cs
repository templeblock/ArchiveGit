namespace TileLibrary
{
	// Deep Zoom uses a Morton fractal layout to place images on image tiles.
	// See: http://www.getpivot.com/developer-info/image-content.aspx#Deep_Zoom_Images
	public static class MortonHelpers
	{
		// Convert a Morton number and a level to its tile and subtile coordinates
		internal static void MortonBreakdown(int morton, int level, int maxLevel, out int tileX, out int tileY, out int subTileX, out int subTileY)
		{
			int x,y;
			MortonToXY(morton, out x, out y);
			int levelsHi = level;
			int levelsLo = maxLevel - level;
			int maskLo = BitMask(levelsLo);
			int maskHi = BitMask(levelsHi);
			tileX = (x >> levelsLo) & maskHi;
			tileY = (y >> levelsLo) & maskHi;
			subTileX = x & maskLo;
			subTileY = y & maskLo;
		}

		private static int BitMask(int numBits)
		{
			return (1 << numBits) - 1;
		}

		// Convert a Morton number to an x,y coordinate
		internal static void MortonToXY(int morton, out int x, out int y)
		{
			const int pairsPerByte = 4;
			const int pairsPerInt = sizeof(int) * pairsPerByte;

			x = y = 0;
			for (int i = 0; i < pairsPerInt; i++)
			{
				x |= ((morton & 1) >> 0) << i;
				y |= ((morton & 2) >> 1) << i;
				morton >>= 2;
			}
		}

		// Convert an x,y coordinate to a Morton number
		internal static int XYToMorton(int x, int y)
		{
			const int pairsPerByte = 4;
			const int pairsPerInt = sizeof(int) * pairsPerByte;

			int morton = 0;
			for (int i = 0; i < pairsPerInt; i++)
			{
				morton |= (x & 1) << (i * 2);
				morton |= (y & 1) << (i * 2 + 1);
				x >>= 1;
				y >>= 1;
			}

			return morton;
		}

		// For a given level in a pyramid, convert the x,y of a tile into a morton start number
		// and also return the number of items in the tile
		public static int LevelXYToMortonStart(int level, int tileX, int tileY, int maxLevel)
		{
			int morton = XYToMorton(tileX, tileY);
			return MortonLevelToMortonStart(morton, level, maxLevel);
		}

		// At a given level, return the number of subtile items along one side of a tile
		public static int LevelToSubTileCount(int level, int maxLevel)
		{
			return 1 << (maxLevel - level);
		}

		// For a given level in a pyramid, convert the x,y of a tile into a morton start number
		private static int MortonLevelToMortonStart(int morton, int level, int maxLevel)
		{
			int mortonStart = morton << (2 * (maxLevel - level));
			return mortonStart;
		}
#if NOTUSED
		// Return the morton number for a tile and subtile
		public static int LevelTileXYCoordXYToMorton(int level, int tileX, int tileY, int maxLevel, int subTileX, int subTileY)
		{
			int tileMorton = XYToMorton(tileX, tileY);
			int mortonStart = MortonLevelToMortonStart(tileMorton, level, maxLevel);
			int morton = XYToMorton(subTileX, subTileY);
			return mortonStart + morton;
		}
#endif
#if NOTUSED
		// Convert an x,y coordinate into the Morton number
		// The bit count is the number of bits in each of x and y
		public static int XYToMortonOld(int bitCount, int x, int y)
		{ // bitcount should be: DefaultMaxLevel - level
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
#endif
	}
}
