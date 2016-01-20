using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Xml.Linq;
using ClassLibrary;
using TrumpITReporting.DBService;

#if EXPLICIT
		// Instead of creating the object as below...
		DBServiceClient px = new DBServiceClient();

		// It is possible to pass a binding and an endpoint address when creating the service proxy,
		// and dynamically choosing localhost or the web server (not really necessary)
		Uri webServiceUri = new Uri("http://localhost:55823/DBService.svc");
		Uri webServiceUri = new Uri("http://dreamnit.com/TrumpITReporting.Web/Services/DBService.svc");
		DBServiceClient px = new DBServiceClient(new BasicHttpBinding(), new EndpointAddress(webServiceUri));
#endif

namespace TrumpITReporting
{
	// Usage class example
/*
	internal class Location1SqlQuery : DBServiceBase<Location>
	{
		private string _postalCode;

		internal Location1SqlQuery(string postalCode)
		{
			_postalCode = postalCode;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT _FM_zips.City
				FROM _FM_zips 
				WHERE (_FM_zips.ZipCode = '{0}');", _postalCode);
		}

		internal override IEnumerable LinqQuery(XDocument xmlDoc)
		{
			IEnumerable<Location> query = from item in xmlDoc.Root.Descendants()
			select new Location
			{
				City = item.AttributeValue("City"),
			};

			return query;
		}
	}
*/
	internal abstract class DBServiceBase<TT>
	{
		internal event Action<Collection<TT>> Result;

		internal abstract string SqlQuery();
		internal abstract IEnumerable LinqQuery(XDocument xmlDoc);

		internal void Execute(string connectionStringName)
		{
			try
			{
				string query = SqlQuery();
				if (query.IsEmpty())
				{
					ExecuteCompleted(this, new DoWorkEventArgs("No sql query"));
					return;
				}

				DBServiceClient px = new DBServiceClient();
				px.GetCompleted += OnGetCompleted;
				px.GetAsync(query, connectionStringName);
			}
			catch (Exception ex)
			{
				ExecuteCompleted(this, new DoWorkEventArgs(ex.Details()));
			}
		}

		private void OnGetCompleted(object sender, GetCompletedEventArgs e)
		{
			try
			{
				string message = null;
				if (e.Error != null || e.Cancelled)
				{
					message = (e.Cancelled ? "Cancelled" : e.Error.Details());
					ExecuteCompleted(this, new DoWorkEventArgs(message) { Cancel = e.Cancelled });
					return;
				}

				XElement xmlElement = e.Result;
				XDeclaration xmlDeclaration = new XDeclaration("1.0", "UTF-8", "yes");
				XDocument xmlDoc = new XDocument(xmlDeclaration, xmlElement); // xmlDoc.Root.Name should be "Root" or null
				// Note that if there is no root, the XML document is empty
				if (xmlDoc.Root != null && xmlDoc.Root.Name == "Error")
				{
					message = "Error accessing the web service" + Environment.NewLine + Environment.NewLine + xmlDoc.Root.Value;
					ExecuteCompleted(this, new DoWorkEventArgs(message));
					return;
				}

				ExecuteCompleted(this, new DoWorkEventArgs(null) { Result = xmlDoc });
			}
			catch (Exception ex)
			{
				ExecuteCompleted(this, new DoWorkEventArgs(ex.Details()));
			}
		}

		private void ExecuteCompleted(object sender, DoWorkEventArgs e)
		{
			// e.Result is successful result object
			// e.Argument is an error string
			// e.Cencel = cancelled
			if (e.Argument != null)
			{
				string message = e.Argument as string;
				MessageBoxEx.ShowError("Web access", message, null);
				LinqQueryToResult(null);
				return;
			}

			XDocument xmlDoc = e.Result as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc == null || xmlDoc.Root == null) // An valid empty result
			{
				LinqQueryToResult(null);
				return;
			}

			IEnumerable query = LinqQuery(xmlDoc);
			LinqQueryToResult(query);
		}

		private void LinqQueryToResult(IEnumerable query)
		{
			if (Result == null)
				return;

			IEnumerable<TT> enumerable = query as IEnumerable<TT>;
			Collection<TT> collection = (enumerable != null ? enumerable.ToCollection<TT>() : null);
			Result(collection);
		}
	}
}
