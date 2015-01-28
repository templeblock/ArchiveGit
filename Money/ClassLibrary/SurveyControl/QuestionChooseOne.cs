using System.Windows.Controls;

namespace ClassLibrary
{
	internal class QuestionChooseOne : QuestionControl
	{
		public QuestionChooseOne(QuestionBase question, Panel panel)
			: base(question, panel)
		{
			if (question.Hidden)
				return;

			StackPanel responsePanel = base.ResponsePanel;

			ComboBox comboBox = null;
			if (question.List)
			{
				comboBox = new ComboBox();
				responsePanel.Children.Add(comboBox);
				comboBox.ApplyStyle(panel, "SurveyComboBox");
			}

			foreach (string text in question.ResponseList)
			{
				if (question.List)
				{
					ComboBoxItem item = new ComboBoxItem();
					comboBox.Items.Add(item);
					item.ApplyStyle(panel, "SurveyComboBoxItem");
					item.Content = text;
					if (text.EqualsIgnoreCase(question.ResponseDefault))
						item.IsSelected = true;
				}
				else
				{
					RadioButton button = new RadioButton();
					responsePanel.Children.Add(button);
					button.ApplyStyle(panel, "SurveyRadioButton");
					button.Content = text;
					if (text.EqualsIgnoreCase(question.ResponseDefault))
						button.IsChecked = true;
				}
			}
		}

		public override void SetAnswer()
		{
			base.Answer = "";
			StackPanel responsePanel = base.ResponsePanel;
			if (base.Question.List)
			{
				ComboBox comboBox = responsePanel.Children[0] as ComboBox;
				foreach (ComboBoxItem item in comboBox.Items)
				{
					bool bChecked = item.IsSelected;
					if (!bChecked)
						continue;
					if (!base.Answer.IsEmpty())
						base.Answer += "|";
					base.Answer += item.IsSelected.ToString();
				}
			}
			else
			{
				foreach (RadioButton radioButton in responsePanel.Children)
				{
					bool? bChecked = radioButton.IsChecked;
					if (bChecked == false)
						continue;
					if (!base.Answer.IsEmpty())
						base.Answer += "|";
					base.Answer += radioButton.IsChecked.ToString();
				}
			}
		}
	}

	internal class QuestionYesNo : QuestionChooseOne
	{
		static readonly string[] m_Responses = { "Yes", "No" };

		public QuestionYesNo(QuestionBase question, Panel panel)
			: base(question.SetResponses(m_Responses), panel)
		{
		}
	}

	internal class QuestionState : QuestionChooseOne
	{
		static readonly string[] m_Responses = { "AK", "HI" , "MA" };

		public QuestionState(QuestionBase question, Panel panel)
			: base(question.SetResponses(m_Responses), panel)
		{
		}
	}
}