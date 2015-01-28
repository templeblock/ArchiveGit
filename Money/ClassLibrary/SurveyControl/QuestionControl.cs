using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace ClassLibrary
{
	internal class QuestionControl
	{
		public QuestionBase Question { get { return m_question; } }
		private readonly QuestionBase m_question;

		public string Answer { get { return m_Answer; } set { m_Answer = value; } }
		private string m_Answer;

		public StackPanel StatementPanel { get { return m_StatementPanel; } }
		private readonly StackPanel m_StatementPanel = new StackPanel();

		public StackPanel ResponsePanel { get { return m_ResponsePanel; } }
		private readonly StackPanel m_ResponsePanel = new StackPanel();

		public string Id { get { return m_question.Name; } }

		public QuestionControl(QuestionBase question, Panel panel)
		{
			m_question = question;
			if (m_question.Hidden)
				return;

			// Initialize the statement panel
			panel.Children.Add(m_StatementPanel);
			m_StatementPanel.Orientation = Orientation.Horizontal;

			int nIndent = question.Indent;
			if (nIndent > 0 || m_question.Image.Length != 0)
			{
				if (m_question.Image.Length != 0)
				{
					BitmapImage bm = new BitmapImage(); 
					bm.UriSource = UriHelper.UriAppRelative(m_question.Image);

					Image image = new Image(); 
					m_StatementPanel.Children.Add(image);
					image.ApplyStyle(panel, "SurveyImage");
					image.Source = bm;
					image.Stretch = Stretch.None;
					image.VerticalAlignment = VerticalAlignment.Top;
					image.HorizontalAlignment = HorizontalAlignment.Center;
					if (Double.IsNaN(image.Width))
						image.Width = 0;
					if (nIndent < image.Width)
						nIndent = (int)image.Width;
					if (nIndent != 0 && image.Width < nIndent)
						image.Width = nIndent;
				}
				else
				{
					StackPanel spacerPanel = new StackPanel();
					m_StatementPanel.Children.Add(spacerPanel);
					spacerPanel.Width = nIndent;
				}
			}

			RichTextBlock textBlock = new RichTextBlock();
			m_StatementPanel.Children.Add(textBlock);
			textBlock.MaxWidth = 700;
			textBlock.TextWrapping = TextWrapping.Wrap;
			textBlock.ApplyStyle(panel, "SurveyStatement");
			textBlock.SetHtml(m_question.Statement);

			// Initialize the response panel
			panel.Children.Add(m_ResponsePanel);
			m_ResponsePanel.Tag = question.Name;
			m_ResponsePanel.Orientation = question.ResponseLayoutDirection;
			m_ResponsePanel.HorizontalAlignment = HorizontalAlignment.Left;
			
			if (nIndent > 0)
			{
				StackPanel spacerPanel = new StackPanel();
				m_ResponsePanel.Children.Add(spacerPanel);
				spacerPanel.Width = nIndent;
			}
		}

		public virtual void SetAnswer()
		{
		}
	}
}