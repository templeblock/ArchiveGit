using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;
using System.Xml.Linq;
using Windows.System;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Animation;
using Millicents = System.Int64;

namespace ClassLibrary
{
	public static class RandomExt
	{
		private static Random _Random = new Random();

		public static bool Bool()
		{
			return _Random.Next(0, 2) != 0;
		}

		public static double Double()
		{
			return _Random.NextDouble();
		}

		public static double Double(double max)
		{
			return (_Random.NextDouble() * max);
		}

		public static double Double(double min, double max)
		{
			return (_Random.NextDouble() * (max - min)) + min;
		}

		public static int Int()
		{
			return _Random.Next();
		}

		public static int Int(int max)
		{
			return _Random.Next(max+1);
		}

		public static int Int(int min, int max)
		{
			return _Random.Next(min, max+1);
		}
	}

	public static class EnumExt
	{
		// Extension for Enums
		public static int ToValue(this Enum input)
		{
			return Convert.ToInt32(input);
		}

		// Extension for Enums
		public static void SetFromInt(this Enum input, int value)
		{
			if (Enum.IsDefined(input.GetType(), value))
				input = (Enum)Enum.ToObject(input.GetType(), value);
		}

		// Extension for flags oriented Enums
		public static void SetFromIntFlags(this Enum input, int flags)
		{
			input = (Enum)Enum.ToObject(input.GetType(), flags);
		}

		// Extension for flags oriented Enums
		public static bool ContainsAll(this Enum input, Enum flag)
		{
			int value = input.ToValue();
			int valueMatch = flag.ToValue();
			return (value & valueMatch) == valueMatch;
		}

		// Extension for flags oriented Enums
		public static bool ContainsAny(this Enum input, Enum flag)
		{
			int value = input.ToValue();
			int valueMatch = flag.ToValue();
			return (value & valueMatch) != 0;
		}
	}

	public static class AssemblyExt
	{
		// Extension for Assembly
		public static string Name(this Assembly assembly)
		{
			string name = assembly.FullName;
			return name.Substring(0, name.IndexOf(','));
		}
	}

	public static class ColorExt
	{
		// Extension for Color
		public static SolidColorBrush ToBrush(this Color color)
		{
			return new SolidColorBrush(color);
		}

		// Extension for Color
		public static int ToArgb(this Color color)
		{
			int argb = color.A << 24;
			argb += color.R << 16;
			argb += color.G << 8;
			argb += color.B;

			return argb;
		}
	}

	public static class MillicentsExt
	{
		// Extension for Millicents
		public static double ToDouble(this Millicents millicents)
		{
			return (double)millicents / 1000;
		}

		// Extension for Millicents
		public static double ToDoublePecent(this Millicents millicents)
		{
			return (double)millicents / 100000;
		}
	}

	public static class DoubleExt
	{
		// Extension for double
		public static bool IsNaN(this double dvalue)
		{
			return double.IsNaN(dvalue);
		}

		// Extension for double
		public static double Round(this double dvalue)
		{
			return Math.Round(dvalue);
		}

		// Extension for double
		public static double Round(this double dvalue, int digits)
		{
			return Math.Round(dvalue, digits);
		}

		// Extension for double
		public static Int32 ToInt32(this double dvalue)
		{
			return (Int32)(dvalue + (dvalue >= 0 ? 0.5 : -0.5));
		}

		// Extension for double
		public static Int64 ToInt64(this double dvalue)
		{
			return (Int64)(dvalue + (dvalue >= 0 ? 0.5 : -0.5));
		}

		// Extension for double
		public static Millicents ToMillicents(this double dvalue)
		{
			dvalue *= 1000.0;
			return (Millicents)(dvalue + (dvalue >= 0 ? 0.5 : -0.5));
		}
	}

	public static class StringExt
	{
		// Extension for string
		public static bool EqualsIgnoreCase(this string source, string target)
		{
			return source.Equals(target, StringComparison.CurrentCultureIgnoreCase);
		}

		// Extension for string
		public static bool StartsWithIgnoreCase(this string source, string target)
		{
			return source.StartsWith(target, StringComparison.CurrentCultureIgnoreCase);
		}

		// Extension for string
		public static bool ContainsIgnoreCase(this string source, string target)
		{
			return (source.ToLower().Contains(target.ToLower()));
		}

		// Extension for string
		public static bool IsEmpty(this string source)
		{
			return string.IsNullOrEmpty(source);
		}

		// Extension for string
		public static void Trace(this string message)
		{
			Debug.WriteLine(message);
		}

		// Extension for string
		public static void Alert(this string message)
		{
			throw new Exception();
			//j MessageBox.Show(message, "Alert", MessageBoxButton.OK);
		}

		// Extension for string
		public static TT ToEnum<TT>(this string source) where TT: struct
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
		public static int ToInt(this string source)
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
		public static byte ToByte(this string source)
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
		public static char ToChar(this string source)
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
		public static uint ToUInt(this string source)
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
		public static UInt64 ToUInt64(this string source)
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
		public static Int64 ToInt64(this string source)
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
		public static double ToDouble(this string source)
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
		public static decimal ToDecimal(this string source)
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
		public static bool ToBool(this string source)
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
		public static Type ToType(this string source)
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

		public static DateTime ToDateTime(this string source)
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

		// Extension for string
		public static string ExpandMixedCase(this string source)
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

	public static class ByteArrayExt
	{
		// Extension for byte[]
		public static string ToUtfString(this byte[] source)
		{
			return Encoding.UTF8.GetString(source, 0, source.Length);
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

	public static class DependencyObjectExt
	{
		// Extension for DependencyObject
		public static IEnumerable<DependencyObject> Descendants(this DependencyObject element)
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
		public static TT ChildOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			if (element == null)
				return null;

			return element.Descendants()
					.OfType<TT>()
					.FirstOrDefault() as TT;
		}
#else
		public static TT ChildOfType<TT>(this DependencyObject element) where TT : DependencyObject
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
		public static TT ChildOfTypeAndName<TT>(this DependencyObject element, string nameFragment) where TT : FrameworkElement
		{
			if (element == null || nameFragment == null)
				return null;

			return element.Descendants()
					.OfType<TT>()
					.Where(item => item.Name != null && item.Name.StartsWithIgnoreCase(nameFragment))
					.FirstOrDefault() as TT;
		}
#else
		public static TT ChildOfTypeAndName<TT>(this DependencyObject element, string nameFragment) where TT : DependencyObject
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
		public static TT ChildOfTypeAndTag<TT>(this DependencyObject element, string tagFragment) where TT : FrameworkElement
		{
			if (element == null || tagFragment == null)
				return null;

			return element.Descendants()
					.OfType<TT>()
					.Where(item => item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
					.FirstOrDefault() as TT;
		}
#else
		public static TT ChildOfTypeAndTag<TT>(this DependencyObject element, string tagFragment) where TT : DependencyObject
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
		public static FrameworkElement ChildOfTag(this DependencyObject element, string tagFragment)
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
		public static IEnumerable<TT> ChildListOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			if (element == null)
				return new Collection<TT>();

			return element.Descendants()
					.OfType<TT>();
		}
#else
		public static IList<TT> ChildListOfType<TT>(this DependencyObject element) where TT : DependencyObject
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
		public static bool IsDescendentOf(this DependencyObject element, DependencyObject parentElement)
		{
			if (element == parentElement)
				return true;
			if (element == null || !(element is FrameworkElement))
				return false;
			return IsDescendentOf((element as FrameworkElement).Parent, parentElement);
		}

		// Extension for DependencyObject
		public static TT ParentOfType<TT>(this DependencyObject element) where TT : DependencyObject
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
		public static int NumControlChildren(this DependencyObject element)
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
		public static Control NextControlSibling(this DependencyObject element)
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

	public static class UIElementExt
	{
		// Extension for UIElement
		public static bool HasFocus(this UIElement element)
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
		public static void TakeFocus(this UIElement element)
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
		public static void HandleKey(this UIElement controlParent, VirtualKey key, UIElement control, RoutedEventHandler handler)
		{
			controlParent.KeyDown += delegate(object sender, KeyRoutedEventArgs e)
			{
				if (e.Key == key && control != null && handler != null)
				{
					//control.FocusEx();
					handler(control, new RoutedEventArgs());
				}
			};
		}

		// Extension for UIElement
		public static void HandleEnterKey(this UIElement controlParent, UIElement control, RoutedEventHandler handler)
		{
			HandleKey(controlParent, VirtualKey.Enter, control, handler);
		}

		// Extension for UIElement
		public static void HandleEscapeKey(this UIElement controlParent, UIElement control, RoutedEventHandler handler)
		{
			HandleKey(controlParent, VirtualKey.Escape, control, handler);
		}

		// Extension for UIElement
		public static void SetTop(this UIElement element, double value)
		{
			if (element != null)
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
			if (element != null)
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
			if (element != null)
				element.SetValue(Canvas.ZIndexProperty, zIndex);
		}

		// Extension for UIElement
		public static bool IsVisible(this UIElement element)
		{
			return (element != null && element.Visibility == Visibility.Visible);
		}

		// Extension for UIElement
		public static bool IsVisibleInTree(this UIElement uiElement)
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
		public static void SetVisible(this UIElement element, bool bVisible)
		{
			if (element != null)
				element.Visibility = (bVisible ? Visibility.Visible : Visibility.Collapsed);
		}

		// Extension for UIElement
		public static void Hide(this UIElement element)
		{
			if (element != null && element.Visibility != Visibility.Collapsed)
				element.Visibility = Visibility.Collapsed;
		}

		// Extension for UIElement
		public static void Show(this UIElement element)
		{
			if (element.Visibility != Visibility.Visible)
				element.Visibility = Visibility.Visible;
		}

		// Extension for UIElement
		//public static ICollection<UIElement> HitTest(this UIElement element, MouseEventArgs mouse)
		//{
		//	Point pluginPosition = mouse.GetPosition(null);
		//	return VisualTreeHelper.FindElementsInHostCoordinates(pluginPosition, element) as ICollection<UIElement>;
		//}

		// Extension for UIElement
		public static void SetToolTip(this UIElement element, object content)
		{
			ToolTipService.SetToolTip(element, content);
		}

		// Extension for UIElement
		public static object GetToolTip(this UIElement element)
		{
			return ToolTipService.GetToolTip(element);
		}
	}

	public static class ControlExt
	{
		// Extension for Control
		public static bool FocusEx(this Control control)
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
				success = control.Focus(FocusState.Programmatic);
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

	public static class ExceptionExt
	{
		// Extension for Exception
		public static void Alert(this Exception ex)
		{
			string message = ex.Details();
			//Debug.Assert(false, message);
			//j MessageBox.Show(message, "Alert", MessageBoxButton.OK);
		}

		// Extension for Exception
		public static void DebugOutput(this Exception ex, bool micro = false)
		{
#if DEBUG
			if (micro)
				Debug.WriteLine(ex.MicroDetails());
			else
				Debug.WriteLine(ex.MiniDetails());
#endif
		}

		// Extension for Exception
		public static string Details(this Exception ex)
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
		public static string MicroDetails(this Exception ex)
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
		public static string MiniDetails(this Exception ex)
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

	public static class XDocumentExt
	{
		// Extension for XDocument
		public static XDocument ReplacePattern(this XDocument xmlDoc, string pattern, int matchGroup, Dictionary<string, string> replacementDictionary, out int count)
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

	public static class XElementExt
	{
		// Extension for XElement
		public static string AttributeValue(this XElement element, string name)
		{
			if (element == null)
				return null;
			XAttribute attribute = element.Attribute(name);
			if (attribute == null)
				return null;
			return element.Attribute(name).Value;
		}
	}

	public static class TimelineExt
	{
		// Extension for Timeline
		public static string GetTargetName(this Timeline timeline)
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
		public static UIElement GetTarget(this Timeline timeline)
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
		public static void SetTarget(this Timeline timeline, DependencyObject target)
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
		public static void SetTargetName(this Timeline timeline, string name)
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
		public static void SetTargetProperty(this Timeline timeline, string propertyName)
		{
			try
			{
				Storyboard.SetTargetProperty(timeline, propertyName);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}
	}

	public static class CollectionExt
	{
		// Extension for IEnumerable<TT>
		public static TTcollection ToObservableCollection<TTcollection, TT>(this IEnumerable<TT> enumerable) where TTcollection : ObservableCollection<TT>
		{
			TTcollection collection = Activator.CreateInstance<TTcollection>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}

		// Extension for IEnumerable<TT>
		public static ObservableCollection<TT> ToObservableCollection<TT>(this IEnumerable<TT> enumerable)
		{
			ObservableCollection<TT> collection = new ObservableCollection<TT>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}

		// Extension for IEnumerable<TT>
		public static Collection<TT> ToCollection<TT>(this IEnumerable<TT> enumerable)
		{
			Collection<TT> collection = new Collection<TT>();
			foreach (TT item in enumerable)
				collection.Add(item);
			return collection;
		}
	}

	public static class ExpressionExt
	{
		// Extension for Expression<Func<T>>
		// usage: string name = ExpressionExt.ToPropertyName(() => TheProperty);
		public static string ToPropertyName<T>(this Expression<Func<T>> expression)
		{
			if (expression == null)
				return null;

			// this cast will always succeed
			return ((MemberExpression)expression.Body).Member.Name;
		}
	}


	public static class DictionaryExt
	{
		// Extension for IDictionary<string, TValue>
		public static bool ContainsKeyIgnoreCase<TValue>(this IDictionary<string, TValue> dictionary, string key)
		{
			foreach (string keyItem in dictionary.Keys)
			{
				if (key.EqualsIgnoreCase(keyItem))
					return true;
			}

			return false;
		}

		// Extension for IDictionary<string, TValue>
		public static TValue GetValueIgnoreCase<TValue>(this IDictionary<string, TValue> dictionary, string key, TValue defaultValue)
		{
			foreach (string keyItem in dictionary.Keys)
			{
				if (key.EqualsIgnoreCase(keyItem))
					return dictionary[keyItem];
			}

			return defaultValue;
		}

		// Extension for IDictionary<string, TValue>
		public static TValue GetValueIgnoreCase<TValue>(this IDictionary<string, TValue> dictionary, string key)
		{
			return dictionary.GetValueIgnoreCase(key, default(TValue));
		}
	}
}
