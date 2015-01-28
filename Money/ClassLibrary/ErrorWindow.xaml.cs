using System;
using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	public partial class ErrorWindow : ChildWindow
	{
		public ErrorWindow(Exception ex)
		{
			InitializeComponent();
			if (ex != null)
				ErrorTextBox.Text = ex.Details();
		}

		public ErrorWindow(Uri uri)
		{
			InitializeComponent();
			if (uri != null)
				ErrorTextBox.Text = "Page not found: \"" + uri.ToString() + "\"";
		}

		public ErrorWindow(string message, string details)
		{
			InitializeComponent();
			ErrorTextBox.Text = message + Environment.NewLine + Environment.NewLine + details;
		}

		private void OKButton_Click(object sender, RoutedEventArgs e)
		{
			this.DialogResult = true;
		}
	}
}
