using System.Web.Security;

namespace Pivot.Web
{
	internal static class RoleHelper
	{
		public static void AddUser(string userName, string roleName)
		{
			if (roleName == null)
				return;
			if (!Roles.RoleExists(roleName))
				Roles.CreateRole(roleName);
			if (!Roles.IsUserInRole(userName, roleName))
				Roles.AddUserToRole(userName, roleName);
		}

		public static void RemoveUser(string userName, string roleName)
		{
			if (roleName == null)
				return;
			if (!Roles.RoleExists(roleName))
				return;
			if (Roles.IsUserInRole(userName, roleName))
				Roles.RemoveUserFromRole(userName, roleName);
		}

		public static void Update(string userName, bool hasRole, string roleName)
		{
			if (roleName == null)
				return;
			if (!Roles.RoleExists(roleName))
				Roles.CreateRole(roleName);
			bool inRole = Roles.IsUserInRole(userName, roleName);
			if (hasRole && !inRole)
				Roles.AddUserToRole(userName, roleName);
			else
			if (!hasRole && inRole)
				Roles.RemoveUserFromRole(userName, roleName);
		}
	}
}
