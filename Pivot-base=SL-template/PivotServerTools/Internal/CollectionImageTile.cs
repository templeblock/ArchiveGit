using System;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace PivotServerTools.Internal
{
	// Draw collection tiles by using the System.Windows.Media imaging objects.
	internal class CollectionImageTile
	{
		private int _tileDimension;
		private int _level;
		private DrawingVisual _visual;
		private DrawingContext _context;
		private List<IDisposable> _disposables;

		public void Create(int tileDimension, int level)
		{
			_tileDimension = tileDimension;
			_level = level;
			_visual = new DrawingVisual();
			_context = _visual.RenderOpen();
			_disposables = new List<IDisposable>();
		}

		public void Close()
		{
			CloseContext();
			CloseDisposables();
		}

		public void DrawSubImage(ImageProviderBase image, int x, int y, int width, int height)
		{
			Rect itemRect = new Rect(x, y, width, height);
			IDisposable disposeAfterRender = image.Draw(_context, itemRect, _level);
			if (disposeAfterRender != null) //add the disposable to our list.
				_disposables.Add(disposeAfterRender);
		}

		public void WriteTo(Stream stream)
		{
			//EndDrawSubImages
			CloseContext();

			const double dotsPerInch = 96.0;

			RenderTargetBitmap tileBitmap = new RenderTargetBitmap(_tileDimension, _tileDimension,
				dotsPerInch, dotsPerInch, PixelFormats.Default);
			tileBitmap.Render(_visual);

			CloseDisposables();

			using (MemoryStream memStream = new MemoryStream())
			{
				WriteJpgToStream(memStream, tileBitmap);
				memStream.Position = 0; //Rewind the stream
				memStream.CopyTo(stream);
			}
		}

		private void CloseContext()
		{
			if (_context != null)
			{
				_context.Close();
				_context = null;
			}
		}

		private void CloseDisposables()
		{
			if (_disposables != null)
			{
				foreach (IDisposable d in _disposables)
					d.Dispose();

				_disposables = null;
			}
		}

		private void WriteJpgToStream(Stream stream, BitmapSource bitmap)
		{
			BitmapEncoder encoder = new JpegBitmapEncoder();
			encoder.Frames.Add(BitmapFrame.Create(bitmap));
			encoder.Save(stream); //Note, cannot save to an HttpResponseStream
		}
	}
}
