using System;
using System.Collections.ObjectModel;
using System.Security.Principal;
using System.ServiceModel.DomainServices.Hosting;
using System.ServiceModel.DomainServices.Server.ApplicationServices;
using System.Web.Security;

namespace Money.Web
{
	[EnableClientAccess]
	public class AuthenticationService : AuthenticationBase<User>
	{
		// Override so a single method (UserContext.Service.SaveUser) can be user to save all user related properties
		protected override void UpdateUserCore(User user)
		{ // Triggered from a UserContext.Service.SaveUser()
			bool updateMemberRequired = false;
			MembershipUser member = Membership.GetUser(user.Name);

			// MemberEmail
			if (member.Email != user.MemberEmail)
			{
				Validate.EmailAddress(user.MemberEmail);
				member.Email = user.MemberEmail;
				updateMemberRequired = true;
			}

			// MemberComment
			if (member.Comment != user.MemberComment)
			{
				member.Comment = user.MemberComment;
				updateMemberRequired = true;
			}

			if (updateMemberRequired)
				Membership.UpdateUser(member);

			// MemberIsAdmin
			string roleName = "Administrator";
			RoleHelper.Update(user.Name, user.MemberIsAdmin, roleName);

			// MemberIsPremium
			roleName = "Premium";
			RoleHelper.Update(user.Name, user.MemberIsPremium, roleName);

			// This will save any profile properties in the user object, stored in the aspnet_Profile table
			base.UpdateUserCore(user);
		}

		// Override so a single method (UserContext.Service.LoadUser) can be user to get all user related properties
		protected override User GetAuthenticatedUser(IPrincipal principal)
		{ // Triggered from a UserContext.Service.LoadUser()
			User user = base.GetAuthenticatedUser(principal);
			MembershipUser member = Membership.GetUser(user.Name);
			if (member == null)
				return user;

			user.MemberUserId = new Guid(member.ProviderUserKey.ToString());
			user.MemberComment = member.Comment;
			user.MemberEmail = member.Email;
			//user.MemberIsApproved = member.IsApproved;
			//user.MemberIsLockedOut = member.IsLockedOut;
			//user.MemberIsOnline = member.IsOnline;
			//user.MemberPasswordQuestion = member.PasswordQuestion;
			//user.MemberProviderName = member.ProviderName;
			//user.MemberCreationDate = member.CreationDate;
			//user.MemberLastLockoutDate = member.LastLockoutDate;
			//user.MemberLastLoginDate = member.LastLoginDate;
			//user.MemberLastPasswordChangedDate = member.LastPasswordChangedDate;

			Collection<string> roles = user.Roles.ToCollection<string>();
			user.MemberIsAdmin = roles.Contains("Administrator");
			user.MemberIsPremium = roles.Contains("Premium");
			return user;
		}

		// No need to override the following methods
#if NOTUSED //j
		protected override void ClearAuthenticationToken()
		{
			base.ClearAuthenticationToken();
		}

		protected override User CreateUser()
		{
			return base.CreateUser();
		}

		protected override User GetAnonymousUser()
		{
			return base.GetAnonymousUser();
		}

		protected override void IssueAuthenticationToken(IPrincipal principal, bool isPersistent)
		{
			base.IssueAuthenticationToken(principal, isPersistent);
		}

		protected override bool ValidateUser(string userName, string password)
		{
			return base.ValidateUser(userName, password);
		}
#endif
	}
}
