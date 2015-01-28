using System;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Money.Web
{
	public partial class Sitemap : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			// Set the content-type as XML
			this.Response.ContentType = "text/xml";
		}
	}
}
