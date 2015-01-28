using System;
using System.Text.RegularExpressions;

namespace PivotServerTools.Internal
{
	// Translates the URL query parameters for a DZI XML request into useful properties.
	internal class DziRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*)/dzi/(.*).dzi", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);
		public string CollectionKey { get; private set; }
		public int ItemId { get; private set; }

		public DziRequest(Uri url)
		{
			Match match = s_matcher.Match(url.AbsolutePath);
			if (match.Groups.Count != 3)
				return; // This object will be rejected due to a bad url format

			CollectionKey = match.Groups[1].Value;
			ItemId = int.Parse(match.Groups[2].Value);
		}
	}
}
