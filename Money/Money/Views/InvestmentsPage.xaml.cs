using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;
using StockVisualization;
using System.Windows.Controls.DataVisualization.Charting;

namespace Money
{
	public partial class InvestmentsPage : Page
	{
		private GoogleInvestmentDataQuerier _liveDataQuerier;
		private InvestmentHistoryDataQuerier _historicDataQuerier;

		public InvestmentsPage()
		{
			InitializeComponent();
			HtmlTextBlock content = base.Content.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;

			_liveDataQuerier = new GoogleInvestmentDataQuerier();
			_liveDataQuerier.OnDataUpdated += OnDataUpdated;
			_historicDataQuerier = new InvestmentHistoryDataQuerier();
			_historicDataQuerier.OnQueryCompleted += OnQueryCompleted;
			UpdateChartData();
		}

		public void UpdateChartData()
		{
			// Clear the title of the chart
			x_Chart.Title = "";
			// Show the loading indicator
			x_PanelFetchingData.Visibility = Visibility.Visible;
			var stockCode = (x_ComboStockCode.SelectedItem as ComboBoxItem).Content.ToString();
			// Change the live updates to the new stock
			_liveDataQuerier.Subscribe(stockCode);
			// Query a quarter's data and set the chart to show the last 6 months (the rest can be panned)
			_historicDataQuerier.QueryData(stockCode, DateTime.Now.AddMonths(-6), DateTime.Now);
			// If the X axis has not yet been initalized (that is we're calling UpdateChartData from the MainPage constructor) we'll have to manually assign it
			//if (x_Chart.XAxis == null)
			//    x_Chart.XAxis = new DateTimeAxis();
			//x_Chart.XAxis.Range = new DateTimeRange() { Minimum = DateTime.Now.AddMonths(-1), Maximum = DateTime.Now };
		}

		private void OnUpdateClick(object sender, RoutedEventArgs e)
		{
			x_Chart.Title = "";
			UpdateChartData();
		}

		void OnQueryCompleted(object sender, EventWithDataArgs<IList<InvestmentHistoryData>> e)
		{
			// Hide the loading indicator
			x_PanelFetchingData.Visibility = Visibility.Collapsed;
			// Set the returned data as ItemsSource to the BindableDataSeries: this will update the series
			((DataPointSeries)x_Chart.Series[0]).ItemsSource = e.Data;
		}

		void OnDataUpdated(object sender, EventWithDataArgs<InvestmentData> e)
		{
			if (e.Data == null)
				return;

			Dispatcher.BeginInvoke(() =>
			{
				x_Chart.Title = string.Format("{0}: {1} ({2})", e.Data.Symbol, e.Data.Price, e.Data.Change);
			});
		}
	}
}
