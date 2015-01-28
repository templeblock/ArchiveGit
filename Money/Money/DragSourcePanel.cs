using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using ClassLibrary;

namespace Money
{
	public class DragSourcePanel : ViewPanel
	{
		public DragSourcePanel()
		{
			// The Grid Background musn't be transparent, or it won't get mouse down events for dragging
			if (base.Background == null)
				base.Background = new SolidColorBrush("#01FFFFFF".ToColor());
			base.MouseEnter += OnMouseEnter;
		}

		private void OnMouseEnter(object sender, RoutedEventArgs e)
		{
			base.MouseEnter -= OnMouseEnter;

			object payload = Tag;
			if (payload is Package)
			{
				Package package = payload as Package;
				if (!package.HasOption('T'))
				{
					base.Cursor = Cursors.Arrow;
					return;
				}
			}

			App.MainPage.DragDrop.AddSource(this, payload);
		}
	}
}
