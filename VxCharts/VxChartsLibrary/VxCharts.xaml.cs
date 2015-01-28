using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.DataVisualization;
using System.Windows.Controls.DataVisualization.Charting;

namespace ClassLibrary
{
	public partial class VxCharts : UserControl
	{
		// The Chart dependency property
		public static readonly DependencyProperty ChartProperty =
			DependencyProperty.Register("Chart", typeof(Chart), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Chart Chart
		{
			get { return x_Chart; }
			set { x_Chart = value; }
		}

		// The Axes dependency property
		public static readonly DependencyProperty AxesProperty =
			DependencyProperty.Register("Axes", typeof(Collection<IAxis>), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Collection<IAxis> Axes
		{
			get { return x_Chart.Axes; }
			set { x_Chart.Axes = value; }
		}

		// The Palette dependency property
		public static readonly DependencyProperty PaletteProperty =
			DependencyProperty.Register("Palette", typeof(Collection<ResourceDictionary>), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Collection<ResourceDictionary> Palette
		{
			get { return x_Chart.Palette; }
			set { x_Chart.Palette = value; }
		}

		// The Series dependency property
		public static readonly DependencyProperty SeriesProperty =
			DependencyProperty.Register("Series", typeof(Collection<ISeries>), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Collection<ISeries> Series
		{
			get { return x_Chart.Series; }
			set { x_Chart.Series = value; }
		}

		// The LegendItems dependency property
		public static readonly DependencyProperty LegendItemsProperty =
			DependencyProperty.Register("LegendItems", typeof(Collection<object>), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Collection<object> LegendItems
		{
			get { return x_Chart.LegendItems; }
		}

		// The LegendStyle dependency property
		public static readonly DependencyProperty LegendStyleProperty =
			DependencyProperty.Register("LegendStyle", typeof(Style), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Style LegendStyle
		{
			get { return x_Chart.LegendStyle; }
			set { x_Chart.LegendStyle = value; }
		}

		// The ChartAreaStyle dependency property
		public static readonly DependencyProperty ChartAreaStyleProperty =
			DependencyProperty.Register("ChartAreaStyle", typeof(Style), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Style ChartAreaStyle
		{
			get { return x_Chart.ChartAreaStyle; }
			set { x_Chart.ChartAreaStyle = value; }
		}

		// The PlotAreaStyle dependency property
		public static readonly DependencyProperty PlotAreaStyleProperty =
			DependencyProperty.Register("PlotAreaStyle", typeof(Style), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Style PlotAreaStyle
		{
			get { return x_Chart.PlotAreaStyle; }
			set { x_Chart.PlotAreaStyle = value; }
		}

		// The Title dependency property
		public static readonly DependencyProperty TitleProperty =
			DependencyProperty.Register("Title", typeof(object), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public object Title
		{
			get { return x_Chart.Title; }
			set { x_Chart.Title = value; }
		}

		// The TitleStyle dependency property
		public static readonly DependencyProperty TitleStyleProperty =
			DependencyProperty.Register("TitleStyle", typeof(Style), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public Style TitleStyle
		{
			get { return x_Chart.TitleStyle; }
			set { x_Chart.TitleStyle = value; }
		}

		// The LegendTitle dependency property
		public static readonly DependencyProperty LegendTitleProperty =
			DependencyProperty.Register("LegendTitle", typeof(object), typeof(VxCharts),
			new PropertyMetadata(null, null));

		public object LegendTitle
		{
			get { return x_Chart.LegendTitle; }
			set { x_Chart.LegendTitle = value; }
		}

		public VxCharts()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
#if NOTUSED
			Series.CollectionChanged += delegate(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
			{
			};
#endif
		}

		void OnLoaded(object sender, RoutedEventArgs e)
		{
			//foreach (ISeries series in Series)
			//{
			//    DefinitionSeries defSeries = series as DefinitionSeries;
			//    if (defSeries == null)
			//        continue;
			//    Collection<SeriesDefinition> collection = defSeries.SeriesDefinitions;
			//    int count = collection.Count;
			//    Collection<SeriesDefinition> reverseCollection = new Collection<SeriesDefinition>();
			//    for (int i = count - 1; i >= 0; i--)
			//        reverseCollection.Add(collection[i]);
			//    collection.Clear();
			//    for (int i = 0; i < count; i++)
			//        collection.Add(reverseCollection[i]);
			//}
		}

#if NOTUSED
		private void OnSeriesListChanged(Collection<ISeries> oldSeries, Collection<ISeries> newSeries)
		{
			if (oldSeries == newSeries || newSeries == null)
				return;
			//x_Chart.Series.Add(newSeries);
		}

		private void OnSeriesChanged(ISeries oldSeries, ISeries newSeries)
		{
			if (oldSeries == newSeries || newSeries == null)
				return;
			//x_Chart.Series.Add(newSeries);
		}
#endif
	}

	public class VxResourceDictionaryCollection : ResourceDictionaryCollection { }
	// Charting types
	public class VxAxisLabel : AxisLabel { }
	public class VxAreaDataPoint : AreaDataPoint { }
	//public class VxAxis : Axis { }
	public class VxAreaSeries : AreaSeries { }
	public class VxBarDataPoint : BarDataPoint { }
	public class VxBarSeries : BarSeries { }
	public class VxBubbleDataPoint : BubbleDataPoint { }
	public class VxBubbleSeries : BubbleSeries { }
	public class VxCategoryAxis : CategoryAxis { }
	public class VxColumnDataPoint : ColumnDataPoint { }
	public class VxColumnSeries : ColumnSeries { }
	public class VxDataPoint : DataPoint { }
	//public class VxDataPointSeries : DataPointSeries { }
	//public class VxDataPointSeriesWithAxes : DataPointSeriesWithAxes { }
	//public class VxDataPointSingleSeriesWithAxes : DataPointSingleSeriesWithAxes { }
	public class VxDateTimeAxis : DateTimeAxis { }
	public class VxDateTimeAxisLabel : DateTimeAxisLabel { }
	//public class VxDefinitionSeries : DefinitionSeries { }
	//public class VxDisplayAxis : DisplayAxis { }
	public class VxLegendItem : LegendItem { }
	public class VxLinearAxis : LinearAxis { }
	public class VxLineDataPoint : LineDataPoint { }
	public class VxLineSeries : LineSeries { }
	//public class VxNumericAxis : NumericAxis { }
	public class VxNumericAxisLabel : NumericAxisLabel { }
	public class VxPieDataPoint : PieDataPoint { }
	public class VxPieSeries : PieSeries { }
	//public class VxRangeAxis : RangeAxis { }
	public class VxScatterDataPoint : ScatterDataPoint { }
	public class VxScatterSeries : ScatterSeries { }
	public class VxSeries : Series { }
	public class VxSeriesDefinition : SeriesDefinition { }
	public class VxStacked100AreaSeries : Stacked100AreaSeries { }
	public class VxStacked100BarSeries : Stacked100BarSeries { }
	public class VxStacked100ColumnSeries : Stacked100ColumnSeries { }
	public class VxStacked100LineSeries : Stacked100LineSeries { }
	public class VxStackedAreaLineSeriesvStackedAreaLineSeries { }
	public class VxStackedAreaSeries : StackedAreaSeries { }
	//public class VxStackedBarColumnSeries : StackedBarColumnSeries { }
	public class VxStackedBarSeries : StackedBarSeries { }
	public class VxStackedColumnSeries : StackedColumnSeries { }
	public class VxStackedLineSeries : StackedLineSeries { }
}
