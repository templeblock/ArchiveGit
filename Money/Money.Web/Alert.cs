using System.Web;
using System.Web.UI;

namespace Money.Web
{
	internal static class Alert
	{
		public static void Show(string message)
		{
			// Cleans the message to allow single quotation marks
			string cleanMessage = message.Replace("'", "\\'");
			string script = "<script type=\"text/javascript\">alert('" + cleanMessage + "');</script>";

			// Checks if the handler is a Page and that the script isn't allready on the Page
			Page page = HttpContext.Current.CurrentHandler as Page;
			if (page != null && !page.ClientScript.IsClientScriptBlockRegistered("alert"))
				page.ClientScript.RegisterClientScriptBlock(typeof(Alert), "alert", script);
		}
	}
}
