using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;

namespace TileLibrary
{
	// Draw collection tiles
	internal class CollectionTile : IDisposable
	{
		private readonly Bitmap _bitmapOutput;
		private readonly Graphics _graphics;
		private readonly int _subTilesize;

		public CollectionTile(int tileSize, int subTilesize)
		{
			_subTilesize = subTilesize;
			_bitmapOutput = new Bitmap(tileSize, tileSize);
			_bitmapOutput.SetResolution(100, 100);

			_graphics = Graphics.FromImage(_bitmapOutput);
			_graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
			_graphics.SmoothingMode = SmoothingMode.None;
			_graphics.PageUnit = GraphicsUnit.Pixel;
			_graphics.FillRectangle(Brushes.Black, 0, 0, _bitmapOutput.Width, _bitmapOutput.Height);
		}

		public void Load(Stream inputStream)
		{
			try
			{
				using (Image image = Image.FromStream(inputStream))
				{
					Rectangle targetRect = new Rectangle(0, 0, image.Width, image.Height);
					_graphics.DrawImage(image, targetRect);
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public void Draw(ImageProviderBase imageProvider, int x, int y)
		{
			try
			{
				if (imageProvider == null || imageProvider.Image == null)
					return;

				Size size = imageProvider.Size;
				while (Math.Max(size.Width, size.Height) > _subTilesize)
				{
					if ((size.Width & 1) == 1) size.Width++;
					size.Width /= 2;
					if ((size.Height & 1) == 1) size.Height++;
					size.Height /= 2;
				}

				if (size.Width == 0) size.Width = 1;
				if (size.Height == 0) size.Height = 1;

				Rectangle dstRect = new Rectangle(x, y, size.Width, size.Height);
				_graphics.FillRectangle(AllTileDefaults.BackgroundBrush, x, y, size.Width, size.Height);
				double rate = Scale.ScaleToFit(ref dstRect, imageProvider.Image.Width, imageProvider.Image.Height,
					AllTileDefaults.TileFill, AllTileDefaults.TileUpsizing, AllTileDefaults.TileMarginPercent,
					AllTileDefaults.TileHorizontalAlignment, AllTileDefaults.TileVerticalAlignment);

				_graphics.DrawImage(imageProvider.Image, dstRect);
				// Cover up and over-painting
				_graphics.FillRectangle(Brushes.Black, x + size.Width, y, _subTilesize - size.Width, _subTilesize);
				_graphics.FillRectangle(Brushes.Black, x, y + size.Height, _subTilesize, _subTilesize - size.Height);
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
