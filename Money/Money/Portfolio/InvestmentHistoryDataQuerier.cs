using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Net;
using System.Text.RegularExpressions;
using System.Threading;

namespace StockVisualization
{
	public class InvestmentHistoryDataQuerier
	{
		public event StockDataEventHandler<IList<InvestmentHistoryData>> OnQueryCompleted;

		private const string _ServiceUrl = "http://finance.google.co.uk/finance/historical?q={0}&startdate={1}&enddate={2}&output=csv";

		public void QueryData(string symbol, DateTime startDate, DateTime endDate)
		{
			WebClient historicWebClient = new WebClient();
			historicWebClient.OpenReadCompleted += OnOpenReadCompleted;
			Thread.CurrentThread.CurrentCulture = new CultureInfo("en-US");
			var url = String.Format(_ServiceUrl,
				symbol,
				startDate.ToString("MMM+dd\\%2C+yyyy"),
				endDate.ToString("MMM+dd\\%2C+yyyy")
				);
			var proxiedUrl = ProxyUrl.Get(url);
			historicWebClient.BaseAddress = proxiedUrl;
			historicWebClient.OpenReadAsync(new Uri(proxiedUrl, UriKind.Absolute));
		}

		void OnOpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
		{
			if (e.Error != null)
				return;

			var webClient = sender as WebClient;
			if (webClient == null)
				return;

			try
			{
				using (StreamReader reader = new StreamReader(e.Result))
				{
					var stockDataList = new List<InvestmentHistoryData>();
					string contents = reader.ReadToEnd();
					var lines = contents.Split('\n');
					bool firstLine = true;
					foreach (var line in lines)
					{
						// First line is metadata ==> ignore it
						if (firstLine)
						{
							firstLine = false;
							continue;
						}
						// Skip last empty line
						if (line.Length == 0)
							continue;
						var parts = line.Split(',');
						// Google data is separated as Date, Open, High, Low, Close, Volume
						var stockData = new InvestmentHistoryData()
						{
							Date = Convert.ToDateTime(parts[0]),
							Open = Convert.ToDouble(parts[1]),
							High = Convert.ToDouble(parts[2]),
							Low = Convert.ToDouble(parts[3]),
							Close = Convert.ToDouble(parts[4]),
							Price = Convert.ToDouble(parts[4]),
							Volume = Convert.ToDouble(parts[5]),
							Symbol = GetSymbolFromUrl(webClient.BaseAddress)
						};
						stockDataList.Add(stockData);
					}
					// Raise the OnChanged event
					if (OnQueryCompleted != null)
					{
						OnQueryCompleted(this, new EventWithDataArgs<IList<InvestmentHistoryData>>(stockDataList));
					}
				}
			}
			// Something went wrong!
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private string GetSymbolFromUrl(string url)
		{
			string pattern = ".*?q=([^&]+)*";
			var matches = Regex.Matches(url, pattern, RegexOptions.None);
			string stockName = matches[0].Groups[1].Value;
			return stockName;
		}
	}
}
