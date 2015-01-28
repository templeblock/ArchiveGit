#if CollectionCachingIsMemoryIntensize
using System;
using System.Web;
using System.Web.Caching;
using CollectionLibrary;

namespace JITHttpRequestLibrary
{
	// A cache of collection objects, to use for servicing the subsequent requests for DZC and collection image tiles.
	// This implementation just uses the global HttpRuntime Cache object. For a multi-server farm,
	// use a centralized or distributed cache so that all the servers have access to the same cached objects.
	public class CollectionCache
	{
		// The default cache expiration policy is set to a 20 minute sliding window.
		static readonly TimeSpan s_cacheExpiryDuration = new TimeSpan(0, 20, 0);
		static CollectionCache s_instance;

		public static CollectionCache Instance { get { if (s_instance == null) s_instance = new CollectionCache(); return s_instance; } }

		public CollectionCache()
		{
		}

		public void Add(string key, PivotCollection collection)
		{
			if (collection != null)
				HttpRuntime.Cache.Insert(key, collection, null, Cache.NoAbsoluteExpiration, s_cacheExpiryDuration);
		}

		public PivotCollection Get(string key)
		{
			if (key == null)
				return null;
			return HttpRuntime.Cache.Get(key) as PivotCollection;
		}
	}
}
#endif
