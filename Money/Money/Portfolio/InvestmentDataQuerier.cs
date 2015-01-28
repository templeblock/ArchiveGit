using System;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Json;
using System.Net;
using System.Text.RegularExpressions;
using System.Threading;

namespace StockVisualization
{
	public class GoogleInvestmentDataQuerier
	{
		private string _Symbol;
		private BackgroundWorker _queryService;
		private const string _ServiceUrl = "http://www.google.com/finance/info?q={0}";

		public event StockDataEventHandler<InvestmentData> OnDataUpdated;

		public void Subscribe(string symbol)
		{
			_Symbol = symbol;
			_queryService = new BackgroundWorker();
			_queryService.DoWork += new DoWorkEventHandler((s, e) => { this.QueryData(_Symbol); });
			_queryService.RunWorkerAsync();
		}

		private void QueryData(string symbol)
		{
			WebClient queryWebClient = new WebClient();
			queryWebClient.OpenReadCompleted += OnOpenReadCompleted;
			var url = String.Format(_ServiceUrl, symbol);
			var proxiedUrl = ProxyUrl.Get(url);
			queryWebClient.BaseAddress = proxiedUrl;
			queryWebClient.OpenReadAsync(new Uri(proxiedUrl, UriKind.Absolute));
		}

		void OnOpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
		{
			if (e.Error != null)
				return;

			var webClient = sender as WebClient;
			if (webClient == null)
				return;

			using (StreamReader reader = new StreamReader(e.Result))
			{
				string contents = reader.ReadToEnd();
				contents = contents.Replace("//", "");

				// Change the thread culture to en-US to make parsing of the dates returned easier
				var originalCulture = Thread.CurrentThread.CurrentCulture;
				Thread.CurrentThread.CurrentCulture = new CultureInfo("en-US");
				try
				{
					JsonArray items = (JsonArray)JsonArray.Parse(contents);
					if (items.Count > 0)
					{
						foreach (JsonObject item in items)
						{
							double value = Convert.ToDouble((string)item["l"]);
							double change = Convert.ToDouble((string)item["c"]);
							string dateTime = (string)item["lt"];
							dateTime = dateTime.Replace(" EDT", " EST");
							DateTime date = DateTime.ParseExact(dateTime, "MMM d, h:mmtt EST", CultureInfo.CurrentUICulture);
							var data = new InvestmentData()
							{
								Symbol = _Symbol,
								TimeStamp = date,
								Price = value,
								Change = change
							};
							// For simplicity, fire a change event every time
							if (OnDataUpdated != null)
							{
								OnDataUpdated(this, new EventWithDataArgs<InvestmentData>(data));
							}
							// Start over
							Thread.Sleep(1000);
							this.QueryData(_Symbol);
						}
					}
				}
				// The response could not be parsed
				catch (Exception ex)
				{
					ex.GetType();
				}
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
