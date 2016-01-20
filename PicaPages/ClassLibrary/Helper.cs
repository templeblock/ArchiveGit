using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Xml.Linq;

namespace ClassLibrary
{
	public static class FrameworkElementExt
	{
		// Extension for Assembly
		public static string Name(this Assembly assembly)
		{
			string name = assembly.FullName;
			return name.Substring(0, name.IndexOf(','));
		}

		// Extension for FrameworkElement
		public static void ApplyStyle(this FrameworkElement element, FrameworkElement elementResource, string name)
		{
			Style style = elementResource.FindStyle(name);
			if (!element.ApplyStyle(style))
				Helper.Alert("Error applying style '" + name + "' to " + element.ToString() + " " + element.Name);
		}

		// Extension for FrameworkElement
		public static bool ApplyStyle(this FrameworkElement element, Style style)
		{
			try
			{
				if (style == null)
					return false;

				element.Style = style;
				return true;
			}
			catch (Exception e)
			{
				//j This exception occurs when setting styles more than once
				Debug.WriteLine(e.Message);
			}

			return false;
		}

		// Extension for FrameworkElement
		public static Style FindStyle(this FrameworkElement element, string name)
		{
			return element.FindResource(name) as Style;
		}

		// Extension for FrameworkElement
		public static object FindResource(this FrameworkElement element, string name)
		{
			try
			{
				if (element == null)
					element = Application.Current.RootVisual as FrameworkElement;

				while (element != null)
				{
					if (element.Resources.Contains(name))
						return element.Resources[name];
					element = element.Parent as FrameworkElement;
				}

				if (Application.Current.Resources.Contains(name))
					return Application.Current.Resources[name];
			}
			catch (Exception e)
			{
				Debug.WriteLine(e.Message);
				e.Assert();
			}

			Helper.Alert("Can't find resource: " + name);
			return null;
		}
	}

	public static class ColorExt
	{
		// Extension for Color
		public static SolidColorBrush ToBrush(this Color color)
		{
			return new SolidColorBrush(color);
		}
	}

	public static class StringExt
	{
		// Extension for string
		public static TT ToEnum<TT>(this string source)
		{
			return (TT)Enum.Parse(typeof(TT), source, true);
		}

		// Extension for string
		public static int ToInt(this string source)
		{
			return Convert.ToInt32(source);
			//j alternatively, return int.Parse(source);
		}

		// Extension for string
		public static bool ToBool(this string source)
		{
			try
			{
				return Convert.ToBoolean(source);
			}
			catch (Exception)
			{
			}

			return (source.ToInt() != 0);
		}

		// Extension for string
		public static string ToBase64String(this string source)
		{
			return Convert.ToBase64String(source.ToByteArray());
		}

		// Extension for string
		public static string FromBase64String(this string source)
		{
			return Convert.FromBase64String(source).ToUtfString();
		}

		// Extension for string
		public static byte[] ToByteArray(this string source)
		{
			UTF8Encoding encoding = new UTF8Encoding();
			return encoding.GetBytes(source);
		}

		// Extension for string
		public static Color ToNamedColor(this string colorName)
		{
			colorName = colorName.ToLower();
			if (colorName == "black") return Colors.Black;
			if (colorName == "blue") return Colors.Blue;
			if (colorName == "brown") return Colors.Brown;
			if (colorName == "cyan") return Colors.Cyan;
			if (colorName == "darkgray") return Colors.DarkGray;
			if (colorName == "gray") return Colors.Gray;
			if (colorName == "green") return Colors.Green;
			if (colorName == "lightgray") return Colors.LightGray;
			if (colorName == "magenta") return Colors.Magenta;
			if (colorName == "orange") return Colors.Orange;
			if (colorName == "purple") return Colors.Purple;
			if (colorName == "red") return Colors.Red;
			if (colorName == "white") return Colors.White;
			if (colorName == "yellow") return Colors.Yellow;
			return Colors.Transparent;
		}

		// Extension for string
		public static Color ToColor(this string sColor)
		{
			try
			{
				Color color = ToNamedColor(sColor);
				if (color != Colors.Transparent)
					return color;

				if (sColor.IndexOf('#') == 0 && sColor.Length == 7)
				{
					return Color.FromArgb(0xFF,
						byte.Parse(sColor.Substring(1, 2), NumberStyles.HexNumber),
						byte.Parse(sColor.Substring(3, 2), NumberStyles.HexNumber),
						byte.Parse(sColor.Substring(5, 2), NumberStyles.HexNumber));
				}

				if (sColor.IndexOf('#') == 0 && sColor.Length == 9)
				{
					return Color.FromArgb(
						byte.Parse(sColor.Substring(1, 2), NumberStyles.HexNumber),
						byte.Parse(sColor.Substring(3, 2), NumberStyles.HexNumber),
						byte.Parse(sColor.Substring(5, 2), NumberStyles.HexNumber),
						byte.Parse(sColor.Substring(7, 2), NumberStyles.HexNumber));
				}
			}
			catch (Exception e)
			{
				Debug.WriteLine(e.Message);
				e.Assert();
			}

			return Colors.Red;
		}

		// Extension for string
		public static string ReplacePattern(this string input, string pattern, int matchGroup, Dictionary<string, string> replacementDictionary, out int count)
		{
			int ncount = 0;
			MatchEvaluator matchFunction =
				delegate(Match match)
				{
					string key = match.Groups[matchGroup].Value;
					string value;
					if (!replacementDictionary.TryGetValue(key, out value))
						return match.Value;

					ncount++;
					return value;
				};

			/*static readonly*/ Regex m_Regex = new Regex(pattern, RegexOptions.None/*Compiled*/);
			string output = m_Regex.Replace(input, matchFunction);
			count = ncount;
			return output;
		}
	}

	public static class ByteArrayExt
	{
		// Extension for byte[]
		public static string ToUtfString(this byte[] source)
		{
			UTF8Encoding encoding = new UTF8Encoding();
			return encoding.GetString(source, 0, source.Length);
		}

		// Extension for byte[]
		public static string ToHexString(this byte[] source)
		{
			StringBuilder builder = new StringBuilder();
			foreach (byte b in source)
				builder.Append(b.ToString("x2"));
			return builder.ToString();
		}
	}

	public static class UIElementExt
	{
		// Extension for UIElement
		public static void SetTop(this UIElement element, double value)
		{
			element.SetValue(Canvas.TopProperty, value);
		}

		// Extension for UIElement
		public static double GetTop(this UIElement element)
		{
			return (double)element.GetValue(Canvas.TopProperty);
		}

		// Extension for UIElement
		public static void SetLeft(this UIElement element, double value)
		{
			element.SetValue(Canvas.LeftProperty, value);
		}

		// Extension for UIElement
		public static double GetLeft(this UIElement element)
		{
			return (double)element.GetValue(Canvas.LeftProperty);
		}

		// Extension for UIElement
		public static int GetZIndex(this UIElement element)
		{
			return (int)element.GetValue(Canvas.ZIndexProperty);
		}

		// Extension for UIElement
		public static void SetZIndex(this UIElement element, int zIndex)
		{
			element.SetValue(Canvas.ZIndexProperty, zIndex);
		}

		// Extension for UIElement
		public static bool IsVisible(this UIElement element)
		{
			return (element.Visibility == Visibility.Visible);
		}

		// Extension for UIElement
		public static void Hide(this UIElement element)
		{
			if (element.Visibility != Visibility.Collapsed)
				element.Visibility = Visibility.Collapsed;
		}

		// Extension for UIElement
		public static void Show(this UIElement element)
		{
			if (element.Visibility != Visibility.Visible)
				element.Visibility = Visibility.Visible;
		}

		public static List<UIElement> HitTest(this UIElement element, Point point)
		{
			return VisualTreeHelper.FindElementsInHostCoordinates(point, element) as List<UIElement>;
		}

		public static List<UIElement> HitTest(this UIElement element, Rect rect)
		{
			return VisualTreeHelper.FindElementsInHostCoordinates(rect, element) as List<UIElement>;
		}

		public static void SetToolTip(this UIElement element, object content)
		{
			ToolTipService.SetToolTip(element, content);
		}

		public static object GetToolTip(this UIElement element)
		{
			return ToolTipService.GetToolTip(element);
		}
	}

	public static class ExceptionExt
	{
		// Extension for Exception
		public static void Assert(this Exception e)
		{
			string message = "";
			try { message += e.GetType().ToString() + ": "; }
			catch (Exception) { }
			try { message += e.Message + "\r\n\r\n"; }
			catch (Exception) { }
			try { message += e.StackTrace + "\r\n\r\n"; }
			catch (Exception) { }
			if (e.Data != null)
				try { foreach (DictionaryEntry de in e.Data) message += "Key: " + de.Key + "; Value: " + de.Value + "\r\n"; }
				catch (Exception) { }
			//Debug.Assert(false, message);
			Helper.Alert(message);
		}
	}

	public static class DependencyObjectExt
	{
		// Extension for any DependencyObject class
		public static TT Clone<TT>(this TT source) where TT : DependencyObject
		{
			Type type = source.GetType();
			TT cloned = (TT)Activator.CreateInstance(type);
			foreach (PropertyInfo propertyInfo in type.GetProperties())
			{
				MethodInfo getter = propertyInfo.GetGetMethod(false/*nonPublic*/);
				MethodInfo setter = propertyInfo.GetSetMethod(false/*nonPublic*/);
				if (getter == null || setter == null)
					continue;

				// Handle Non-indexer properties
				if (propertyInfo.Name != "Item")
				{
					// get property from source
					object getValue = propertyInfo.GetValue(source, new object[] {});
					//object getValue = getter.Invoke(source, new object[] {});

					// clone if needed
					if (getValue != null && getValue is DependencyObject)
						getValue = Clone(getValue as DependencyObject);

					// set property on cloned
					try
					{
						propertyInfo.SetValue(cloned, getValue, null);
						//setter.Invoke(cloned, new object[] { getValue });
					}
					catch (Exception e)
					{
						System.Diagnostics.Debug.WriteLine(e.Message);
					}
				}
				// Handle indexer properties
				else
				{
					// get count for indexer
					int numberofItemInColleciton = (int)
						propertyInfo.ReflectedType.GetProperty("Count").GetGetMethod().Invoke(source, new object[] {});

					// run on indexer
					for (int i = 0; i < numberofItemInColleciton; i++)
					{
						// get item through Indexer
						object getValue = getter.Invoke(source, new object[] { i });

						// clone if needed
						if (getValue != null && getValue is DependencyObject)
							getValue = Clone(getValue as DependencyObject);

						// add item to collection
						propertyInfo.ReflectedType.GetMethod("Add").Invoke(cloned, new object[] { getValue });
					}
				}
			}

			return cloned;
		}

		// Extension for any DependencyObject class
		public static TT Clone2<TT>(this TT source) where TT : DependencyObject
		{
			Type type = source.GetType();
			TT cloned = (TT)Activator.CreateInstance(type);

			Type wtype = type;
			while (wtype.BaseType != typeof(DependencyObject))
			{
				FieldInfo[] fieldInfo = wtype.GetFields(BindingFlags.Static | BindingFlags.Public);
				for (int i = 0; i < fieldInfo.Length; i++)
				{
					{
						DependencyProperty property = fieldInfo[i].GetValue(source) as DependencyProperty;
						if (property != null && fieldInfo[i].Name != "NameProperty")
						{
							DependencyObject obj = source.GetValue(property) as DependencyObject;
							if (obj != null)
							{
								object o = obj.Clone2();
								cloned.SetValue(property, o);
							}
							else
							{
								if (fieldInfo[i].Name != "CountProperty" &&
									fieldInfo[i].Name != "GeometryTransformProperty" &&
									fieldInfo[i].Name != "ActualWidthProperty" &&
									fieldInfo[i].Name != "ActualHeightProperty" &&
									fieldInfo[i].Name != "MaxWidthProperty" &&
									fieldInfo[i].Name != "MaxHeightProperty" &&
									fieldInfo[i].Name != "StyleProperty")
								{
									cloned.SetValue(property, source.GetValue(property));
								}

							}
						}
					}
				}

				wtype = wtype.BaseType;
			}

			foreach (PropertyInfo propertyInfo in type.GetProperties())
			{
				MethodInfo getter = propertyInfo.GetGetMethod(false/*nonPublic*/);
				MethodInfo setter = propertyInfo.GetSetMethod(false/*nonPublic*/);
				if (getter == null || setter == null)
					continue;

				if (propertyInfo.Name != "Name" &&
					propertyInfo.Name != "Parent" &&
					propertyInfo.CanRead && 
					propertyInfo.CanWrite &&
					!propertyInfo.PropertyType.IsArray &&
					!propertyInfo.PropertyType.IsSubclassOf(typeof(DependencyObject)) &&
					propertyInfo.GetIndexParameters().Length == 0 &&
					propertyInfo.GetValue(source, null) != null &&
					propertyInfo.GetValue(source, null) == (object)default(int) &&
					propertyInfo.GetValue(source, null) == (object)default(double) &&
					propertyInfo.GetValue(source, null) == (object)default(float))
						propertyInfo.SetValue(cloned, propertyInfo.GetValue(source, null), null);
				else
				if (propertyInfo.PropertyType.GetInterface("IList", true) != null)
				{
					int count = (int)propertyInfo.PropertyType.InvokeMember("get_Count", BindingFlags.InvokeMethod, null, propertyInfo.GetValue(source, null), null);
					for (int c = 0; c < count; c++)
					{
						object val = propertyInfo.PropertyType.InvokeMember("get_Item", BindingFlags.InvokeMethod, null, propertyInfo.GetValue(source, null), new object[] { c });
						object nVal = val;
						DependencyObject obj2 = val as DependencyObject;
						if (obj2 != null)
							nVal = obj2.Clone2();
						if (propertyInfo.GetValue(cloned, null) == null)
						{
							object obj = Activator.CreateInstance(propertyInfo.PropertyType);
							propertyInfo.SetValue(cloned, obj, null);
						}

						propertyInfo.PropertyType.InvokeMember("Add", BindingFlags.InvokeMethod, null, propertyInfo.GetValue(cloned, null), new object[] { nVal });
					}
				}
			}

			return cloned;
		}
	}

	public static class XDocumentExt
	{
		// Extension for XDocument
		public static XDocument ReplacePattern(this XDocument xmlDoc, string pattern, int matchGroup, Dictionary<string, string> replacementDictionary, out int count)
		{
			count = 0;
			try
			{
				string xmlString = xmlDoc.ToString(SaveOptions.DisableFormatting);
				xmlString = xmlString.ReplacePattern(pattern, matchGroup, replacementDictionary, out count);
				if (count > 0)
					return XDocument.Parse(xmlString);
			}
			catch (Exception e)
			{
				e.Assert();
			}

			return xmlDoc;
		}
	}

	public static class DateTimeExt
	{
		// Extension for DateTime
		// Compute a Julian day from a DateTime date
		public static JulianDay ToJulian(this DateTime date)
		{
			return new JulianDay(date.Month, date.Day, date.Year);
		}
	}

	public static class TimelineExt
	{
		// Extension for Timeline
		public static void SetTarget(this Timeline timeline, DependencyObject target)
		{
			Storyboard.SetTarget(timeline, target);
		}

		// Extension for Timeline
		public static void SetTargetName(this Timeline timeline, string name)
		{
			Storyboard.SetTargetName(timeline, name);
		}

		// Extension for Timeline
		public static void SetTargetProperty(this Timeline timeline, string propertyName)
		{
			Storyboard.SetTargetProperty(timeline, new PropertyPath(propertyName));
		}
	}

	public static class Helper
	{
		public static Uri HttpXapPath()
		{
			return Application.Current.Host.Source;
		}

		public static Uri HttpSitePath(string relativeUri)
		{
			// Find the site relative to the /ClientBin folder which contains the XAP
			Uri uri = new Uri(HttpXapPath() + "/../../" + relativeUri);
			return uri;
		}

		public static Uri HttpImagesPath(string relativeUri)
		{
			return HttpSitePath("images/" + relativeUri);
		}

		public static Uri HostPath(string relativeUri)
		{
			Uri uri = HttpSitePath(null);
			string sUri = uri.AbsoluteUri.ToString().Replace(uri.AbsolutePath, "");
			uri = new Uri(sUri + "/" + relativeUri);
			return uri;
		}

		public static void Trace(string str, params object[] args)
		{
			System.Diagnostics.Debug.WriteLine(str, args);
		}

		// Extension for string
		public static void Alert(this string message)
		{
			//MessageBox.Show(message, "Alert", MessageBoxButton.OK);
			HtmlPage.Window.Alert(message);
		}
#if NOTUSED
		/////////////////////////////////////////////////////////////////////////////
		public static void ThreadExample()
		{
			ThreadStart threadStart = new ThreadStart(ThreadFunction);
			Thread thread = new Thread(threadStart);
			thread.Start();
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void ThreadFunction()
		{
		}
		
		/////////////////////////////////////////////////////////////////////////////
		private static void SyncronizeExample()
		{
			ManualResetEvent resetEvent = new ManualResetEvent(false);
			resetEvent.Reset();
			resetEvent.Set();
			resetEvent.WaitOne();
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DisplayXmlDocument(string xmlString)
		{
//j			xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>" + xmlString;
//j			xmlString = HttpUtility.HtmlEncode(xmlString);
//j			xmlString = xmlString.Replace('\'', 'z');
//j			xmlString = xmlString.Replace('"', 'q');
			string str = "var newWin = window.open('about:blank', '_blank'); if (newWin != null) newWin.document.write('" + xmlString + "');";
			try
			{
				HtmlPage.Window.Eval(str); 
			}
			catch (Exception e)
			{
				e.Assert();
			}
		}
#endif
	}
}
