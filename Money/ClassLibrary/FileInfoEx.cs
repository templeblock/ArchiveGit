using System;
using System.IO;
using System.Windows.Controls;

namespace ClassLibrary
{
	// This class is the actual working subset of System.IO.FileInfo
	// Portions of System.IO.FileInfo do not work due to Silverlight security restrictions
	// Using this class will remove the confusion about what works and does not work
	internal class FileInfoHelper
	{
		private FileInfo m_FileInfo;

		// Create an instance by assigning a FileInfo object to a FileInfoHelper object
		private FileInfoHelper(FileInfo fileInfo)
		{
			m_FileInfo = fileInfo;
		}

		public static implicit operator FileInfoHelper(FileInfo fileInfo)
		{
			return new FileInfoHelper(fileInfo);
		}

		internal bool Exists { get { return m_FileInfo.Exists; } }
		internal long Length { get { return m_FileInfo.Length; } }
		internal string Name { get { return m_FileInfo.Name; } }

		internal static FileInfoHelper OpenDialog(string filter)
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Multiselect = false;
			if (!filter.IsEmpty())
				ofd.Filter = filter;

			if (ofd.ShowDialog() == false)
				return null;

			FileInfo fileInfo = ofd.File;
			FileInfoHelper fileInfoHelper = fileInfo;
			return fileInfoHelper;
		}

		internal FileStream OpenRead()
		{
			return m_FileInfo.OpenRead();
		}

		internal StreamReader OpenText()
		{
			return m_FileInfo.OpenText();
		}

		internal string ReadString()
		{
			using (StreamReader reader = OpenText())
			{
				if (reader == null)
					return null;
				return reader.ReadToEnd();
			}
		}
	}
}
