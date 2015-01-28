using System;
using System.Web.UI;

namespace Payroll.Web
{
	public partial class CreateAccount : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			if (base.IsPostBack)
				return;
		}

		protected string RequestQuery(string name)
		{
			if (string.IsNullOrEmpty(Request.QueryString[name]))
				return string.Empty;
			return Request.QueryString[name].ToString();
		}
	}
}