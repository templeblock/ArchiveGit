using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Threading;
using System.Web;
using CollectionLibrary;
using FactoriesLibrary;
using ImageProviderForItem;
using TileLibrary;
using System.IO;

namespace JITHttpRequestLibrary
{
	public static class PivotHttpHandlers
	{
		private static ReaderWriterLockSlim s_lock = new ReaderWriterLockSlim();
		private static PivotHttpHandlersImpl s_impl;

		static PivotHttpHandlers()
		{
		}

		public static void ApplicationStart()
		{
			MakeImplementation();
		}

		public static void ApplicationEnd()
		{
			//TODO: How to synchronize this
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

		// Get the list of colection URLs hosted by this server
		public static IEnumerable<string> GetCollectionUrls()
		{
			return GetImplementation().GetCollectionUrls();
		}

		// Return a CXML response by using the relevant Collection Factory for this request
		public static void ServeCxml(HttpContext context)
		{
			GetImplementation().ServeCxml(context);
		}

		public static void ServeDzc(HttpContext context)
		{
			GetImplementation().ServeDzc(context);
		}

		public static void ServeCollectionTile(HttpContext context)
		{
			GetImplementation().ServeCollectionTile(context);
		}

		public static void ServeDzi(HttpContext context)
		{
			GetImplementation().ServeDzi(context);
		}

		public static void ServeDziTile(HttpContext context)
		{
			GetImplementation().ServeDziTile(context);
		}

		private static void MakeImplementation()
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

		private static PivotHttpHandlersImpl GetImplementation()
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
#if DEBUG
		private readonly bool IsDebug = true;
#else
		private readonly bool IsDebug = false;
#endif
		private CollectionFactories _factories = new CollectionFactories();
		//private CollectionCache _collectionCache = new CollectionCache();

		public PivotHttpHandlersImpl()
		{
		}

		public void AddFactoriesFromFolder(string folderPath)
		{
			_factories.AddFromFolder(folderPath);
		}

		public void ServeCxml(HttpContext context)
		{
			AddDefaultFactoryLocationIfNone();
			context.Response.Cache.SetNoStore();

			CxmlRequest request = new CxmlRequest(context);
			CollectionFactoryBase factory = _factories.Get(request.FactoryName);
			if (factory != null)
			{
				PivotCollection collection = factory.MakeCollection(new CollectionRequestContext(context));
				if (collection != null)
				{
					//_collectionCache.Add(collection.CollectionKey, collection);
					if (request.Serve(context, collection, IsDebug/*cacheToDisk*/))
					{ // Anticipate the DZC file request and create it while we have our hands on the collection
						DzcRequest.AnticipateServe(context, collection);
						return;
					}
				}
			}

			CacheFile.Serve(request.CachePath, context, "text/xml");
		}

		public void ServeDzc(HttpContext context)
		{
			DzcRequest request = new DzcRequest(context);
			//if (!CacheFile.Exists(request.CachePath))
			//{
			//    PivotCollection collection = _collectionCache.Get(request.CollectionKey);
			//    if (request.Serve(context, collection, IsDebug/*cacheToDisk*/))
			//        return;
			//}

			CacheFile.Serve(request.CachePath, context, "text/xml");
			// PivtViewer makes are 2 requests for the DZC for some reason, so don't delete the cached file
			//if (!IsDebug && CacheFile.Exists(request.CachePath))
			//    CacheFile.Delete(request.CachePath);
		}

		public void ServeCollectionTile(HttpContext context)
		{
#if JITCollectionTilesAreNotPractical
			CollectionTileRequest request = new CollectionTileRequest(context);
			PivotCollection collection = _collectionCache.Get(request.CollectionKey);
			if (request.Serve(collection, context, true/*cacheToDisk*/))
				return;
#endif
			CacheFile.Serve(context.Request.PhysicalPath, context, "image/jpeg");
		}

		public void ServeDzi(HttpContext context)
		{
			DziRequest request = new DziRequest(context);
			// DZI files are too small to bother caching
			//if (!CacheFile.Exists(request.CachePath))
			{
				Size imageSize = GetImageSizeFromFactory(request.FactoryName, request.ImageID);
				if (request.Serve(context, imageSize, false/*cacheToDisk*/))
					return;
			}

			//CacheFile.Serve(request.CachePath, context, "text/xml");
		}

		public void ServeDziTile(HttpContext context)
		{
			DziTileRequest request = new DziTileRequest(context);
			if (!CacheFile.Exists(request.CachePath))
			{
				ImageProviderBase imageProvider = CreateImageProviderFromFactory(request.FactoryName, request.ImageID);
				if (request.Serve(context, imageProvider, true/*cacheToDisk*/))
					return;
			}

			CacheFile.Serve(request.CachePath, context, "image/jpeg");
		}

		private Size GetImageSizeFromFactory(string factoryName, int imageID)
		{
			ItemImage itemImage = CreateItemImageFromFactory(factoryName, imageID);
			if (itemImage.CommonSize != null)
				return (Size)itemImage.CommonSize;
			ImageProviderBase imageProvider = itemImage.CreateImageProvider();
			if (imageProvider == null)
				return Size.Empty;
			return imageProvider.Size;
		}

		private ItemImage CreateItemImageFromFactory(string factoryName, int imageID)
		{
			CollectionFactoryBase factory = _factories.Get(factoryName);
			if (factory == null)
				return null;
			return factory.MakeItemImage(imageID);
		}

		private ImageProviderBase CreateImageProviderFromFactory(string factoryName, int imageID)
		{
			ItemImage itemImage = CreateItemImageFromFactory(factoryName, imageID);
			return itemImage.CreateImageProvider();
		}

		// Return the list of sample URLs provided by the collection factories
		public IEnumerable<string> GetCollectionUrls()
		{
			AddDefaultFactoryLocationIfNone();

			var sortedFactoriesByName = _factories.EnumerateFactories().OrderBy(factory => factory.FactoryName);
			foreach (CollectionFactoryBase factory in sortedFactoriesByName)
			{
				if (factory.SampleQueries == null || factory.SampleQueries.Count == 0)
				{
					string url = string.Format("{0}.cxml", factory.FactoryName);
					yield return url;
				}
				else
				{
					foreach (string query in factory.SampleQueries)
					{
						string url = string.Format("{0}.cxml?{1}", factory.FactoryName, query);
						yield return url;
					}
				}
			}
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
