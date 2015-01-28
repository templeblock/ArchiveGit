using System;
using System.ServiceModel.DomainServices.Client;
using System.ServiceModel.DomainServices.Client.ApplicationServices;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace Pivot
{
	public partial class AuthenticationDialog : DialogPanel
	{
		private const string g_TryAgainMessage = "User name or password is incorrect.";
		private const string g_AccountNotDeleted = "Your account was NOT deleted.";
		private const string g_PasswordNotChanged = "Your password was NOT changed";

		private string SignInUserName { get { return x_SignInUserName.Text; } set { x_SignInUserName.Text = value; } }
		private string SignInPassword { get { return x_SignInPassword.Password; } set { x_SignInPassword.Password = value; } }

		private string RegisterUserName { get { return x_RegisterUserName.Text; } set { x_RegisterUserName.Text = value; } }
		private string RegisterPassword { get { return x_RegisterPassword.Password; } set { x_RegisterPassword.Password = value; } }
		private string RegisterEmail { get { return x_RegisterEmail.Text; } set { x_RegisterEmail.Text = value; } }

		private string RecoverEmail { get { return x_RecoverEmail.Text; } set { x_RecoverEmail.Text = value; } }
		public string RecoverErrorStatus { get { return x_RecoverErrorStatus.Text; } set { x_RecoverErrorStatus.Text = value; x_RecoverErrorStatus.Visibility = Visibility.Visible; } }

		public string ResetUserName { get { return x_ResetUserName.Text; } set { x_ResetUserName.Text = value; } }
		private string ResetPassword { get { return x_ResetPassword.Password; } set { x_ResetPassword.Password = value; } }

		public string ResetPasswordCode { get { return m_ResetPasswordCode; } set { m_ResetPasswordCode = value; } }
		private string m_ResetPasswordCode;

		private Control m_FocusControl = null;

		public enum DialogType
		{
			SignIn,
			Register,
			RecoverSignIn,
			ResetPassword,
			ChangePassword,
			DeleteAccount,
		}

		public AuthenticationDialog(DialogType dialogType)
		{
			InitializeComponent();
			base.Loaded += OnLoaded;

			SignInUserName = (App.Model.UserServices.LastUserName != null ? App.Model.UserServices.LastUserName : "");

			switch (dialogType)
			{
				case DialogType.SignIn:
					OnSwitchToSignInPanelClick(null, null); break;
				case DialogType.Register:
					OnSwitchToRegisterPanelClick(null, null); break;
				case DialogType.RecoverSignIn:
					OnSwitchToRecoverPanelClick(null, null); break;
				case DialogType.ResetPassword:
					OnSwitchToResetPasswordPanelClick(null, null); break;
				case DialogType.ChangePassword:
					OnSwitchToChangePanelClick(null, null); break;
				case DialogType.DeleteAccount:
					OnSwitchToDeletePanelClick(null, null); break;
			}

			// m_FocusControl set during initialization above
			InitializeDialogPanel(true/*bModal*/, m_FocusControl);
		}

		public override void Dispose()
		{
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}

		private void OnEnterKeyDown(object sender, RoutedEventArgs e)
		{
			if (x_SignInPanel.IsVisible())
				OnSigninClick(sender, e);
			else
			if (x_RegisterPanel.IsVisible())
				OnRegisterClick(sender, e);
			else
			if (x_RecoverPanel.IsVisible())
				OnRecoverClick(sender, e);
			else
			if (x_ResetPasswordPanel.IsVisible())
				OnResetPasswordClick(sender, e);
			else
			if (x_ChangePanel.IsVisible())
				OnChangeClick(sender, e);
		}

		private void OnSigninClick(object sender, RoutedEventArgs e)
		{
			x_Signin.IsEnabled = false;
			if (SignInPassword == "deletethis")
				App.Model.UserServices.DeleteUser(SignInUserName, true/*deleteAllRelatedData*/, OnDeleteUserCompleted, null/*userState*/);
			else
				App.Model.UserServices.Login(SignInUserName, SignInPassword, true/*isPersistent*/, OnSigninCompleted, null/*userState*/);
		}

		private void OnDeleteUserCompleted(InvokeOperation<bool> operation)
		{
			x_Signin.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			bool deleted = (errorStatus == null && operation.Value == true);
			x_SignInErrorStatus.Text = string.Format("User account '{0}' was{1} deleted", SignInUserName, (deleted ? "" : " NOT"));
			x_SignInErrorStatus.Visibility = Visibility.Visible;
		}

		private void OnSigninCompleted(LoginOperation operation)
		{
			x_Signin.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null || !operation.LoginSuccess)
			{
				x_SignInErrorStatus.Text = (errorStatus != null ? errorStatus : g_TryAgainMessage);
				x_SignInErrorStatus.Visibility = Visibility.Visible;
				return;
			}

			x_SignInErrorStatus.Text = null;
			x_SignInErrorStatus.Visibility = Visibility.Collapsed;
			DialogResult = MessageBoxResult.OK;
		}

		private void OnRegisterClick(object sender, RoutedEventArgs e)
		{
			x_Register.IsEnabled = false;
			App.Model.UserServices.CreateUser(RegisterUserName, RegisterPassword, RegisterEmail, null/*roleName*/, OnRegisterCompleted, null/*userState*/);
		}

		private void OnRegisterCompleted(InvokeOperation operation)
		{
			x_Register.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				x_RegisterErrorStatus.Text = errorStatus;
				x_RegisterErrorStatus.Visibility = Visibility.Visible;
				return;
			}

			App.Model.UserServices.Login(RegisterUserName, RegisterPassword, true/*isPersistent*/, OnRegisterCompletedStep2, null/*userState*/);

			x_RegisterErrorStatus.Text = null;
			x_RegisterErrorStatus.Visibility = Visibility.Collapsed;
			DialogResult = MessageBoxResult.OK;
		}

		private void OnRegisterCompletedStep2(LoginOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null || !operation.LoginSuccess)
			{
				MessageBoxEx.ShowError("Register account", errorStatus, null);
				return;
			}

			App.Model.UserServices.CheckOKtoSetProperty();
			App.Model.UserServices.User.OptInUpdates = (bool)x_OptInUpdates.IsChecked;
			App.Model.UserServices.SaveUser(OnRegisterCompletedStep3, null/*userState*/);
		}

		private void OnRegisterCompletedStep3(SaveUserOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				MessageBoxEx.ShowError("Register account", errorStatus, null);
				return;
			}

			string replacements = string.Format("HttpDocumentUri={0}& Password={0}", UriHelper.UriDocument().ToString(), RegisterPassword);
			App.Model.UserServices.EmailTemplateUser(RegisterUserName, ResxResources.EmailFrom, ResxResources.EmailAddress, ResxResources.EmailSubjectWelcome, "Welcome.htm", replacements, OnRegisterCompletedStep4, null/*userState*/);
		}

		private void OnRegisterCompletedStep4(InvokeOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				MessageBoxEx.ShowError("Register account", errorStatus, null);
				return;
			}
		}

		private void OnRecoverClick(object sender, RoutedEventArgs e)
		{
			x_Recover.IsEnabled = false;
			string replacements = string.Format("HttpDocumentUri={0}", UriHelper.UriDocument().ToString());
			App.Model.UserServices.EmailTemplateUserEmail(RecoverEmail, ResxResources.EmailFrom, ResxResources.EmailAddress, ResxResources.EmailSubjectRecover, "PasswordRecovery.htm", replacements, OnRecoverCompleted, null/*userState*/);
		}

		private void OnRecoverCompleted(InvokeOperation operation)
		{
			x_Recover.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				x_RecoverErrorStatus.Text = errorStatus;
				x_RecoverErrorStatus.Visibility = Visibility.Visible;
				return;
			}

			x_RecoverErrorStatus.Text = null;
			x_RecoverErrorStatus.Visibility = Visibility.Collapsed;
			base.Closed += OnRecoverDialogClosed;
			DialogResult = MessageBoxResult.OK;
		}

		private void OnRecoverDialogClosed(object sender, EventArgs e)
		{
			base.Closed -= OnRecoverDialogClosed;
			MessageBoxEx.ShowInfo("Help is on the way!", string.Format("We just sent an email to you at '{0}'.\n\nYou can use the link in that message to reset your password and sign in.\n\nIf the email does not arrive promptly, check your spam folder for a message from '{1}'.", RecoverEmail, ResxResources.EmailFrom), null);
		}

		private void OnResetPasswordClick(object sender, RoutedEventArgs e)
		{
			if (ResetPassword.IsEmpty())
			{
				x_ResetErrorStatus.Text = null;
				x_ResetErrorStatus.Visibility = Visibility.Collapsed;
				return;
			}

			x_Reset.IsEnabled = false;
			App.Model.UserServices.ResetAndChangePassword(m_ResetPasswordCode, ResetPassword, OnResetPasswordCompleted, null/*userState*/);
		}

		private void OnResetPasswordCompleted(InvokeOperation<bool> operation)
		{
			x_Reset.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				x_ResetErrorStatus.Text = errorStatus;
				x_ResetErrorStatus.Visibility = Visibility.Visible;
				return;
			}

			App.Model.UserServices.Login(ResetUserName, ResetPassword, true/*isPersistent*/, OnResetPasswordCompletedStep2, null/*userState*/);

			x_ResetErrorStatus.Text = null;
			x_ResetErrorStatus.Visibility = Visibility.Collapsed;
			DialogResult = MessageBoxResult.OK;
		}

		private void OnResetPasswordCompletedStep2(LoginOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null || !operation.LoginSuccess)
			{
				MessageBoxEx.ShowError("Reset password", errorStatus, null);
				return;
			}
		}

		private void OnChangeClick(object sender, RoutedEventArgs e)
		{
			x_Change.IsEnabled = false;
			App.Model.UserServices.ChangePassword(App.Model.UserServices.User.Name, x_ChangeCurrentPassword.Password, x_ChangeNewPassword.Password, OnChangePasswordCompleted, null/*userState*/);
		}

		private void OnChangePasswordCompleted(InvokeOperation<bool> operation)
		{
			x_Change.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			bool changed = (errorStatus == null && operation.Value == true);
			if (!changed)
			{
				x_ChangeErrorStatus.Text = (errorStatus != null ? errorStatus : g_PasswordNotChanged);
				x_ChangeErrorStatus.Visibility = Visibility.Visible;
				return;
			}

			x_ChangeErrorStatus.Text = null;
			x_ChangeErrorStatus.Visibility = Visibility.Collapsed;
			x_ChangeCurrentPassword.Password = "";
			x_ChangeNewPassword.Password = "";
			DialogResult = (changed ? MessageBoxResult.OK : MessageBoxResult.Cancel);
		}

		private void OnDeleteClick(object sender, RoutedEventArgs e)
		{
			x_Delete.IsEnabled = false;
			string userName = App.Model.UserServices.User.Name;
			App.Model.UserServices.DeleteUser(userName, true/*deleteAllRelatedData*/, OnDeleteCompleted, null/*userState*/);
		}

		private void OnDeleteCompleted(InvokeOperation<bool> operation)
		{
			x_Delete.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			bool deleted = (errorStatus == null && operation.Value == true);
			if (!deleted)
			{
				x_DeleteErrorStatus.Text = (errorStatus != null ? errorStatus : g_AccountNotDeleted);
				x_DeleteErrorStatus.Visibility = Visibility.Visible;
				return;
			}

			App.Model.UserServices.Logout(null, null);
			x_DeleteErrorStatus.Text = null;
			x_DeleteErrorStatus.Visibility = Visibility.Collapsed;
			DialogResult = (deleted ? MessageBoxResult.OK : MessageBoxResult.Cancel);
		}

		private void OnSwitchToSignInPanelClick(object sender, RoutedEventArgs e)
		{
			x_SignInPanel.Visibility = Visibility.Visible;
			x_RegisterPanel.Visibility = Visibility.Collapsed;
			x_RecoverPanel.Visibility = Visibility.Collapsed;
			x_ResetPasswordPanel.Visibility = Visibility.Collapsed;
			x_ChangePanel.Visibility = Visibility.Collapsed;
			x_DeletePanel.Visibility = Visibility.Collapsed;
			if (x_SignInUserName.Text.IsEmpty())
				m_FocusControl = x_SignInUserName;
			else
			if (x_SignInPassword.Password.IsEmpty())
				m_FocusControl = x_SignInPassword;
			else
				m_FocusControl = x_SignInUserName;

			m_FocusControl.FocusEx();
		}

		private void OnSwitchToRegisterPanelClick(object sender, RoutedEventArgs e)
		{
			x_SignInPanel.Visibility = Visibility.Collapsed;
			x_RegisterPanel.Visibility = Visibility.Visible;
			x_RecoverPanel.Visibility = Visibility.Collapsed;
			x_ResetPasswordPanel.Visibility = Visibility.Collapsed;
			x_ChangePanel.Visibility = Visibility.Collapsed;
			x_DeletePanel.Visibility = Visibility.Collapsed;
			if (x_RegisterUserName.Text.IsEmpty())
				m_FocusControl = x_RegisterUserName;
			else
			if (x_RegisterPassword.Password.IsEmpty())
				m_FocusControl = x_RegisterPassword;
			else
			if (x_RegisterEmail.Text.IsEmpty())
				m_FocusControl = x_RegisterEmail;
			else
				m_FocusControl = x_RegisterEmail;

			m_FocusControl.FocusEx();
		}

		private void OnSwitchToRecoverPanelClick(object sender, RoutedEventArgs e)
		{
			x_SignInPanel.Visibility = Visibility.Collapsed;
			x_RegisterPanel.Visibility = Visibility.Collapsed;
			x_RecoverPanel.Visibility = Visibility.Visible;
			x_ResetPasswordPanel.Visibility = Visibility.Collapsed;
			x_ChangePanel.Visibility = Visibility.Collapsed;
			x_DeletePanel.Visibility = Visibility.Collapsed;
			m_FocusControl = x_RecoverEmail;

			m_FocusControl.FocusEx();
		}

		private void OnSwitchToResetPasswordPanelClick(object sender, RoutedEventArgs e)
		{
			x_SignInPanel.Visibility = Visibility.Collapsed;
			x_RegisterPanel.Visibility = Visibility.Collapsed;
			x_RecoverPanel.Visibility = Visibility.Collapsed;
			x_ResetPasswordPanel.Visibility = Visibility.Visible;
			x_ChangePanel.Visibility = Visibility.Collapsed;
			x_DeletePanel.Visibility = Visibility.Collapsed;
			m_FocusControl = x_ResetPassword;

			m_FocusControl.FocusEx();
		}

		private void OnSwitchToChangePanelClick(object sender, RoutedEventArgs e)
		{
			x_SignInPanel.Visibility = Visibility.Collapsed;
			x_RegisterPanel.Visibility = Visibility.Collapsed;
			x_RecoverPanel.Visibility = Visibility.Collapsed;
			x_ResetPasswordPanel.Visibility = Visibility.Collapsed;
			x_ChangePanel.Visibility = Visibility.Visible;
			x_DeletePanel.Visibility = Visibility.Collapsed;
			if (x_ChangeCurrentPassword.Password.IsEmpty())
				m_FocusControl = x_ChangeCurrentPassword;
			else
			if (x_ChangeNewPassword.Password.IsEmpty())
				m_FocusControl = x_ChangeNewPassword;
			else
				m_FocusControl = x_ChangeCurrentPassword;

			m_FocusControl.FocusEx();
		}

		private void OnSwitchToDeletePanelClick(object sender, RoutedEventArgs e)
		{
			x_SignInPanel.Visibility = Visibility.Collapsed;
			x_RegisterPanel.Visibility = Visibility.Collapsed;
			x_RecoverPanel.Visibility = Visibility.Collapsed;
			x_ResetPasswordPanel.Visibility = Visibility.Collapsed;
			x_ChangePanel.Visibility = Visibility.Collapsed;
			x_DeletePanel.Visibility = Visibility.Visible;

			m_FocusControl = null;
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}
	}

	internal class ResetPasswordDialog
	{
		private string m_ResetPasswordCode;

		public ResetPasswordDialog(string resetPasswordCode)
		{
			m_ResetPasswordCode = resetPasswordCode;
			App.Model.UserServices.ValidateResetPasswordCode(resetPasswordCode, OnValidateResetPasswordCompleted, null/*userState*/);
		}

		private void OnValidateResetPasswordCompleted(InvokeOperation<string> operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			string userName = (errorStatus != null ? null : operation.Value);
			if (userName != null)
			{
				AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.ResetPassword);
				dialog.ResetUserName = userName;
				dialog.ResetPasswordCode = m_ResetPasswordCode;
			}
			else
			{
				AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.RecoverSignIn);
				dialog.RecoverErrorStatus = "Sorry but the link you clicked on is no longer valid.  Enter your email address again and we will send you a new recovery email.";
			}
		}
	}
}
