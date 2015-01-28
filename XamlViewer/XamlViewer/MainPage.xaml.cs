using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Markup;
using System.Windows.Media;

namespace XamlViewer
{
	public partial class MainPage : UserControl
	{
		private static readonly Brush _okBrush = new SolidColorBrush(Colors.Black);
		private static readonly Brush _errorBrush = new SolidColorBrush(Colors.Red);

		public MainPage()
		{
			InitializeComponent();

			editingTextBox.Text =
				"<UserControl\r\n" +
				"    xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\"\r\n" +
				"    xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\"\r\n" +
				"    xmlns:sdk=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation/sdk\"\r\n" +
				"    xmlns:toolkit=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation/toolkit\"\r\n" +
				">\r\n" +
				"    <TextBlock Text=\"Paste your XAML into the textbox.\"\r\n" +
				"        HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\" TextAlignment=\"Center\"\r\n" +
				"        Foreground=\"Green\" FontSize=\"48\" TextWrapping=\"Wrap\" />\r\n" +
				"</UserControl>\r\n";

			CreateObjectFromXaml(editingTextBox.Text);
		}

		private void OnEditingTextChanged(object sender, TextChangedEventArgs args)
		{
			CreateObjectFromXaml(editingTextBox.Text);
		}

		private void CreateObjectFromXaml(string str)
		{
			object obj;

			try
			{
				obj = XamlReader.Load(str);
			}
			catch (Exception exc)
			{
				statusTextBlock.Text = exc.Message.ToString();
				editingTextBox.Foreground = _errorBrush;
				return;
			}

			if (obj == null)
			{
				statusTextBlock.Text = "NULL object";
				editingTextBox.Foreground = _errorBrush;
				return;
			}

			statusTextBlock.Text = "OK";
			editingTextBox.Foreground = _okBrush;

			if (obj is UIElement)
			{
				destinationElement.Child = obj as UIElement;
			}
			else
			{
				TextBlock txtblk = new TextBlock();
				txtblk.Text = obj.GetType().ToString();
				destinationElement.Child = txtblk;
			}
		}

		private void OnEditingSelectionChanged(object sender, RoutedEventArgs e)
		{
			int index = editingTextBox.SelectionStart;
			string str = GetSelectionText(index);

			int length = editingTextBox.SelectionLength;
			if (length > 0)
				str += " - " + GetSelectionText(index + length);

			positionTextBlock.Text = str;
		}

		private string GetSelectionText(int index)
		{
			int line, col;
			editingTextBox.GetPositionFromIndex(index, out line, out col);
			return string.Format("Line {0} Col {1}", line + 1, col + 1);
		}
	}
}
