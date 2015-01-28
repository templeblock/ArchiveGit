using System;
using System.Collections.Generic;
using System.Net.Mail;
using System.Web.UI;

namespace Payroll.Web
{
	public partial class EnrollPost : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			try
			{
				// Create the form variables replacement dictionary
				Dictionary<string, string> variables = new Dictionary<string, string>();
				variables.Add("BusinessType", Request.ValueEncoded("_BusinessType"));
				variables.Add("BizName", Request.ValueEncoded("_BizName"));
				variables.Add("Address", Request.ValueEncoded("_Address"));
				variables.Add("Address2", Request.ValueEncoded("_Address2"));
				variables.Add("City", Request.ValueEncoded("_City"));
				variables.Add("County", Request.ValueEncoded("_County"));
				variables.Add("State", Request.ValueEncoded("_State"));
				variables.Add("ZipCode", Request.ValueEncoded("_ZipCode"));
				variables.Add("FirstName", Request.ValueEncoded("_FirstName"));
				variables.Add("LastName", Request.ValueEncoded("_LastName"));
				variables.Add("Phone", Request.ValueEncoded("_Phone"));
				variables.Add("EmailAddr", Request.ValueEncoded("_EmailAddr"));

				string replaceVariablesPattern = @"\{([a-zA-Z0-9]+)\}";
				SendConfirmationEmail(replaceVariablesPattern, variables);

				Response.Write(Request.ValueDecoded("_EmailAddr"));
			}
			catch (Exception ex)
			{
				Response.Write("Error: " + ex.Message);
			}
		}

		private void SendConfirmationEmail(string replaceVariablesPattern, Dictionary<string, string> variables)
		{
			// Create the email link
			string emailLink = @"
				https://discountpayroll.net/CreateAccount.aspx?
				BusinessType={BusinessType}&
				BizName={BizName}&
				Address={Address}&
				Address2={Address2}&
				City={City}&
				County={County}&
				State={State}&
				ZipCode={ZipCode}&
				FirstName={FirstName}&
				LastName={LastName}&
				Phone={Phone}&
				EmailAddr={EmailAddr}&
				".ReplaceWhiteSpace("");

			// Create the new user email
			string subject = "Email Verification";
			string body = @"
				<html><head></head><body>
				<p>Thank you for choosing DiscountPayroll.net.
				</p>
				<p>Before your new online payroll account can been created, you must <a href='{0}'>click 
				here to confirm your email address<a/>.
				</p>
				<p>Once confirmed, you will be prompted to select a user id to login to the secure payroll site.
				</p>
				<p>If you received this email by mistake, simply delete it. You won't be enrolled if you 
				don't follow through by clicking the confirmation link above.
				</p>
				<p>Thank you,<br/>
				The DiscountPayroll.net Team<br/>
				</p>
				</body></html>
				".ReplaceWhiteSpace(" ");

			body = string.Format(body, emailLink);

			// Replace any form variables found in the body
			int numReplacements = 0;
			body = body.ReplacePattern(replaceVariablesPattern, 1, variables, out numReplacements);

			// Send the user a confirmation email
			Helper.SendMail(
				new MailAddress("sales@discountpayroll.net", "DiscountPayroll.net Sales"),
				new MailAddress(Request.Value("_EmailAddr"), Request.Value("_FirstName") + " " + Request.Value("_LastName")),
				subject, body);
		}
	}
}
