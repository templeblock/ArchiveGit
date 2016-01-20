using System;
using System.Windows;
using System.Windows.Browser;

namespace ClassLibrary
{
	internal static class UriHelper
	{
		// Extension for Uri
		internal static Uri Site(this Uri uri)
		{
			try
			{
				if (uri.LocalPath.IsEmpty())
					return uri;
				string path = uri.LocalPath + uri.Fragment;
				int index = uri.AbsoluteUri.LastIndexOf(path);
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
			// as in http://www.domain.com/appfolder/Index.aspx#/main/sub
			return HtmlPage.Document.DocumentUri;
		}

		internal static Uri UriDocument()
		{
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
			// as in http://www.domain.com
			return HtmlPage.Document.DocumentUri.Site();
		}

		internal static string DocumentFragment()
		{
			// "#/main/sub" is returned for uri http://www.domain.com/appfolder/Index.aspx#/main/sub
			string other = HtmlPage.Window.CurrentBookmark; // without the "#"
			return HtmlPage.Document.DocumentUri.Fragment;
		}

		internal static Uri UriAppRelative(string relativeUri)
		{
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
