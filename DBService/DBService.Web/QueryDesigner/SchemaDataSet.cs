using System;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;
using System.Diagnostics;

namespace QueryDesigner
{
	/// <summary>
	/// DataSet that knows how to retrieve a schema (tables, columns, views, 
	/// sprocs, constraints, relations) based on a connection string.
	/// </summary>
	internal class SchemaDataSet : DataSet
	{
		// constants
		const string TABLE = "TABLE";
		const string VIEW = "VIEW";
		const string LINK = "LINK";
		const string PARAMETERS = "PARAMETERS";
		const string TABLE_NAME = "TABLE_NAME";
		const string TABLE_TYPE = "TABLE_TYPE";
		const string TABLE_DEFINITION = "TABLE_DEFINITION";
		const string TABLE_SCHEMA = "TABLE_SCHEMA";
		const string COLUMN_NAME = "COLUMN_NAME";
		const string PK_TABLE_NAME = "PK_TABLE_NAME";
		const string FK_TABLE_NAME = "FK_TABLE_NAME";
		const string PK_COLUMN_NAME = "PK_COLUMN_NAME";
		const string FK_COLUMN_NAME = "FK_COLUMN_NAME";
		const string PROCEDURE_NAME = "PROCEDURE_NAME";
		const string PROCEDURE_SCHEMA = "PROCEDURE_SCHEMA";
		const string PARAMETER_NAME = "PARAMETER_NAME";
		const string PARAMETER_DEFAULT = "PARAMETER_DEFAULT";
		const string PROCEDURE_PARAMETERS = "PROCEDURE_PARAMETERS";
		const string PROCEDURE_DEFINITION = "PROCEDURE_DEFINITION";
		const string DATA_TYPE = "DATA_TYPE";
		const string RETURN_VALUE = "RETURN_VALUE";
		const string TABLE_RETURN_VALUE = "TABLE_RETURN_VALUE";

		private string _connectionString = string.Empty;
		public string ConnectionString
		{
			get { return _connectionString; }
			set
			{
				_connectionString = value;
			}
		}

		public SchemaDataSet(string connectionString)
		{
			_connectionString = connectionString;
		}

		public static SchemaDataSet Create(string connString)
		{
			if (string.IsNullOrEmpty(connString))
				return null;

			try
			{
				SchemaDataSet ds = new SchemaDataSet(connString);
				ds.Initialize();
				return ds;
			}
			catch (Exception ex)
			{
				ex.GetType();
				return null;
			}
		}

		public void Initialize()
		{
			// initialize this DataSet
			Reset();
			EnforceConstraints = false;

			// go get the schema
			using (OleDbConnection connection = new OleDbConnection(_connectionString))
			{
				try
				{
					connection.Open();
					GetTables(connection, "bb_");
					GetRelations(connection);
					GetConstraints(connection);
					GetStoredProcedures(connection);
				}
				catch (Exception ex)
				{
					ex.GetType();
				}
			}
		}

		void GetTables(OleDbConnection connection, string tablePrefix)
		{
			// add tables
			using (DataTable dt = connection.GetOleDbSchemaTable(OleDbSchemaGuid.Tables, null))
			{
				foreach (DataRow dr in dt.Rows)
				{
					// get type (table/view)
					string type = dr[TABLE_TYPE].ToString();
					if (type != TABLE && type != VIEW && type != LINK)
						continue;

					// create table
					string name = dr[TABLE_NAME].ToString();
					if (tablePrefix != null && name.IndexOf(tablePrefix) < 0)
						continue;

					using (DataTable table = new DataTable(name))
					{
						table.ExtendedProperties[TABLE_TYPE] = type;

						// save definition in extended properties
						foreach (DataColumn column in dt.Columns)
						{
							table.ExtendedProperties[column.ColumnName] = dr[column];
						}

						// get table schema
						string select = DataTableHelper.GetSelectStatement(table);
						using (OleDbDataAdapter da = new OleDbDataAdapter(select, connection))
						{
							try
							{
								da.FillSchema(table, SchemaType.Mapped);
							}
							catch (Exception ex)
							{
								Debug.WriteLine(ex.Message);
							}
						}

						// add to collection (with or without schema)
						Tables.Add(table);
					}
				}
			}
		}


		// get relations from schema
		void GetRelations(OleDbConnection connection)
		{
			try
			{
				using (DataTable dt = connection.GetOleDbSchemaTable(OleDbSchemaGuid.Foreign_Keys, null))
				{
					foreach (DataRow dr in dt.Rows)
					{
						// get primary/foreign table and column names
						string pkTableName = (string)dr[PK_TABLE_NAME];
						string fkTableName = (string)dr[FK_TABLE_NAME];
						string pkColumnName = (string)dr[PK_COLUMN_NAME];
						string fkColumnName = (string)dr[FK_COLUMN_NAME];

						// make sure both tables are in our DataSet
						if (!Tables.Contains(pkTableName) || !Tables.Contains(fkTableName))
							continue;

						// make sure tables are different
						if (pkTableName == fkTableName)
							continue;

						// get unique relation name
						string relationName = pkTableName + '_' + fkTableName;
						if (Relations.Contains(relationName))
							relationName += Relations.Count.ToString();

						// add to collection
						DataColumn pkColumn = Tables[pkTableName].Columns[pkColumnName];
						DataColumn fkColumn = Tables[fkTableName].Columns[fkColumnName];
						Relations.Add(relationName, pkColumn, fkColumn, true);
					}
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		// get constraints from schema
		void GetConstraints(OleDbConnection connection)
		{
			using (DataTable dt = connection.GetOleDbSchemaTable(OleDbSchemaGuid.Primary_Keys, null))
			{
				var uniqueTables = new Dictionary<string, string>();
				foreach (DataRow dr in dt.Rows)
				{
					// get primary key info
					string tableName = dr[TABLE_NAME].ToString();
					string columnName = dr[COLUMN_NAME].ToString();

					// make sure this table is in our DataSet
					if (!Tables.Contains(tableName))
						continue;

					// make sure it's unique
					if (uniqueTables.ContainsKey(tableName))
					{
						uniqueTables.Remove(tableName);
						continue;
					}

					// save and move on
					uniqueTables[tableName] = columnName;
				}

				// built unique list, now set up primary key columns
				foreach (string tableName in uniqueTables.Keys)
				{
					try
					{
						// set up column
						string columnName = uniqueTables[tableName];
						DataTable table = Tables[tableName];
						DataColumn pkColumn = table.Columns[columnName];
						if (pkColumn == null)
							continue;

						pkColumn.Unique = true;
						pkColumn.AllowDBNull = false;

						// try setting auto increment
						if (pkColumn.DataType != typeof(string) && pkColumn.DataType != typeof(byte) && pkColumn.DataType != typeof(Guid))
						{
							pkColumn.AutoIncrement = true;
							pkColumn.ReadOnly = true;
						}

						// set primary key on parent table
						Tables[tableName].PrimaryKey = new[] { pkColumn };
					}
					catch (Exception ex)
					{
						ex.GetType();
					}
				}
			}
		}

		void GetStoredProcedures(OleDbConnection connection)
		{
			// add stored procedures
			using (DataTable dt = connection.GetOleDbSchemaTable(OleDbSchemaGuid.Procedures, null))
			{
				foreach (DataRow dr in dt.Rows)
				{
					// get the procedure name, skip system stuff
					string name = dr[PROCEDURE_NAME].ToString();
					if (name.StartsWith("~") || name.StartsWith("dt_", StringComparison.OrdinalIgnoreCase))
						continue;

					string schema = dr[PROCEDURE_SCHEMA].ToString();
					if (schema.Equals("sys"))
						continue;

					// trim number that comes after name in Sql databases
					int pos = name.LastIndexOf(';');
					if (pos >= 0)
					{
						int i;
						if (int.TryParse(name.Substring(pos + 1), out i))
							name = name.Substring(0, pos);
					}

					// create table
					DataTable table = new DataTable(name);

					// get parameters
					List<OleDbParameter> parmList = new List<OleDbParameter>();
					table.ExtendedProperties[PROCEDURE_PARAMETERS] = parmList;
					if (!connection.Provider.Contains("SQLOLEDB"))
						GetAccessParameters(dr, parmList);
					else
					{
						using (DataTable dtParms = connection.GetOleDbSchemaTable(OleDbSchemaGuid.Procedure_Parameters, new object[] { null, null, name, null }))
						{
							bool returnsValue = GetSqlServerParameters(dtParms, parmList);
							if (!returnsValue)
								continue;
						}
					}

					// get table schema
					string select = DataTableHelper.GetSelectStatement(table);
					using (OleDbDataAdapter da = new OleDbDataAdapter(select, connection))
					{
						try
						{
							da.FillSchema(table, SchemaType.Mapped);
						}
						catch (Exception ex)
						{
							Debug.WriteLine(ex.Message);
						}
					}

					// add to collection (with or without schema)
					Tables.Add(table);
				}
			}
		}

		// gets parameters from "PARAMETERS" statement in procedure definition (Access)
		void GetAccessParameters(DataRow dr, List<OleDbParameter> parmList)
		{
			string procDef = dr[PROCEDURE_DEFINITION].ToString();
			if (!procDef.StartsWith(PARAMETERS, StringComparison.OrdinalIgnoreCase))
				return;

			int pos = procDef.IndexOf(';');
			if (pos < 0)
				return;

			string parmDef = procDef.Substring(11, pos - 11);
			foreach (string parm in parmDef.Split(','))
			{
				pos = parm.LastIndexOf(' ');
				if (pos < 0)
					continue;

				OleDbParameter p = new OleDbParameter
				{
				    ParameterName = parm.Substring(0, pos).Trim(),
				    OleDbType = DataTableHelper.GetOleDbType(parm.Substring(pos + 1))
				};
				parmList.Add(p);
			}
		}

		// get parameters from the parameters table (SqlServer)
		bool GetSqlServerParameters(DataTable dtParms, List<OleDbParameter> parmList)
		{
			bool returnsValue = false;
			foreach (DataRow parm in dtParms.Rows)
			{
				// get parameter name
				string name = parm[PARAMETER_NAME].ToString().Substring(1);
				if (name == RETURN_VALUE || name == TABLE_RETURN_VALUE)
				{
					returnsValue = true;
					continue;
				}

				// save parameter
				OleDbParameter p = new OleDbParameter(name, (OleDbType)parm[DATA_TYPE])
				{
				    Value = parm[PARAMETER_DEFAULT] as string ?? string.Empty
				};

				parmList.Add(p);
			}

			return returnsValue;
		}
	}
}
