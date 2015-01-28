using System.Drawing;
using CollectionLibrary;
using TileLibrary;

namespace ImageProviderForItem
{
	internal static class ItemImageProvider
	{
		// Extension for Item
		public static Size GetImageSize(this ItemImage itemImage)
		{
			return (itemImage.CommonSize != null ? (Size)itemImage.CommonSize : itemImage.GetImageProvider().Size);
		}

		// Extension for ItemImage
		public static ImageProviderBase GetImageProvider(this ItemImage itemImage)
		{
			ImageProviderBase imageProvider = itemImage.Provider as ImageProviderBase;
			if (imageProvider != null)
				return imageProvider;
			itemImage.Provider = itemImage.CreateImageProvider();
			return itemImage.Provider as ImageProviderBase;
		}

		// Extension for ItemImage
		public static ImageProviderBase CreateImageProvider(this ItemImage itemImage)
		{
			return ImageProvider.Create(itemImage.Path, itemImage.IsFile, itemImage.CommonSize);
		}
	}
}
