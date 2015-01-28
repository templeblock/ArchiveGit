using System;
using System.ServiceModel.DomainServices.Client;
using System.ServiceModel.DomainServices.Client.ApplicationServices;
using ClassLibrary;
using Pivot.Web;

namespace Pivot
{
	public class UserServices : ViewModelBase
	{
		// For databinding...
		//		1.) in code, set DataContext to UserContext
		//		2.) in XAML, set Binding to User.*

		// For databinding to DataContext
		public WebContext UserContext { get { return WebContext.Current; } }

		// Do not store a reference to the UserContext.User object, since it changes during calls to LoadUser
		public UserProfile User { get { return UserContext.User; } }

		public string LastUserName { get; set; }

		private AuthenticationOperation m_Operation;
		private void CheckOKtoStartOperation() { if (m_Operation != null && !m_Operation.IsComplete) Helper.Alert("Can't start a new operation while one is in progress"); }
		public void CheckOKtoSetProperty() { if (m_Operation != null && !m_Operation.IsComplete) Helper.Alert("Can't change a user property while an operation in progress"); }

		private const string g_LastLoginCookieName = "lastlogin";

		public UserServices()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
		}

		internal void Initialize()
		{
			LastUserName = Cookie.Read(g_LastLoginCookieName);

			CheckOKtoStartOperation();
			m_Operation = UserContext.Authentication.LoadUser(OnLoadUserCompleted, null/*userState*/);
		}

		private void OnLoadUserCompleted(LoadUserOperation operation)
		{
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
				return;

			UpdateLastUserName();
		}

		private void UpdateLastUserName()
		{
			if (!User.Name.IsEmpty())
			{
				LastUserName = User.Name;
				Cookie.Write(g_LastLoginCookieName, LastUserName, -1);

				App.Analytics.TrackEvent(Analytics.ActionValue.SignIn, LastUserName);
			}
		}

		internal void SaveUser(Action<SaveUserOperation> completedAction, object userState)
		{
			CheckOKtoStartOperation();
			m_Operation = UserContext.Authentication.SaveUser(completedAction, userState);
		}

		internal void Login(string userName, string password, bool isPersistent, Action<LoginOperation> completedAction, object userState)
		{
			LoginParameters login = new LoginParameters(userName, password, isPersistent, null);
			CheckOKtoStartOperation();
			m_Operation = UserContext.Authentication.Login(login, completedAction, userState);
			m_Operation.Completed += delegate(object sender, EventArgs e)
			{
				UpdateLastUserName();
			};
		}

		internal void Logout(Action<LogoutOperation> completedAction, object userState)
		{
			CheckOKtoStartOperation();
			m_Operation = UserContext.Authentication.Logout(completedAction, userState);

			App.Analytics.TrackEvent(Analytics.ActionValue.SignOut, LastUserName);
		}

		internal void GetUserNameByEmail(string emailToMatch, Action<InvokeOperation<string>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.GetUserNameByEmail(emailToMatch, completedAction, userState);
		}

		internal void GetNumberOfUsersOnline(Action<InvokeOperation<int>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.GetNumberOfUsersOnline(completedAction, userState);
		}

		internal void ValidateUser(string userName, string password, Action<InvokeOperation<bool>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.ValidateUser(userName, password, completedAction, userState);
		}

		internal void CreateUser(string userName, string password, string emailAddress, string roleName, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.CreateUser(userName, password, emailAddress, roleName, completedAction, userState);
		}

		internal void DeleteUser(string userName, bool deleteAllRelatedData, Action<InvokeOperation<bool>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.DeleteUser(userName, deleteAllRelatedData, completedAction, userState);
		}

		internal void ValidateResetPasswordCode(string resetPasswordCode, Action<InvokeOperation<string>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.ValidateResetPasswordCode(resetPasswordCode, completedAction, userState);
		}

		internal void ResetAndChangePassword(string resetPasswordCode, string newPassword, Action<InvokeOperation<bool>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.ResetAndChangePassword(resetPasswordCode, newPassword, completedAction, userState);
		}

		internal void ResetPassword(string userName, Action<InvokeOperation<string>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.ResetPassword(userName, completedAction, userState);
		}

		internal void ChangePassword(string userName, string currentPassword, string newPassword, Action<InvokeOperation<bool>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.ChangePassword(userName, currentPassword, newPassword, completedAction, userState);
		}

		internal void ChangePasswordQuestionAndAnswer(string userName, string password, string newPasswordQuestion, string newPasswordAnswer, Action<InvokeOperation<bool>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.ChangePasswordQuestionAndAnswer(userName, password, newPasswordQuestion, newPasswordAnswer, completedAction, userState);
		}

		internal void UnlockUser(string userName, Action<InvokeOperation<bool>> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.UnlockUser(userName, completedAction, userState);
		}

		internal void EmailTemplateUser(string userName, string fromDisplayName, string from, string subject, string bodyFileName, string replacements, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.EmailTemplateUser(userName, fromDisplayName, from, subject, bodyFileName, replacements, completedAction, userState);
		}

		internal void EmailTemplateEmail(string emailAddress, string fromDisplayName, string from, string subject, string bodyFileName, string replacements, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.EmailTemplateEmail(emailAddress, fromDisplayName, from, subject, bodyFileName, replacements, completedAction, userState);
		}

		internal void EmailTemplateUserEmail(string emailAddress, string fromDisplayName, string from, string subject, string bodyFileName, string replacements, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.EmailTemplateUserEmail(emailAddress, fromDisplayName, from, subject, bodyFileName, replacements, completedAction, userState);
		}

		internal void EmailBodyUser(string userName, string fromDisplayName, string from, string subject, string body, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.EmailBodyUser(userName, fromDisplayName, from, subject, body, completedAction, userState);
		}

		internal void EmailBodyEmail(string emailAddress, string fromDisplayName, string from, string subject, string body, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.EmailBodyEmail(emailAddress, fromDisplayName, from, subject, body, completedAction, userState);
		}

		internal void EmailBodyUserEmail(string emailAddress, string fromDisplayName, string from, string subject, string body, Action<InvokeOperation> completedAction, object userState)
		{
			MembershipContext membership = new MembershipContext();
			membership.EmailBodyUserEmail(emailAddress, fromDisplayName, from, subject, body, completedAction, userState);
		}
	}
}
