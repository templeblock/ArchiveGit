using System.Windows.Controls;

namespace ClassLibrary
{
	internal class QuestionChooseAny : QuestionControl
	{
		public QuestionChooseAny(QuestionBase question, Panel panel)
			: base(question, panel)
		{
			if (question.Hidden)
				return;

			StackPanel responsePanel = base.ResponsePanel;

			ListBox listBox = null;
			if (question.List)
			{
				listBox = new ListBox();
				responsePanel.Children.Add(listBox);
				listBox.ApplyStyle(panel, "SurveyListBox");
			}

			foreach (string text in question.ResponseList)
			{
				if (question.List)
				{
					ListBoxItem item = new ListBoxItem();
					listBox.Items.Add(item);
					item.ApplyStyle(panel, "SurveyListBoxItem");
					item.Content = text;
					if (text.EqualsIgnoreCase(question.ResponseDefault))
						item.IsSelected = true;
				}
				else
				{
					CheckBox checkBox = new CheckBox();
					responsePanel.Children.Add(checkBox);
					checkBox.ApplyStyle(panel, "SurveyCheckBox");
					checkBox.Content = text;
					if (text.EqualsIgnoreCase(question.ResponseDefault))
						checkBox.IsChecked = true;
				}
			}
		}

		public override void SetAnswer()
		{
			base.Answer = "";
			StackPanel responsePanel = base.ResponsePanel;
			foreach (CheckBox checkbox in responsePanel.Children)
			{
				bool? bChecked = checkbox.IsChecked;
				if (bChecked == false)
					continue;
				if (!base.Answer.IsEmpty())
					base.Answer += "|";
				base.Answer += checkbox.IsChecked.ToString();
			}
		}
	}
}