using System;
using System.Configuration;
using System.Data;
using System.Data.Odbc;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Web.Security;
using System.Net.Mail;

public partial class LoginControl : UserControl
{
	protected void Page_Load(object sender, EventArgs e)
	{
		panelPassword2.Visible = false;
	}

	protected void LoginBtn_Click(object sender, EventArgs e)
	{
		OdbcConnection MyConnection = null;
		try
		{
			// Create a connection to the data source
			string sConnection = ConfigurationManager.AppSettings.Get("MyConnectionString");
			MyConnection = new OdbcConnection(sConnection);
			MyConnection.Open();

			string sHashPassword = FormsAuthentication.HashPasswordForStoringInConfigFile(ctlPassword.Text.Trim(), "MD5"/*or "SHA1"*/);
			string sSqlCommandString = "SELECT * FROM users WHERE EmailAddress = ? AND Password = ?";
			OdbcCommand MyCommand = new OdbcCommand(sSqlCommandString, MyConnection);
			MyCommand.Parameters.AddWithValue(""/*EmailAddress*/, ctlEmailAddress.Text.Trim());
			MyCommand.Parameters.AddWithValue(""/*Password*/, sHashPassword);
			OdbcDataReader MyDataReader = MyCommand.ExecuteReader();
			bool bAuthenticated = MyDataReader.Read();
			MyDataReader.Close();
			MyConnection.Close();

			if (!bAuthenticated)
			{
			//	panelPassword2.Visible = true;
				ctlErrorMsg.Text = "Unregistered email address or invalid password.";
				ctlErrorMsg.Visible = true;
				return;
			}

			//	FormsAuthentication.RedirectFromLoginPage(ctlEmailAddress.Text.Trim(), true/*bPersistantCookie*/);
			FormsAuthentication.SetAuthCookie(ctlEmailAddress.Text.Trim(), true/*bPersistantCookie*/);
			Page.Response.Redirect("default.aspx");
		}
		catch(Exception ex)
		{
			MyConnection.Close();
			ctlErrorMsg.Text = ex.Message;
			ctlErrorMsg.Visible = true;
			return;
		}
	}

	protected void ForgotBtn_Click(object sender, EventArgs e)
	{
		string sNewPassword = "bird";
		OdbcConnection MyConnection = null;
		try
		{
			// Create a connection to the data source
			string sConnection = ConfigurationManager.AppSettings.Get("MyConnectionString");
			MyConnection = new OdbcConnection(sConnection);
			MyConnection.Open();

			string sSqlCommandString = "SELECT * FROM users WHERE EmailAddress = ?";
			OdbcCommand MyCommand = new OdbcCommand(sSqlCommandString, MyConnection);
			MyCommand.Parameters.AddWithValue(""/*EmailAddress*/, ctlEmailAddress.Text.Trim());
			OdbcDataReader MyDataReader = MyCommand.ExecuteReader();
			bool bEmailAddressExists = MyDataReader.Read();
			MyDataReader.Close();

			int iRowsAffected = 0;
			if (bEmailAddressExists)
			{
				string sHashPassword = FormsAuthentication.HashPasswordForStoringInConfigFile(sNewPassword, "MD5"/*or "SHA1"*/);
				sSqlCommandString = "UPDATE Users SET Password = ?, EmailVerified = ? WHERE EmailAddress = ?";
				OdbcCommand MyCommand1 = new OdbcCommand(sSqlCommandString, MyConnection);
				MyCommand1.Parameters.AddWithValue(""/*Password*/, sHashPassword);
				MyCommand1.Parameters.AddWithValue(""/*EmailVerified*/, false);
				MyCommand1.Parameters.AddWithValue(""/*EmailAddress*/, ctlEmailAddress.Text.Trim());
				iRowsAffected = MyCommand1.ExecuteNonQuery();
			}

			MyConnection.Close();

			if (!bEmailAddressExists)
			{
				ctlErrorMsg.Text = "Unregistered email address.";
				ctlErrorMsg.Visible = true;
				return;
			}

			if (iRowsAffected <= 0)
			{
				ctlErrorMsg.Text = "Could not change the password.";
				ctlErrorMsg.Visible = true;
				return;
			}
		}
		catch(Exception ex)
		{
			MyConnection.Close();
			ctlErrorMsg.Text = ex.Message;
			ctlErrorMsg.Visible = true;
			return;
		}

		try
		{
			string sMailServer = ConfigurationManager.AppSettings.Get("MailServer");
			string sTo = ctlEmailAddress.Text.Trim();
			string sFrom = ConfigurationManager.AppSettings.Get("MailFrom");
			string sFromName = ConfigurationManager.AppSettings.Get("MailFromName");
			string sSubject = ConfigurationManager.AppSettings.Get("MailSubject");
			string sBody = "<html><body>At your request, your password to our site has been reset.  Your new temporary password is: " + sNewPassword + ".  Click here to reenter a new password and login to our site.<p/><a href='http://www.facetofacesoftware.com/fm/Login.aspx'>Login</a>.  If you did not request this information, please let us know immediately.<br/><br/>Customer Care<br/>Face To Face Software</body></html>";

			MailAddress oFromAddress = new MailAddress(sFrom, sFromName);
			MailAddress oToAddress = new MailAddress(sTo, sTo);
			MailMessage oMessage = new MailMessage(oFromAddress, oToAddress);
			oMessage.Body = sBody;
			oMessage.Subject = sSubject + " " + oMessage.BodyEncoding;
			//oMessage.BodyEncoding = ?

			SmtpClient oSmtpClient = new SmtpClient(sMailServer);
			oSmtpClient.Send(oMessage);
		}
		catch(Exception ex)
		{
			ctlErrorMsg.Text = ex.Message;
			ctlErrorMsg.Visible = true;
			return;
		}

		ctlErrorMsg.Text = "Your password has been reset and an email confirmtion sent.";
		ctlErrorMsg.Visible = true;
	}
}
