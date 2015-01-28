using System;

namespace PivotServerTools
{
	public class ItemImage
	{
		public string ImageFilePath { get; set; }
		public Uri ImageUrl { get; set; }

		public ItemImage()
		{
		}

		public ItemImage(string imageFilePath)
		{
			ImageFilePath = imageFilePath;
		}

		public ItemImage(Uri imageUrl)
		{
			ImageUrl = imageUrl;
		}
	}
}
