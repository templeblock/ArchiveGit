using System;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;
using System.IO;
using System.ServiceModel;
using System.ServiceModel.Activation;
using System.Threading;
using System.Web;
using System.Xml.Linq;
using ClassLibrary;
using QueryDesigner;

namespace DBService
{
	[ServiceContract]
	//[SilverlightFaultBehavior]

	// Related to web.config element: <serviceHostingEnvironment aspNetCompatibilityEnabled="true" />
	// This is needed since this service is only supported with HTTP protocol
	[AspNetCompatibilityRequirements(RequirementsMode = AspNetCompatibilityRequirementsMode.Allowed)]

	// The XmlSerializerFormat attribute on the service class tells WCF to use the 
	// XmlSerializer instead of the default DataContractSerializer.
	// This maintains consistency and compatibility with XML data types
	[XmlSerializerFormat]
	public class DBService
	{
		public DBService()
		{
			Thread.CurrentPrincipal = HttpContext.Current.User;
		}

		//[PrincipalPermission(SecurityAction.Demand, Role = "Administrator")]
		[OperationContract]
		public string GetDeflate(string query)
		{
			try
			{
				XElement element = Get(query);
				return Compression.Compress(element.ToString());
			}
			catch (Exception ex)
			{
				return Compression.Compress(ErrorResult(ex.Message).ToString()); //j + Environment.NewLine + Environment.NewLine + e.StackTrace);
			}
		}

		//[PrincipalPermission(SecurityAction.Demand, Role = "Administrator")]
		[OperationContract]
		public string GetGZip(string query)
		{
			try
			{
				XElement element = Get(query);
				return Compression.Compress(element.ToString(), Compressor.GZip);
			}
			catch (Exception ex)
			{
				return Compression.Compress(ErrorResult(ex.Message).ToString(), Compressor.GZip); //j + Environment.NewLine + Environment.NewLine + e.StackTrace);
			}
		}

		//[PrincipalPermission(SecurityAction.Demand, Role = "Administrator")]
		[OperationContract]
		public XElement Get(string query)
		{
			try
			{
				//if (!HttpContext.Current.User.Identity.IsAuthenticated)
				//    return ErrorResult("Please sign in first");

				var serviceConnectionString = ConfigurationManager.ConnectionStrings["DBService"];
				if (serviceConnectionString == null)
					return ErrorResult("No 'DBService' connection string");
				string connectionString = serviceConnectionString.ConnectionString;

				// Create a connection to the data source
				using (SqlConnection connection = new SqlConnection(connectionString))
				{
					connection.Open();
					if (connection.State != ConnectionState.Open)
						return ErrorResult("No open connection");

					using (SqlDataAdapter dataAdapter = new SqlDataAdapter(query, connection))
					{
						// "Root" is the root element name
						using (DataSet dataSet = new DataSet("Root"))
						{
							// "Row" is the name of each row in the set
							int rowCount = dataAdapter.Fill(dataSet, "Row");
							if (dataSet.Tables.Count > 0)
							{
								// Indicate we want columns mapped as Xml attributes instead of elements
								//j	foreach (DataColumn column in dataSet.Tables[0].Columns)
								foreach (DataTable table in dataSet.Tables)
									foreach (DataColumn column in table.Columns)
										column.ColumnMapping = MappingType.Attribute;
							}

							// Convert the DataSet into an XElement
#if true
							XElement element = XElement.Parse(dataSet.GetXml(), LoadOptions.None);
#else
							XElement element = null;
							XmlDataDocument xmlDataDocument = new XmlDataDocument(dataSet);
							using (XmlNodeReader reader = new XmlNodeReader(xmlDataDocument))
								element = element.Load(reader, LoadOptions.None);
#endif
							return element;
							//return ErrorResult("Test");
						}
					}
				}
			}
			catch (Exception ex)
			{
				return ErrorResult(ex.Message); //j + Environment.NewLine + Environment.NewLine + e.StackTrace);
			}
		}

		[OperationContract]
		public string GetSchema()
		{
			try
			{
				var serviceConnectionString = ConfigurationManager.ConnectionStrings["OleDBService"];
				if (serviceConnectionString == null)
					return ErrorResult("No 'DBService' connection string").ToString();

				string connectionString = serviceConnectionString.ConnectionString;
				using (SchemaDataSet dataSet = SchemaDataSet.Create(connectionString))
				{
					if (dataSet.Tables.Count > 0)
					{
						// Indicate we want columns mapped as Xml attributes instead of elements
						foreach (DataTable table in dataSet.Tables)
							foreach (DataColumn column in table.Columns)
								column.ColumnMapping = MappingType.Attribute;
					}

					// Convert the DataSet into an XElement
					StringWriter text = new StringWriter();
					dataSet.WriteXmlSchema(text);
					return text.ToString().Substring(0, 8000);
					//XElement element = XElement.Parse(dataSet.GetXmlSchema(), LoadOptions.None);
					//return element.ToString();
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			return null;
		}

		private XElement ErrorResult(string error)
		{
			string xmlText = "<Error>DBService: " + error + "</Error>";
			XElement element = XElement.Parse(xmlText);
			return element;
		}
	}
}
