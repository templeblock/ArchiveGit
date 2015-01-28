using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;
using Pivot.DBService;

#if EXPLICIT
		// Instead of creating the object as below...
		DBServiceClient px = new DBServiceClient();

		// It is possible to pass a binding and an endpoint address when creating the service proxy,
		// and dynamically choosing localhost or the web server (not really necessary)
		Uri webServiceUri = new Uri("http://localhost:51698/DBService.svc");
		Uri webServiceUri = new Uri("http://dreamnit.com/Money.Web/Services/DBService.svc");
		DBServiceClient px = new DBServiceClient(new BasicHttpBinding(), new EndpointAddress(webServiceUri));
#endif

namespace Pivot
{
	// Simple implementation of the abstract class that returns the number of rows resulting from the query
	internal class CountResultsSqlQuery : DBServiceBase<int>
	{
		private string _query;

		internal CountResultsSqlQuery(string query)
		{
			_query = query;
		}

		internal override string SqlQuery()
		{
			return string.Format("SELECT COUNT(*) AS Count FROM ({0}) AS [Dummy]", _query);
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<int> query = from item in element.Descendants()
				  select item.AttributeValue("Count").ToInt();

			return query;
		}
	}

	internal abstract class DBServiceBase<TT>
	{
		internal event Action<Collection<TT>, string> Result;

		internal abstract string SqlQuery();
		internal abstract IEnumerable LinqQuery(XElement element);

		internal void Execute()
		{
			string query = SqlQuery();
			if (query.IsEmpty())
			{
				ExecuteCompleted(this, new DoWorkEventArgs("No sql query"));
				return;
			}

			try
			{
				DBServiceClient px = new DBServiceClient();
				px.GetCompleted += OnGetCompleted;
				px.GetAsync(query);
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

				// Note that if there is no element, it is an empty result
				// otherwise, element.Name should be "Root" or null
				XElement element = e.Result;
				if (element != null && element.Name == "Error")
				{
					string error = "Error accessing the web service" + Environment.NewLine + Environment.NewLine + element.Value;
					ExecuteCompleted(this, new DoWorkEventArgs(error));
					return;
				}

				ExecuteCompleted(this, new DoWorkEventArgs(null) { Result = element });
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
			// e.Cancel = cancelled
			if (e.Argument != null)
			{
				string error = e.Argument as string;
				error.Trace();
				LinqQueryToResult(null, error);
				return;
			}

			XElement element = e.Result as XElement; // element.Name should be "Root" or null
			if (element == null) // An valid empty result
			{
				LinqQueryToResult(null, null);
				return;
			}

			IEnumerable query = LinqQuery(element);
			LinqQueryToResult(query, null);
		}

		private void LinqQueryToResult(IEnumerable query, string error)
		{
			if (Result == null)
				return;

			IEnumerable<TT> enumerable = query as IEnumerable<TT>;
			Collection<TT> collection = (enumerable != null ? enumerable.ToCollection<TT>() : null);
			Result(collection, error);
		}
	}
}
