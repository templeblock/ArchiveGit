using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Net.Mail;
using System.ServiceModel.DomainServices.Hosting;
using System.ServiceModel.DomainServices.Server;
using System.Text;
using System.Web;
using System.Web.Profile;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Money.Web
{
	// Class name becomes "UserRegistrationContext" on the client side if named "UserRegistrationService" here
	[EnableClientAccess]
	public class UserRegistrationService : DomainService
	{
		[Invoke]
		public void CreateUser(string userName, string password, string emailAddress, string roleName)
		{
			Validate.NewUserName(userName);
			Validate.NewPassword(password);
			Validate.EmailAddress(emailAddress);

			MembershipUser member = Membership.CreateUser(userName, password, emailAddress);
			RoleHelper.AddUser(userName, roleName);

			// Insert a new record into the Profile table
			ProfileBase profile = ProfileBase.Create(userName, true/*bIsAuthenticated*/);
			profile["OptInUpdates"] = false;
			profile["Language"] = 0;
			profile["ProfileXml"] = "";
			profile["DocumentXml"] = "";
			profile["IncludeSpouse"] = false;
			profile["IncludeDependants"] = false;
			profile["MeFirstName"] = "";
			profile["MeGender"] = "Male";
			profile["MeBirthDate"] = 0;
			profile["MeLifeExpectancy"] = 0.0;
			profile["MeRelationshipStatus"] = "Single";
			profile["SpouseFirstName"] = "";
			profile["SpouseGender"] = "Female";
			profile["SpouseBirthDate"] = 0;
			profile["SpouseLifeExpectancy"] = 0.0;
			profile["SpouseRelationshipStatus"] = "Single";
			profile.Save();
		}

		[Invoke]
		public bool DeleteUser(string userName, bool deleteAllRelatedData)
		{
//j			AuthenticationService authentication = new AuthenticationService();
//j			User oldUser = authentication.Logout();
			return Membership.DeleteUser(userName, deleteAllRelatedData);
		}

		[Invoke]
		public string ResetPassword(string userName)
		{
			MembershipUser member = GetMembershipUser(userName);
			return member.ResetPassword();
		}

		[Invoke]
		public bool ChangePassword(string userName, string currentPassword, string newPassword)
		{
			if (!Membership.ValidateUser(userName, currentPassword))
				throw new DomainException("The current password is invalid.");
			Validate.NewPassword(newPassword);
			MembershipUser member = GetMembershipUser(userName);
			return member.ChangePassword(currentPassword, newPassword);
		}

		[Invoke]
		public string ValidateResetPasswordCode(string passwordCode)
		{
			string userName = GetUserNameFromPasswordCode(passwordCode);
			MembershipUser member = GetMembershipUser(userName);
			if (passwordCode != ComputePasswordCode(member))
				return null;
			return userName;
		}

		[Invoke]
		public bool ResetAndChangePassword(string passwordCode, string newPassword)
		{
			string userName = GetUserNameFromPasswordCode(passwordCode);
			MembershipUser member = GetMembershipUser(userName);
			if (passwordCode != ComputePasswordCode(member))
				throw new DomainException("The reset password code is no longer valid.");
			Validate.NewPassword(newPassword);
			string currentPassword = member.ResetPassword();
			return member.ChangePassword(currentPassword, newPassword);
		}

		private string GetUserNameFromPasswordCode(string passwordCode)
		{
			if (passwordCode == null)
				return null;
			passwordCode = passwordCode.FromBase64();
			string userName = passwordCode.Substring(0, passwordCode.IndexOf('='));
			return userName;
		}

		private string ComputePasswordCode(MembershipUser member)
		{
			string passwordCode = string.Format("{0}={1}", member.UserName.ToLower(), member.LastLoginDate.ToBinary());
			passwordCode = passwordCode.ToBase64();
			return passwordCode;
		}

		[Invoke]
		public bool ChangePasswordQuestionAndAnswer(string userName, string password, string newPasswordQuestion, string newPasswordAnswer)
		{
			MembershipUser member = GetMembershipUser(userName);
			return member.ChangePasswordQuestionAndAnswer(password, newPasswordQuestion, newPasswordAnswer);
		}

		[Invoke]
		public bool UnlockUser(string userName)
		{
			MembershipUser member = GetMembershipUser(userName);
			return member.UnlockUser();
		}

		[Invoke]
		public string GetUserNameByEmail(string emailAddress)
		{
			Validate.EmailAddress(emailAddress);
			return Membership.GetUserNameByEmail(emailAddress);
		}

		[Invoke]
		public int GetNumberOfUsersOnline()
		{
			return Membership.GetNumberOfUsersOnline();
		}

		[Invoke]
		public bool ValidateUser(string userName, string password)
		{
			return Membership.ValidateUser(userName, password);
		}

		[Invoke]
		public void EmailTemplateUser(string userName, string fromDisplayName, string from, string subject, string bodyFileName, string replacements)
		{
			EmailTemplate(userName, null/*emailAddress*/, fromDisplayName, from, subject, bodyFileName, replacements);
		}

		[Invoke]
		public void EmailTemplateEmail(string emailAddress, string fromDisplayName, string from, string subject, string bodyFileName, string replacements)
		{
			EmailTemplate(null/*userName*/, emailAddress, fromDisplayName, from, subject, bodyFileName, replacements);
		}

		[Invoke]
		public void EmailTemplateUserEmail(string emailAddress, string fromDisplayName, string from, string subject, string bodyFileName, string replacements)
		{
			string userName = GetUserNameByEmail(emailAddress);
			if (userName == null)
				throw new DomainException("There is no user with that email address.");

			EmailTemplate(userName, emailAddress, fromDisplayName, from, subject, bodyFileName, replacements);
		}

		private void EmailTemplate(string optionalUserName, string optionalEmailAddress, string fromDisplayName, string from, string subject, string bodyFileName, string replacements)
		{
			string bodyFilePath = HttpContext.Current.Server.MapPath("~/EmailTemplates/" + bodyFileName);
			if (!File.Exists(bodyFilePath))
				throw new DomainException(string.Format("'{0}' is an invalid email template.", bodyFileName));

			Dictionary<string, string> replace = new Dictionary<string, string>();

			if (replacements != null && replacements.Length > 0)
			{
				string[] pairs = replacements.Split('&');
				foreach (string pair in pairs)
				{
					string[] nameValue = pair.Split('=');
					replace.AddSubstitutionPair(nameValue[0].Trim(), nameValue[1].Trim());
				}
			}

			if (optionalUserName != null)
			{
				MembershipUser member = GetMembershipUser(optionalUserName);
				if (optionalEmailAddress == null)
					optionalEmailAddress = member.Email;

				//replace.AddSubstitutionPair("HttpDocumentUri()", );
				replace.AddSubstitutionPair("AllUsersByEmail", AllUsersByEmail(member.Email));
				replace.AddSubstitutionPair("ResetPasswordCode", ComputePasswordCode(member));
				replace.AddSubstitutionPair("UserName", member.UserName);
				replace.AddSubstitutionPair("UserId", member.ProviderUserKey);
				replace.AddSubstitutionPair("Comment", member.Comment);
				replace.AddSubstitutionPair("Email", Uri.EscapeUriString(member.Email));
				//replace.AddSubstitutionPair("IsApproved", member.IsApproved);
				//replace.AddSubstitutionPair("IsLockedOut", member.IsLockedOut);
				//replace.AddSubstitutionPair("IsOnline", member.IsOnline);
				//replace.AddSubstitutionPair("PasswordQuestion", member.PasswordQuestion);
				//replace.AddSubstitutionPair("ProviderName", member.ProviderName);
				//replace.AddSubstitutionPair("CreationDate", member.CreationDate);
				//replace.AddSubstitutionPair("LastActivityDate", member.LastActivityDate);
				//replace.AddSubstitutionPair("LastLockoutDate", member.LastLockoutDate);
				//replace.AddSubstitutionPair("LastLoginDate", member.LastLoginDate);
				//replace.AddSubstitutionPair("LastPasswordChangedDate", member.LastPasswordChangedDate);

				string[] roles = Roles.GetAllRoles();
				foreach (string role in roles)
				{
					bool isInRole = Roles.IsUserInRole(optionalUserName, role);
					replace.AddSubstitutionPair(role, isInRole);
				}

				ProfileBase profile = ProfileBase.Create(optionalUserName, true/*bIsAuthenticated*/);
				foreach (SettingsProperty sp in ProfileBase.Properties)
				{
					string name = sp.Name;
					object value = profile.GetPropertyValue(name);
					replace.AddSubstitutionPair(name, value);
				}
			}

			MailDefinition template = new MailDefinition();
			template.BodyFileName = bodyFilePath;
			MailMessage mailMessage = template.CreateMailMessage(optionalEmailAddress, replace, new LiteralControl());
			mailMessage.From = new MailAddress(from, fromDisplayName);
			//mailMessage.To[0].DisplayName = user.UserName;
			mailMessage.Subject = subject;
			mailMessage.IsBodyHtml = (mailMessage.Body.IndexOf("<html") >= 0);
			if (mailMessage.IsBodyHtml)
			{
				mailMessage.AlternateViews.Add(AlternateView.CreateAlternateViewFromString(mailMessage.Body, null, "text/html"));
				mailMessage.AlternateViews.Add(AlternateView.CreateAlternateViewFromString(mailMessage.Body, null, "text/plain"));
			}

			SmtpClient smtp = new SmtpClient();
			smtp.Send(mailMessage);
			//smtp.SendAsync(mailMessage, mailMessage);
		}

		[Invoke]
		public void EmailBodyUser(string userName, string fromDisplayName, string from, string subject, string body)
		{
			EmailBody(userName, null/*emailAddress*/, fromDisplayName, from, subject, body);
		}

		[Invoke]
		public void EmailBodyEmail(string emailAddress, string fromDisplayName, string from, string subject, string body)
		{
			EmailBody(null/*userName*/, emailAddress, fromDisplayName, from, subject, body);
		}

		[Invoke]
		public void EmailBodyUserEmail(string emailAddress, string fromDisplayName, string from, string subject, string body)
		{
			string userName = GetUserNameByEmail(emailAddress);
			if (userName == null)
				throw new DomainException("There is no user with that email address.");

			EmailBody(userName, emailAddress, fromDisplayName, from, subject, body);
		}

		private void EmailBody(string optionalUserName, string optionalEmailAddress, string fromDisplayName, string from, string subject, string body)
		{
			if (optionalUserName != null)
			{
				MembershipUser member = GetMembershipUser(optionalUserName);
				if (optionalEmailAddress == null)
					optionalEmailAddress = member.Email;
			}

			MailMessage mailMessage = new MailMessage(from, optionalEmailAddress);
			mailMessage.Body = body;
			mailMessage.From = new MailAddress(from, fromDisplayName);
			mailMessage.Subject = subject;
			mailMessage.IsBodyHtml = (mailMessage.Body.IndexOf("<html") >= 0);
			if (mailMessage.IsBodyHtml)
			{
				mailMessage.AlternateViews.Add(AlternateView.CreateAlternateViewFromString(mailMessage.Body, null, "text/html"));
				mailMessage.AlternateViews.Add(AlternateView.CreateAlternateViewFromString(mailMessage.Body, null, "text/plain"));
			}

			SmtpClient smtp = new SmtpClient();
			smtp.Send(mailMessage);
			//smtp.SendAsync(mailMessage, mailMessage);
		}

		private string AllUsersByEmail(string emailAddress)
		{
			StringBuilder userNames = null;
			MembershipUserCollection userCollection = Membership.FindUsersByEmail(emailAddress);
			foreach (MembershipUser member in userCollection)
			{
				if (userNames != null)
					userNames.Append(", ");
				else
					userNames = new StringBuilder();
				userNames.Append(member.UserName);
			}

			return userNames.ToString();
		}

		private MembershipUser GetMembershipUser(string userName)
		{
			MembershipUser member = Membership.GetUser(userName);
			if (member == null)
				throw new DomainException(string.Format("User '{0}' does not exist.", userName));
			return member;
		}

#if NOTUSED //j Client has no access to the MembershipUser or MembershipUserCollection classes
		[Invoke]
		public MembershipUserCollection GetAllUsers(int pageIndex, int pageSize, out int totalRecords)
		{
			return Membership.GetAllUsers(pageIndex, pageSize, out totalRecords);
		}

		[Invoke]
		public MembershipUserCollection FindUsersByName(string userNameToMatch)
		{
			return Membership.FindUsersByName(userNameToMatch);
		}

		[Invoke]
		public MembershipUserCollection FindUsersByEmail(string emailToMatch)
		{
			return Membership.FindUsersByEmail(emailToMatch);
		}
#endif
	}
}
