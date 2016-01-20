using System;
using System.ServiceModel;
using System.ServiceModel.Activation;
using System.Runtime.Serialization;
using System.Collections.Generic;
using System.Data;
using System.Data.Odbc;
using System.Data.OleDb;
using System.Xml;
using System.Configuration;
using System.IO;

namespace PicaPagesWebServices
{
	// NOTE: If you change the class name "DBService", you must also update the reference in Web.config and in the associated .svc file.
	public class DBService : IDBService
	{
		public XmlDataDocument GetDataSet(string query)
		{
			return ErrorResult("Sorry - Old call");
			try
			{
				// Create a connection to the data source
				using (OdbcConnection connection = new OdbcConnection(GetConnectionString()))
				{
					if (connection == null)
						return ErrorResult("No connection");
					connection.Open();
					if (connection.State != ConnectionState.Open)
						return ErrorResult("No open connection");

					using (OdbcDataAdapter dataAdapter = new OdbcDataAdapter(query, connection))
					{
						if (dataAdapter == null)
							return ErrorResult("No data adapter");

						// "Root" is the root element name
						using (DataSet dataSet = new DataSet("Root"))
						{
							// "Row" is the name of each row in the set
							int rowCount = dataAdapter.Fill(dataSet, "Row");
							if (dataSet.Tables.Count <= 0)
								return ErrorResult("No dataset returned");
							// Indicate we want columns mapped as Xml attributes instead of elements
							foreach (DataColumn column in dataSet.Tables[0].Columns)
								column.ColumnMapping = MappingType.Attribute;

							connection.Close();
							connection.Dispose();
							dataAdapter.Dispose();
							return new XmlDataDocument(dataSet);
						}
					}
				}
			}
			catch (Exception e)
			{
				return ErrorResult(e.Message); //j + "\r\n\r\n" + e.StackTrace);
			}
		}

//j NEW
		public XmlDataDocument GetOleDbDataSet(string query)
		{
			try
			{
				// Create a connection to the data source
				using (OleDbConnection connection = new OleDbConnection(GetOleDbConnectionString()))
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
							if (dataSet.Tables.Count <= 0)
								return ErrorResult("No dataset returned");
							// Indicate we want columns mapped as Xml attributes instead of elements
							foreach (DataColumn column in dataSet.Tables[0].Columns)
								column.ColumnMapping = MappingType.Attribute;

							connection.Close();
							connection.Dispose();
							dataAdapter.Dispose();
							return new XmlDataDocument(dataSet);
						}
					}
				}
			}
			catch (Exception e)
			{
				return ErrorResult(e.Message); //j + "\r\n\r\n" + e.StackTrace);
			}
		}

		private string GetConnectionString()
		{
			string sConnection = ConfigurationManager.AppSettings.Get("MyConnectionString");
			//string sProvider = "provider=MSDASQL.1;";
			//string sDriver   = "driver={MySQL ODBC 3.51 Driver};";
			//string sServer   = "server=facetofacesoftwa.ipowermysql.com;";
			//string sDatabase = "database=facetofa_mysql;";
			//string sUid      = "uid=facetofa_jim;";
			//string sPwd      = "pwd=theusual;";
			//sConnection = sProvider + sDriver + sServer + sDatabase + sUid + sPwd;
			return sConnection;
		}

//j NEW
		private string GetOleDbConnectionString()
		{
			string sProvider = "Provider=SQLOLEDB;";
			string sDriver   = "";
			string sServer   = "Data Source=tcp:sql2k801.discountasp.net;";
			string sDatabase = "Initial Catalog=SQL2008_539371_facetoface;";
			string sUid      = "User ID=SQL2008_539371_facetoface_user;";
			string sPwd      = "Password=theusual;";
			string sConnection = sProvider + sDriver + sServer + sDatabase + sUid + sPwd;
			return sConnection;
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
		public List<ComplexType> GetComplexTypes()
		{
			List<ComplexType> complexTypes = new List<ComplexType>();
			complexTypes.Add(GetComplexType(null));
			return complexTypes;
		}

		public ComplexType GetComplexType(string name)
		{
			ComplexType complexType = new ComplexType();
			complexType.CustomerID = 1;
			complexType.EmailAddress = "jmccurdy@facetofacesoftware.com";
			return complexType;
		}
#endif
	}

	// NOTE: If you change the interface name "IDBService", you must also update the reference in Web.config.
	[ServiceContract(Namespace = "urn:SerializationTest")]
	[XmlSerializerFormat]
	public interface IDBService
	{
		[OperationContract]
		XmlDataDocument GetDataSet(string query);
		
		[OperationContract]
		XmlDataDocument GetOleDbDataSet(string query);
		
#if NOTUSED
		[OperationContract]
		ComplexType GetComplexType(string name);

		[OperationContract]
		List<ComplexType> GetComplexTypes();
#endif
	}

	// Crucial document that helped make this WCF sevice work with a shared ISP
	// http://community.discountasp.net/default.aspx?f=24&m=16709&p=1
	public class CustomHostFactory : ServiceHostFactory
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
				baseAddresses = new Uri[1] { new Uri("http://www.dreamnit.com/FutureMoneyWebServices/DBService.svc") };
//j				baseAddresses = new Uri[1] { new Uri("http://facetofacesoftware.com/FutureMoneyWebServices/DBService.svc") };

			return new ServiceHost(serviceType, baseAddresses);
#if NOTUSED
			return new CustomHost(serviceType, baseAddresses);
#endif
		}
	}

#if NOTUSED
	public class CustomHost : ServiceHost
	{
		public CustomHost(Type serviceType, params Uri[] baseAddresses)
			: base(serviceType, baseAddresses)
		{
		}

		protected override void ApplyConfiguration()
		{
			base.ApplyConfiguration();
		}
	}
#endif

#if NOTUSED
	// Use a data contract as illustrated in the sample below to add complex types to service operations
	[DataContract]
	public class ComplexType
	{
		int m_CustomerID;
		string m_sEmailAddress;

		[DataMember]
		public int CustomerID
		{
			get { return m_CustomerID; }
			set { m_CustomerID = value; }
		}

		[DataMember]
		public string EmailAddress
		{
			get { return m_sEmailAddress; }
			set { m_sEmailAddress = value; }
		}
	}
#endif
}
