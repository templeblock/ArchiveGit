using System;
using System.Collections.Generic;
using System.IO;
#if SILVERLIGHT
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Resources;
using System.Linq.Expressions;
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
			string line = null;
			int lineCount = 0;
			StreamReader sr = new StreamReader(stream);
			while ((line = sr.ReadLine()) != null)
			{
				lineCount++;
				// Allow lines to be commented out
				if (line.StartsWith("//"))
					continue;

				string[] columns = SplitLine(line, ',', '"');
				// If all columns are empty, skip this line
				if (columns.Length == columns.Count(column => string.IsNullOrWhiteSpace(column)))
					continue;

				if (header == null)
				{
					header = columns;
					continue;
				}

				if (columns.Length != header.Length)
					throw new Exception(string.Format("Bad CSV format on line {0}", lineCount));

				Dictionary<string, string> columnSet = new Dictionary<string, string>();
				int i = 0;
				foreach (string column in columns)
					columnSet.Add(header[i++].Trim(), column.Trim());

				dataset.Add(columnSet);
			}

			return dataset;
		}

		public static string[] SplitLine(string line, char separator, char quote)
		{
			const string newSeparator = "^~";
			string output = string.Empty;
			bool inQuotes = false;
			foreach (char chr in line)
			{
				bool isQuote = (chr == quote);
				bool isSeparator = (chr == separator);

				if (isQuote)
					inQuotes = !inQuotes;
				else
				if (isSeparator && !inQuotes)
					output += newSeparator;
				else
					output += chr;
			}

			return output.Split(new[] { newSeparator }, StringSplitOptions.None);
		}
	}

	public static class CsvWriter
	{
		// Example
		//public class Person
		//{
		//    public string FirstName { get; set; }
		//    public string LastName { get; set; }
		//}

		//public static void Example()
		//{
		//    Person[] people = new[]
		//    {
		//        new Person() {FirstName = "Joe", LastName = "Bloggs"},
		//        new Person() {FirstName = "Fred", LastName = "Bloggs"},
		//    };
		//    using (Stream fileStream = new FileStream("People.csv", FileMode.Create, FileAccess.Write, FileShare.None))
		//        people.ToCsv(fileStream, x => x.LastName, x => x.FirstName);
		//}

		public static void ToCsv<T>(this IEnumerable<T> objects, Stream outputStream, params Expression<Func<T, object>>[] outputValues)
		{
			objects.ToCsv(outputStream, Encoding.UTF8, ',', "\r\n", '"', true, null/*columnHeaders*/, outputValues);
		}

		public static void ToCsv<T>(this IEnumerable<T> objects, Stream outputStream, Encoding encoding, char columnSeparator, string lineTerminator, char encapsulationCharacter, bool autoGenerateColumnHeaders, string[] columnHeaders, params Expression<Func<T, object>>[] outputValues)
		{
			StreamWriter writer = new StreamWriter(outputStream, encoding);
			WriteColumnHeaders(writer, columnSeparator, lineTerminator, encapsulationCharacter, autoGenerateColumnHeaders, columnHeaders, outputValues);
			WriteData(objects, writer, columnSeparator, lineTerminator, encapsulationCharacter, outputValues);
			writer.Flush();
		}

		private static string EncapsulateIfRequired(this string theString, char columnSeparator, char encapsulationCharacter)
		{
			if (theString.Contains(columnSeparator))
				return String.Format("{1}{0}{1}", theString, encapsulationCharacter);
			return theString;
		}

		private static void WriteColumnHeaders<T>(StreamWriter writer, char columnSeparator, string lineTerminator, char encapsulationCharacter, bool autoGenerateColumnHeaders, string[] columnHeaders, params Expression<Func<T, object>>[] outputValues)
		{
			if (autoGenerateColumnHeaders)
			{
				foreach (Expression<Func<T, object>> expression in outputValues)
				{
					string columnHeader;
					if (expression.Body is MemberExpression)
						columnHeader = ((MemberExpression)expression.Body).Member.Name;
					else
						columnHeader = expression.Body.ToString();

					writer.Write(String.Format("{0}{1}", columnHeader.EncapsulateIfRequired(columnSeparator, encapsulationCharacter), columnSeparator));
				}

				writer.Write(lineTerminator);
			}
			else
			{
				if (columnHeaders != null && columnHeaders.Length > 0)
				{
					if (columnHeaders.Length == outputValues.Length)
					{
						foreach (string t in columnHeaders)
							writer.Write(String.Format("{0}{1}", t.EncapsulateIfRequired(columnSeparator, encapsulationCharacter), columnSeparator));
						writer.Write(lineTerminator);
					}
					else
						throw new ArgumentException("The number of column headers does not match the number of output values.");
				}
			}
		}

		private static void WriteData<T>(IEnumerable<T> objects, StreamWriter writer, char columnSeparator, string lineTerminator, char encapsulationCharacter, params Expression<Func<T, object>>[] outputValues)
		{
			foreach (T obj in objects)
			{
				foreach (Expression<Func<T, object>> t in outputValues)
				{
					Func<T, object> valueFunc = t.Compile();
					object value = valueFunc(obj);
					if (value != null)
						writer.Write(value.ToString().EncapsulateIfRequired(columnSeparator, encapsulationCharacter));

					writer.Write(columnSeparator);
				}

				writer.Write(lineTerminator);
			}
		}
	}
}
