using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace BusinessApplication1.Controls
{
	public class TiledBackground : UserControl
	{
		private Image _tiledImage = new Image();
		private BitmapImage _bitmap;
		private int _lastWidth, _lastHeight = 0;
		private WriteableBitmap _sourceBitmap;

		public TiledBackground()
		{
			// create an image as the content of the control
			_tiledImage.Stretch = Stretch.None;
			base.Content = _tiledImage;

			// no sizechanged to override
			base.SizeChanged += new SizeChangedEventHandler(TiledBackground_SizeChanged);
		}

		void TiledBackground_SizeChanged(object sender, SizeChangedEventArgs e)
		{
			UpdateTiledImage();
		}

		private void UpdateTiledImage()
		{
			if (_sourceBitmap != null)
			{
				int width = (int)Math.Ceiling(base.ActualWidth);
				int height = (int)Math.Ceiling(base.ActualHeight);

				// only regenerate the image if the width/height has grown
				if (width < _lastWidth && height < _lastHeight)
					return;
				_lastWidth = width;
				_lastHeight = height;

				WriteableBitmap final = new WriteableBitmap(width, height);

				for (int x = 0; x < final.PixelWidth; x++)
				{
					for (int y = 0; y < final.PixelHeight; y++)
					{
						int tiledX = (x % _sourceBitmap.PixelWidth);
						int tiledY = (y % _sourceBitmap.PixelHeight);
						final.Pixels[y * final.PixelWidth + x] = _sourceBitmap.Pixels[tiledY * _sourceBitmap.PixelWidth + tiledX];
					}
				}

				_tiledImage.Source = final;
			}
		}

		public Uri SourceUri
		{
			get { return (Uri)GetValue(SourceUriProperty); }
			set { SetValue(SourceUriProperty, value); }
		}
		public static readonly DependencyProperty SourceUriProperty =
			DependencyProperty.Register("SourceUri", typeof(Uri), typeof(TiledBackground),
			new PropertyMetadata(null, new PropertyChangedCallback(OnSourceUriChanged)));

		private static void OnSourceUriChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			((TiledBackground)d).OnSourceUriChanged(e);
		}

		protected virtual void OnSourceUriChanged(DependencyPropertyChangedEventArgs e)
		{
			_bitmap = new BitmapImage(e.NewValue as Uri);
			_bitmap.CreateOptions = BitmapCreateOptions.None;
			_bitmap.ImageOpened += OnBitmapImageOpened;
		}

		private void OnBitmapImageOpened(object sender, RoutedEventArgs e)
		{
			_sourceBitmap = new WriteableBitmap(_bitmap);
			UpdateTiledImage();
		}
	}
}
