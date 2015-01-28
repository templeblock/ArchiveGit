using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Mail;
using System.Text;
using System.Web.UI;

namespace Payroll.Web
{
	public partial class CreateAccountPost : Page
	{
		private const string _host = "https://www.managepayroll.com";
		private const string _loginUrl = _host + "/sp/client/default.jsp";
		private const string _addClientUrlSB = _host + "/signup/profile/addClient.jsp";
		private const string _addClientUrlHH = _host + "/signup/profile/addClientHH.jsp";
		private const string _loginFormDataFormat = "userStrId=jimmccurdy2&password=7202jimm&ProfileExclude=password&Login.x=0&Login.y=0&loginSalt={0}&loginNow=1&hss=1";
		private const string _adminEmailSubject = "New user at DiscountPayroll.net!";
		private const string _adminEmailAddress = "sales@discountpayroll.net";
		private const string _adminEmailDisplayName = "DiscountPayroll.net Sales";
		private const string _welcomeEmailSubject = "Welcome to DiscountPayroll.net!";
		private const string _welcomeEmailAddress = "support@discountpayroll.net";
		private const string _welcomeEmailDisplayName = "DiscountPayroll.net Support";
		private const string _replaceVariablesPattern = @"\{([a-zA-Z0-9]+)\}";

		protected void Page_Load(object sender, EventArgs e)
		{
			try
			{
				// Create the form variables replacement dictionary
				Dictionary<string, string> variables = new Dictionary<string, string>();
				variables.Add("UserId", Request.ValueEncoded("_UserId"));

				// Restore the other request variables

				bool isSmallBusiness = (Request.ValueEncoded("_BusinessType") == "SB");
				bool isHousehold = (Request.ValueEncoded("_BusinessType") == "HH");
				if (isSmallBusiness == isHousehold)
					return;

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

				string addClientUrl = (isSmallBusiness ? _addClientUrlSB : _addClientUrlHH);
				string addClientFormData = GetAddClientFormData(isSmallBusiness, variables);

				string error = CreateAccount(addClientUrl, addClientFormData);
				if (!string.IsNullOrEmpty(error))
				{
					SendAdminEmail(error, addClientUrl, addClientFormData);
					Response.Write("Error: " + error);
					return;
				}

				SendAdminEmail(error, addClientUrl, addClientFormData);
				SendWelcomeEmail(variables);

				Response.Write(Request.ValueDecoded("_UserId"));
			}
			catch (Exception ex)
			{
				Response.Write("Error: " + ex.Message);
			}
		}

		private string CreateAccount(string addClientUrl, string addClientFormData)
		{
			string result = string.Empty;
			string error = string.Empty;
			CookieContainer cookies = new CookieContainer();

			// Make the login get
			error = Post(_loginUrl, string.Empty, false, ref cookies, out result);
			if (!string.IsNullOrEmpty(error))
				return error;

			// Process the result, and look for login salt as <input type="hidden" name="loginSalt" value="???????"/>
			string loginSalt = Extract("name=\"loginSalt\" value=\"", "\"", result);
			if (string.IsNullOrEmpty(loginSalt))
				return "Can't login";

			// Make the login post
			string formData = string.Format(_loginFormDataFormat, loginSalt);
			error = Post(_loginUrl, formData, true, ref cookies, out result);
			if (!string.IsNullOrEmpty(error))
				return error;

			// Make the add client post
			error = Post(addClientUrl, addClientFormData, true, ref cookies, out result);
			if (!string.IsNullOrEmpty(error))
				return error;

			return string.Empty;
		}

		private string Post(string url, string formData, bool post, ref CookieContainer cookies, out string result)
		{
			result = string.Empty;
			try
			{
				string referer = url;
				if (!post && !string.IsNullOrEmpty(formData))
					url += "?" + formData;

				HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
				request.Method = (post ? "POST" : "GET");
				request.CookieContainer = cookies;
				request.UserAgent = Request.UserAgent;
				request.Referer = referer;

				if (post && !string.IsNullOrEmpty(formData))
				{
					request.ContentType = "application/x-www-form-urlencoded";
					ASCIIEncoding encoding = new ASCIIEncoding();
					byte[] bodyArray = encoding.GetBytes(formData);
					request.ContentLength = bodyArray.Length;
					using (Stream bodyStream = request.GetRequestStream())
					{
						bodyStream.Write(bodyArray, 0, bodyArray.Length);
					}
				}

				HttpWebResponse response = (HttpWebResponse)request.GetResponse();
				if (response.StatusCode != HttpStatusCode.OK)
					return "Cannot access the server.";

				// Retain the cookies
				//foreach (Cookie cookie in response.Cookies)
				//    cookies.Add(cookie);

				string contentType = response.ContentType;
				using (Stream stream = response.GetResponseStream())
				{
					using (StreamReader streamReader = new StreamReader(stream))
					{
						result = streamReader.ReadToEnd();
					}
				}

				// Process the result, and look for errors as <div class="setupError">{errorMessage}</div>
				return Extract("<div class=\"setupError\">", "</div>", result);
			}
			catch (Exception ex)
			{
				return ex.Message;
			}
		}

		private string Extract(string search1, string search2, string source)
		{
			int index = source.IndexOf(search1);
			if (index < 0)
				return string.Empty;

			string result = source.Substring(index + search1.Length);
			index = result.IndexOf(search2);
			if (index < 0)
				return string.Empty;

			return result.Substring(0, index).Trim();
		}

		private void SendWelcomeEmail(Dictionary<string, string> variables)
		{
			// Create the welcome email
			string body = @"
				<html><head></head><body>
				<p>Welcome to DiscountPayroll.net!
				</p>
				<p>Your new payroll administrator account has been created, and you can 
				<a href='https://discountpayroll.net/Payroll.aspx'>Access Your Payoll<a/> here.
				</p>
				<p>Your account details are as follows:
				</p>
				<p>User Id: <b>{UserId}</b><br/>
				Temporary password: <b>password1</b>
				</p>
				<p>Once logged in, you will be prompted to select your own personal password, and you<br/>
				will be prompted to run through a streamlined setup of your new online payroll account.
				</p>
				<p>If you wish to change your User Id at any point, you can do it from the Setup menu.
				</p>
				<p>If you have any questions, please email our support staff at 
				<a href='mailto:support@discountpayroll.net'>support@discountpayroll.net</a>.
				</p>
				<p>Thank you,<br/>
				The DiscountPayroll.net Team
				</p>
				</body></html>
				".ReplaceWhiteSpace(" ");

			// Replace any form variables found in the body
			int numReplacements = 0;
			body = body.ReplacePattern(_replaceVariablesPattern, 1, variables, out numReplacements);

			// Send the user a confirmation email
			Helper.SendMail(
				new MailAddress(_welcomeEmailAddress, _welcomeEmailDisplayName),
				new MailAddress(Request.Value("_EmailAddr"), Request.Value("_FirstName") + " " + Request.Value("_LastName")),
				_welcomeEmailSubject, body);
		}

		private void SendAdminEmail(string error, string addClientUrl, string addClientFormData)
		{
			// Create the new user admin email
			string body = @"
				<html><head></head><body>
				<p>Error: {0}</p>
				<p>AddClientUrl: <a href='{1}'>{1}</a></p>
				<p>AddClientFormData:</p>
				<p>{2}</p>
				<br/>
				</body></html>
				".ReplaceWhiteSpace(" ");
			body = string.Format(body, error, addClientUrl, addClientFormData);

			// Send our admin a new user tracking email
			Helper.SendMail(
				new MailAddress(_adminEmailAddress, _adminEmailDisplayName),
				new MailAddress(_adminEmailAddress, _adminEmailDisplayName),
				_adminEmailSubject, body);
		}

		private string GetAddClientFormData(bool isSmallBusiness, Dictionary<string, string> variables)
		{
			string addClientUrl = string.Empty;
			if (isSmallBusiness)
				addClientUrl = @"
					isProviderLoggedIn=true&
					isAccountantBilled=true&
					editAccessOnly=null&
					addAccessOnly=null&
					hasError=false&
					errorData=null&
					BizName={BizName}&
					Address={Address}&
					AddressTwo={Address2}&
					City={City}&
					County={County}&
					State={State}&
					ZipCode={ZipCode}&
					Industry=&
					FirstName={FirstName}&
					MiddleInitial=&
					LastName={LastName}&
					Title=&
					WorkPhone={Phone}&
					WorkExtension=&
					MobilePhone=&
					FaxNumber=&
					accessSetup=ClientLogin&
					AccessRoleID=1&
					FeatureSet=2&
					sourceCode=ActeleAPDNV3Mo_64526&
					userStrId={UserId}&
					Password=password1&
					PasswordTwo=password1&
					LoginEmailAddr={EmailAddr}&
					FormSubmit=FormSubmit&
					isSB=yes&
					only1099=false&
					BillType=sp&
					providerId=6480280&
					hco=1194143&
					hsp=1194143&
					hss=1&
					ok.x=27&
					ok.y=10&
					".ReplaceWhiteSpace("");

			// Create the add client url
			// Removed:
			//	BizName
			//	Industry
			//	Title
			//	isSB
			//	only1099
			// New:
			//	HomePhone
			//	isHH
			// Modifed:
			//	FeatureSet

			if (!isSmallBusiness)
				addClientUrl = @"
					isProviderLoggedIn=true&
					isAccountantBilled=true&
					editAccessOnly=null&
					addAccessOnly=null&
					hasError=false&
					errorData=null&
					FirstName={FirstName}&
					MiddleInitial=&
					LastName={LastName}&
					Address={Address}&
					AddressTwo={Address2}&
					City={City}&
					County={County}&
					State={State}&
					ZipCode={ZipCode}&
					HomePhone={Phone}&
					WorkPhone=&
					WorkExtension=&
					MobilePhone=&
					FaxNumber=&
					accessSetup=ClientLogin&
					AccessRoleID=1&
					sourceCode=ActeleAPDNV3Mo_64526&
					userStrId={UserId}&
					Password=password1&
					PasswordTwo=password1&
					LoginEmailAddr={EmailAddr}&
					FormSubmit=FormSubmit&
					FeatureSet=7&
					isHH=yes&
					BillType=sp&
					providerId=6480280&
					hco=1194143&
					hsp=1194143&
					hss=1&
					ok.x=27&
					ok.y=10&
					".ReplaceWhiteSpace("");

			// Replace any form variables found in the addClientUrl
			int numReplacements = 0;
			return addClientUrl.ReplacePattern(_replaceVariablesPattern, 1, variables, out numReplacements);
		}
	}
}
