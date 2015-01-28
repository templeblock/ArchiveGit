using System;
using System.Text.RegularExpressions;

namespace PivotServerTools.Internal
{
	// Translates the URL query parameters for a tile image request into useful properties.
	internal class ImageRequest
	{
		static readonly Regex s_matcher = new Regex(".*/(.*)_files/(.*)/(.*)_(.*).jpg", RegexOptions.Compiled
			| RegexOptions.CultureInvariant | RegexOptions.IgnoreCase | RegexOptions.Singleline);
		public string CollectionKey { get; private set; }
		public int Level { get; private set; }
		public int X { get; private set; }
		public int Y { get; private set; }

		public ImageRequest(Uri url)
		{
			Match match = s_matcher.Match(url.AbsolutePath);
			if (match.Groups.Count != 5)
				return; // This object will be rejected due to a bad url format

			CollectionKey = match.Groups[1].Value;
			Level = int.Parse(match.Groups[2].Value);
			X = int.Parse(match.Groups[3].Value);
			Y = int.Parse(match.Groups[4].Value);
		}
	}
}
