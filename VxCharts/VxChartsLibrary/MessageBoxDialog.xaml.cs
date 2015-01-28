using System;
using System.Windows;
using System.Windows.Media.Imaging;

namespace ClassLibrary
{
	public static class MessageBoxEx
	{
		public static void ShowInfo(string title, string text, EventHandler closeHandler)
		{
			new MessageBoxDialog(title, text, MessageBoxButtons.OK, MessageBoxIcon.Info, closeHandler);
		}

		public static void ShowError(string title, string text, EventHandler closeHandler)
		{
			new MessageBoxDialog(title, text, MessageBoxButtons.OK, MessageBoxIcon.Error, closeHandler);
		}

		public static void ShowWarning(string title, string text, EventHandler closeHandler)
		{
			new MessageBoxDialog(title, text, MessageBoxButtons.OK, MessageBoxIcon.Warning, closeHandler);
		}

		public static void ShowOKCancel(string title, string text, MessageBoxIcon icon, EventHandler closeHandler)
		{
			new MessageBoxDialog(title, text, MessageBoxButtons.OKCancel, icon, closeHandler);
		}

		public static void ShowYesNo(string title, string text, MessageBoxIcon icon, EventHandler closeHandler)
		{
			new MessageBoxDialog(title, text, MessageBoxButtons.YesNo, icon, closeHandler);
		}

		public static void ShowYesNoCancel(string title, string text, MessageBoxIcon icon, EventHandler closeHandler)
		{
			new MessageBoxDialog(title, text, MessageBoxButtons.YesNoCancel, icon, closeHandler);
		}
	}

	public enum MessageBoxButtons
	{
		OK,
		OKCancel,
		YesNo,
		YesNoCancel,
	}

	public enum MessageBoxIcon
	{
		None,
		Info,
		Error,
		Warning,
		Question,
	}

	public partial class MessageBoxDialog : DialogPanel
	{
		private EventHandler m_CloseHandler;

		public MessageBoxDialog(string title, string text, MessageBoxButtons buttons, MessageBoxIcon icon, EventHandler closeHandler)
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_OK);
			base.Loaded += OnLoaded;
			m_CloseHandler = closeHandler;
			if (m_CloseHandler != null)
				base.Closed += m_CloseHandler;

			x_Title.Text = title;
			x_Text.Text = text;
			x_OK.SetVisible(buttons == MessageBoxButtons.OK || buttons == MessageBoxButtons.OKCancel);
			x_Cancel.SetVisible(buttons == MessageBoxButtons.OKCancel || buttons == MessageBoxButtons.YesNoCancel);
			x_Yes.SetVisible(buttons == MessageBoxButtons.YesNo || buttons == MessageBoxButtons.YesNoCancel);
			x_No.SetVisible(buttons == MessageBoxButtons.YesNo || buttons == MessageBoxButtons.YesNoCancel);

			x_Icon.SetVisible(icon != MessageBoxIcon.None);
			if (icon != MessageBoxIcon.None)
				x_Icon.Source = GetBitmapImage(icon);
		}

		public override void Dispose()
		{
			if (m_CloseHandler != null)
				base.Closed -= m_CloseHandler;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}

		private void OnOkClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.OK;
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		private void OnYesClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Yes;
		}

		private void OnNoClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.No;
		}

		private static BitmapImage GetBitmapImage(MessageBoxIcon icon)
		{
			string name = null;
			if (icon == MessageBoxIcon.Info)		name = "Info"; else
			if (icon == MessageBoxIcon.Error)		name = "Error"; else
			if (icon == MessageBoxIcon.Warning)		name = "Warning"; else
			if (icon == MessageBoxIcon.Question)	name = "Question";

			return ResourceFile.GetBitmap("Images/MessageBox" + name + ".png", From.This);
		}
	}
}
