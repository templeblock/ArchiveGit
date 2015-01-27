using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Browser;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	internal static class UriHelper
	{
		// Extension for Uri
		internal static IDictionary<string, string> QueryDictionary(this Uri uri, bool decode = false)
		{
			if (uri == null || string.IsNullOrEmpty(uri.Query))
				return null;

			IDictionary<string, string> dictionary = new Dictionary<string, string>(StringComparer.Ordinal);
			string query = uri.Query;
			if (query[0] == '?')
				query = query.Substring(1);

			string[] parameters = query.Split("&".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
			foreach (string parameter in parameters)
			{
				int index = parameter.IndexOf("=", StringComparison.Ordinal);
				string key = (index == -1 ? parameter : parameter.Substring(0, index));
				string value = (index == -1 ? string.Empty : parameter.Substring(index + 1));
				dictionary.Add(decode ? HttpUtility.UrlDecode(key) : key,
							   decode ? HttpUtility.UrlDecode(value) : value);
			}

			return dictionary;
		}

		// Extension for Uri
		internal static Uri Site(this Uri uri)
		{
			try
			{
				if (uri.LocalPath.IsEmpty())
					return uri;
				string path = uri.LocalPath + uri.Fragment;
				int index = uri.AbsoluteUri.LastIndexOf(path);
				if (index < 0)
					return uri;
				Uri baseUri = new Uri(uri.AbsoluteUri.Substring(0, index), UriKind.Absolute);
				return baseUri;
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return uri;
			}
		}

		// Extension for Uri
		internal static Uri Append(this Uri baseUri, string relativeUri)
		{
			return new Uri(baseUri, relativeUri);
		}

		internal static Uri UriXap()
		{
			// as in http://www.domain.com/appfolder/ClientBin/MyApp.xap
			return Application.Current.Host.Source;
		}

		internal static string RelativeXap()
		{
			// as in /appfolder/ClientBin/MyApp.xap
			return Application.Current.Host.Source.AbsolutePath;
		}

		internal static Uri UriDocumentFull()
		{
			if (Helper.IsInDesignTool)
				return new Uri("http://microsoft.com");

			// as in http://www.domain.com/appfolder/Index.aspx#/main/sub
			return HtmlPage.Document.DocumentUri;
		}

		internal static Uri UriDocument()
		{
			if (Helper.IsInDesignTool)
				return new Uri("http://microsoft.com");

			// as in http://www.domain.com/appfolder/Index.aspx
			Uri uri = HtmlPage.Document.DocumentUri;
			if (uri.Fragment.IsEmpty())
				return uri;
			int index = uri.AbsoluteUri.IndexOf(uri.Fragment);
			uri = new Uri(uri.AbsoluteUri.Substring(0, index), UriKind.Absolute);
			return uri;
		}

		internal static Uri UriSite()
		{
			if (Helper.IsInDesignTool)
				return new Uri("http://microsoft.com");

			// as in http://www.domain.com
			return HtmlPage.Document.DocumentUri.Site();
		}

		internal static string DocumentFragment()
		{
			if (Helper.IsInDesignTool)
				return string.Empty;

			// "#/main/sub" is returned for uri http://www.domain.com/appfolder/Index.aspx#/main/sub
			string other = HtmlPage.Window.CurrentBookmark; // without the "#"
			return HtmlPage.Document.DocumentUri.Fragment;
		}

		internal static Uri UriAppRelative(string relativeUri)
		{
			if (Helper.IsInDesignTool)
				return new Uri("http://microsoft.com");

			// as in http://www.domain.com/appfolder/relativeUri
			Uri uri = new Uri(HtmlPage.Document.DocumentUri, "./" + relativeUri);
			return uri;
		}

		internal static Uri UriImages(string relativeUri)
		{
			// as in http://www.domain.com/appfolder/Images
			return UriAppRelative("Images/" + relativeUri);
		}
	}
}
