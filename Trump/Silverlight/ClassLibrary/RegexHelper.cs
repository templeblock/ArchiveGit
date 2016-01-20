using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace ClassLibrary
{
	// Used in WildcardToRegex
	internal enum WildcardSearch
	{
		Exact,
		Anywhere,
		StartsWith,
		EndsWith,
	}

	internal class RegexHelper
	{
		// Extension for string
		// Usage:
		//	Dictionary<string, string> replacementDictionary = new Dictionary<string, string>();
		//	replacementDictionary.Add("zipCode", "01810");
		//	replacementDictionary.Add("comcast", "verizon");
		//	string pattern = "([a-zA-Z]+)"; // "(&amp;)([a-zA-Z]+)([;])";
		//	int count = 0;
		//	str = str.ReplacePattern(pattern, 1, replacementDictionary, out count);
		internal static string ReplacePattern(string input, string pattern, int matchGroup, Dictionary<string, string> replacementDictionary, out int count)
		{
			int ncount = 0;
			MatchEvaluator matchFunction = delegate(Match match)
			{
				string key = match.Groups[matchGroup].Value;
				string value;
				if (!replacementDictionary.TryGetValue(key, out value))
					return match.Value;

				ncount++;
				return value;
			};

			/*static readonly*/
			Regex regex = new Regex(pattern, RegexOptions.None/*Compiled*/);
			string output = regex.Replace(input, matchFunction);
			count = ncount;
			return output;
		}

		// Extension for string
		internal static string ReplacePattern(string input, string pattern, string replace)
		{
			Regex regex = new Regex(pattern, RegexOptions.None/*Compiled*/);
			return regex.Replace(input, replace);
		}

		// Extension for string
		internal static string ReplaceCStyleComments(string input, string replace)
		{
			return ReplacePattern(input, @"/\*(?:.|[\n\r])*?\*/", replace);
		}

		// Extension for string
		internal static string WildcardToPattern(string pattern, WildcardSearch matchType)
		{
			string escapedPattern = Regex.Escape(pattern);
			escapedPattern = escapedPattern.Replace("\\*", ".*?"); // lazy star not a greedy star
			escapedPattern = escapedPattern.Replace("\\?", ".");
			if (matchType == WildcardSearch.Exact)
				return "^" + escapedPattern + "$";
			if (matchType == WildcardSearch.Anywhere)
				return escapedPattern;
			if (matchType == WildcardSearch.StartsWith)
				return "^" + escapedPattern;
			if (matchType == WildcardSearch.EndsWith)
				return escapedPattern + "$";
			return pattern;
		}
	}
}
