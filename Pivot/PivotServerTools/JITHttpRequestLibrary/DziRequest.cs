using System;
using System.Drawing;
using System.IO;
using System.Text.RegularExpressions;
using System.Web;
using TileLibrary;

namespace JITHttpRequestLibrary
{
	internal class DziRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*)/dzi/(.*).dzi", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);

		public string CachePath { get; private set; }
		public string FactoryName { get; private set; }
		public int ImageID { get; private set; }

		public DziRequest(HttpContext context)
		{
			try
			{
				Uri url = context.Request.Url;
				Match match = s_matcher.Match(url.AbsolutePath);
				if (match.Groups.Count != 3)
					return; // This object will be rejected due to a bad url format

				CachePath = context.Request.PhysicalPath;
				FactoryName = match.Groups[1].Value;
				ImageID = int.Parse(match.Groups[2].Value);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public bool Serve(HttpContext context, Size imageSize, bool cacheToDisk)
		{
			using (MemoryStream memoryStream = new MemoryStream())
			{
				DziSerializer.Serialize(imageSize, memoryStream);
				memoryStream.Position = 0; // Rewind the stream, ready for reading
				memoryStream.WriteTo(context.Response.OutputStream);
				context.Response.ContentType = "text/xml";
				if (cacheToDisk)
					CacheFile.Save(memoryStream, CachePath);
			}

			return true;
		}
	}
}
