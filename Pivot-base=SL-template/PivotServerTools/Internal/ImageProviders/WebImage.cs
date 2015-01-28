using System;
using System.Drawing;
using System.IO;
using System.Net;

namespace PivotServerTools.Internal
{
	// Create an item image by loading it from a URL.
	internal class WebImage : ImageBase
	{
		Uri _uri;

		public WebImage(Uri url)
		{
			_uri = url;
		}

		protected override Image MakeImage()
		{
			using (WebClient web = new WebClient())
			{
				web.UseDefaultCredentials = true;
				using (Stream webStream = web.OpenRead(_uri))
					return Image.FromStream(webStream);
			}
		}
	}
}
