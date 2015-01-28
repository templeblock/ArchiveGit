using System;
using System.Windows;
using ClassLibrary;

namespace Money
{
	internal class DebugFunctionality
	{
		internal static void OnOpenDocumentISOClick(object sender, RoutedEventArgs e)
		{
			if (!IsolatedFileHelper.IsStorageAllowed())
				return;

			OpenSaveDialog openDialog = new OpenSaveDialog(App.Model.DocumentHolder.CurrentDocumentFileName,  ResxResources.DefaultDocumentType, true/*bOpen*/);
			openDialog.Closed += delegate(object senderDialog, EventArgs eDialog)
			{
				OpenSaveDialog dialog = senderDialog as OpenSaveDialog;
				if (dialog == null)
					return;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				string filePath = dialog.OpenFilePathXml;
				if (!App.Model.DocumentHolder.Open(filePath))
					MessageBoxEx.ShowError("Open document", string.Format("Cannot open document '{0}'", filePath), null);
			};
		}

		internal static void OnOpenDocumentFileClick(object sender, RoutedEventArgs e)
		{
			string filter = string.Format(
				"YinYangMoney Plans|*.{0}|XML Files|*.xml|All Files (*.*)|*.*", ResxResources.DefaultDocumentType);
			FileInfoHelper fileInfo = FileInfoHelper.OpenDialog(filter);
			if (fileInfo == null)
				return;

			if (!App.Model.DocumentHolder.Open(fileInfo))
				MessageBoxEx.ShowError("Open document", string.Format("Cannot open document '{0}'", fileInfo.Name), null);
		}

		internal static void OnSaveDocumentISOClick(object sender, RoutedEventArgs e)
		{
			if (!IsolatedFileHelper.IsStorageAllowed())
				return;

			OpenSaveDialog saveDialog = new OpenSaveDialog(App.Model.DocumentHolder.CurrentDocumentFileName, ResxResources.DefaultDocumentType, false/*bOpen*/);
			saveDialog.Closed += delegate(object senderDialog, EventArgs eDialog)
			{
				OpenSaveDialog dialog = senderDialog as OpenSaveDialog;
				if (dialog == null)
					return;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				string filePath = dialog.SaveFilePathXml;
				if (!App.Model.DocumentHolder.Save(filePath))
					MessageBoxEx.ShowError("Save document", string.Format("Cannot save document '{0}'", filePath), null);
			};
		}

		internal static void OnCreateDocumentClick(object sender, RoutedEventArgs e)
		{
			// Create a empty document
			bool bOK = App.Model.DocumentHolder.Open(null/*empty*/);
		}

		internal static void OnCreateDummyDocumentClick(object sender, RoutedEventArgs e)
		{
			// Create a empty document
			bool bOK = App.Model.DocumentHolder.Open("Sample");
		}

		internal static void OnCreateDatabasePackageClick(object sender, RoutedEventArgs e)
		{
			new DatabasePackageDialog(null);
		}

#if NOTUSED //j
		internal static void OnSurveyClick(object sender, RoutedEventArgs e)
		{
			SurveyDialog dialog = new SurveyDialog("Content/Survey.xml", "My Survey");
			dialog.Closed += SurveyDialogClosed;
		}

		// Raised when the SurveyDialog is closed
		private static void SurveyDialogClosed(object sender, EventArgs e)
		{
			SurveyDialog dialog = sender as SurveyDialog;
			if (dialog == null)
				return;
			if (dialog.DialogResult == MessageBoxResult.Cancel)
				return;
		}
#endif

		internal static void OnOpenProfileISOClick(object sender, RoutedEventArgs e)
		{
			if (!IsolatedFileHelper.IsStorageAllowed())
				return;

			OpenSaveDialog openDialog = new OpenSaveDialog(ResxResources.DefaultProfileFileName, ResxResources.DefaultProfileType, true/*bOpen*/);
			openDialog.Closed += delegate(object senderDialog, EventArgs eDialog)
			{
				OpenSaveDialog dialog = senderDialog as OpenSaveDialog;
				if (dialog == null)
					return;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				string filePath = dialog.OpenFilePathXml;
				if (!App.Model.ProfileHolder.Open(filePath))
					MessageBoxEx.ShowError("Open profile", string.Format("Cannot open profile '{0}'", filePath), null);
			};
		}

		internal static void OnOpenProfileFileClick(object sender, RoutedEventArgs e)
		{
			string filter = string.Format(
				"YinYangMoney Profiles|*.{0}|XML Files|*.xml|All Files (*.*)|*.*", ResxResources.DefaultProfileType);
			FileInfoHelper fileInfo = FileInfoHelper.OpenDialog(filter);
			if (fileInfo == null)
				return;

			if (!App.Model.ProfileHolder.Open(fileInfo))
				MessageBoxEx.ShowError("Open profile", string.Format("Cannot open profile '{0}'", fileInfo.Name), null);
		}

		internal static void OnSaveProfileISOClick(object sender, RoutedEventArgs e)
		{
			//if (App.Model.ProfileHolder.Profile == null)
			//{
			//	MessageBoxEx.ShowOK("Save profile", "A profile has not been created", null);
			//	return;
			//}

			if (!IsolatedFileHelper.IsStorageAllowed())
				return;

			bool bOK = IsolatedFileHelper.IncreaseQuotaTo(10 * 1024 * 1024);

			OpenSaveDialog saveDialog = new OpenSaveDialog(ResxResources.DefaultProfileFileName, ResxResources.DefaultProfileType, false/*bOpen*/);
			saveDialog.Closed += delegate(object senderDialog, EventArgs eDialog)
			{
				OpenSaveDialog dialog = senderDialog as OpenSaveDialog;
				if (dialog == null)
					return;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				string filePath = dialog.SaveFilePathXml;
				if (!App.Model.ProfileHolder.Save(filePath))
					MessageBoxEx.ShowError("Save profile", string.Format("Cannot save profile '{0}'", filePath), null);
			};
		}

		internal static void OnCreateProfileClick(object sender, RoutedEventArgs e)
		{
			// Create a empty profile
			bool bOK = App.Model.ProfileHolder.Open(null/*empty*/);
		}
	}
}
