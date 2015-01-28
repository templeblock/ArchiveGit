using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using ClassLibrary;

// SQL query builder references:
// http://www.codeproject.com/KB/database/QueryDesigner.aspx
// http://www.activequerybuilder.com/javademo.html
#if false
	// SQL query functions:
	//		DISTINCT() COUNT(1), COUNT(DISTINCT column), SUM(), AVG(), MIN(), MAX(), UPPER(), LOWER(), 
	//		MONTH(date), DAY(date) YEAR(date), BETWEEN()
	// SELECT
	// TOP
	// FROM
	// [INNER | [LEFT | FULL] OUTER | CROSS] JOIN
	// WHERE ()
	// DISTINCT eliminates redundant results
	// IN(), NOT IN() used in a WHERE clause to find a value in a set, as in: birthState IN ('MA', 'CA')
	// GROUP BY
	// HAVING - similar to WHERE; only applied after SELECT is performed; mainly used with group functions; used to ELIMINATE rows from the selection
	// ORDER BY (ASC|DESC) example
	// Sub-queries:
	//		SELECT * FROM (SELECT ...)
	//		SELECT * FROM tablename WHERE value > (SELECT statement that returns one value)

	// Sample queries

	// Count the results of a sub-query
	SELECT COUNT(1) AS Count
	FROM
	(
		SELECT DISTINCT birthState
		FROM bb_Master AS m
		WHERE (birthState IN ('MA', 'CA'))
	) AS Dummy
	//2

	// Aggregate: How many players were born in Massachusetts or California
	SELECT birthState AS state, COUNT(1) AS total
	FROM bb_Master AS m
	WHERE(birthState IN ('MA', 'CA'))
	GROUP BY birthState
	//MA	643
	//CA	1977

	// Aggregate: How many states do we have players from?
	SELECT COUNT(DISTINCT birthState) AS total
	FROM bb_Master AS m
	// 79

	// Aggregate: From what states do we have more than 1200 players?
	SELECT birthState, COUNT(1) AS total
	FROM bb_Master AS m
	GROUP BY birthState
	HAVING (COUNT(1) > 1200)
	//PA	1334
	//NULL	1858
	//CA	1977

	// A player's yearly statistics
	SELECT m.playerID, m.nameFirst, m.nameLast, b.yearID, b.AB
	FROM bb_Master AS m 
	INNER JOIN bb_Batting AS b ON m.playerID = b.playerID
	WHERE (b.playerID = 'aaronha01')
	ORDER BY b.yearID ASC

	// A player's total years played, and his career batting average
	SELECT m.playerID, m.nameFirst, m.nameLast, COUNT(DISTINCT b.yearID) AS Years, SUM(b.H) * 1000 / SUM(b.AB) AS 'BA'
	FROM bb_Master AS m
	INNER JOIN bb_Batting AS b ON m.playerID = b.playerID
	WHERE (b.playerID = 'aaronha01')
	GROUP BY m.playerID, m.nameFirst, m.nameLast
	// aaronha01	Hank	Aaron	23	304

	// The list of a player's teams
	SELECT DISTINCT t.name AS team
	FROM bb_Appearances AS a
	INNER JOIN bb_Teams AS t ON a.teamID = t.teamID AND a.yearID = t.yearID
	WHERE (a.playerID = @playerID)
	GROUP BY t.yearID, t.teamID, t.name

	// Best Inline approximation of group_concat
	SELECT playerID,
	(
		SELECT DISTINCT TOP (100) PERCENT teamID + ',' AS [data()]
		FROM bb_Appearances AS a
		WHERE playerID = m.playerID
		ORDER BY yearID, [data()]
		FOR XML PATH('')
	) AS list
	FROM bb_Master AS m
	WHERE (playerID = 'aardsda01')
	GROUP BY playerID

	// Inline approximation of group_concat
	SELECT playerID, LEFT(list, LEN(list) - 1) AS list
	FROM bb_Master AS m CROSS APPLY
	(
		SELECT DISTINCT teamID + ','
		FROM bb_Appearances AS a
		WHERE m.playerID = a.playerID 
		GROUP BY yearID, teamID 
		FOR XML PATH('')
	) pre_trimmed(list)
	WHERE (playerID = 'aardsda01')
	GROUP BY playerID, list

	// UDF approximation of group_concat
	ALTER function bb_PlayerIDToTeams(@playerID varchar(10)) returns varchar(1000) as
	begin
		declare @List varchar(1000)
		select @List = coalesce(@List + ',', '') + team
		from 
		(
			select distinct t.name as team
			from bb_Appearances as a
			inner join bb_Teams as t on a.teamID = t.teamID and a.yearID = t.yearID
			where a.playerID = @playerID
			group by t.yearID, t.Name
		)
		as teams
		return @List
	end

	// The best technique for rolling up sub-queries
	// Here are all a player/manager's games totals
	SELECT m.playerID, m.managerID, man.G AS G_man, a.G_all, a.G_p, a.G_c, a.G_1b, a.G_2b, a.G_3b, a.G_lf, a.G_cf, a.G_rf, a.G_dh
	FROM bb_Master AS m
	LEFT OUTER JOIN
	(
		SELECT DISTINCT MIN(managerID) AS managerID, SUM(G) AS G
		FROM bb_Managers
		GROUP BY managerID
	) AS man ON man.managerID = m.managerID
	LEFT OUTER JOIN
	(
		SELECT DISTINCT MIN(playerID) AS playerID, SUM(G_all) AS G_all, SUM(G_p) AS G_p, SUM(G_c) AS G_c, SUM(G_1b) AS G_1b, SUM(G_2b) AS G_2b,
						SUM(G_3b) AS G_3b, SUM(G_lf) AS G_lf, SUM(G_cf) AS G_cf, SUM(G_rf) AS G_rf, SUM(G_dh) AS G_dh
		FROM bb_Appearances
		GROUP BY playerID
	) AS a ON a.playerID = m.playerID
	LEFT OUTER JOIN
	(
		SELECT DISTINCT t.name as team
		FROM bb_Appearances as a
		INNER JOIN bb_Teams as t on a.teamID = t.teamID and a.yearID = t.yearID
		WHERE a.playerID = @playerID
		GROUP by t.Name
	) AS a1 ON a1.playerID = m.playerID
	ORDER BY G_man DESC

#endif

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

	public class QueryItems : List<QueryItem>
	{
		public bool LoadFromCsv(string path)
		{
			// Load the query definition from a CSV file
			try
			{
				using (FileStream fileStream = new FileStream(path, FileMode.Open, FileAccess.Read))
				{
					ICollection<Dictionary<string, string>> queryItems = CsvReader.FromStream(fileStream);
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
				SELECT DISTINCT TOP ({0}) 
				m.lahmanID AS _ID,
				m.lahmanID AS _0,
				MAX(i.fileID) as _1,
				nameFirst + ' ' + nameLast AS _2,
				'GU/'+nameLast+',+'+nameFirst AS _3, 
				'Bats: '+bats+', Throws: '+throws AS _4,
				dbo.bb_PlayerIDToTeams(m.playerID) AS _7,
				CAST((CASE WHEN m.managerID IS NULL THEN 'no' ELSE 'yes' END) AS varchar(3)) AS _8,
				CAST((CASE WHEN h.inducted = 'Y' THEN 'yes' ELSE 'no' END) AS varchar(3)) AS _9,
				CAST(RTRIM(birthYear * 10000 + birthMonth * 100 + birthDay) AS datetime) AS _10, 
				birthCity + ', ' + birthState + ' ' + birthCountry AS _11,
				CAST(RTRIM(deathYear * 10000 + deathMonth * 100 + deathDay) AS datetime) AS _12, 
				deathCity + ', ' + deathState + ' ' + deathCountry AS _15, 
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
				m.lahmanID AS _5,
				m.playerID AS _6, 
				'Baseball Reference=BR/'+bbrefID+',Baseball Page=TBP/'+m.playerID AS _29
				FROM bb_Master AS m 
				LEFT OUTER JOIN bb_HallOfFame as h ON m.hofID = h.hofID AND h.inducted = 'Y'
				LEFT OUTER JOIN bb_MasterImage as i ON m.lahmanID = i.lahmanID AND CHARINDEX('C', i.fileID) > 0
				WHERE (m.lahmanID >= {1}) AND i.fileID IS NOT NULL
				GROUP BY m.lahmanID, m.nameFirst, m.nameLast, m.bats, m.throws, m.playerID, m.managerID, m.hofID,
					m.birthYear, m.birthMonth, m.birthDay, m.birthCity, m.birthState, m.birthCountry, 
					m.deathYear, m.deathMonth, m.deathDay, m.deathCity, m.deathState, m.deathCountry, 
					m.nameNote, m.nameGiven, m.nameNick, m.weight, m.height, m.debut, m.finalGame, m.college, m.bbrefID, h.inducted
				ORDER BY _ID
			";
			//WHERE (m.lahmanID >= {1}) AND (debut BETWEEN '1900-01-01' AND '1980-01-01')
		}

		public QueryItem FindItemByName(string name)
		{
			foreach (QueryItem item in this)
			{
				if (item.Output && name == item.Alias)
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
			Type = dictionary["Type"].ToType();
			Criteria = dictionary["Criteria"];
			CriteriaOr = dictionary["CriteriaOr"];
			Group = dictionary["Group"].ToBool();
			SortType = dictionary["SortType"].ToType<SortType>();
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
			return string.Format("_A{0}", _arbitraryIndex++);
		}
	}
}