using System;
using System.IO;
using System.Web;
using CollectionLibrary;
using TileLibrary;

namespace JITHttpRequestLibrary
{
	internal class DzcRequest
	{
		public string CachePath { get; set; }
		public string CollectionKey { get; set; }

		public DzcRequest(HttpContext context)
		{
			try
			{
				CollectionKey = PivotCollection.ExtractCollectionKey(context.Request.Url);
				CachePath = Path.GetDirectoryName(context.Request.PhysicalPath) + "\\" + CollectionKey + ".dzc";
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public static void AnticipateServe(HttpContext cxmlContext, PivotCollection collection)
		{
			if (collection == null)
				return;

			string CachePath = Path.GetDirectoryName(cxmlContext.Request.PhysicalPath) + "\\" + collection.CollectionKey + ".dzc";
			using (MemoryStream memoryStream = new MemoryStream())
			{
				DzcSerializer.Serialize(collection, memoryStream);
				CacheFile.Save(memoryStream, CachePath);
			}
		}

		public bool Serve(HttpContext context, PivotCollection collection)
		{
			if (collection == null)
				return false;

			using (MemoryStream memoryStream = new MemoryStream())
			{
				DzcSerializer.Serialize(collection, memoryStream);
				memoryStream.Position = 0; // Rewind the stream, ready for reading
				memoryStream.WriteTo(context.Response.OutputStream);
				context.Response.ContentType = "text/xml";
				CacheFile.Save(memoryStream, CachePath);
			}

			return true;
		}
	}
}
