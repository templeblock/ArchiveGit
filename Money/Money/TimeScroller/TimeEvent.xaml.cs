using System.Windows;
using System.Windows.Controls;
using System;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public partial class TimeEvent : Control
	{
		private TimeScroller m_ParentTimeScroller;
		public TimeScroller ParentTimeScroller { get { return m_ParentTimeScroller; } internal set { m_ParentTimeScroller = value; ComputeLocation(); } }

		private JulianDay m_StartDate;
		public JulianDay StartDate { get { return m_StartDate; } set { m_StartDate = value; ComputeLocation(); } }

		private JulianDay m_EndDate;
		public JulianDay EndDate { get { return m_EndDate; } set { m_EndDate = value; ComputeLocation(); } }

		private string m_EventName;
		public string EventName { get { return m_EventName; } set { m_EventName = value; if (x_EventName != null) x_EventName.Text = m_EventName; } }

		private TextBlock x_EventName;

		public TimeEvent()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			ComputeLocation();
		}

		public override void OnApplyTemplate()
		{
			base.OnApplyTemplate();
			x_EventName = base.GetTemplateChild("x_EventName") as TextBlock;
			x_EventName.Text = m_EventName;
		}

		internal void ComputeLocation()
		{
			if (m_StartDate == 0 || m_EndDate == 0 || m_ParentTimeScroller == null)
				return;

			Point point = m_ParentTimeScroller.ComputeTimeEventLocation(m_StartDate, m_EndDate);
			double left = point.X;
			if (left != this.GetLeft())
				this.SetLeft(left);

			double width = point.Y - point.X;
			if (width <= 0)
				width = base.ActualWidth;
			if (width > 0 && width != base.Width)
				base.Width = width;
		}
	}
}
