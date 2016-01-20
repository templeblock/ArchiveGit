using System;
using System.Configuration;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;
using System.Data.Odbc;

/// <summary>
/// Summary description for MySqlConnection.
/// </summary>
public partial class MySqlConnection : Page
{
	protected void Page_Load(object sender, EventArgs e)
	{
		string sConnection = ConfigurationManager.AppSettings.Get("MyConnectionString");
		string sProvider = "provider=MSDASQL.1;";
		string sDriver   = "driver={MySQL ODBC 3.51 Driver};";
		string sServer   = "server=facetofacesoftwa.ipowermysql.com;";
		string sDatabase = "database=facetofa_mysql;";
		string sUid      = "uid=facetofa_jim;";
		string sPwd      = "pwd=7202jimm;";
		sConnection = sProvider + sDriver + sServer + sDatabase + sUid + sPwd;

		// Create a connection to the data source
		OdbcConnection MyConnection = new OdbcConnection(sConnection);
		MyConnection.Open();

		string strMessage = "";
		strMessage += ("Connection Opened!<br/><br/>");
		strMessage += (sConnection);
		strMessage += ("<br/><br/>");

		// Specify our SQL query statement
		string sSqlQuery = "SELECT * FROM users;";

		// 1
		// Create an OdbcCommand object with the SQL statement
		OdbcCommand MyCommand = new OdbcCommand(sSqlQuery, MyConnection);

		// Create an OdbcDataReader object from the command's results
		OdbcDataReader MyDataReader = MyCommand.ExecuteReader();

		// Bind the data to the DataGrid control
		ctlDataGrid.DataSource = MyDataReader;
		ctlDataGrid.DataBind();

		// Close the data reader
		MyDataReader.Close();

		// 2
		OdbcDataReader MyDataReader2 = MyCommand.ExecuteReader();

		// Call Read to access the row data
		while (MyDataReader2.Read())
		{
			string sData = MyDataReader2["Password"].ToString();
			strMessage += ("Password = ");
			strMessage += (sData);
			strMessage += ("<br/>");
		}

		// Close the data reader
		MyDataReader2.Close();

		// 3
		OdbcDataAdapter MyDataAdapter = new OdbcDataAdapter(sSqlQuery, MyConnection);

		// Fill a DataSet with data using the data adapter
		DataSet MyDataset = new DataSet();
		MyDataAdapter.Fill(MyDataset);

		// Create a new DataTable object and assign to it the first table in the Tables collection
		DataTable MyTable = new DataTable();
		MyTable = MyDataset.Tables[0];

		// Find how many rows are in the Rows collection of the new DataTable object
		int nRows = MyTable.Rows.Count;
		strMessage += ("Rows = ");
		strMessage += (nRows);
		strMessage += ("<br/>");
		strMessage += ("<br/>");

		MyConnection.Close();

		strMessage += ("<br/>");
		ctlMessage.Text = strMessage;
	}
}
