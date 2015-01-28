using System;
using System.Drawing;
using System.IO;
using System.Net;

namespace TileLibrary
{
	// Create an item image by loading it from a file or a URL
	internal class StandardImage : ImageProviderBase
	{
		private readonly string _filePath;
		private readonly bool _isFile;

		public StandardImage(string filePath, bool isFile, Size? commonSize)
		{
			_filePath = filePath;
			_isFile = isFile;
			CommonSize = commonSize;
		}

		protected override Image MakeImage()
		{
			if (_isFile)
				return Image.FromFile(_filePath);

			using (WebClient web = new WebClient())
			{
				web.UseDefaultCredentials = true;
				using (Stream webStream = web.OpenRead(new Uri(_filePath)))
					return Image.FromStream(webStream);
			}
		}
	}
}
