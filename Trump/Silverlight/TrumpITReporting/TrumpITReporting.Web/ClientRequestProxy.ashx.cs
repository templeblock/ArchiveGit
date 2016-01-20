using System;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Net;
using System.Text;
using System.Web;
using System.Web.Services;

namespace TrumpITReporting.Web
{
	[WebService(Namespace = "http://tempuri.org/")]
	[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
	public class ClientRequestProxy : IHttpHandler
	{
		public bool IsReusable { get { return false; } }

		public void ProcessRequest(HttpContext context)
		{
			// Add cookie headers
			WebClient wc = SetCookies(new WebClient(), context.Request);
			string url = context.Request["url"];

			try
			{
				string searchUrl = context.Request.Url.ToString().ToLower();
				if (context.Request.HttpMethod == "GET" && (searchUrl.Contains(".jpg") || searchUrl.Contains(".png")))
				{
					// Image requests
					byte[] responseArray = wc.DownloadData(new Uri(url));
					context.Response.BinaryWrite(responseArray);
				}
				else
				if (context.Request.HttpMethod == "GET")
				{
					// Plain old GET requests
					string responseString = wc.DownloadString(url);
					context.Response.Write(responseString);
				}
				else
				if (context.Request.HttpMethod == "POST")
				{
					if (context.Request.ContentType == "application/x-www-form-urlencoded")
					{
						// Form posts
						wc.Headers.Add("Content-Type", "application/x-www-form-urlencoded");
						NameValueCollection nameValueCollection = context.Request.Form;
						byte[] responseArray = wc.UploadValues(url, "POST", nameValueCollection);
						string responseString = Encoding.UTF8.GetString(responseArray);
						context.Response.Write(responseString);
					}
					else
					{
						// SOAP, JSON requests
						wc.Headers["Content-Type"] = context.Request.ContentType;
						string soapAction = context.Request.Headers["SOAPAction"];
						if (soapAction != null)
							wc.Headers["SOAPAction"] = soapAction;
						byte[] bytes = context.Request.BinaryRead(context.Request.TotalBytes);
						byte[] responseArray = wc.UploadData(url, bytes);

						//j	string responseString = Encoding.UTF8.GetString(responseArray);
						//j	context.Response.Write(responseString);
						context.Response.BinaryWrite(responseArray);
					}
				}
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.ToString());
				context.Response.Write(ex.ToString());
			}

			wc.Dispose();
		}

		// transfer any cookies from original Silverlight request to real outbound request
		private WebClient SetCookies(WebClient wc, HttpRequest request)
		{
			HttpCookieCollection cookies = request.Cookies;
			foreach (string cookieName in cookies.Keys)
				wc.Headers.Add("Cookie", cookieName + "=" + cookies[cookieName].Value);

			return wc;
		}

		//private WebClient SetHeaders(WebClient wc, NameValueCollection headers)
		//{
		//    foreach (string headerName in headers.Keys)
		//    {
		//        string headerValue = headers[headerName];
		//        if (headerName == "Content-Type" || headerName == "SOAPAction")
		//            wc.Headers.Add(headerName, headerValue);
		//    }
		//
		//    return wc;
		//}
	}
}
