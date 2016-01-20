using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using System.Collections.Generic;

namespace ClassLibrary
{
	public static class MessageBoxEx
	{
		public static void Show(string title, string text, EventHandler closeHandler)
		{
			MessageBox.Show(text, title, MessageBoxButton.OK);
//j			new MessageBoxDialog(title, text, closeHandler);
		}
	}

	public partial class MessageBoxDialog : UserControl, IDialogPanel
	{
		// Implement IDialogPanel
		private DialogPanelImpl m_DialogPanelImpl = new DialogPanelImpl();
		public void DialogPanelInit(FrameworkElement panel, bool bModal, EventHandler closeHandler) { m_DialogPanelImpl.DialogPanelInit(panel, bModal, closeHandler); }
		public void DialogPanelShow(bool bShow) { m_DialogPanelImpl.DialogPanelShow(bShow); }

		private MessageBoxResult m_DialogResult;
		public MessageBoxResult Result { get { return m_DialogResult; } }

		public MessageBoxDialog(string title, string text, EventHandler closeHandler)
		{
			base.Loaded += Page_Loaded;

			// Required to create class member variables from named XAML elements
			InitializeComponent();

			x_Title.Text = title;
			x_Text.Text = text;
			x_Cancel.Visibility = Visibility.Collapsed;

			DialogPanelInit(this, true/*bModal*/, closeHandler);
		}

		public void Page_Loaded(object sender, EventArgs args)
		{
		}

		private void OkButton_Click(object sender, EventArgs args)
		{
			m_DialogResult = MessageBoxResult.OK;
			DialogPanelShow(false);
		}

		private void CancelButton_Click(object sender, EventArgs args)
		{
			m_DialogResult = MessageBoxResult.Cancel;
			DialogPanelShow(false);
		}
	}
}
