using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Xml.Linq;

namespace PivotServerTools.Internal
{
	// Create a collection item image from a Deep Zoom Image.
	internal class DeepZoomImage : ImageBase
	{
		private const int _invalidDimension = -1;
		private Uri _url;
		private string _imageExtension; //The file extension of images within the DZI pyramid, without the period, e.g. "jpg", "jpeg", "png".
		private int _width;
		private int _height;

		// Create an image based on a Deep Zoom Image.
		// Using this constructor will download and parse the DZI file, so may cause a performance hit.
		// Prefer using the DeepZoomImage constructor where you specify the imageExtension, width and height,
		// e.g. by storing these values with the information used to build the Collection object.
		public DeepZoomImage(Uri url)
			: this(url, null, _invalidDimension, _invalidDimension)
		{
		}

		// Create an image based on a Deep Zoom Image, using a Deep Zoom Image pyramid stored in a file and folder hierarchy.
		public DeepZoomImage(string filePath)
			: this(new Uri("file://" + filePath))
		{
		}

		// Create an image based on a Deep Zoom Image.
		// This constructor allows the image extension ("jpg", "png"), width and height to specified
		// which avoids the server downloading the DZI XML to get those values.
		public DeepZoomImage(Uri dziXmlUrl, string imageExtension, int width, int height)
		{
			_url = dziXmlUrl;
			_imageExtension = imageExtension;
			_width = width;
			_height = height;

			base.DziPath = dziXmlUrl.AbsoluteUri;
		}

		protected override void EnsureIsSize()
		{
			if (_width < 0)
			{
				// Set the image size by loading the values from the DZI file.
				LoadDziXml();
				base.Size = new Size(_width, _height);
			}
		}

		private void LoadDziXml()
		{
			string dziXml;
			using (WebClient web = new WebClient())
			{
				web.UseDefaultCredentials = true;

				// TODO: For non-sample use, evaluate whether to use asynchronous methods to release
				// this web server thread back to the IIS threadpool while waiting for download.
				// For details, see http://msdn.microsoft.com/en-us/magazine/cc164128.aspx
				dziXml = web.DownloadString(_url);
			}

			XElement xmlImage = XElement.Parse(dziXml);
			if (xmlImage.Name.LocalName != "Image")
			{
				throw new InvalidDataException("Root element is not \"Image\": " + _url);
			}

			XNamespace xmlns = "http://schemas.microsoft.com/deepzoom/2008";

			_imageExtension = xmlImage.Attribute("Format").Value;

			XElement xmlSize = xmlImage.Element(xmlns + "Size");
			_width = int.Parse(xmlSize.Attribute("Width").Value);
			_height = int.Parse(xmlSize.Attribute("Height").Value);
		}

		protected override void EnsureIsLoaded()
		{
			if (ImageDataInternal != null)
				return;

			// Save the image size we loaded from the DZI XML because base.EnsureIsLoaded() will
			// overwrite it with the size of the level 8 image we load from the pyramid.
			Size realSize = base.Size;

			base.EnsureIsLoaded();

			// Set the full size again.
			base.Size = realSize;
		}

		protected override Image MakeImage()
		{
			// Use the level 8 DZI image for drawing onto the collection tile, because it fits into 256x256.
			Uri imageUrl = GetSubImageUrl(_url, _imageExtension, 8, 0, 0);

			using (WebClient web = new WebClient())
			{
				web.UseDefaultCredentials = true;
				using (Stream webStream = web.OpenRead(imageUrl))
					return Image.FromStream(webStream);
			}
		}

		private static Uri GetSubImageUrl(Uri dziUrl, string imageExtension, int level, int x, int y)
		{
			// Take the Url to the dzi and strip off the extension.
			string fullUrl = dziUrl.AbsoluteUri;
			string baseUrl = fullUrl.Substring(0, fullUrl.LastIndexOf('.'));
			string imageUrl = string.Format("{0}_files/{1}/{2}_{3}.{4}", baseUrl, level, x, y, imageExtension);
			return new Uri(imageUrl);
		}
	}
}
