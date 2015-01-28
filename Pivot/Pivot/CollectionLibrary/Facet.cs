using System;
using System.Collections.Generic;

namespace CollectionLibrary
{
	public class Facet
	{
		public bool IsTags { get { return (Tags != null && Tags.Length > 0); } }
		public string Category { get; private set; }
		public FacetType FacetType { get; private set; }
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
			FacetType = facetType;
			Tags = tags;
		}

		public Facet(string category, params string[] tags)
		{
			Construct<string>(category, FacetType.ShortString, tags);
		}

		public Facet(string category, params double[] tags)
		{
			Construct<double>(category, FacetType.Number, tags);
		}

		public Facet(string category, params DateTime[] tags)
		{
			Construct<DateTime>(category, FacetType.DateTime, tags);
		}

		public Facet(string category, params Link[] tags)
		{
			Construct<Link>(category, FacetType.Link, tags);
		}

		private void Construct<T>(string category, FacetType facetType, T[] tags)
		{
			Category = category;
			FacetType = facetType;

			if (tags == null)
				return;

			Tags = new object[tags.Length];
			for (int i = 0; i < tags.Length; ++i)
				Tags[i] = tags[i];
		}

		public IEnumerable<object> EnumerateNonEmptyTags()
		{
			foreach (object tag in Tags)
			{
				if (tag == null)
					continue;

				if ((FacetType == FacetType.ShortString || FacetType == FacetType.LongString)
					&& string.IsNullOrEmpty(tag.ToString()))
					continue;

				yield return tag;
			}
		}
	}
}
