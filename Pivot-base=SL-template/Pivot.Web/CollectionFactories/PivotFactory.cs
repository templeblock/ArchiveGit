using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using Pivot.Web;
using ClassLibrary;
using PivotServerTools;

// SQL query builder references:
// http://www.codeproject.com/KB/database/QueryDesigner.aspx
// http://www.activequerybuilder.com/javademo.html

namespace CollectionFactories
{
	public enum AggregateType
	{
		Count,
		CountDistinct,
		Sum,
		SumDistinct,
		Avg,
		AvgDistinct,
		Min,
		MinDistinct,
		Max,
		MaxDistinct,
	}

	public enum SortType
	{
		NoSort,
		Ascending,
		Descending,
	}

	public class TypeConvert
	{
		private static Dictionary<string, Type> _dictionary = new Dictionary<string, Type>(StringComparer.OrdinalIgnoreCase);

		public static TT FromValue<TT>(string value)
		{
			try
			{
				if (typeof(TT).IsEnum)
					return (TT)Enum.Parse(typeof(TT), value, true/*ignoreCase*/);
				return (TT)Convert.ChangeType(value, typeof(TT));
			}
			catch (Exception ex)
			{
				ex.GetType();
				return default(TT);
			}
		}

		public static Type FromName(string name)
		{
			if (_dictionary.Count == 0)
				Initialize();
			if (_dictionary.Keys.Contains(name))
				return _dictionary[name];
			return typeof(string); // bad types default to string, including bool and bool[]
		}

		private static void Initialize()
		{
			_dictionary.Add("string", typeof(string));
			_dictionary.Add("int", typeof(int));
			_dictionary.Add("double", typeof(double));
			_dictionary.Add("DateTime", typeof(DateTime));
			_dictionary.Add("FacetHyperlink", typeof(FacetHyperlink));
			//_dictionary.Add("bool", typeof(bool));
			_dictionary.Add("string[]", typeof(string[]));
			_dictionary.Add("int[]", typeof(int[]));
			_dictionary.Add("double[]", typeof(double[]));
			_dictionary.Add("DateTime[]", typeof(DateTime[]));
			_dictionary.Add("FacetHyperlink[]", typeof(FacetHyperlink[]));
			//_dictionary.Add("bool[]", typeof(bool[]));
		}
	}

	public class QueryItems : List<QueryItem>
	{
		public bool LoadFromCsv(string path)
		{
			// Load the query definition from a CSV file
			try
			{
				using (FileStream fileStream = new FileStream(path, FileMode.Open, FileAccess.Read))
				{
					List<Dictionary<string, string>> queryItems = CsvReader.FromStream(fileStream);
					if (queryItems != null)
					{
						foreach (Dictionary<string, string> dictionary in queryItems)
							Add(new QueryItem(dictionary));
					}
				}

				return true;
			}
			catch (Exception ex)
			{
				ex.GetType();
				return false;
			}
		}

		public string GetSqlQueryFormat()
		{
			//j This must be built
			return @"
				SELECT TOP ({0}) 
				lahmanID AS _ID,
				nameFirst + ' ' + nameLast AS _0,
				'GU/'+nameLast+',+'+nameFirst AS _1, 
				'Bats: '+bats+'\r\nThrows: '+throws AS _2, 
				'yaz.jpg' as _3,
				lahmanID AS _4,
				playerID AS _5, 
				CAST((CASE WHEN managerID IS NULL THEN 0 ELSE 1 END) AS Bit) AS _6,
				CAST((CASE WHEN hofID IS NULL THEN 0 ELSE 1 END) AS Bit) AS _7,
				CAST(RTRIM(birthYear * 10000 + birthMonth * 100 + birthDay) AS datetime) AS _8, 
				birthCity + ', ' +  birthState + ' ' + birthCountry AS _11,
				CAST(RTRIM(deathYear * 10000 + deathMonth * 100 + deathDay) AS datetime) AS _12, 
				deathCity + ', ' +  deathState + ' ' + deathCountry AS _15, 
				nameFirst AS _16,
				nameLast AS _17,
				nameNote AS _18,
				nameGiven AS _19,
				nameNick AS _20, 
				weight AS _21,
				height AS _22,
				bats AS _23,
				throws AS _24, 
				debut AS _25,
				finalGame AS _26,
				college AS _27,
				bbrefID AS _28, 
				'Baseball Reference=BR/'+bbrefID+',Baseball Page=TBP/'+playerID AS _29
				FROM bb_Master AS m
				WHERE (lahmanID >= {1}) AND (debut BETWEEN '1960-01-01' AND '1970-01-01')
				ORDER BY lahmanID
			";
		}

		public QueryItem FindItemByAlias(string alias)
		{
			foreach (QueryItem item in this)
			{
				if (!item.Output)
					continue;
				if (alias == item.Alias)
					return item;
			}

			return null;
		}
	}

	public class QueryItem
	{
		public string Table;
		public string[] Variables;
		public string Expression;
		public string Alias;
		public Type Type;
		public string Criteria;
		public string CriteriaOr;
		public bool Group;
		public SortType? SortType;
		public int SortOrder;

		public string FacetName;
		public bool FilterPane;
		public bool SortList;
		public bool InfoPane;
		public string Format;
		public bool Output;

		private static int _arbitraryIndex;

		public QueryItem(Dictionary<string, string> dictionary)
		{
			Table = dictionary["Table"];
			Variables = dictionary["Variables"].Split(',');
			Expression = dictionary["Expression"];
			Alias = dictionary["Alias"];
			Type = TypeConvert.FromName(dictionary["Type"]);
			Criteria = dictionary["Criteria"];
			CriteriaOr = dictionary["CriteriaOr"];
			Group = dictionary["Group"].ToBool();
			SortType = TypeConvert.FromValue<SortType>(dictionary["SortType"]);
			SortOrder = dictionary["SortOrder"].ToInt();

			FacetName = dictionary["FacetName"];
			FilterPane = dictionary["FilterPane"].ToBool();
			SortList = dictionary["SortList"].ToBool();
			InfoPane = dictionary["InfoPane"].ToBool();
			Format = dictionary["Format"];

			if (string.IsNullOrWhiteSpace(Alias))
				Alias = GetArbitraryAlias();
			Output = !string.IsNullOrWhiteSpace(FacetName);
		}

		private string GetArbitraryAlias()
		{
			return string.Format("#{0}", _arbitraryIndex++);
		}
	}

	public class PivotFactory : CollectionFactoryBase
	{
		private QueryItems _queryItems = new QueryItems();
		private string _filePath;
		private string _sitePath;

		public PivotFactory()
		{
			// Data accessible to the web page:
			//	 Discovered factories: PivotHttpHandlers.GetCollectionUrls()
			//	 Name, Summary, and SampleQueries: PivotHttpHandlers.CollectionInfoHtml()
			base.Name = "Pivot"; // Name must match the requested CXML filename without the extension or the query string
			base.Summary = "Create a collection derived from Lahman baseball database.";
			base.SampleQueries = new string[] {
				"name=Baseball Master Database1&queryFile=query.csv#$view$=2&$facet0$=Debut Game",
				"name=Baseball Master Database2&queryFile=query.csv#$view$=2&$facet0$=Debut Game",
			};
#if false
			// SQL query functions:
			//		DISTINCT() COUNT(1), COUNT(DISTINCT column), SUM(), AVG(), MIN(), MAX(), UPPER(), LOWER(), 
			//		MONTH(date), DAY(date) YEAR(date), BETWEEN()
			// SELECT
			// TOP
			// FROM
			// [INNER | [LEFT | FULL] OUTER] JOIN
			// WHERE ()
			// DISTINCT eliminates redundant results
			// IN(), NOT IN() used in a WHERE clause to find a value in a set, as in: birthState IN ('MA', 'CA')
			// GROUP BY
			// HAVING - similar to WHERE; only applied after SELECT is performed; mainly used with group functions; used to ELIMINATE rows from the selection
			// ORDER BY (ASC|DESC) example
			// Sub-query SELECT * FROM (SELECT)\
			//		SELECT * FROM tablename WHERE value > (select statement that returns one value)

			// Count the results of a sub-query
			string query0 = @"
				SELECT  COUNT(1) AS Count
				FROM    (SELECT DISTINCT birthState
						  FROM            bb_Master AS m
						  WHERE        (birthState IN ('MA', 'CA'))) AS Dummy
				";
			//2

			// Aggregate: How many players were born in Massachusetts or California
			string query1 = @"
				SELECT        birthState AS state, COUNT(1) AS total
				FROM            bb_Master AS m
				WHERE        (birthState IN ('MA', 'CA'))
				GROUP BY birthState
				";
			//MA	643
			//CA	1977

			// Aggregate: How many states do we have players from?
			string query2 = @"
				SELECT        COUNT(DISTINCT birthState) AS total
				FROM            bb_Master AS m
				";
			// 79

			// Aggregate: From what states do we have more than 1200 players?
			string query3 = @"
				SELECT        birthState, COUNT(1) AS total
				FROM            bb_Master AS m
				GROUP BY birthState
				HAVING        (COUNT(1) > 1200)
				";
			//PA	1334
			//NULL	1858
			//CA	1977

			// A player's yearly statistics
			string query4 = @"
				SELECT        m.playerID, m.nameFirst, m.nameLast, b.yearID, b.AB
				FROM            bb_Master AS m INNER JOIN
										 bb_Batting AS b ON m.playerID = b.playerID
				WHERE        (b.playerID = 'aaronha01')
				ORDER BY b.yearID ASC
				";

			// A player's total years played, and his career batting average
			string query5 = @"
				SELECT        m.playerID, m.nameFirst, m.nameLast, COUNT(DISTINCT b.yearID) AS Years, SUM(b.H) * 1000 / SUM(b.AB) AS 'BA'
				FROM            bb_Master AS m INNER JOIN
										 bb_Batting AS b ON m.playerID = b.playerID
				WHERE        (b.playerID = 'aaronha01')
				GROUP BY m.playerID, m.nameFirst, m.nameLast
			";
			// aaronha01	Hank	Aaron	23	304

SELECT        m.playerID, m.nameFirst, m.nameLast, COUNT(DISTINCT b.yearID) AS Years, SUM(b.H) * 1000 / SUM(b.AB) AS 'BA',
STUFF((SELECT ',' + b.teamID AS Team
FROM            bb_Teams AS t
WHERE        (teamID = b.teamID)
ORDER BY teamID
FOR XML PATH(''), TYPE, ROOT).value('root[1]','nvarchar(max)'), 1, 1, '') as Teams

FROM            bb_Master AS m INNER JOIN
bb_Batting AS b ON m.playerID = b.playerID
WHERE        (b.playerID = 'aaronha01')
GROUP BY m.playerID, m.nameFirst, m.nameLast, b.teamID

SELECT        playerID, yearID
FROM            bb_Batting AS p1
WHERE        (playerID = 'aardsda01')
GROUP BY playerID, yearID

DECLARE @PageSize INT;
SET @PageSize = 3;
SELECT        playerID,
(SELECT        TOP (@PageSize) yearID AS [data()]
FROM            bb_Batting AS b2
WHERE        (playerID = b.playerID)
ORDER BY yearID FOR XML PATH('')) AS Years
FROM            bb_Batting AS b
WHERE        (playerID = 'aardsda01')
GROUP BY playerID

SELECT        playerID,
(SELECT        TOP (100) PERCENT CAST(yearID AS VARCHAR(MAX)) + ',' AS y
FROM            bb_Batting AS p2
WHERE        (playerID = p1.playerID)
ORDER BY yearID FOR XML PATH('')) AS Years
FROM            bb_Batting AS p1
WHERE        (playerID = 'aardsda01')
GROUP BY playerID

#endif
		}

		public override Collection MakeCollection(CollectionRequestContext cxmlContext)
		{ //j How can I return errors (null collections) back to the user?
			try
			{
				// Return the cached collection if we have one
				if (cxmlContext.CachedCollection != null)
					return cxmlContext.CachedCollection;

				// Compute the file paths
				_filePath = Path.GetDirectoryName(cxmlContext.Context.Request.PhysicalPath) + '\\';
				_sitePath = cxmlContext.Url;
				int index = _sitePath.IndexOf('?');
				if (index >= 0)
					_sitePath = _sitePath.Substring(0, index);
				_sitePath = _sitePath.Substring(0, _sitePath.LastIndexOf('/') + 1);

				// Extract the query string variables
				string queryFile = cxmlContext.Query["queryFile"] ?? "defaultQuery.csv";
				string iconUrl = cxmlContext.Query["iconUrl"] ?? "Baseball.ico";
				string name = cxmlContext.Query["name"] ?? queryFile;

				// Load the query from the CSV
				if (!_queryItems.LoadFromCsv(_filePath + "Content/" + queryFile))
					return null;

				// Create the new collection
				Collection collection = new Collection(cxmlContext.Context.Request.Url);
				collection.Name = name;
				collection.IconUrl = (iconUrl != null ? new Uri(_sitePath + "Images/" + iconUrl) : null);
				collection.EnableInfoPaneBingSearch = true; // Not stored in CXML file
				//collection.BrandImage = ??
				//collection.AdditionalSearchText = ??

				// Create the facet categories, formats, and display options
				// Custom number formatting info: http://msdn.microsoft.com/en-us/library/0c899ak8.aspx
				// DateTime formatting info: http://msdn.microsoft.com/en-us/library/8kb3ddd4.aspx
				foreach (QueryItem item in _queryItems)
				{
					collection.AddFacetCategory(item.FacetName, item.Type.ToFacetType(),
						item.Format, item.FilterPane, item.InfoPane, item.SortList);
				}

				string queryFormat = _queryItems.GetSqlQueryFormat();

				// Get the count
				int count = 0;
				string queryString = string.Format(queryFormat, 99999/*limit*/, 0/*offset*/);
				CountResultsSqlQuery countQuery = new CountResultsSqlQuery(queryString);
				countQuery.Result += delegate(Collection<int> queryItems, string error)
				{
					if (error == null && queryItems != null && queryItems.Count >= 1)
						count = queryItems[0];
				};
				countQuery.Execute();

				// Load the collection from the database
				const int totalItemsLimit = 3000;
				const int queryItemsLimit = 750;
				Collection<Row> rows = null;
				RowsSqlQuery query = new RowsSqlQuery(queryFormat);
				query.Result += delegate(Collection<Row> queryItems, string error)
				{
					rows = queryItems;
				};

				int totalItems = 0;
				while (true)
				{
					query.QueryItemsLimit = Math.Min(queryItemsLimit, totalItemsLimit - totalItems);
					query.Execute();
					if (rows == null || rows.Count <= 0)
						break;

					foreach (Row row in rows)
						LoadCollectionItem(collection, row);

					totalItems += rows.Count;
					if (totalItems >= totalItemsLimit)
						break;

					query.Offset = rows[rows.Count - 1].ID + 1;
				}

				// Cache the CXML and DZC files for better performance
				try
				{
					string cacheFolder = _filePath;
					collection.ToCxml(cacheFolder + collection.CollectionKey + ".cxml");
					//using (StreamWriter textWriter = File.CreateText(cacheFolder + collection.CollectionKey + ".dzc"))
					//    collection.ToDzc(textWriter);
				}
				catch (Exception ex)
				{
					ex.GetType();
				}

				return collection;
			}
			catch (Exception ex)
			{
				return ErrorCollection.FromException(ex);
			}
		}

		private void LoadCollectionItem(Collection collection, Row row)
		{
			try
			{
				string itemName  = null;
				string itemLink  = null;
				string itemDesc  = null;
				string itemImage = null;

				Collection<Facet> facets = new Collection<Facet>();
				foreach (Column column in row.Columns)
				{
					QueryItem item = _queryItems.FindItemByAlias(column.Alias);
					if (item == null)
						continue;
					if (!item.FacetName.StartsWith("$"))
						facets.AddFacet(item.FacetName, ChangeType(column.Value, item.Type));
					else
					{
						if (item.FacetName == "$Name")	itemName  = column.Value; else
						if (item.FacetName == "$Link")	itemLink  = column.Value; else
						if (item.FacetName == "$Desc")	itemDesc  = column.Value; else
						if (item.FacetName == "$Image")	itemImage = column.Value;
					}
				}

				//facets.AddFacet("Height", item.height.InchesToFeet());			//j SQL will have to do InchesToFeet()
				//facets.AddFacet("Birth Month", item.birthMonth.ToMonthName());	//j SQL will have to do ToMonthName()
				//facets.AddFacet("Death Month", item.deathMonth.ToMonthName());
				//facets.AddFacet("More on this player:",
				//    new FacetHyperlink("Baseball Reference", "BR/" + bbrefID),
				//    new FacetHyperlink("Baseball Page", "TBP/" + playerID),
				//    new FacetHyperlink("Baseball Almanac", "BA/" + playerID),
				//    new FacetHyperlink("Rotowire", "RW/" + playerID),
				//    new FacetHyperlink("Baseball Prospectus", "BP/" + playerID),
				//    new FacetHyperlink("Sports Mogul", "SM/" + playerID),
				//    new FacetHyperlink("Alex Reisner", "AR/" + playerID));

				ItemImage image = (string.IsNullOrWhiteSpace(itemImage) ? null : new ItemImage(_filePath + itemImage));
				//ItemImage image = (string.IsNullOrWhiteSpace(itemImage) ? null : new ItemImage(new Uri(_sitePath + itemImage)));
				collection.AddItem(itemName, itemLink, itemDesc, image, facets.ToArray());
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private object ChangeType(string valueString, Type type)
		{
			try
			{
				// Nulls are returned if the value is the default value for that type
				//  This results in "no-info" data in PivotViewer
				if (string.IsNullOrEmpty(valueString))
					return null;
				if (type == typeof(FacetHyperlink))
				{ // Special case
					string[] values = valueString.Split('=');
					return (values.Length >= 2 ? new FacetHyperlink(values[0], values[1]) : null);
				}
				if (type.HasElementType) // Do e want this converted to an array?
				{
					string[] array = valueString.Split(',');
					Type newType = type.GetElementType();
					if (newType == typeof(string))
						return array;
					object[] newArray = new object[array.Length];
					int i = 0;
					foreach (string item in array)
						newArray[i++] = ChangeType(item, newType);
					return newArray;
				}
				object value = Convert.ChangeType(valueString, type);
				if (value == null)
					return null;
				object defaultValue = type.IsValueType ? Activator.CreateInstance(type) : null;
				return (value.Equals(defaultValue) ? null : value);
			}
			catch (Exception ex)
			{
				ex.GetType();
				return valueString;
			}
		}
	}

	public static class CollectionExtensions
	{
		// Extension for Collection<Facet>
		public static void AddFacet(this Collection<Facet> facets, string facetName, FacetType facetType, params object[] tags)
		{
			// Save memory by not adding Facets with null data
			if (tags == null || tags[0] == null)
				return;

			if (tags[0].GetType().IsArray)
				tags = tags[0] as object[];
			facets.Add(new Facet(facetName, facetType, tags));
		}

		// Extension for Collection<Facet>
		public static void AddFacet(this Collection<Facet> facets, string facetName, params object[] tags)
		{
			// Save memory by not adding Facets with null data
			if (tags == null || tags[0] == null)
				return;

			if (tags[0].GetType().IsArray)
				tags = tags[0] as object[];
			FacetType facetType = tags[0].GetType().ToFacetType();
			facets.Add(new Facet(facetName, facetType, tags));
		}

		// Extension for Type
		public static FacetType ToFacetType(this Type type)
		{
			if (type == typeof(string) || type == typeof(string[]))
				return FacetType.Text;
			if (type == typeof(int) || type == typeof(int[]))
				return FacetType.Number;
			if (type == typeof(double) || type == typeof(double[]))
				return FacetType.Number;
			if (type == typeof(DateTime) || type == typeof(DateTime[]))
				return FacetType.DateTime;
			if (type == typeof(FacetHyperlink) || type == typeof(FacetHyperlink[]))
				return FacetType.Link;
			if (type == typeof(bool) || type == typeof(bool[]))
				return FacetType.Text;
			throw new ArgumentException("ComputeFacetType failed on an unknown type '" + type + "'.");
		}

		//j SQL will have to do ToMonthName()
		//public static string ToMonthName(this int mm)
		//{
		//    if (mm < 1 || mm > 12)
		//        return null;
		//    string[] months = {
		//        "January", "February", "March", "April", "May", "June",
		//        "July", "August", "September", "October", "November", "December" };
		//    return months[mm-1];
		//}

		//j SQL will have to do InchesToFeet()
		//public static int InchesToFeet(this double totalInches)
		//{
		//    if (totalInches <= 0)
		//        return 0;
		//    int feet = (int)totalInches / 12;
		//    int inches = (int)totalInches % 12;
		//    int result = feet * 100 + inches;
		//    result = (int)(totalInches * 100 / 12);
		//    return result;
		//}
	}

	internal class RowsSqlQuery : DBServiceBase<Row>
	{
		public int Offset = 0;
		public int QueryItemsLimit = 99999;

		private string _queryFormat;

		public RowsSqlQuery(string queryFormat)
		{
			_queryFormat = queryFormat;
		}

		internal override string SqlQuery()
		{
			return string.Format(_queryFormat, QueryItemsLimit, Offset);
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<Row> query = from item in element.Descendants()
				select new Row(item);
			return query;
		}
	}

	public class Row
	{
		public List<Column> Columns { get; private set; }
		public int ID { get; private set; }

		public Row(XElement item)
		{
			Columns = new List<Column>();
			if (item == null)
				return;

			foreach (XAttribute attribute in item.Attributes())
			{
				string alias = attribute.Name.ToString();
				string value = attribute.Value.ToString();
				Columns.Add(new Column(alias, value));
				//j Hack!
				if (alias == "_ID")
					ID = value.ToInt();
			}
		}
	}

	public class Column
	{
		public string Alias { get; private set; }
		public string Value { get; private set; }

		public Column(string alias, string value)
		{
			Alias = alias;
			Value = value;
		}
	}
}
