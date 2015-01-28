using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
    public partial class TimeScaleController : UserControl
    {
		private JulianDay m_CurrentDate;
		public JulianDay CurrentDate { get { return m_CurrentDate; } set { SetCurrentDate(value); } }

		private double m_ViewportWidth;
		private Span m_Span;
		private TimeScale m_LeftPanel;
		private TimeScale m_CenterPanel;
		private TimeScale m_RightPanel;

		public TimeScaleController()
		{
			InitializeComponent();

			m_Span = Spans.Find(SpanUnit.Year);
			m_LeftPanel = x_Panel0;
			m_CenterPanel = x_Panel1;
			m_RightPanel = x_Panel2;
		}

		internal void Initialize(JulianDay currentDate, Span span, double viewportWidth)
		{
			m_Span = span;
			m_ViewportWidth = viewportWidth;
			m_CurrentDate = currentDate;

			RectangleGeometry rect = new RectangleGeometry();
			rect.Rect = new Rect(0, 0, m_ViewportWidth, base.ActualHeight);
			base.Clip = rect;

			InitializePanels();
		}

		private void InitializePanels()
		{
			if (m_ViewportWidth == 0 || m_Span == null)
				return;

			// Set the panel spans
			m_LeftPanel.Span = m_Span;
			m_CenterPanel.Span = m_Span;
			m_RightPanel.Span = m_Span;

			// Set the panel widths
			m_LeftPanel.Width = m_ViewportWidth;
			m_CenterPanel.Width = m_ViewportWidth;
			m_RightPanel.Width = m_ViewportWidth;

			JulianDay panelDate = m_Span.GetStartOfSpan(m_CurrentDate);
			int offsetDays = (int)m_CurrentDate - (int)panelDate;
			double panelLeft = (m_ViewportWidth / 2) - (offsetDays * m_Span.PixelsPerDay(m_ViewportWidth));

			// Set the panel positions
			m_LeftPanel.SetLeft(panelLeft - m_ViewportWidth);
			m_CenterPanel.SetLeft(panelLeft);
			m_RightPanel.SetLeft(panelLeft + m_ViewportWidth);

			// Set the panel start dates, which will update the date labels and ticks
			m_LeftPanel.Start = m_Span.OffsetDate(panelDate, -1);
			m_CenterPanel.Start = panelDate;
			m_RightPanel.Start = m_Span.OffsetDate(panelDate, +1);
		}

		private void SetCurrentDate(JulianDay currentDate)
		{
			if (m_ViewportWidth == 0 || m_Span == null)
				return;

			m_CurrentDate = currentDate;

			JulianDay panelDate = m_Span.GetStartOfSpan(m_CurrentDate);
			int offsetDays = (int)m_CurrentDate - (int)panelDate;
			double panelLeft = (m_ViewportWidth / 2) - (offsetDays * m_Span.PixelsPerDay(m_ViewportWidth));

			double currentLeft = m_CenterPanel.GetLeft();
			double delta = panelLeft - currentLeft;
			if (delta != 0)
			{
				// Set the panel positions
				m_LeftPanel.SetLeft(m_LeftPanel.GetLeft() + delta);
				m_CenterPanel.SetLeft(m_CenterPanel.GetLeft() + delta);
				m_RightPanel.SetLeft(m_RightPanel.GetLeft() + delta);

				// Set the panel start dates, which will update the date labels and ticks
				if (m_CenterPanel.Start != panelDate)
				{
					m_LeftPanel.Start = m_Span.OffsetDate(panelDate, -1);
					m_CenterPanel.Start = panelDate;
					m_RightPanel.Start = m_Span.OffsetDate(panelDate, +1);
				}
			}
#if NOTUSED //j
			// If the center panel is still in view, get out
			if (panelLeft + m_ViewportWidth >= 0 &&
				panelLeft < m_ViewportWidth)
				return;

			int spansRight = 0;
			while (panelLeft + m_ViewportWidth < 0)
			{
				panelLeft += m_ViewportWidth;
				spansRight++;
			}
			int spansLeft = 0;
			while (panelLeft >= m_ViewportWidth)
			{
				panelLeft -= m_ViewportWidth;
				spansLeft++;
			}

			if (spansRight >= 3 || spansLeft >= 3)
			{
				// Set the panel positions
				m_LeftPanel.SetLeft(panelLeft - m_ViewportWidth);
				m_CenterPanel.SetLeft(panelLeft);
				m_RightPanel.SetLeft(panelLeft + m_ViewportWidth);

				// Set the panel start dates, which will update the date labels and ticks
				int spans = (spansRight > 0 ? spansRight : -spansLeft);
				JulianDay startDate = m_Span.OffsetDate(m_CenterPanel.Start, spans);
				m_LeftPanel.Start = m_Span.OffsetDate(startDate, -1);
				m_CenterPanel.Start = startDate;
				m_RightPanel.Start = m_Span.OffsetDate(startDate, +1);
				return;
			}

			// rotate panels to right
			while (--spansRight >= 0)
            {
                // swap panels
				TimeScale saved = m_LeftPanel;
				m_LeftPanel = m_CenterPanel;
				m_CenterPanel = m_RightPanel;
				m_RightPanel = saved;

				// reposition the new right panel to be right of the center panel
				m_RightPanel.SetLeft(m_CenterPanel.GetLeft() + m_ViewportWidth);
				// set the start date on rotated panel, which will update the date labels and ticks
				m_RightPanel.Start = m_Span.OffsetDate(m_CenterPanel.Start, +1);
            }

			// rotate panels to left
			while (--spansLeft >= 0)
			{
                // swap panels
				TimeScale saved = m_RightPanel;
				m_RightPanel = m_CenterPanel;
				m_CenterPanel = m_LeftPanel;
				m_LeftPanel = saved;

				// reposition the new left panel to be left of the center panel
				m_LeftPanel.SetLeft(m_CenterPanel.GetLeft() - m_ViewportWidth);
				// set the start date on rotated panel, which will update the date labels and ticks
				m_LeftPanel.Start = m_Span.OffsetDate(m_CenterPanel.Start, -1);
            }
#endif
		}
	}
}
