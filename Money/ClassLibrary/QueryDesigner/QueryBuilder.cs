using System.Collections.Generic;
using System.Collections.Specialized;
using System.Text;

namespace QueryDesigner
{
	/// <summary>
	/// Specifies options for grouping data.
	/// </summary>
	public enum GroupByExtension
	{
		None,
		Cube,
		Rollup,
		All
	}

	public class Relationship
	{
		public string ParentTable { get; set; }
		public string ParentColumn { get; set; }
		public string ChildTable { get; set; }
		public string ChildColumn { get; set; }

		public Relationship(string p, string pc, string c, string cc)
		{
			ParentTable = p;
			ParentColumn = pc;
			ChildTable = c;
			ChildColumn = cc;
		}
	}

	/// <summary>
	/// Manages a collection of query fields and converts them into SQL statements.
	/// </summary>
	public class QueryBuilder
	{
		public List<Relationship> Relationships
		{
			get { return _relationships; }
			set
			{
				if (_relationships != value)
					_relationships = value;
			}
		}
		List<Relationship> _relationships;

		public QueryFieldCollection QueryFields
		{
			get { return _queryFields; }
		}
		QueryFieldCollection _queryFields = new QueryFieldCollection();

		public bool GroupBy
		{
			get { return _groupBy; }
			set
			{
				if (_groupBy != value)
				{
					_groupBy = value;
					_sql = null;
				}
			}
		}
		bool _groupBy = true;

		public GroupByExtension GroupByExtension
		{
			get { return _groupByExtension; }
			set
			{
				if (_groupByExtension != value)
				{
					_groupByExtension = value;
					_sql = null;
				}
			}
		}
		GroupByExtension _groupByExtension; // cube/rollup/all

		public int Top
		{
			get { return _top; }
			set
			{
				_top = value;
				_sql = null;
			}
		}
		int _top = 100;

		public bool Distinct
		{
			get { return _distinct; }
			set
			{
				_distinct = value;
				_sql = null;
			}
		}
		bool _distinct;

		public string Sql
		{
			get
			{
				if (_sql == null || _sqlIsDirty)
				{
					_sqlIsDirty = false;
					_sql = BuildSqlStatement();
				}
				return _sql;
			}
		}
		string _sql;
		bool _sqlIsDirty;

		public bool MissingJoins // not all tables joined
		{
			get { return _missingJoins; }
		}
		bool _missingJoins;

		internal QueryBuilder()
		{
			_queryFields.CollectionChanged += OnQueryFieldsListChanged;
		}

		// field list has changed, need to re-gen the SQL
		private void OnQueryFieldsListChanged(object sender, NotifyCollectionChangedEventArgs e)
		{
			_sqlIsDirty = true;
		}

		// build the SQL statement from the QueryFields collection.
		private string BuildSqlStatement()
		{
			// sanity
			if (_queryFields.Count == 0)
			{
				_missingJoins = false;
				return string.Empty;
			}

			// prepare to build sql statement
			StringBuilder sb = new StringBuilder();

			// select
			sb.Append("SELECT ");
			if (_distinct)
				sb.Append("DISTINCT ");

			if (_top > 0)
				sb.AppendFormat("TOP {0} ", _top);
	
			sb.Append("\r\n\t");
			sb.Append(BuildSelectClause());

			sb.AppendFormat("\r\nFROM\r\n\t{0}", BuildFromClause());

			if (_groupBy)
			{
				string groupBy = BuildGroupByClause();
				if (groupBy.Length > 0)
					sb.AppendFormat("\r\nGROUP BY\r\n\t{0}", groupBy);

				// having
				string having = BuildWhereClause();
				if (having.Length > 0)
					sb.AppendFormat("\r\nHAVING\r\n\t{0}", having);
			}
			else
			{
				// where
				string where = BuildWhereClause();
				if (where.Length > 0)
					sb.AppendFormat("\r\nWHERE\r\n\t{0}", where);
			}

			// order by
			string orderBy = BuildOrderByClause();
			if (orderBy.Length > 0)
				sb.AppendFormat("\r\nORDER BY\r\n\t{0}", orderBy);

			sb.Append(';');
			return sb.ToString();
		}

		// build the SELECT clause
		string BuildSelectClause()
		{
			StringBuilder sb = new StringBuilder();
			foreach (QueryField field in _queryFields)
			{
				if (field.Output)
				{
					if (sb.Length > 0)
						sb.Append(",\r\n\t");

					// add field expression ("table.column" or "colexpr")
					string item = field.GetFullName(_groupBy);
					sb.Append(item);

					// add alias (use brackets to contain spaces, reserved words, etc)
					if (!string.IsNullOrEmpty(field.Alias))
						sb.AppendFormat(" AS {0}", DbHelper.BracketName(field.Alias));
				}
			}
			return sb.ToString();
		}

		string BuildFromClause()
		{
			// build list of tables in query
			var tables = new List<string>();
			foreach (QueryField field in _queryFields)
			{
				string fullTableName = field.GetFullTableName();
				if (fullTableName != null && !tables.Contains(fullTableName))
					tables.Add(fullTableName);
			}

			// build list of joined tables so each table is related to the next one
			var joinedTables = new List<string>();
			bool done = false;
			while (joinedTables.Count < tables.Count && !done)
			{
				done = true;
				foreach (string table in tables)
				{
					bool inserted = InsertRelatedTable(table, joinedTables);
					if (inserted)
						done = false;
				}
			}

			// build join list
			var joins = new List<string>();
			for (int index = 0; index < joinedTables.Count - 1; index++)
			{
				string join = GetJoinRelationship(joinedTables[index], joinedTables[index + 1]);
				if (join != null)
					joins.Add(join);
			}

			// build from statement
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < joinedTables.Count - 1; i++)
			{
				string table = joinedTables[i];
				if (sb.Length > 0)
					sb.Append("\r\n\t");

				sb.AppendFormat("({0} INNER JOIN", table);
			}

			sb.AppendFormat(" {0}", joinedTables[joinedTables.Count - 1]);

			for (int i = joins.Count - 1; i >= 0; i--)
			{
				string join = joins[i];
				sb.AppendFormat("\r\n\tON {0})", join);
			}

			// not all tables joined? probably not what the user wants...
			_missingJoins = joinedTables.Count < tables.Count;

			// add tables that couldn't be joined
			if (_missingJoins)
			{
				foreach (string table in tables)
				{
					if (!joinedTables.Contains(table))
					{
						sb.AppendFormat(", {0}", table);
						joinedTables.Add(table);
					}
				}
			}

			// done
			return sb.ToString();
		}

		// insert a table into the list in a position such that the table is
		// related to the table before and after it; return true on success
		bool InsertRelatedTable(string table, List<string> tables)
		{
			// skip tables that have already been added
			if (tables.Contains(table))
				return false;

			// insert the first one
			if (tables.Count == 0)
			{
				tables.Add(table);
				return true;
			}

			// look for a good insertion point
			for (int index = 0; index <= tables.Count; index++)
			{
				// related to table before?
				bool before = (index == 0 || GetJoinRelationship(table, tables[index - 1]) != null);

				// related to table after?
				bool after = (index == tables.Count || GetJoinRelationship(table, tables[index]) != null);

				// found a good insertion point, move on
				if (before && after)
				{
					tables.Insert(index, table);
					return true;
				}
			}

			// failed to insert
			return false;
		}

		// get the join relationship between two tables
		string GetJoinRelationship(string table1, string table2)
		{
			if (_relationships == null)
				return null;

			foreach (Relationship relationship in _relationships)
			{
				if ((relationship.ParentTable == table1 && relationship.ChildTable == table2) ||
					(relationship.ParentTable == table2 && relationship.ChildTable == table1))
				{
					return string.Format("{0}.{1} = {2}.{3}",
						relationship.ParentTable, relationship.ParentColumn, relationship.ChildTable, relationship.ChildColumn);
				}
			}
			return null;
		}

		// build the WHERE clause
		string BuildWhereClause()
		{
			StringBuilder sb = new StringBuilder();
			foreach (QueryField field in _queryFields)
			{
				if (field.Filter.Length > 0)
				{
					// parse item
					string item = field.GetFilterExpression();
					if (item.Length > 0)
					{
						if (sb.Length > 0)
							sb.Append(" AND\r\n\t");

						// add item (e.g. 'x > y')
						sb.Append(item);
					}
				}
			}
			return sb.ToString();
		}

		// build the GROUPBY clause
		string BuildGroupByClause()
		{
			StringBuilder sb = new StringBuilder();
			if (_groupBy)
			{
				// GROUPBY fields
				foreach (QueryField field in _queryFields)
				{
					if (field.GroupBy == Aggregate.GroupBy)
					{
						if (sb.Length > 0)
							sb.Append(",\r\n\t");

						// add field expression ("table.column" or "colexpr")
						sb.Append(field.GetExpression());
					}
				}

				// extension
				switch (_groupByExtension)
				{
					case GroupByExtension.All:
						return "ALL " + sb.ToString();

					case GroupByExtension.Cube:
						sb.Append(" WITH CUBE");
						break;

					case GroupByExtension.Rollup:
						sb.Append(" WITH ROLLUP");
						break;
				}
			}

			// done
			return sb.ToString();
		}

		// build the ORDERBY clause
		string BuildOrderByClause()
		{
			StringBuilder sb = new StringBuilder();
			foreach (QueryField field in _queryFields)
			{
				if (field.Sort != Sort.NoSort)
				{
					if (sb.Length > 0)
						sb.Append(",\r\n\t");

					// add ORDER BY expression ("table.column" or "colexpr")
					string item = field.GetFullName(true);
					sb.Append(item);

					// descending
					if (field.Sort == Sort.Descending)
						sb.Append(" DESC");
				}
			}

			return sb.ToString();
		}
	}
}
