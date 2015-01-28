//#define YAHOO
#define SIMPLE
using System;
using System.Linq;
using System.Net;
using System.Security;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using System.Collections.Generic;

namespace ClassLibrary
{
	internal class GeoAddress
	{
#if YAHOO
		// Help at http://developer.yahoo.com/maps/rest/V1/geocode.html
		private bool m_bYahoo = true;
		private const string m_apiURL = "http://local.yahooapis.com/MapsService/V1/geocode?appid=YD-9G7bey8_JXxQP6rxl.fBFGgCdNjoDMACQA--"; // &street=701+First+Ave&city=Sunnyvale&state=CA&zip=&
#else
		// Help at http://www.nearby.org.uk/geonames-webservices.php?like=Update
		private const bool m_bYahoo = false;
		private const string m_apiURL = "http://ws.geonames.org/postalCodeSearch?maxRows=10"; // &maxRows=&postalcode=&country=&
#endif
		private EventHandler m_RequestCompletedHandler;
		private GeoAddress m_geo;
		public string Street		{ get; set; }
		public string City			{ get; set; }
		public string StateCode		{ get; set; }
		public string ZipCode		{ get; set; }
		public string Latitude		{ get; set; }
		public string Longitude		{ get; set; }
		public string Precision		{ get; set; }
		public string Warning		{ get; set; }
		public string ErrorMessage	{ get; set; }

		public GeoAddress()
		{
			Street			= "";
			City			= "";
			StateCode		= "";
			ZipCode			= "";
			Latitude		= "";
			Longitude		= "";
			Precision		= "";
			Warning			= "";
			ErrorMessage	= "";
		}

		internal void Request(string street, string city, string stateCode, string zipCode, EventHandler requestCompletedHandler)
		{
			m_geo = this;
			Street = street;
			City = city;
			StateCode = stateCode;
			ZipCode = zipCode;
			m_RequestCompletedHandler = requestCompletedHandler;

			string url = "";
			if (m_geo.Street.Length > 0)
				url += "&street=" + m_geo.Street;
			if (m_geo.City.Length > 0)
				url += "&city=" + m_geo.City;
			if (m_geo.StateCode.Length == 2)
				url += "&state=" + m_geo.StateCode;
			if (m_geo.ZipCode.Length >= 5)
				url += (m_bYahoo ? "&zip=" : "&postalcode=") + m_geo.ZipCode;
			url = m_apiURL + url;
			url = url.Replace(" ", "+");  // Yahoo example shows + sign instead of spaces.

			WebClient webClient = new WebClient();
			try
			{
#if SIMPLE
				webClient.DownloadStringCompleted += RequestCompleted;
				webClient.DownloadStringAsync(new Uri(url));
#else
				webClient.OpenReadCompleted += RequestComplete;
				webClient.OpenReadAsync(new Uri(url));
#endif
			}
			catch (SecurityException ex)
			{
				// handle synchronous exception
				ex.DebugOutput();
			}
		}

#if SIMPLE
		internal void RequestCompleted(object sender, DownloadStringCompletedEventArgs e)
#else
		internal void RequestCompleted(object sender, OpenReadCompletedEventArgs e)
#endif
		{
			object result = null;
			try
			{ // Wrap the access "results" in order to catch exceptions during an asynchronous web request

				result = e.Result;
			}
			catch (SecurityException ex)
			{
				ex.DebugOutput();
			}
			
			bool bNoResults = (result == null ? true : result.ToString().IsEmpty());
			if (e.Error != null || e.Cancelled || bNoResults)
			{
				if (m_RequestCompletedHandler != null)
				{
					if (e.Cancelled)
						m_geo.ErrorMessage = "Cancelled";
					else
					if (bNoResults)
						m_geo.ErrorMessage = "No results returned";
					else
						m_geo.ErrorMessage = e.Error.Message;
					m_RequestCompletedHandler(m_geo, e);
				}
				return;
			}

			//m_geo.Street
			//m_geo.City
			//m_geo.StateCode
			//m_geo.ZipCode
			m_geo.Latitude = "";
			m_geo.Longitude = "";
			m_geo.Precision = "";
			m_geo.Warning = "";
			m_geo.ErrorMessage = "";
			try
			{
#if SIMPLE
				XDocument xmlDoc = XDocument.Parse(e.Result); // xmlDoc.Root.Name should be "geonames" or null
#else
				XmlReaderSettings settings = new XmlReaderSettings();
				settings.DtdProcessing = DtdProcessing.Ignore;
				settings.IgnoreWhitespace = true;
				settings.IgnoreProcessingInstructions = true;
				settings.IgnoreComments = true;
				Stream stream = e.Result;
				using (XmlReader xmlReader = XmlReader.Create(stream, settings))
				{
					XDocument xmlDoc = XDocument.Load(xmlReader);
				}
#endif
				if (xmlDoc.Root != null) // An non-empty result
				{
					IEnumerable<GeoAddress> query = from item in xmlDoc.Descendants("code")
					where item.Element("countryCode").Value == "US"
					orderby item.Element("countryCode").Value ascending
					select new GeoAddress
					{
						City = item.Element("name").Value,
						StateCode = item.Element("adminCode1").Value,
						ZipCode = item.Element("postalcode").Value,
						Latitude = item.Element("lat").Value,
						Longitude = item.Element("lng").Value,
					};

					int iCount = query.Count();
					m_geo = query.FirstOrDefault();
					//List<GeoAddress> geolist = query.ToList<GeoAddress>();
					//List<GeoAddress> geolist = query.Take(3).ToList<GeoAddress>();
				}   

				m_geo.Warning = Regex.Replace(Warning, @"<(.|\n)*?>", string.Empty);
			}
			catch (WebException ex)
			{
				m_geo.ErrorMessage = ex.Message;
			}

			if (m_RequestCompletedHandler != null)
				m_RequestCompletedHandler(m_geo, e);
		}
	}
}
