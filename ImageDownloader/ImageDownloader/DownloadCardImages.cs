using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data;
using System.Data.OleDb;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Xml.Linq;

namespace ImageDownloaderApp
{
	public class DownloadCardImages
	{
		public static void DownloadFromBBCyberMuseum(bool rejectedOnly = true)
		{
			const string fileIDPrefix = "CF";
			const string vendor = "BCCM";
			string root = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\";
			string csvPath = root + "ScrapedData" + vendor + ".csv.txt";
			root += vendor;
			FileStream csvStream = new FileStream(csvPath, FileMode.Create);
			StreamWriter csv = new StreamWriter(csvStream);
			csv.WriteLine("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}",
				"vendor", "year", "team", "last", "first", "rejected", "jpg", "#card", "vendorID", "fileID");

			const string urlFormat = "http://www.guitar9.com/bc/displaybaseballcards.pl?{0}lastaction=List+All&action=List+All&seq={1}&flip=&display=1x1f";

			const string jpgStart = "http://lh";
			const string jpgEnd = "\" width=\"";

			const string teamStart = "team\">";
			const string teamEnd = "</a>";

			const string nameStart = "flip=&amp;position=";
			const string nameEnd = "\" title=";
			const string lastEnd = ", ";

			const string yearStart = "year\">'";
			const string yearEnd = "</a>";

			const string numStart = "</a> (";
			const string numEnd = ")</font>";

			const string login = "emailuserid=jimmccurdy1@verizon.net&passwd=7202jimm&genaction=SignIn&";

			WebClient webClient = new WebClient { UseDefaultCredentials = true };
			int retries = 0;
			string rejected = string.Empty;
			string jpg = "";
			string team = "";
			string first = "";
			string last = "";
			string year = "";
			string num = "";

			const int limit = 50531;
			for (int i = 0; i < limit; i++)
			{
				if (i % 50 == 0)
					Debug.WriteLine(i.ToString());

				if (i > 0 && (!rejectedOnly || !string.IsNullOrEmpty(rejected)))
				{
					int vendorID = (i - 1);
					csv.WriteLine("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}\t{10}\t{11}",
						vendor, year, team, last, first, "#" + num, vendorID, IdToFileId(fileIDPrefix, vendorID), rejected, "", "", jpg);
				}

				rejected = string.Empty;
				jpg = "";
				team = "";
				first = "";
				last = "";
				year = "";
				num = "";

				try
				{
					// Download the page
					string results = webClient.DownloadString(string.Format(urlFormat, login, i));
					if (string.IsNullOrWhiteSpace(results))
						continue;

					if (!Grab(results, jpgStart, jpgEnd, ref jpg))
					{
						rejected = "No Jpg";
						continue;
					}
					jpg = jpgStart + jpg;

					if (!Grab(results, yearStart, yearEnd, ref year))
					{
						rejected = "No year";
						continue;
					}
					bool twentyFirstCentury = (year.Length >= 1 && year[0] <= '1');
					year = (twentyFirstCentury ? "20" : "19") + year;

					if (!Grab(results, nameStart, nameEnd, ref last))
					{
						rejected = "No name";
						continue;
					}

					if (!Grab(results, numStart, numEnd, ref num))
					{
						rejected = "No card number";
						continue;
					}

					string name = last;
					if (!Grab(name, null, lastEnd, ref last))
					{
						rejected = "No last name";
						continue;
					}

					if (!Grab(name, lastEnd, null, ref first))
					{
						rejected = "No first name";
						continue;
					}

					if (!Grab(results, teamStart, teamEnd, ref team))
					{
						rejected = "No team";
						continue;
					}

					// If this is not a single player card...
					if (last.IndexOf("Checklist") >= 0 || first.IndexOf("Checklist") >= 0 ||
						last.IndexOf("Team") >= 0 || first.IndexOf("Team") >= 0 ||
						last.IndexOf("Rookie") >= 0 || first.IndexOf("Rookie") >= 0 ||
						last.IndexOf("Future") >= 0 || first.IndexOf("Future") >= 0 ||
						last.IndexOf("Leaders") >= 0 || first.IndexOf("Leaders") >= 0 ||
						last.IndexOf("All-Star") >= 0 || first.IndexOf("All-Star") >= 0 ||
						last.IndexOf("Coaches") >= 0 || first.IndexOf("Coaches") >= 0 ||
						last.IndexOf("Stars") >= 0 || first.IndexOf("Stars") >= 0 ||
						(last.IndexOf(" ") >= 0 && first.IndexOf(" ") >= 0) ||
						string.IsNullOrWhiteSpace(first) || string.IsNullOrWhiteSpace(last))
					{
						rejected = "Not a single player";
						continue;
					}

					string filePath = root + "\\" + IdToFileId(fileIDPrefix, i) + ".jpg";
#if NOTUSED
					if (!File.Exists(filePath))
					{ // Download the image
						CreateFolder(filePath);
						webClient.DownloadFile(jpg, filePath);
					}
#endif
					retries = 0;
				}
				catch (Exception ex)
				{
					if (++retries <= 3)
						i--; // retry
					else
					{
						retries = 0;
						rejected = "Exception: " + ex.Message;
					}
				}
			}

			csv.Dispose();
			csvStream.Dispose();
		}

		public static void DownloadFromBBSimulator(bool rejectedOnly = true)
		{
			const string fileIDPrefix = "SF";
			const string vendor = "BSIM";
			string root = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\";
			string csvPath = root + "ScrapedData" + vendor + ".csv.txt";
			root += vendor;
			FileStream csvStream = new FileStream(csvPath, FileMode.Create);
			StreamWriter csv = new StreamWriter(csvStream);
			csv.WriteLine("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}",
				"vendor", "year", "team", "last", "first", "rejected", "jpg", "#card", "vendorID", "fileID");

			const string urlFormat = "http://baseballsimulator.com/baseballcards/front.php?id={0}";

			const string jpgStart = "<center><img src=";
			const string jpgEnd = " USEMAP";

			const string team1Start = "</font></p><p>";
			const string team2Start = " -  ";
			const string teamEnd = "</p>";

			const string firstStart = "twitter.com/home?status=";
			const string firstEnd = "+";

			const string lastEnd = "+";

			const string yearEnd = " - ";

			const string numStart = "Card Number: ";
			const string numEnd = "</p>";

			WebClient webClient = new WebClient { UseDefaultCredentials = true };
			int retries = 0;
			string rejected = string.Empty;
			string jpg = "";
			string team = "";
			string first = "";
			string last = "";
			string year = "";
			string num = "";

			const int limit = 37150;
			for (int i = 1; i < limit; i++)
			{
				if (i % 50 == 0)
					Debug.WriteLine(i.ToString());

				if (i > 0 && (!rejectedOnly || !string.IsNullOrEmpty(rejected)))
				{
					int vendorID = (i - 1);
					csv.WriteLine("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}\t{10}\t{11}",
						vendor, year, team, last, first, "#" + num, vendorID, IdToFileId(fileIDPrefix, vendorID), rejected, "", "", jpg);
				}

				rejected = string.Empty;
				jpg = "";
				team = "";
				first = "";
				last = "";
				year = "";
				num = "";

				string[] twoWordCities = { "New York", "St. Louis", "Kansas City", "San Diego", "San Francisco", "Tampa Bay", "Los Angeles", "Fort Wayne", "New Haven", "St. Paul" };
				try
				{
					// Download the page
					string results = webClient.DownloadString(string.Format(urlFormat, i));
					if (string.IsNullOrWhiteSpace(results))
						continue;

					if (!Grab(results, jpgStart, jpgEnd, ref jpg))
					{
						rejected = "No Jpg";
						continue;
					}

					if (!Grab(results, firstStart, firstEnd, ref first))
					{
						rejected = "No first name";
						continue;
					}

					string lastStart = first + firstEnd;
					if (!Grab(results, lastStart, lastEnd, ref last))
					{
						rejected = "No last name";
						continue;
					}

					string yearStart = last + lastEnd;
					if (!Grab(results, yearStart, yearEnd, ref year))
					{
						rejected = "No year";
						continue;
					}

					if (!Grab(results, numStart, numEnd, ref num))
					{
						rejected = "No card number";
						continue;
					}

					string team1 = "";
					if (!Grab(results, team1Start, teamEnd, ref team1))
					{
						rejected = "No team";
						continue;
					}

					if (!Grab(team1 + teamEnd, team2Start, teamEnd, ref team))
					{
						rejected = "No team";
						continue;
					}

					bool twoWordCity = false;
					foreach (string city in twoWordCities)
					{
						if (team.StartsWith(city))
						{
							twoWordCity = true;
							break;
						}
					}

					int index = team.IndexOf(" ") + 1;
					if (twoWordCity)
						index = team.IndexOf(" ", index) + 1;
					if (index > 0)
						team = team.Substring(index);

					// If this is not a single player card...
					if (last.IndexOf("Checklist") >= 0 || first.IndexOf("Checklist") >= 0 ||
						last.IndexOf("Team") >= 0 || first.IndexOf("Team") >= 0 ||
						last.IndexOf("Rookie") >= 0 || first.IndexOf("Rookie") >= 0 ||
						last.IndexOf("Future") >= 0 || first.IndexOf("Future") >= 0 ||
						last.IndexOf("Leaders") >= 0 || first.IndexOf("Leaders") >= 0 ||
						last.IndexOf("All-Star") >= 0 || first.IndexOf("All-Star") >= 0 ||
						last.IndexOf("Coaches") >= 0 || first.IndexOf("Coaches") >= 0 ||
						last.IndexOf("Stars") >= 0 || first.IndexOf("Stars") >= 0 ||
						(last.IndexOf(" ") >= 0 && first.IndexOf(" ") >= 0) ||
						string.IsNullOrWhiteSpace(first) || string.IsNullOrWhiteSpace(last))
					{
						rejected = "Not a single player";
						continue;
					}

					string filePath = root + "\\" + IdToFileId(fileIDPrefix, i) + ".jpg";
#if NOTUSED
					if (!File.Exists(filePath))
					{ // Download the image
						CreateFolder(filePath);
						webClient.DownloadFile(jpg, filePath);
					}
#endif
					retries = 0;
				}
				catch (Exception ex)
				{
					if (++retries <= 3)
						i--; // retry
					else
					{
						retries = 0;
						rejected = "Exception: " + ex.Message;
					}
				}
			}

			csv.Dispose();
			csvStream.Dispose();
		}

		private static string IdToFileId(string prefix, int id)
		{
			return string.Format("{0}{1:D5}", prefix, id);
		}

		private static void CreateFolder(string filePath)
		{
			string path = Path.GetDirectoryName(filePath);
			if (!Directory.GetParent(path).Exists)
				CreateFolder(Directory.GetParent(path).FullName);
			if (!Directory.Exists(path))
				Directory.CreateDirectory(path);
		}

		private static bool Grab(string input, string keyStart, string keyEnd, ref string result)
		{
			int start = (string.IsNullOrEmpty(keyStart) ? 0 : input.IndexOf(keyStart));
			if (start < 0)
				return false;
			start += (string.IsNullOrEmpty(keyStart) ? 0 : keyStart.Length);
			int end = (string.IsNullOrEmpty(keyEnd) ? input.Length : input.IndexOf(keyEnd, start));
			if (end < 0)
				return false;
			result = input.Substring(start, end - start);
			result = result.Replace("`", "'");
			result = result.Replace("+", " ");
			return true;
		}

		// return a list of all the files matching the source wildcards in a folder
		private static IEnumerable<string> Search(string root, string searchPattern, bool recurse = true)
		{
			SearchOption searchOption = (recurse ? SearchOption.AllDirectories : SearchOption.TopDirectoryOnly);
			return Directory.GetFiles(root, searchPattern, searchOption);
		}

		public static void LookupPlayerIDs()
		{
			try
			{
				const string queryFormat = @"
					SELECT DISTINCT m.lahmanID
					FROM bb_Master AS m
					LEFT OUTER JOIN bb_Appearances AS a ON a.playerID = m.playerID
					LEFT OUTER JOIN bb_Managers AS man ON man.managerID = m.managerID
					GROUP BY m.lahmanID, m.playerID, m.managerID, m.debut, m.finalGame, m.nameLast, m.nameFirst, m.nameNick, m.nameGiven
					HAVING
					(m.nameLast = '{1}')
						AND
					(
						(CHARINDEX(m.nameFirst, '{0}') > 0)
							OR
						(CHARINDEX('{0}', m.nameFirst) > 0)
							OR
						(CHARINDEX('{0}', m.nameNick) > 0)
							OR
						(CHARINDEX('{0}', m.nameGiven) > 0)
					)
						AND
					(
						((m.managerID IS NOT NULL) AND ({2} BETWEEN MIN(man.yearID) - 5 AND MAX(man.yearID) + 5))
							OR
						((m.playerID IS NOT NULL) AND ({2} BETWEEN YEAR(m.debut) - 5 AND YEAR(m.finalGame) + 5))
					)
					";

				// Create a connection to the data source
				const string connectionString = "Provider=SQLOLEDB;Data Source=tcp:sql2k801.discountasp.net;Initial Catalog=SQL2008_539371_facetoface;User ID=SQL2008_539371_facetoface_user;Password=7202jimm;";
				using (OleDbConnection connection = new OleDbConnection(connectionString))
				{
					connection.Open();
					if (connection.State != ConnectionState.Open)
						return; // ErrorResult("No open connection");

					const string csvPathIn = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\ScrapedData.csv";
					FileStream csvStreamIn = new FileStream(csvPathIn, FileMode.Open);
					StreamReader csvIn = new StreamReader(csvStreamIn);
					const string csvPathOut = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\ScrapedDataWithLahmanID.csv";
					FileStream csvStreamOut = new FileStream(csvPathOut, FileMode.Create);
					StreamWriter csvOut = new StreamWriter(csvStreamOut);

					string line = null;
					while (!csvIn.EndOfStream)
					{
						if (line != null)
							csvOut.WriteLine(line);

						line = csvIn.ReadLine();

						try
						{
							Item item = Item.ItemFactory(line);
							if (!string.IsNullOrWhiteSpace(item.rejected))
								continue;
							if (!string.IsNullOrWhiteSpace(item.lahmanID))
								continue;

							string first = item.nameFirst.Replace("'", "''");
							string last = item.nameLast.Replace("'", "''");
							string query = string.Format(queryFormat, first, last, item.yearID);
							using (OleDbDataAdapter dataAdapter = new OleDbDataAdapter(query, connection))
							{
								// "Root" is the root element name
								using (DataSet dataSet = new DataSet("Root"))
								{
									// "Row" is the name of each row in the set
									int rowCount = dataAdapter.Fill(dataSet, "Row");
									if (rowCount > 1)
										continue;
									if (dataSet.Tables.Count > 0)
									{
										// Indicate we want columns mapped as Xml attributes instead of elements
										foreach (DataColumn column in dataSet.Tables[0].Columns)
											column.ColumnMapping = MappingType.Attribute;
									}

									// Convert the DataSet into an XElement
									XElement root = XElement.Parse(dataSet.GetXml(), LoadOptions.None);
									Collection<XElement> elements = root.Descendants().ToCollection();
									if (elements.Count > 0)
									{
										string lahmanID = elements[0].Attribute("lahmanID").Value.ToString();
										if (!string.IsNullOrWhiteSpace(lahmanID))
											line += "," + lahmanID;
									}
								}
							}
						}
						catch (Exception ex)
						{
							ex.GetType();
						}
					}

					if (line != null)
						csvOut.WriteLine(line);

					csvIn.Dispose();
					csvOut.Dispose();
					csvStreamIn.Dispose();
					csvStreamOut.Dispose();
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public static void MoveAndRenameFiles()
		{
			const string fileIDPrefix = "CF";
			const string vendor = "BCCM";
			const string root = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\" + vendor;
			IEnumerable<string> oldFilePaths = Search(root, "*.jpg", true);
			foreach (string oldFilePath in oldFilePaths)
			{
				int IdNumber = Convert.ToInt32(Path.GetFileNameWithoutExtension(oldFilePath));
				string newFilePath = root + IdToFileId(fileIDPrefix, IdNumber) + ".jpg";
				//CreateFolder(newFilePath);
				File.Move(oldFilePath, newFilePath);
			}
		}

		public static void CsvReformatter()
		{
			const string csvPathIn = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\ScrapedData.csv";
			FileStream csvStreamIn = new FileStream(csvPathIn, FileMode.Open);
			StreamReader csvIn = new StreamReader(csvStreamIn);
			const string csvPathOut = "C:\\My Projects\\Pivot\\Pivot.Web\\Cards\\All\\ScrapedDataNew.csv";
			FileStream csvStreamOut = new FileStream(csvPathOut, FileMode.Create);
			StreamWriter csvOut = new StreamWriter(csvStreamOut);

			while (!csvIn.EndOfStream)
			{
				string line = csvIn.ReadLine();
				int start = line.IndexOf('#');
				if (start >= 0)
				{
					start = line.IndexOf(',', start);
					if (start >= 0)
					{
						start++;
						int end = line.IndexOf(".jpg", start);
						if (end >= 0)
						{
							string id = line.Substring(start + "CF".Length, end - start - "CF".Length);
							//id = (line.StartsWith("BC") ? "CF" : "SF") + id.PadLeft(5, '0');
							line = line.Substring(0, start) + Convert.ToInt32(id) + line.Substring(end + ".jpg".Length);
						}
					}
				}
				csvOut.WriteLine(line);
			}

			csvIn.Dispose();
			csvOut.Dispose();
			csvStreamIn.Dispose();
			csvStreamOut.Dispose();
		}
	}

	public class Item
	{
		public string yearID { get; set; }
		public string nameLast { get; set; }
		public string nameFirst { get; set; }
		public string rejected { get; set; }
		public string lahmanID { get; set; }

		public static Item ItemFactory(string valueString)
		{
			string[] values = valueString.Split(',');
			return new Item(values[1], values[3], values[4], values[5], (values.Length > 10 ? values[10] : string.Empty));
		}

		public Item(string _yearID, string _nameLast, string _nameFirst, string _rejected, string _lahmanID)
		{
			yearID = _yearID;
			nameLast = _nameLast;
			nameFirst = _nameFirst;
			rejected = _rejected;
			lahmanID = _lahmanID;
		}
	}

	public static class IEnumerableExt
	{
		// Extension for IEnumerable<TT>
		internal static Collection<TT> ToCollection<TT>(this IEnumerable<TT> enumerable)
		{
			Collection<TT> collection = new Collection<TT>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}
	}
}
