using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Web;

// http://www.silverlightshow.net/items/Deep-zooming-on-the-fly.aspx
namespace Pivot.Web
{
	public class TileHandler : IHttpHandler
	{
		private static readonly Dictionary<string, object> _imageLocks = new Dictionary<string, object>();
		private static readonly object _sync = new object();
		private const string _folderName = "/Temp";

		public bool IsReusable { get { return true; } }

		public void ProcessRequest(HttpContext context)
		{
			context.Response.ContentType = "image/jpeg";

			NameValueCollection queryString = context.Request.QueryString;
			int tileLevel = int.Parse(queryString["tileLevel"]);
			int tilePositionX = int.Parse(queryString["tilePositionX"]);
			int tilePositionY = int.Parse(queryString["tilePositionY"]);
			int tileHeight = int.Parse(queryString["tileHeight"]);
			int tileWidth = int.Parse(queryString["tileWidth"]);

			string fileName = Path.GetFileNameWithoutExtension(context.Request.Url.LocalPath);
			string tileFolder = context.Server.MapPath(_folderName) + "\\" + fileName;

			if (!Directory.Exists(tileFolder))
			{
				lock (GetImageLock(fileName))
				{
					if (!Directory.Exists(tileFolder))
					{
						string sourceImagePath = context.Request.PhysicalPath;
						int index = sourceImagePath.LastIndexOf("_tile");
						if (index >= 0)
							sourceImagePath = sourceImagePath.Remove(index);
						CreateTiles(sourceImagePath, tileFolder, tileWidth, tileHeight);
						RemoveImageLock(fileName);
					}
				}
			}

			string tilePath = string.Format("{0}\\{1}\\{2}_{3}.jpg", tileFolder, tileLevel, tilePositionX, tilePositionY);
			using (FileStream tileFileStream = new FileStream(tilePath, FileMode.Open))
			{
				using (MemoryStream memoryStream = new MemoryStream())
				{
					memoryStream.SetLength(tileFileStream.Length);
					tileFileStream.Read(memoryStream.GetBuffer(), 0, (int)tileFileStream.Length);
					memoryStream.WriteTo(context.Response.OutputStream);
				}
			}
		}

		private void CreateTiles(string sourceImagePath, string tileFolder, int tileWidth, int tileHeight)
		{
			if (!Directory.Exists(tileFolder))
				Directory.CreateDirectory(tileFolder);

			using (FileStream imageFileStream = new FileStream(sourceImagePath, FileMode.Open))
			{
				using (Image originalImage = Image.FromStream(imageFileStream))
				{
					double tileLevel = 0;
					double scaleRate = originalImage.Width/1;
					while (scaleRate > 1)
					{
						double size = Math.Pow(2, tileLevel);
						scaleRate = (originalImage.Width / size) > 1 ? originalImage.Width / size : 1;

						string tileLevelFolder = string.Format("{0}\\{1}", tileFolder, tileLevel);
						if (!Directory.Exists(tileLevelFolder))
							Directory.CreateDirectory(tileLevelFolder);

						using (Image scaledImage = ScaleImage(originalImage, scaleRate))
						{
							int tilePositionX = 0;
							for (int i = 0; i < scaledImage.Width; i += tileWidth)
							{
								int tilePositionY = 0;
								for (int j = 0; j < scaledImage.Height; j += tileHeight)
								{
									using (Bitmap tileImage = CreateTileImage(i, j, tileWidth, tileHeight, scaledImage))
									{
										ImageCodecInfo[] info = ImageCodecInfo.GetImageEncoders();
										EncoderParameters encoderParameters = new EncoderParameters(1);
										encoderParameters.Param[0] = new EncoderParameter(Encoder.Quality, 100L);
										string tilePath = string.Format("{0}\\{1}_{2}.jpg", tileLevelFolder, tilePositionX, tilePositionY);

										using (FileStream tileFileStream = new FileStream(tilePath, FileMode.OpenOrCreate))
										{
											tileImage.Save(tileFileStream, info[1], encoderParameters);
										}
									}

									tilePositionY++;
								}

								tilePositionX++;
							}

							tileLevel++;
						}
					}
				}
			}
		}

		private Bitmap CreateTileImage(int tileLeft, int tileTop, int tileWidth, int tileHeight, Image scaledImage)
		{
			Rectangle srcRectnagle = new Rectangle(tileLeft, tileTop,
				tileLeft + tileWidth < scaledImage.Width ? tileWidth : scaledImage.Width - tileLeft,
				tileTop + tileHeight < scaledImage.Height ? tileHeight : scaledImage.Height - tileTop);
			Rectangle dstRectangle = new Rectangle(0, 0, srcRectnagle.Width, srcRectnagle.Height);
			Bitmap tileImage = new Bitmap(dstRectangle.Right, dstRectangle.Bottom);
			using (Graphics graphic = Graphics.FromImage(tileImage))
			{
				graphic.InterpolationMode = InterpolationMode.HighQualityBicubic;
				graphic.SmoothingMode = SmoothingMode.HighQuality;
				graphic.PixelOffsetMode = PixelOffsetMode.HighQuality;
				graphic.CompositingQuality = CompositingQuality.HighQuality;

				graphic.DrawImage(scaledImage, dstRectangle, srcRectnagle, GraphicsUnit.Pixel);
			}

			return tileImage;
		}

		private Image ScaleImage(Image originalImage, double scaleRate)
		{
			Rectangle scaleRect = new Rectangle(0, 0, Convert.ToInt32(originalImage.Width / scaleRate), Convert.ToInt32(originalImage.Height / scaleRate));
			Image scaledImage = new Bitmap(scaleRect.Right, scaleRect.Bottom);
			using (Graphics graphic = Graphics.FromImage(scaledImage))
			{
				graphic.InterpolationMode = InterpolationMode.HighQualityBicubic;
				graphic.SmoothingMode = SmoothingMode.HighQuality;
				graphic.PixelOffsetMode = PixelOffsetMode.HighQuality;
				graphic.CompositingQuality = CompositingQuality.HighQuality;

				graphic.DrawImage(originalImage, scaleRect, new Rectangle(0, 0, originalImage.Width, originalImage.Height), GraphicsUnit.Pixel);
			}

			return scaledImage;
		}

		private object GetImageLock(string fileName)
		{
			lock (_sync)
			{
				object imageLock;
				if (_imageLocks.ContainsKey(fileName))
					imageLock = _imageLocks[fileName];
				else
				{
					imageLock = new object();
					_imageLocks.Add(fileName, imageLock);
				}

				return imageLock;
			}
		}

		private void RemoveImageLock(string fileName)
		{
			lock (_sync)
			{
				_imageLocks.Remove(fileName);
			}
		}
	}
}
