using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	internal static class ApplicationEx
	{
		internal static UserControl RootUserControl { get { return Application.Current.RootVisual as UserControl; } }
		internal static Panel LayoutRoot { get { return RootUserControl.FindName("x_LayoutRoot") as Panel; } }
	}
}
