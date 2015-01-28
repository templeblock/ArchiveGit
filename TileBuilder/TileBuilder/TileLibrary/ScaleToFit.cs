using System;
using System.Drawing;

namespace TileLibrary
{
	class Scale
	{
		public static double ScaleToFit(ref Rectangle destination, int srcWidth, int srcHeight,
			bool fill, bool allowUpsizing, int marginPercent, int horizontalAlignment, int verticalAlignment)
		{
			int originalWidth = destination.Width;
			int originalHeight = destination.Height;
			int margin = (Math.Min(originalWidth, originalHeight) * marginPercent) / 100;

			// Scale the source window to fit the destination window...
			double scale1 = ((double)originalWidth - margin) / srcWidth;
			double scale2 = ((double)originalHeight - margin) / srcHeight;

			// Adjust the desination size, and return the scale factor
			double rate = (fill ? Math.Max(scale1, scale2) : Math.Min(scale1, scale2));
			if (!allowUpsizing && rate > 1.0)
				rate = 1.0;

			destination.Width = (int)(srcWidth * rate);
			destination.Height = (int)(srcHeight * rate);

			// Adjust for the desired horizontal alignment
			switch (horizontalAlignment)
			{
				case -1: // left
					destination.X += 0;
					break;
				case 0:
					destination.X += (originalWidth - destination.Width) / 2;
					break; // center
				case 1:
					destination.X += (originalWidth - destination.Width);
					break; // right
			}

			// Adjust for the desired vertical alignment
			switch (verticalAlignment)
			{
				case -1: // top
					destination.Y += 0;
					break;
				case 0: // center
					destination.Y += (originalHeight - destination.Height) / 2;
					break;
				case 1: // bottom
					destination.Y += (originalHeight - destination.Height);
					break;
			}

			return rate;
		}
	}
}
