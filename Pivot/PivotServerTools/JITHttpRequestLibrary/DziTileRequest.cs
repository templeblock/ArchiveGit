using System;
using System.Drawing.Imaging;
using System.IO;
using System.Text.RegularExpressions;
using System.Web;
using TileLibrary;

namespace JITHttpRequestLibrary
{
	internal class DziTileRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*)/dzi/(.*)_files/(.*)/(.*)_(.*).jpg", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);

		public string CachePath { get; private set; }
		public string FactoryName { get; private set; }
		public int ImageID { get; private set; }
		public int Level { get; private set; }
		public int X { get; private set; }
		public int Y { get; private set; }

		public DziTileRequest(HttpContext context)
		{
			try
			{
				Uri url = context.Request.Url;
				Match match = s_matcher.Match(url.AbsolutePath);
				if (match.Groups.Count != 6)
					return; // This object will be rejected due to a bad url format

				CachePath = context.Request.PhysicalPath;
				FactoryName = match.Groups[1].Value;
				ImageID = int.Parse(match.Groups[2].Value);
				Level = int.Parse(match.Groups[3].Value);
				X = int.Parse(match.Groups[4].Value);
				Y = int.Parse(match.Groups[5].Value);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public bool Serve(HttpContext context, ImageProviderBase imageProvider, bool cacheToDisk)
		{
			if (imageProvider == null)
				return false;

			using (MemoryStream memoryStream = new MemoryStream())
			{
				using (DziTile tile = new DziTile(AllTileDefaults.DziTileSize))
				{
					tile.Draw(imageProvider, Level, X, Y);
					tile.Save(memoryStream, ImageFormat.Jpeg);
					memoryStream.Position = 0; // Rewind the stream, ready for reading
					memoryStream.WriteTo(context.Response.OutputStream);
					context.Response.ContentType = "image/jpeg";
					if (cacheToDisk)
						CacheFile.Save(memoryStream, CachePath);
				}
			}

			return true;
		}
	}
}
