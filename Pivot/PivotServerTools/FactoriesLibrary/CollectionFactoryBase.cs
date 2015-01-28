using System.Collections.Generic;
using System.Collections.Specialized;
using System.Web;
using CollectionLibrary;

namespace FactoriesLibrary
{
	// Contains data about the CXML query, passed to overrides of CollectionFactoryBase.MakeCollection().
	public class CollectionRequestContext
	{
		public HttpContext Context { get; set; }
		public NameValueCollection Query { get; private set; }
		public string Url { get; private set; }

		internal CollectionRequestContext(HttpContext context)
		{
			Context = context;
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
		public string FactoryName { get; set; }

		// A list of sample queries that can be appended after the '?' in the URL.
		public ICollection<string> SampleQueries { get; set; }
		
		protected CollectionFactoryBase()
		{
		}

		// Override this method to provide a Collection object for the request.
		public abstract PivotCollection MakeCollection(CollectionRequestContext context);
		public abstract ItemImage MakeItemImage(int itemID);
	}
}
