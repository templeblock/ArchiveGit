using System;
using System.Collections.Generic;

namespace PivotServerTools
{
	public enum FacetType
	{
		Text,
		Number,
		DateTime,
		Link
	}

	public class FacetHyperlink
	{
		public string Name;
		public string Url;

		public FacetHyperlink(string linkName, string linkUrl)
		{
			Name = linkName;
			Url = linkUrl;
		}
	}

	public class Facet
	{
		public bool IsTags { get { return (Tags != null && Tags.Length > 0); } }
		public string Category { get; private set; }
		public FacetType DataType { get; private set; }
		public object[] Tags { get; private set; }
		public object Tag
		{
			get
			{
				if (Tags == null || Tags.Length == 0)
					return null;

				if (Tags.Length > 1)
					throw new ArgumentOutOfRangeException("Facet is not single-valued");

				return Tags[0];
			}
			set { Tags = new object[] { value }; }
		}

		public Facet(string category, FacetType facetType, params object[] tags)
		{
			Category = category;
			DataType = facetType;
			Tags = tags;
		}

		public Facet(string category, params string[] tags)
		{
			//TODO: Ignore string.empty or null.
			Construct<string>(category, FacetType.Text, tags);
		}

		public Facet(string category, params double[] tags)
		{
			Construct<double>(category, FacetType.Number, tags);
		}

		public Facet(string category, params DateTime[] tags)
		{
			Construct<DateTime>(category, FacetType.DateTime, tags);
		}

		public Facet(string category, params FacetHyperlink[] tags)
		{
			Construct<FacetHyperlink>(category, FacetType.Link, tags);
		}

		private void Construct<T>(string category, FacetType facetType, T[] tags)
		{
			Category = category;
			DataType = facetType;

			if (tags == null)
				return;

			Tags = new object[tags.Length];
			for (int i = 0; i < tags.Length; ++i)
				Tags[i] = tags[i];
		}

		public static bool IsReservedCategory(string category)
		{
			return Collection.IsReservedCategoryName(category);
		}

		public IEnumerable<object> EnumerateNonEmptyTags()
		{
			foreach (object o in Tags)
			{
				if (o == null)
					continue;

				if (DataType == FacetType.Text && string.IsNullOrEmpty((string)o))
					continue;

				yield return o;
			}
		}
	}
}
