using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;

namespace TileLibrary
{
	// Create an image tile within a Deep Zoom Image pyramid
	internal class DziTile : IDisposable
	{
		private Bitmap _bitmapOutput;
		private Graphics _graphics;
		private int _tileSize;

		public DziTile(int tileSize)
		{
			_tileSize = tileSize;
		}

		public void Draw(ImageProviderBase imageProvider, int level, int tileX, int tileY)
		{
			try
			{
				if (imageProvider == null)
					return;

				double maxTileSize = Math.Pow(2, level);
				Size size = imageProvider.Size;
				while (Math.Max(size.Width, size.Height) > maxTileSize)
				{
					if ((size.Width & 1) == 1) size.Width++;
					size.Width /= 2;
					if ((size.Height & 1) == 1) size.Height++;
					size.Height /= 2;
				}

				if (size.Width == 0) size.Width = 1;
				if (size.Height == 0) size.Height = 1;

				int overhangX = tileX * _tileSize;
				int overhangY = tileY * _tileSize;

				Size bitmapSize = new Size(Math.Min(size.Width - overhangX, _tileSize), Math.Min(size.Height - overhangY, _tileSize));
				_bitmapOutput = new Bitmap(bitmapSize.Width, bitmapSize.Height);
				_bitmapOutput.SetResolution(100, 100);

				_graphics = Graphics.FromImage(_bitmapOutput);
				_graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
				_graphics.SmoothingMode = SmoothingMode.None;
				_graphics.PageUnit = GraphicsUnit.Pixel;
				_graphics.FillRectangle(AllTileDefaults.BackgroundBrush, 0, 0, _bitmapOutput.Width, _bitmapOutput.Height);

				if (imageProvider.Image == null)
					return;

				Rectangle dstRect = new Rectangle(-overhangX, -overhangY, size.Width, size.Height);
				double rate = Scale.ScaleToFit(ref dstRect, imageProvider.Image.Width, imageProvider.Image.Height,
					AllTileDefaults.TileFill, AllTileDefaults.TileUpsizing, AllTileDefaults.TileMarginPercent,
					AllTileDefaults.TileHorizontalAlignment, AllTileDefaults.TileVerticalAlignment);

				_graphics.DrawImage(imageProvider.Image, dstRect);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public void Save(Stream stream, ImageFormat format)
		{
			try
			{
				if (_bitmapOutput != null)
					_bitmapOutput.Save(stream, format);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public void Dispose()
		{
			if (_graphics != null)
				_graphics.Dispose();
			if (_bitmapOutput != null)
				_bitmapOutput.Dispose();
		}
	}
}
