using System.Drawing;
using System.IO;
using System.Xml;
using System.Xml.Linq;

namespace PivotServerTools.Internal
{
	// Serialize a Collection's image data into Deep Zoom Collection XML.
	// Reference: http://www.getpivot.com/developer-info/image-content.aspx
	internal class DziSerializer
	{
		XNamespace Xmlns { get { return "http://schemas.microsoft.com/deepzoom/2008"; } }

		public static int DefaultTileSize = 256; //Must be a power of 2.
		public static int DefaultOverlap = 0; //DeepZoomImageTile currently assumes this is always 0.
		public static string DefaultFormat = "jpg";

		private Size _size;

		// Write a collection's image data as a DZC into a TextWriter.
		public static void Serialize(TextWriter textWriter, Size size)
		{
			using (XmlWriter xmlWriter = XmlWriter.Create(textWriter))
			{
				Serialize(xmlWriter, size);
			}
		}

		// Write a collection's image data as a DZC into an XmlWriter.
		static void Serialize(XmlWriter xmlWriter, Size size)
		{
			DziSerializer serializer = new DziSerializer(size);
			serializer.Write(xmlWriter);
		}

		private DziSerializer(Size size)
		{
			_size = size;
		}

		public static string MakeDziPath(string collectionKey, int itemId)
		{
			return string.Format("{0}/dzi/{1}.dzi", collectionKey, itemId);
		}

		private void Write(XmlWriter outputWriter)
		{
			outputWriter.WriteStartDocument();

			XElement root = new XElement(Xmlns + "Image",
				new XAttribute("TileSize", DefaultTileSize),
				new XAttribute("Overlap", DefaultOverlap),
				new XAttribute("Format", DefaultFormat),
				new XElement(Xmlns + "Size",
						new XAttribute("Width", _size.Width),
						new XAttribute("Height", _size.Height)));
			root.WriteTo(outputWriter);
		}
	}
}
