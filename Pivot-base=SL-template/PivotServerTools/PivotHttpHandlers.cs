using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Web;
using PivotServerTools.Internal;

namespace PivotServerTools
{
	public static class PivotHttpHandlers
	{
		static ReaderWriterLockSlim s_lock;
		static PivotHttpHandlersImpl s_impl;

		static PivotHttpHandlers()
		{
			s_lock = new ReaderWriterLockSlim();
		}

		public static void ApplicationStart()
		{
			MakeImplementation();
		}

		public static void ApplicationEnd()
		{
			//TODO: How to synchronize this.
			//ClearImplementation();
			if (s_lock != null)
			{
				s_lock.Dispose();
				s_lock = null;
			}
		}

		public static void AddFactoriesFromFolder(string folderPath)
		{
			GetImplementation().AddFactoriesFromFolder(folderPath);
		}

		// Get the collection factory names, descriptions and sample URLs hosted by this server, as an HTML fragment
		public static string CollectionInfoHtml()
		{
			return GetImplementation().CollectionInfoHtml();
		}

		// Get the list of colection URLs hosted by this server
		public static IEnumerable<string> GetCollectionUrls()
		{
			return GetImplementation().GetCollectionUrls();
		}

		// Return a CXML response by using the relevant Collection Factory for this request.
		public static void ServeCxml(HttpContext context)
		{
			GetImplementation().ServeCxml(context);
		}

		// Return a CXML response using the given Collection object.
		public static void ServeCxml(HttpContext context, Collection collection)
		{
			GetImplementation().ServeCxml(context, collection);
		}

		public static void ServeDzc(HttpContext context)
		{
			GetImplementation().ServeDzc(context);
		}

		public static void ServeImageTile(HttpContext context)
		{
			GetImplementation().ServeImageTile(context);
		}

		public static void ServeDzi(HttpContext context)
		{
			GetImplementation().ServeDzi(context);
		}

		public static void ServeDziImageTile(HttpContext context)
		{
			GetImplementation().ServeDziImageTile(context);
		}

		static void MakeImplementation()
		{
			s_lock.EnterWriteLock();
			try
			{
				//If we're setting this, existing threads will still have their old implementation to use.
				s_impl = new PivotHttpHandlersImpl();
			}
			finally
			{
				s_lock.ExitWriteLock();
			}
		}

		static PivotHttpHandlersImpl GetImplementation()
		{
			s_lock.EnterUpgradeableReadLock();
			try
			{
				if (s_impl == null)
					MakeImplementation();

				return s_impl;
			}
			finally
			{
				s_lock.ExitUpgradeableReadLock();
			}
		}
	}

	// The instance implementation of the handlers.
	public class PivotHttpHandlersImpl
	{
		CollectionFactories _factories;
		CollectionCache _collectionCache;

		public PivotHttpHandlersImpl()
		{
			_factories = new CollectionFactories();
			_collectionCache = new CollectionCache();
		}

		public void AddFactoriesFromFolder(string folderPath)
		{
			_factories.AddFromFolder(folderPath);
		}

		// Create an HTML fragment listing the available factories.
		//TODO: Also return these as a Pivot collection.
		public string CollectionInfoHtml()
		{
			AddDefaultFactoryLocationIfNone();

			StringBuilder text = new StringBuilder();
			text.Append("<div class='PivotFactories'>");

			var sortedFactoriesByName = _factories.EnumerateFactories().OrderBy(factory => factory.Name);
			foreach (CollectionFactoryBase factory in sortedFactoriesByName)
			{
				text.Append("<div class='PivotFactory'>");
				text.AppendFormat("<div class='PivotFactoryName'>{0}</div>", factory.Name);
				if (!string.IsNullOrEmpty(factory.Summary))
				{
					//Convert any new-lines into break tags.
					string htmlSummary = HttpUtility.HtmlEncode(factory.Summary);
					htmlSummary = htmlSummary.Replace("\n", "<br/>");

					//TODO: Convert URLs into hyperlinks.

					text.AppendFormat("<div class='PivotFactorySummary'>{0}</div>", htmlSummary);
				}
				text.Append("<div class='PivotFactorySampleQueries'>");
				if (factory.SampleQueries != null && factory.SampleQueries.Count > 0)
				{
					foreach (string query in factory.SampleQueries)
					{
						string url = string.Format("{0}.cxml?{1}", factory.Name, query);
						text.AppendFormat("<div class='PivotFactorySampleQuery'><a href='{0}'>{1}</a></div>",
							url, HttpUtility.HtmlEncode(query));
					}
				}
				else
				{
					string url = factory.Name + ".cxml";
					text.AppendFormat("<div class='PivotFactorySampleQuery'><a href='{0}'>{0}</a></div>", url);
				}

				text.Append("</div>");
				text.Append("</div>");
			}

			text.Append("</div>");
			return text.ToString();
		}

		// Return the list of sample URLs provided by the collection factories.
		//TODO: Also return these as a Pivot collection.
		public IEnumerable<string> GetCollectionUrls()
		{
			AddDefaultFactoryLocationIfNone();

			var sortedFactoriesByName = _factories.EnumerateFactories().OrderBy(factory => factory.Name);
			foreach (CollectionFactoryBase factory in sortedFactoriesByName)
			{
				if (factory.SampleQueries == null || factory.SampleQueries.Count == 0)
				{
					yield return (factory.Name + ".cxml");
				}
				else
				{
					foreach (string query in factory.SampleQueries)
					{
						string url = string.Format("{0}.cxml?{1}", factory.Name, query);
						yield return url;
					}
				}
			}
		}

		public void ServeStaticFile(HttpContext context, string contentType)
		{
			// Load any static file resources
			try
			{
				using (FileStream fileStream = new FileStream(context.Request.PhysicalPath, FileMode.Open))
				{
					using (MemoryStream memoryStream = new MemoryStream())
					{
						memoryStream.SetLength(fileStream.Length);
						fileStream.Read(memoryStream.GetBuffer(), 0, (int)fileStream.Length);
						memoryStream.WriteTo(context.Response.OutputStream);
						context.Response.ContentType = contentType;
						return;
					}
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			//The requested resource doesn't exist. Return HTTP status code 404.
			context.Response.StatusCode = (int)HttpStatusCode.NotFound;
			context.Response.StatusDescription = "Resource not found.";
		}

		public void ServeCxml(HttpContext context)
		{
			AddDefaultFactoryLocationIfNone();

			string factoryKey = Collection.ComputeFactoryKey(context.Request.Url);
			CollectionFactoryBase factory = _factories.Get(factoryKey);
			if (factory == null)
			{
				ServeStaticFile(context, "text/xml");
				return;
			}

			string collectionKey = Collection.ComputeCollectionKey(context.Request.Url);
			Collection collection = (collectionKey != null ? _collectionCache.Get(collectionKey) : null);
			collection = factory.MakeCollection(new CollectionRequestContext(collection, context));
			if (collection == null)
			{
				ServeStaticFile(context, "text/xml");
				return;
			}

			ServeCxml(context, collection);
		}

		public void ServeCxml(HttpContext context, Collection collection)
		{
			_collectionCache.Add(collection.CollectionKey, collection);

			context.Response.ContentType = "text/xml";
			collection.ToCxml(context.Response.Output);
		}

		public void ServeDzc(HttpContext context)
		{
			string collectionKey = Collection.ComputeCollectionKey(context.Request.Url);
			Collection collection = _collectionCache.Get(collectionKey);
			if (collection == null)
			{
				ServeStaticFile(context, "text/xml");
				return;
			}

			context.Response.ContentType = "text/xml";
			collection.ToDzc(context.Response.Output);
		}

		public void ServeDzi(HttpContext context)
		{
			DziRequest request = new DziRequest(context.Request.Url);
			Collection collection = _collectionCache.Get(request.CollectionKey);
			if (collection == null)
			{
				ServeStaticFile(context, "text/xml");
				return;
			}

			CollectionItem item = collection.Items[request.ItemId];
			ImageProviderBase image = item.ImageProvider;

			context.Response.ContentType = "text/xml";
			DziSerializer.Serialize(context.Response.Output, image.Size);
		}

		public void ServeDziImageTile(HttpContext context)
		{
			DziImageRequest request = new DziImageRequest(context.Request.Url);
			Collection collection = _collectionCache.Get(request.CollectionKey);
			if (collection == null)
			{
				ServeStaticFile(context, "image/jpeg");
				return;
			}

			CollectionItem item = collection.Items[request.ItemId];
			ImageProviderBase image = item.ImageProvider;

			DziImageTile imageTile = new DziImageTile(image, request,
				DziSerializer.DefaultTileSize, DziSerializer.DefaultOverlap, DziSerializer.DefaultFormat);
			imageTile.Write(context.Response);
		}

		public void ServeImageTile(HttpContext context)
		{
			ImageRequest request = new ImageRequest(context.Request.Url);
			Collection collection = _collectionCache.Get(request.CollectionKey);
			if (collection == null)
			{
				ServeStaticFile(context, "image/jpeg");
				return;
			}

			CollectionImageTileBuilder builder = new CollectionImageTileBuilder(collection, request,
				DzcSerializer.DefaultMaxLevel, DzcSerializer.DefaultTileDimension);
			builder.Write(context.Response);
		}

		private void AddDefaultFactoryLocationIfNone()
		{
			if (_factories.Count == 0)
			{
				string defaultAssemblyFolder = HttpRuntime.BinDirectory;
				AddFactoriesFromFolder(defaultAssemblyFolder);
			}
		}
	}
}
