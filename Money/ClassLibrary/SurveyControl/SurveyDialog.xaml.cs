using System.Windows;

namespace ClassLibrary
{
	public partial class SurveyDialog : DialogPanel
	{
		private SurveyPanels m_SurveyPanels;

		public SurveyDialog(string surveyFile, string title)
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_Next);
			base.Loaded += OnLoaded;

			x_Title.Text = title;
			m_SurveyPanels = new SurveyPanels(x_SurveyPanel, surveyFile, null);
			ActivatePanel(0);
		}

		public override void Dispose()
		{
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}

		private void OnBackClick(object sender, RoutedEventArgs e)
		{
			ActivatePanel(-1);
		}

		private void OnNextClick(object sender, RoutedEventArgs e)
		{
			ActivatePanel(+1);
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		private void ActivatePanel(int iPanelIncrement)
		{
			m_SurveyPanels.ActivatePanel(iPanelIncrement);
			x_Back.Content = m_SurveyPanels.BackText;
			x_Next.Content = m_SurveyPanels.NextText;
			x_Back.Opacity = (m_SurveyPanels.BackText.Length == 0 ? 0 : 1);
			x_Next.Opacity = (m_SurveyPanels.NextText.Length == 0 ? 0 : 1);
		}
	}
}
