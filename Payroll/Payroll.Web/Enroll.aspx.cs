using System;
using System.Web.UI;

namespace Payroll.Web
{
	public partial class Enroll : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			if (base.IsPostBack)
				return;
		}
	}
}