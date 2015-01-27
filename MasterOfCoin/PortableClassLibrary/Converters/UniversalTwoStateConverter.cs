using System;
using System.Reflection;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;

namespace ClassLibrary
{
    public class UniversalTwoStateConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string culture)
        {
            bool isPositive;

            if (parameter == null)
            { // If the parameter is null
                isPositive = Value.IsPositive(value);
            }
            else
            {// If the parameter is NOT null
                bool recognized;
                bool parameterAsBool = Value.IsPositive(parameter, out recognized);

                if (recognized)
                { // If the parameter is recognized
                    isPositive = Value.IsPositive(value);
                    if (!parameterAsBool) // Invert if parameterAsBool == false
                        isPositive = !isPositive;
                }
                else
                { // If the parameter is NOT recognized
                    isPositive = (value != null && value.ToString() == parameter.ToString());
                }
            }

            object targetValue = Value.Compute(targetType, isPositive);
            if (targetValue != null)
                return targetValue;
            throw new NotSupportedException();
        }

        public object ConvertBack(object value, Type targetType, object parameter, string culture)
        {
            bool invert = (parameter != null && !bool.Parse(parameter.ToString())); // Invert if parameter == false
            return Convert(value, targetType, !invert, culture);
        }
    }

    public static class Value
    {
        internal static bool IsPositive(object value)
        {
            bool recognized;
            return IsPositive(value, out recognized);
        }

        internal static bool IsPositive(object value, out bool recognized)
        {
            recognized = true;
            if (value == null)
                return false;
            if (value is bool)
                return (bool)value;
            if (value is Visibility)
                return (Visibility)value == Visibility.Visible;
            if (value is ScrollBarVisibility)
                return (ScrollBarVisibility)value == ScrollBarVisibility.Auto || (ScrollBarVisibility)value == ScrollBarVisibility.Visible;
            if (value.GetType().GetTypeInfo().IsValueType)
                return (Convert.ToDouble(value) != 0);
            if (value is string)
            {
                string valueAsString = value.ToString();

                bool valueAsBool;
                bool valueIsBool = bool.TryParse(valueAsString, out valueAsBool);
                if (valueIsBool)
                    return valueAsBool;

                Visibility valueAsVisibility;
                try
                {
                    bool valueIsVisibility = Enum.TryParse(valueAsString, out valueAsVisibility);
                    if (valueIsVisibility)
                        return valueAsVisibility == Visibility.Visible;

                    ScrollBarVisibility valueAsScrollBarVisibility;
                    bool valueIsScrollBarVisibility = Enum.TryParse(valueAsString, out valueAsScrollBarVisibility);
                    if (valueIsScrollBarVisibility)
                        return valueAsScrollBarVisibility == ScrollBarVisibility.Auto || valueAsScrollBarVisibility == ScrollBarVisibility.Visible;
                }
                catch (FormatException exc)
                {
					exc.Details();
                }

                double valueAsDouble;
                bool valueIsDouble = double.TryParse(valueAsString, out valueAsDouble);
                if (valueIsDouble)
                    return (valueAsDouble != 0);

                recognized = false;
                return !string.IsNullOrEmpty(valueAsString);
            }

            recognized = false;
            return true;
        }

        public static object Compute(Type targetType, bool isPositive)
        {
            if (targetType == typeof(bool?) || targetType == typeof(bool))
                return isPositive;
            if (targetType == typeof(Visibility))
                return (isPositive ? Visibility.Visible : Visibility.Collapsed);
            if (targetType == typeof(ScrollBarVisibility))
                return (isPositive ? ScrollBarVisibility.Auto : ScrollBarVisibility.Hidden);
            if (targetType == typeof(int?) || targetType == typeof(int))
                return (isPositive ? 1 : 0);
            if (targetType == typeof(double?) || targetType == typeof(double))
                return (isPositive ? 1.0 : 0.0);
            return null;
        }
    }
}
