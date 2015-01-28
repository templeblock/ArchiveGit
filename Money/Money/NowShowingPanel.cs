using System.Windows;
using ClassLibrary;

namespace Money
{
	public enum NowShowingViewAs
	{
		Grid,
		Chart,
//j		Timeline,
	}

	public class NowShowingPanel : StretchPanel
	{
		private NowShowingViewAs m_NowShowingViewAs;
		public NowShowingViewAs NowShowingViewAs { get { return m_NowShowingViewAs; } set { m_NowShowingViewAs = SetNowShowingViewAs(value); } }

		public NowShowingPanel()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			base.DataContext = this;
			NowShowingViewAs = NowShowingViewAs.Grid;
		}

		private NowShowingViewAs SetNowShowingViewAs(NowShowingViewAs nowShowingViewAs)
		{
			m_NowShowingViewAs = nowShowingViewAs;
			TriSelector selector = App.MainPage.x_PlanEventsSelector;
			if (selector == null)
				return m_NowShowingViewAs;

			if (m_NowShowingViewAs == NowShowingViewAs.Grid)
				selector.Value = "A";
			else
			if (m_NowShowingViewAs == NowShowingViewAs.Chart)
				selector.Value = "B";
#if NOTUSED //j
			else
			if (m_NowShowingViewAs == NowShowingViewAs.Timeline)
				selector.Value = "C";
#endif
			return m_NowShowingViewAs;
		}
	}
}
