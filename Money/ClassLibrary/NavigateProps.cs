using System;
using System.Windows;
using System.Windows.Input;

namespace ClassLibrary
{
	public static class Navigate
	{
		// The Uri attached property
		public static readonly DependencyProperty UriProperty =
			DependencyProperty.RegisterAttached("Uri", typeof(Uri), typeof(Navigate),
				new PropertyMetadata(null, OnUriPropertyChanged));

		public static Uri GetUri(DependencyObject obj)
		{
			return (Uri)obj.GetValue(UriProperty);
		}

		public static void SetUri(DependencyObject obj, Uri value)
		{
			obj.SetValue(UriProperty, value);
		}

		private static void OnUriPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
		{
			UIElement element = sender as UIElement;
			if (element == null)
				return;
			Uri uri = (Uri)e.NewValue;
			//if (enabled)
			//    element.Click += OnClick;
			//else
			//    element.Click -= OnClick;
		}

		private static void OnClick(object sender, MouseWheelEventArgs e)
		{
		}
	}
}
