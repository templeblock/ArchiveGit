using System;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Xml;
using ClassLibrary; // for exception.Details()
using Money.XigniteHistoricalService;

namespace Money
{
	// Web service reference: XigniteHistoricalService, http://www.xignite.com/xHistorical.asmx?WSDL
#if false //SAMPLE_USAGE
	internal void TestingXigniteServiceHelper()
	{
		XigniteServiceHelper xignite = new XigniteServiceHelper();
		xignite.GetHistoricalQuote("MSFT", "01/13/2009", GetHistoricalQuoteCompleted);
	}

	private void GetHistoricalQuoteCompleted(object sender, DoWorkEventArgs e)
	{
		// e.Result is successful result object
		// e.Argument is an error string
		// e.Cencel = cancelled
		if (e.Argument != null)
		{
			string message = e.Argument as string;
			MessageBoxEx.ShowOK("Error", message, null);
			return;
		}

		double price = (double)e.Result;
	}
#endif

	internal class XigniteServiceHelper
	{
		private const string k_XigniteRegisteredEmailAddress = "jmccurdy@facetofacesoftware.com";
		private readonly XigniteHistoricalSoapClient m_Proxy = new XigniteHistoricalSoapClient();
		private readonly MessageHeader m_MessageHeader = MessageHeader.CreateHeader(
			"Header", "http://www.xignite.com/services/",
			"<Username>" + k_XigniteRegisteredEmailAddress + "</Username>", new RawObjectSerializer());

		internal void GetHistoricalQuote(string symbol, string date, DoWorkEventHandler requestCompleteHandler)
		{
			try
			{
				if (requestCompleteHandler == null)
					return;

				using (OperationContextScope ocs = new OperationContextScope(m_Proxy.InnerChannel))
				{
					m_Proxy.GetHistoricalQuoteCompleted += GetHistoricalQuoteCompleted;
					OperationContext.Current.OutgoingMessageHeaders.Add(m_MessageHeader);
					m_Proxy.GetHistoricalQuoteAsync(symbol, IdentifierTypes.Symbol, date, requestCompleteHandler);
				}
			}
			catch (Exception ex)
			{
				requestCompleteHandler(this, new DoWorkEventArgs(ex.Details()));
			}
		}

		private void GetHistoricalQuoteCompleted(object sender, GetHistoricalQuoteCompletedEventArgs e)
		{
			DoWorkEventHandler requestCompleteHandler = e.UserState as DoWorkEventHandler;
			try
			{
				string message = null;
				if (e.Error != null || e.Cancelled)
				{
					message = (e.Cancelled ? "Cancelled" : e.Error.Details());
					requestCompleteHandler(this, new DoWorkEventArgs(message) { Cancel = e.Cancelled });
					return;
				}

				HistoricalQuote objHistoricalQuote = e.Result;
				if (objHistoricalQuote == null) // could be caused by HTTP error (404,500...)
					message = "Service is unavailable at this time.";
				else
				if (objHistoricalQuote.Outcome == OutcomeTypes.RegistrationError)
					message = "Our subscription to this service has expired.";
				else
				if (objHistoricalQuote.Outcome == OutcomeTypes.RequestError)
					message = objHistoricalQuote.Message;
				else
				if (objHistoricalQuote.Outcome == OutcomeTypes.SystemError)
					message = "Service is unavailable at this time.";

				if (message != null)
				{
					requestCompleteHandler(this, new DoWorkEventArgs(message));
					return;
				}

				double price = objHistoricalQuote.Last;
				requestCompleteHandler(this, new DoWorkEventArgs(null) { Result = price });
			}
			catch (Exception ex)
			{
				requestCompleteHandler(this, new DoWorkEventArgs(ex.Details()));
			}
		}
	}

	internal class RawObjectSerializer : XmlObjectSerializer
	{
		public override void WriteObjectContent(XmlDictionaryWriter writer, object graph)
		{
			writer.WriteRaw(graph.ToString());
		}

		public override void WriteStartObject(XmlDictionaryWriter writer, object graph) { }
		public override void WriteEndObject(XmlDictionaryWriter writer) { }
		public override bool IsStartObject(XmlDictionaryReader reader) { return false; }
		public override object ReadObject(XmlDictionaryReader reader, bool verifyObjectName) { return null; }
	}
}
