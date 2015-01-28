using System.Collections.Generic;
using System.Collections.Specialized;
using System.Web;

namespace PivotServerTools
{
	// Contains data about the CXML query, passed to overrides of CollectionFactoryBase.MakeCollection().
	public class CollectionRequestContext
	{
		public HttpContext Context { get; set; }
		public Collection CachedCollection { get; set; }
		public NameValueCollection Query { get; private set; }
		public string Url { get; private set; }

		internal CollectionRequestContext(Collection cachedCollection, HttpContext context)
		{
			Context = context;
			CachedCollection = cachedCollection;
			Query = Context.Request.QueryString;
			Url = Context.Request.Url.ToString();
		}
	}

	// Your collection factory must derive from this class.
	// Derived instances will be detected and loaded automatically,
	// so must have a public constructor that takes no parameters.
	public abstract class CollectionFactoryBase
	{
		// The file name portion of the url, used to refer to this collection. If null, the classname is used.
		public string Name { get; set; }

		// A summary of the collection, including URL query parameters that it accepts.
		public string Summary { get; set; }

		// A list of sample queries that can be appended after the '?' in the URL.
		public ICollection<string> SampleQueries { get; set; }
		
		protected CollectionFactoryBase()
		{
		}

		// Override this method to provide a Collection object for the request.
		public abstract Collection MakeCollection(CollectionRequestContext context);
	}
}
