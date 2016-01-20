//#define EXPLICIT
using System;
using System.ServiceModel;
using System.Xml.Linq;
using PicaPages.DBService;
using System.Text;
using System.Xml;
using System.IO;
using System.Collections.Generic;
using ClassLibrary;
using System.Text.RegularExpressions;

namespace PicaPages
{
	public class DBServiceHelper
	{
		private EventHandler m_RequestCompleteHandler;

		/////////////////////////////////////////////////////////////////////////////
		public void GetDataSet(string query, EventHandler requestCompleteHandler)
		{
			m_RequestCompleteHandler = requestCompleteHandler;

			try
			{
#if EXPLICIT
				// It is not really necessary to pass a binding and an endpoint address when creating the service proxy,
				// however, we want to dynamically choose either localhost or the web server
				Uri webServiceUri = new Uri("http://localhost:51698/DBService.svc");
//j				Uri webServiceUri = new Uri("http://facetofacesoftware.com/FutureMoneyWebServices/DBService.svc");
//j				Uri webServiceUri = new Uri("http://www.dreamnit.com/FutureMoneyWebServices/DBService.svc");
				DBServiceClient px = new DBServiceClient(new BasicHttpBinding(), new EndpointAddress(webServiceUri));
#else
				DBServiceClient px = new DBServiceClient();
#endif
				px.GetOleDbDataSetCompleted += GetDataSetCompleted;
				px.GetOleDbDataSetAsync(query);
			}
			catch (Exception e)
			{
				if (m_RequestCompleteHandler != null)
					m_RequestCompleteHandler.Invoke(e.Message, null);
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		private void GetDataSetCompleted(object sender, GetOleDbDataSetCompletedEventArgs e)
		{
			if (m_RequestCompleteHandler == null)
				return;

			if (e.Error != null || e.Cancelled == true)
			{
				string error = (e.Cancelled ? "Cancelled" : e.Error.Message);
				m_RequestCompleteHandler.Invoke(error, null);
				return;
			}

			try
			{
				XElement xmlElement = e.Result;
				XDeclaration xmlDeclaration = new XDeclaration("1.0", "UTF-8", "yes");
				XDocument xmlDoc = new XDocument(xmlDeclaration, xmlElement); // xmlDoc.Root.Name should be "Root" or null
				// Note that if there is no root, the XML document is empty
				if (xmlDoc.Root != null && xmlDoc.Root.Name == "Error")
				{
					string error = "Error accessing the web service\r\n\r\n" + xmlDoc.Root.Value;
					m_RequestCompleteHandler.Invoke(error, null);
					return;
				}

				m_RequestCompleteHandler.Invoke(xmlDoc, null);
			}
			catch (Exception ex)
			{
				m_RequestCompleteHandler.Invoke(ex.Message, null);
			}
		}
	}
}
