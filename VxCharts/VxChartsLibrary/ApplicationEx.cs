using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	public static class ApplicationEx
	{
		public static UserControl RootUserControl { get { return Application.Current.RootVisual as UserControl; } }
		public static Panel LayoutRoot { get { return RootUserControl.FindName("x_LayoutRoot") as Panel; } }
	}
}
