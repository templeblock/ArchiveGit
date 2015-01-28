using System;
using System.Collections.Generic;

namespace PivotServerTools.Internal
{
	// Used internally to represent an item within a collection
	internal class CollectionItem
	{
		public string Name { get; set; }
		public string Url { get; set; }
		public string Description { get; set; }
		public ImageProviderBase ImageProvider { get; private set; }
		public ICollection<Facet> FacetValues { get; set; }

		public CollectionItem()
		{
		}

		public void SetImage(ItemImage image)
		{
			if (image == null) // No image set, so draw one ourselves
				ImageProvider = new DynamicImage(Name, Description);
			else
			if (!string.IsNullOrEmpty(image.ImageFilePath))
			{
				if (image.ImageFilePath.EndsWith(".dzi", StringComparison.InvariantCultureIgnoreCase))
					ImageProvider = new DeepZoomImage(image.ImageFilePath);
				else
					ImageProvider = new FileImage(image.ImageFilePath);
			}
			else
			if (image.ImageUrl != null)
			{
				if (image.ImageUrl.LocalPath.EndsWith(".dzi", StringComparison.InvariantCultureIgnoreCase))
					ImageProvider = new DeepZoomImage(image.ImageUrl);
				else
					ImageProvider = new WebImage(image.ImageUrl);
			}
		}
	}
}
