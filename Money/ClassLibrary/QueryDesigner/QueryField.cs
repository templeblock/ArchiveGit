using System;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace QueryDesigner
{
	public enum Sort
	{
		NoSort,
		Ascending,
		Descending
	}

	public enum Aggregate
	{
		GroupBy,
		Sum,
		Avg,
		Min,
		Max,
		Count,
		//Expression,
		//Where,
		SumDistinct,
		AvgDistinct,
		MinDistinct,
		MaxDistinct,
		CountDistinct,
		StDev,
		StDevP,
		Var,
		VarP
	}

	public class QueryField : INotifyPropertyChanged
	{
		// for parsing filter statements
		static Regex _rx1 = new Regex(@"^([^<>=]*)\s*(<|>|=|<>|<=|>=)\s*([^<>=]+)$", RegexOptions.IgnoreCase);
		static Regex _rx2 = new Regex(@"^([^<>=]*)\s*BETWEEN\s+(.+)\s+AND\s+(.+)$", RegexOptions.IgnoreCase);

		public string ColumnName
		{
			get { return _columnName; }
			set
			{
				if (_columnName != value)
				{
					_columnName = value;
					OnPropertyChanged("Column");
				}
			}
		}
		string _columnName;	// column name (or expression)

		public string Alias
		{
			get { return _alias ?? string.Empty; }
			set
			{
				if (_alias != value)
				{
					_alias = value;
					OnPropertyChanged("Alias");
				}
			}
		}
		string _alias;		// alias for this field (optional)

		public string TableName
		{
			get { return _tableName; }
		}
		string _tableName;

		public string GetFullTableName()
		{
			return _fullTableName;
		}
		string _fullTableName;

		public string GetExpression()
		{
			return _expression;
		}
		string _expression;

		public bool Output
		{
			get { return _output; }
			set
			{
				if (_output != value)
				{
					_output = value;
					OnPropertyChanged("Output");
				}
			}
		}
		bool _output;	// include in SELECT clause

		public Aggregate GroupBy
		{
			get { return _groupBy; }
			set
			{
				if (_groupBy != value)
				{
					_groupBy = value;
					OnPropertyChanged("GroupBy");
				}
			}
		}
		Aggregate _groupBy;	// GROUP BY clause

		public Sort Sort
		{
			get { return _sort; }
			set
			{
				if (_sort != value)
				{
					_sort = value;
					OnPropertyChanged("Sort");
				}
			}
		}
		Sort _sort;		// ORDER BY clause

		public string Filter
		{
			get { return _filter ?? string.Empty; }
			set
			{
				if (_filter != value)
				{
					_filter = value;
					OnPropertyChanged("Filter");
				}
			}
		}
		string _filter;	// WHERE clause

		public QueryField(string tableName, string columnName, string fullTableName)
		{
			_tableName = tableName;
			_columnName = columnName;
			_fullTableName = fullTableName;
			_expression = DbHelper.BracketName(_columnName);
			if (fullTableName != null) // a table.column string, not part of a table (e.g. expression)
				_expression = fullTableName + "." + _expression;

			_output = true;
		}

		/// <summary>
		/// Gets a formatted filter expression to be used in the SQL statement.
		/// </summary>
		/// <returns>A formatted filter expression to be used in the SQL statement.</returns>
		public string GetFilterExpression()
		{
			// empty? easy
			string filter = this.Filter.Trim();
			if (filter.Length == 0)
				return string.Empty;

			// get simple expressions
			Match m = _rx1.Match(filter);
			if (m.Success)
			{
				return m.Groups[1].Value.Length == 0
					? string.Format("({0} {1})", this.GetFullName(true), filter) // > x
					: string.Format("({0})", filter); // y > x
			}

			// get 'between' expressions
			m = _rx2.Match(filter);
			if (m.Success)
			{
				return m.Groups[1].Value.Length == 0
					? string.Format("({0} {1})", _expression, filter) // between x and y
					: string.Format("({0})", filter); // z between x and y
			}

			// oops...
			Debug.WriteLine("Warning: failed to parse filter...");
			return string.Format("({0} {1})", this.GetFullName(true), filter);
		}

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void OnPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
				PropertyChanged(this, e);
		}

		void OnPropertyChanged(string propName)
		{
			OnPropertyChanged(new PropertyChangedEventArgs(propName));
		}

		/// <summary>
		/// Gets the full field name, including the parent table name and brackets,
		/// and optionally including a GROUPBY clause.
		/// </summary>
		/// <param name="groupBy">Whether to include a GROUPBY clause.</param>
		/// <returns>The full field name.</returns>
		public string GetFullName(bool groupBy)
		{
			// default handling
			string str = _expression;

			// handle GROUPBY clauses
			if (groupBy)
			{
				string fmt = "{0}";
				switch (GroupBy)
				{
					case Aggregate.Sum:
						fmt = "SUM({0})";
						break;
					case Aggregate.Avg:
						fmt = "AVG({0})";
						break;
					case Aggregate.Min:
						fmt = "MIN({0})";
						break;
					case Aggregate.Max:
						fmt = "MAX({0})";
						break;
					case Aggregate.Count:
						fmt = "COUNT({0})";
						break;
					case Aggregate.StDev:
						fmt = "STDEV({0})";
						break;
					case Aggregate.StDevP:
						fmt = "STDEVP({0})";
						break;
					case Aggregate.Var:
						fmt = "VAR({0})";
						break;
					case Aggregate.VarP:
						fmt = "VARP({0})";
						break;
					case Aggregate.SumDistinct:
						fmt = "SUM(DISTINCT {0})";
						break;
					case Aggregate.AvgDistinct:
						fmt = "AVG(DISTINCT {0})";
						break;
					case Aggregate.MinDistinct:
						fmt = "MIN(DISTINCT {0})";
						break;
					case Aggregate.MaxDistinct:
						fmt = "MAX(DISTINCT {0})";
						break;
					case Aggregate.CountDistinct:
						fmt = "COUNT(DISTINCT {0})";
						break;
				}
				str = string.Format(fmt, str);
			}

			// done
			return str;
		}
	}

	/// <summary>
	/// Represents a bindable collection of <see cref="QueryField"/> objects.
	/// </summary>
	public class QueryFieldCollection : ObservableCollection<QueryField>
	{
		/// <summary>
		/// Overridden to perform validation at list level.
		/// </summary>
		/// <param name="e"><see cref="ListChangedEventArgs"/> that contains the event data.</param>
		protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
		{
			// fix fields when they change
			if (true) //j (e.ListChangedType == ListChangedType.ItemChanged)
			{
				QueryField f = null; //j this[e.NewIndex];
				switch ("Alias") //j (e.PropertyDescriptor.Name)
				{
					// prevent duplicate aliases
					case "Alias":
					{
						foreach (QueryField field in this)
						{
							if (field != f && f.Alias == field.Alias)
							{
								f.Alias = CreateUniqueAlias(f);
								break;
							}
						}
						break;
					}

					// if GroupBy is an aggregate, the field needs an alias
					case "GroupBy":
					{
						if (f.GroupBy != Aggregate.GroupBy && string.IsNullOrEmpty(f.Alias))
							f.Alias = CreateUniqueAlias(f);
						break;
					}
				}
			}

			// raise event as usual
			base.OnCollectionChanged(e);
		}

		// creates a unique alias for a field
		private string CreateUniqueAlias(QueryField f)
		{
			for (int i = 1; true; i++)
			{
				// try Expr1, Expr2, etc...
				string alias = string.Format("Expr{0}", i);

				// check if this one exists
				bool duplicate = false;
				foreach (QueryField field in this)
				{
					if (field != f && string.Compare(alias, field.Alias, StringComparison.CurrentCultureIgnoreCase) == 0)
					{
						duplicate = true;
						break;
					}
				}

				// doesn't exist? we're done here
				if (!duplicate)
					return alias;
			}
		}
	}
}
