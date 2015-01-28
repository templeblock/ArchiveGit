using System.Collections.Generic;

namespace CollectionLibrary
{
	public class Item
	{
		public int UniqueID { get; set; }
		public int ImageID { get; set; }
		public string Name { get; set; }
		public string Url { get; set; }
		public string Description { get; set; }
		public ItemImage ItemImage { get; set; }
		public ICollection<Facet> Facets { get; set; }
		public ICollection<Link> RelatedLinks { get; set; }

		public Item(int uniqueID, string name, string url, string description, ItemImage image, ICollection<Facet> facets = null, ICollection<Link> relatedLinks = null)
		{
			UniqueID = uniqueID;
			ImageID = (image != null ? image.ID : 0);
			Name = name;
			Url = url;
			Description = description;
			ItemImage = (image != null ? image : new ItemImage());
			Facets = facets;
			RelatedLinks = relatedLinks;
		}
	}
}
