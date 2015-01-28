using System;
using System.Web;
using System.Web.UI;

namespace Payroll.Web
{
	public partial class Payroll : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			try
			{
				// Use any incoming Query string as the new PathAndQuery portion of the iframe's src attribute
				string query = base.ClientQueryString;
				if (string.IsNullOrEmpty(query))
					return;

				// Convert the Query string back into a PathAndQuery string
				string newPathAndQuery = HttpUtility.UrlDecode(query);
				if (newPathAndQuery.StartsWith("cmd="))
				{ // Remove the cmd= prefix and change the first '&' to a '?'
					newPathAndQuery = newPathAndQuery.Substring(4);
					int index = newPathAndQuery.IndexOf("&");
					if (index >= 0)
						newPathAndQuery = newPathAndQuery.Substring(0, index) + '?' + newPathAndQuery.Substring(index + 1);
				}

				// Make sure newPathAndQuery has a leading slash!
				if (newPathAndQuery.Length > 0 && newPathAndQuery[0] != '/')
					newPathAndQuery = '/' + newPathAndQuery;

				// Get the iframe's src attribute as a uri
				Uri uri = new Uri(_frame.Attributes["src"]);

				// Replace any existing PathAndQuery with the new one
				uri = new Uri(uri, newPathAndQuery);

				_frame.Attributes.Add("src", uri.ToString());
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}
	}
}