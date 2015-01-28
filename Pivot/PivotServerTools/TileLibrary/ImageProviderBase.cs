using System;
using System.Drawing;
using System.IO;

namespace TileLibrary
{
	// Override this class to create custom image providers that will create a System.Drawing.Image
	// Just implement the MakeImage() method
	internal abstract class ImageProviderBase
	{
		public Size? CommonSize { get; set; }

		public Size Size { get { if (CommonSize != null) return (Size)CommonSize; if (_size.IsEmpty) NeedImage(); return _size; } protected set { _size = value; } }
		private Size _size;

		public Image Image { get { NeedImage(); return _image; } protected set { _image = value; } }
		private Image _image;

		// Override this method to return the desired Image object to display
		protected abstract Image MakeImage();

		public bool NeedImage()
		{
			try
			{
				if (_image == null)
					_image = MakeImage();
				if (_image != null && _size.IsEmpty)
					_size = _image.Size;
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			return (_image != null);
		}
	}

	internal static class ImageProvider
	{
		public static ImageProviderBase Create(string sourcePath, bool sourceIsFile, Size? commonSize)
		{
			ImageProviderBase imageProvider = null;
			string sourcePathLower = (sourceIsFile && sourcePath != null ? sourcePath.ToLower() : string.Empty);
			if (sourcePathLower.EndsWith(".jpg", StringComparison.InvariantCultureIgnoreCase) ||
				sourcePathLower.EndsWith(".png", StringComparison.InvariantCultureIgnoreCase))
				imageProvider = new StandardImage(sourcePath, sourceIsFile, commonSize);
			else
			if (sourcePathLower.EndsWith(".dzi", StringComparison.InvariantCultureIgnoreCase))
				imageProvider = new DeepZoomImage(sourcePath, sourceIsFile, commonSize);

			if (imageProvider == null || imageProvider.Size == Size.Empty /*|| !File.Exists(sourcePath)*/)
				imageProvider = new DynamicImage("Name", "Description", commonSize);
	
			return imageProvider;
		}

		public static string FolderPrep(string filePath)
		{
			string folder = Path.GetDirectoryName(filePath);
			CreateFolder(folder);
			return folder;
		}

		public static void CreateFolder(string path)
		{
			if (!Directory.GetParent(path).Exists)
				CreateFolder(Directory.GetParent(path).FullName);
			if (!Directory.Exists(path))
				Directory.CreateDirectory(path);
		}
#if DOWNLOAD_ORIGINALS
		public static bool VerifyImage(string filePath)
		{
			try
			{
				if (filePath == null || !File.Exists(filePath))
					return false;
#if THOROUGH_BUT_SLOW
				Image imageFile = Image.FromFile(filePath);
				float width = imageFile.PhysicalDimension.Width;
				float height = imageFile.PhysicalDimension.Height;
				return (width != 0 && height != 0);
#else
				return true;
#endif
			}
			catch (Exception ex)
			{
				ex.GetType();
				return false;
			}
		}

		public static bool DownloadSourceImage(string filePath, string sourceUri)
		{
			if (sourceUri == null)
				return false;
			WebClient webClient = new WebClient();
			int retries = 0;
			while (++retries <= 2)
			{
				try
				{
					webClient.DownloadFile(sourceUri, filePath);
					break;
				}
				catch (Exception ex)
				{
					ex.GetType();
				}
			}

			bool fileOK = VerifyFile(filePath);
			if (!fileOK)
			{
				try
				{
					File.Delete(filePath);
				}
				catch (Exception ex)
				{
					if (ex is IOException)
						return true;
				}
			}

			return fileOK;
		}
#endif
	}
}
