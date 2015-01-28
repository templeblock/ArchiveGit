using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Xml.Linq;

namespace CollectionLibrary
{
	// Serialize a Collection object to CXML; Reference: http://www.getpivot.com/developer-info/xml-schema.aspx
	public class CxmlSerializer
	{
		private string Xmlns { get { return "http://schemas.microsoft.com/collection/metadata/2009"; } }
		private string PivotXmlns { get { return "http://schemas.microsoft.com/livelabs/pivot/collection/2009"; } }
		private PivotCollection _collection;
		private XmlWriter _w;

		private CxmlSerializer(PivotCollection collection)
		{
			_collection = collection;
		}

		// Write a collection object as CXML into an XmlWriter
		public static void Serialize(XmlWriter writer, PivotCollection collection)
		{
			CxmlSerializer serializer = new CxmlSerializer(collection);
			serializer.Write(writer);
		}

		// Write a collection object as CXML to a stream
		public static void Serialize(Stream outputStream, PivotCollection collection)
		{
			CxmlSerializer serializer = new CxmlSerializer(collection);
			XmlWriterSettings settings = new XmlWriterSettings();
			settings.Indent = true;
			settings.IndentChars = string.Empty;
			settings.NewLineChars = Environment.NewLine;
			using (XmlWriter writer = XmlWriter.Create(outputStream, settings))
				serializer.Write(writer);
		}

		private void Write(XmlWriter writer)
		{
			try
			{
				_w = writer;
				_w.WriteStartDocument();
					WriteCollectionElement();
				_w.WriteEndDocument();
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void WriteCollectionElement()
		{
			_w.WriteStartElement("Collection", Xmlns);
				_w.WriteAttributeString("xmlns", "p", null, PivotXmlns);
				_w.WriteAttributeString("SchemaVersion", _collection.SchemaVersion.ToString());
				if (!string.IsNullOrEmpty(_collection.Name))
					_w.WriteAttributeString("Name", _collection.Name);
				if (_collection.IconUrl != null)
					_w.WriteAttributeString("Icon", PivotXmlns, _collection.IconUrl.AbsoluteUri);
				if (_collection.BrandImageUrl != null)
					_w.WriteAttributeString("BrandImage", PivotXmlns, _collection.BrandImageUrl.AbsoluteUri);
				if (_collection.Culture != null && !string.IsNullOrEmpty(_collection.Culture.Name))
					_w.WriteAttributeString("xml:lang", _collection.Culture.Name);
				if (!string.IsNullOrEmpty(_collection.Supplement))
					_w.WriteAttributeString("Supplement", PivotXmlns, _collection.Supplement);
				_w.WriteAttributeString("AdditionalSearchText", PivotXmlns, (_collection.AdditionalSearchText == null ? "__block" : _collection.AdditionalSearchText));
				WriteFacetCategoriesElement();
				WriteItemsElement();
				WriteExtensionElements();
			_w.WriteEndElement(); // Collection
		}

		private void WriteFacetCategoriesElement()
		{
			if (!_collection.HasFacets)
				return;

			_w.WriteStartElement("FacetCategories");
				foreach (FacetCategory category in _collection.Categories)
					WriteFacetCategoryElement(category);
			_w.WriteEndElement(); // FacetCategories
		}

		private void WriteFacetCategoryElement(FacetCategory facetCategory)
		{
			_w.WriteStartElement("FacetCategory");
				_w.WriteAttributeString("Name", facetCategory.Name);
				_w.WriteAttributeString("Type", facetCategory.FacetType.ToElementName());
				if (!string.IsNullOrEmpty(facetCategory.DisplayFormat))
					_w.WriteAttributeString("Format", facetCategory.DisplayFormat);
				if (!facetCategory.ShowInFilterPane)
					_w.WriteAttributeString("IsFilterVisible", PivotXmlns, "false");
				if (!facetCategory.ShowInInfoPane)
					_w.WriteAttributeString("IsMetaDataVisible", PivotXmlns, "false");
				if (!facetCategory.ShowInSortList)
					_w.WriteAttributeString("IsWordWheelVisible", PivotXmlns, "false");
			_w.WriteEndElement(); // FacetCategory
		}

		private void WriteItemsElement()
		{
			_w.WriteStartElement("Items");
				_w.WriteAttributeString("ImgBase", _collection.ImgBase);
				if (!string.IsNullOrEmpty(_collection.HrefBase))
					_w.WriteAttributeString("HrefBase", _collection.HrefBase);
				foreach (Item item in _collection.Items)
					WriteItemElement(item);
			_w.WriteEndElement(); // Items
		}

		private void WriteItemElement(Item item)
		{
			_w.WriteStartElement("Item");
			_w.WriteAttributeString("Id", item.UniqueID.ToString());
			_w.WriteAttributeString("Img", "#" + item.UniqueID.ToString()); // This #(ID) must match the ID in the DZC file; FYI, do not use the image ID here - that mapping happens in the DZC file
				if (!string.IsNullOrEmpty(item.Name))
					_w.WriteAttributeString("Name", item.Name);
				if (!string.IsNullOrEmpty(item.Url))
					_w.WriteAttributeString("Href", item.Url);
				if (!string.IsNullOrEmpty(item.Description))
					_w.WriteElementString("Description", item.Description);
				WriteFacetElements(item);
				WriteItemRelatedCollections(item);
			_w.WriteEndElement(); // Item
		}

		private void WriteFacetElements(Item item)
		{
			// Note: Pivot does not accept an empty Facets element under Item
			if (item.Facets == null || item.Facets.Count <= 0)
				return;

			_w.WriteStartElement("Facets");
				foreach (Facet facet in item.Facets)
					WriteFacetTags(facet);
			_w.WriteEndElement(); // Facets
		}

		private void WriteFacetTags(Facet facet)
		{
			// Only emit a Facet element if there is content inside it
			if (!facet.IsTags)
				return;

			_w.WriteStartElement("Facet");
			_w.WriteAttributeString("Name", facet.Category);
			if (facet.FacetType == FacetType.Link)
			{
				foreach (Link value in facet.Tags)
				{
					_w.WriteStartElement(facet.FacetType.ToElementName());
					_w.WriteAttributeString("Name", value.Name);
					_w.WriteAttributeString("Href", value.Href);
					_w.WriteEndElement();  //  facet.DataType
				}
			}
			else
			{
				foreach (object value in facet.EnumerateNonEmptyTags())
				{
					_w.WriteStartElement(facet.FacetType.ToElementName());
					_w.WriteAttributeString("Value", new XAttribute("Value", value).Value); // This performs the proper value conversion for DateTime
					_w.WriteEndElement(); // facet.DataType
				}
			}
			_w.WriteEndElement(); // Facet
		}

		private void WriteItemRelatedCollections(Item item)
		{
			ICollection<Link> links = item.RelatedLinks;
			if (links == null || links.Count <= 0)
				return;

			_w.WriteStartElement("Extension");
				_w.WriteStartElement("Related", PivotXmlns);
					foreach (Link link in links)
					{
						_w.WriteStartElement("Link", PivotXmlns);
						_w.WriteAttributeString("Name", link.Name);
						_w.WriteAttributeString("Href", link.Href);
						_w.WriteEndElement(); // Link
					}
				_w.WriteEndElement(); // Related
			_w.WriteEndElement(); // Extension
		}

		private void WriteExtensionElements()
		{
			if (_collection.Copyright == null)
				return;

			_w.WriteStartElement("Extension");
				WriteCopyrightElement();
			_w.WriteEndElement(); // Extension
		}

		private void WriteCopyrightElement()
		{
			_w.WriteStartElement("Copyright", PivotXmlns);
				_w.WriteAttributeString("Name", _collection.Copyright.Name);
				_w.WriteAttributeString("Href", _collection.Copyright.Href);
			_w.WriteEndElement(); // Copyright
		}
	}
}
