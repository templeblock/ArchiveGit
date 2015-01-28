using System;
using System.Drawing;
using System.IO;
using System.Web;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace PivotServerTools.Internal
{
	// Create an image tile within a Deep Zoom Image pyramid.
	internal class DziImageTile
	{
		private ImageProviderBase _image;
		private DziImageRequest _imageRequest;
		private int _level;
		private int _tilePixelDimension;

		public DziImageTile(ImageProviderBase image, DziImageRequest imageRequest,
			int tilePixelDimension, int overlap, string format)
		{
			_image = image;
			_imageRequest = imageRequest;
			_level = imageRequest.Level;
			_tilePixelDimension = tilePixelDimension;
		}

		public void Write(HttpResponse response)
		{
			response.ContentType = "image/jpeg";
			Write(response.OutputStream);
		}

		public void Write(Stream outStream)
		{
			BitmapSource bitmap = MakeTileBitmap();
			if (bitmap != null)
			{
				using (MemoryStream memStream = new MemoryStream())
				{
					WriteJpgToStream(memStream, bitmap);
					memStream.Position = 0; //Rewind the stream
					memStream.CopyTo(outStream);
				}
			}
		}

		private BitmapSource MakeTileBitmap()
		{
			const double dotsPerInch = 96.0;

			IDisposable disposable = null;
			try
			{
				Rect sourceRect, targetRect;
				if (!MakeSourceAndTargetRects(out sourceRect, out targetRect))
					return null;

				DrawingVisual drawingVisual = new DrawingVisual();
				using (DrawingContext drawingContext = drawingVisual.RenderOpen())
					disposable = _image.DrawPortion(drawingContext, targetRect, sourceRect, _level);

				RenderTargetBitmap tileBitmap = new RenderTargetBitmap((int)targetRect.Width, (int)targetRect.Height,
					dotsPerInch, dotsPerInch, PixelFormats.Default);
				tileBitmap.Render(drawingVisual);
				return tileBitmap;
			}
			finally
			{
				if (disposable != null)
					disposable.Dispose();
			}
		}

		private bool MakeSourceAndTargetRects(out Rect sourceRect, out Rect targetRect)
		{
			System.Drawing.Size worldSize = _image.Size;
			int worldLevel = GetWorldLevel(worldSize);
			int levelDelta = worldLevel - _imageRequest.Level;
			if (levelDelta < 0)
			{
				sourceRect = targetRect = Rect.Empty;
				return false; //Trying to request more detail than exists.
			}

			Rectangle worldRect = new Rectangle(new System.Drawing.Point(0, 0), _image.Size);

			int tileDimension = DziSerializer.DefaultTileSize; //This is only true for overlap=0.
			int viewDimension = tileDimension << levelDelta;
			Rectangle viewRect = new Rectangle(_imageRequest.X * viewDimension, _imageRequest.Y * viewDimension,
				viewDimension, viewDimension);
			viewRect.Intersect(worldRect); //clamp the view so it isn't larger than the world rect.

			//Now calculate the target size, which is the view rect scaled down.
			int targetWidth = viewRect.Width >> levelDelta;
			int targetHeight = viewRect.Height >> levelDelta;

			sourceRect = new Rect(viewRect.X, viewRect.Y, viewRect.Width, viewRect.Height);
			targetRect = new Rect(0, 0, targetWidth, targetHeight);
			return true;
		}

		int GetWorldLevel(System.Drawing.Size worldSize)
		{
			int worldMaxDimension = Math.Max(worldSize.Width, worldSize.Height);
			return MaxLevelForDimension(worldMaxDimension);
		}

		private int MaxLevelForDimension(int dimension)
		{
			--dimension;
			int maxLevel = 0;
			while (dimension > 0)
			{
				dimension >>= 1;
				++maxLevel;
			}
			return maxLevel;
		}

		private void WriteJpgToStream(Stream stream, BitmapSource bitmap)
		{
			BitmapEncoder encoder = new JpegBitmapEncoder();
			encoder.Frames.Add(BitmapFrame.Create(bitmap));
			encoder.Save(stream); //Note, cannot save to an HttpResponseStream
		}
	}
}
