using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace DBServiceQueries
{
	// Simple implementation of the abstract class that returns the number of rows resulting from the query
	internal class CounterSqlQuery : DBServiceBase<int>
	{
		internal override bool UseCompression { get { return false; }}
		internal override string SqlQuery { get
		{
			// Wrap the desired query with a results counter
			return string.Format("SELECT COUNT(*) AS Count FROM ({0}) AS [Dummy]", _query);
		}}

		private string _query;

		internal CounterSqlQuery(string query)
		{
			_query = query;
		}

		internal override IEnumerable ProcessResults(XElement element)
		{
			IEnumerable<int> query = from item in element.Descendants()
				select item.AttributeValue("Count").ToInt();
			return query;
		}
	}

	// Implementation of the abstract class that returns the results of a query
	internal class RowDataSqlQuery : DBServiceBase<RowData>
	{
		internal override bool UseCompression { get { return true; } }
		internal override string SqlQuery { get
		{
			return string.Format(_queryFormat, QueryItemsLimit, Offset);
		}}

		public int Offset = 0;
		public int QueryItemsLimit = 99999;

		private string _queryFormat;

		public RowDataSqlQuery(string queryFormat)
		{
			_queryFormat = queryFormat;
		}

		internal override IEnumerable ProcessResults(XElement element)
		{
			IEnumerable<RowData> query = from item in element.Descendants()
				select new RowData(item);
			return query;
		}
	}

	public class RowData
	{
		public ICollection<ColumnData> Columns { get; private set; }

		public RowData(XElement item)
		{
			Columns = new Collection<ColumnData>();
			if (item == null)
				return;

			foreach (XAttribute attribute in item.Attributes())
			{
				string alias = attribute.Name.ToString();
				string value = attribute.Value.ToString();
				Columns.Add(new ColumnData(alias, value));
			}
		}
	}

	public class ColumnData
	{
		public string Name { get; private set; }
		public string Value { get; private set; }

		public ColumnData(string name, string value)
		{
			Name = name;
			Value = value;
		}
	}
}