using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Xml.Linq;
using ClassLibrary;
using Pivot.Web.DBService;

#if EXPLICIT
		// Instead of creating the object as below...
		DBServiceClient px = new DBServiceClient();

		// It is possible to pass a binding and an endpoint address when creating the service proxy,
		// and dynamically choosing localhost or the web server (not really necessary)
		Uri webServiceUri = new Uri("http://localhost:51698/DBService.svc");
		Uri webServiceUri = new Uri("http://dreamnit.com/DBService.Web/DBService.svc");
		DBServiceClient px = new DBServiceClient(new BasicHttpBinding(), new EndpointAddress(webServiceUri));
#endif

namespace DBServiceQueries
{
	internal abstract class DBServiceBase<TT>
	{
		internal event Action<Collection<TT>, string> Completed;

		internal abstract string SqlQuery { get; }
		internal abstract IEnumerable ProcessResults(XElement element);
		internal abstract bool UseCompression { get; }

		internal void Execute()
		{
			string query = SqlQuery;
			if (query.IsEmpty())
			{
				ExecuteCompleted(this, new DoWorkEventArgs("No sql query"));
				return;
			}

			try
			{
				DBServiceClient px = new DBServiceClient();
				XElement element = null;
				if (UseCompression)
				{
					string result = px.GetDeflate(query);
					string xElement = Compression.Decompress(result, Compressor.Deflate);
					element = XElement.Parse(xElement);
				}
				else
					element = px.Get(query);

				// Note that if there is no element, it is an empty result
				// otherwise, element.Name should be "Root" or null
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
				ForwardResults(null, error);
				return;
			}

			XElement element = e.Result as XElement; // element.Name should be "Root" or null
			if (element == null) // An valid empty result
			{
				ForwardResults(null, null);
				return;
			}

			IEnumerable queryResults = ProcessResults(element);
			ForwardResults(queryResults, null);
		}

		private void ForwardResults(IEnumerable queryResults, string error)
		{
			if (Completed == null)
				return;

			IEnumerable<TT> enumerable = queryResults as IEnumerable<TT>;
			Collection<TT> collection = (enumerable != null ? enumerable.ToCollection<TT>() : null);
			Completed(collection, error);
		}
	}
}
