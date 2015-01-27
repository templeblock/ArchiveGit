// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ClassLibrary
{
	public class StretchPanel : Grid
	{
		// The Orientation dependency property
		public static readonly DependencyProperty OrientationProperty =
			DependencyProperty.Register("Orientation", typeof(Orientation), typeof(StretchPanel),
			new PropertyMetadata(Orientation.Vertical, OnLayoutPropertyChanged));

		public Orientation Orientation
		{
			get { return (Orientation)GetValue(OrientationProperty); }
			set { SetValue(OrientationProperty, value); }
		}

		private static void OnLayoutPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			StretchPanel panel = obj as StretchPanel;
			if (panel == null)
				return;

			panel.LayoutUpdate();
		}

		// The Length attached property
		public static readonly DependencyProperty LengthProperty =
			DependencyProperty.RegisterAttached("Length", typeof(GridLength), typeof(StretchPanel),
				new PropertyMetadata(GridLength.Auto, OnLayoutAttachedPropertyChanged));

		public static GridLength GetLength(DependencyObject obj)
		{
			return (GridLength)obj.GetValue(LengthProperty);
		}

		public static void SetLength(DependencyObject obj, GridLength value)
		{
			obj.SetValue(LengthProperty, value);
		}

		private static void OnLayoutAttachedPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			FrameworkElement element = obj as FrameworkElement;
			if (element == null || element.Parent == null)
				return;
			StretchPanel panel = element.Parent as StretchPanel;
			if (panel == null)
				return;

			panel.LayoutUpdate();
		}

		private bool _loaded;
		private int _lastLayoutChildCount;

		public StretchPanel()
		{
			Loaded += OnLoaded;
			LayoutUpdated += OnLayoutUpdated;
		}

		private void OnLayoutUpdated(object sender, object o)
		{
			if (!_loaded)
				return;
			if (_lastLayoutChildCount == Children.Count)
				return;
			LayoutUpdate();
			//j Dispatcher.BeginInvoke(LayoutUpdate);
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			_loaded = true;
			Loaded -= OnLoaded;
			LayoutUpdate();
		}

		private void LayoutUpdate()
		{
			if (_loaded == false)
				return;

			_lastLayoutChildCount = Children.Count;

			ColumnDefinitions.Clear();
			RowDefinitions.Clear();

			int index = 0;
			foreach (UIElement child in Children)
			{
				FrameworkElement element = child as FrameworkElement;
				if (element == null)
					continue;

				GridLength length = GetLength(element);
				if (Orientation == Orientation.Horizontal)
				{
					ColumnDefinition definition = new ColumnDefinition { Width = length };
					//if (index == 0)
					//    ColumnDefinitions.Clear();
					ColumnDefinitions.Add(definition);
					SetColumn(element, index++);
					SetColumnSpan(element, 1);
				}
				else
				{
					RowDefinition definition = new RowDefinition { Height = length };
					//if (index == 0)
					//    RowDefinitions.Clear();
					RowDefinitions.Add(definition);
					SetRow(element, index++);
					SetRowSpan(element, 1);
				}
			}
		}
	}
}
