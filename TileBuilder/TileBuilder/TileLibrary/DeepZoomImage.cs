using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Xml.Linq;

namespace TileLibrary
{
	// Create an item image from a Deep Zoom Image
	internal class DeepZoomImage : ImageProviderBase
	{
		private readonly string _filePath;
		private readonly bool _isFile;
		private string _extension; // The extension used within the DZI pyramid, without the dot, e.g. "jpg", "png"

		// Create an image based on a Deep Zoom file and folder hierarchy
		// Using a !isFile constructor will download and parse the DZI file, so may cause a performance hit
		// Prefer using the isFile constructor where you specify the common size
		public DeepZoomImage(string filePath, bool isFile, Size? commonSize = null)
		{
			_filePath = filePath;
			_isFile = isFile;
			CommonSize = commonSize;
			if (CommonSize == null)
				Size = GetSizeFromDziXml();
		}

		protected override Image MakeImage()
		{
			int maxDimension = Math.Max(Size.Width, Size.Height);
			int maxLevel = 1;
			while ((maxDimension /= 2) > 0)
				maxLevel++;

			// Use the default max level for DZI images for drawing into the collection tile
			string filePath = GetSubImagePath(_filePath, _extension ?? AllTileDefaults.DziTileFormat, maxLevel, 0, 0);
			if (_isFile)
				return Image.FromFile(filePath);

			using (WebClient web = new WebClient())
			{
				web.UseDefaultCredentials = true;
				using (Stream webStream = web.OpenRead(new Uri(filePath)))
					return Image.FromStream(webStream);
			}
		}

		private Size GetSizeFromDziXml()
		{
			try
			{
#if false
				string xmlString;
				using (WebClient web = new WebClient())
				{
					web.UseDefaultCredentials = true;
					Uri uri = new Uri((_isFile ? "file://" : "") + _filePath);
					xmlString = web.DownloadString(uri);
				}
				XElement xmlImage = XElement.Parse(xmlString);
#else
				string uri = (_isFile ? "file://" : "") + _filePath;
				XElement xmlImage = XElement.Load(uri);
#endif
				if (xmlImage.Name.LocalName != "Image")
					throw new InvalidDataException("Root element is not \"Image\": " + _filePath);

				_extension = xmlImage.Attribute("Format").Value;

				XNamespace xmlns = "http://schemas.microsoft.com/deepzoom/2008";
				XElement xmlSize = xmlImage.Element(xmlns + "Size");
				int width = int.Parse(xmlSize.Attribute("Width").Value);
				int height = int.Parse(xmlSize.Attribute("Height").Value);
				return new Size(width, height);
			}
			catch (Exception ex)
			{
				ex.GetType();
				return Size.Empty;
			}
		}

		private string GetSubImagePath(string filePath, string imageExtension, int level, int x, int y)
		{
			// Strip off the extension and add the subimage details
			string basePath = filePath.Substring(0, filePath.LastIndexOf('.'));
			return string.Format("{0}_files/{1}/{2}_{3}.{4}", basePath, level, x, y, imageExtension);
		}
	}
}
