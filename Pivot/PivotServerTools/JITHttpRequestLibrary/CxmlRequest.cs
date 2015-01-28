using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Web;
using CollectionLibrary;

namespace JITHttpRequestLibrary
{
	internal class CxmlRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*).cxml", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);

		public string CachePath { get; private set; }
		public string FactoryName { get; private set; }

		public CxmlRequest(HttpContext context)
		{
			try
			{
				Uri url = context.Request.Url;
				Match match = s_matcher.Match(url.AbsolutePath);
				if (match.Groups.Count != 2)
					return; // This object will be rejected due to a bad url format

				CachePath = context.Request.PhysicalPath;
				FactoryName = match.Groups[1].Value;
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public bool Serve(HttpContext context, PivotCollection collection, bool cacheToDisk)
		{
			if (collection == null)
				return false;

			CachePath = Path.GetDirectoryName(CachePath) + "\\" + collection.CollectionKey + ".cxml";
			using (MemoryStream memoryStream = new MemoryStream())
			{
				collection.ToCxml(memoryStream);
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
