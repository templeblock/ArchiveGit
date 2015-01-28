using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Diagnostics;

namespace ClassLibrary
{
	internal static class BrokenBindings
	{
		public static void Load(DependencyObject dp)
		{
			LoadBrokenBindings(GetRoot(dp));
		}

		private static void LoadBrokenBindings(DependencyObject dp)
		{
			FrameworkElement fwe = dp as FrameworkElement;
			foreach (FieldInfo fi in fwe.GetType().GetFields(BindingFlags.Public | BindingFlags.FlattenHierarchy | BindingFlags.Instance | BindingFlags.Static))
			{
				if (fi.FieldType != typeof(DependencyProperty))
					continue;

				BindingExpression be = fwe.GetBindingExpression(fi.GetValue(null) as DependencyProperty);
#if false
				if (be == null || be.ParentBinding.Source != null || be.ParentBinding.Path.Path.IsEmpty())
					continue;

				string controlName = fwe.Name;
				string controlTypeName = fwe.GetType().Name;
				string propertyName = fi.Name;
				string bindingPath = be.ParentBinding.Path.Path;
				string message = string.Format("Control Name: {0}, Type: {1}, Property: {2}, BindingPath: {3}",
					controlName, controlTypeName, propertyName, bindingPath);
				Debug.WriteLine("Broken Binding - " + message);
#endif
				if (be == null || be.ParentBinding.Source != null || be.ParentBinding.RelativeSource != null)
					continue;

				string controlName = fwe.Name;
				string controlTypeName = fwe.GetType().Name;
				string propertyName = fi.Name;
				string bindingPath = be.ParentBinding.Path.Path;
				
				bool bolIsInherited = false;
                if (fwe.DataContext != null && !string.IsNullOrEmpty(bindingPath))
				{
                    foreach (PropertyInfo p in fwe.DataContext.GetType().GetProperties(BindingFlags.Public | BindingFlags.FlattenHierarchy | BindingFlags.Instance | BindingFlags.Static))
					{
						if (string.Compare(p.Name, bindingPath) == 0)
						{
                            bolIsInherited = true;
                            break;
                        }
					}
                }

                if (bolIsInherited)
                    break; //j Correct?

                // this code handles empty bindings on the Button controls
                // I'll have to look into why the Button has an empty or unresolved binding
				if (controlName == "" && controlTypeName == "TextBlock" && propertyName == "TextProperty" && bindingPath == "")
					break; //j Correct?

				string message = string.Format("Control Name: {0}, Type: {1}, Property: {2}, BindingPath: {3}",
					controlName, controlTypeName, propertyName, bindingPath);
				Debug.WriteLine("Broken Binding - " + message);
            }

			int children = VisualTreeHelper.GetChildrenCount(fwe);
			for (int i=0; i<children; i++)
			{
				FrameworkElement child = (FrameworkElement)VisualTreeHelper.GetChild(fwe, i);
				if (child != null)
					LoadBrokenBindings(child);
			}
		}

		private static DependencyObject GetRoot(DependencyObject dpo)
		{
			DependencyObject current = dpo;
			DependencyObject result = null;

			while (current != null)
			{
				result = current;
				current = VisualTreeHelper.GetParent(current);
			}

			return result;
		}
	}
}
