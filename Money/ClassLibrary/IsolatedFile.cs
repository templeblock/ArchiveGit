using System;
using System.IO;
using System.IO.IsolatedStorage;

namespace ClassLibrary
{
	internal class IsolatedFileHelper
	{
		internal static bool IsStorageAllowed()
		{
			try
			{
				return (IsolatedStorageFile.GetUserStoreForApplication() != null);
			}
			catch (Exception ex)
			{
				bool bStorageDisabled = (ex.Message == "Initialization failed.");
				string message =
					"To enable Silverlight application storage:\n\n1.) Right click on this message.\n3.) Select 'Silverlight Configuration' from the popup menu.\n4.) Select the 'Application Storage' tab from the configuration dialog.\n5.) Be sure that 'Enable application storage' is checked and click 'OK'.";
				MessageBoxEx.ShowInfo("Silverlight application storage has not been enabled", message, null/*closeHandler*/);
				return false;
			}
		}

		internal static string[] GetFileNames(string filePath)
		{
			try
			{
				using (IsolatedStorageFile file = IsolatedStorageFile.GetUserStoreForApplication())
				{
					if (file == null)
						return null;
					string[] files = file.GetFileNames(filePath);
					return files;
				}
			}
			catch (Exception ex)
			{
				ex.Alert();
				return null;
			}
		}

		internal static bool Exists(string filePath)
		{
			try
			{
				using (IsolatedStorageFile file = IsolatedStorageFile.GetUserStoreForApplication())
				{
					if (file == null)
						return false;
					return file.FileExists(filePath);
				}
			}
			catch (Exception ex)
			{
				ex.Alert();
				return false;
			}
		}

		internal static bool FileDelete(string filePath)
		{
			try
			{
				using (IsolatedStorageFile file = IsolatedStorageFile.GetUserStoreForApplication())
				{
					if (file == null || !file.FileExists(filePath))
						return false;
					file.DeleteFile(filePath);
					return true;
				}
			}
			catch (Exception ex)
			{
				ex.Alert();
				return false;
			}
		}

		internal static bool IncreaseQuotaTo(long spaceNeeded)
		{
			// Note: You must call this function from a user event such as a button click.
			// For security reasons, IncreaseQuotaTo() will return false immediately
			// if the user did not initiate the call.
			try
			{
				using (IsolatedStorageFile file = IsolatedStorageFile.GetUserStoreForApplication())
				{
					if (file == null)
						return false;
					return (file.Quota >= spaceNeeded || file.IncreaseQuotaTo(spaceNeeded));
				}
			}
			catch (Exception ex)
			{
				ex.Alert();
				return false;
			}
		}

		internal static IsolatedFile OpenForRead(string filePath)
		{
			IsolatedFile file = new IsolatedFile();
			if (!file.OpenForRead(filePath))
				return null;

			return file;
		}

		internal static string ReadFileString(string filePath)
		{
			using (IsolatedFile file = OpenForRead(filePath))
			{
				if (file == null)
					return null;

				return file.ReadString();
			}
		}

		internal static IsolatedFile OpenForWrite(string filePath)
		{
			IsolatedFile file = new IsolatedFile();
			if (!file.OpenForWrite(filePath))
				return null;

			return file;
		}

		internal static void WriteFileString(string filePath, string str)
		{
			using (IsolatedFile file = OpenForWrite(filePath))
			{
				if (file == null)
					return;

				file.WriteString(str);
			}
		}
	}

	internal class IsolatedFile : IDisposable
	{
		public IsolatedStorageFile File { get { return m_File; } set { m_File = value; } }
		private IsolatedStorageFile m_File;

		public IsolatedStorageFileStream Stream { get { return m_Stream; } set { m_Stream = value; } }
		private IsolatedStorageFileStream m_Stream;

		public StreamReader Reader { get { return m_Reader; } set { m_Reader = value; } }
		private StreamReader m_Reader;

		public StreamWriter Writer { get { return m_Writer; } set { m_Writer = value; } }
		private StreamWriter m_Writer;

		public IsolatedFile()
		{
			m_File = null;
			m_Stream = null;
			m_Writer = null;
			m_Reader = null;
		}

		public void Dispose()
		{
			Close();
		}

		internal void Close()
		{
			if (m_Reader != null)
				m_Reader.Dispose();
			m_Reader = null;

			if (m_Writer != null)
				m_Writer.Dispose();
			m_Writer = null;

			if (m_Stream != null)
				m_Stream.Dispose();
			m_Stream = null;

			if (m_File != null)
				m_File.Dispose();
			m_File = null;
		}

		internal bool OpenForRead(string filePath)
		{
			try
			{
				if (m_File != null)
					return false;

				m_File = IsolatedStorageFile.GetUserStoreForApplication();
				if (m_File == null)
					return false;

				// Open an existing file and a stream object to access it
				if (!m_File.FileExists(filePath))
					return false;

				m_Stream = m_File.OpenFile(filePath, FileMode.Open, FileAccess.Read);
				m_Reader = new StreamReader(m_Stream);

				return (m_Stream != null && (m_Reader != null || m_Writer != null));
			}
			catch (Exception ex)
			{
				ex.Alert();
				return false;
			}
		}

		internal bool OpenForWrite(string filePath)
		{
			try
			{
				if (m_File != null)
					return false;

				m_File = IsolatedStorageFile.GetUserStoreForApplication();
				if (m_File == null)
					return false;

				// Create new file and a stream object to access it
				m_Stream = m_File.OpenFile(filePath, FileMode.Create, FileAccess.Write);
				m_Writer = new StreamWriter(m_Stream);

				return (m_Stream != null && (m_Reader != null || m_Writer != null));
			}
			catch (Exception ex)
			{
				ex.Alert();
				return false;
			}
		}

		internal string ReadLine()
		{
			if (m_Reader == null)
				return "";

			return m_Reader.ReadLine();
		}

		internal string ReadString()
		{
			if (m_Reader == null)
				return "";

			return m_Reader.ReadToEnd();
		}

		internal int ReadBlock(char[] buffer, int index, int count)
		{
			if (m_Reader == null)
				return 0;

			return m_Reader.ReadBlock(buffer, index, count);
		}

		internal void WriteLine(string str)
		{
			if (m_Writer == null)
				return;

			m_Writer.WriteLine(str);
		}

		internal void WriteString(string str)
		{
			if (m_Writer == null)
				return;

			m_Writer.Write(str);
		}

		internal void WriteBlock(char[] buffer, int index, int count)
		{
			if (m_Writer == null)
				return;

			m_Writer.Write(buffer, index, count);
		}
	}
}
