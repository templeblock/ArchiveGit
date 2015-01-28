using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq.Expressions;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Xml.Linq;
using Millicents = System.Int64;

namespace ClassLibrary
{
	internal static class TypeExt
	{
		// Extension for Type
		internal static bool Is(this Type type, Type baseType)
		{
			return (type.Equals(baseType) || type.IsSubclassOf(baseType));
		}
	}

	internal static class RandomExt
	{
		private static Random _Random = new Random();

		internal static bool Bool()
		{
			return _Random.Next(0, 2) != 0;
		}

		internal static double Double()
		{
			return _Random.NextDouble();
		}

		internal static double Double(double max)
		{
			return (_Random.NextDouble() * max);
		}

		internal static double Double(double min, double max)
		{
			return (_Random.NextDouble() * (max - min)) + min;
		}

		internal static int Int()
		{
			return _Random.Next();
		}

		internal static int Int(int max)
		{
			return _Random.Next(max+1);
		}

		internal static int Int(int min, int max)
		{
			return _Random.Next(min, max+1);
		}
	}

	internal static class EnumExt
	{
		// Extension for Enums
		internal static int ToValue(this Enum input)
		{
			return Convert.ToInt32(input);
		}

		// Extension for Enums
		internal static void SetFromInt(this Enum input, int value)
		{
			if (Enum.IsDefined(input.GetType(), value))
				input = (Enum)Enum.ToObject(input.GetType(), value);
		}

		// Extension for flags oriented Enums
		internal static void SetFromIntFlags(this Enum input, int flags)
		{
			input = (Enum)Enum.ToObject(input.GetType(), flags);
		}

		// Extension for flags oriented Enums
		internal static bool ContainsAll(this Enum input, Enum flag)
		{
			int value = input.ToValue();
			int valueMatch = flag.ToValue();
			return (value & valueMatch) == valueMatch;
		}

		// Extension for flags oriented Enums
		internal static bool ContainsAny(this Enum input, Enum flag)
		{
			int value = input.ToValue();
			int valueMatch = flag.ToValue();
			return (value & valueMatch) != 0;
		}
	}

	internal static class AssemblyExt
	{
		// Extension for Assembly
		internal static string Name(this Assembly assembly)
		{
			string name = assembly.FullName;
			return name.Substring(0, name.IndexOf(','));
		}
	}

	internal static class DoubleExt
	{
		// Extension for double
		internal static bool IsNaN(this double dvalue)
		{
			return double.IsNaN(dvalue);
		}

		// Extension for double
		internal static double Round(this double dvalue)
		{
			return Math.Round(dvalue);
		}

		// Extension for double
		internal static double Round(this double dvalue, int digits)
		{
			return Math.Round(dvalue, digits);
		}

		// Extension for double
		internal static Int32 ToInt32(this double dvalue)
		{
			return (Int32)(dvalue + (dvalue >= 0 ? 0.5 : -0.5));
		}

		// Extension for double
		internal static Int64 ToInt64(this double dvalue)
		{
			return (Int64)(dvalue + (dvalue >= 0 ? 0.5 : -0.5));
		}

		// Extension for double
		internal static Millicents ToMillicents(this double dvalue)
		{
			dvalue *= 1000.0;
			return (Millicents)(dvalue + (dvalue >= 0 ? 0.5 : -0.5));
		}
	}

	internal static class StringExt
	{
		// Extension for string
		internal static bool EqualsIgnoreCase(this string source, string target)
		{
			return source.Equals(target, StringComparison.CurrentCultureIgnoreCase);
		}

		// Extension for string
		internal static bool StartsWithIgnoreCase(this string source, string target)
		{
			return source.StartsWith(target, StringComparison.CurrentCultureIgnoreCase);
		}

		// Extension for string
		internal static bool ContainsIgnoreCase(this string source, string target)
		{
			return (source.ToLower().Contains(target.ToLower()));
		}

		// Extension for string
		internal static bool IsEmpty(this string source)
		{
			return string.IsNullOrEmpty(source);
		}

		// Extension for string
		internal static void Trace(this string message)
		{
			Debug.WriteLine(message);
		}

		// Extension for string
		internal static void Alert(this string message)
		{
			Debug.WriteLine(message);
		}

		// Extension for string
		internal static TT ToEnum<TT>(this string source) where TT: struct
		{
			TT value;
			if (Enum.TryParse<TT>(source, true, out value))
				return value;

			string[] array = source.Split(new char[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries);
			int flags = 0;
			foreach (string word in array)
			{
				if (Enum.TryParse<TT>(word, true, out value))
					flags |= Convert.ToInt32(value);
			}

			try
			{
				value = (TT)Enum.ToObject(typeof(TT), flags);
			}
			catch (Exception)
			{
			}

			return value;
		}

		// Extension for string
		internal static int ToInt(this string source)
		{
			try
			{
				if (source == null)
					return 0;
				return Convert.ToInt32(source); // or int.Parse(source)
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return 0;
			}
		}

		// Extension for string
		internal static byte ToByte(this string source)
		{
			try
			{
				if (source == null)
					return 0;
				return Convert.ToByte(source); // or int.Parse(source)
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return 0;
			}
		}

		// Extension for string
		internal static char ToChar(this string source)
		{
			try
			{
				if (source == null)
					return '\0';
				return Convert.ToChar(source); // or int.Parse(source)
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return '\0';
			}
		}

		// Extension for string
		internal static uint ToUInt(this string source)
		{
			try
			{
				if (source == null)
					return 0;
				return Convert.ToUInt32(source); // or int.Parse(source)
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return 0;
			}
		}

		// Extension for string
		internal static UInt64 ToUInt64(this string source)
		{
			try
			{
				if (source == null)
					return 0;
				return Convert.ToUInt64(source); // or int.Parse(source)
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return 0;
			}
		}

		// Extension for string
		internal static Int64 ToInt64(this string source)
		{
			try
			{
				if (source == null)
					return 0;
				return Convert.ToInt64(source); // or int.Parse(source)
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return 0;
			}
		}

		// Extension for string
		internal static double ToDouble(this string source)
		{
			try
			{
				if (source == null)
					return 0;
				return Convert.ToDouble(source);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return 0;
			}
		}

		// Extension for string
		internal static decimal ToDecimal(this string source)
		{
			try
			{
				if (source == null)
					return default(decimal);
				return Convert.ToDecimal(source);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return default(decimal);
			}
		}

		// Extension for string
		internal static bool ToBool(this string source)
		{
			try
			{
				if (source == null)
					return false;
				return Convert.ToBoolean(source);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return (source.ToInt() != 0);
			}
		}

		// Extension for string
		internal static Type ToType(this string source)
		{
			try
			{
				if (source == null)
					return null;
				return Type.GetType(source);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return null;
			}
		}

		internal static DateTime ToDateTime(this string source)
		{
			try
			{
				if (source == null)
					return default(DateTime);
				return Convert.ToDateTime(source);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return default(DateTime);
			}
		}

		// Extension for string
		internal static string ToBase64(this string source)
		{
			return Convert.ToBase64String(source.ToByteArray());
		}

		// Extension for string
		internal static string FromBase64(this string source)
		{
			return Convert.FromBase64String(source).ToUtfString();
		}

		// Extension for string
		internal static byte[] ToByteArray(this string source)
		{
			return Encoding.UTF8.GetBytes(source);
		}

		// Extension for string
		internal static string ExpandMixedCase(this string source)
		{
			StringBuilder builder = new StringBuilder();
			foreach (char c in source)
			{
				if (Char.IsUpper(c) && builder.Length > 0)
					builder.Append(' ');
				builder.Append(c != '_' ? c : ' ');
			}

			return builder.ToString();
		}
	}

	internal static class ByteArrayExt
	{
		// Extension for byte[]
		internal static string ToUtfString(this byte[] source)
		{
			return Encoding.UTF8.GetString(source, 0, source.Length);
		}

		// Extension for byte[]
		internal static string ToHexString(this byte[] source)
		{
			StringBuilder builder = new StringBuilder();
			foreach (byte b in source)
				builder.Append(b.ToString("x2"));
			return builder.ToString();
		}
	}

	internal static class ExceptionExt
	{
		// Extension for Exception
		internal static void Alert(this Exception ex)
		{
			string message = ex.Details();
			//Debug.Assert(false, message);
			Debug.WriteLine(message);
		}

		// Extension for Exception
		internal static void DebugOutput(this Exception ex, bool micro = false)
		{
#if DEBUG
			if (micro)
				Debug.WriteLine(ex.MicroDetails());
			else
				Debug.WriteLine(ex.MiniDetails());
#endif
		}

		// Extension for Exception
		internal static string Details(this Exception ex)
		{
			string message = ex.MiniDetails();

			if (ex.Data != null)
			{
				try
				{
					foreach (DictionaryEntry de in ex.Data)
						message += "Key: " + de.Key + "; Value: " + de.Value + Environment.NewLine;
				}
				catch (Exception) { }
			}

			if (ex.InnerException != null)
			{
				try
				{
					message += ex.InnerException.Details();
				}
				catch (Exception) { }
			}

			return message;
		}

		// Extension for Exception
		internal static string MicroDetails(this Exception ex)
		{
			string message = "";
			try
			{
				string input = ex.GetType().ToString();
				message += RegexHelper.ReplacePattern(input, RegexHelper.WildcardToPattern("System.", WildcardSearch.Anywhere), "") + ": ";
			}
			catch (Exception) { }
			try
			{
				message += ex.Message + Environment.NewLine;
			}
			catch (Exception) { }

			return message;
		}

		// Extension for Exception
		internal static string MiniDetails(this Exception ex)
		{
			string message = "";
			try
			{
				string input = ex.GetType().ToString();
				message += RegexHelper.ReplacePattern(input, RegexHelper.WildcardToPattern("System.", WildcardSearch.Anywhere), "") + ": ";
			}
			catch (Exception) { }
			try
			{
				message += ex.Message + Environment.NewLine;
			}
			catch (Exception) { }
			try
			{
				string stackTrace = ex.StackTrace;
				stackTrace = RegexHelper.ReplacePattern(stackTrace, RegexHelper.WildcardToPattern("(*)", WildcardSearch.Anywhere), "()");
				stackTrace = RegexHelper.ReplacePattern(stackTrace, RegexHelper.WildcardToPattern("System.", WildcardSearch.Anywhere), "");
				stackTrace = RegexHelper.ReplacePattern(stackTrace, RegexHelper.WildcardToPattern("   at ", WildcardSearch.Anywhere), "\t");
				message += stackTrace + Environment.NewLine;
			}
			catch (Exception) { }

			return message;
		}
	}

	internal static class XDocumentExt
	{
		// Extension for XDocument
		internal static XDocument ReplacePattern(this XDocument xmlDoc, string pattern, int matchGroup, Dictionary<string, string> replacementDictionary, out int count)
		{
			count = 0;
			try
			{
				string xmlString = xmlDoc.ToString(SaveOptions.DisableFormatting);
				xmlString = RegexHelper.ReplacePattern(xmlString, pattern, matchGroup, replacementDictionary, out count);
				if (count > 0)
					return XDocument.Parse(xmlString);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return xmlDoc;
		}
	}

	internal static class XElementExt
	{
		// Extension for XElement
		internal static string AttributeValue(this XElement element, string name, string defaultValue = null)
		{
			if (element == null)
				return defaultValue;
			XAttribute attribute = element.Attribute(name);
			if (attribute == null)
				return defaultValue;
			if (element.Attribute(name).Value.ToLower() == "null")
				return defaultValue;
			return element.Attribute(name).Value;
		}

		public static string InnerValue(this XElement element, string defaultValue = null)
		{
			if (element == null || element.Value.ToLower() == "null")
				return defaultValue;
			return element.Value;
		}
	}

	internal static class CollectionExt
	{
		// Extension for IEnumerable<TT>
		internal static TTcollection ToObservableCollection<TTcollection, TT>(this IEnumerable<TT> enumerable) where TTcollection : ObservableCollection<TT>
		{
			TTcollection collection = Activator.CreateInstance<TTcollection>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}

		// Extension for IEnumerable<TT>
		internal static ObservableCollection<TT> ToObservableCollection<TT>(this IEnumerable<TT> enumerable)
		{
			ObservableCollection<TT> collection = new ObservableCollection<TT>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}

		// Extension for IEnumerable<TT>
		internal static Collection<TT> ToCollection<TT>(this IEnumerable<TT> enumerable)
		{
			Collection<TT> collection = new Collection<TT>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}
	}

	internal static class ExpressionExt
	{
		// Extension for Expression<Func<T>>
		// usage: string name = ExpressionExt.ToPropertyName(() => TheProperty);
		internal static string ToPropertyName<T>(this Expression<Func<T>> expression)
		{
			if (expression == null)
				return null;

			// this cast will always succeed
			return ((MemberExpression)expression.Body).Member.Name;
		}
	}

	internal static class IdHelper
	{
		private static RNGCryptoServiceProvider m_Random;

		internal static Int64 RandomLongTimeOrder()
		{
			if (m_Random == null)
				m_Random = new RNGCryptoServiceProvider();

			byte[] buffer = new byte[4];
			m_Random.GetBytes(buffer);
			uint lo = BitConverter.ToUInt32(buffer, 0);
			TimeSpan timeSpan = DateTime.Now - new DateTime(2009, 1, 1);
			uint hi = (uint)timeSpan.TotalSeconds;
			Int64 value = ((Int64)hi << 32) | lo;
			return value & 0x7fffffffffffffff; // No negative id's
		}

		internal static Int64 RandomLong()
		{
			if (m_Random == null)
				m_Random = new RNGCryptoServiceProvider();

			byte[] buffer = new byte[8];
			m_Random.GetBytes(buffer);
			Int64 value = BitConverter.ToInt64(buffer, 0);
			return value & 0x7fffffffffffffff; // No negative id's
		}

		internal static Int64 GuidToLong()
		{ // Sample ever other bit to convert 128 bits to 64
			Guid guid = Guid.NewGuid();
			byte[] buffer = guid.ToByteArray();
			Int64 value = 0;
			for (int i = 0; i < 128; i += 2)
			{
				value <<= 1;
				int index = i / 8;
				byte byt = buffer[index];
				int bit = 7 - (i - (index * 8));
				if ((byt & (1 << bit)) != 0)
					value |= 1;
			}

			return value & 0x7fffffffffffffff; // No negative id's
		}
	}

	internal static class DictionaryExt
	{
		// Extension for IDictionary<string, TValue>
		internal static bool ContainsKeyIgnoreCase<TValue>(this IDictionary<string, TValue> dictionary, string key)
		{
			foreach (string keyItem in dictionary.Keys)
			{
				if (key.EqualsIgnoreCase(keyItem))
					return true;
			}

			return false;
		}

		// Extension for IDictionary<string, TValue>
		internal static TValue GetValueIgnoreCase<TValue>(this IDictionary<string, TValue> dictionary, string key, TValue defaultValue)
		{
			foreach (string keyItem in dictionary.Keys)
			{
				if (key.EqualsIgnoreCase(keyItem))
					return dictionary[keyItem];
			}

			return defaultValue;
		}

		// Extension for IDictionary<string, TValue>
		internal static TValue GetValueIgnoreCase<TValue>(this IDictionary<string, TValue> dictionary, string key)
		{
			return dictionary.GetValueIgnoreCase(key, default(TValue));
		}
	}
}
