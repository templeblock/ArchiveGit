using System;
using System.Collections.ObjectModel;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Data;
using Millicents = System.Int64;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	public class ValueIsNullFalseZeroEmpty : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			bool empty = Value.IsNullFalseZeroEmpty(value);

			if (targetType == typeof(Visibility))
				return (empty ? Visibility.Visible : Visibility.Collapsed);
			if (targetType == typeof(bool?) || targetType == typeof(bool))
				return (empty ? true : false);
			if (targetType == typeof(int?) || targetType == typeof(int))
				return (empty ? 1 : 0);
			if (targetType == typeof(double?) || targetType == typeof(double))
				return (empty ? 1.0 : 0.0);
			throw new NotSupportedException();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return Convert(value, targetType, parameter, culture);
		}
	}

	public class ValueIsNotNullFalseZeroEmpty : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			bool notEmpty = !Value.IsNullFalseZeroEmpty(value);

			if (targetType == typeof(Visibility))
				return (notEmpty ? Visibility.Visible : Visibility.Collapsed);
			if (targetType == typeof(bool?) || targetType == typeof(bool))
				return (notEmpty ? true : false);
			if (targetType == typeof(int?) || targetType == typeof(int))
				return (notEmpty ? 1 : 0);
			if (targetType == typeof(double?) || targetType == typeof(double))
				return (notEmpty ? 1.0 : 0.0);
			throw new NotSupportedException();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return Convert(value, targetType, parameter, culture);
		}
	}

	internal class Value
	{
		internal static bool IsNullFalseZeroEmpty(object value)
		{
			if ((value is bool) && !(bool)value)
				return true;
			if ((value is Visibility) && (Visibility)value == Visibility.Collapsed)
				return true;
			if ((value is string) && string.IsNullOrEmpty((string)value))
				return true;
			if (value != null && value.GetType().IsValueType && System.Convert.ToDouble(value) == 0)
				return true;
			return (value == null);
		}
	}

	public class DateTimeToLongString : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!(value is DateTime))
				throw new Exception(string.Format("'{0}' is not a DateTime", value));

			DateTime dateTime = (DateTime)value;
			return dateTime.ToLongDateString();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class BoolToOpacity : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			bool hidden = (bool)value;
			return hidden ? 0.5 : 1.0;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			double opacity = (double)value;
			return (opacity != 1.0);
		}
	}

	public class ValueToString : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			return value.ToString().ExpandMixedCase();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class EnumToValue : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsEnum)
	            throw new Exception(string.Format("'{0}' is not an enum", value));
			if (!targetType.IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", targetType));

			return System.Convert.ToInt32(value);
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", value));
			if (!targetType.IsEnum)
				throw new Exception(string.Format("'{0}' is not an enum", targetType));

			// During data binding de-referencing, -1 values are passed in, 
			// and we have no choice but to return a null value
			if (!Enum.IsDefined(targetType, value))
				return null;

			return Enum.ToObject(targetType, value);
		}
	}

	public class EnumToName : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsEnum)
	            throw new Exception(string.Format("'{0}' is not an enum", value));

			return value.ToString().ExpandMixedCase();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!(value is string))
				throw new Exception(string.Format("'{0}' is not a string", value));
			if (!targetType.IsEnum)
				throw new Exception(string.Format("'{0}' is not an enum", targetType));

			string name = ((string)value).Replace(" ", null);
			if (!Enum.IsDefined(targetType, name))
				return null;

			return Enum.Parse(targetType, name, true/*ignoreCase*/);
		}
	}

	public class EnumToList : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsEnum)
	            throw new Exception(string.Format("'{0}' is not an enum", value));

			Collection<string> enumList = new Collection<string>();
			FieldInfo[] fieldInfo = value.GetType().GetFields();
			foreach (FieldInfo field in fieldInfo)
			{
				if (!field.IsLiteral)
					continue;
				object fieldValue = field.GetValue(value.GetType());
				int enumValue = System.Convert.ToInt32(fieldValue);
				// As convention, don't put negative enum values in the list so we can "hide" them
				if (enumValue < 0)
					continue;
				string name = fieldValue.ToString().ExpandMixedCase();
				enumList.Add(name);
			}

			return enumList;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class DoubleRound : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			double dvalue = (double)value;
			return dvalue.Round();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return value;
		}
	}

	public class MillicentsToCurrency : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			Millicents millicents = (Millicents)value;
			string format = (parameter is string ? ((string)parameter) : "{0:c}"); // No dollar/euro sign is "{0:n3}"
			return string.Format(culture, format, millicents.ToDouble());
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;

			double dvalue = Double.Parse(value.ToString(), NumberStyles.Currency);
			return dvalue.ToMillicents();
		}
	}

	public class MillicentsToDouble : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			Millicents millicents = (long)value;
			double dvalue = millicents.ToDouble();
			return dvalue;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;

			double dvalue = (double)value;
			return dvalue.ToMillicents();
		}
	}

	public class IconNameToImageUri : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			string iconName = (string)value;
			string imageName = iconName + ".png";
			string imageUri = UriHelper.UriImages(imageName).ToString();
			return imageUri;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;

			string imageUri = (string)value;
			string iconName = Path.GetFileNameWithoutExtension(imageUri);
			return iconName;
		}
	}
}
