using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace ClassLibrary
{
	public static class ApplicationEx
	{
		static public UserControl RootUserControl { get { return Application.Current.RootVisual as UserControl; } }
		static public Panel RootPanel { get { return RootUserControl.FindName("x_LayoutRoot") as Panel; } }
	}
}
