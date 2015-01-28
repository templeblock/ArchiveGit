using System;
using System.Collections.Generic;
using System.Linq;
using CollectionLibrary;

namespace CollectionFactories
{
	public static class TypeBuilder
	{
		private static Dictionary<string, Type> _dictionary = new Dictionary<string, Type>(StringComparer.OrdinalIgnoreCase);

		// Extension for string
		public static object ChangeType(this string valueString, Type type)
		{
			try
			{
				// Nulls are returned if the value is the default value for that type
				//  This results in "no-info" data in PivotViewer
				if (string.IsNullOrEmpty(valueString))
					return null;
				if (type == typeof(Link))
				{ // Special case
					string[] values = valueString.Split('=');
					return (values.Length >= 2 ? new Link() { Name = values[0], Href = values[1] } : null);
				}
				if (type.HasElementType) // Do we want this converted to an array?
				{
					string[] array = valueString.Split(',');
					Type newType = type.GetElementType();
					if (newType == typeof(string))
						return array;
					object[] newArray = new object[array.Length];
					int i = 0;
					foreach (string item in array)
						newArray[i++] = item.ChangeType(newType);
					return newArray;
				}
				object value = Convert.ChangeType(valueString, type);
				if (value == null)
					return null;
				object defaultValue = type.IsValueType ? Activator.CreateInstance(type) : null;
				return (value.Equals(defaultValue) ? null : value);
			}
			catch (Exception ex)
			{
				ex.GetType();
				return valueString;
			}
		}

		// Extension for string
		public static TT ToType<TT>(this string value)
		{
			try
			{
				if (typeof(TT).IsEnum)
					return (TT)Enum.Parse(typeof(TT), value, true/*ignoreCase*/);
				return (TT)Convert.ChangeType(value, typeof(TT));
			}
			catch (Exception ex)
			{
				ex.GetType();
				return default(TT);
			}
		}

		// Extension for string
		public static Type ToType(this string name)
		{
			if (_dictionary.Count == 0)
				Initialize();
			if (_dictionary.Keys.Contains(name))
				return _dictionary[name];
			return typeof(string); // bad types default to string, including bool and bool[]
		}

		private static void Initialize()
		{
			_dictionary.Add("string", typeof(string));
			_dictionary.Add("int", typeof(int));
			_dictionary.Add("double", typeof(double));
			_dictionary.Add("DateTime", typeof(DateTime));
			_dictionary.Add("Link", typeof(Link));
			//_dictionary.Add("bool", typeof(bool));
			_dictionary.Add("string[]", typeof(string[]));
			_dictionary.Add("int[]", typeof(int[]));
			_dictionary.Add("double[]", typeof(double[]));
			_dictionary.Add("DateTime[]", typeof(DateTime[]));
			_dictionary.Add("Link[]", typeof(Link[]));
			//_dictionary.Add("bool[]", typeof(bool[]));
		}
	}
}