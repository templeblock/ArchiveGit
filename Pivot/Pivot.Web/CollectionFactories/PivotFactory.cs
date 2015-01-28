using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
using System.IO;
using System.Linq;
using ClassLibrary;
using CollectionLibrary;
using DBServiceQueries;
using FactoriesLibrary;

namespace CollectionFactories
{
	public class PivotFactory : CollectionFactoryBase
	{
		private const string SourcePathFormat = "{0}Cards\\All\\BCCM\\{1}.jpg";
		private const string DefaultFileID = "CF00000";
		private const string DefaultFactoryName = "Pivot"; // Name must match the requested CXML filename without the extension
		private const string DefaultCollectionName = "McPivot Baseball Database";
		private const string DefaultCsvFile = "Content/query.csv";
		private const string DefaultIconFile = "Images/Baseball.ico";
		private const string DefaultBrandImageFile = "Images/Baseball64.png";
		private const string DefaultSortFacet = "Debut Game";
		private const int DefaultViewerStateType = 2;
		private const int DefaultImageWidth = 358;//285;
		private const int DefaultImageHeight = 512;//408;
		private const int TotalItemsLimit = 7461; //3000;
		private const int QueryItemsLimit = 1500;

		private QueryItems _queryItems = new QueryItems();
		private Dictionary<int, string> _imageMap = new Dictionary<int, string>();
		private string _filePath;
		private string _sitePath;

		public PivotFactory()
		{
			base.FactoryName = DefaultFactoryName;
			base.SampleQueries = new string[] {
				string.Format("name={0}&queryFile={1}#$view$={2}&$facet0$={3}",
					DefaultCollectionName, DefaultCsvFile, DefaultViewerStateType, DefaultSortFacet),
			};
		}

		public override ItemImage MakeItemImage(int imageID)
		{
			string fileID = (_imageMap.ContainsKey(imageID) ? _imageMap[imageID] : null);
			return CreateItemImage(imageID, fileID);
		}

		private ItemImage CreateItemImage(int imageID, string fileID)
		{
			string sourcePath = string.Format(SourcePathFormat, _filePath, fileID);
			bool isFile = !sourcePath.StartsWith("http://");
			Size? commonSize = new Size(DefaultImageWidth, DefaultImageHeight);
			return new ItemImage(imageID, sourcePath, isFile, commonSize);
		}

		public override PivotCollection MakeCollection(CollectionRequestContext cxmlContext)
		{ //j How can I return errors (null collections) back to the user?
			try
			{
				// Typical request are as follows:
				//		Request.Url: http://mcpivot.com/Pivot.Web/Pivot.cxml?name=McPivot Baseball Database1&queryFile=query.csv
				//		Request.PhysicalPath: E:\web\dreamnitcom\htdocs\Pivot.Web\Pivot.cxml
#if NOTUSED
				static bool logged;
				if (!logged)
				{
					logged = true;
					string logPath = Path.GetDirectoryName(context.Request.PhysicalPath) + "\\logFactory.txt";
					using (FileStream logStream = new FileStream(logPath, FileMode.OpenOrCreate))
					{
						using (StreamWriter writer = new StreamWriter(logStream))
						{
							writer.WriteLine(context.Request.Url);
							writer.WriteLine(context.Request.PhysicalPath);
							writer.WriteLine(name);
						}
					}
				}
#endif
				// The factoryName is computed to be the filename, without the extension
				base.FactoryName = Path.GetFileNameWithoutExtension(cxmlContext.Context.Request.PhysicalPath);
				// Compute the file paths
				_filePath = Path.GetDirectoryName(cxmlContext.Context.Request.PhysicalPath) + '\\';
				_sitePath = cxmlContext.Url;
				int index = _sitePath.IndexOf('?');
				if (index >= 0)
					_sitePath = _sitePath.Substring(0, index);
				_sitePath = _sitePath.Substring(0, _sitePath.LastIndexOf('/') + 1);

				// Extract the query string variables
				string queryFile = cxmlContext.Query["queryFile"] ?? DefaultCsvFile;
				string iconFile = cxmlContext.Query["iconFile"] ?? DefaultIconFile;
				string brandImageFile = cxmlContext.Query["brandImageFile"] ?? DefaultBrandImageFile;
				string collectionName = cxmlContext.Query["name"] ?? DefaultCollectionName;

				// Load the query from the CSV
				if (!_queryItems.LoadFromCsv(_filePath + queryFile))
					return null;

				// Create the new collection
				PivotCollection collection = new PivotCollection(base.FactoryName);
				collection.Name = collectionName;
				collection.IconUrl = (iconFile != null ? new Uri(_sitePath + iconFile) : null);
				collection.BrandImageUrl = (brandImageFile != null ? new Uri(_sitePath + brandImageFile) : null);
				//collection.Supplement = "http://google.com";
				//collection.Copyright = new Link() { Name = "My Copyright", Href = "http://dreamnit.com" };
				//collection.HrefBase = "http://localhost:49000//";
				//collection.AdditionalSearchText = "Baseball";

				// Create the facet categories
				// Custom number formatting info: http://msdn.microsoft.com/en-us/library/0c899ak8.aspx
				// DateTime formatting info: http://msdn.microsoft.com/en-us/library/8kb3ddd4.aspx
				foreach (QueryItem queryItem in _queryItems)
				{
					if (queryItem.FacetName.StartsWith("$"))
						continue;
					collection.AddFacetCategory(queryItem.FacetName, queryItem.Type.ToFacetType(),
						queryItem.Format, queryItem.FilterPane, queryItem.InfoPane, queryItem.SortList);
				}

				string queryFormat = _queryItems.GetSqlQueryFormat();

				// Get the count
				int count = 0;
				string queryString = string.Format(queryFormat, 99999/*limit*/, 0/*offset*/);
				CounterSqlQuery countQuery = new CounterSqlQuery(queryString);
				countQuery.Completed += delegate(Collection<int> queryItems, string error)
				{
					if (error == null && queryItems != null && queryItems.Count >= 1)
						count = queryItems[0];
				};
				countQuery.Execute();

				// Load the collection from the database

				Collection<RowData> rows = null;
				RowDataSqlQuery query = new RowDataSqlQuery(queryFormat);
				query.Completed += delegate(Collection<RowData> queryItems, string error)
				{
					rows = queryItems;
				};

				int totalItems = 0;
				query.Offset = 0;
				while (true)
				{
					query.QueryItemsLimit = Math.Min(QueryItemsLimit, TotalItemsLimit - totalItems);
					query.Execute();
					if (rows == null || rows.Count <= 0)
						break;

					int maxID = 0;
					foreach (RowData row in rows)
					{
						int uniqueID = AddCollectionItem(collection, row);
						if (maxID < uniqueID) maxID = uniqueID;
					}

					totalItems += rows.Count;
					if (totalItems >= TotalItemsLimit)
						break;

					query.Offset = maxID + 1;
				}

				return collection;
			}
			catch (Exception ex)
			{
				return ErrorCollection.FromException(ex);
			}
		}

		private int AddCollectionItem(PivotCollection collection, RowData row)
		{
			try
			{
				int itemID = 0;
				int imageID = 0;
				string fileID = null;
				string name = null;
				string url = null;
				string desc  = null;

				Collection<Facet> facets = new Collection<Facet>();
				foreach (ColumnData column in row.Columns)
				{
					QueryItem queryItem = _queryItems.FindItemByName(column.Name);
					if (queryItem == null)
						continue;
					if (queryItem.FacetName.StartsWith("$"))
					{ // Look for the reserved facet names
						if (queryItem.FacetName == "$ItemID")		itemID = column.Value.ToInt(); else
						if (queryItem.FacetName == "$ImageID")		imageID = column.Value.ToInt(); else
						if (queryItem.FacetName == "$FileID")		fileID = column.Value; else
						if (queryItem.FacetName == "$Name")			name = column.Value; else
						if (queryItem.FacetName == "$Url")			url = column.Value; else
						if (queryItem.FacetName == "$Desc")			desc = column.Value; else
							continue;
						continue;
					}

					facets.AddFacet(queryItem.FacetName, column.Value.ChangeType(queryItem.Type));
				}

				if (collection.FindItem(itemID) != null)
					return itemID; // This item already exists

				//facets.AddFacet("Height", item.height.InchesToFeet());			//j SQL will have to do InchesToFeet()
				//facets.AddFacet("Birth Month", item.birthMonth.ToMonthName());	//j SQL will have to do ToMonthName()
				//facets.AddFacet("Death Month", item.deathMonth.ToMonthName());
				//facets.AddFacet("More on this player:",
				//    new Link("Baseball Reference", "BR/" + bbrefID),
				//    new Link("Baseball Page", "TBP/" + playerID),
				//    new Link("Baseball Almanac", "BA/" + playerID),
				//    new Link("Rotowire", "RW/" + playerID),
				//    new Link("Baseball Prospectus", "BP/" + playerID),
				//    new Link("Sports Mogul", "SM/" + playerID),
				//    new Link("Alex Reisner", "AR/" + playerID));

				//ICollection<Link> relatedLinks = new Collection<Link>() {
				//    new Link() { Name="Related Baseball Reference", Href="http://dreamnit.com" },
				//    new Link() { Name="Related Baseball Page", Href="http://dreamnit.com" },
				//};

				if (string.IsNullOrWhiteSpace(fileID) || fileID == DefaultFileID)
				{ // Using a "role" for a default image is a better idea
					imageID = 0;
					fileID = DefaultFileID;
				}

				if (!_imageMap.ContainsKey(imageID))
					_imageMap[imageID] = fileID;

				Item item = new Item(itemID, name, url, desc, CreateItemImage(imageID, fileID), facets.ToArray(), null/*relatedLinks*/);
				collection.AddItem(item);
				return itemID;
			}
			catch (Exception ex)
			{
				ex.GetType();
				return 0;
			}
		}
	}
}
