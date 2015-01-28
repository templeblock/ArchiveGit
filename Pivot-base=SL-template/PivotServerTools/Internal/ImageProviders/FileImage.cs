using System.Drawing;

namespace PivotServerTools.Internal
{
	// Create an item image by loading it from a file.
	internal class FileImage : ImageBase
	{
		string _filePath;

		public FileImage(string filePath)
		{
			_filePath = filePath;
		}

		protected override Image MakeImage()
		{
			return Image.FromFile(_filePath);
		}
	}
}
