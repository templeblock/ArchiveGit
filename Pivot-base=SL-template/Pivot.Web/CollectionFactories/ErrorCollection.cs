using System;
using PivotServerTools;

namespace CollectionFactories
{
	public static class ErrorCollection
	{
		// Create a single item collection that displays the error message from an exception.
		public static Collection FromException(Exception ex)
		{
			Collection collection = new Collection(null);
			collection.Name = "Error";

			string title = ex.Message;
			string summary = (ex.InnerException == null ? null : ex.InnerException.Message);
			collection.AddItem(title, null, summary, null);
			return collection;
		}
	}
}
