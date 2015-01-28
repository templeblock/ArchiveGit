using System;

namespace CollectionLibrary
{
	public class FacetCategory
	{
		public string Name { get; private set; }
		public FacetType FacetType { get; set; }
		public string DisplayFormat { get; set; }
		public bool ShowInFilterPane { get; set; }
		public bool ShowInInfoPane { get; set; }
		public bool ShowInSortList { get; set; }

		private static readonly string[] _reservedCategoryNames = { "Name", "Description" };

		public FacetCategory(string name, FacetType type, string displayFormat = null,
				bool showInFilterPane = true,  bool showInInfoPane = true, bool showInSortList = true)
		{
			Name = name;
			FacetType = type;
			DisplayFormat = displayFormat;
			ShowInFilterPane = showInFilterPane;
			ShowInInfoPane = showInInfoPane;
			ShowInSortList = showInSortList;

			ThrowIfReservedCategoryName(name);
		}

		private void ThrowIfReservedCategoryName(string name)
		{
			if (string.IsNullOrEmpty(name))
				throw new ArgumentNullException("The facet category name cannot be null");

			if (IsReservedCategoryName(name))
				throw new ArgumentException(string.Format("The facet category \"{0}\" is reserved and may not be used", name));
		}

		private bool IsReservedCategoryName(string name)
		{
			foreach (string categoryName in _reservedCategoryNames)
			{
				if (name.Equals(categoryName, StringComparison.CurrentCultureIgnoreCase))
					return true;
			}

			return false;
		}
	}
}
