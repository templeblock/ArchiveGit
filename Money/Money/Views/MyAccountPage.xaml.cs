using System;
using System.ServiceModel.DomainServices.Client.ApplicationServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class MyAccountPage : Page
	{
		public MyAccountPage()
		{
			InitializeComponent();
			HtmlTextBlock content = this.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}

		private void OnUpdateSettingsClick(object sender, RoutedEventArgs e)
		{
			if (!App.Model.UserServices.User.IsAuthenticated)
			{
				MessageBoxEx.ShowWarning("Update account settings", "You are not signed in", null);
				return;
			}

			App.Model.UserServices.SaveUser(OnUpdateSettingsCompleted, null/*userState*/);
		}

		private void OnUpdateSettingsCompleted(SaveUserOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				MessageBoxEx.ShowError("Update account settings", errorStatus, null);
				return;
			}

			MessageBoxEx.ShowInfo("Update account settings", "Your settings have been updated", null);
		}

		internal void OnUpgradePremiumClick(object sender, RoutedEventArgs e)
		{
			if (!App.Model.UserServices.User.IsAuthenticated)
			{
				MessageBoxEx.ShowWarning("Upgrade to premium", "You are not signed in", null);
				return;
			}

			if (App.Model.UserServices.User.MemberIsPremium)
			{
				MessageBoxEx.ShowInfo("Upgrade to premium", "You are already a Premium member", null);
				return;
			}

			App.Model.UserServices.CheckOKtoSetProperty();
			App.Model.UserServices.User.MemberIsPremium = true;
			App.Model.UserServices.SaveUser(OnUpgradeAccountCompleted, null/*userState*/);
		}

		private static void OnUpgradeAccountCompleted(SaveUserOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				MessageBoxEx.ShowError("Upgrade to premium", errorStatus, null);
				return;
			}

			MessageBoxEx.ShowInfo("Upgrade to premium", "You have been upgraded to a Premium member", null);
		}

		private void OnCancelPremiumClick(object sender, RoutedEventArgs e)
		{
			if (!App.Model.UserServices.User.IsAuthenticated)
			{
				MessageBoxEx.ShowWarning("Cancel premium membership", "You are not signed in", null);
				return;
			}

			if (!App.Model.UserServices.User.MemberIsPremium)
			{
				MessageBoxEx.ShowWarning("Cancel premium membership", "You are not a Premium member", null);
				return;
			}

			App.Model.UserServices.CheckOKtoSetProperty();
			App.Model.UserServices.User.MemberIsPremium = false;
			App.Model.UserServices.SaveUser(OnCancelPremiumCompleted, null/*userState*/);
		}

		private void OnCancelPremiumCompleted(SaveUserOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				MessageBoxEx.ShowError("Cancel premium membership", errorStatus, null);
				return;
			}

			MessageBoxEx.ShowInfo("Cancel premium membership", "Your Premium membership has been cancelled", null);
		}

		private void OnChangePasswordClick(object sender, RoutedEventArgs e)
		{
			if (!App.Model.UserServices.User.IsAuthenticated)
			{
				MessageBoxEx.ShowWarning("Change password", "You are not signed in", null);
				return;
			}

			AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.ChangePassword);
			dialog.Closed += ChangePasswordDialogClosed;
		}

		private void ChangePasswordDialogClosed(object sender, EventArgs e)
		{
			AuthenticationDialog dialog = sender as AuthenticationDialog;
			if (dialog == null)
				return;
			if (dialog.DialogResult == MessageBoxResult.Cancel)
				return;
		}

		private void OnDeleteAccountClick(object sender, RoutedEventArgs e)
		{
			if (!App.Model.UserServices.User.IsAuthenticated)
			{
				MessageBoxEx.ShowWarning("Delete account", "You are not signed in", null);
				return;
			}

			AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.DeleteAccount);
			dialog.Closed += DeleteAccountDialogClosed;
		}

		private void DeleteAccountDialogClosed(object sender, EventArgs e)
		{
			AuthenticationDialog dialog = sender as AuthenticationDialog;
			if (dialog == null)
				return;
			if (dialog.DialogResult == MessageBoxResult.Cancel)
				return;
		}
	}
}
