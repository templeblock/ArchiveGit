using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls.DataVisualization.Charting;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public abstract class CommonChartPanel : ViewPanel
	{
		private Chart _Chart;
		private ChartExtender _ChartExtender;
		private Line _VerticalLine = new Line();
		private bool _MouseDown;
		private LegendDialog _LegendDialog;

		internal abstract CommonChartData GetChartData();
		internal abstract string GetLegendLabel();
		internal abstract Brush GetLegendBrush();

		public CommonChartPanel()
		{
			base.Loaded += OnLoaded;
			base.VisibilityChange += OnVisibilityChange;
		}

		public override void Dispose()
		{
			base.Dispose();
			GetChartData().Dispose();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			_Chart = this.ChildOfType<Chart>();

			_ChartExtender = new ChartExtender(_Chart);
			_ChartExtender.MouseMove += OnMouseMove;
			_ChartExtender.MouseLeftButtonDown += OnMouseLeftButtonDown;
			_ChartExtender.MouseLeftButtonUp += OnMouseLeftButtonUp;

			// Start listening to the document holder
			App.Model.DocumentHolder.DocumentCreated += OnDocumentCreated;
			OnDocumentCreated(null, null);

			// Start listening to the profile holder
			App.Model.ProfileHolder.ProfileCreated += OnProfileCreated;
			OnProfileCreated(null, null);
		}

		private void OnVisibilityChange(bool visible)
		{
			if (base.IsVisible)
			{
				AttachBinding();
				if (_LegendDialog == null)
					AddChartOverlays();
			}
			else
			{
				//RemoveChartOverlays();
				DetachBinding();
			}
		}

		private void OnDocumentCreated(object sender, PropertyChangedEventArgs e)
		{
			// Start listening to the new document
			App.Model.DocumentHolder.Document.PropertyChanged += OnDocumentPropertyChanged;
			OnDocumentPropertyChanged(sender, null);
			if (base.IsVisible)
				AttachBinding();
		}

		private void OnDocumentPropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			if (e == null || e.PropertyName == "RunCount")
			{
				GetChartData().Build();
				SetChartOverlaysWithDate();
			}
		}

		private void OnProfileCreated(object sender, PropertyChangedEventArgs e)
		{
			// Start listening to the new profile
			App.Model.ProfileHolder.Profile.ProfileChanged += OnProfileChanged;
			OnProfileChanged(null, null);
		}

		private void OnProfileChanged(object sender, PropertyChangedEventArgs e)
		{
			if (e == null || e.PropertyName == "CurrentDate")
			{
				SetChartOverlaysWithDate();
			}
		}

		private void DetachBinding()
		{
			if (_Chart == null)
				return;
			_Chart.DataContext = null;
			((DataPointSeries)_Chart.Series[0]).ItemsSource = null;
			((DataPointSeries)_Chart.Series[1]).ItemsSource = null;
		}

		private void AttachBinding()
		{
			if (_Chart == null)
				return;
			_Chart.DataContext = GetChartData().Boundaries;
			((DataPointSeries)_Chart.Series[0]).ItemsSource = GetChartData().Balances;
			((DataPointSeries)_Chart.Series[1]).ItemsSource = GetChartData().TrendBalances;
		}

		internal void OnPlotAreaSizeChanged(object sender, SizeChangedEventArgs e)
		{
			SetChartOverlaysWithDate();
		}

		private void SetChartOverlaysWithDate()
		{
			JulianDay date = App.Model.ProfileHolder.Profile.CurrentDate;
			if (date == 0)
				return;

			Point position = new Point();
			position.X = _ChartExtender.GetPositionFromCoordinate(date.ToDateTime(), true/*xAxis*/);
			PositionVerticalLine(position);

			if (_LegendDialog != null)
			{
				_LegendDialog.Data.Label = GetLegendLabel();
				_LegendDialog.Data.BalanceBrush = GetLegendBrush();
				_LegendDialog.Data.Title = date.Year().ToString();
				_LegendDialog.Data.Balance = GetChartData().GetBalance(date);
				_LegendDialog.Data.TrendBalance = GetChartData().GetTrendBalance(date);
			}
		}

		private void OnMouseMove(object sender, ChartMouseEventArgs e)
		{
			if (!_MouseDown)
				return;

			PositionVerticalLine(e.PlotAreaPosition);
			Pair coordinate = e.PlotAreaCoordinate;
			JulianDay date = ((DateTime)coordinate.X).ToJulian();
			App.Model.ProfileHolder.Profile.CurrentDate = date;
		}

		private void OnMouseLeftButtonDown(object sender, ChartMouseEventArgs e)
		{
			_MouseDown = (sender as UIElement).CaptureMouse();
			OnMouseMove(sender, e);
		}

		private void OnMouseLeftButtonUp(object sender, ChartMouseEventArgs e)
		{
			_MouseDown = false;
			(sender as UIElement).ReleaseMouseCapture();
		}

		private void AddChartOverlays()
		{
			if (_ChartExtender == null || _ChartExtender.PlotArea == null)
				return;

			_ChartExtender.PlotArea.SizeChanged += OnPlotAreaSizeChanged;

			double dY = _ChartExtender.PlotArea.ActualHeight;
			_VerticalLine = new Line() { X2 = 0, Y2 = 1000, VerticalAlignment = VerticalAlignment.Stretch, Stroke = Colors.Black.ToBrush() };
			_ChartExtender.PlotArea.Children.Add(_VerticalLine);

			_LegendDialog = new LegendDialog(_ChartExtender.ChartArea);

			SetChartOverlaysWithDate();
		}
#if NOTUSED //j
		private void RemoveChartOverlays()
		{
			if (_ChartExtender == null || _ChartExtender.PlotArea == null)
				return;
			_ChartExtender.PlotArea.Children.Remove(_VerticalLine);

			_LegendDialog.Hide();
			_LegendDialog.Dispose();
			_LegendDialog = null;
		}
#endif
		private void PositionVerticalLine(Point position)
		{
			double X = position.X;
			_VerticalLine.X1 = X;
			_VerticalLine.X2 = X;
		}
	}

	public class IncomeChartPanel : CommonChartPanel
	{
		private IncomeChartData _ChartData = new IncomeChartData();

		internal override CommonChartData GetChartData()
		{
			return _ChartData;
		}

		internal override string GetLegendLabel()
		{
			return "Income";
		}

		internal override Brush GetLegendBrush()
		{
			return Colors.Green.ToBrush();
		}
	}

	public class SpendingChartPanel : CommonChartPanel
	{
		private SpendingChartData _ChartData = new SpendingChartData();

		internal override CommonChartData GetChartData()
		{
			return _ChartData;
		}

		internal override string GetLegendLabel()
		{
			return "Spending";
		}

		internal override Brush GetLegendBrush()
		{
			return Colors.Red.ToBrush();
		}
	}

	public class PortfolioChartPanel : CommonChartPanel
	{
		private PortfolioChartData _ChartData = new PortfolioChartData();

		internal override CommonChartData GetChartData()
		{
			return _ChartData;
		}

		internal override string GetLegendLabel()
		{
			return "Portfolio";
		}

		internal override Brush GetLegendBrush()
		{
			return Colors.Red.ToBrush();
		}
	}
}
