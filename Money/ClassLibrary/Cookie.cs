using System;
using System.Windows.Browser;

namespace ClassLibrary
{
	internal static class Cookie
	{
		internal static bool Exists(string key, string value)
		{
			return HtmlPage.Document.Cookies.Contains(key + "=" + value);
		}

		internal static string Read(string key)
		{
			string[] cookies = HtmlPage.Document.Cookies.Split(';');
			foreach (string cookie in cookies)
			{
				string[] keyValuePair = cookie.Split('=');
				if (keyValuePair.Length == 2 && key == keyValuePair[0].Trim())
					return keyValuePair[1].Trim();
			}

			return null;
		}

		internal static void Write(string key, string value, int expireDays)
		{
			// expireDays = 0, indicates a session cookie that will not be written to disk 
			// expireDays = -1, indicates that the cookie will not expire and will be permanent
			// expireDays = n, indicates that the cookie will expire in “n” days
			string expires = "";
			if (expireDays != 0)
			{
				DateTime expireDate = (expireDays > 0 ?
					DateTime.Now + TimeSpan.FromDays(expireDays) : 
					DateTime.MaxValue);
				expires = ";expires=" + expireDate.ToString("R");
			}

			string cookie = key + "=" + value + expires;
			HtmlPage.Document.SetProperty("cookie", cookie);
		}

		internal static void Delete(string key)
		{
			DateTime expireDate = DateTime.Now - TimeSpan.FromDays(1); // yesterday
			string expires = ";expires=" + expireDate.ToString("R");
			string cookie = key + "=" + expires;
			HtmlPage.Document.SetProperty("cookie", cookie);
		}
	}
}
