using System;
using CollectionLibrary;

namespace CollectionFactories
{
	public static class ErrorCollection
	{
		// Create a single item collection that displays the error message from an exception.
		public static PivotCollection FromException(Exception ex)
		{
			PivotCollection collection = new PivotCollection("Error");
			collection.Name = "Error";

			string title = ex.Message;
			string description = (ex.InnerException == null ? null : ex.InnerException.Message);
			Item item = new Item(0, title, null, description, null);
			collection.AddItem(item);
			return collection;
		}
	}
}
