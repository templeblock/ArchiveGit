using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.DataVisualization.Charting;
using System.Windows.Input;
using ClassLibrary;

namespace Money
{
	public class PlanNetWorthDataGridPanel : ViewPanel
	{
		private DataGrid _DataGrid;

		public PlanNetWorthDataGridPanel()
		{
			base.Loaded += OnLoaded;
			base.VisibilityChange += OnVisibilityChange;
		}

		private void OnVisibilityChange(bool visible)
		{
			if (base.IsVisible)
				AttachBinding();
			else
				DetachBinding();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			_DataGrid = this.ChildOfType<DataGrid>();

			// Start listening to the document holder
			App.Model.DocumentHolder.DocumentCreated += OnDocumentCreated;
			OnDocumentCreated(null, null);
		}

		private void OnDocumentCreated(object sender, PropertyChangedEventArgs e)
		{
			if (base.IsVisible)
				AttachBinding();
		}

		private void DetachBinding()
		{
			if (_DataGrid != null)
				_DataGrid.ItemsSource = null;
		}

		private void AttachBinding()
		{
			if (_DataGrid != null)
				_DataGrid.ItemsSource = App.Model.DocumentHolder.Document.Accounts.VisibleAccountsFilter();
		}
	}

	public class PlanNetWorthChartPanel : ViewPanel
	{
		private Chart _Chart;

		public PlanNetWorthChartPanel()
		{
			base.Loaded += OnLoaded;
			base.VisibilityChange += OnVisibilityChange;
		}

		private void OnVisibilityChange(bool visible)
		{
			if (base.IsVisible)
				AttachBinding();
			else
				DetachBinding();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			_Chart = this.ChildOfType<Chart>();

			// Add event handlers for our children
			_Chart.MouseLeftButtonDown += OnMouseLeftButtonDown;

			// Start listening to the document holder
			App.Model.DocumentHolder.DocumentCreated += OnDocumentCreated;
			OnDocumentCreated(null, null);
		}

		private void OnDocumentCreated(object sender, PropertyChangedEventArgs e)
		{
			if (base.IsVisible)
				AttachBinding();
		}

		private void DetachBinding()
		{
			if (_Chart != null)
				((DataPointSeries)_Chart.Series[0]).ItemsSource = null;
		}

		private void AttachBinding()
		{
			if (_Chart != null)
				((DataPointSeries)_Chart.Series[0]).ItemsSource = App.Model.DocumentHolder.Document.Accounts.VisibleAccountsFilter();
		}

		// Called when the Chart is clicked
		private void OnMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Chart chart = sender as Chart;
			if (chart == null)
				return;
		}
	}
}
