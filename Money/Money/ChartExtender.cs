using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.DataVisualization;
using System.Windows.Controls.DataVisualization.Charting;
using System.Windows.Controls.DataVisualization.Charting.Primitives;
using System.Windows.Input;
using ClassLibrary;

namespace Money
{
	public delegate void ChartMouseEventHandler(object sender, ChartMouseEventArgs e);

	public sealed class ChartMouseEventArgs
	{
		public MouseEventArgs MouseEventArgs { get; set; }
//j		public IRangeAxis XAxis { get; set; }
//j		public IRangeAxis YAxis { get; set; }
//j		public EdgePanel ChartArea { get; set; }
//j		public Panel PlotArea { get; set; }
		public Point PlotAreaPosition { get; set; }
		public Pair PlotAreaCoordinate { get; set; }
	}

	public class ChartExtender
	{
		public event ChartMouseEventHandler MouseEnter;
		public event ChartMouseEventHandler MouseLeave;
		public event ChartMouseEventHandler MouseMove;
		public event ChartMouseEventHandler MouseLeftButtonDown;
		public event ChartMouseEventHandler MouseLeftButtonUp;

		public Chart Chart { get { return _Chart; } set { _Chart = value; } }
		private Chart _Chart;

		#region Properties which provide convenient access to visual tree elements

		//	<chart>
		//	+ other stuff
		//		+	<chartingprimitives:EdgePanel x:Name="ChartArea">
		//			+	<Grid> // PlotArea container; this is what we return in the PlotArea property
		//				+ <Canvas x:Name="PlotArea">

		public EdgePanel ChartArea
		{
			get
			{
				if (_EdgePanel == null && _Chart != null)
					_EdgePanel = _Chart.ChildOfType<EdgePanel>();
				return _EdgePanel;
			}
		}
		private EdgePanel _EdgePanel;

		public Panel PlotArea
		{
			get
			{
				if (_PlotArea == null && ChartArea != null)
					_PlotArea = ChartArea.ChildOfType<Grid>();
				//or its child; _PlotArea = ChartArea.ChildOfTypeAndName<Canvas>("PlotArea");
				return _PlotArea;
			}
		}
		private Panel _PlotArea;

		protected IRangeAxis XAxis
		{
			get
			{
				if (_XAxis == null && _Chart != null)
				{
					foreach (IAxis axis in _Chart.Axes)
					{
						if ((axis is IRangeAxis) && axis.Orientation == AxisOrientation.X)
							_XAxis = (IRangeAxis)axis;
					}
				}
				return _XAxis;
			}
		}
		private IRangeAxis _XAxis;

		protected IRangeAxis YAxis
		{
			get
			{
				if (_YAxis == null && _Chart != null)
				{
					foreach (IAxis axis in _Chart.Axes)
					{
						if ((axis is IRangeAxis) && axis.Orientation == AxisOrientation.Y)
							_YAxis = (IRangeAxis)axis;
					}
				}
				return _YAxis;
			}
		}
		private IRangeAxis _YAxis;

		#endregion

		public ChartExtender(Chart chart)
		{
			_Chart = chart;
			if (_Chart == null)
				return;

			_Chart.MouseEnter += OnMouseEnter;
			_Chart.MouseLeave += OnMouseLeave;
			_Chart.MouseMove += OnMouseMove;
			_Chart.MouseLeftButtonDown += OnMouseLeftButtonDown;
			_Chart.MouseLeftButtonUp += OnMouseLeftButtonUp;
		}

		private void OnMouseEnter(object sender, MouseEventArgs e)
		{
			if (MouseEnter == null)
				return;
			ChartMouseEventArgs args = GetChartMouseEventArgs(sender, e);
			if (args != null)
				MouseEnter(sender, args);
		}

		private void OnMouseLeave(object sender, MouseEventArgs e)
		{
			if (MouseLeave == null)
				return;
			ChartMouseEventArgs args = GetChartMouseEventArgs(sender, e);
			if (args != null)
				MouseLeave(sender, args);
		}

		private void OnMouseMove(object sender, MouseEventArgs e)
		{
			if (MouseMove == null /*|| !InsidePlotArea(e)*/)
				return;
			ChartMouseEventArgs args = GetChartMouseEventArgs(sender, e);
			if (args != null)
				MouseMove(sender, args);
		}

		private void OnMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			if (MouseLeftButtonDown == null)
				return;
			ChartMouseEventArgs args = GetChartMouseEventArgs(sender, e);
			if (args != null)
				MouseLeftButtonDown(sender, args);
		}

		private void OnMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			if (MouseLeftButtonUp == null)
				return;
			ChartMouseEventArgs args = GetChartMouseEventArgs(sender, e);
			if (args != null)
				MouseLeftButtonUp(sender, args);
		}

		private bool InsidePlotArea(MouseEventArgs e)
		{
			return (PlotArea != null && PlotArea.HitTest(e).Count != 0);
		}

		private ChartMouseEventArgs GetChartMouseEventArgs(object sender, MouseEventArgs e)
		{
			if (_Chart == null || ChartArea == null || PlotArea == null || XAxis == null || YAxis == null)
				return null;

			ChartMouseEventArgs args = new ChartMouseEventArgs();
			args.MouseEventArgs = e;
//j			args.XAxis = XAxis;
//j			args.YAxis = YAxis;
//j			args.ChartArea = ChartArea;
//j			args.PlotArea = PlotArea;
			args.PlotAreaPosition = e.GetPosition(PlotArea);
			args.PlotAreaCoordinate = GetCoordinateFromPosition(args.PlotAreaPosition);
#if NOTUSED //j
			LineSeries ls = null;
			foreach (ISeries series in x_Chart.Series)
				if (series is LineSeries)
					ls = (LineSeries)series;
			if (ls != null && ls.Points != null)
			{
				List<Point> points = ls.Points.ToList();
				Point point = points.Where(p => p.X >= mousePos.X).ElementAtOrDefault(0);
				int index = (points.Contains(point) ? points.IndexOf(point) : -1);
			}
#endif
			return args;
		}

		private Pair GetCoordinateFromPosition(Point position)
		{
			if (PlotArea == null || XAxis == null || YAxis == null)
				return new Pair { X = null, Y = null };

			IComparable xValue = XAxis.GetValueAtPosition(new UnitValue(position.X, Unit.Pixels));
			IComparable yValue = YAxis.GetValueAtPosition(new UnitValue(PlotArea.ActualHeight - position.Y, Unit.Pixels));
			Pair coord = new Pair() { X = xValue, Y = yValue };
			return coord;
		}

		public double GetPositionFromCoordinate(object coord, bool xAxis)
		{
			if (PlotArea == null || XAxis == null || YAxis == null)
				return 0;

			IRangeAxis axis = (xAxis ? XAxis : YAxis);
			UnitValue position = axis.GetPlotAreaCoordinate(coord);
			return position.Value;
		}
	}
}
