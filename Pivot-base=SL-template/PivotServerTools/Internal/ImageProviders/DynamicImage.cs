using System.Drawing;
using System.Drawing.Drawing2D;

namespace PivotServerTools.Internal
{
	// Create an image by drawing it, using the name and description. 
	internal class DynamicImage : ImageBase
	{
		static readonly Size imageSize_c = new Size(256, 256); //Default size is chosen so it fits exactly to a collection tile.
		string _title;
		string _body;

		public DynamicImage(string name, string description)
		{
			_title = name;
			_body = description;
		}

		protected override Image MakeImage()
		{
			return DrawBitmap(imageSize_c.Width, imageSize_c.Height, _title, _body);
		}

		private static Bitmap DrawBitmap(int width, int height, string title, string body)
		{
			Bitmap bitmap = new Bitmap(width, height);
			try
			{
				using (Graphics g = Graphics.FromImage(bitmap))
				{
					Rectangle rect = new Rectangle(0, 0, width - 1, height - 1);
					DrawBackground(g, rect);
					DrawContent(g, rect, title, body);
				}
			}
			catch
			{
				bitmap.Dispose();
				bitmap = null;
			}
			return bitmap;
		}

		private static void DrawBackground(Graphics g, Rectangle rect)
		{
			using (LinearGradientBrush brush = new LinearGradientBrush(new Point(0, 0),
				new Point(rect.Width, rect.Height), Color.White, Color.LightGreen))
			{
				g.FillRectangle(brush, rect);
			}
		}

		private static void DrawContent(Graphics g, Rectangle rect, string title, string body)
		{
			if (!string.IsNullOrEmpty(title))
			{
				using (Font titleFont = new Font(FontFamily.GenericSansSerif, 18.0f, FontStyle.Bold))
				{
					g.DrawString(title, titleFont, Brushes.Black, rect);

					//Update the rect to position the body text.
					SizeF titleSize = g.MeasureString(title, titleFont, rect.Width);
					int titleHeight = (int)titleSize.Height + 1;
					rect.Offset(0, titleHeight);
					rect.Height -= titleHeight;
				}
			}

			if (!string.IsNullOrEmpty(body))
			{
				using (Font bodyFont = new Font(FontFamily.GenericSerif, 12.0f, FontStyle.Regular))
				{
					rect.Inflate(-2, 0);
					g.DrawString(body, bodyFont, Brushes.Black, rect);
				}
			}
		}
	}
}
