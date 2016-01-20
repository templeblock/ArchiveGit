using System;
using System.Data;
using System.Data.OleDb;
using System.IO;
using System.ServiceModel;
using System.ServiceModel.Activation;
using System.Threading;
using System.Web;
using System.Xml;
using System.Configuration;

namespace TrumpITReporting.Web
{
	// Related to web.config element: <serviceHostingEnvironment aspNetCompatibilityEnabled="true" />
	// This is needed since this service is only supported with HTTP protocol
	[AspNetCompatibilityRequirements(RequirementsMode = AspNetCompatibilityRequirementsMode.Allowed)]

	// The XmlSerializerFormat attribute on the service class tells WCF to use the 
	// XmlSerializer instead of the default DataContractSerializer.
	// This maintains consistency and compatibility with XML data types
	[XmlSerializerFormat]

	// NOTE: If you change the class name "DBService", you must also update the reference in Web.config and in the associated .svc file.
	[ServiceContract(Namespace = "")]
	public class DBService
	{
		public DBService()
		{
			Thread.CurrentPrincipal = HttpContext.Current.User;
		}

		//[PrincipalPermission(SecurityAction.Demand, Role = "Administrator")]
		[OperationContract]
		public XmlDataDocument Get(string query, string connectionStringName)
		{
			try
			{
				//if (!HttpContext.Current.User.Identity.IsAuthenticated)
				//    return ErrorResult("Please sign in first");

				// Create a connection to the data source
				string connect = ConfigurationManager.ConnectionStrings[connectionStringName].ConnectionString;
				using (OleDbConnection connection = new OleDbConnection(connect))
				{
					if (connection == null)
						return ErrorResult("No connection");
					connection.Open();
					if (connection.State != ConnectionState.Open)
						return ErrorResult("No open connection");

					using (OleDbDataAdapter dataAdapter = new OleDbDataAdapter(query, connection))
					{
						if (dataAdapter == null)
							return ErrorResult("No data adapter");

						// "Root" is the root element name
						using (DataSet dataSet = new DataSet("Root"))
						{
							// "Row" is the name of each row in the set
							int rowCount = dataAdapter.Fill(dataSet, "Row");
							if (dataSet.Tables.Count > 0)
							{
								// Indicate we want columns mapped as Xml attributes instead of elements
								foreach (DataColumn column in dataSet.Tables[0].Columns)
									column.ColumnMapping = MappingType.Attribute;
							}

							return new XmlDataDocument(dataSet);
						}
					}
				}
			}
			catch (Exception ex)
			{
				return ErrorResult(ex.Message); //j + Environment.NewLine + Environment.NewLine + e.StackTrace);
			}
		}

		private XmlDataDocument ErrorResult(string error)
		{
			string xmlText = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Error>" + error + "</Error>";
			XmlTextReader xmlReader = new XmlTextReader(new StringReader(xmlText));
			XmlDataDocument xmlDataDoc = new XmlDataDocument();
			xmlDataDoc.Load(xmlReader);
			return xmlDataDoc;
		}

#if NOTUSED
		[OperationContract]
		public List<ComplexType> GetComplexTypes()
		{
			List<ComplexType> complexTypes = new List<ComplexType>();
			complexTypes.Add(GetComplexType(null));
			return complexTypes;
		}

		[OperationContract]
		public ComplexType GetComplexType(string name)
		{
			ComplexType complexType = new ComplexType();
			complexType.CustomerID = 1;
			complexType.EmailAddress = "jmccurdy@facetofacesoftware.com";
			return complexType;
		}
#endif
	}

	// We need a ServiceHost factory to make this WCF sevice work with a shared ISP and multiple addresses/host headers
	public class DBServiceHostFactory : ServiceHostFactory // try DomainServiceHostFactory
	{
		protected override ServiceHost CreateServiceHost(Type serviceType, Uri[] baseAddresses)
		{
			bool bLocalHost = false;
			foreach (Uri uri in baseAddresses)
			{
				if (uri.ToString().Contains("://localhost"))
					bLocalHost = true;
			}

			if (!bLocalHost) // Specify the exact URL of the web service
				baseAddresses = new Uri[]
				{
					new Uri("http://dreamnit.com/TrumpITReporting.Web/Services/DBService.svc"),
				};

			return new ServiceHost(serviceType, baseAddresses);
		}
	}
}
