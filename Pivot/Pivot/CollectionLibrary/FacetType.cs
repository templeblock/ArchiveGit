using System;
using System.Collections.ObjectModel;

namespace CollectionLibrary
{
	public enum FacetType
	{
		ShortString,
		LongString,
		Number,
		DateTime,
		Link
	}

	public static class FacetTypeExtensions
	{
		// Extension for Type
		public static FacetType ToFacetType(this Type type)
		{
			if (type == typeof(string) || type == typeof(string[]))
				return FacetType.ShortString;
			if (type == typeof(int) || type == typeof(int[]))
				return FacetType.Number;
			if (type == typeof(double) || type == typeof(double[]))
				return FacetType.Number;
			if (type == typeof(DateTime) || type == typeof(DateTime[]))
				return FacetType.DateTime;
			if (type == typeof(Link) || type == typeof(Link[]))
				return FacetType.Link;
			if (type == typeof(bool) || type == typeof(bool[]))
				return FacetType.ShortString;
			throw new ArgumentException("FacetType failed to convert an unknown type '" + type + "'.");
		}

		// Extension for FacetType
		public static string ToElementName(this FacetType facetType)
		{
			switch (facetType)
			{
				default: // falls through to ShortString
				case FacetType.ShortString:
					return "String";

				case FacetType.LongString:
					return "LongString";

				case FacetType.Number:
					return "Number";

				case FacetType.DateTime:
					return "DateTime";

				case FacetType.Link:
					return "Link";
			}
		}
	}
}
