using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
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

	internal static class FrameworkElementExt
	{
		// Extension for FrameworkElement
		internal static void ApplyStyle(this FrameworkElement element, FrameworkElement elementResource, string name)
		{
			Style style = elementResource.FindResource(name) as Style;
			if (style != null)
				element.Style = style;
		}

		// Extension for FrameworkElement
		internal static object FindResource(this FrameworkElement element, string name)
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

			string.Format("Can't find resource '{0}'", name).Alert();
			return null;
		}
	}

	internal static class ColorExt
	{
		// Extension for Color
		internal static SolidColorBrush ToBrush(this Color color)
		{
			return new SolidColorBrush(color);
		}

		// Extension for Color
		internal static int ToArgb(this Color color)
		{
			int argb = color.A << 24;
			argb += color.R << 16;
			argb += color.G << 8;
			argb += color.B;

			return argb;
		}
	}

	internal static class MillicentsExt
	{
		// Extension for Millicents
		internal static double ToDouble(this Millicents millicents)
		{
			return (double)millicents / 1000;
		}

		// Extension for Millicents
		internal static double ToDoublePecent(this Millicents millicents)
		{
			return (double)millicents / 100000;
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
			MessageBox.Show(message, "Alert", MessageBoxButton.OK);
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

	internal static class DependencyObjectExt
	{
		// Extension for DependencyObject
		internal static IEnumerable<DependencyObject> Descendants(this DependencyObject element)
		{
#if true //j this is redundant and produces duplicate entries in the list
			if (element is ContentControl)
			{
				DependencyObject child = (element as ContentControl).Content as DependencyObject;
				if (child != null)
				{
					yield return child;
					foreach (DependencyObject descendent in child.Descendants())
						yield return descendent;
				}
			}
#endif
			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				yield return child;
				foreach (DependencyObject descendent in child.Descendants())
					yield return descendent;
			}
		}

		// Extension for DependencyObject
		// Return the first child of a given type
#if true
		internal static TT ChildOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			if (element == null)
				return null;

			return element.Descendants()
					.OfType<TT>()
					.FirstOrDefault() as TT;
		}
#else
		internal static TT ChildOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			return ChildOfType(element, delegate(TT obj)
			{
				return true;
			});
		}
#endif

		// Extension for DependencyObject
		// Return the first child of a given type and name fragment
#if true
		internal static TT ChildOfTypeAndName<TT>(this DependencyObject element, string nameFragment) where TT : FrameworkElement
		{
			if (element == null || nameFragment == null)
				return null;

			return element.Descendants()
					.OfType<TT>()
					.Where(item => item.Name != null && item.Name.StartsWithIgnoreCase(nameFragment))
					.FirstOrDefault() as TT;
		}
#else
		internal static TT ChildOfTypeAndName<TT>(this DependencyObject element, string nameFragment) where TT : DependencyObject
		{
			return ChildOfType(element, delegate(TT obj)
			{
				if (!(obj is FrameworkElement)) return false;
				string name = (obj as FrameworkElement).Name;
				return (name != null && name.StartsWithIgnoreCase(nameFragment));
			});
		}
#endif

		// Extension for DependencyObject
		// Return the first child of a given type and tag fragment
#if true
		internal static TT ChildOfTypeAndTag<TT>(this DependencyObject element, string tagFragment) where TT : FrameworkElement
		{
			if (element == null || tagFragment == null)
				return null;

			return element.Descendants()
					.OfType<TT>()
					.Where(item => item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
					.FirstOrDefault() as TT;
		}
#else
		internal static TT ChildOfTypeAndTag<TT>(this DependencyObject element, string tagFragment) where TT : DependencyObject
		{
			return ChildOfType(element, delegate(TT obj)
			{
				if (!(obj is FrameworkElement)) return false;
				object tag = (obj as FrameworkElement).Tag;
				return (tag != null && tag.ToString().StartsWithIgnoreCase(tagFragment));
			});
		}

		// Private Extension for DependencyObject
		// Return the first child of a given type, and passing the Checker delegate
		private static TT ChildOfType<TT>(this DependencyObject element, Predicate<TT> checker) where TT : DependencyObject
		{
			if (element == null)
				return null;

			if (element is ContentControl)
			{
				DependencyObject child = (element as ContentControl).Content as DependencyObject;
				TT childFound = child.ChildOfTypeSearch(checker);
				if (childFound != null)
					return childFound;
			}

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				TT childFound = child.ChildOfTypeSearch(checker);
				if (childFound != null)
					return childFound;
			}

			return null;
		}

		// Private Extension for DependencyObject
		// Return the first child of a given type, and passing the Checker delegate
		private static TT ChildOfTypeSearch<TT>(this DependencyObject child, Predicate<TT> checker) where TT : DependencyObject
		{
			TT childFound = child as TT;
			if (childFound != null && checker(childFound))
				return childFound;
			childFound = ((DependencyObject)child).ChildOfType(checker);
			if (childFound != null && checker(childFound))
				return childFound;
			return null;
		}
#endif

		// Extension for DependencyObject
		// Return the first child of a given type and tag fragment
		internal static FrameworkElement ChildOfTag(this DependencyObject element, string tagFragment)
		{
			if (element == null || tagFragment == null)
				return null;

			return element.Descendants()
					.OfType<FrameworkElement>()
					.Where(item => item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
					.FirstOrDefault() as FrameworkElement;
		}

		// Extension for DependencyObject
		// Return a list of all children of a given type
#if true
		internal static IEnumerable<TT> ChildListOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			if (element == null)
				return new Collection<TT>();

			return element.Descendants()
					.OfType<TT>();
		}
#else
		internal static IList<TT> ChildListOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			List<TT> list = new List<TT>();
			if (element != null)
				element.ChildListOfType(ref list);

			return list;
		}

		// Extension for DependencyObject
		private static IList<TT> ChildListOfType<TT>(this DependencyObject element, ref List<TT> list) where TT : DependencyObject
		{
			if (element is ContentControl)
			{
				DependencyObject child = (element as ContentControl).Content as DependencyObject;
				if (child is TT)
					list.Add((TT)child);
				if (child is DependencyObject)
					((DependencyObject)child).ChildListOfType(ref list);
			}

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				if (child is TT)
					list.Add((TT)child);
				if (child is DependencyObject)
					((DependencyObject)child).ChildListOfType(ref list);
			}

			return list;
		}
#endif

		// Extension for DependencyObject
		internal static bool IsDescendentOf(this DependencyObject element, DependencyObject parentElement)
		{
			if (element == parentElement)
				return true;
			if (element == null || !(element is FrameworkElement))
				return false;
			return IsDescendentOf((element as FrameworkElement).Parent, parentElement);
		}

		// Extension for DependencyObject
		internal static TT ParentOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			if (element == null)
				return default(TT);

			while ((element = VisualTreeHelper.GetParent(element)) != null)
			{
				if (element is TT)
					return (TT)element;
			}

			return null;
		}

		// Extension for DependencyObject
		internal static int NumControlChildren(this DependencyObject element)
		{
			if (element == null)
				return 0;

			int count = 0;
			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				if ((child is Control) && (child as Control).IsEnabled && (child as Control).IsVisible())
					count++;
			}

			return count;
		}

		// Extension for DependencyObject
		internal static Control NextControlSibling(this DependencyObject element)
		{
			if (element == null)
				return null;

			DependencyObject parent = null;
			int targetCount = 1 + (element is Control ? 1 : 0);
			while (true)
			{
				parent = VisualTreeHelper.GetParent(element);
				if (parent == null)
					return null;

				if (parent.NumControlChildren() >= targetCount)
					break;

				targetCount = 1;
				element = parent;
			}

			bool selectNext = false;
			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parent); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(parent, i);
				if (child == element)
					selectNext = true;
				else
				if (selectNext && ((child is Control) && (child as Control).IsEnabled) && (child as Control).IsVisible())
					return (child as Control);
			}

			return null;
		}
	}

	internal static class UIElementExt
	{
		// Extension for UIElement
		internal static bool HasFocus(this UIElement element)
		{
			if (element == null)
				return false;

			DependencyObject focusedElement = FocusManager.GetFocusedElement() as DependencyObject;
			while (focusedElement != null)
			{
				if (element == focusedElement)
					return true;
				focusedElement = VisualTreeHelper.GetParent(focusedElement);
			}

			return false;
		}

		// Extension for UIElement
		internal static void TakeFocus(this UIElement element)
		{
			Control control = (element as Control);
			if (control != null)
			{
				if (control.FocusEx())
					return;
			}

			IEnumerable<Control> childControls = element.ChildListOfType<Control>();
			foreach (Control childControl in childControls)
			{
				if (childControl.FocusEx())
					return;
			}
		}

		// Extension for UIElement
		internal static void HandleKey(this UIElement controlParent, Key key, UIElement control, RoutedEventHandler handler)
		{
			controlParent.KeyDown += delegate(object sender, KeyEventArgs e)
			{
				if (e.Key == key && control != null && handler != null)
				{
					//control.FocusEx();
					handler(control, new RoutedEventArgs());
				}
			};
		}

		// Extension for UIElement
		internal static void HandleEnterKey(this UIElement controlParent, UIElement control, RoutedEventHandler handler)
		{
			HandleKey(controlParent, Key.Enter, control, handler);
		}

		// Extension for UIElement
		internal static void HandleEscapeKey(this UIElement controlParent, UIElement control, RoutedEventHandler handler)
		{
			HandleKey(controlParent, Key.Escape, control, handler);
		}

		// Extension for UIElement
		internal static void SetTop(this UIElement element, double value)
		{
			if (element != null)
				element.SetValue(Canvas.TopProperty, value);
		}

		// Extension for UIElement
		internal static double GetTop(this UIElement element)
		{
			return (double)element.GetValue(Canvas.TopProperty);
		}

		// Extension for UIElement
		internal static void SetLeft(this UIElement element, double value)
		{
			if (element != null)
				element.SetValue(Canvas.LeftProperty, value);
		}

		// Extension for UIElement
		internal static double GetLeft(this UIElement element)
		{
			return (double)element.GetValue(Canvas.LeftProperty);
		}

		// Extension for UIElement
		internal static int GetZIndex(this UIElement element)
		{
			return (int)element.GetValue(Canvas.ZIndexProperty);
		}

		// Extension for UIElement
		internal static void SetZIndex(this UIElement element, int zIndex)
		{
			if (element != null)
				element.SetValue(Canvas.ZIndexProperty, zIndex);
		}

		// Extension for UIElement
		internal static bool IsVisible(this UIElement element)
		{
			return (element != null && element.Visibility == Visibility.Visible);
		}

		// Extension for UIElement
		internal static bool IsVisibleInTree(this UIElement uiElement)
		{
			DependencyObject element = uiElement;
			if (element == null)
				return false;

			do
			{
				if ((element is UIElement) && (element as UIElement).Visibility != Visibility.Visible)
					return false;
			}
			while ((element = VisualTreeHelper.GetParent(element)) != null);

			return true;
		}

		// Extension for UIElement
		internal static void SetVisible(this UIElement element, bool bVisible)
		{
			if (element != null)
				element.Visibility = (bVisible ? Visibility.Visible : Visibility.Collapsed);
		}

		// Extension for UIElement
		internal static void Hide(this UIElement element)
		{
			if (element != null && element.Visibility != Visibility.Collapsed)
				element.Visibility = Visibility.Collapsed;
		}

		// Extension for UIElement
		internal static void Show(this UIElement element)
		{
			if (element.Visibility != Visibility.Visible)
				element.Visibility = Visibility.Visible;
		}

		// Extension for UIElement
		internal static ICollection<UIElement> HitTest(this UIElement element, MouseEventArgs mouse)
		{
			Point pluginPosition = mouse.GetPosition(null);
			return VisualTreeHelper.FindElementsInHostCoordinates(pluginPosition, element) as ICollection<UIElement>;
		}

		// Extension for UIElement
		internal static void SetToolTip(this UIElement element, object content)
		{
			ToolTipService.SetToolTip(element, content);
		}

		// Extension for UIElement
		internal static object GetToolTip(this UIElement element)
		{
			return ToolTipService.GetToolTip(element);
		}
	}

	internal static class ControlExt
	{
		// Extension for Control
		internal static bool FocusEx(this Control control)
		{
			if (control == null)
				return false;

			bool success = false;
			if (control == FocusManager.GetFocusedElement())
				success = true;
			else
			{
				// To get Focus() to work properly, call UpdateLayout() immediately before
				control.UpdateLayout();
				success = control.Focus();
			}

			ListBox listBox = control as ListBox;
			if (listBox != null)
			{
				if (listBox.SelectedIndex < 0 && listBox.Items.Count > 0)
					listBox.SelectedIndex = 0;
			}

			return success;
		}
	}

	internal static class ExceptionExt
	{
		// Extension for Exception
		internal static void Alert(this Exception ex)
		{
			string message = ex.Details();
			//Debug.Assert(false, message);
			MessageBox.Show(message, "Alert", MessageBoxButton.OK);
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
		internal static string AttributeValue(this XElement element, string name)
		{
			if (element == null)
				return null;
			XAttribute attribute = element.Attribute(name);
			if (attribute == null)
				return null;
			return element.Attribute(name).Value;
		}
	}

	internal static class TimelineExt
	{
		// Extension for Timeline
		internal static string GetTargetName(this Timeline timeline)
		{
			try
			{
				return Storyboard.GetTargetName(timeline);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return null;
			}
		}

		// Extension for Timeline
		internal static UIElement GetTarget(this Timeline timeline)
		{
			try
			{
				return null;
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return null;
			}
		}

		// Extension for Timeline
		internal static void SetTarget(this Timeline timeline, DependencyObject target)
		{
			try
			{
				Storyboard.SetTarget(timeline, target);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}

		// Extension for Timeline
		internal static void SetTargetName(this Timeline timeline, string name)
		{
			try
			{
				Storyboard.SetTargetName(timeline, name);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}

		// Extension for Timeline
		internal static void SetTargetProperty(this Timeline timeline, string propertyName)
		{
			try
			{
				Storyboard.SetTargetProperty(timeline, new PropertyPath(propertyName));
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
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

	internal static class HtmlElementExt
	{
		// Extension for HtmlElement
		internal static double GetElementDimension(this HtmlElement element, string attributeName)
		{
			if (element == null)
				return 0;

			string dimension = element.GetAttribute(attributeName);
			if (String.IsNullOrEmpty(dimension))
			{
				dimension = element.GetStyleAttribute(attributeName);
				if (String.IsNullOrEmpty(dimension))
					return 0;
			}

			bool percent = false;
			if (dimension.EndsWith("%"))
			{
				dimension = dimension.Substring(0, dimension.Length - 1);
				percent = true;
			}
			else
			if (dimension.EndsWith("px"))
				dimension = dimension.Substring(0, dimension.Length - 2);

			double value = 0;
			bool ok = Double.TryParse(dimension, out value);

			if (percent)
			{
				FrameworkElement root = Application.Current.RootVisual as FrameworkElement;
				bool isWidth = attributeName.Equals("width", StringComparison.CurrentCultureIgnoreCase);
				value *= ((isWidth ? root.DesiredSize.Width : root.DesiredSize.Height) / 100);
			}

			return value;
		}

		// Extension for HtmlElement
		internal static void SetElementDimension(this HtmlElement element, string attributeName, double value)
		{
			if (element == null)
				return;

			//value *= Application.Current.Host.Content.ZoomFactor;
			value = Math.Max(value, 72);
			string valueText = ((int)Math.Round(value)).ToString() + "px";
			element.SetAttribute(attributeName, valueText);
			element.SetStyleAttribute(attributeName, valueText);
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
