using System.Web;
using PivotServerTools;

namespace Pivot.Web
{
	// Handle requests for CXML, DZC, DZI, ImageTile and DeepZoomImage files. See the associated entries in web.config.
	// This handler finds all implementations of CollectionFactoryBase in any assembly in the bin folder.
	// To add your own collection factory using this method, add a class that implements CollectionFactoryBase
	// into the CollectionFactories assembly.

	// You can create your own CXML handler by uncommenting this sample implementation
	// and adding a corresponding entry in the handlers section of web.config.
	// e.g., <add name="MyCXML" verb="GET" path="my.cxml" type="PivotServer.MyCxmlHandler"/>
	//public class MyCxmlHandler : IHttpHandler
	//{
	//    public bool IsReusable { get { return true; } }

	//    public void ProcessRequest(HttpContext context)
	//    {
	//        Collection collection = new Collection();
	//        collection.Name = "My specific collection";
	//        for (int i = 0; i < 10; ++i)
	//            collection.AddItem(i.ToString(), null, null, null);

	//        PivotHttpHandlers.ServeCxml(context, collection);
	//    }
	//}

	public class CxmlHandler : IHttpHandler
	{
		public bool IsReusable { get { return true; } }

		public void ProcessRequest(HttpContext context)
		{
			PivotHttpHandlers.ServeCxml(context);
		}
	}

	public class DzcHandler : IHttpHandler
	{
		public bool IsReusable { get { return true; } }

		public void ProcessRequest(HttpContext context)
		{
			PivotHttpHandlers.ServeDzc(context);
		}
	}

	public class DziHandler : IHttpHandler
	{
		public bool IsReusable { get { return true; } }

		public void ProcessRequest(HttpContext context)
		{
			PivotHttpHandlers.ServeDzi(context);
		}
	}

	public class ImageTileHandler : IHttpHandler
	{
		public bool IsReusable { get { return true; } }

		public void ProcessRequest(HttpContext context)
		{
			PivotHttpHandlers.ServeImageTile(context);
		}
	}

	public class DziImageTileHandler : IHttpHandler
	{
		public bool IsReusable { get { return true; } }

		public void ProcessRequest(HttpContext context)
		{
			PivotHttpHandlers.ServeDziImageTile(context);
		}
	}
}
