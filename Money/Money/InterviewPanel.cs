using System;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;
using ClassLibrary.SurveyControl;

namespace Money
{
	public class InterviewPanel : GridPanel
	{
		private SurveyPanels m_SurveyPanels;

		public InterviewPanel()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			Panel surveyPanel = this.ChildOfTypeAndName<Panel>("x_SurveyPanel");
			m_SurveyPanels = new SurveyPanels(surveyPanel, "Resources/Profile.xml", null);
			ActivatePanel(0);
		}

		public void OnProfileBackClick(object sender, RoutedEventArgs e)
		{
			ActivatePanel(-1);
		}

		public void OnProfileNextClick(object sender, RoutedEventArgs e)
		{
			ActivatePanel(+1);
		}

		private void ActivatePanel(int iPanelIncrement)
		{
			m_SurveyPanels.ActivatePanel(iPanelIncrement);

			Button backButton = this.ChildOfTypeAndName<Button>("x_ProfileBackButton");
			backButton.Content = m_SurveyPanels.BackText;
			backButton.Opacity = (m_SurveyPanels.BackText.Length == 0 ? 0 : 1);
			Button nextButton = this.ChildOfTypeAndName<Button>("x_ProfileNextButton");
			nextButton.Content = m_SurveyPanels.NextText;
			nextButton.Opacity = (m_SurveyPanels.NextText.Length == 0 ? 0 : 1);
		}
	}
}
