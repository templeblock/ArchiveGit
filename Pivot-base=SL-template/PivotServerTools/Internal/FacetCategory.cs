using System;

namespace PivotServerTools.Internal
{
	internal class FacetCategory
	{
		public string Name { get; private set; }
		public FacetType FacetType { get; set; }
		public string DisplayFormat { get; set; }
		public bool ShowInFilterPane { get; set; }
		public bool ShowInInfoPane { get; set; }
		public bool ShowInSortList { get; set; }

		public FacetCategory(string name, FacetType type)
		{
			Name = name;
			FacetType = type;
			ShowInFilterPane = true;
			ShowInInfoPane = true;
			ShowInSortList = true;
		}
	}
}
