using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	internal class QuestionText : QuestionControl
	{
		private readonly TextBox m_textBox = new TextBox();

		public QuestionText(QuestionBase question, Panel panel)
			: base(question, panel)
		{
			if (question.Hidden)
				return;

			StackPanel responsePanel = base.ResponsePanel;

			bool bMultiLine = (question.Rows > 1);

			m_textBox.VerticalScrollBarVisibility = ScrollBarVisibility.Auto;
			responsePanel.Children.Add(m_textBox);
			m_textBox.ApplyStyle(panel, "SurveyTextBox");
			m_textBox.Tag = question.Name;
			m_textBox.Text = " "; // To force the control to have assigned height
//j			if (question.Required)
//j				m_textBox.Watermark = "<Required>";
			m_textBox.AcceptsReturn = bMultiLine;
			m_textBox.TextWrapping = (bMultiLine ? TextWrapping.Wrap : TextWrapping.NoWrap);
			m_textBox.Width = question.Cols * 8 + 12;
			m_textBox.Height = question.Rows * 12 + 12;
			m_textBox.Text = question.ResponseDefault;
			m_textBox.MaxLength = question.MaxChars;
			m_textBox.HorizontalAlignment = HorizontalAlignment.Left;
#if NOTUSED
			if (base.Question.Required)
			{
				RequiredFieldValidator child = new RequiredFieldValidator();
				child.ControlToValidate = m_textBox.Tag;
				child.EnableClientScript = false;
				child.Text = base.Question.RequiredText;
				responsePanel.Children.Add(child);
				responsePanel.Children.Add(new LiteralControl("<br>"));
			}
#endif
		}

		public override void SetAnswer()
		{
			base.Answer = m_textBox.Text;
		}
	}

	internal class QuestionPassword : QuestionControl
	{
		private readonly PasswordBox m_PasswordBox = new PasswordBox();

		public QuestionPassword(QuestionBase question, Panel panel)
			: base(question, panel)
		{
			if (question.Hidden)
				return;

			StackPanel responsePanel = base.ResponsePanel;

			responsePanel.Children.Add(m_PasswordBox);
			m_PasswordBox.ApplyStyle(panel, "SurveyPasswordBox");
			m_PasswordBox.Tag = question.Name;
			m_PasswordBox.Width = question.Cols * 8 + 12;
			m_PasswordBox.Height = question.Rows * 12 + 12;
			m_PasswordBox.Password = question.ResponseDefault;
			m_PasswordBox.MaxLength = question.MaxChars;
			m_PasswordBox.HorizontalAlignment = HorizontalAlignment.Left;
		}

		public override void SetAnswer()
		{
			base.Answer = m_PasswordBox.Password;
		}
	}

	internal class QuestionDate : QuestionControl
	{
		private readonly DatePicker m_DatePicker = new DatePicker();

		public QuestionDate(QuestionBase question, Panel panel)
			: base(question, panel)
		{
			if (question.Hidden)
				return;

			StackPanel responsePanel = base.ResponsePanel;

			responsePanel.Children.Add(m_DatePicker);
			m_DatePicker.ApplyStyle(panel, "SurveyDatePicker");
			m_DatePicker.Tag = question.Name;
			m_DatePicker.Width = question.Cols * 8 + 12;
			m_DatePicker.Height = question.Rows * 12 + 12;
			m_DatePicker.Text = question.ResponseDefault;
			m_DatePicker.HorizontalAlignment = HorizontalAlignment.Left;
		}

		public override void SetAnswer()
		{
			base.Answer = m_DatePicker.Text;
		}
	}
}