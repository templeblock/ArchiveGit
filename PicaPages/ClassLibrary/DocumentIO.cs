using System;
using System.IO;
using System.IO.IsolatedStorage;

namespace ClassLibrary
{
	public class DocumentIO
	{
		private IsolatedStorageFile m_file;
		private IsolatedStorageFileStream m_stream;
		private StreamWriter m_writer;
		private StreamReader m_reader;

		// Public properties
		public StreamReader Reader { set { m_reader = value; } get { return m_reader; } }
		public StreamWriter Writer { set { m_writer = value; } get { return m_writer; } }
		public IsolatedStorageFileStream IsoStream { set { m_stream = value; } get { return m_stream; } }
		public IsolatedStorageFile IsoStore { set { m_file = value; } get { return m_file; } }

		/////////////////////////////////////////////////////////////////////////////
		public DocumentIO()
		{
			m_file = null;
			m_stream = null;
			m_writer = null;
			m_reader = null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public bool Open(string sFileName, bool bWriter)
		{
			if (m_file != null)
				return false;

			// Obtain Isolated Storage for the user.
			try
			{
				m_file = IsolatedStorageFile.GetUserStoreForApplication();
			}
			catch (Exception e)
			{
				System.Diagnostics.Debug.WriteLine(e.Message);
				e.Assert();
				return false;
			}

			if (m_file == null)
				return false;

			if (bWriter)
			{
				// Create new file and a stream object to access it
				try
				{
					m_stream = new IsolatedStorageFileStream(sFileName, FileMode.Create, m_file);
				}
				catch (Exception e)
				{
					System.Diagnostics.Debug.WriteLine(e.Message);
					e.Assert();
					return false;
				}

				try
				{
					m_writer = new StreamWriter(m_stream);
				}
				catch (Exception e)
				{
					System.Diagnostics.Debug.WriteLine(e.Message);
					e.Assert();
					return false;
				}
			}
			else
			{
				// Open an existing file and a stream object to access it
				try
				{
					m_stream = new IsolatedStorageFileStream(sFileName, FileMode.Open, m_file);
				}
				catch (Exception e)
				{
					System.Diagnostics.Debug.WriteLine(e.Message);
					e.Assert();
					return false;
				}
				try
				{
					// Read the contents from the file
					m_reader = new StreamReader(m_stream);
				}
				catch (Exception e)
				{
					System.Diagnostics.Debug.WriteLine(e.Message);
					e.Assert();
					return false;
				}
			}

			return (m_stream != null && (m_reader != null || m_writer != null));
		}

		/////////////////////////////////////////////////////////////////////////////
		~DocumentIO()
		{
			Close();
		}

		/////////////////////////////////////////////////////////////////////////////
		public void Close()
		{
//j			m_file.DeleteFile("IsoStoreFile.txt");
//j			string[] files = m_file.GetFileNames("*.*");

			if (m_reader != null)
				m_reader.Close();
			m_reader = null;

			if (m_writer != null)
				m_writer.Close();
			m_writer = null;

			if (m_stream != null)
				m_stream.Close();
			m_stream = null;

			if (m_file != null)
				m_file.Dispose();
			m_file = null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public string ReadLine()
		{
			if (m_reader == null)
				return "";

			return m_reader.ReadLine();
		}

		/////////////////////////////////////////////////////////////////////////////
		public string ReadToEnd()
		{
			if (m_reader == null)
				return "";

			return m_reader.ReadToEnd();
		}

		/////////////////////////////////////////////////////////////////////////////
		public void WriteLine(string str)
		{
			if (m_writer == null)
				return;

			m_writer.WriteLine(str);
		}

		/////////////////////////////////////////////////////////////////////////////
		public void Write(string str)
		{
			if (m_writer == null)
				return;

			m_writer.Write(str);
		}
	}
}
