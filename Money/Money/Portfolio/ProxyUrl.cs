using System;
using System.Windows.Browser;

namespace StockVisualization
{
	public static class ProxyUrl
	{
		private const string _proxyUrl = "http://ideateamspirit.appspot.com/proxy?url={0}";

		public static string Get(string url)
		{
			return String.Format(_proxyUrl, HttpUtility.UrlEncode(url));
		}
	}
}
