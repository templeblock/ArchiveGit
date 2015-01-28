using System;
using System.Text.RegularExpressions;

namespace PivotServerTools.Internal
{
	// Translates the URL query parameters for a Deep Zoom Image tile request into useful properties.
	internal class DziImageRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*)/dzi/(.*)_files/(.*)/(.*)_(.*).jpg", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);
		public string CollectionKey { get; private set; }
		public int ItemId { get; private set; }
		public int Level { get; private set; }
		public int X { get; private set; }
		public int Y { get; private set; }

		public DziImageRequest(Uri url)
		{
			Match match = s_matcher.Match(url.AbsolutePath);
			if (match.Groups.Count != 6)
				return; // This object will be rejected due to a bad url format

			CollectionKey = match.Groups[1].Value;
			ItemId = int.Parse(match.Groups[2].Value);
			Level = int.Parse(match.Groups[3].Value);
			X = int.Parse(match.Groups[4].Value);
			Y = int.Parse(match.Groups[5].Value);
		}
	}
}
