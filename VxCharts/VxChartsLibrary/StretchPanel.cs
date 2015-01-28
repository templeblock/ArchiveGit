using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	public class StretchPanel : Grid
	{
		// The Orientation dependency property
		public static readonly DependencyProperty OrientationProperty =
			DependencyProperty.Register("Orientation", typeof(Orientation), typeof(StretchPanel),
			new PropertyMetadata(Orientation.Vertical, (d, e) => ((StretchPanel)d).OnOrientationPropertyChanged((Orientation)e.OldValue, (Orientation)e.NewValue)));

		public Orientation Orientation
		{
			get { return (Orientation)GetValue(OrientationProperty); }
			set { SetValue(OrientationProperty, value); }
		}

		private void OnOrientationPropertyChanged(Orientation oldValue, Orientation newValue)
		{
		}

		// The Length attached property
		public static readonly DependencyProperty LengthProperty =
			DependencyProperty.RegisterAttached("Length", typeof(GridLength), typeof(StretchPanel),
				new PropertyMetadata(GridLength.Auto, null));

		public static GridLength GetLength(FrameworkElement element)
		{
			return (GridLength)element.GetValue(LengthProperty);
		}

		public static void SetLength(FrameworkElement element, GridLength value)
		{
			element.SetValue(LengthProperty, value);
		}

		public StretchPanel()
		{
			base.Loaded += OnLoaded;

			Orientation = Orientation.Vertical;
			base.Visibility = Visibility.Collapsed; // Wait till OnLoaded completes
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			int index = 0;
			foreach (UIElement child in base.Children)
			{
				FrameworkElement element = child as FrameworkElement;
				GridLength length = StretchPanel.GetLength(element);

				if (Orientation == Orientation.Horizontal)
				{
					ColumnDefinition definition = new ColumnDefinition() { Width = length };
					if (index == 0)
						base.ColumnDefinitions.Clear();
					base.ColumnDefinitions.Add(definition);
					Grid.SetColumn(element, index++);
					Grid.SetColumnSpan(element, 1);
				}
				else
				{
					RowDefinition definition = new RowDefinition() { Height = length };
					if (index == 0)
						base.RowDefinitions.Clear();
					base.RowDefinitions.Add(definition);
					Grid.SetRow(element, index++);
					Grid.SetRowSpan(element, 1);
				}
			}

			base.Visibility = Visibility.Visible;
		}
	}
}
