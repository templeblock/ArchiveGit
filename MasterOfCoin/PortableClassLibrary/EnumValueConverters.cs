using System;
using System.Linq;
using System.Reflection;
using Windows.UI.Xaml.Data;

namespace ClassLibrary
{
	public class EnumToIndex : IValueConverter
	{
        public object Convert(object value, Type targetType, object parameter, string language)
		{
			if (value == null) // Special case
				return -1;
            if (!value.GetType().GetTypeInfo().IsEnum)
	            throw new Exception(string.Format("'{0}' is not an enum", value));
		    int index = (int)value;
			return index;
		}

        public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			if (value == null) // Special case
				return null;
            if (!value.GetType().GetTypeInfo().IsValueType)
                throw new Exception(string.Format("'{0}' is not a value type", value));
            int index = (int)value;
			if (index < 0) // Special case
				return null;
            return (targetType == typeof(object) ? index : Enum.ToObject(targetType, index));
		}
	}

	public class EnumToArray : IValueConverter
	{
        public object Convert(object value, Type targetType, object parameter, string language)
		{
			if (value == null) // Special case
				return null;
            if (!value.GetType().GetTypeInfo().IsEnum)
	            throw new Exception(string.Format("'{0}' is not an enum", value));
			Type enumType = value.GetType();
            var fieldInfo = enumType.GetTypeInfo().DeclaredFields;
            return fieldInfo.Where(field => field.IsLiteral).Select(field => field.GetValue(enumType).ToString()).ToArray();
		}

        public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			throw new Exception();
		}
	}
}
