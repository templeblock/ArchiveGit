using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace PivotServerTools.Internal
{
	// The base class for item image implementations that create the image bytes from some source.
	// Just override the MakeImage() method.
	internal abstract class ImageBase : ImageProviderBase
	{
		public override Size Size { get { EnsureIsSize(); return _size; } protected set { _size = value; } }
		public override byte[] ImageData { get { EnsureIsLoaded(); return _imageData; } }
		protected byte[] ImageDataInternal { get { return _imageData; } }

		private Size _size;
		private byte[] _imageData;

		public ImageBase()
		{
		}

		// Override this method to return the desired Image object to display.
		protected abstract Image MakeImage();

		public override void Draw(Graphics g, Rectangle itemRectangle, int level)
		{
			EnsureIsLoaded();

			using (MemoryStream stream = new MemoryStream(_imageData))
			using (Image image = Image.FromStream(stream))
			{
				Size scaledSize = FileImage.ScaleToFillSize(image.Size, itemRectangle.Size);
				g.DrawImage(image, itemRectangle.X, itemRectangle.Y, scaledSize.Width, scaledSize.Height);
			}
		}

		// If the image is a constant size, you may override this to set the size directly.
		protected virtual void EnsureIsSize()
		{
			EnsureIsLoaded();
		}

		protected virtual void EnsureIsLoaded()
		{
			//Note, this method can be called concurrently by multiple image-request threads.
			// TODO: For non-sample use, lock the portion of code that loads the image to avoid
			// duplicating work, or implement a shared queue that manages asynchronous loading of all source images.

			if (_imageData != null)
				return;

			try
			{
				using (Image image = MakeImage())
				{
					_size = image.Size;
					using (MemoryStream stream = new MemoryStream())
					{
						image.Save(stream, ImageFormat.Jpeg);
						_imageData = stream.ToArray();
					}
				}
			}
			catch
			{
			}
		}

		internal static Size ScaleToFillSize(Size size, Size maxSize)
		{
			Size newSize = new Size();
			double aspectRatio = ((double)size.Width) / size.Height;
			if (aspectRatio > 1.0)
			{
				newSize.Width = maxSize.Width;
				newSize.Height = (int)((double)newSize.Width / aspectRatio);
			}
			else
			{
				newSize.Height = maxSize.Height;
				newSize.Width = (int)(newSize.Height * aspectRatio);
			}

			return newSize;
		}
	}
}
