using System;
#if SILVERLIGHT
using System.Windows;
#else
using System.Drawing;
#endif

namespace CollectionLibrary
{
	public class ItemImage
	{
		public int ID { get; set; }
		public string Path { get; set; }
		public bool IsFile { get; set; }
		public Size? CommonSize { get; set; }
		public object Provider { get; set; }

		public ItemImage()
		{
		}

		public ItemImage(int imageID, string path, bool isFile, Size? commonSize = null)
		{
			ID = imageID;
			Path = path;
			IsFile = isFile;
			CommonSize = commonSize;
		}
	}
}
