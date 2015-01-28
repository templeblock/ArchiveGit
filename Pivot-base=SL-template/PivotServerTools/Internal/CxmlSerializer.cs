using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace PivotServerTools.Internal
{
	// Serialize a Collection object into CXML.
	// Reference: http://www.getpivot.com/developer-info/xml-schema.aspx
	internal class CxmlSerializer
	{
		private XNamespace Xmlns { get { return "http://schemas.microsoft.com/collection/metadata/2009"; } }
		private XNamespace PivotXmlns { get { return "http://schemas.microsoft.com/livelabs/pivot/collection/2009"; } }

		Collection _collection;


		private CxmlSerializer(Collection collection)
		{
			_collection = collection;
		}

		// Write a collection object as CXML into a TextWriter.
		public static void Serialize(TextWriter textWriter, Collection collection)
		{
			using (XmlWriter writer = XmlWriter.Create(textWriter))
				Serialize(writer, collection);
		}

		// Write a collection object as CXML into an XmlWriter.
		public static void Serialize(XmlWriter outputWriter, Collection collection)
		{
			CxmlSerializer s = new CxmlSerializer(collection);
			s.Write(outputWriter);
		}

		// Write a collection object as CXML into the given stream.
		public static void Serialize(Stream outputStream, Collection collection)
		{
			CxmlSerializer s = new CxmlSerializer(collection);
			using (XmlWriter writer = XmlWriter.Create(outputStream))
				s.Write(writer);
		}

		// Write the collection object as CXML to a string.
		public static string Serialize(Collection collection)
		{
			using (MemoryStream memStream = new MemoryStream())
			{
				Serialize(memStream, collection);
				memStream.Position = 0; //Rewind the stream, ready for reading.

				using (StreamReader reader = new StreamReader(memStream))
				{
					string text = reader.ReadToEnd();
					return text;
				}
			}
		}

		private void Write(XmlWriter outputWriter)
		{
			outputWriter.WriteStartDocument(); //Write the XML declaration

			XStreamingElement root = MakeCxmlTree();
			root.WriteTo(outputWriter);
		}

		private XStreamingElement MakeCxmlTree()
		{
			XStreamingElement root = new XStreamingElement(Xmlns + "Collection", MakeCollectionContent());
			return root;
		}

		private IEnumerable<object> MakeCollectionContent()
		{
			yield return new XAttribute("xmlns", Xmlns);
			yield return new XAttribute(XNamespace.Xmlns + "ui", PivotXmlns);

			yield return new XAttribute("SchemaVersion", _collection.SchemaVersion.ToString());

			if (!string.IsNullOrEmpty(_collection.Name))
				yield return new XAttribute("Name", _collection.Name);

			if (_collection.IconUrl != null)
				yield return new XAttribute(PivotXmlns + "Icon", _collection.IconUrl.AbsoluteUri);

			if (_collection.Culture != null)
			{
				string language = _collection.Culture.Name;
				if(!string.IsNullOrEmpty(language))
					yield return new XAttribute(XNamespace.Xml + "lang", language);
			}

			if (!_collection.EnableInfoPaneBingSearch)
				yield return new XAttribute(PivotXmlns + "AdditionalSearchText", "__block");

			if (_collection.HasFacets)
				yield return MakeFacetCategories();

			yield return MakeItems();
		}

		private XStreamingElement MakeFacetCategories()
		{
			XStreamingElement facetCats = new XStreamingElement(Xmlns + "FacetCategories", MakeFacetCategoriesContent());
			return facetCats;
		}

		private IEnumerable<XStreamingElement> MakeFacetCategoriesContent()
		{
			foreach (var cat in _collection.FacetCategories)
				yield return new XStreamingElement(Xmlns + "FacetCategory", MakeFacetCategoryContent(cat));
		}

		private IEnumerable<XAttribute> MakeFacetCategoryContent(FacetCategory cat)
		{
			yield return new XAttribute("Name", cat.Name);
			yield return new XAttribute("Type", FacetTypeText(cat.FacetType));

			if (!string.IsNullOrEmpty(cat.DisplayFormat))
				yield return new XAttribute("Format", cat.DisplayFormat);

			if (!cat.ShowInFilterPane)
				yield return new XAttribute(PivotXmlns + "IsFilterVisible", false);

			if (!cat.ShowInInfoPane)
				yield return new XAttribute(PivotXmlns + "IsMetaDataVisible", false);

			if (!cat.ShowInSortList)
				yield return new XAttribute(PivotXmlns + "IsWordWheelVisible", false);
		}

		private string FacetTypeText(FacetType facetType)
		{
			switch (facetType)
			{
				default: //default falls through to Text
				case FacetType.Text:
					return "String";

				case FacetType.Number:
					return "Number";

				case FacetType.DateTime:
					return "DateTime";

				case FacetType.Link:
					return "Link";
			}
		}

		private XStreamingElement MakeItems()
		{
			XStreamingElement items = new XStreamingElement(Xmlns + "Items", MakeItemsContent());
			return items;
		}

		private IEnumerable<object> MakeItemsContent()
		{
			yield return new XAttribute("ImgBase", _collection.CollectionKey + ".dzc");

			if (!string.IsNullOrEmpty(_collection.HrefBase))
				yield return new XAttribute("HrefBase", _collection.HrefBase);

			int itemId = 0;
			foreach(var item in _collection.Items)
				yield return new XStreamingElement(Xmlns + "Item", MakeItemContent(item, itemId++));
		}

		private IEnumerable<object> MakeItemContent(CollectionItem item, int id)
		{
			yield return new XAttribute("Id", id);
			yield return new XAttribute("Img", "#" + id.ToString());

			if(!string.IsNullOrEmpty(item.Name))
				yield return new XAttribute("Name", item.Name);

			if (!string.IsNullOrEmpty(item.Url))
				yield return new XAttribute("Href", item.Url);

			if (item.FacetValues != null && item.FacetValues.Count > 0)
				//Note, Pivot does not accept an empty Facets element under Item.
				yield return new XStreamingElement(Xmlns + "Facets", MakeItemFacets(item.FacetValues));
		}

		private IEnumerable<XStreamingElement> MakeItemFacets(IEnumerable<Facet> facets)
		{
			foreach (Facet facet in facets)
			{
				if (facet.IsTags)
				{
					XStreamingElement[] tagElements = MakeItemFacetTags(facet).ToArray();
					//Only emit a Facet element if there is content inside it.
					if (tagElements.Length > 0)
						yield return new XStreamingElement(Xmlns + "Facet", new XAttribute("Name", facet.Category), tagElements);
				}
			}
		}

		private IEnumerable<XStreamingElement> MakeItemFacetTags(Facet facet)
		{
			if (facet.DataType == FacetType.Link)
			{
				foreach (FacetHyperlink value in facet.Tags)
					yield return new XStreamingElement(Xmlns + FacetTypeText(facet.DataType), new XAttribute("Name", value.Name), new XAttribute("Href", value.Url));
			}
			else
			{
				foreach (var value in facet.EnumerateNonEmptyTags())
					yield return new XStreamingElement(Xmlns + FacetTypeText(facet.DataType), new XAttribute("Value", value));
			}
		}
	}
}
