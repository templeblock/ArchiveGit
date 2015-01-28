using System;
using System.Collections.Generic;

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
	}
}
