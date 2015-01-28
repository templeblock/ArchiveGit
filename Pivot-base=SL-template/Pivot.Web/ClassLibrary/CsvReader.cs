using System;
using System.Collections.Generic;
using System.IO;
#if SILVERLIGHT
using System.Windows;
using System.Windows.Resources;
#endif

namespace ClassLibrary
{
	public class CsvReader
	{
#if SILVERLIGHT
		public static List<Dictionary<string, string>> FromResource(string resourceUri)
		{
			if (resourceUri == null)
				return null;
			Uri uri = new Uri(resourceUri, UriKind.Relative);
			StreamResourceInfo streamInfo = Application.GetResourceStream(uri);
			if (streamInfo == null)
				return null;
			return FromStream(streamInfo.Stream);
		}
#endif

		public static List<Dictionary<string, string>> FromStream(Stream stream)
		{
			if (stream == null)
				return null;

			List<Dictionary<string, string>> dataset = new List<Dictionary<string, string>>();
			string[] header = null;
			string row = null;
			int rowCount = 0;
			StreamReader sr = new StreamReader(stream);
			while ((row = sr.ReadLine()) != null)
			{
				rowCount++;
				string[] columns = CsvReader.SplitRow(row, ',', '"');

				// Allow rows to be commented out
				if (columns.Length > 0 && columns[0].StartsWith("//"))
					continue;

				if (header == null)
				{
					header = columns;
					continue;
				}

				if (columns.Length != header.Length)
				{
					Helper.Alert(string.Format("Data on row {0} does not line up with the header row's {1} columns; ignoring the row", rowCount, header.Length));
					continue;
				}

				Dictionary<string, string> columnSet = new Dictionary<string, string>();
				int i = 0;
				foreach (string column in columns)
					columnSet.Add(header[i++].Trim(), column.Trim());

				dataset.Add(columnSet);
			}

			return dataset;
		}

		public static string[] SplitRow(string row, char separator, char quote)
		{
			const string newSeparator = "^~";
			string output = "";
			bool inQuotes = false;
			for (int i = 0; i < row.Length; i++)
			{
				char chr = row[i];
				bool chrIsQuote = (chr == quote);
				bool chrIsSeparator = (chr == separator);
				bool chrNextIsQuote = (i + 1 < row.Length ? row[i + 1] == quote : false);

				if (chrIsSeparator && !inQuotes)
				{
					output += newSeparator;
					if (chrNextIsQuote)
					{
						inQuotes = true;
						i++;
					}
				}
				else
				if (chrIsQuote)
				{
					if (i == 0)
					{
						if (chrNextIsQuote)
						{
							inQuotes = true;
							i++;
						}
					}
					else
					if (inQuotes)
					{
						if (chrNextIsQuote)
						{
							output += chr;
							i++;
						}
						else
						{
							inQuotes = false;
						}
					}
				}
				else
				{
					output += chr;
				}
			}

			return output.Split(new string[] { newSeparator }, StringSplitOptions.None);
		}
	}
}
