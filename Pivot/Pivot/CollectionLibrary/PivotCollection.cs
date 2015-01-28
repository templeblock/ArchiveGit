using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.IO;

namespace CollectionLibrary
{
	public class PivotCollection
	{
		public string FactoryName { get; set; }
		public string Name { get; set; }
		public string CollectionKey { get; private set; } // A unique value allowing this collection object to be cached
		public CultureInfo Culture { get; set; }
		public Uri IconUrl { get; set; } // URL or path to an icon file; The icon appears in the title bar unless a Brand Image is specified. It also appears in History and Quick Links.
		public Uri BrandImageUrl { get; set; } // URL or path to a Brand Image; the Brand Image appears in the title bar as a larger alternative to a traditional icon. Point to a PNG image, up to 29 pixels tall, with no width restrictions.
		public string AdditionalSearchText { get; set; } // Pivot finds Links to related web pages by conducting a web search using the item's title and then displaying the top results. Specify additional search text to make these results more specific
		public string HrefBase { get; set; }
		public string ImgBase { get; private set; }
		public IList<Item> Items { get { return _items; } }
		public IEnumerable<FacetCategory> Categories { get { foreach (var entry in _categories) { yield return entry; } } }
		public bool HasFacets { get { return _categories.Count > 0; } }
		public double SchemaVersion { get; private set; }
		public Link Copyright { get; set; }
		public string Supplement { get; set; }

		private IList<Item> _items = new List<Item>();
		private FacetCategoryCollection _categories = new FacetCategoryCollection();

		public PivotCollection(string factoryName)
		{
			SchemaVersion = 1.0;
			FactoryName = factoryName;
			CollectionKey = FactoryName + "_" + Guid.NewGuid().ToString("N");
			ImgBase = FactoryName + ".dzc?key=" + CollectionKey;
		}

		public static string ExtractCollectionKey(Uri uri)
		{
			string query = uri.Query;
			if (query == null)
				return null;
			if (query.StartsWith("?"))
				query = query.Substring(1);
			string[] pairs = query.Split('&');
			foreach (string pair in pairs)
			{
				string[] keyValue = pair.Split('=');
				if (keyValue.Length == 2 && keyValue[0] == "key")
					return keyValue[1];
			}

			return null;
		}

		public void AddFacetCategory(string category, FacetType facetType, string format = null,
			bool showInFilterPane = true, bool showInInfoPane = true, bool showInSortList = true)
		{
			FacetCategory facetCategory = _categories.TryGet(category);
			if (facetCategory != null)
			{
				if (facetCategory.FacetType != facetType)
					throw new ArgumentException(string.Format("Facet category \"{0}\" already has type \"{1}\", which does not match the requested type \"{2}\".", facetCategory.Name, facetCategory.FacetType, facetType));
				return;
			}

			facetCategory = new FacetCategory(category, facetType, format, showInFilterPane, showInInfoPane, showInSortList);
			_categories.Add(facetCategory);
		}

		// Set a format string for display of a numeric or datetime facet.
		// Setting a format string for other facet types has no effect.
		// Custom number formatting: http://msdn.microsoft.com/en-us/library/0c899ak8.aspx
		// DateTime formatting: http://msdn.microsoft.com/en-us/library/8kb3ddd4.aspx
		public void SetFacetFormat(string category, string format)
		{
			FacetCategory facetCategory = GetFacetCategory(category);
			facetCategory.DisplayFormat = format;
		}

		// Choose which parts of the Pivot user interface should display or use the given facet category
		public void SetFacetDisplay(string category, bool showInFilterPane, bool showInInfoPane, bool showInSortList)
		{
			FacetCategory facetCategory = GetFacetCategory(category);
			facetCategory.ShowInFilterPane = showInFilterPane;
			facetCategory.ShowInInfoPane = showInInfoPane;
			facetCategory.ShowInSortList = showInSortList;
		}

		private FacetCategory GetFacetCategory(string category)
		{
			FacetCategory facetCategory = _categories.TryGet(category);
			if (facetCategory == null)
				throw new ArgumentException("Facet category \"" + category + "\" has not been added.");

			return facetCategory;
		}

		public void AddItem(Item item)
		{
			if (item == null)
				return;

			if (item.Facets != null)
				EnsureFacetCategories(item.Facets);

			_items.Add(item);
		}

		public Item FindItem(int uniqueID)
		{
			foreach (Item item in Items)
			{
				if (item.UniqueID == uniqueID)
					return item;
			}

			return null;
		}

		private void EnsureFacetCategories(IEnumerable<Facet> facets)
		{
			foreach (Facet facet in facets)
			{
				AddFacetCategory(facet.Category, facet.FacetType);
			}
		}

		public void ToCxml(Stream stream)
		{
			CxmlSerializer.Serialize(stream, this);
		}

		public void ToCxml(string filePath)
		{
			using (StreamWriter writer = File.CreateText(filePath))
			{
				CxmlSerializer.Serialize(writer.BaseStream, this);
			}
		}

		public string MakeDziPath(int imageID)
		{
			return string.Format("{0}/dzi/{1}.dzi", FactoryName, imageID);
		}

		private class FacetCategoryCollection : KeyedCollection<string, FacetCategory>
		{
			public FacetCategory TryGet(string key)
			{
				return Contains(key) ? this[key] : null;
			}

			protected override string GetKeyForItem(FacetCategory facetCategory)
			{
				return facetCategory.Name;
			}
		}
	}

	public static class CollectionExtensions
	{
		// Extension for Collection<Facet>
		public static void AddFacet(this Collection<Facet> facets, string facetName, FacetType facetType, params object[] tags)
		{
			// Save memory by not adding Facets with null data
			if (tags == null || tags[0] == null)
				return;

			if (tags[0].GetType().IsArray)
				tags = tags[0] as object[];
			facets.Add(new Facet(facetName, facetType, tags));
		}

		// Extension for Collection<Facet>
		public static void AddFacet(this Collection<Facet> facets, string facetName, params object[] tags)
		{
			// Save memory by not adding Facets with null data
			if (tags == null || tags[0] == null)
				return;

			if (tags[0].GetType().IsArray)
				tags = tags[0] as object[];
			FacetType facetType = tags[0].GetType().ToFacetType();
			facets.Add(new Facet(facetName, facetType, tags));
		}
	}
}
