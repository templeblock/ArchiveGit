using System;
using System.Windows.Controls;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public partial class TimeScale : UserControl
	{
		private Span m_Span;
		internal Span Span { get { return m_Span; } set { m_Span = value; Render(); } }

		private JulianDay m_Start;
		public JulianDay Start { get { return m_Start; } set { m_Start = value; Render(); } }

		private int m_Intervals;

		public TimeScale()
		{
			InitializeComponent();
		}

		private void Render()
		{
			if (m_Span == null || m_Start == 0)
				return;

			x_StartDate.Text = m_Span.GetLabel(m_Start);

			// If the intervals and text labels are always the same, there is nothing to do...
			int intervals = m_Span.GetIntervals(m_Start);
			if (m_Intervals == intervals && (m_Span.Unit == SpanUnit.Week || m_Span.Unit == SpanUnit.Month || m_Span.Unit == SpanUnit.Year))
				return;

			Grid tickMarkGrid = x_TickMarkGrid;
			if (m_Intervals != intervals)
			{
				// If we have tick marks to remove...
				for (int i = m_Intervals-1; i >= intervals; i--)
				{
					tickMarkGrid.ColumnDefinitions.RemoveAt(i);
					tickMarkGrid.Children.RemoveAt(i);
				}

				// If we have tick marks to add...
				for (int i = m_Intervals; i < intervals; i++)
				{
					tickMarkGrid.ColumnDefinitions.Add(new ColumnDefinition());
					TickMark tm = new TickMark();
					Grid.SetColumn(tm, i);
					tickMarkGrid.Children.Add(tm);
				}

				m_Intervals = intervals;
			}

			// Set all the text labels
			for (int i = 0; i < intervals; i++)
			{
				TickMark tm = tickMarkGrid.Children[i] as TickMark;
				if (tm != null)
					tm.Label = m_Span.GetText(m_Start, i);
			}
		}
	}
}
