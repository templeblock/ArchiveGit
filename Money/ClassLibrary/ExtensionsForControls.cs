using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
		internal static class TabControlExt
		{
#if true
			// Extension for TabControl
			internal static TabItem FindByTag(this TabControl element, string tagFragment)
			{
				if (element == null || tagFragment == null)
					return null;

				return element.Items
						.OfType<TabItem>()
						.Where(item => item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
						.FirstOrDefault();
			}
#else
		// Extension for TabControl
		internal static TabItem FindByTag(this TabControl element, string tagFragment)
		{
			if (element == null || tagFragment == null)
				return null;

			foreach (object item in element.Items)
			{
				TabItem item = item as TabItem;
				if (item != null && item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
					return item;
			}

			return null;
		}
#endif
		}

		internal static class RadioButtonExt
		{
			// Extension for RadioButton
			internal static RadioButton SelectedItem(this RadioButton element)
			{
				if (element == null)
					return null;

				return element.Parent.Descendants()
						.OfType<RadioButton>()
						.Where(item => item.GroupName == element.GroupName && item.IsChecked == true)
						.FirstOrDefault();
			}
#if true
			// Extension for RadioButton
			internal static RadioButton FindByTag(this RadioButton element, string tagFragment)
			{
				if (element == null || tagFragment == null)
					return null;

				return element.Parent.Descendants()
						.OfType<RadioButton>()
						.Where(item => item.GroupName == element.GroupName && item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
						.FirstOrDefault();
			}
#else
		// Extension for RadioButton
		internal static RadioButton FindByTag(this RadioButton element, string tagFragment)
		{
			if (element == null || tagFragment == null)
				return null;

			IEnumerable<RadioButton> list = element.Parent.ChildListOfType<RadioButton>();
			foreach (RadioButton item in list)
			{
				if (item.GroupName == element.GroupName && item.Tag != null && item.Tag.ToString().StartsWithIgnoreCase(tagFragment))
					return item;
			}

			return null;
		}
#endif
		}

		internal static class ScrollViewerExt
		{
			// Extension for FrameworkElement
			internal static void ScrollIntoView(this FrameworkElement element, TimeSpan timeSpan)
			{
				ScrollViewer viewer = element.ParentOfType<ScrollViewer>();
				if (viewer != null)
					viewer.ScrollIntoView(element, 0, 1, timeSpan);
			}
		}
}
