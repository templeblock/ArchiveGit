//#define YAHOO
#define SIMPLE
using System;
using System.Net;
using System.Text.RegularExpressions;
using System.Xml;
using System.Threading;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using System.Collections.Generic;
using System.Security;

namespace ClassLibrary
{
	public class GeoAddress
	{
#if YAHOO
		// Help at http://developer.yahoo.com/maps/rest/V1/geocode.html
		private bool m_bYahoo = true;
		private const string m_apiURL = "http://local.yahooapis.com/MapsService/V1/geocode?appid=YD-9G7bey8_JXxQP6rxl.fBFGgCdNjoDMACQA--"; // &street=701+First+Ave&city=Sunnyvale&state=CA&zip=&
#else
		// Help at http://www.nearby.org.uk/geonames-webservices.php?like=Update
		private bool m_bYahoo = false;
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

		public void Request(string street, string city, string stateCode, string zipCode, EventHandler requestCompletedHandler)
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
				webClient.DownloadStringCompleted += RequestComplete;
				webClient.DownloadStringAsync(new Uri(url));
#else
				webClient.OpenReadCompleted += RequestComplete;
				webClient.OpenReadAsync(new Uri(url));
#endif
		    }
			catch (SecurityException)
			{
				// handle synchronous exception
			}
		}

#if SIMPLE
		public void RequestComplete(object sender, DownloadStringCompletedEventArgs args)
#else
		public void RequestComplete(object sender, OpenReadCompletedEventArgs args)
#endif
		{
			object result = null;
			try
			{ // Wrap the access "results" in order to catch exceptions during an asynchronous web request

				result = args.Result;
			}
			catch (SecurityException)
			{
			}
			
			bool bNoResults = (result == null ? true : string.IsNullOrEmpty(result.ToString()));
			if (args.Error != null || args.Cancelled || bNoResults)
			{
				if (m_RequestCompletedHandler != null)
				{
					if (args.Cancelled)
						m_geo.ErrorMessage = "Cancelled";
					else
					if (bNoResults)
						m_geo.ErrorMessage = "No results returned";
					else
						m_geo.ErrorMessage = args.Error.Message;
					m_RequestCompletedHandler.Invoke(m_geo, args);
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
				XDocument xmlDoc = XDocument.Parse(args.Result); // xmlDoc.Root.Name should be "geonames" or null
#else
				XmlReaderSettings settings = new XmlReaderSettings();
				settings.IgnoreWhitespace = true;
				settings.IgnoreProcessingInstructions = true;
				settings.IgnoreComments = true;
				Stream stream = args.Result;
				XmlReader reader = XmlReader.Create(stream, settings);
				XDocument xmlDoc = XDocument.Load(reader);
#endif
				if (xmlDoc.Root != null) // An non-empty result
				{
					var query = from item in xmlDoc.Descendants("code")
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
					m_geo = query.First();
					//List<GeoAddress> geolist = query.ToList<GeoAddress>();
					//List<GeoAddress> geolist = query.Take(3).ToList<GeoAddress>();
				}   

				m_geo.Warning = Regex.Replace(Warning, @"<(.|\n)*?>", string.Empty);
			}
			catch (WebException webException)
			{
				m_geo.ErrorMessage = webException.Message;
			}

			if (m_RequestCompletedHandler != null)
				m_RequestCompletedHandler.Invoke(m_geo, args);
		}
	}
}
