using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Web;

namespace Payroll.Web
{
	public static class Extensions
	{
		// Extension for string
		public static string ReplaceWhiteSpace(this string input, string value)
		{
			Regex regex = new Regex(@"\s+");
			return regex.Replace(input, value);
		}

		// Extension for string
		// ReplacePattern Usage:
		//	Dictionary<string, string> variables = new Dictionary<string, string>();
		//	variables.Add("zipCode", "01810");
		//	variables.Add("comcast", "verizon");
		//	string pattern = "([a-zA-Z0-9]+)"; // "(&amp;)([a-zA-Z0-9]+)([;])";
		//	int numReplacements = 0;
		//	str = str.ReplacePattern(pattern, 1, variables, out numReplacements);
		public static string ReplacePattern(this string input, string pattern, int matchGroup, Dictionary<string, string> variables, out int numReplacements)
		{
			int ncount = 0;
			MatchEvaluator matchFunction = delegate(Match match)
			{
				string key = match.Groups[matchGroup].Value;
				string value;
				if (!variables.TryGetValue(key, out value))
					return match.Value;

				ncount++;
				return value;
			};

			Regex regex = new Regex(pattern);
			string output = regex.Replace(input, matchFunction);
			numReplacements = ncount;
			return output;
		}

		// Extension for HttpRequest
		public static string Value(this HttpRequest request, string name)
		{
			if (string.IsNullOrEmpty(request.Form[name]))
				return string.Empty;
			return request.Form[name].ToString();
		}

		// Extension for HttpRequest
		public static string ValueEncoded(this HttpRequest request, string name)
		{
			return HttpUtility.UrlEncode(request.Value(name));
		}

		// Extension for HttpRequest
		public static string ValueDecoded(this HttpRequest request, string name)
		{
			return HttpUtility.UrlDecode(request.Value(name));
		}
	}
}