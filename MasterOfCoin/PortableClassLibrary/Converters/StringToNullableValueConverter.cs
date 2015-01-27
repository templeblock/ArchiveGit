using System;
using Windows.UI.Xaml.Data;

namespace ClassLibrary
{
	public class StringToNullableValueConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string language)
		{
			try
			{
				var nullable = value as double?;
				if (nullable != null)
					return nullable.Value.ToString();
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			return string.Empty;
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			try
			{
				var text = value as string;
				return (!string.IsNullOrWhiteSpace(text) ? (object)double.Parse(text) : null);
			}
			catch (Exception ex)
			{
				ex.GetType();
				return double.NaN;
			}
		}
	}
}