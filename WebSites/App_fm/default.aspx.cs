using System;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Mine
{
	public partial class MyDefault : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			bool bLoggedIn = Page.User.Identity.IsAuthenticated;
			ctlLoggedIn.Visible = bLoggedIn;
			ctlNotLoggedIn.Visible = !bLoggedIn;
		}
	}
}
