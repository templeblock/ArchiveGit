using System.ServiceModel.DomainServices.Server;
using System.Text.RegularExpressions;
using System.Web.Security;

namespace Money.Web
{
	internal static class Validate
	{
		public static void NewPassword(string password)
		{
			if (password.Length < Membership.MinRequiredPasswordLength)
				throw new DomainException(string.Format("The new password must be at least {0} characters long.", Membership.MinRequiredPasswordLength));
		}

		public static void EmailAddress(string emailAddress)
		{
			if (string.IsNullOrEmpty(emailAddress))
				throw new DomainException(string.Format("An email address is required.  Please enter a unique email address."));
			if (Membership.GetUser(emailAddress) != null)
				throw new DomainException(string.Format("The email address has already been registered.  If this is your email address, click 'Forgot your password?' and we'll send you an email to recover your username and password.  Otherwise, enter a different email address."));
			//string emailAddressPattern = 
			//	@"^([a-zA-Z0-9_\-\.]+)@((\[[0-9]{1,3}" +
			//	@"\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\" + 
			//	@".)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)$";
			string pattern =
				@"[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?" +
				@":[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?";
			Regex regex = new Regex(pattern, RegexOptions.None/*Compiled*/);
			if (!regex.IsMatch(emailAddress))
				throw new DomainException(string.Format("The email address is not valid."));
		}

		public static void NewUserName(string userName)
		{
			if (string.IsNullOrEmpty(userName))
				throw new DomainException(string.Format("A username is required.  Please enter a unique username."));
			if (Membership.GetUser(userName) != null)
				throw new DomainException(string.Format("The username has already been registered.  If you believe that you are '{0}', click 'Sign in!' or 'Forgot your password?'.  Otherwise, enter a different username.", userName));
			string pattern = @"^(?![._-].*|.*[._-]$)[\w.-]+$";
			Regex regex = new Regex(pattern, RegexOptions.None/*Compiled*/);
			if (!regex.IsMatch(userName))
				throw new DomainException(string.Format("The usernames must be between 4 and 28 characters in length, and accepts alpha-numeric, underscores, dots, and dashes."));
		}
	}
}
