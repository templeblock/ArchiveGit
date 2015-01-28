using System;
using System.Drawing;
using System.IO;
using System.Xml;
using System.Xml.Linq;

namespace TileLibrary
{
	// Serialize a Collection's image data into Deep Zoom Collection XML
	// Reference: http://www.getpivot.com/developer-info/image-content.aspx
	internal class DziSerializer
	{
		private const int _dziTileOverlap = 0; // DeepZoomImageTile currently assumes this is always 0
		private XNamespace Xmlns { get { return "http://schemas.microsoft.com/deepzoom/2008"; } }
		private Size _size;

		private DziSerializer(Size size)
		{
			_size = size;
		}

		// Write a collection's image data as a DZC to an XmlWriter
		static void Serialize(Size size, XmlWriter xmlWriter)
		{
			DziSerializer serializer = new DziSerializer(size);
			serializer.Write(xmlWriter);
		}

		// Write a collection's image data as a DZC to a Stream
		public static void Serialize(Size size, Stream stream)
		{
			XmlWriterSettings settings = new XmlWriterSettings();
			settings.Indent = true;
			settings.IndentChars = string.Empty;
			settings.NewLineChars = Environment.NewLine;
			using (XmlWriter xmlWriter = XmlWriter.Create(stream, settings))
			{
				Serialize(size, xmlWriter);
			}
		}

		private void Write(XmlWriter writer)
		{
			writer.WriteStartDocument();

			XElement root = new XElement(Xmlns + "Image",
				new XAttribute("TileSize", AllTileDefaults.DziTileSize),
				new XAttribute("Overlap", _dziTileOverlap),
				new XAttribute("Format", AllTileDefaults.DziTileFormat),
				new XElement(Xmlns + "Size", new XAttribute("Width", _size.Width), new XAttribute("Height", _size.Height)));
			root.WriteTo(writer);

			writer.WriteEndDocument();
		}
	}
}
