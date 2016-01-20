using System;
using System.Configuration;
using System.Data;
using System.Data.Odbc;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Web.Security;

namespace Test
{
	public partial class Register : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
		}

		public void Create_User(object sender, EventArgs e)
		{
			if (!Page.IsValid)
				return;

			OdbcConnection MyConnection = null;
			try
			{
				// Create a connection to the data source
				string sConnection = ConfigurationManager.AppSettings.Get("MyConnectionString");
				MyConnection = new OdbcConnection(sConnection);
				MyConnection.Open();

				string sHashPassword = FormsAuthentication.HashPasswordForStoringInConfigFile(ctlPassword.Text.Trim(), "MD5"/*or "SHA1"*/);
				string sSqlCommandString = "INSERT INTO users (EmailAddress, EmailVerified, Password) VALUES (?, ?, ?)";
				OdbcCommand MyCommand = new OdbcCommand(sSqlCommandString, MyConnection);
				MyCommand.Parameters.AddWithValue(""/*EmailAddress*/, ctlEmailAddress.Text.Trim());
				MyCommand.Parameters.AddWithValue(""/*EmailVerified*/, false);
				MyCommand.Parameters.AddWithValue(""/*Password*/, sHashPassword);
				int rowsAffected = MyCommand.ExecuteNonQuery();
				MyConnection.Close();

				FormsAuthentication.SetAuthCookie(ctlEmailAddress.Text.Trim(), true/*bPersistantCookie*/);
				Response.Redirect("Congrats.aspx");
			}
			catch (OdbcException ex)
			{
				MyConnection.Close();
				if (ex.Message.ToString().IndexOf("23000") >= 0)
					ctlErrorMsg.Text = "That email address is already registered.";
				else
					ctlErrorMsg.Text = ex.Message.ToString();
				ctlErrorMsg.Text += "  Please try Again.";
				ctlErrorMsg.Visible = true;
			}
		}
	}
}
