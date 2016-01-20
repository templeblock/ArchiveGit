using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace TrumpITReporting.Web
{
	internal static class Extensions
	{
		// Extension for Dictionary<string, string>
		public static void AddSubstitutionPair(this Dictionary<string, string> dictionary, string key, object value)
		{
			key = string.Format("<%{0}%>", key);
			if (!dictionary.ContainsKey(key))
				dictionary.Add(key, (value == null ? "" : value.ToString()));
		}

		// Extension for IEnumerable<TT>
		public static Collection<TT> ToCollection<TT>(this IEnumerable<TT> collection)
		{
			Collection<TT> list = new Collection<TT>();
			foreach (TT item in collection)
				list.Add(item);
			return list;
		}

		// Extension for string
		public static string ToBase64(this string source)
		{
			return Convert.ToBase64String(source.ToByteArray());
		}

		// Extension for string
		public static string FromBase64(this string source)
		{
			return Convert.FromBase64String(source).ToUtfString();
		}

		// Extension for string
		public static byte[] ToByteArray(this string source)
		{
			return Encoding.UTF8.GetBytes(source);
		}

		// Extension for byte[]
		public static string ToUtfString(this byte[] source)
		{
			return Encoding.UTF8.GetString(source, 0, source.Length);
		}
	}
}
