using System.Drawing;
using System.Drawing.Drawing2D;

namespace TileLibrary
{
	// Create an item image by drawing it, using the name and description
	internal class DynamicImage : ImageProviderBase
	{
		private string _title;
		private string _description;

		public DynamicImage(string title, string description, Size? commonSize)
		{
			_title = title;
			_description = description;
			base.CommonSize = commonSize;
			if (base.CommonSize == null)
				base.Size = new Size(AllTileDefaults.DziTileSize, AllTileDefaults.DziTileSize);
		}

		protected override Image MakeImage()
		{
			int width = base.Size.Width;
			int height = base.Size.Height;
			Bitmap bitmap = new Bitmap(width, height);
			try
			{
				using (Graphics graphics = Graphics.FromImage(bitmap))
				{
					Rectangle rect = new Rectangle(0, 0, width - 1, height - 1);
					DrawContent(graphics, rect);
				}
			}
			catch
			{
				bitmap.Dispose();
				bitmap = null;
			}

			return bitmap;
		}

		private void DrawContent(Graphics graphics, Rectangle rect)
		{
			using (LinearGradientBrush brush = new LinearGradientBrush(new Point(0, 0),
				new Point(rect.Width, rect.Height), Color.White, Color.LightGreen))
			{
				graphics.FillRectangle(brush, rect);
			}

			if (!string.IsNullOrEmpty(_title))
			{
				using (Font titleFont = new Font(FontFamily.GenericSansSerif, 18.0f, FontStyle.Bold))
				{
					graphics.DrawString(_title, titleFont, Brushes.Black, rect);

					// Update the rect to position the body text
					SizeF titleSize = graphics.MeasureString(_title, titleFont, rect.Width);
					int titleHeight = (int)titleSize.Height + 1;
					rect.Offset(0, titleHeight);
					rect.Height -= titleHeight;
				}
			}

			if (!string.IsNullOrEmpty(_description))
			{
				using (Font bodyFont = new Font(FontFamily.GenericSerif, 12.0f, FontStyle.Regular))
				{
					rect.Inflate(-2, 0);
					graphics.DrawString(_description, bodyFont, Brushes.Black, rect);
				}
			}
		}
	}
}
