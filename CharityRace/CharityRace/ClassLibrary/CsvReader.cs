using System;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Resources;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	public class CsvReader
	{
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

		public static List<Dictionary<string, string>> FromStream(Stream stream)
		{
			if (stream == null)
				return null;

			List<Dictionary<string, string>> dataset = new List<Dictionary<string, string>>();
			string[] header = null;
			string line = null;
			int lineCount = 0;
			StreamReader sr = new StreamReader(stream);
			while ((line = sr.ReadLine()) != null)
			{
				lineCount++;
				// Allow lines to be commented out
				if (line.StartsWith("//"))
					continue;

				string[] fields = CsvReader.SplitLine(line, ',', '"');
				if (header == null)
				{
					header = fields;
					continue;
				}

				if (fields.Length != header.Length)
					throw new Exception(string.Format("Bad CSV format on line {0}", lineCount));

				Dictionary<string, string> fieldset = new Dictionary<string, string>();
				int i = 0;
				foreach (string field in fields)
					fieldset.Add(header[i++].Trim(), field.Trim());

				dataset.Add(fieldset);
			}

			return dataset;
		}

		public static string[] SplitLine(string line, char separator, char quote)
		{
			const string newSeparator = "^~";
			string output = "";
			bool inQuotes = false;
			for (int i = 0; i < line.Length; i++)
			{
				char chr = line[i];
				bool chrIsQuote = (chr == quote);
				bool chrIsSeparator = (chr == separator);
				bool chrNextIsQuote = (i + 1 < line.Length ? line[i + 1] == quote : false);

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
