using System;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;
using System.Diagnostics;
using System.Text;

namespace QueryDesigner
{
	internal class DataTableHelper
	{
		// constants
		const string TABLE = "TABLE";
		const string VIEW = "VIEW";
		const string LINK = "LINK";
		const string TABLE_TYPE = "TABLE_TYPE";
		const string TABLE_SCHEMA = "TABLE_SCHEMA";
		const string PROCEDURE_PARAMETERS = "PROCEDURE_PARAMETERS";

		/// <summary>
		/// Gets a select statement that can be used to retrieve data from a given table, 
		/// view, or stored procedure.
		/// </summary>
		/// <param name="table"><see cref="DataTable"/> that specifies the source table, 
		/// view, or stored procedure that contains the data.</param>
		/// <returns>A select statement that can be used to retrieve the data.</returns>
		public static string GetSelectStatement(DataTable table)
		{
			return GetTableType(table) == TableType.Procedure
				? string.Format("EXEC {0}{1}", GetFullTableName(table), GetProcedureParameters(table))
				: string.Format("SELECT * from {0}", GetFullTableName(table));
		}

		/// <summary>
		/// Gets the table's underlying type (Table, View, or Stored Procedure).
		/// </summary>
		/// <param name="table"><see cref="DataTable"/> whose type will be returned.</param>
		/// <returns>The table type.</returns>
		public static TableType GetTableType(DataTable table)
		{
			// use TABLE_TYPE property
			switch (table.ExtendedProperties[TABLE_TYPE] as string)
			{
				case TABLE:
					return TableType.Table;
				case LINK: // ?
				case VIEW:
					return TableType.View;
			}

			// no type, this is a stored procedure
			return TableType.Procedure;
		}

		/// <summary>
		/// Gets the full table name (e.g. 'dbo.[Order Details]).
		/// </summary>
		/// <param name="table"><see cref="DataTable"/> whose name will be returned.</param>
		/// <returns>The full table name, including table schema and brackets when needed.</returns>
		public static string GetFullTableName(DataTable table)
		{
			var sb = new StringBuilder();
			var schemaName = table.ExtendedProperties[TABLE_SCHEMA] as string;
			if (schemaName != null)
				sb.AppendFormat("{0}.", schemaName);

			// append actual table name
			sb.Append(DbHelper.BracketName(table.TableName));
			return sb.ToString();
		}

		/// <summary>
		/// Gets the parameters required by a stored procedure.
		/// </summary>
		/// <param name="table">DataTable that contains the schema for the stored procedure.</param>
		/// <returns>A list of <see cref="OleDbParameter"/> objects.</returns>
		public static List<OleDbParameter> GetTableParameters(DataTable table)
		{
			return table.ExtendedProperties[PROCEDURE_PARAMETERS] as List<OleDbParameter>;
		}

		/// <summary>
		/// Checks whether a given type is numeric.
		/// </summary>
		/// <param name="type"><see cref="Type"/> to check.</param>
		/// <returns>True if the type is numeric; false otherwise.</returns>
		public static bool IsNumeric(Type type)
		{
			// handle regular types
			switch (Type.GetTypeCode(type))
			{
				case TypeCode.Decimal:
				case TypeCode.Double:
				case TypeCode.Single:
				case TypeCode.Byte:
				case TypeCode.Int16:
				case TypeCode.Int32:
				case TypeCode.Int64:
				case TypeCode.SByte:
				case TypeCode.UInt16:
				case TypeCode.UInt32:
				case TypeCode.UInt64:
					return true;
			}

			// handle nullable types
			type = Nullable.GetUnderlyingType(type);
			return type != null && IsNumeric(type);
		}

		/// <summary>
		/// Translates an <see cref="OleDbType"/> into a .NET type.
		/// </summary>
		/// <param name="oleDbType"><see cref="OleDbType"/> to translate.</param>
		/// <returns>The corresponding .NET <see cref="Type"/>.</returns>
		public static Type GetType(OleDbType oleDbType)
		{
			switch ((int)oleDbType)
			{
				case 0:
					return typeof(Nullable);
				case 2:
					return typeof(short);
				case 3:
					return typeof(int);
				case 4:
					return typeof(float);
				case 5:
					return typeof(double);
				case 6:
					return typeof(decimal);
				case 7:
					return typeof(DateTime);
				case 8:
					return typeof(string);
				case 9:
					return typeof(object);
				case 10:
					return typeof(Exception);
				case 11:
					return typeof(bool);
				case 12:
					return typeof(object);
				case 13:
					return typeof(object);
				case 14:
					return typeof(decimal);
				case 16:
					return typeof(sbyte);
				case 17:
					return typeof(byte);
				case 18:
					return typeof(ushort);
				case 19:
					return typeof(uint);
				case 20:
					return typeof(long);
				case 21:
					return typeof(ulong);
				case 64:
					return typeof(DateTime);
				case 72:
					return typeof(Guid);
				case 128:
					return typeof(byte[]);
				case 129:
					return typeof(string);
				case 130:
					return typeof(string);
				case 131:
					return typeof(decimal);
				case 133:
					return typeof(DateTime);
				case 134:
					return typeof(TimeSpan);
				case 135:
					return typeof(DateTime);
				case 138:
					return typeof(object);
				case 139:
					return typeof(decimal);
				case 200:
					return typeof(string);
				case 201:
					return typeof(string);
				case 202:
					return typeof(string);
				case 203:
					return typeof(string);
				case 204:
					return typeof(byte[]);
				case 205:
					return typeof(byte[]);
			}

			Debug.WriteLine("** unknown type: " + oleDbType.ToString());
			return typeof(string);
		}

		/// <summary>
		/// Translates an Access parameter type (in PARAMETERS clause) into an <see cref="OleDbType"/>.
		/// </summary>
		/// <param name="typeName">Access parameter type.</param>
		/// <returns>The corresponding <see cref="OleDbType"/></returns>
		public static OleDbType GetOleDbType(string typeName)
		{
			switch (typeName.ToLower())
			{
				case "Bit":
					return OleDbType.Boolean;
				case "Byte":
					return OleDbType.TinyInt;
				case "Short":
					return OleDbType.SmallInt;
				case "Long":
					return OleDbType.BigInt;
				case "Currency":
					return OleDbType.Currency;
				case "IEEESingle":
					return OleDbType.Single;
				case "IEEEDouble":
					return OleDbType.Double;
				case "DateTime":
					return OleDbType.Date;
				case "Text":
					return OleDbType.VarChar;
				case "Decimal":
					return OleDbType.Decimal;
			}

			Debug.WriteLine("** unknown type: '{0}'", typeName);
			return OleDbType.VarChar;
		}

		// returns a string containing stored procedure parameters
		static string GetProcedureParameters(DataTable table)
		{
			var sb = new StringBuilder();
			var parms = table.ExtendedProperties[PROCEDURE_PARAMETERS] as List<OleDbParameter>;
			if (parms != null)
			{
				foreach (OleDbParameter parm in parms)
				{
					if (sb.Length > 0)
						sb.Append(", ");

					var value = parm.Value.ToString();
					sb.AppendFormat("'{0}'", value.Replace("'", "''"));
				}
			}

			return sb.ToString();
		}
	}
}
