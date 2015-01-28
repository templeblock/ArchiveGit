using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.IO;
using System.Text;
using PivotServerTools.Internal;

namespace PivotServerTools
{
	// Use this class to create your collection and add items to it.
	public class Collection
	{
		public string Name { get; set; }
		public CultureInfo Culture { get; set; }
		public Uri IconUrl { get; set; }
		public bool EnableInfoPaneBingSearch { get; set; }
		public string CollectionKey { get; private set; } // A unique value allowing this collection object to be cached

		internal string HrefBase { get; set; }
		internal IList<CollectionItem> Items { get { return _items; } }
		internal IEnumerable<FacetCategory> FacetCategories { get { foreach (var entry in _facetCategories) { yield return entry; } } }
		internal bool HasFacets { get { return _facetCategories.Count > 0; } }
		internal double SchemaVersion { get; private set; }

		static readonly string[] reservedCategoryNames_c = { "Name", "Description" };

		private List<CollectionItem> _items = new List<CollectionItem>();
		private FacetCategoryCollection _facetCategories = new FacetCategoryCollection();

		public Collection(Uri requestUri)
		{
			SetCollectionKey(requestUri);
			SchemaVersion = 1.0;
			EnableInfoPaneBingSearch = true;
		}

		private string SetCollectionKey(Uri requestUri)
		{
			string collectionKey = ComputeCollectionKey(requestUri);
			collectionKey = (!string.IsNullOrEmpty(collectionKey) ? collectionKey : Guid.NewGuid().ToString("N"));
			CollectionKey = collectionKey;
			return CollectionKey;
		}

		public static string ComputeFactoryKey(Uri uri)
		{
			// The factory key is computed to be the beginning of the localpath, and if the filename, without the extension
			if (uri == null || string.IsNullOrWhiteSpace(uri.LocalPath) || uri.LocalPath.Length <= 1)
				return null;
			string localPath = uri.LocalPath.Substring(1);
			int index1 = localPath.IndexOf('/');
			int index2 = localPath.LastIndexOf('.');
			int index = (index1 >= 0 && index1 < index2 ? index1 : index2);
			string key = (index >= 0 ? localPath.Substring(0, index) : localPath);
			return key;
		}

		public static string ComputeCollectionKey(Uri uri)
		{
			// The collection key is computed to be the factory key, plus the query string hashcode
			// The collection key will ultimately be used as a file name for the dynamic deep zoom content
			string key = ComputeFactoryKey(uri);
			if (key == null)
				return null;
			if (!string.IsNullOrWhiteSpace(uri.Query))
				key += Math.Abs(uri.Query.GetHashCode()).ToString();
			return key;
		}

		public void AddFacetCategory(string category, FacetType facetType, string format = null,
			bool showInFilterPane = true, bool showInInfoPane = true, bool showInSortList = true)
		{
			FacetCategory facetCategory = _facetCategories.TryGet(category);
			if (facetCategory != null)
			{
				if (facetCategory.FacetType != facetType)
					throw new ArgumentException(string.Format("Facet category \"{0}\" already has type \"{1}\", which does not match the requested type \"{2}\".", facetCategory.Name, facetCategory.FacetType, facetType));
				return;
			}

			ThrowIfReservedCategoryName(category);

			facetCategory = new FacetCategory(category, facetType);
			_facetCategories.Add(facetCategory);
			facetCategory.DisplayFormat = format;
			facetCategory.ShowInFilterPane = showInFilterPane;
			facetCategory.ShowInInfoPane = showInInfoPane;
			facetCategory.ShowInSortList = showInSortList;
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
			FacetCategory facetCategory = _facetCategories.TryGet(category);
			if (facetCategory == null)
				throw new ArgumentException("Facet category \"" + category + "\" has not been added.");

			return facetCategory;
		}

		// Call this method to add items to your collection.
		public void AddItem(string name, string url, string description, ItemImage image, params Facet[] facets)
		{
			CollectionItem item = new CollectionItem() { Name = name, Url = url, Description = description };
			item.SetImage(image);

			if (facets != null)
			{
				EnsureFacetCategories(facets);
				item.FacetValues = facets; //TODO: Ensure no duplication of categories, or collapse them automatically.
			}

			_items.Add(item);
		}

		private void EnsureFacetCategories(IEnumerable<Facet> facets)
		{
			foreach (Facet facet in facets)
			{
				AddFacetCategory(facet.Category, facet.DataType);
			}
		}

		private static void ThrowIfReservedCategoryName(string category)
		{
			if (category == null)
				throw new ArgumentNullException("Facet.Category cannot be null");

			if (IsReservedCategoryName(category))
				throw new ArgumentException(string.Format("The facet category \"{0}\" is reserved and may not be used", category));
		}

		internal static bool IsReservedCategoryName(string category)
		{
			foreach (string s in reservedCategoryNames_c)
			{
				if (string.Compare(s, category, true) == 0)
					return true;
			}

			return false;
		}

		public string ToCxml()
		{
			return CxmlSerializer.Serialize(this);
		}

		public void ToCxml(TextWriter textWriter)
		{
			CxmlSerializer.Serialize(textWriter, this);
		}

		public void ToCxml(string filePath)
		{
			using (StreamWriter writer = File.CreateText(filePath))
			{
				CxmlSerializer.Serialize(writer.BaseStream, this);
			}
		}

		public void ToDzc(TextWriter textWriter)
		{
			DzcSerializer.Serialize(textWriter, this);
		}

		private class FacetCategoryCollection : KeyedCollection<string, FacetCategory>
		{
			public FacetCategory TryGet(string key)
			{
				return Contains(key) ? this[key] : null;
			}

			protected override string GetKeyForItem(FacetCategory item)
			{
				return item.Name;
			}
		}
	}
}
