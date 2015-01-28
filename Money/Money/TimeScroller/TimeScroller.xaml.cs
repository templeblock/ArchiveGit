using System.Collections.Specialized;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public partial class TimeScroller : ItemsControl
	{
		private const string ScrollViewerElement = "x_ScrollViewer";
		private const string TimeScaleElement = "x_TimeScaleController";
		private const string ItemsPanelElement = "x_ItemsPanel";

		private Span m_Span = Spans.Find(SpanUnit.Year);
		internal Span Span { get { return m_Span; } set { m_Span = value; SetScrollArea(); } }

		private JulianDay m_StartDate;
		public int StartDate { get { return (int)m_StartDate; } set { m_StartDate = (JulianDay)value; SetScrollArea(); } }

		private JulianDay m_EndDate;
		public int EndDate { get { return (int)m_EndDate; } set { m_EndDate = (JulianDay)value; SetScrollArea(); } }

		private ScrollViewer x_ScrollViewer;
		private TimeScaleController x_TimeScaleController;
		private Panel x_ItemsPanel;

		private JulianDay m_CurrentDate;
		private bool m_MouseDown = false;
		//private ScrollBar m_HorizontalScrollBar;
		private Point m_MousePosition = new Point();
		private Point m_ScrollViewerOffset = new Point();

		public TimeScroller()
		{
			base.DefaultStyleKey = typeof(TimeScroller);
			InitializeComponent();
		}

		// Undoes the effects of the System.Windows.Controls.ItemsControl.PrepareContainerForItemOverride(System.Windows.DependencyObject,System.Object) method
		protected override void ClearContainerForItemOverride(DependencyObject element, object item)
		{
			base.ClearContainerForItemOverride(element, item);
			//j Disconnect any events from the dataItem
		}

		// Creates or identifies the element that is used to display the given item.
		protected override DependencyObject GetContainerForItemOverride()
		{
			TimeEvent item = new TimeEvent();
			return item;
		}

		// Determines if the specified item is (or is eligible to be) its own container.
		protected override bool IsItemItsOwnContainerOverride(object item)
		{
			return item is TimeScroller; //base.IsItemItsOwnContainerOverride(item);
		}

		// Invoked when the System.Windows.Controls.ItemsControl.Items property changes.
		protected override void OnItemsChanged(NotifyCollectionChangedEventArgs e)
		{
			base.OnItemsChanged(e);
		}

		// Prepares the specified element to display the specified item.
		protected override void PrepareContainerForItemOverride(DependencyObject element, object item)
		{
			base.PrepareContainerForItemOverride(element, item);
			TimeEvent timeEvent = (TimeEvent)element;
			timeEvent.ParentTimeScroller = this;
			if (item is TimeEvent)
		    {
				TimeEvent timeEventSource = item as TimeEvent;
				timeEvent.EventName = timeEventSource.EventName;
				timeEvent.StartDate = timeEventSource.StartDate;
				timeEvent.EndDate = timeEventSource.EndDate;
			}
			else
			if (item is Package)
			{
				Package package = item as Package;
				timeEvent.EventName = package.Name;
				timeEvent.StartDate = package.StartProfileDate.JulianDay;
				timeEvent.EndDate = package.EndProfileDate.JulianDay;
			}
		}

		public override void OnApplyTemplate()
		{
			base.OnApplyTemplate();
			x_ScrollViewer = base.GetTemplateChild(ScrollViewerElement) as ScrollViewer;
			x_ScrollViewer.SizeChanged += OnScrollViewerSizeChanged;

			x_TimeScaleController = base.GetTemplateChild(TimeScaleElement) as TimeScaleController;

//j			App.MainPage.x_SpendingTimeline.DateChanged += OnTimelineDateChanged;
		}
#if NOTUSED //j
		protected override Size MeasureOverride(Size availableSize)
		{
			if (x_ItemsPanel == null)
				return availableSize;

			double totalWidth = 0;
			double totalHeight = 0;

			foreach (UIElement item in x_ItemsPanel.Children)
			{
				item.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
				Size childSize = item.DesiredSize;
				totalWidth += childSize.Width;
				totalHeight += childSize.Height;
			}

			//return new Size(totalWidth, totalHeight);
			return availableSize;
		}

		protected override Size ArrangeOverride(Size finalSize)
		{
			if (x_ItemsPanel == null)
				return finalSize;

			double lastX = 0;
			double lastY = 0;
			foreach (UIElement item in x_ItemsPanel.Children)
			{
				double y = item.GetTop();
				if (double.IsNaN(y) == true)
					y = 0;

				TimeEvent timeEvent = item as TimeEvent;
				Point? point = DateToPosition(timeEvent.StartDate);
				double pos = ((Point)point).X;

				double left = finalSize.Width * pos;
				double top = y;

				double width = item.DesiredSize.Width;
				double height = item.DesiredSize.Height;
				if (lastX + width > left)
					top = lastY + 20 + y;
				else
					top = 0 + y;

				item.Arrange(new Rect(left, top, width, height));
				lastX = left;
				lastY = top;
			}

			return finalSize;
		}
#endif
		private void SetScrollArea()
		{
			if (x_ScrollViewer == null || x_TimeScaleController == null)
				return;

			SetItemsPanelSize();

//j			if (m_CurrentDate == 0)
//j				OnTimelineDateChanged(App.MainPage.x_SpendingTimeline.Date);

			x_TimeScaleController.Initialize(m_CurrentDate, m_Span, x_ScrollViewer.ViewportWidth);
		}

		private void SetItemsPanelSize()
		{
			if (x_ItemsPanel == null)
				return;

			int days = (int)m_EndDate - (int)m_StartDate;
			double width = days * m_Span.PixelsPerDay(x_ScrollViewer.ViewportWidth);
			x_ItemsPanel.Width = width + x_ScrollViewer.ViewportWidth;
			if (x_ItemsPanel.Height == 0 || double.IsNaN(x_ItemsPanel.Height))
				x_ItemsPanel.Height = base.ActualHeight;
			x_ItemsPanel.UpdateLayout();
			if (x_ItemsPanel.Children == null)
				return;

			// Arrange all of the child time events so that they do now overlap
			double nextLeft = 0;
			double nextTop = 0;
			foreach (UIElement item in x_ItemsPanel.Children)
			{
				TimeEvent timeEvent = item as TimeEvent;
				timeEvent.ComputeLocation();

				double left = timeEvent.GetLeft();
				if (left < nextLeft)
					nextTop += (timeEvent.DesiredSize.Height - 1);
				nextLeft = left + timeEvent.Width;

				if (nextTop != timeEvent.GetTop())
					timeEvent.SetTop(nextTop);
			}
		}

		public Point ComputeTimeEventLocation(JulianDay startDate, JulianDay endDate)
		{
			double halfViewportWidth = x_ScrollViewer.ViewportWidth / 2;
			double start = halfViewportWidth + DateToPosition(startDate);
			double end = halfViewportWidth + DateToPosition(endDate);
			return new Point(start, end);
		}

		private double DateToPosition(JulianDay date)
		{
			if (x_ScrollViewer == null || m_Span == null)
				return 0;

			int days = (int)date - (int)m_StartDate;
			double position = days * m_Span.PixelsPerDay(x_ScrollViewer.ViewportWidth);
			return position;
		}

		private JulianDay PositionToDate(double position)
		{
			if (x_ScrollViewer == null || m_Span == null)
				return 0;

			double pixelsPerDay = m_Span.PixelsPerDay(x_ScrollViewer.ViewportWidth);
			int days = (int)(((position / pixelsPerDay).Round()));
			return (JulianDay)((int)m_StartDate + days);
		}

		private void OnScrollViewerSizeChanged(object sender, SizeChangedEventArgs e)
		{
			//if (m_HorizontalScrollBar == null)
			//{
			//    FrameworkElement element = VisualTreeHelper.GetChild(x_ScrollViewer, 0) as FrameworkElement;
			//    m_HorizontalScrollBar = (element != null ? element.FindName("HorizontalScrollBar") as ScrollBar : null);
			//}

			//SetScrollArea();
		}

		private void OnTimelineDateChanged(JulianDay date)
		{
			m_CurrentDate = date;
			x_TimeScaleController.CurrentDate = date;
			double position = DateToPosition(date);
			x_ScrollViewer.ScrollToHorizontalOffset(position);
		}

		void OnItemsPanelLoaded(object sender, RoutedEventArgs e)
		{
			x_ItemsPanel = sender as Panel;
			x_ItemsPanel.Loaded -= OnItemsPanelLoaded;
			SetScrollArea();
		}

		private void OnItemsPanelLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			if (m_MouseDown)
				return;

			m_MouseDown = x_ItemsPanel.CaptureMouse();
			m_MousePosition = e.GetPosition(x_ScrollViewer);
			m_ScrollViewerOffset.X = x_ScrollViewer.HorizontalOffset;
			m_ScrollViewerOffset.Y = x_ScrollViewer.VerticalOffset;
		}

		private void OnItemsPanelLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			if (!m_MouseDown)
				return;

			m_MouseDown = false;
			x_ItemsPanel.ReleaseMouseCapture();
		}

		private void OnItemsPanelMove(object sender, MouseEventArgs e)
		{
			if (!m_MouseDown)
				return;

			double hoff = m_ScrollViewerOffset.X - (e.GetPosition(x_ScrollViewer).X - m_MousePosition.X);
			double voff = m_ScrollViewerOffset.Y - (e.GetPosition(x_ScrollViewer).Y - m_MousePosition.Y);
			if (hoff < 0) hoff = 0;
			if (voff < 0) voff = 0;
			if (hoff > x_ScrollViewer.ExtentWidth) hoff = x_ScrollViewer.ExtentWidth;
			if (voff > x_ScrollViewer.ExtentHeight) voff = x_ScrollViewer.ExtentHeight;
			x_ScrollViewer.ScrollToHorizontalOffset(hoff);
			x_ScrollViewer.ScrollToVerticalOffset(voff);

//j			App.MainPage.x_SpendingTimeline.Date = PositionToDate(hoff);
		}

		private void OnTimeScaleControllerClick(object sender, MouseButtonEventArgs e)
		{
			if (m_Span.Unit == SpanUnit.Week)	Span = Spans.Find(SpanUnit.Month); else
			if (m_Span.Unit == SpanUnit.Month)	Span = Spans.Find(SpanUnit.Year); else
			if (m_Span.Unit == SpanUnit.Year)	Span = Spans.Find(SpanUnit.Decade); else
			if (m_Span.Unit == SpanUnit.Decade)	Span = Spans.Find(SpanUnit.Week);
		}
	}
}

#if NOTUSED //j
		private void ListControl_DeleteSelectedTransaction(object sender)
		{
			ContentPresenter item = (sender as DependencyObject).ParentOfType<ContentPresenter>();
			if (item == null)
				return;
			Transaction transaction = (item.Content as Transaction);
			if (transaction != null)
				m_CurrentPackage.Transactions.Remove(transaction);
		}

		private void InsertNewTransaction(object sender)
		{
			if (m_DataGrid != null && m_DataGrid.IsVisible() && !m_DataGrid.IsReadOnly)
				DataGrid_InsertNewTransaction(sender);
			if (m_ListControl != null && m_ListControl.IsVisible())
				ListControl_InsertNewTransaction(sender);
		}

		private void DataGrid_InsertNewTransaction(object sender)
		{
			DataGridRow item = (sender as DependencyObject).ParentOfType<DataGridRow>();
			if (item != null)
				m_DataGrid.SelectedIndex = item.GetIndex();
			Transaction transaction = (m_DataGrid.SelectedItem as Transaction);
			int index = m_CurrentPackage.Transactions.IndexOf(transaction);
			if (index >= 0)
				index++;
			else
				index = m_CurrentPackage.Transactions.Count;

			m_CurrentPackage.Transactions.Insert(index, Transaction.CreateDefault());
			m_DataGrid.SelectedIndex = index;
		}

		private void ListControl_InsertNewTransaction(object sender)
		{
			ContentPresenter item = (sender as DependencyObject).ParentOfType<ContentPresenter>();
			if (item == null)
				return;
			Transaction transaction = (item.Content as Transaction);
			int index = m_CurrentPackage.Transactions.IndexOf(transaction);
			if (index >= 0)
				index++;
			else
				index = m_CurrentPackage.Transactions.Count;

			m_CurrentPackage.Transactions.Insert(index, Transaction.CreateDefault());
		}
#endif
