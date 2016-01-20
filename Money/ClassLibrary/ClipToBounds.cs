using System.Windows;
using System.Windows.Media;

namespace ClassLibrary
{
	public class ClipToBounds
	{
		public static bool GetEnabled(DependencyObject obj)
		{
			return (bool)obj.GetValue(EnabledProperty);
		}

		public static void SetEnabled(DependencyObject obj, bool enabled)
		{
			obj.SetValue(EnabledProperty, enabled);
		}

		public static readonly DependencyProperty EnabledProperty = DependencyProperty.RegisterAttached("Enabled", typeof(bool), typeof(ClipToBounds),
			new PropertyMetadata(OnEnabledPropertyChanged));

		private static void OnEnabledPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			FrameworkElement element = obj as FrameworkElement;
			if (element == null)
				return;

			OnClipToBounds(element);

			// when the element is loaded or resized, update its clipping geometry
			element.Loaded += OnLoaded;
			element.SizeChanged += OnSizeChanged;
		}

		static void OnLoaded(object sender, RoutedEventArgs e)
		{
			OnClipToBounds(sender as FrameworkElement);
		}

		static void OnSizeChanged(object sender, SizeChangedEventArgs e)
		{
			OnClipToBounds(sender as FrameworkElement);
		}

		private static void OnClipToBounds(FrameworkElement element)
		{
			element.Clip = (GetEnabled(element) ? new RectangleGeometry { Rect = new Rect(0, 0, element.ActualWidth, element.ActualHeight) } : null);
		}
	}
}
