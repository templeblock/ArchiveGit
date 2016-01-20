using System;
using System.Web;
using System.Web.UI;
using System.Web.Security;

/// <summary>
/// Summary description for Logout.
/// </summary>
public partial class Logout : Page
{
	protected void Page_Load(object sender, System.EventArgs e)
	{
		FormsAuthentication.SignOut();
	}
}
